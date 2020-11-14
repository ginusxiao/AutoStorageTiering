#include "AST/AST.hpp"
#include "AST/ASTDiskUltimatePerfCache.hpp"
#include "AST/ASTAdvice.hpp"

ywb_status_t TierBasedAdviceSet::AddAdvice(ywb_uint32_t tier, Advice* advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    mAdviseLock.Lock();
    advice->IncRef();
    mAdvise[tier].push_back(advice);
    mAdviseLock.Unlock();

    return ret;
}

ywb_status_t
TierBasedAdviceSet::GetFirstAdvice(ywb_uint32_t tier, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    mAdviseLock.Lock();
    if(mAdvise[tier].empty())
    {
        ret = YFS_ENOENT;
        *advice = NULL;
    }
    else
    {
        mIt[tier] = mAdvise[tier].begin();
        *advice = *mIt[tier];
        (*advice)->IncRef();
        mIt[tier]++;
    }
    mAdviseLock.Unlock();

    return ret;
}

ywb_status_t
TierBasedAdviceSet::GetNextAdvice(ywb_uint32_t tier, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    mAdviseLock.Lock();
    if(mIt[tier] == mAdvise[tier].end())
    {
        ret = YFS_ENOENT;
        *advice = NULL;
    }
    else
    {
        *advice = *mIt[tier];
        (*advice)->IncRef();
        mIt[tier]++;
    }
    mAdviseLock.Unlock();

    return ret;
}

/*the caller must provide lock protection*/
ywb_uint32_t
TierBasedAdviceSet::GetFirstTier()
{
    ywb_uint32_t tier = Tier::TIER_0;

    for(; tier < Constant::TIER_NUM; tier++)
    {
        if(!(mAdvise[tier].empty()))
        {
            break;
        }
    }

    mLastChosenTier = tier;
    return tier;
}

/*the caller must provide lock protection*/
ywb_uint32_t
TierBasedAdviceSet::GetNextTier()
{
    ywb_uint32_t curtier = mLastChosenTier + 1;

    if(Constant::TIER_NUM < curtier)
    {
        return Constant::TIER_NUM;
    }

    while((curtier < Constant::TIER_NUM) &&
            ((mAdvise[curtier].empty()) ||
            (mAdvise[curtier].size() <= mAdviseChosen[curtier])))
    {
        curtier++;
    }

    if(curtier == Constant::TIER_NUM)
    {
        curtier = Tier::TIER_0;
        while((curtier < mLastChosenTier) &&
                ((mAdvise[curtier].empty()) ||
                (mAdvise[curtier].size() <= mAdviseChosen[curtier])))
        {
            curtier++;
        }
    }

    /*check the returned next tier*/
    if((mAdvise[curtier].empty()) ||
            (mAdvise[curtier].size() <= mAdviseChosen[curtier]))
    {
        curtier = Constant::TIER_NUM;
    }

    mLastChosenTier = curtier;
    return curtier;
}

ywb_status_t
TierBasedAdviceSet::GetFirstAdvice(Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t firsttier = 0;
    ywb_uint32_t tier = 0;

    mAdviseLock.Lock();
    /*
     * NOTE: initialization is necessary, for no one can prevent
     * GetFirstAdvice from being called multiple times
     **/
    mLastChosenTier = Constant::TIER_NUM;
    for(; tier < Constant::TIER_NUM; tier++)
    {
        mIt2[tier] = mAdvise[tier].begin();
        mAdviseChosen[tier] = 0;
    }

    firsttier = GetFirstTier();
    if(firsttier != Constant::TIER_NUM)
    {
        /*we are assure at least one advice here*/
        *advice = *mIt2[firsttier];
        (*advice)->IncRef();
        mAdviseChosen[firsttier]++;
        mIt2[firsttier]++;
    }
    else
    {
        *advice = NULL;
        ret = YFS_ENOENT;
    }
    mAdviseLock.Unlock();

    return ret;
}

/*try our best to balance among different tiers*/
ywb_status_t
TierBasedAdviceSet::GetNextAdvice(Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t nexttier = 0;

    mAdviseLock.Lock();
    nexttier = GetNextTier();
    if((0 <= nexttier) && (nexttier < Constant::TIER_NUM))
    {
        *advice = *mIt2[nexttier];
        (*advice)->IncRef();
        mAdviseChosen[nexttier]++;
        mIt2[nexttier]++;
    }
    else
    {
        *advice = NULL;
        ret = YFS_ENOENT;
    }
    mAdviseLock.Unlock();

    return ret;
}

ywb_status_t
TierBasedAdviceSet::GetFirstAdviceSatisfy(ywb_uint32_t flag,
        ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = GetFirstAdvice(advice);
    while((YWB_SUCCESS == ret) && (*advice != NULL))
    {
        if(set == (*advice)->TestFlag(flag))
        {
            return YWB_SUCCESS;
        }
        else
        {
            PutAdvice(*advice);
            ret = GetNextAdvice(advice);
        }
    }

    return ret;
}

ywb_status_t
TierBasedAdviceSet::GetNextAdviceSatisfy(ywb_uint32_t flag,
        ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = GetNextAdvice(advice);
    while((YWB_SUCCESS == ret) && (*advice != NULL))
    {
        if(set == (*advice)->TestFlag(flag))
        {
            return YWB_SUCCESS;
        }
        else
        {
            PutAdvice(*advice);
            ret = GetNextAdvice(advice);
        }
    }

    return ret;
}

ywb_bool_t
TierBasedAdviceSet::IsFirstTimeToGetFrom()
{
    ywb_uint32_t tier = 0;
    ywb_uint32_t chosensum = 0;

    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        chosensum += mAdviseChosen[tier];
    }

    if(0 == chosensum)
    {
        return true;
    }
    else
    {
        return false;
    }
}


ywb_status_t
TierBasedAdviceSet::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = 0;
    Advice* advice = NULL;

    mAdviseLock.Lock();
    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        if(!((mAdvise[tier]).empty()))
        {
            mIt[tier] = mAdvise[tier].begin();
            for(; mIt[tier] != mAdvise[tier].end(); )
            {
                advice = *mIt[tier];
                mIt[tier] = mAdvise[tier].erase(mIt[tier]);
                PutAdvice(advice);
            }
        }
    }
    mAdviseLock.Unlock();

    return ret;
}

ywb_status_t DiskAdvise::AddAdvice(Advice* advice)
{
    YWB_ASSERT(advice != NULL);
    Mutex::Locker locker(mLock);
    mAdvise.push_back(advice);
    return YWB_SUCCESS;
}

ywb_status_t DiskAdvise::GetFirstAdviceLocked(Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;

    *advice = NULL;
    mIt = mAdvise.begin();
    if(mIt != mAdvise.end())
    {
        *advice = *mIt;
        mIt++;
        (*advice)->IncRef();
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t DiskAdvise::GetNextAdviceLocked(Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;

    *advice = NULL;
    if(mAdvise.end() != mIt)
    {
        *advice = *mIt;
        mIt++;
        (*advice)->IncRef();
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t DiskAdvise::GetFirstAdvice(Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;

    Mutex::Locker locker(mLock);
    ret = GetFirstAdviceLocked(advice);

    return ret;
}

ywb_status_t DiskAdvise::GetNextAdvice(Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;

    Mutex::Locker locker(mLock);
    ret = GetNextAdviceLocked(advice);

    return ret;
}

ywb_status_t DiskAdvise::GetFirstAdviceSatisfy(
        ywb_uint32_t flag, ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    Mutex::Locker locker(mLock);
    ret = GetFirstAdviceLocked(advice);
    while((YWB_SUCCESS == ret) && (*advice != NULL))
    {
        if(set == (*advice)->TestFlag(flag))
        {
            return YWB_SUCCESS;
        }
        else
        {
            PutAdvice(*advice);
            ret = GetNextAdviceLocked(advice);
        }
    }

    return ret;
}

ywb_status_t DiskAdvise::GetNextAdviceSatisfy(
        ywb_uint32_t flag, ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    Mutex::Locker locker(mLock);
    ret = GetNextAdviceLocked(advice);
    while((YWB_SUCCESS == ret) && (*advice != NULL))
    {
        if(set == (*advice)->TestFlag(flag))
        {
            return YWB_SUCCESS;
        }
        else
        {
            PutAdvice(*advice);
            ret = GetNextAdviceLocked(advice);
        }
    }

    return ret;
}

void DiskAdvise::Destroy()
{
    list<Advice*>::iterator iter;
    Advice* advice = NULL;

    Mutex::Locker locker(mLock);
    iter = mAdvise.begin();
    while(mAdvise.end() != iter)
    {
        advice = *iter;
        iter++;
        delete advice;
        advice = NULL;
    }

    mAdvise.clear();
}

ywb_status_t DiskBasedAdviceSet::AddAdvice(DiskKey& diskkey, Advice* advice)
{
    std::map<DiskKey, DiskAdvise*, DiskKeyCmp>::iterator iter;
    DiskAdvise* diskadvise = NULL;

    iter = mAdvise.find(diskkey);
    if(mAdvise.end() != iter)
    {
        diskadvise = iter->second;
    }
    else
    {
        diskadvise = new DiskAdvise();
        YWB_ASSERT(diskadvise != NULL);
        mAdvise.insert(std::make_pair(diskkey, diskadvise));
    }

    return diskadvise->AddAdvice(advice);
}

ywb_status_t DiskBasedAdviceSet::GetFirstAdvice(DiskKey& diskkey, Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;
    std::map<DiskKey, DiskAdvise*, DiskKeyCmp>::iterator iter;

    iter = mAdvise.find(diskkey);
    if(mAdvise.end() != iter)
    {
        ret = iter->second->GetFirstAdvice(advice);
    }

    return ret;
}

ywb_status_t DiskBasedAdviceSet::GetNextAdvice(DiskKey& diskkey, Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;
    std::map<DiskKey, DiskAdvise*, DiskKeyCmp>::iterator iter;

    iter = mAdvise.find(diskkey);
    if(mAdvise.end() != iter)
    {
        ret = iter->second->GetNextAdvice(advice);
    }

    return ret;
}

ywb_status_t DiskBasedAdviceSet::GetFirstAdviceSatisfy(DiskKey& diskkey,
        ywb_uint32_t flag, ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;
    std::map<DiskKey, DiskAdvise*, DiskKeyCmp>::iterator iter;

    iter = mAdvise.find(diskkey);
    if(mAdvise.end() != iter)
    {
        ret = iter->second->GetFirstAdviceSatisfy(flag, set, advice);
    }

    return ret;
}

ywb_status_t DiskBasedAdviceSet::GetNextAdviceSatisfy(DiskKey& diskkey,
        ywb_uint32_t flag, ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;
    std::map<DiskKey, DiskAdvise*, DiskKeyCmp>::iterator iter;

    iter = mAdvise.find(diskkey);
    if(mAdvise.end() != iter)
    {
        ret = iter->second->GetNextAdviceSatisfy(flag, set, advice);
    }

    return ret;
}

void DiskBasedAdviceSet::Destroy()
{
    std::map<DiskKey, DiskAdvise*, DiskKeyCmp>::iterator iter;
    DiskAdvise* diskadvise = NULL;

    iter = mAdvise.begin();
    while(mAdvise.end() != iter)
    {
        diskadvise = iter->second;
        iter++;
        delete diskadvise;
        diskadvise = NULL;
    }

    mAdvise.clear();
}

ywb_uint32_t
SubPoolAdvice::GetAdviceTargetTier(ywb_uint32_t srctier, AdviceRule& rule)
{
    ywb_uint32_t tgttier = Tier::TIER_cnt;
    ywb_uint32_t advicetype = rule.GetAdviceType();

    switch(mTierComb)
    {
    case SubPool::TC_ssd:
    case SubPool::TC_ent:
    case SubPool::TC_sata:
        break;
    case SubPool::TC_ssd_ent:
        if((Tier::TIER_1 == srctier) &&
                ((AdviceType::AT_ctr_hot == advicetype) ||
                (AdviceType::AT_swap_up == advicetype)))
        {
            tgttier = Tier::TIER_0;
        }
        else if((Tier::TIER_0 == srctier) &&
                ((AdviceType::AT_ctr_cold == advicetype) ||
                (AdviceType::AT_swap_down == advicetype)))
        {
            tgttier = Tier::TIER_1;
        }
        break;
    case SubPool::TC_ssd_sata:
        if((Tier::TIER_2 == srctier) &&
                ((AdviceType::AT_ctr_hot == advicetype) ||
                (AdviceType::AT_swap_up == advicetype)))
        {
            tgttier = Tier::TIER_0;
        }
        else if((Tier::TIER_0 == srctier) &&
                ((AdviceType::AT_ctr_cold == advicetype) ||
                (AdviceType::AT_swap_down == advicetype)))
        {
            tgttier = Tier::TIER_2;
        }
        break;
    case SubPool::TC_ent_sata:
        if((Tier::TIER_2 == srctier) &&
                ((AdviceType::AT_ctr_hot == advicetype) ||
                (AdviceType::AT_swap_up == advicetype)))
        {
            tgttier = Tier::TIER_1;
        }
        else if((Tier::TIER_1 == srctier) &&
                ((AdviceType::AT_ctr_cold == advicetype) ||
                (AdviceType::AT_swap_down == advicetype)))
        {
            tgttier = Tier::TIER_2;
        }
        break;
    case SubPool::TC_ssd_ent_sata:
        if((Tier::TIER_1 == srctier) &&
                ((AdviceType::AT_ctr_hot == advicetype) ||
                (AdviceType::AT_swap_up == advicetype)))
        {
            tgttier = Tier::TIER_0;
        }
        else if((Tier::TIER_2 == srctier) &&
                ((AdviceType::AT_ctr_hot == advicetype) ||
                (AdviceType::AT_swap_up == advicetype)))
        {
            tgttier = Tier::TIER_1;
        }
        else if((Tier::TIER_0 == srctier) &&
                ((AdviceType::AT_ctr_cold == advicetype) ||
                (AdviceType::AT_swap_down == advicetype)))
        {
            tgttier = Tier::TIER_1;
        }
        else if((Tier::TIER_1 == srctier) &&
                ((AdviceType::AT_ctr_cold == advicetype) ||
                (AdviceType::AT_swap_down == advicetype)))
        {
            tgttier = Tier::TIER_2;
        }
        break;
    default:
        break;
    }

    return tgttier;
}

ywb_status_t
SubPoolAdvice::AddAdvice(ywb_uint32_t tier, ywb_uint32_t type, Advice* advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(NULL == mAdviceSet1[type])
    {
        mAdviceSet1[type] = new TierBasedAdviceSet();
        if(NULL == mAdviceSet1[type])
        {
            return YFS_EOUTOFMEMORY;
        }
    }

    ret = mAdviceSet1[type]->AddAdvice(tier, advice);

    return ret;
}

ywb_status_t
SubPoolAdvice::AddAdvice(DiskKey& diskkey, ywb_uint32_t type, Advice* advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(NULL == mAdviceSet2[type])
    {
        mAdviceSet2[type] = new DiskBasedAdviceSet();
        if(NULL == mAdviceSet2[type])
        {
            return YFS_EOUTOFMEMORY;
        }
    }

    ret = mAdviceSet2[type]->AddAdvice(diskkey, advice);

    return ret;
}

ywb_status_t
SubPoolAdvice::GetFirstAdvice(ywb_uint32_t tier,
            ywb_uint32_t type, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(NULL == mAdviceSet1[type])
    {
        *advice = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = mAdviceSet1[type]->GetFirstAdvice(tier, advice);
        if(!((YWB_SUCCESS == ret) && (advice != NULL)))
        {
            *advice = NULL;
            ret = YFS_ENOENT;
        }
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::GetNextAdvice(ywb_uint32_t tier,
        ywb_uint32_t type, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(NULL == mAdviceSet1[type])
    {
        *advice = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = mAdviceSet1[type]->GetNextAdvice(tier, advice);
        if(!((YWB_SUCCESS == ret) && (advice != NULL)))
        {
            *advice = NULL;
            ret = YFS_ENOENT;
        }
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::GetFirstAdvice(DiskKey& diskkey,
        ywb_uint32_t type, Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    *advice = NULL;
    if(NULL != mAdviceSet2[type])
    {
        ret = mAdviceSet2[type]->GetFirstAdvice(diskkey, advice);
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::GetNextAdvice(DiskKey& diskkey,
        ywb_uint32_t type, Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    *advice = NULL;
    if(NULL != mAdviceSet2[type])
    {
        ret = mAdviceSet2[type]->GetNextAdvice(diskkey, advice);
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::GetFirstAdvice(ywb_uint32_t type, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(NULL == mAdviceSet1[type])
    {
        *advice = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = mAdviceSet1[type]->GetFirstAdvice(advice);
        if(!((YWB_SUCCESS == ret) && (advice != NULL)))
        {
            *advice = NULL;
            ret = YFS_ENOENT;
        }
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::GetNextAdvice(ywb_uint32_t type, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(NULL == mAdviceSet1[type])
    {
        *advice = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = mAdviceSet1[type]->GetNextAdvice(advice);
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::GetFirstAdviceSatisfy(ywb_uint32_t type,
        ywb_uint32_t flag, ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(NULL == mAdviceSet1[type])
    {
        *advice = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = mAdviceSet1[type]->GetFirstAdviceSatisfy(flag, set, advice);
        if(!((YWB_SUCCESS == ret) && (advice != NULL)))
        {
            *advice = NULL;
            ret = YFS_ENOENT;
        }
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::GetNextAdviceSatisfy(ywb_uint32_t type,
        ywb_uint32_t flag, ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(NULL == mAdviceSet1[type])
    {
        *advice = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = mAdviceSet1[type]->GetNextAdviceSatisfy(flag, set, advice);
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::GetFirstAdviceSatisfy(DiskKey& diskkey, ywb_uint32_t type,
        ywb_uint32_t flag, ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    *advice = NULL;
    if(NULL != mAdviceSet2[type])
    {
        ret = mAdviceSet2[type]->GetFirstAdviceSatisfy(diskkey, flag, set, advice);
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::GetNextAdviceSatisfy(DiskKey& diskkey, ywb_uint32_t type,
        ywb_uint32_t flag, ywb_bool_t set, Advice** advice)
{
    ywb_status_t ret = YFS_ENOENT;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    *advice = NULL;
    if(NULL != mAdviceSet2[type])
    {
        ret = mAdviceSet2[type]->GetNextAdviceSatisfy(diskkey, flag, set, advice);
    }

    return ret;
}

ywb_bool_t
SubPoolAdvice::IsFirstTimeToGetFromTierBasedAdvice(ywb_uint32_t type)
{
    YWB_ASSERT((0 <= type) && (type < AdviceType::AT_cnt));
    if(NULL == mAdviceSet1[type])
    {
        return true;
    }
    else
    {
        return mAdviceSet1[type]->IsFirstTimeToGetFrom();
    }
}

ywb_status_t
SubPoolAdvice::DestroyTierBasedAdviceSet(ywb_uint32_t type)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(mAdviceSet1[type] != NULL)
    {
        delete mAdviceSet1[type];
        mAdviceSet1[type] = NULL;
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::DestroyDiskBasedAdviceSet(ywb_uint32_t type)
{
    ywb_status_t ret = YWB_SUCCESS;

    if((type < 0) || (type >= AdviceType::AT_cnt))
    {
        return YFS_EINVAL;
    }

    if(mAdviceSet2[type] != NULL)
    {
        delete mAdviceSet2[type];
        mAdviceSet2[type] = NULL;
    }

    return ret;
}

ywb_status_t
SubPoolAdvice::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t type = 0;

    for(; type < Constant::ADVICE_TYPE_CNT; type++)
    {
        if(mAdviceSet1[type] != NULL)
        {
            delete mAdviceSet1[type];
            mAdviceSet1[type] = NULL;
        }

        if(mAdviceSet2[type] != NULL)
        {
            delete mAdviceSet2[type];
            mAdviceSet2[type] = NULL;
        }
    }

    mTierComb = SubPool::TC_cnt;
    return ret;
}

ywb_status_t
SubPoolAdvice::Destroy(ywb_int32_t advicetype)
{
    ywb_status_t ret = YWB_SUCCESS;

    YWB_ASSERT((0 <= advicetype) &&
            (advicetype < (ywb_int32_t)Constant::ADVICE_TYPE_CNT));
    if(mAdviceSet1[advicetype] != NULL)
    {
        delete mAdviceSet1[advicetype];
        mAdviceSet1[advicetype] = NULL;
    }

    if(mAdviceSet2[advicetype] != NULL)
    {
        delete mAdviceSet2[advicetype];
        mAdviceSet2[advicetype] = NULL;
    }

    return ret;
}

ywb_status_t
AdviceManager::GetSubPoolAdvice(SubPoolKey& key, SubPoolAdvice** subpooladvise)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolAdvice*, SubPoolKeyCmp>::iterator iter;

    Lock();
    iter = mAdvise.find(key);
    if(iter != mAdvise.end())
    {
        *subpooladvise = iter->second;
        (*subpooladvise)->IncRef();
    }
    else
    {
        ret = YFS_ENOENT;
        *subpooladvise = NULL;
    }
    Unlock();

    return ret;
}

ywb_status_t
AdviceManager::AddSubPoolAdvice(SubPoolKey& key, SubPoolAdvice* subpooladvice)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<SubPoolKey, SubPoolAdvice*, SubPoolKeyCmp>::iterator, bool> pairret;

    ast_log_debug("Add subpool advice into advice manager, "
            "subpool: ["<< key.GetOss()
            << ", "  << key.GetPoolId()
            << ", " << key.GetSubPoolId()
            << "]");
    Lock();
    pairret = mAdvise.insert(std::make_pair(key, subpooladvice));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }
    else
    {
        subpooladvice->IncRef();
    }
    Unlock();

    return ret;
}

ywb_status_t
AdviceManager::RemoveSubPoolAdvice(SubPoolKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolAdvice*, SubPoolKeyCmp>::iterator iter;
    SubPoolAdvice* subpooladvice = NULL;

    ast_log_debug("Remove subpool advice from advice manager, subpool: ["
            << ", " << key.GetOss()
            << ", "  << key.GetPoolId()
            << ", " << key.GetSubPoolId()
            << "]");

    Lock();
    iter = mAdvise.find(key);
    if(iter != mAdvise.end())
    {
        subpooladvice = iter->second;
        mAdvise.erase(iter);
        subpooladvice->DecRef();
    }
    else
    {
        ret = YFS_ENOENT;
    }
    Unlock();

    return ret;
}

/*
 * FIXME: there should be a more sophisticated algorithm
 * for calculating theoretical advice number in one tier
 **/
ywb_status_t
AdviceManager::CalculateTheoreticalAdviceNum(SubPoolKey& key,
        ywb_uint32_t tiertype, ywb_uint32_t advicetype,
        ywb_uint32_t* adviceup, ywb_uint32_t* advicedown)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tiercomb = SubPool::TC_cnt;
    ywb_uint64_t maxreadperf = 0;
    ywb_uint32_t maxmigratableobjs = 0;
    AST* ast = NULL;
    DiskUltimatePerfCache* ultimateperfmgr = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    list<DiskKey> diskkeys;
    list<Disk*> disks;
    list<Disk*>::iterator iter;
    Disk* disk = NULL;
    DiskBaseProp* diskprop = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ast->GetDiskUltimatePerfCache(&ultimateperfmgr);
    *adviceup = 0;
    *advicedown = 0;
    ret = config->GetSubPool(key, &subpool);
    if(YWB_SUCCESS == ret && NULL != subpool)
    {
        tiercomb = subpool->GetCurTierComb();
        if(YWB_SUCCESS == subpool->GetTier(tiertype, &tier))
        {
            tier->GetKeysAndDisks(diskkeys, disks);
            iter = disks.begin();
            while(disks.end() != iter)
            {
                disk = *iter;
                disk->GetBaseProp(&diskprop);
                tier->PutDisk(disk);
                /*
                 * currently will only take source tier's read bandwidth
                 * into consideration
                 * */
                maxreadperf += ultimateperfmgr->GetUltimatePerf(*diskprop,
                        DiskUltimatePerfCache::F_bandwidth |
                        DiskUltimatePerfCache::F_read);
                iter++;
            }

            maxmigratableobjs = maxreadperf / ((Constant::DEFAULT_OBJ_SIZE));
            subpool->PutTier(tier);
        }

        config->PutSubPool(subpool);
        switch(tiercomb)
        {
        /*currently only support cross-tier relocation*/
        case SubPool::TC_ssd:
        case SubPool::TC_ent:
        case SubPool::TC_sata:
            *adviceup = 0;
            *advicedown = 0;
            break;
        case SubPool::TC_ssd_ent:
            if(Tier::TIER_2 == tiertype)
            {
                *adviceup = 0;
                *advicedown = 0;
            }
            else if((Tier::TIER_1 == tiertype) &&
                    (AdviceType::AT_ctr_hot == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else if((Tier::TIER_0 == tiertype) &&
                    (AdviceType::AT_ctr_cold == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else
            {
                *adviceup = 0;
                *advicedown = 0;
            }

            break;
        case SubPool::TC_ssd_sata:
            if(Tier::TIER_1 == tiertype)
            {
                *adviceup = 0;
                *advicedown = 0;
            }
            else if((Tier::TIER_2 == tiertype) &&
                    (AdviceType::AT_ctr_hot == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else if((Tier::TIER_0 == tiertype) &&
                    (AdviceType::AT_ctr_cold == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else
            {
                *adviceup = 0;
                *advicedown = 0;
            }

            break;
        case SubPool::TC_ent_sata:
            if(Tier::TIER_0 == tiertype)
            {
                *adviceup = 0;
                *advicedown = 0;
            }
            else if((Tier::TIER_2 == tiertype) &&
                    (AdviceType::AT_ctr_hot == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else if((Tier::TIER_1 == tiertype) &&
                    (AdviceType::AT_ctr_cold == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else
            {
                *adviceup = 0;
                *advicedown = 0;
            }

            break;
        case SubPool::TC_ssd_ent_sata:
            if((AdviceType::AT_ctr_hot == advicetype) &&
                    ((Tier::TIER_2 == tiertype) || (Tier::TIER_1 == tiertype)))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else if((AdviceType::AT_ctr_cold == advicetype) &&
                    ((Tier::TIER_0 == tiertype) || (Tier::TIER_1 == tiertype)))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else
            {
                *adviceup = 0;
                *advicedown = 0;
            }

            break;
        default:
            YWB_ASSERT(0);
            break;
        }
    }

    if(*adviceup != 0)
    {
        *adviceup = maxmigratableobjs * (Constant::DEFAULT_ADVICE_NUM_GAIN_FACTOR);
    }

    if(*advicedown != 0)
    {
        *advicedown = maxmigratableobjs * (Constant::DEFAULT_ADVICE_NUM_GAIN_FACTOR);
    }

    ast_log_debug("Calculated theoretical advice num for"
            << " subpool: [" << key.GetOss()
            << ", " << key.GetPoolId()
            << ", " << key.GetSubPoolId()
            << "] and tier: " << tiertype
            << ", above boundary: "<< *adviceup
            << ", below boundary: " << *advicedown);
    return ret;
}

ywb_status_t
AdviceManager::CalculateTheoreticalAdviceNum(SubPoolKey& subpoolkey,
            ywb_uint32_t tiertype, DiskKey& diskkey, ywb_uint32_t advicetype,
            ywb_uint32_t* adviceup, ywb_uint32_t* advicedown)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tiercomb = SubPool::TC_cnt;
    ywb_uint64_t maxreadperf = 0;
    ywb_uint32_t maxmigratableobjs = 0;
    AST* ast = NULL;
    DiskUltimatePerfCache* ultimateperfmgr = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;
    DiskBaseProp* diskprop = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ast->GetDiskUltimatePerfCache(&ultimateperfmgr);
    *adviceup = 0;
    *advicedown = 0;
    ret = config->GetSubPool(subpoolkey, &subpool);
    if(YWB_SUCCESS == ret && NULL != subpool)
    {
        tiercomb = subpool->GetCurTierComb();
        if(YWB_SUCCESS == subpool->GetTier(tiertype, &tier))
        {
            ret = tier->GetDisk(diskkey, &disk);
            if(YWB_SUCCESS == ret)
            {
                disk->GetBaseProp(&diskprop);
                tier->PutDisk(disk);
                /*
                 * currently will only take source tier's read bandwidth
                 * into consideration
                 * */
                maxreadperf += ultimateperfmgr->GetUltimatePerf(*diskprop,
                        DiskUltimatePerfCache::F_bandwidth |
                        DiskUltimatePerfCache::F_read);
            }

            maxmigratableobjs = maxreadperf / ((Constant::DEFAULT_OBJ_SIZE));
            subpool->PutTier(tier);
        }

        config->PutSubPool(subpool);
        switch(tiercomb)
        {
        /*currently only support cross-tier relocation*/
        case SubPool::TC_ssd:
        case SubPool::TC_ent:
        case SubPool::TC_sata:
            *adviceup = 0;
            *advicedown = 0;
            break;
        case SubPool::TC_ssd_ent:
            if(Tier::TIER_2 == tiertype)
            {
                *adviceup = 0;
                *advicedown = 0;
            }
            else if((Tier::TIER_1 == tiertype) &&
                    (AdviceType::AT_swap_up == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else if((Tier::TIER_0 == tiertype) &&
                    (AdviceType::AT_swap_down == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else
            {
                *adviceup = 0;
                *advicedown = 0;
            }

            break;
        case SubPool::TC_ssd_sata:
            if(Tier::TIER_1 == tiertype)
            {
                *adviceup = 0;
                *advicedown = 0;
            }
            else if((Tier::TIER_2 == tiertype) &&
                    (AdviceType::AT_swap_up == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else if((Tier::TIER_0 == tiertype) &&
                    (AdviceType::AT_swap_down == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else
            {
                *adviceup = 0;
                *advicedown = 0;
            }

            break;
        case SubPool::TC_ent_sata:
            if(Tier::TIER_0 == tiertype)
            {
                *adviceup = 0;
                *advicedown = 0;
            }
            else if((Tier::TIER_2 == tiertype) &&
                    (AdviceType::AT_swap_up == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else if((Tier::TIER_1 == tiertype) &&
                    (AdviceType::AT_swap_down == advicetype))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else
            {
                *adviceup = 0;
                *advicedown = 0;
            }

            break;
        case SubPool::TC_ssd_ent_sata:
            if((AdviceType::AT_swap_up == advicetype) &&
                    ((Tier::TIER_2 == tiertype) || (Tier::TIER_1 == tiertype)))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else if((AdviceType::AT_swap_down == advicetype) &&
                    ((Tier::TIER_0 == tiertype) || (Tier::TIER_1 == tiertype)))
            {
                *adviceup = YWB_UINT32_MAX;
                *advicedown = YWB_UINT32_MAX;
            }
            else
            {
                *adviceup = 0;
                *advicedown = 0;
            }

            break;
        default:
            YWB_ASSERT(0);
            break;
        }
    }

    if(*adviceup != 0)
    {
        *adviceup = maxmigratableobjs * (Constant::DEFAULT_ADVICE_NUM_GAIN_FACTOR);
    }

    if(*advicedown != 0)
    {
        *advicedown = maxmigratableobjs * (Constant::DEFAULT_ADVICE_NUM_GAIN_FACTOR);
    }

    ast_log_debug("Calculated theoretical advice num for"
            << " disk: [" << diskkey.GetSubPoolId()
            << ", " << diskkey.GetDiskId()
            << "] and tier: " << tiertype
            << ", above boundary: "<< *adviceup
            << ", below boundary: " << *advicedown);
    return ret;
}

/*TODO:*/
ywb_status_t
AdviceManager::CalculateTheoreticalAdviceNum(SubPoolKey& key,
        DiskKey& disk, ywb_uint32_t advicetype,
        ywb_uint32_t* adviceabove, ywb_uint32_t* advicebelow)
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_status_t
AdviceManager::AdoptAdviceRule(SubPoolKey& key,
        ywb_uint32_t tier, AdviceRule& rule)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t estimatenumabove = 0;
    ywb_uint32_t estimatenumbelow = 0;
    AST* ast = NULL;
    HeatDistributionManager* heatdistribution = NULL;

    GetAST(&ast);
    ast->GetHeatDistributionManager(&heatdistribution);
    ast_log_debug("Adopt advice rule on subpool: ["
            << key.GetOss()
            << ", " << key.GetPoolId()
            << ", " << key.GetSubPoolId()
            << "] and tier: " << tier);

    ret = CalculateTheoreticalAdviceNum(key, tier,
            rule.GetAdviceType(), &estimatenumabove, &estimatenumbelow);
    if((YWB_SUCCESS == ret) &&
            ((estimatenumabove > 0) || (estimatenumbelow > 0)))
    {
        ret = heatdistribution->SetupSubPoolHeatDistribution(key, tier,
                rule, estimatenumabove, estimatenumbelow);
    }
    else if(YWB_SUCCESS == ret)
    {
        ret = YFS_ENODATA;
    }

    return ret;
}

ywb_status_t
AdviceManager::AdoptAdviceRule(SubPoolKey& subpool, ywb_uint32_t tier,
        DiskKey& disk, AdviceRule& rule)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t estimatenumabove = 0;
    ywb_uint32_t estimatenumbelow = 0;
    AST* ast = NULL;
    HeatDistributionManager* heatdistribution = NULL;

    GetAST(&ast);
    ast->GetHeatDistributionManager(&heatdistribution);
    ast_log_debug("Adopt advice rule on disk: ["
            << disk.GetSubPoolId()
            << ", " << disk.GetDiskId()
            << "] and tier: " << tier);

    ret = CalculateTheoreticalAdviceNum(subpool, tier, disk,
            rule.GetAdviceType(), &estimatenumabove, &estimatenumbelow);
    if((YWB_SUCCESS == ret) &&
            ((estimatenumabove > 0) || (estimatenumbelow > 0)))
    {
        ret = heatdistribution->SetupSubPoolHeatDistribution(subpool, tier,
                disk, rule, estimatenumabove, estimatenumbelow);
    }
    else if(YWB_SUCCESS == ret)
    {
        ret = YFS_ENODATA;
    }

    return ret;
}

/*TODO:*/
ywb_status_t
AdviceManager::AdoptAdviceRule(SubPoolKey& subpool,
        DiskKey& disk, AdviceRule& rule)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
AdviceManager::AdoptFilterRule(SubPoolKey& subpool, ywb_uint32_t tier,
        AdviceRule* filterrule, ywb_uint32_t filternum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t filterindex = 0;

    ast_log_debug("Adopt filter rule on subpool-tier---->>>>");
    if((filterrule != NULL) && (filternum > 0))
    {
        for(; filterindex < filternum; filterindex++)
        {
            ret = AdoptAdviceRule(subpool, tier, filterrule[filterindex]);
            if((ret != YWB_SUCCESS))
            {
                break;
            }
        }
    }

    return ret;
}

ywb_status_t
AdviceManager::AdoptFilterRule(SubPoolKey& subpool, ywb_uint32_t tier,
        DiskKey& disk, AdviceRule* filterrule, ywb_uint32_t filternum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t filterindex = 0;

    ast_log_debug("Adopt filter rule on subpool-tier-disk ---->>>>");
    if((filterrule != NULL) && (filternum > 0))
    {
        for(; filterindex < filternum; filterindex++)
        {
            ret = AdoptAdviceRule(subpool, tier, disk, filterrule[filterindex]);
            if((ret != YWB_SUCCESS))
            {
                break;
            }
        }
    }

    return ret;
}

ywb_status_t
AdviceManager::AdoptFilterRule(SubPoolKey& subpool, DiskKey& disk,
        AdviceRule* filterrule, ywb_uint32_t filternum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t filterindex = 0;

    ast_log_debug("Adopt filter rule on subpool-disk---->>>>");
    if((filterrule != NULL) && (filternum > 0))
    {
        for(; filterindex < filternum; filterindex++)
        {
            ret = AdoptAdviceRule(subpool, disk, filterrule[filterindex]);
            if((ret != YWB_SUCCESS))
            {
                break;
            }
        }
    }

    return ret;
}

ywb_status_t
AdviceManager::IntersectAdviceAndFilterRule(SubPoolKey& key,
        ywb_uint32_t tiertype, AdviceRule& rule,
        AdviceRule* filterrule, ywb_uint32_t filternum)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    HeatDistributionManager* heatdistribution = NULL;
    SubPoolHeatDistribution* subpoolheat = NULL;
    TierWiseHeatDistribution* tierheat = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    OBJKey* objkey = NULL;
    ChunkIOStat* stat;
    Advice* advice = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    IOStat::Type heattype = IOStat::IOST_cnt;
    ywb_uint32_t srctier = 0;
    ywb_uint32_t tgttier = 0;
    ywb_uint32_t loop = 0;
    ywb_uint32_t advicenum = 0;
    set<OBJVal*> OBJSets[Constant::MAX_FILTER_NUM];
    set<OBJVal*>::iterator iter;
    ywb_bool_t satisfy = false;

    ast_log_debug("Intersection subpool-tier wise advice rule and filter rule");
    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ast->GetHeatDistributionManager(&heatdistribution);
    ret = config->GetSubPool(key, &subpool);
    YWB_ASSERT(subpool != NULL);
    ret = heatdistribution->GetSubPoolHeatDistribution(key, &subpoolheat);
    YWB_ASSERT((YWB_SUCCESS == ret) && (subpoolheat != NULL));
    subpoolheat->GetTierWiseHeatDistribution(tiertype, &tierheat);
    YWB_ASSERT(tierheat);
    tierheat->GetHeatRangeTypeFromAdviceRule(rule, &heattype);
    YWB_ASSERT(heattype != IOStat::IOST_cnt);
    ret = GetSubPoolAdvice(key, &subpooladvice);
    YWB_ASSERT((YWB_SUCCESS == ret) && (subpooladvice != NULL));

    /*Get all OBJs satisfying filter rule*/
    if(filternum != 0)
    {
        for(loop = 0; loop < filternum; loop++)
        {
            tierheat->GetAllSatisfyingOBJs(filterrule[loop], &OBJSets[loop]);
        }

        for(loop = 0; loop < filternum; loop++)
        {
            if(!OBJSets[loop].empty())
            {
                ast_log_debug("Totally " << OBJSets[loop].size()
                        << " satisfying OBJS for filter rule["
                        << loop << "]");

//                iter = OBJSets[loop].begin();
//                while(iter != OBJSets[loop].end())
//                {
//                    obj = *iter;
//                    obj->Dump(&ostr);
//                    iter++;
//                }
            }
            else
            {
                ast_log_debug("NO satisfying OBJS for filter rule["
                        << loop << "]");

                PutSubPoolAdvice(subpooladvice);
                config->PutSubPool(subpool);
                return YFS_ENODATA;
            }
        }
    }

    srctier = tiertype;
    tgttier = subpooladvice->GetAdviceTargetTier(srctier, rule);
    if(Tier::TIER_cnt == tgttier)
    {
        PutSubPoolAdvice(subpooladvice);
        config->PutSubPool(subpool);
        return YFS_ENODATA;
    }

    if(AdviceType::AT_ctr_hot == rule.GetAdviceType())
    {
        ret = subpoolheat->GetTierWiseFirstHotOBJ(tiertype, rule, &obj);
        while((YWB_SUCCESS == ret) && (obj != NULL))
        {
            obj->GetOBJKey(&objkey);
            obj->GetOBJIO(&objio);
            objio->GetIOStat(heattype, &stat);

            satisfy = true;
            /*check whether chosen OBJs satisfying all filter rules*/
            for(loop = 0; loop < filternum; loop++)
            {
                iter = OBJSets[loop].find(obj);
                /*OBJ is not satisfying given filter rule*/
                if(iter == OBJSets[loop].end())
                {
                    satisfy = false;
                    break;
                }
            }

            if(satisfy)
            {
                /*
                 * FIXME: here OBJ size should be queried from LogicalConfig
                 * or IOManager, but currently use default value directly
                 **/
                advice = new Advice(*objkey, srctier, tgttier, *stat, heattype,
                        /*Constant::DEFAULT_OBJ_SIZE*/1);
                if(NULL == advice)
                {
                    ast_log_debug("Out of memory!");
                    PutSubPoolAdvice(subpooladvice);
                    config->PutSubPool(subpool);
                    return YFS_EOUTOFMEMORY;
                }

                ast_log_debug("Advice: [objkey: [" << objkey->GetStorageId()
                        << ", " << objkey->GetInodeId()
                        << ", " << objkey->GetChunkId()
                        << ", " << objkey->GetChunkIndex()
                        << ", " << objkey->GetChunkVersion()
                        << "], srctier: " << srctier
                        << ", tgttier: " << tgttier << "]");
                subpooladvice->AddAdvice(tiertype, rule.GetAdviceType(), advice);
                advicenum++;
            }

            ret = subpoolheat->GetTierWiseNextHotOBJ(tiertype, rule, &obj);
        }

        if((advicenum > 0) && (YFS_ENOENT == ret))
        {
            subpool->GetTier(tiertype, &tier);
            YWB_ASSERT(tier != NULL);
            tier->ClearFlag(Tier::TF_no_advice);
            subpool->PutTier(tier);
            ret = YWB_SUCCESS;
            ast_log_debug("Totally " << advicenum
                    << " advise for tier " << tiertype);
        }

        PutSubPoolAdvice(subpooladvice);
        config->PutSubPool(subpool);
    }
    else if(AdviceType::AT_ctr_cold == rule.GetAdviceType())
    {
        ret = subpoolheat->GetTierWiseFirstColdOBJ(tiertype, rule, &obj);
        while((YWB_SUCCESS == ret) && (obj != NULL))
        {
            obj->GetOBJKey(&objkey);
            obj->GetOBJIO(&objio);
            objio->GetIOStat(heattype, &stat);

            satisfy = true;
            /*check whether chosen OBJs satisfying filter rule*/
            for(loop = 0; loop < filternum; loop++)
            {
                iter = OBJSets[loop].find(obj);
                /*OBJ is not satisfying given filter rule*/
                if(iter == OBJSets[loop].end())
                {
                    satisfy = false;
                    break;
                }
            }

            if(satisfy)
            {
                /*
                 * FIXME: here OBJ size should be queried from LogicalConfig
                 * or IOManager, but currently use default value directly
                 **/
                advice = new Advice(*objkey, srctier, tgttier, *stat, heattype,
                        /*Constant::DEFAULT_OBJ_SIZE*/1);
                if(NULL == advice)
                {
                    ast_log_debug("Out of memory!");
                    PutSubPoolAdvice(subpooladvice);
                    config->PutSubPool(subpool);
                    ret = YFS_EOUTOFMEMORY;
                    return ret;
                }

                ast_log_debug("Advice: [objkey: [" << objkey->GetStorageId()
                        << ", " << objkey->GetInodeId()
                        << ", " << objkey->GetChunkId()
                        << ", " << objkey->GetChunkIndex()
                        << ", " << objkey->GetChunkVersion()
                        << "], srctier: " << srctier
                        << ", tgttier: " << tgttier << "]");
                subpooladvice->AddAdvice(tiertype, rule.GetAdviceType(), advice);
                advicenum++;
            }

            ret = subpoolheat->GetTierWiseNextColdOBJ(tiertype, rule, &obj);
        }

        if((advicenum > 0) && (YFS_ENOENT == ret))
        {
            subpool->GetTier(tiertype, &tier);
            YWB_ASSERT(tier != NULL);
            tier->ClearFlag(Tier::TF_no_advice);
            subpool->PutTier(tier);
            ret = YWB_SUCCESS;
            ast_log_debug("Totally " << advicenum
                    << " advise for tier " << tiertype);
        }

        PutSubPoolAdvice(subpooladvice);
        config->PutSubPool(subpool);
    }

    return ret;
}

ywb_status_t
AdviceManager::IntersectAdviceAndFilterRule(SubPoolKey& subpoolkey,
        ywb_uint32_t tiertype, DiskKey& diskkey, AdviceRule& rule,
        AdviceRule* filterrule, ywb_uint32_t filternum)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    HeatDistributionManager* heatdistribution = NULL;
    SubPoolHeatDistribution* subpoolheat = NULL;
    TierInternalDiskWiseHeatDistribution* tierinternalheat = NULL;
    DiskHeatDistribution* diskheat = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    OBJKey* objkey = NULL;
    ChunkIOStat* stat;
    Advice* advice = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;
    IOStat::Type heattype = IOStat::IOST_cnt;
    ywb_uint32_t srctier = 0;
    ywb_uint32_t tgttier = 0;
    ywb_uint32_t loop = 0;
    ywb_uint32_t advicenum = 0;
    set<OBJVal*> OBJSets[Constant::MAX_FILTER_NUM];
    set<OBJVal*>::iterator iter;
    ywb_bool_t satisfy = false;

    ast_log_debug("Intersection disk wise advice rule and filter rule");
    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ast->GetHeatDistributionManager(&heatdistribution);
    ret = config->GetSubPool(subpoolkey, &subpool);
    YWB_ASSERT(subpool != NULL);
    ret = heatdistribution->GetSubPoolHeatDistribution(subpoolkey, &subpoolheat);
    YWB_ASSERT((YWB_SUCCESS == ret) && (subpoolheat != NULL));
    subpoolheat->GetTierInternalDiskWiseHeatDistribution(tiertype, &tierinternalheat);
    YWB_ASSERT(tierinternalheat);
    tierinternalheat->GetDiskHeatDistribution(diskkey, &diskheat);
    YWB_ASSERT(diskheat);
    diskheat->GetHeatRangeTypeFromAdviceRule(rule, &heattype);
    YWB_ASSERT(heattype != IOStat::IOST_cnt);
    ret = GetSubPoolAdvice(subpoolkey, &subpooladvice);
    YWB_ASSERT((YWB_SUCCESS == ret) && (subpooladvice != NULL));

    /*Get all OBJs satisfying filter rule*/
    if(filternum != 0)
    {
        for(loop = 0; loop < filternum; loop++)
        {
            diskheat->GetAllSatisfyingOBJs(filterrule[loop], &OBJSets[loop]);
        }

        for(loop = 0; loop < filternum; loop++)
        {
            if(!OBJSets[loop].empty())
            {
                ast_log_debug("Totally " << OBJSets[loop].size()
                        << " satisfying OBJS for filter rule["
                        << loop << "]");
            }
            else
            {
                ast_log_debug("NO satisfying OBJS for filter rule["
                        << loop << "]");

                PutSubPoolAdvice(subpooladvice);
                config->PutSubPool(subpool);
                return YFS_ENODATA;
            }
        }
    }

    srctier = tiertype;
    tgttier = subpooladvice->GetAdviceTargetTier(srctier, rule);
    if(Tier::TIER_cnt == tgttier)
    {
        PutSubPoolAdvice(subpooladvice);
        config->PutSubPool(subpool);
        return YFS_ENODATA;
    }

    if(AdviceType::AT_swap_up == rule.GetAdviceType())
    {
#if 0
        ret = subpoolheat->GetTierInternalDiskWiseFirstHotOBJ(tiertype, rule, &obj);
#endif
        ret = diskheat->GetFirstHotOBJ(rule, &obj);
        while((YWB_SUCCESS == ret) && (obj != NULL))
        {
            obj->GetOBJKey(&objkey);
            obj->GetOBJIO(&objio);
            objio->GetIOStat(heattype, &stat);

            satisfy = true;
            /*check whether chosen OBJs satisfying all filter rules*/
            for(loop = 0; loop < filternum; loop++)
            {
                iter = OBJSets[loop].find(obj);
                /*OBJ is not satisfying given filter rule*/
                if(iter == OBJSets[loop].end())
                {
                    satisfy = false;
                    break;
                }
            }

            if(satisfy)
            {
                /*
                 * FIXME: here OBJ size should be queried from LogicalConfig
                 * or IOManager, but currently use default value directly
                 **/
                advice = new Advice(*objkey, srctier, tgttier, *stat, heattype,
                        /*Constant::DEFAULT_OBJ_SIZE*/1);
                if(NULL == advice)
                {
                    ast_log_debug("Out of memory!");
                    PutSubPoolAdvice(subpooladvice);
                    config->PutSubPool(subpool);
                    return YFS_EOUTOFMEMORY;
                }

                ast_log_debug("Advice: [objkey: [" << objkey->GetStorageId()
                        << ", " << objkey->GetInodeId()
                        << ", " << objkey->GetChunkId()
                        << ", " << objkey->GetChunkIndex()
                        << ", " << objkey->GetChunkVersion()
                        << "], srctier: " << srctier
                        << ", tgttier: " << tgttier << "]");
                /*subpooladvice->AddAdvice(tiertype, rule.GetAdviceType(), advice);*/
                subpooladvice->AddAdvice(diskkey, rule.GetAdviceType(), advice);
                advicenum++;
            }

#if 0
            ret = subpoolheat->GetTierInternalDiskWiseNextHotOBJ(tiertype, rule, &obj);
#endif
            ret = diskheat->GetNextHotOBJ(rule, &obj);
        }

        if((advicenum > 0) && (YFS_ENOENT == ret))
        {
            tier = NULL;
            disk = NULL;
            subpool->GetTier(tiertype, &tier);
            YWB_ASSERT(tier != NULL);
            tier->GetDisk(diskkey, &disk);
            YWB_ASSERT(disk != NULL);
            disk->ClearFlag(Disk::DF_no_advice);
            tier->PutDisk(disk);
            tier->ClearFlag(Tier::TF_no_advice);
            subpool->PutTier(tier);
            ret = YWB_SUCCESS;
            ast_log_debug("Totally " << advicenum
                    << " advise for disk: ["<< diskkey.GetSubPoolId()
                    << ", "<< diskkey.GetDiskId() << "]");
        }

        PutSubPoolAdvice(subpooladvice);
        config->PutSubPool(subpool);
    }
    else if(AdviceType::AT_swap_down == rule.GetAdviceType())
    {
#if 0
        ret = subpoolheat->GetTierInternalDiskWiseFirstColdOBJ(tiertype, rule, &obj);
#endif
        ret = diskheat->GetFirstColdOBJ(rule, &obj);
        while((YWB_SUCCESS == ret) && (obj != NULL))
        {
            obj->GetOBJKey(&objkey);
            obj->GetOBJIO(&objio);
            objio->GetIOStat(heattype, &stat);

            satisfy = true;
            /*check whether chosen OBJs satisfying filter rule*/
            for(loop = 0; loop < filternum; loop++)
            {
                iter = OBJSets[loop].find(obj);
                /*OBJ is not satisfying given filter rule*/
                if(iter == OBJSets[loop].end())
                {
                    satisfy = false;
                    break;
                }
            }

            if(satisfy)
            {
                /*
                 * FIXME: here OBJ size should be queried from LogicalConfig
                 * or IOManager, but currently use default value directly
                 **/
                advice = new Advice(*objkey, srctier, tgttier, *stat, heattype,
                        /*Constant::DEFAULT_OBJ_SIZE*/1);
                if(NULL == advice)
                {
                    ast_log_debug("Out of memory!");
                    PutSubPoolAdvice(subpooladvice);
                    config->PutSubPool(subpool);
                    ret = YFS_EOUTOFMEMORY;
                    return ret;
                }

                ast_log_debug("Advice: [objkey: [" << objkey->GetStorageId()
                        << ", " << objkey->GetInodeId()
                        << ", " << objkey->GetChunkId()
                        << ", " << objkey->GetChunkIndex()
                        << ", " << objkey->GetChunkVersion()
                        << "], srctier: " << srctier
                        << ", tgttier: " << tgttier << "]");
                /*subpooladvice->AddAdvice(tiertype, rule.GetAdviceType(), advice);*/
                subpooladvice->AddAdvice(diskkey, rule.GetAdviceType(), advice);
                advicenum++;
            }

#if 0
            ret = subpoolheat->GetTierInternalDiskWiseNextColdOBJ(tiertype, rule, &obj);
#endif
            ret = diskheat->GetNextColdOBJ(rule, &obj);
        }

        if((advicenum > 0) && (YFS_ENOENT == ret))
        {
            tier = NULL;
            disk = NULL;
            subpool->GetTier(tiertype, &tier);
            YWB_ASSERT(tier != NULL);
            tier->GetDisk(diskkey, &disk);
            YWB_ASSERT(disk != NULL);
            disk->ClearFlag(Disk::DF_no_advice);
            tier->PutDisk(disk);
            tier->ClearFlag(Tier::TF_no_advice);
            subpool->PutTier(tier);
            ret = YWB_SUCCESS;
            ast_log_debug("Totally " << advicenum
                    << " advise for disk: ["<< diskkey.GetSubPoolId()
                    << ", "<< diskkey.GetDiskId() << "]");
        }

        PutSubPoolAdvice(subpooladvice);
        config->PutSubPool(subpool);
    }

    return ret;
}

/*TODO:*/
ywb_status_t
AdviceManager::IntersectAdviceAndFilterRule(SubPoolKey& subpoolkey,
        DiskKey& diskkey, AdviceRule& rule,
        AdviceRule* filterrule, ywb_uint32_t filternum)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
AdviceManager::GenerateAdvice(SubPoolKey& key, ywb_uint32_t tier,
        AdviceRule& rule, AdviceRule* filterrule, ywb_uint32_t filternum)
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Generate advice for subpool: ["
            << key.GetOss()
            << ", " << key.GetPoolId()
            << ", " << key.GetSubPoolId()
            << "] and tier: " << tier);

    ret = AdoptAdviceRule(key, tier, rule);
    if(YWB_SUCCESS == ret)
    {
        ret = AdoptFilterRule(key, tier, filterrule, filternum);
        if(YWB_SUCCESS == ret)
        {
            ret = IntersectAdviceAndFilterRule(key,
                    tier, rule, filterrule, filternum);
        }
    }

    return ret;
}

ywb_status_t
AdviceManager::GenerateAdvice(
        SubPoolKey& subpool, ywb_uint32_t tier, DiskKey& disk,
        AdviceRule& rule, AdviceRule* filterrule, ywb_uint32_t filternum)
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Generate advice for disk: ["
            << disk.GetSubPoolId()
            << ", " << disk.GetDiskId()
            << "] and tier: " << tier);

    ret = AdoptAdviceRule(subpool, tier, disk, rule);
    if(YWB_SUCCESS == ret)
    {
        ret = AdoptFilterRule(subpool, tier, disk, filterrule, filternum);
    }

    if(YWB_SUCCESS == ret)
    {
        ret = IntersectAdviceAndFilterRule(subpool, tier, disk,
                rule, filterrule, filternum);
    }

    return ret;
}

ywb_status_t
AdviceManager::GenerateAdvice(SubPoolKey& subpool, DiskKey& disk,
        AdviceRule& rule, AdviceRule* filterrule, ywb_uint32_t filternum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ast_log_debug("Generate advice for disk: ["
            << disk.GetSubPoolId()
            << ", " << disk.GetDiskId()
            << "]");

    ret = AdoptAdviceRule(subpool, disk, rule);
    if(YWB_SUCCESS == ret)
    {
        ret = AdoptFilterRule(subpool, disk, filterrule, filternum);
    }

    if(YWB_SUCCESS == ret)
    {
        ret = IntersectAdviceAndFilterRule(subpool,
                disk, rule, filterrule, filternum);
    }

    return ret;
}

ywb_status_t
AdviceManager::PreGenerateCheck(SubPool* subpool)
{
    ywb_status_t ret = YWB_SUCCESS;
    Tier* tier = NULL;
    Disk* disk = NULL;
    ywb_uint32_t tiertype = Tier::TIER_cnt;
    ywb_uint32_t nodisktiernum = 0;
    ywb_uint32_t noobjtiernum = 0;

    YWB_ASSERT(subpool != NULL);
    if(SubPool::TC_cnt == subpool->GetCurTierComb())
    {
        subpool->SetFlagComb(SubPool::SPF_no_disk);
        subpool->SetFlagComb(SubPool::SPF_no_obj);
        subpool->SetFlagComb(SubPool::SPF_no_advice);
        return YFS_ENODATA;
    }
    else
    {
        subpool->ClearFlag(SubPool::SPF_no_disk);
        /*
         * we will not clear SPF_no_advice flag until AdviceManager
         * do generate advice for this subpool
         **/
        subpool->SetFlagComb(SubPool::SPF_no_advice);
    }

    for(tiertype = Tier::TIER_0; tiertype < Tier::TIER_cnt; tiertype++)
    {
        ret = subpool->GetTier(tiertype, &tier);
        if((YWB_SUCCESS == ret) && (tier != NULL))
        {
            /*tier has no disk*/
            if((0 == tier->GetDiskCnt()))
            {
                tier->SetFlagComb(Tier::TF_no_disk);
                tier->SetFlagComb(Tier::TF_no_obj);
                tier->SetFlagComb(Tier::TF_no_advice);
                nodisktiernum++;
            }
            else
            {
                tier->ClearFlag(Tier::TF_no_disk);
                /*
                 * we will not clear TF_no_advice flag until AdviceManager
                 * do generate advice on this tier
                 **/
                tier->SetFlagComb(Tier::TF_no_advice);
            }

            /*tier has no OBJ*/
            if((0 == tier->GetOBJCnt()))
            {
                tier->SetFlagComb(Tier::TF_no_obj);
                noobjtiernum++;
            }
            else
            {
                tier->ClearFlag(Tier::TF_no_obj);
            }

            ret = tier->GetFirstDisk(&disk);
            while(YWB_SUCCESS == ret)
            {
                /*
                 * we will not clear DF_no_advice flag until AdviceManager
                 * do generate advice for this disk
                 **/
                disk->SetFlagComb(Disk::DF_no_advice);
                ret = tier->GetNextDisk(&disk);
            }

            subpool->PutTier(tier);
        }
        else
        {
            nodisktiernum++;
            noobjtiernum++;
        }
    }

    YWB_ASSERT(nodisktiernum != Tier::TIER_cnt);
    if(noobjtiernum == Tier::TIER_cnt)
    {
        subpool->SetFlagComb(SubPool::SPF_no_obj);
        return YFS_ENODATA;
    }
    else
    {
        subpool->ClearFlag(SubPool::SPF_no_obj);
    }

    return YWB_SUCCESS;
}

ywb_status_t
AdviceManager::GenerateAdvice(SubPoolKey& key, AdviceRule& rule,
        AdviceRule* filterrule, ywb_uint32_t filternum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t filterindex = 0;
    ywb_uint32_t tiercomb = SubPool::TC_cnt;
    Tier* tiers[Constant::TIER_NUM] = {NULL, NULL, NULL};
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    DiskKey diskkey;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ret = config->GetSubPool(key, &subpool);
    if(YWB_SUCCESS == ret && NULL != subpool)
    {
        /*do not generate advise for subpool marked as no migration*/
        if(subpool->TestFlag(SubPool::SPF_no_migration))
        {
            ast_log_info("will not generate advice for subpool: "
                       << "[" << key.GetOss()
                       << ", " << key.GetPoolId()
                       << ", " << key.GetSubPoolId()
                       << "] for it is set to be no data movement");
            config->PutSubPool(subpool);
            return YFS_ENODATA;
        }

        if((filterrule != NULL) && (filternum > Constant::MAX_FILTER_NUM))
        {
            ast_log_debug("Too much filter rules!");
            config->PutSubPool(subpool);
            return YFS_EINVAL;
        }

        ast_log_debug("Advice rule: [advicetype: " << rule.GetAdviceType()
                << ", partscope: " << rule.GetPartitionScope()
                << ", partbase: " << rule.GetPartitionBase()
                << ", sortbase: " << rule.GetSortBase()
                << ", selector: " << rule.GetSelector()
                << "]");

        if((filterrule != NULL) && (filternum != 0))
        {
            for(filterindex = 0; filterindex < filternum; filterindex++)
            {
                ast_log_debug("Filter rule[" << filterindex
                        << "]: [advicetype: " << filterrule[filterindex].GetAdviceType()
                        << ", partscope: " << filterrule[filterindex].GetPartitionScope()
                        << ", partbase: " << filterrule[filterindex].GetPartitionBase()
                        << ", sortbase: " << filterrule[filterindex].GetSortBase()
                        << ", selector: " << filterrule[filterindex].GetSelector()
                        << "]");
            }
        }

        /*advice rule validation*/
        ret = rule.ValidateRule();
        if(YWB_SUCCESS != ret)
        {
            ast_log_debug("Advice rule validation fails");
            config->PutSubPool(subpool);
            return ret;
        }

        /*filter rule validation*/
        if((filterrule != NULL) && (filternum != 0))
        {
            for(filterindex = 0; filterindex < filternum; filterindex++)
            {
                ret = filterrule[filterindex].ValidateRule();
                if(YWB_SUCCESS != ret)
                {
                    ast_log_debug("Filter rule validation fails");
                    config->PutSubPool(subpool);
                    return ret;
                }
            }
        }

        /*advice rule and filter rule consistency check*/
        for(filterindex = 0; filterindex < filternum; filterindex++)
        {
            if((rule.GetAdviceType() !=
                    filterrule[filterindex].GetAdviceType()) ||
                    (rule.GetPartitionScope() !=
                            filterrule[filterindex].GetPartitionScope()))
            {
                return YFS_EINVAL;
            }
        }

        tiercomb = subpool->GetCurTierComb();
        subpool->GetTier(Tier::TIER_0, &tiers[Tier::TIER_0]);
        subpool->GetTier(Tier::TIER_1, &tiers[Tier::TIER_1]);
        subpool->GetTier(Tier::TIER_2, &tiers[Tier::TIER_2]);
        switch(tiercomb)
        {
        case SubPool::TC_ssd:
            if(PartitionScope::PS_tier_wise == rule.GetPartitionScope())
            {
                ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_0,
                        rule, filterrule, filternum);
                /*
                 * if GenerateAdvice returns successfully but with no
                 * advice, the return code should be set to YFS_ENODATA
                 **/
                if((YWB_SUCCESS == ret))
                {
                    subpool->ClearFlag(SubPool::SPF_no_advice);
                }
            }
            else if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
            {
                /*
                 * currently will generate advice for all disks in case of
                 * swap-required disks comes after generating advice
                 * */
                ret = tiers[Tier::TIER_0]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_0, diskkey,
                            rule, filterrule, filternum);
                    if(!(disk->TestFlag(Disk::DF_no_advice)))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_0]->GetNextDisk(&disk, &diskkey);
                }
            }

            break;
        case SubPool::TC_ent:
            if(PartitionScope::PS_tier_wise == rule.GetPartitionScope())
            {
                ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_1,
                        rule, filterrule, filternum);
                if((YWB_SUCCESS == ret))
                {
                    subpool->ClearFlag(SubPool::SPF_no_advice);
                }
            }
            else if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
            {
                ret = tiers[Tier::TIER_1]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_1, diskkey,
                            rule, filterrule, filternum);
                    if(!(disk->TestFlag(Disk::DF_no_advice)))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_1]->GetNextDisk(&disk, &diskkey);
                }
            }

            break;
        case SubPool::TC_sata:
            if(PartitionScope::PS_tier_wise == rule.GetPartitionScope())
            {
                ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_2,
                        rule, filterrule, filternum);
                if((YWB_SUCCESS == ret))
                {
                    subpool->ClearFlag(SubPool::SPF_no_advice);
                }
            }
            else if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
            {
                ret = tiers[Tier::TIER_2]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_2, diskkey,
                            rule, filterrule, filternum);
                    if(!(disk->TestFlag(Disk::DF_no_advice)))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_2]->GetNextDisk(&disk, &diskkey);
                }
            }

            break;
        case SubPool::TC_ssd_ent:
            if(PartitionScope::PS_tier_wise == rule.GetPartitionScope())
            {
                ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_0,
                        rule, filterrule, filternum);
                if((YWB_SUCCESS == ret) || (YFS_ENODATA == ret))
                {
                    ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_1,
                            rule, filterrule, filternum);
                }

                YWB_ASSERT(tiers[Tier::TIER_0]);
                YWB_ASSERT(tiers[Tier::TIER_1]);
                if(!tiers[Tier::TIER_0]->TestFlag(Tier::TF_no_advice) ||
                        !tiers[Tier::TIER_1]->TestFlag(Tier::TF_no_advice))
                {
                    subpool->ClearFlag(SubPool::SPF_no_advice);
                }
            }
            else if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
            {
                ret = tiers[Tier::TIER_0]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_0, diskkey,
                            rule, filterrule, filternum);
                    if(!(disk->TestFlag(Disk::DF_no_advice)))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_0]->GetNextDisk(&disk, &diskkey);
                }

                ret = tiers[Tier::TIER_1]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_1, diskkey,
                            rule, filterrule, filternum);
                    if((subpool->TestFlag(SubPool::SPF_no_advice)) &&
                            (!(disk->TestFlag(Disk::DF_no_advice))))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_1]->GetNextDisk(&disk, &diskkey);
                }
            }

            break;
        case SubPool::TC_ssd_sata:
            if(PartitionScope::PS_tier_wise == rule.GetPartitionScope())
            {
                ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_0,
                        rule, filterrule, filternum);
                if((YWB_SUCCESS == ret) || (YFS_ENODATA == ret))
                {
                    ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_2,
                            rule, filterrule, filternum);
                }

                YWB_ASSERT(tiers[Tier::TIER_0]);
                YWB_ASSERT(tiers[Tier::TIER_2]);
                if(!tiers[Tier::TIER_0]->TestFlag(Tier::TF_no_advice) ||
                        !tiers[Tier::TIER_2]->TestFlag(Tier::TF_no_advice))
                {
                    subpool->ClearFlag(SubPool::SPF_no_advice);
                }
            }
            else if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
            {
                ret = tiers[Tier::TIER_0]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_0, diskkey,
                            rule, filterrule, filternum);
                    if(!(disk->TestFlag(Disk::DF_no_advice)))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_0]->GetNextDisk(&disk, &diskkey);
                }

                ret = tiers[Tier::TIER_2]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_2, diskkey,
                            rule, filterrule, filternum);
                    if((subpool->TestFlag(SubPool::SPF_no_advice)) &&
                            (!(disk->TestFlag(Disk::DF_no_advice))))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_2]->GetNextDisk(&disk, &diskkey);
                }
            }

            break;
        case SubPool::TC_ent_sata:
            if(PartitionScope::PS_tier_wise == rule.GetPartitionScope())
            {
                ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_1,
                        rule, filterrule, filternum);
                if((YWB_SUCCESS == ret) || (YFS_ENODATA == ret))
                {
                    ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_2,
                            rule, filterrule, filternum);
                }

                YWB_ASSERT(tiers[Tier::TIER_1]);
                YWB_ASSERT(tiers[Tier::TIER_2]);
                if(!tiers[Tier::TIER_1]->TestFlag(Tier::TF_no_advice) ||
                        !tiers[Tier::TIER_2]->TestFlag(Tier::TF_no_advice))
                {
                    subpool->ClearFlag(SubPool::SPF_no_advice);
                }
            }
            else if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
            {
                ret = tiers[Tier::TIER_1]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_1, diskkey,
                            rule, filterrule, filternum);
                    if(!(disk->TestFlag(Disk::DF_no_advice)))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_1]->GetNextDisk(&disk, &diskkey);
                }

                ret = tiers[Tier::TIER_2]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_2, diskkey,
                            rule, filterrule, filternum);
                    if((subpool->TestFlag(SubPool::SPF_no_advice)) &&
                            (!(disk->TestFlag(Disk::DF_no_advice))))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_2]->GetNextDisk(&disk, &diskkey);
                }
            }

            break;
        case SubPool::TC_ssd_ent_sata:
            if(PartitionScope::PS_tier_wise == rule.GetPartitionScope())
            {
                ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_0,
                        rule, filterrule, filternum);
                if((YWB_SUCCESS == ret) || (YFS_ENODATA == ret))
                {
                    ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_1,
                            rule, filterrule, filternum);
                }
                else
                {
                    break;
                }

                if((YWB_SUCCESS == ret) || (YFS_ENODATA == ret))
                {
                    ret = GenerateAdvice(key, (ywb_uint32_t)Tier::TIER_2,
                            rule, filterrule, filternum);
                }

                YWB_ASSERT(tiers[Tier::TIER_0]);
                YWB_ASSERT(tiers[Tier::TIER_1]);
                YWB_ASSERT(tiers[Tier::TIER_2]);
                if(!tiers[Tier::TIER_0]->TestFlag(Tier::TF_no_advice) ||
                        !tiers[Tier::TIER_1]->TestFlag(Tier::TF_no_advice) ||
                        !tiers[Tier::TIER_2]->TestFlag(Tier::TF_no_advice))
                {
                    subpool->ClearFlag(SubPool::SPF_no_advice);
                }
            }
            else if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
            {
                ret = tiers[Tier::TIER_0]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_0, diskkey,
                            rule, filterrule, filternum);
                    if(!(disk->TestFlag(Disk::DF_no_advice)))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_0]->GetNextDisk(&disk, &diskkey);
                }

                ret = tiers[Tier::TIER_1]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_1, diskkey,
                            rule, filterrule, filternum);
                    if((subpool->TestFlag(SubPool::SPF_no_advice)) &&
                            (!(disk->TestFlag(Disk::DF_no_advice))))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_1]->GetNextDisk(&disk, &diskkey);
                }

                ret = tiers[Tier::TIER_2]->GetFirstDisk(&disk, &diskkey);
                while(YWB_SUCCESS == ret)
                {
                    GenerateAdvice(key, Tier::TIER_2, diskkey,
                            rule, filterrule, filternum);
                    if((subpool->TestFlag(SubPool::SPF_no_advice)) &&
                            (!(disk->TestFlag(Disk::DF_no_advice))))
                    {
                        subpool->ClearFlag(SubPool::SPF_no_advice);
                    }

                    config->PutDisk(disk);
                    ret = tiers[Tier::TIER_2]->GetNextDisk(&disk, &diskkey);
                }
            }

            break;
        default:
            /*
             * subpool with tier combination of SubPool::TC_cnt should
             * be filtered by PreGenerateCheck
             **/
            YWB_ASSERT(0);
            break;
        }

        if(tiers[Tier::TIER_0])
        {
            subpool->PutTier(tiers[Tier::TIER_0]);
        }

        if(tiers[Tier::TIER_1])
        {
            subpool->PutTier(tiers[Tier::TIER_1]);
        }

        if(tiers[Tier::TIER_2])
        {
            subpool->PutTier(tiers[Tier::TIER_2]);
        }

        config->PutSubPool(subpool);
    }

    return ret;
}

/*TODO*/
ywb_status_t
AdviceManager::GenerateAdvice(SubPoolKey& key, AdviceRule& rule,
        void* less(void* key1, void*key2), void* filterchain)
{
    return YWB_SUCCESS;
}

ywb_status_t
AdviceManager::Destroy(SubPoolKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolAdvice*, SubPoolKeyCmp>::iterator iter;
    SubPoolAdvice* subpooladvice = NULL;

    Lock();
    iter = mAdvise.find(key);
    if(iter != mAdvise.end())
    {
        subpooladvice = iter->second;
        mAdvise.erase(iter);
        subpooladvice->DecRef();
    }
    else
    {
        ret = YFS_ENOENT;
    }
    Unlock();

    return ret;
}

ywb_status_t
AdviceManager::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolAdvice*, SubPoolKeyCmp>::iterator iter;
    SubPoolAdvice* subpooladvice = NULL;

    Lock();
    iter = mAdvise.begin();
    for(; iter != mAdvise.end(); )
    {
        subpooladvice = iter->second;
        mAdvise.erase(iter++);
        subpooladvice->DecRef();
    }
    Unlock();

    mAst = NULL;
    return ret;
}

ywb_status_t
AdviceManager::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolAdvice*, SubPoolKeyCmp>::iterator iter;
    SubPoolAdvice* subpooladvice = NULL;
    SubPoolKey subpoolkey;

    ast_log_debug("AdviceManager reset");
    Lock();
    iter = mAdvise.begin();
    for(; iter != mAdvise.end(); )
    {
        subpoolkey = iter->first;
        ast_log_debug("Destroy subpool advise of subpool: ["
                << subpoolkey.GetOss()
                << ", " << subpoolkey.GetPoolId()
                << ", " << subpoolkey.GetSubPoolId()
                << "]");
        subpooladvice = iter->second;
        mAdvise.erase(iter++);
        subpooladvice->DecRef();
    }
    Unlock();

    return ret;
}

ywb_status_t
AdviceManager::Init()
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPoolKey subpoolkey;
    SubPool* subpool = NULL;
    SubPoolAdvice* subpooladvice = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);

    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        subpooladvice = new SubPoolAdvice(subpool->GetCurTierComb());
        if(NULL == subpooladvice)
        {
            ast_log_debug("Out of memory!");
            config->PutSubPool(subpool);
            ret = YFS_EOUTOFMEMORY;
            return ret;
        }
        else
        {
            ret = AddSubPoolAdvice(subpoolkey, subpooladvice);
            YWB_ASSERT(YWB_SUCCESS == ret);
        }

        config->PutSubPool(subpool);
        ret = config->GetNextSubPool(&subpool, &subpoolkey);
    }

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
