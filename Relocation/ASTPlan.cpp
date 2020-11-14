#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTAdvice.hpp"
#include "AST/ASTPerformance.hpp"
#include "AST/ASTArb.hpp"
#include "AST/ASTPlan.hpp"
#ifdef WITH_INITIAL_PLACEMENT
#include "AST/ASTSwapOut.hpp"
#endif

void
DiskScheStat::UpdateDiskScheTimesByNum(ywb_uint32_t num, bool increase)
{
    if(increase)
    {
        mDiskScheTimes += num;
    }
    else
    {
        mDiskScheTimes -= num;
    }
}

bool
NodeCmp::operator()(const Node* nodea, const Node* nodeb) const
{
    if(nodea->mDynamicPriority > nodeb->mDynamicPriority)
    {
        return true;
    }
    else
    {
        return false;
    }
}

SubPoolPlanDirStat::SubPoolPlanDirStat()
{
    ywb_uint32_t direction = Plan::PD_ctr_promote;
    mCurAvailPlanDirs = 0;
    for(; direction < Plan::PD_cnt; direction++)
    {
        mPlanDirScheTimes[direction] = 0;
        mPlanDirPenalty[direction] = 0;
        mCurAvailPlanDirs |= 1 << direction;
    }

    /*
     * should keep sum of all plan directions' priority equals to 100,
     * the smaller the higher the priority is.
     **/
    mPlanDirStaticPriority[Plan::PD_ctr_promote] = 20;
    mPlanDirStaticPriority[Plan::PD_ctr_demote] = 30;
    mPlanDirStaticPriority[Plan::PD_swap_up] = 25;
    mPlanDirStaticPriority[Plan::PD_swap_down] = 25;
    mLastMissedPlanDir = Plan::PD_cnt;
    InitDynamicPlanPriority();
}

SubPoolPlanDirStat::~SubPoolPlanDirStat()
{
    Destroy();
}

ywb_status_t
SubPoolPlanDirStat::InitDynamicPlanPriority()
{
    ywb_status_t ret = YWB_SUCCESS;
    Node* node = NULL;

    ywb_uint32_t direction = Plan::PD_ctr_promote;
    for(; direction < Plan::PD_cnt; direction++)
    {
        node = new Node(direction, mPlanDirStaticPriority[direction]);
        if(NULL == node)
        {
            ret = YFS_EOUTOFMEMORY;
            return ret;
        }

        mPlanDirDynamicPriority.push(node);
    }

    return ret;
}

void
SubPoolPlanDirStat::UpdatePlanDirScheTimes(
        ywb_uint32_t direction, ywb_uint32_t num, bool increase)
{
    YWB_ASSERT(0 <= direction && direction < Plan::PD_cnt);
    if(increase)
    {
        mPlanDirScheTimes[direction] += num;
    }
    else
    {
        if(mPlanDirScheTimes[direction] > num)
        {
            mPlanDirScheTimes[direction] -= num;
        }
        else
        {
            mPlanDirScheTimes[direction] = 0;
        }
    }
}

void
SubPoolPlanDirStat::UpdatePlanDirPenalty(
        ywb_uint32_t direction, ywb_uint32_t num, bool increase)
{
    YWB_ASSERT(0 <= direction && direction < Plan::PD_cnt);
    if(increase)
    {
        mPlanDirPenalty[direction] += num;
    }
    else
    {
        if(mPlanDirPenalty[direction] > num)
        {
            mPlanDirPenalty[direction] -= num;
        }
        else
        {
            mPlanDirPenalty[direction] = 0;
        }
    }
}

void
SubPoolPlanDirStat::ResetLastMissedPlanDir()
{
    mLastMissedPlanDir = Plan::PD_cnt;
}

ywb_uint32_t
SubPoolPlanDirStat::CalculateDynamicPriority(ywb_uint32_t direction)
{
    YWB_ASSERT(0 <= direction && direction < Plan::PD_cnt);
    /*penalty will eventually reflect on schedule times*/
    mPlanDirScheTimes[direction] += mPlanDirPenalty[direction];

    if(0 != mPlanDirScheTimes[direction])
    {
        return (mPlanDirStaticPriority[direction]) *
                    (mPlanDirScheTimes[direction]);
    }
    else
    {
        return mPlanDirStaticPriority[direction];
    }
}

ywb_uint32_t
SubPoolPlanDirStat::GetHigherPlanDirection(
        ywb_uint32_t direction, ywb_uint32_t direction2)
{
    /*the higher the smaller*/
    if(GetPlanDirStaticPriority(direction) <
            GetPlanDirStaticPriority(direction2))
    {
        return direction;
    }
    else
    {
        return direction2;
    }
}

ywb_uint32_t
SubPoolPlanDirStat::ExcludeDirectionFromAvailableDirections(
        ywb_uint32_t availableset, ywb_uint32_t direction)
{
    ywb_uint32_t tobeexcludedval = 0;

    if((0 <= direction) && (direction < Plan::PD_cnt))
    {
        tobeexcludedval = 1 << direction;
        if(availableset & tobeexcludedval)
        {
            availableset &= (~tobeexcludedval);
        }
    }

    /*DO NOT TOUCH mPlanDirDynamicPriority!*/
    return availableset;
}

ywb_uint32_t
SubPoolPlanDirStat::ExcludeDirectionFromAvailableDirections(
        ywb_uint32_t direction)
{
    ywb_uint32_t tobeexcludedval = 0;
    ywb_uint32_t availableset = mCurAvailPlanDirs;
    Node* node = NULL;
    vector<Node*> nodevec;
    vector<Node*>::iterator iter;

    if((0 <= direction) && (direction < Plan::PD_cnt))
    {
        tobeexcludedval = 1 << direction;
        if(availableset & tobeexcludedval)
        {
            availableset &= (~tobeexcludedval);
            nodevec.reserve(mPlanDirDynamicPriority.size());
            while(!mPlanDirDynamicPriority.empty())
            {
                node = mPlanDirDynamicPriority.top();
                mPlanDirDynamicPriority.pop();
                if(node->GetPlanDirection() != direction)
                {
                    nodevec.push_back(node);
                }
                else
                {
                    delete node;
                    node = NULL;
                    break;
                }
            }

            iter = nodevec.begin();
            for(; iter != nodevec.end(); iter++)
            {
                mPlanDirDynamicPriority.push(*iter);
            }

            nodevec.clear();
        }
    }

    mCurAvailPlanDirs = availableset;
    return availableset;
}

/*
 * FIXME: there should be a more sophisticated algorithm
 **/
ywb_uint32_t
SubPoolPlanDirStat::GetPriorPlanDirectionFromAvailable(
        ywb_uint32_t availableset)
{
    ywb_uint32_t direction = 0;
    ywb_uint32_t dynamicalpriority = 0;
    Node* node = NULL;
    vector<Node*> nodevec;
    vector<Node*>::iterator iter;

    /*firstly adjust dynamic priority for all plan directions*/
    nodevec.reserve(mPlanDirDynamicPriority.size());
    while(!mPlanDirDynamicPriority.empty())
    {
        node = mPlanDirDynamicPriority.top();
        mPlanDirDynamicPriority.pop();
        direction = node->GetPlanDirection();

        dynamicalpriority = CalculateDynamicPriority(direction);
        node->SetPriority(dynamicalpriority);
        nodevec.push_back(node);
    }

    /*add node back to priority queue*/
    if(!nodevec.empty())
    {
        for(iter = nodevec.begin(); iter != nodevec.end(); iter++)
        {
            mPlanDirDynamicPriority.push(*iter);
        }

        nodevec.clear();
    }

    /*get direction with highest priority*/
    while(!mPlanDirDynamicPriority.empty())
    {
        node = mPlanDirDynamicPriority.top();
        mPlanDirDynamicPriority.pop();
        direction = node->GetPlanDirection();

        /*direction is in available direction set*/
        if(availableset & (1 << direction))
        {
            if(!nodevec.empty())
            {
                for(iter = nodevec.begin(); iter != nodevec.end(); iter++)
                {
                    mPlanDirDynamicPriority.push(*iter);
                }

                nodevec.clear();
            }

            mPlanDirDynamicPriority.push(node);
            return direction;
        }
        else
        {
            nodevec.push_back(node);
        }
    }

    if(!nodevec.empty())
    {
        for(iter = nodevec.begin(); iter != nodevec.end(); iter++)
        {
            mPlanDirDynamicPriority.push(*iter);
        }

        nodevec.clear();
    }

    return Plan::PD_cnt;
}

/*
 * FIXME: there should be a more sophisticated algorithm
 **/
ywb_uint32_t
SubPoolPlanDirStat::GetPriorPlanDirectionFromAvailable()
{
    ywb_uint32_t availableset = mCurAvailPlanDirs;

    return GetPriorPlanDirectionFromAvailable(availableset);
}

ywb_uint32_t
SubPoolPlanDirStat::GetAdviceTypeFromPlanDirection(ywb_uint32_t direction)
{
    ywb_uint32_t advicetype = 0;

    YWB_ASSERT((0 <= direction) && (direction < Plan::PD_cnt));
    switch(direction)
    {
    case Plan::PD_ctr_promote:
        advicetype = AdviceType::AT_ctr_hot;
        break;
    case Plan::PD_ctr_demote:
        advicetype = AdviceType::AT_ctr_cold;
        break;
    case Plan::PD_swap_up:
        advicetype = AdviceType::AT_swap_up;
        break;
    case Plan::PD_swap_down:
        advicetype = AdviceType::AT_swap_down;
        break;
    default:
        advicetype = AdviceType::AT_cnt;
        break;
    }

    return advicetype;
}

ywb_status_t
SubPoolPlanDirStat::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    Node* node = NULL;

    while(!mPlanDirDynamicPriority.empty())
    {
        node = mPlanDirDynamicPriority.top();
        mPlanDirDynamicPriority.pop();
        delete node;
        node = NULL;
    }

    return ret;
}

ywb_status_t
SubPoolPlans::AddPlan(Plan* plan)
{
    ywb_status_t ret = YWB_SUCCESS;

    YWB_ASSERT(plan != NULL);
    mPlans.push_back(plan);

    return ret;
}

ywb_status_t
SubPoolPlans::GetFirstPlan(Plan** plan)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mPlans.empty())
    {
        mIt = mPlans.begin();
        *plan = *mIt;
    }
    else
    {
        *plan = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolPlans::GetNextPlan(Plan** plan)
{
    ywb_status_t ret = YWB_SUCCESS;

    mIt++;
    if(mIt != mPlans.end())
    {
        *plan = *mIt;
    }
    else
    {
        /*
         * back one step, this is very important, image such a scenario:
         * firstly AST generates lots of plans, then call GetFirstPlan
         * and GetNextPlan to drain all the generated plans until reaching
         * the end, at this point, the @mIt will be equal to mPlans.end(),
         * if the AST generates some plans again which are put at the back
         * of list and then call GetNextPlan, those newly generated plans
         * will not be got for mIt is equals to mPlans.end()! So iterator
         * gets back by one step is necessary.
         **/
        mIt--;
        *plan = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolPlans::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    Plan* plan = NULL;

    mIt = mPlans.begin();
    for(; mIt != mPlans.end(); )
    {
        plan = *mIt;
        mPlans.erase(mIt++);
        delete plan;
        plan = NULL;
    }

    mSubPoolAdvise->DecRef();
    return ret;
}

ywb_bool_t
TimeOut::IsTimedOut()
{
    if((mStart + mInterval) <= ywb_time_now())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
TimeOutCmp::operator()(const TimeOut& key1, const TimeOut& key2) const
{
    ywb_time_t endtime1;
    ywb_time_t endtime2;

    endtime1 = key1.mStart + key1.mInterval;
    endtime2 = key2.mStart + key2.mInterval;

    if(endtime1 != endtime2)
    {
        return endtime1 < endtime2;
    }
    else if(key1.mStart != key2.mStart)
    {
        return key1.mStart < key2.mStart;
    }
    else
    {
        return false;
    }
}

ywb_status_t
PlanManager::GetFirstSubPoolPlans(SubPoolPlans** subpool, SubPoolKey* key)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mPlans.empty())
    {
        mIt = mPlans.begin();
        *subpool = mIt->second;
        *key = mIt->first;
        mIt++;
    }
    else
    {
        *subpool = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::GetNextSubPoolPlans(SubPoolPlans** subpool, SubPoolKey* key)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIt != mPlans.end())
    {
        *subpool = mIt->second;
        *key = mIt->first;
        mIt++;
    }
    else
    {
        *subpool = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::GetSubPoolPlans(SubPoolKey& subpoolkey,
        SubPoolPlans** subpoolplanmgr)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolPlans*, SubPoolKeyCmp>::iterator iter;

    iter = mPlans.find(subpoolkey);
    if(iter != mPlans.end())
    {
        *subpoolplanmgr = iter->second;
    }
    else
    {
        *subpoolplanmgr = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::RemoveSubPoolPlans(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolPlans*, SubPoolKeyCmp>::iterator iter;
    SubPoolPlans* subpoolplans = NULL;

    iter = mPlans.find(subpoolkey);
    if(iter != mPlans.end())
    {
        subpoolplans = iter->second;
        mPlans.erase(iter);
        delete subpoolplans;
        subpoolplans = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::GetDiskScheStat(DiskKey& diskkey,
            DiskScheStat** diskstat)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskKey, DiskScheStat*, DiskKeyCmp>::iterator iter;

    iter = mDiskScheStat.find(diskkey);
    if(iter != mDiskScheStat.end())
    {
        *diskstat = iter->second;
    }
    else
    {
        *diskstat = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::RemoveDiskScheStat(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskKey, DiskScheStat*, DiskKeyCmp>::iterator iter;
    DiskScheStat* disksche = NULL;

    iter = mDiskScheStat.find(diskkey);
    if(iter != mDiskScheStat.end())
    {
        disksche = iter->second;
        mDiskScheStat.erase(iter);
        delete disksche;
        disksche = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::AddUnavailDisk(DiskKey& diskkey)
{
    return mUnavailDisks.AddDisk(diskkey);
}

ywb_status_t
PlanManager::RemoveUnavailDisk(DiskKey& diskkey)
{
    return mUnavailDisks.RemoveDisk(diskkey);
}

ywb_status_t
PlanManager::AddAvailDisk(DiskKey& diskkey)
{
    return mAvailDisks.AddDisk(diskkey);
}

ywb_status_t
PlanManager::RemoveAvailDisk(DiskKey& diskkey)
{
    return mAvailDisks.RemoveDisk(diskkey);
}

ywb_status_t
PlanManager::ScheduleFirstBatch(
        SubPoolKey& subpoolkey, ywb_uint32_t* numofplan)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t availplandirs = 0;
    ywb_uint32_t highestplandir= 0;
    ywb_uint32_t advicetype = 0;
    ywb_uint32_t iopsrequired = 0;
    ywb_uint32_t bwrequired = 0;
    ywb_uint32_t caprequired = 0;
    ywb_uint32_t heattype = 0;
    ywb_uint32_t tgttier = 0;
    DiskKey targetdiskkey;
    DiskKey sourcediskkey;
    OBJKey objkey;
    bool found = false;
    SubPoolPlans* subpoolplanmgr = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Executor* exe = NULL;
    Error* err = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;
    PerfManager* perfmgr = NULL;
    ChunkIOStat* iostat = NULL;
    Plan* plan = NULL;
    DiskScheStat* sourcedisk = NULL;
    DiskScheStat* targetdisk = NULL;
#ifdef WITH_INITIAL_PLACEMENT
    ywb_bool_t swapdue = ywb_false;
    ywb_bool_t swaprequired = ywb_false;
    ywb_uint32_t tiercomb = SubPool::TC_cnt;
    SwapOutManager* swapmgr = NULL;
#endif

    ast_log_debug("Schedule first batch for subpool: ["
            << "" << subpoolkey.GetOss()
            << ", " << subpoolkey.GetPoolId()
            << ", " << subpoolkey.GetSubPoolId()
            << "]");

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ast->GetExecutor(&exe);
    ast->GetPerfManager(&perfmgr);
    ast->GetError(&err);
    err->SetPLNErr(Error::COM_success);
#ifdef WITH_INITIAL_PLACEMENT
    ast->GetSwapOutManager(&swapmgr);
#endif

    ret = GetSubPoolPlans(subpoolkey, &subpoolplanmgr);
//    YWB_ASSERT((YWB_SUCCESS == ret) && (subpoolplanmgr != NULL));
    if(YWB_SUCCESS != ret)
    {
        ast_log_info("No plan generated for SubPoolPlans corresponding"
                " does not exist");
        err->SetPLNErr(Error::PLNE_subpool_no_more_plan);
        return ret;
    }

    ret = config->GetSubPool(subpoolkey, &subpool);
    if((YWB_SUCCESS == ret) &&
            (subpool->TestFlag(SubPool::SPF_no_migration)))
    {
        ast_log_info("No plan generated for current scheduled SubPool"
                " is marked as SPF_no_migration");
        /*try again later, for SPF_no_migration may be cleared in next cycle*/
        ret = YFS_EAGAIN;
        err->SetPLNErr(Error::PLNE_subpool_schedule_timed_delay);
        config->PutSubPool(subpool);
        return ret;
    }
    else if(((YWB_SUCCESS == ret) &&
            (subpool->TestFlag(SubPool::SPF_removing))) ||
            (YWB_SUCCESS != ret))
    {
        ast_log_info("No plan generated for SubPool may be deleted");
        ret = YFS_ENODATA;
        subpoolplanmgr->SetFlagComb(SubPoolPlans::SPF_subpool_deleted);
        err->SetPLNErr(Error::PLNE_subpool_no_more_plan);
        config->PutSubPool(subpool);
        return ret;
    }

#ifdef WITH_INITIAL_PLACEMENT
    swapdue = swapmgr->GetSwapRelocationDue(subpoolkey);
    swaprequired = swapmgr->GetSwapRequired(subpoolkey);
    tiercomb = subpool->GetCurTierComb();
    /*
     * if @swaprequired is set to be true, then check whether or not
     * swap is possible by checking whether or not there is target
     * disk for swap
     * */
    if(swapdue && (!swaprequired ||
            (tiercomb == SubPool::TC_ssd ||
            tiercomb == SubPool::TC_ent ||
            tiercomb == SubPool::TC_sata)))
    {
        if(swaprequired)
        {
            ast_log_info("No plan generated for SubPool have swap only"
                    " relocation, but currently no target for swap");
        }
        else
        {
            ast_log_info("No plan generated for SubPool have swap only"
                    " relocation, but currently no swap requirement");
        }

        /*
         * try again later, for swap requirement may comes later or
         * swap target may be added into logical configuration
         * */
        ret = YFS_EAGAIN;
        err->SetPLNErr(Error::PLNE_subpool_schedule_timed_delay);
        return ret;
    }
#endif

    config->PutSubPool(subpool);
    subpoolplanmgr->GetSubPoolPlanDirStat(&plandirstat);
    subpoolplanmgr->GetSubPoolAdvise(&subpooladvice);
    availplandirs = plandirstat->GetCurrentAvailablePlanDirections();

#ifdef WITH_INITIAL_PLACEMENT
    if(swapdue)
    {
        /*exclude PD_ctr_promote and PD_ctr_demote from available plan directions*/
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_ctr_promote);
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_ctr_demote);
    }
    else
#endif
    {
        /*exclude PD_swap_up and PD_swap_down from available plan directions*/
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_swap_up);
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_swap_down);
    }

    while((availplandirs != 0) && (found == false) &&
            (Executor::S_stopping != exe->GetStatus()))
    {
        highestplandir = plandirstat->
                GetPriorPlanDirectionFromAvailable(availplandirs);
        if(Plan::PD_cnt == highestplandir)
        {
            break;
        }

        advicetype =
                plandirstat->GetAdviceTypeFromPlanDirection(highestplandir);
        if(AdviceType::AT_cnt == advicetype)
        {
            break;
        }

#ifdef WITH_INITIAL_PLACEMENT
        if(swaprequired && (AdviceType::AT_swap_up == advicetype))
        {
            ret = swapmgr->ScheduleNextSwapDisk(subpoolkey,
                    SwapOutManager::SD_up, sourcediskkey);
            if(YWB_SUCCESS != ret)
            {
                /*exclude Plan::PD_swap_up from local available plan directions*/
                availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                                availplandirs, highestplandir);
                /*
                 * SHOULD not exclude Plan::PD_swap_up from global plan directions
                 * for swap out requirement may change in next cycle
                 * */
                plandirstat->SetLastMissedPlanDir(highestplandir);
                continue;
            }

            ret = subpooladvice->GetFirstAdvice(sourcediskkey, advicetype, &advice);
        }
        else if(swaprequired && (AdviceType::AT_swap_down == advicetype))
        {
            ret = swapmgr->ScheduleNextSwapDisk(subpoolkey,
                    SwapOutManager::SD_down, sourcediskkey);
            if(YWB_SUCCESS != ret)
            {
                /*exclude Plan::PD_swap_up from local available plan directions*/
                availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                                availplandirs, highestplandir);
                /*SHOULD not exclude Plan::PD_swap_up from global plan directions*/
                plandirstat->SetLastMissedPlanDir(highestplandir);
                continue;
            }

            ret = subpooladvice->GetFirstAdvice(sourcediskkey, advicetype, &advice);
        }
        else
#endif
        {
            ret = subpooladvice->GetFirstAdvice(advicetype, &advice);
        }

        if(!((YWB_SUCCESS == ret) && (advice != NULL)))
        {
            /*
             * can not find one plan of @highestplandir plan direction, then
             * assure there will be no more plan of this plan direction in
             * current schedule, free to exclude @highestplantype from local
             * available plan directions
             **/
            availplandirs = plandirstat->
                    ExcludeDirectionFromAvailableDirections(
                            availplandirs, highestplandir);

#ifdef WITH_INITIAL_PLACEMENT
            if(!swapdue)
#endif
            {
                /*
                 * also assure there will be no more plan of this plan direction in
                 * following schedule, feel free to exclude @highestplantype from
                 * global available plan directions
                 **/
                plandirstat->ExcludeDirectionFromAvailableDirections(
                        highestplandir);
            }

            /*penalty*/
            /*plandirstat->UpdatePlanDirPenalty(highestplandir, 1, true);*/
            plandirstat->SetLastMissedPlanDir(highestplandir);
        }
        else
        {
            do
            {
                /*
                 * will not check the OBJ's existence here, offload the check
                 * to migrator instead
                 **/
//            advice->GetOBJKey(&objkey);
//            if(config->CheckOBJExistence(*objkey))
//            {
                /*can not mark it as found until find a target for advice*/
                advice->GetStat(&iostat);
                YWB_ASSERT(iostat != NULL);
                iopsrequired = iostat->GetRndReadIOs() + iostat->GetRndWriteIOs() +
                        iostat->GetSeqReadIOs() + iostat->GetSeqWriteIOs();
                bwrequired = iostat->GetRndReadBW() + iostat->GetRndWriteBW() +
                        iostat->GetSeqReadBW() + iostat->GetSeqWriteBW();
                caprequired = advice->GetCap();
                tgttier = advice->GetTargetTier();
                heattype = advice->GetHeatType();

                /*get target disk for the got advice*/
                ret = perfmgr->GetFavoriteDisk(subpoolkey, tgttier, heattype,
                        iopsrequired, bwrequired, caprequired, &mUnavailDisks,
                        &mAvailDisks, &targetdiskkey);
                if(YWB_SUCCESS == ret)
                {
                    found = true;
                    /*update schedule times*/
                    plandirstat->UpdatePlanDirScheTimes(highestplandir, 1, true);
                    plandirstat->ResetLastMissedPlanDir();
                    advice->GetOBJKey(&objkey);
                    /*mark the advice as chosen*/
                    advice->SetFlagComb(Advice::ADV_chosen);
                }

                subpooladvice->PutAdvice(advice);
//            }
            } while((false == found) && (YFS_ENOENT != ret) &&
#ifdef WITH_INITIAL_PLACEMENT
                    (((swaprequired) ?
                    (YWB_SUCCESS == subpooladvice->GetNextAdvice(sourcediskkey,
                    advicetype, &advice)) :
                    (YWB_SUCCESS == subpooladvice->GetNextAdvice(
                    advicetype, &advice))) &&
#else
                    ((YWB_SUCCESS == subpooladvice->GetNextAdvice(
                    advicetype, &advice)) &&
#endif
                    (advice != NULL)) &&
                    (Executor::S_stopping != exe->GetStatus()));

            if(found == false)
            {
                availplandirs =
                        plandirstat->ExcludeDirectionFromAvailableDirections(
                                availplandirs, highestplandir);
                /*penalty*/
                plandirstat->SetLastMissedPlanDir(highestplandir);
            }
        }
    }

    /*find the first advice*/
    if(found == true)
    {
        /*deal with plan*/
        plan = new Plan(objkey, targetdiskkey.GetDiskId(),
                subpoolkey, *iostat, heattype,
                caprequired, highestplandir, exe->GetEpoch());
//        subpooladvice->PutAdvice(advice);
        if(NULL == plan)
        {
            ast_log_debug("Out of memory!");
            *numofplan = 0;
            err->SetPLNErr(Error::COME_out_of_memory);
            return YFS_EOUTOFMEMORY;
        }

        /*deal with the chosen source disk*/
        sourcediskkey.SetDiskId(objkey.GetStorageId());
        sourcediskkey.SetSubPoolId(subpoolkey.GetSubPoolId());
        ret = GetDiskScheStat(sourcediskkey, &sourcedisk);
        if(!((YWB_SUCCESS == ret) && (sourcedisk != NULL)))
        {
            sourcedisk = new DiskScheStat();
            if(NULL == sourcedisk)
            {
                ast_log_debug("Out of memory!");
                *numofplan = 0;
                delete plan;
                plan = NULL;
                err->SetPLNErr(Error::COME_out_of_memory);
                return YFS_EOUTOFMEMORY;
            }
            else
            {
                mDiskScheStat.insert(make_pair(sourcediskkey, sourcedisk));
            }
        }

        /*deal with the chosen target disk*/
        ret = GetDiskScheStat(targetdiskkey, &targetdisk);
        if(!((YWB_SUCCESS == ret) && (targetdisk != NULL)))
        {
            targetdisk = new DiskScheStat();
            if(NULL == targetdisk)
            {
                ast_log_debug("Out of memory!");
                *numofplan = 0;
                delete plan;
                plan = NULL;
                err->SetPLNErr(Error::COME_out_of_memory);
                return YFS_EOUTOFMEMORY;
            }
            else
            {
                mDiskScheStat.insert(make_pair(targetdiskkey, targetdisk));
            }
        }

        sourcedisk->SetFlagUniq(DiskScheStat::DSSS_chosen);
        sourcedisk->UpdateDiskScheTimesByNum(1, true);
        targetdisk->SetFlagUniq(DiskScheStat::DSSS_chosen);
        targetdisk->UpdateDiskScheTimesByNum(1, true);

        RemoveAvailDisk(sourcediskkey);
        AddUnavailDisk(sourcediskkey);
        RemoveAvailDisk(targetdiskkey);
        AddUnavailDisk(targetdiskkey);

        /*add plan into subpool plan manager*/
        subpoolplanmgr->AddPlan(plan);
        *numofplan = *numofplan + 1;
        ast_log_debug("Find one plan@" << plan << ": [obj: ["
                << objkey.GetStorageId()
                << ",  " << objkey.GetInodeId()
                << ",  " << objkey.GetChunkId()
                << ",  " << objkey.GetChunkIndex()
                << ",  " << objkey.GetChunkVersion()
                << "], sourcedisk: ["
                << sourcediskkey.GetSubPoolId()
                << ",  " << sourcediskkey.GetDiskId()
                << "], targetdisk: ["
                << targetdiskkey.GetSubPoolId()
                << ",  " << targetdiskkey.GetDiskId()
                << "], direction: " << highestplandir
                << "]");

        ret = ScheduleNextFromSpecifiedPlanDirs(subpoolkey, availplandirs);
        while((YWB_SUCCESS == ret) &&
                (Executor::S_stopping != exe->GetStatus()))
        {
            *numofplan = *numofplan + 1;
            ret = ScheduleNextFromSpecifiedPlanDirs(subpoolkey, availplandirs);
        }
    }
    else
    {
#ifdef WITH_INITIAL_PLACEMENT
        if(swapdue)
        {
            *numofplan = 0;
            err->SetPLNErr(Error::PLNE_subpool_schedule_timed_delay);
            ret = YFS_EAGAIN;
        }
        else
#endif
        {
            /*mark this subpool as no more plan for there is even no first advice*/
            subpoolplanmgr->SetFlagComb(SubPoolPlans::SPF_no_more_plan);
            *numofplan = 0;
            err->SetPLNErr(Error::PLNE_subpool_no_more_plan);
            ret = YFS_ENODATA;
        }
    }

    if(*numofplan > 0)
    {
        ret = YWB_SUCCESS;
        err->SetPLNErr(Error::COM_success);
    }

    ast_log_debug("Number of plans scheduled: " << *numofplan);
    return ret;
}

ywb_status_t
PlanManager::ScheduleNextFromSpecifiedPlanDirs(
            SubPoolKey& subpoolkey, ywb_uint32_t availplandirs)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t highestplandir = 0;
    ywb_uint32_t advicetype = 0;
    ywb_uint32_t iopsrequired = 0;
    ywb_uint32_t bwrequired = 0;
    ywb_uint32_t caprequired = 0;
    ywb_uint32_t heattype = 0;
    ywb_uint32_t tgttier = 0;
    DiskKey targetdiskkey;
    DiskKey sourcediskkey;
    OBJKey objkey;
    bool found = false;
    BlackList blacklist;
    SubPoolPlans* subpoolplanmgr = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Executor* exe = NULL;
    Error* err = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;
    PerfManager* perfmgr = NULL;
    ChunkIOStat* iostat = NULL;
    Plan* plan = NULL;
    DiskScheStat* sourcedisk = NULL;
    DiskScheStat* targetdisk = NULL;
#ifdef WITH_INITIAL_PLACEMENT
    ywb_bool_t swapdue = ywb_false;
    ywb_bool_t swaprequired = ywb_false;
    ywb_uint32_t tiercomb = SubPool::TC_cnt;
    SwapOutManager* swapmgr = NULL;
#endif

    ast_log_debug("Schedule next plan for subpool: ["
            << subpoolkey.GetOss()
            << ", " << subpoolkey.GetPoolId()
            << ", " << subpoolkey.GetSubPoolId()
            << "], from specified plan directions: "
            << availplandirs << "[PD_ctr_promote: 0, PD_ctr_demote: 1]");

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    ast->GetExecutor(&exe);
    ast->GetError(&err);
    err->SetPLNErr(Error::COM_success);
#ifdef WITH_INITIAL_PLACEMENT
    ast->GetSwapOutManager(&swapmgr);
#endif

    ret = GetSubPoolPlans(subpoolkey, &subpoolplanmgr);
//    YWB_ASSERT((YWB_SUCCESS == ret) && (subpoolplanmgr != NULL));
    if(YWB_SUCCESS != ret)
    {
        ast_log_info("No more plan generated for SubPoolPlans corresponding"
                " does not exist");
        err->SetPLNErr(Error::PLNE_subpool_no_more_plan);
        return ret;
    }

    ret = config->GetSubPool(subpoolkey, &subpool);
    if((YWB_SUCCESS == ret) &&
            (subpool->TestFlag(SubPool::SPF_no_migration)))
    {
        ast_log_info("No more plan generated for current scheduled SubPool"
                " is marked as SPF_no_migration");
        /*try again later, for SPF_no_migration may be cleared in next cycle*/
        ret = YFS_EAGAIN;
        err->SetPLNErr(Error::PLNE_subpool_schedule_timed_delay);
        config->PutSubPool(subpool);
        return ret;
    }
    else if(((YWB_SUCCESS == ret) &&
            (subpool->TestFlag(SubPool::SPF_removing))) ||
            (YWB_SUCCESS != ret))
    {
        ast_log_info("No more plan generated for SubPool may be deleted");
        ret = YFS_ENODATA;
        subpoolplanmgr->SetFlagComb(SubPoolPlans::SPF_subpool_deleted);
        err->SetPLNErr(Error::PLNE_subpool_no_more_plan);
        config->PutSubPool(subpool);
        return ret;
    }

#ifdef WITH_INITIAL_PLACEMENT
    swapdue = swapmgr->GetSwapRelocationDue(subpoolkey);
    swaprequired = swapmgr->GetSwapRequired(subpoolkey);
    tiercomb = subpool->GetCurTierComb();
    /*
     * if @swaprequired is set to be true, then check whether or not
     * swap is possible by checking whether or not there is target
     * disk for swap
     * */
    if(swapdue && (!swaprequired ||
            (tiercomb == SubPool::TC_ssd ||
            tiercomb == SubPool::TC_ent ||
            tiercomb == SubPool::TC_sata)))
    {
        if(swaprequired)
        {
            ast_log_info("No more plan generated for SubPool with swap only"
                    " relocation, but currently no target for swap");
        }
        else
        {
            ast_log_info("No more plan generated for SubPool with swap only"
                    " relocation, but currently no swap requirement");
        }

        /*
         * try again later, for swap requirement may comes later or
         * swap target may be added into logical configuration
         * */
        ret = YFS_EAGAIN;
        err->SetPLNErr(Error::PLNE_subpool_schedule_timed_delay);
        return ret;
    }
#endif

    config->PutSubPool(subpool);
    subpoolplanmgr->GetSubPoolPlanDirStat(&plandirstat);
    subpoolplanmgr->GetSubPoolAdvise(&subpooladvice);

#ifdef WITH_INITIAL_PLACEMENT
    if(swapdue)
    {
        /*exclude PD_ctr_promote and PD_ctr_demote from available plan directions*/
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_ctr_promote);
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_ctr_demote);
    }
    else
#endif
    {
        /*exclude PD_swap_up and PD_swap_down from available plan directions*/
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_swap_up);
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_swap_down);
    }

    while((availplandirs != 0) && (found == false) &&
            (Executor::S_stopping != exe->GetStatus()))
    {
        highestplandir = plandirstat->
                GetPriorPlanDirectionFromAvailable(availplandirs);
        if(Plan::PD_cnt == highestplandir)
        {
            break;
        }

        advicetype =
                plandirstat->GetAdviceTypeFromPlanDirection(highestplandir);
        if(AdviceType::AT_cnt == advicetype)
        {
            break;
        }

#ifdef WITH_INITIAL_PLACEMENT
        if(swaprequired && (AdviceType::AT_swap_up == advicetype))
        {
            ret = swapmgr->ScheduleNextSwapDisk(subpoolkey,
                    SwapOutManager::SD_up, sourcediskkey);
            if(YWB_SUCCESS != ret)
            {
                /*exclude Plan::PD_swap_up from local available plan directions*/
                availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                                availplandirs, highestplandir);
                /*
                 * SHOULD not exclude Plan::PD_swap_up from global plan directions
                 * for swap out requirement may change in next cycle
                 * */
                plandirstat->SetLastMissedPlanDir(highestplandir);
                continue;
            }

            ret = subpooladvice->GetFirstAdviceSatisfy(sourcediskkey,
                    advicetype, Advice::ADV_chosen, false, &advice);
        }
        else if(swaprequired && (AdviceType::AT_swap_down == advicetype))
        {
            ret = swapmgr->ScheduleNextSwapDisk(subpoolkey,
                    SwapOutManager::SD_down, sourcediskkey);
            if(YWB_SUCCESS != ret)
            {
                /*exclude Plan::PD_swap_up from local available plan directions*/
                availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                                availplandirs, highestplandir);
                /*SHOULD not exclude Plan::PD_swap_up from global plan directions*/
                plandirstat->SetLastMissedPlanDir(highestplandir);
                continue;
            }

            ret = subpooladvice->GetFirstAdviceSatisfy(sourcediskkey,
                    advicetype, Advice::ADV_chosen, false, &advice);
        }
        else
#endif
        /*
         * In current implementation, GetNextAdvice can only be called
         * after calling of GetFirstAdvice, but here we are not sure
         **/
        /*if(subpooladvice->IsFirstTimeToGetFrom(advicetype))
        {
            ret = subpooladvice->GetFirstAdvice(advicetype, &advice);
        }
        else
        {
            ret = subpooladvice->GetNextAdvice(advicetype, &advice);
        }*/

        {
            /*
             * Anyway, we should choose from the first to the end instead of
             * from the last chosen one to the end, for advice at the front
             * has higher priority than the one at the end. But here we will
             * identify those already been chosen advise, and skip them.
             **/
            ret = subpooladvice->GetFirstAdviceSatisfy(
                    advicetype, Advice::ADV_chosen, false, &advice);
        }

        if(!((YWB_SUCCESS == ret) && (advice != NULL)))
        {
            /*
             * can not find one plan of @highestplandir plan direction, then
             * assure there will be no more plan of this plan direction in
             * current schedule, free to exclude @highestplantype from local
             * available plan directions
             **/
            availplandirs = plandirstat->
                    ExcludeDirectionFromAvailableDirections(
                            availplandirs, highestplandir);

#ifdef WITH_INITIAL_PLACEMENT
            if(!swapdue)
#endif
            {
                /*
                 * also assure there will be no more plan of this plan direction
                 * in following schedule, feel free to exclude @highestplantype
                 * from global available plan directions
                 **/
                plandirstat->ExcludeDirectionFromAvailableDirections(
                        highestplandir);
            }

            /*penalty*/
            /*plandirstat->UpdatePlanDirPenalty(highestplandir, 1, true);*/
            plandirstat->SetLastMissedPlanDir(highestplandir);
        }
        else
        {
            do
            {
                /*
                 * will not check the OBJ's existence here, offload the check
                 * to migrator instead
                 **/
//            advice->GetOBJKey(&objkey);
//            if(config->CheckOBJExistence(*objkey))
//            {
                /*can not mark it as found until find a target for advice*/
                advice->GetStat(&iostat);
                YWB_ASSERT(iostat != NULL);
                iopsrequired = iostat->GetRndReadIOs() + iostat->GetRndWriteIOs() +
                        iostat->GetSeqReadIOs() + iostat->GetSeqWriteIOs();
                bwrequired = iostat->GetRndReadBW() + iostat->GetRndWriteBW() +
                        iostat->GetSeqReadBW() + iostat->GetSeqWriteBW();
                caprequired = advice->GetCap();
                tgttier = advice->GetTargetTier();
                heattype = advice->GetHeatType();

                /*get target disk for the got advice*/
                ret = perfmgr->GetFavoriteDisk(subpoolkey, tgttier, heattype,
                        iopsrequired, bwrequired, caprequired, &mUnavailDisks,
                        &mAvailDisks, &targetdiskkey);
                if(YWB_SUCCESS == ret)
                {
                    found = true;
                    /*update schedule times*/
                    plandirstat->UpdatePlanDirScheTimes(highestplandir, 1, true);
                    plandirstat->ResetLastMissedPlanDir();
                    advice->GetOBJKey(&objkey);
                    /*mark the advice as chosen*/
                    advice->SetFlagComb(Advice::ADV_chosen);
                }

                subpooladvice->PutAdvice(advice);
//            }
            } while((false == found) && (YFS_ENOENT != ret) &&
#ifdef WITH_INITIAL_PLACEMENT
                    (((swaprequired) ?
                    (YWB_SUCCESS == subpooladvice->GetNextAdviceSatisfy(
                    sourcediskkey, advicetype, Advice::ADV_chosen, false, &advice)) :
                    (YWB_SUCCESS == subpooladvice->GetNextAdviceSatisfy(
                    advicetype, Advice::ADV_chosen, false, &advice))) &&
#else
                    ((YWB_SUCCESS == subpooladvice->GetNextAdviceSatisfy(
                    advicetype, Advice::ADV_chosen, false, &advice)) &&
#endif
                    (advice != NULL)) &&
                    (Executor::S_stopping != exe->GetStatus()));

            if(found == false)
            {
                availplandirs =
                        plandirstat->ExcludeDirectionFromAvailableDirections(
                                availplandirs, highestplandir);
                /*penalty*/
                plandirstat->SetLastMissedPlanDir(highestplandir);
            }
        }
    }

    if(true == found)
    {
        /*deal with plan*/
        plan = new Plan(objkey, targetdiskkey.GetDiskId(),
                subpoolkey, *iostat, heattype,
                caprequired, highestplandir, exe->GetEpoch());
        if(NULL == plan)
        {
            ast_log_debug("Out of memory!");
            err->SetPLNErr(Error::COME_out_of_memory);
            return YFS_EOUTOFMEMORY;
        }

        /*deal with the chosen source disk*/
        sourcediskkey.SetDiskId(objkey.GetStorageId());
        sourcediskkey.SetSubPoolId(subpoolkey.GetSubPoolId());
        ret = GetDiskScheStat(sourcediskkey, &sourcedisk);
        if(!((YWB_SUCCESS == ret) && (sourcedisk != NULL)))
        {
            sourcedisk = new DiskScheStat();
            if(NULL == sourcedisk)
            {
                ast_log_debug("Out of memory!");
                delete plan;
                plan = NULL;
                err->SetPLNErr(Error::COME_out_of_memory);
                return YFS_EOUTOFMEMORY;
            }
            else
            {
                mDiskScheStat.insert(make_pair(sourcediskkey, sourcedisk));
            }
        }

        /*deal with the chosen target disk*/
        ret = GetDiskScheStat(targetdiskkey, &targetdisk);
        if(!((YWB_SUCCESS == ret) && (targetdisk != NULL)))
        {
            targetdisk = new DiskScheStat();
            if(NULL == targetdisk)
            {
                ast_log_debug("Out of memory!");
                delete plan;
                plan = NULL;
                err->SetPLNErr(Error::COME_out_of_memory);
                return YFS_EOUTOFMEMORY;
            }
            else
            {
                mDiskScheStat.insert(make_pair(targetdiskkey, targetdisk));
            }
        }

        sourcedisk->SetFlagUniq(DiskScheStat::DSSS_chosen);
        sourcedisk->UpdateDiskScheTimesByNum(1, true);
        targetdisk->SetFlagUniq(DiskScheStat::DSSS_chosen);
        targetdisk->UpdateDiskScheTimesByNum(1, true);
        RemoveAvailDisk(sourcediskkey);
        AddUnavailDisk(sourcediskkey);
        RemoveAvailDisk(targetdiskkey);
        AddUnavailDisk(targetdiskkey);

        /*add plan into subpool plan manager*/
        subpoolplanmgr->AddPlan(plan);
        ret = YWB_SUCCESS;
        ast_log_debug("Find one plan@" << plan << ": [obj: ["
                << objkey.GetStorageId()
                << ",  " << objkey.GetInodeId()
                << ",  " << objkey.GetChunkId()
                << ",  " << objkey.GetChunkIndex()
                << ",  " << objkey.GetChunkVersion()
                << "], sourcedisk: ["
                << sourcediskkey.GetSubPoolId()
                << ",  " << sourcediskkey.GetDiskId()
                << "], targetdisk: ["
                << targetdiskkey.GetSubPoolId()
                << ",  " << targetdiskkey.GetDiskId()
                << "], direction: " << highestplandir
                << "]");
    }
    else
    {
        /*
         * here we can not mark the subpool as no more plan, for here we
         * also take the disk availability into consideration(those chosen
         * /migrating), but later when these disks are available again
         * (migration complete), we may get plan again.
         **/
        ret = YFS_ENODATA;
        ast_log_debug("Not found plan!");
    }

    return ret;
}

ywb_status_t
PlanManager::ScheduleNext(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t availplandirs = 0;
    ywb_uint32_t highestplandir= 0;
    ywb_uint32_t advicetype = 0;
    ywb_uint32_t iopsrequired = 0;
    ywb_uint32_t bwrequired = 0;
    ywb_uint32_t caprequired = 0;
    ywb_uint32_t heattype = 0;
    ywb_uint32_t tgttier = 0;
    ywb_uint32_t retrytimes = 0;
    DiskKey targetdiskkey;
    DiskKey sourcediskkey;
    OBJKey objkey;
    bool found = false;
    SubPoolPlans* subpoolplanmgr = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Executor* exe = NULL;
    Error* err = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;
    PerfManager* perfmgr = NULL;
    ChunkIOStat* iostatp = NULL;
    ChunkIOStat iostat;
    Plan* plan = NULL;
    DiskScheStat* sourcedisk = NULL;
    DiskScheStat* targetdisk = NULL;
#ifdef WITH_INITIAL_PLACEMENT
    ywb_bool_t swapdue = ywb_false;
    ywb_bool_t swaprequired = ywb_false;
    ywb_uint32_t swapdisknum = 0;
    ywb_uint32_t scheduletimes = 0;
    ywb_uint32_t tiercomb = SubPool::TC_cnt;
    SwapOutManager* swapmgr = NULL;
#endif

    ast_log_debug("Schedule next plan for subpool: ["
            << "oss: " << subpoolkey.GetOss()
            << ", pool: " << subpoolkey.GetPoolId()
            << ", subpool: " << subpoolkey.GetSubPoolId()
            << "]");

    GetAST(&ast);
    ast->GetPerfManager(&perfmgr);
    ast->GetLogicalConfig(&config);
    ast->GetExecutor(&exe);
    ast->GetError(&err);
    err->SetPLNErr(Error::COM_success);
#ifdef WITH_INITIAL_PLACEMENT
    ast->GetSwapOutManager(&swapmgr);
#endif

    ret = GetSubPoolPlans(subpoolkey, &subpoolplanmgr);
//    YWB_ASSERT((YWB_SUCCESS == ret) && (subpoolplanmgr != NULL));
    if(YWB_SUCCESS != ret)
    {
        ast_log_info("No more plan generated for SubPoolPlans corresponding"
                " does not exist");
        err->SetPLNErr(Error::PLNE_subpool_no_more_plan);
        return ret;
    }

    ret = config->GetSubPool(subpoolkey, &subpool);
    if((YWB_SUCCESS == ret) &&
            (subpool->TestFlag(SubPool::SPF_no_migration)))
    {
        ast_log_info("No more plan generated for current scheduled SubPool"
                " is marked as SPF_no_migration");
        /*try again later, for SPF_no_migration may be cleared in next cycle*/
        ret = YFS_EAGAIN;
        err->SetPLNErr(Error::PLNE_subpool_schedule_timed_delay);
        config->PutSubPool(subpool);
        return ret;
    }
    else if(((YWB_SUCCESS == ret) &&
            (subpool->TestFlag(SubPool::SPF_removing))) ||
            (YWB_SUCCESS != ret))
    {
        ast_log_info("No more plan generated for SubPool is deleted/deleting");
        ret = YFS_ENODATA;
        subpoolplanmgr->SetFlagComb(SubPoolPlans::SPF_subpool_deleted);
        err->SetPLNErr(Error::PLNE_subpool_no_more_plan);
        config->PutSubPool(subpool);
        return ret;
    }

#ifdef WITH_INITIAL_PLACEMENT
    swapdue = swapmgr->GetSwapRelocationDue(subpoolkey);
    swaprequired = swapmgr->GetSwapRequired(subpoolkey);
    tiercomb = subpool->GetCurTierComb();
    /*
     * if @swaprequired is set to be true, then check whether or not
     * swap is possible by checking whether or not there is target
     * disk for swap
     * */
    if(swapdue && (!swaprequired ||
            (tiercomb == SubPool::TC_ssd ||
            tiercomb == SubPool::TC_ent ||
            tiercomb == SubPool::TC_sata)))
    {
        if(swaprequired)
        {
            ast_log_info("No more plan generated for SubPool with swap only"
                    " relocation, but currently no target for swap");
        }
        else
        {
            ast_log_info("No more plan generated for SubPool with swap only"
                    " relocation, but currently no swap requirement");
        }

        /*
         * try again later, for swap requirement may comes later or
         * swap target may be added into logical configuration
         * */
        ret = YFS_EAGAIN;
        err->SetPLNErr(Error::PLNE_subpool_schedule_timed_delay);
        return ret;
    }
#endif

    config->PutSubPool(subpool);
    subpoolplanmgr->GetSubPoolPlanDirStat(&plandirstat);
    subpoolplanmgr->GetSubPoolAdvise(&subpooladvice);
    availplandirs = plandirstat->GetCurrentAvailablePlanDirections();

#ifdef WITH_INITIAL_PLACEMENT
    if(swaprequired)
    {
        /*exclude PD_ctr_promote and PD_ctr_demote from available plan directions*/
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_ctr_promote);
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_ctr_demote);
    }
    else
#endif
    {
        /*exclude PD_swap_up and PD_swap_down from available plan directions*/
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_swap_up);
        availplandirs = plandirstat->ExcludeDirectionFromAvailableDirections(
                    availplandirs, Plan::PD_swap_down);
    }

#ifdef WITH_INITIAL_PLACEMENT
    /*
     * if @swaprequired is set to be true, then regardless of whether or not
     * @mAvailDisks is empty, for the swap required disks may change cycle
     * by cycle and new swap required disks may comes
     *
     * if @swaprequired is set to be false, then schedule next only if
     * @mAvailDisks is not empty
     * */
    if((swaprequired ? true : (!mAvailDisks.Empty())) &&
#else
    /*try to find plan targeting at any disk within available disk set*/
    if((!mAvailDisks.Empty()) &&
#endif
            (!subpoolplanmgr->TestFlag(SubPoolPlans::SPF_subpool_deleted)) &&
            (config->CheckSubPoolExistence(subpoolkey)))
    {
        while((availplandirs != 0) && (found == false) &&
                (Executor::S_stopping != exe->GetStatus()))
        {
            highestplandir = plandirstat->
                    GetPriorPlanDirectionFromAvailable(availplandirs);
            if(Plan::PD_cnt == highestplandir)
            {
                break;
            }

            advicetype = plandirstat->
                    GetAdviceTypeFromPlanDirection(highestplandir);
            if(AdviceType::AT_cnt == advicetype)
            {
                break;
            }

#ifdef WITH_INITIAL_PLACEMENT
            if(swaprequired && (AdviceType::AT_swap_up == advicetype))
            {
                swapdisknum = swapmgr->GetSwapRequiredDiskNum(subpoolkey,
                        SwapOutManager::SD_up, swapdisknum);

                scheduletimes = 0;
                /*find the first swap-required disk not in @mUnavailDisks*/
                do
                {
                    ret = swapmgr->ScheduleNextSwapDisk(subpoolkey,
                            SwapOutManager::SD_up, sourcediskkey);
                }while((YWB_SUCCESS == ret) &&
                        (mUnavailDisks.Find(sourcediskkey)) &&
                        (++scheduletimes < swapdisknum));

                if((YWB_SUCCESS != ret) || (scheduletimes >= swapdisknum))
                {
                    /*exclude Plan::PD_swap_up from local available plan directions*/
                    availplandirs =
                            plandirstat->ExcludeDirectionFromAvailableDirections(
                                    availplandirs, highestplandir);
                    /*
                     * SHOULD not exclude Plan::PD_swap_up from global plan directions
                     * for swap out requirement may change in next cycle
                     * */
                    plandirstat->SetLastMissedPlanDir(highestplandir);
                    continue;
                }

                ret = subpooladvice->GetFirstAdviceSatisfy(sourcediskkey,
                        advicetype, Advice::ADV_chosen, false, &advice);
            }
            else if(swaprequired && (AdviceType::AT_swap_down == advicetype))
            {
                swapdisknum = swapmgr->GetSwapRequiredDiskNum(subpoolkey,
                        SwapOutManager::SD_down, swapdisknum);

                scheduletimes = 0;
                /*find the first swap-required disk not in @mUnavailDisks*/
                do
                {
                    ret = swapmgr->ScheduleNextSwapDisk(subpoolkey,
                            SwapOutManager::SD_down, sourcediskkey);
                }while((YWB_SUCCESS == ret) &&
                        (mUnavailDisks.Find(sourcediskkey)) &&
                        (++scheduletimes < swapdisknum));

                if((YWB_SUCCESS != ret) || (scheduletimes >= swapdisknum))
                {
                    /*exclude Plan::PD_swap_up from local available plan directions*/
                    availplandirs =
                            plandirstat->ExcludeDirectionFromAvailableDirections(
                                    availplandirs, highestplandir);
                    /*SHOULD not exclude Plan::PD_swap_up from global plan directions*/
                    plandirstat->SetLastMissedPlanDir(highestplandir);
                    continue;
                }

                ret = subpooladvice->GetFirstAdviceSatisfy(sourcediskkey,
                        advicetype, Advice::ADV_chosen, false, &advice);
            }
            else
#endif
            /*
             * In current implementation, GetNextAdvice can only be called
             * after calling of GetFirstAdvice, but here we are not sure
             **/
            /*if(subpooladvice->IsFirstTimeToGetFrom(advicetype))
            {
                ret = subpooladvice->GetFirstAdvice(advicetype, &advice);
            }
            else
            {
                ret = subpooladvice->GetNextAdvice(advicetype, &advice);
            }*/

            {
                /*
                 * Anyway, we should choose from the first to the end instead of
                 * from the last chosen one to the end, for advice at the front
                 * has higher priority than the one at the end. But here we will
                 * identify those already been chosen advise, and skip them.
                 **/
                ret = subpooladvice->GetFirstAdviceSatisfy(
                        advicetype, Advice::ADV_chosen, false, &advice);
            }

            if(!((YWB_SUCCESS == ret) && (advice != NULL)))
            {
                /*
                 * can not find one plan of @highestplandir plan direction, then
                 * assure there will be no more plan of this plan direction in
                 * current schedule, free to exclude @highestplantype from local
                 * available plan directions
                 **/
                availplandirs = plandirstat->
                        ExcludeDirectionFromAvailableDirections(
                                availplandirs, highestplandir);

#ifdef WITH_INITIAL_PLACEMENT
                if(!swapdue)
#endif
                {
                    /*
                     * also assure there will be no more plan of this plan
                     * direction in following schedule, feel free to exclude
                     * @highestplantype from global available plan directions
                     **/
                    plandirstat->ExcludeDirectionFromAvailableDirections(
                            highestplandir);
                }

                /*penalty*/
                /*plandirstat->UpdatePlanDirPenalty(highestplandir, 1, true);*/
                plandirstat->SetLastMissedPlanDir(highestplandir);
            }
            else
            {
                retrytimes = 0;
                do{
                    retrytimes++;
                    ast_log_debug("Try for the " << retrytimes << " time");

                    advice->GetStat(&iostatp);
                    YWB_ASSERT(iostatp != NULL);
                    iopsrequired = iostatp->GetRndReadIOs() + iostatp->GetRndWriteIOs() +
                            iostatp->GetSeqReadIOs() + iostatp->GetSeqWriteIOs();
                    bwrequired = iostatp->GetRndReadBW() + iostatp->GetRndWriteBW() +
                            iostatp->GetSeqReadBW() + iostatp->GetSeqWriteBW();
                    caprequired = advice->GetCap();
                    tgttier = advice->GetTargetTier();
                    heattype = advice->GetHeatType();

                    /*get target disk for the got advice*/
                    ret = perfmgr->GetFavoriteDisk(subpoolkey, tgttier, heattype,
                            iopsrequired, bwrequired, caprequired, &mUnavailDisks,
                            &mAvailDisks, &targetdiskkey);
                    if(YWB_SUCCESS == ret)
                    {
                        found = true;
                        iostat = *iostatp;
                        /*update schedule times*/
                        plandirstat->UpdatePlanDirScheTimes(highestplandir, 1, true);
                        plandirstat->ResetLastMissedPlanDir();
                        advice->GetOBJKey(&objkey);
                        /*mark the advice as chosen*/
                        advice->SetFlagComb(Advice::ADV_chosen);
                    }

                    subpooladvice->PutAdvice(advice);
                    /*
                     * schedule next migration plan from given subpool, reschedule
                     * is necessary if one schedule fails and the subpool does exist
                     **/
                }while((false == found) && (YFS_ENOENT != ret) &&
                        (retrytimes <= Constant::RESCHEDULE_PLAN_TIMES) &&
                        (!(subpoolplanmgr->TestFlag(
                                SubPoolPlans::SPF_subpool_deleted) ||
                        !(config->CheckSubPoolExistence(subpoolkey)))) &&
#ifdef WITH_INITIAL_PLACEMENT
                        (((swaprequired) ? (YWB_SUCCESS ==
                        subpooladvice->GetNextAdviceSatisfy(sourcediskkey,
                        advicetype, Advice::ADV_chosen, false, &advice)) :
                        (YWB_SUCCESS == subpooladvice->GetNextAdviceSatisfy(
                        advicetype, Advice::ADV_chosen, false, &advice))) &&
#else
                        ((YWB_SUCCESS == subpooladvice->GetNextAdviceSatisfy(
                        advicetype, Advice::ADV_chosen, false, &advice)) &&
#endif
                        (advice != NULL)) &&
                        (Executor::S_stopping != exe->GetStatus()));

                if(found == false)
                {
                    availplandirs =
                            plandirstat->ExcludeDirectionFromAvailableDirections(
                                    availplandirs, highestplandir);
                    /*penalty*/
                    plandirstat->SetLastMissedPlanDir(highestplandir);
                }
            }
        }

        if(true == found)
        {
            /*deal with plan*/
            plan = new Plan(objkey, targetdiskkey.GetDiskId(),
                    subpoolkey, iostat, heattype,
                    caprequired, highestplandir, exe->GetEpoch());
            if(NULL == plan)
            {
                ast_log_debug("Out of memory!");
                err->SetPLNErr(Error::COME_out_of_memory);
                return YFS_EOUTOFMEMORY;
            }

            /*deal with the chosen source disk*/
            sourcediskkey.SetDiskId(objkey.GetStorageId());
            sourcediskkey.SetSubPoolId(subpoolkey.GetSubPoolId());
            ret = GetDiskScheStat(sourcediskkey, &sourcedisk);
            if(!((YWB_SUCCESS == ret) && (sourcedisk != NULL)))
            {
                sourcedisk = new DiskScheStat();
                if(NULL == sourcedisk)
                {
                    ast_log_debug("Out of memory!");
                    delete plan;
                    plan = NULL;
                    err->SetPLNErr(Error::COME_out_of_memory);
                    return YFS_EOUTOFMEMORY;
                }
                else
                {
                    mDiskScheStat.insert(make_pair(sourcediskkey, sourcedisk));
                }
            }

            /*deal with the chosen target disk*/
            ret = GetDiskScheStat(targetdiskkey, &targetdisk);
            if(!((YWB_SUCCESS == ret) && (targetdisk != NULL)))
            {
                targetdisk = new DiskScheStat();
                if(NULL == targetdisk)
                {
                    ast_log_debug("Out of memory!");
                    delete plan;
                    plan = NULL;
                    err->SetPLNErr(Error::COME_out_of_memory);
                    return YFS_EOUTOFMEMORY;
                }
                else
                {
                    mDiskScheStat.insert(make_pair(targetdiskkey, targetdisk));
                }
            }

            sourcedisk->SetFlagUniq(DiskScheStat::DSSS_chosen);
            sourcedisk->UpdateDiskScheTimesByNum(1, true);
            targetdisk->SetFlagUniq(DiskScheStat::DSSS_chosen);
            targetdisk->UpdateDiskScheTimesByNum(1, true);
            RemoveAvailDisk(sourcediskkey);
            AddUnavailDisk(sourcediskkey);
            RemoveAvailDisk(targetdiskkey);
            AddUnavailDisk(targetdiskkey);

            /*add plan into subpool plan manager*/
            subpoolplanmgr->AddPlan(plan);
            ret = YWB_SUCCESS;
            ast_log_debug("Find one plan@" << plan << ": [obj: ["
                    << objkey.GetStorageId()
                    << ",  " << objkey.GetInodeId()
                    << ",  " << objkey.GetChunkId()
                    << ",  " << objkey.GetChunkIndex()
                    << ",  " << objkey.GetChunkVersion()
                    << "], sourcedisk: ["
                    << sourcediskkey.GetSubPoolId()
                    << ",  " << sourcediskkey.GetDiskId()
                    << "], targetdisk: ["
                    << targetdiskkey.GetSubPoolId()
                    << ",  " << targetdiskkey.GetDiskId()
                    << "], direction: " << highestplandir
                    << "]");
        }
        else
        {
            /*
             * here can not mark the subpool as no more plan, for AST take the
             * disk's availability into consideration, currently unavailable
             * disks(those chosen/migrating disks) may become available again
             * (migration complete), and can get plan again.
             **/
            ret = YFS_ENODATA;
            ast_log_debug("Not found!");
        }
    }
    else
    {
        /*try our best*/
        /*ret = ScheduleNextFromSpecifiedPlanDirs(subpoolkey, availplandirs);*/
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::ScheduleFirstBatchTimedDelay(SubPoolKey& subpoolkey, ywb_time_t interval)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeOut timeout(ywb_time_now(), interval);

    mTimedDelayedSubPoolsForFirstBatch.insert(make_pair(timeout, subpoolkey));

    return ret;
}

/*
 * FIXME:
 * any more sophisticated algorithm???
 * */
ywb_status_t
PlanManager::ScheduleNextTimedDelay(SubPoolKey& subpoolkey, ywb_time_t interval)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeOut timeout(ywb_time_now(), interval);

    mTimedDelayedSubPoolsForNext.insert(make_pair(timeout, subpoolkey));

    return ret;
}

/*
 * FIXME:
 * any more sophisticated algorithm???
 * */
ywb_status_t
PlanManager::ScheduleFirstBatchDelay(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    mDelayedSubPoolsForFirstBatch.push_back(subpoolkey);

    return ret;
}

/*
 * FIXME:
 * any more sophisticated algorithm???
 * */
ywb_status_t
PlanManager::ScheduleNextDelay(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    mDelayedSubPoolsForNext.push_back(subpoolkey);

    return ret;
}

ywb_status_t
PlanManager::ScheduleFirstBatchForTimedDelayed(
        SubPoolKey* scheduledsubpool, ywb_uint32_t* numofplan)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t trytimes = 0;
    ywb_uint32_t maxtrytimes = 0;
    SubPoolKey subpoolkey;
    TimeOut timeout;
    map<TimeOut, SubPoolKey, TimeOutCmp>::iterator iter;
    AST* ast = NULL;
    Executor* exe = NULL;
    Error* err = NULL;

    *numofplan = 0;
    if(!(mTimedDelayedSubPoolsForFirstBatch.empty()))
    {
        maxtrytimes = (mTimedDelayedSubPoolsForFirstBatch.size() <
                Constant::RESCHEDULE_PLAN_TIMES) ?
                mTimedDelayedSubPoolsForFirstBatch.size() :
                Constant::RESCHEDULE_PLAN_TIMES;
        GetAST(&ast);
        ast->GetExecutor(&exe);
        ast->GetError(&err);
        err->SetPLNErr(Error::COM_success);

        iter = mTimedDelayedSubPoolsForFirstBatch.begin();
        YWB_ASSERT(mTimedDelayedSubPoolsForFirstBatch.end() != iter);
        timeout = iter->first;
        subpoolkey = iter->second;
        if(!(timeout.IsTimedOut()))
        {
            return YWB_SUCCESS;
        }

        ret = ScheduleFirstBatch(subpoolkey, numofplan);
        trytimes++;
        while((YWB_SUCCESS != ret) &&
                (trytimes < maxtrytimes) &&
                (Executor::S_stopping != exe->GetStatus()))
        {
            mTimedDelayedSubPoolsForFirstBatch.erase(iter++);
            if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
            {
                timeout.SetStart(ywb_time_now());
                /*re-add into timed-delayed subpools*/
                mTimedDelayedSubPoolsForFirstBatch.insert(
                        make_pair(timeout, subpoolkey));
            }

            trytimes++;
            timeout = iter->first;
            subpoolkey = iter->second;
            if(!(timeout.IsTimedOut()))
            {
                return ret;
            }

            ret = ScheduleFirstBatch(subpoolkey, numofplan);
        }

        if(YWB_SUCCESS == ret)
        {
            *scheduledsubpool = subpoolkey;
            mTimedDelayedSubPoolsForFirstBatch.erase(iter++);
        }
        else
        {
            timeout = iter->first;
            mTimedDelayedSubPoolsForFirstBatch.erase(iter++);
            if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
            {
                timeout.SetStart(ywb_time_now());
                /*re-add into timed-delayed subpools*/
                mTimedDelayedSubPoolsForFirstBatch.insert(
                        make_pair(timeout, subpoolkey));
            }
        }
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::ScheduleNextForTimedDelayed(SubPoolKey* scheduledsubpool)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t trytimes = 0;
    ywb_uint32_t maxtrytimes = 0;
    SubPoolKey subpoolkey;
    TimeOut timeout;
    map<TimeOut, SubPoolKey, TimeOutCmp>::iterator iter;
    AST* ast = NULL;
    Executor* exe = NULL;
    Error* err = NULL;

    if(!(mTimedDelayedSubPoolsForNext.empty()))
    {
        maxtrytimes = (mTimedDelayedSubPoolsForNext.size() <
                Constant::RESCHEDULE_PLAN_TIMES) ?
                        mTimedDelayedSubPoolsForNext.size() :
                Constant::RESCHEDULE_PLAN_TIMES;
        GetAST(&ast);
        ast->GetExecutor(&exe);
        ast->GetError(&err);
        err->SetPLNErr(Error::COM_success);

        iter = mTimedDelayedSubPoolsForNext.begin();
        YWB_ASSERT(mTimedDelayedSubPoolsForNext.end() != iter);
        timeout = iter->first;
        subpoolkey = iter->second;
        if(!(timeout.IsTimedOut()))
        {
            return YWB_SUCCESS;
        }

        ret = ScheduleNext(subpoolkey);
        trytimes++;
        while((YWB_SUCCESS != ret) &&
                (trytimes < maxtrytimes) &&
                (Executor::S_stopping != exe->GetStatus()))
        {
            mTimedDelayedSubPoolsForNext.erase(iter++);
            if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
            {
                timeout.SetStart(ywb_time_now());
                /*re-add into timed-delayed subpools*/
                mTimedDelayedSubPoolsForNext.insert(
                        make_pair(timeout, subpoolkey));
            }

            trytimes++;
            timeout = iter->first;
            subpoolkey = iter->second;
            if(!(timeout.IsTimedOut()))
            {
                return ret;
            }

            ret = ScheduleNext(subpoolkey);
        }

        if(YWB_SUCCESS == ret)
        {
            *scheduledsubpool = subpoolkey;
            mTimedDelayedSubPoolsForNext.erase(iter++);
        }
        else
        {
            timeout = iter->first;
            mTimedDelayedSubPoolsForNext.erase(iter++);
            if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
            {
                timeout.SetStart(ywb_time_now());
                /*re-add into timed-delayed subpools*/
                mTimedDelayedSubPoolsForNext.insert(
                        make_pair(timeout, subpoolkey));
            }
        }
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::ScheduleFirstBatchForDelayed(
        SubPoolKey* scheduledsubpool, ywb_uint32_t* numofplan)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t trytimes = 0;
    ywb_uint32_t maxtrytimes = 0;
    SubPoolKey subpoolkey;
    AST* ast = NULL;
    Executor* exe = NULL;
    Error* err = NULL;

    *numofplan = 0;
    if(!(mDelayedSubPoolsForFirstBatch.empty()))
    {
        maxtrytimes = (mDelayedSubPoolsForFirstBatch.size() < Constant::RESCHEDULE_PLAN_TIMES) ?
                mDelayedSubPoolsForFirstBatch.size() : Constant::RESCHEDULE_PLAN_TIMES;
        GetAST(&ast);
        ast->GetExecutor(&exe);
        ast->GetError(&err);
        err->SetPLNErr(Error::COM_success);

        subpoolkey = mDelayedSubPoolsForFirstBatch.front();
        ret = ScheduleFirstBatch(subpoolkey, numofplan);
        trytimes++;
        while((YWB_SUCCESS != ret) &&
                (trytimes < maxtrytimes) &&
                (Executor::S_stopping != exe->GetStatus()))
        {
            mDelayedSubPoolsForFirstBatch.pop_front();
            if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
            {
                mDelayedSubPoolsForFirstBatch.push_back(subpoolkey);
            }

            trytimes++;
            subpoolkey = mDelayedSubPoolsForFirstBatch.front();
            ret = ScheduleFirstBatch(subpoolkey, numofplan);
        }

        if(YWB_SUCCESS == ret)
        {
            *scheduledsubpool = subpoolkey;
            mDelayedSubPoolsForFirstBatch.pop_front();
        }
        else
        {
            mDelayedSubPoolsForFirstBatch.pop_front();
            if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
            {
                mDelayedSubPoolsForFirstBatch.push_back(subpoolkey);
            }
        }
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::ScheduleNextForDelayed(SubPoolKey* scheduledsubpool)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t trytimes = 0;
    ywb_uint32_t maxtrytimes = 0;
    SubPoolKey subpoolkey;
    AST* ast = NULL;
    Executor* exe = NULL;
    Error* err = NULL;

    if(!(mDelayedSubPoolsForNext.empty()))
    {
        maxtrytimes = (mDelayedSubPoolsForNext.size() < Constant::RESCHEDULE_PLAN_TIMES) ?
                mDelayedSubPoolsForNext.size() : Constant::RESCHEDULE_PLAN_TIMES;
        GetAST(&ast);
        ast->GetExecutor(&exe);
        ast->GetError(&err);
        err->SetPLNErr(Error::COM_success);

        subpoolkey = mDelayedSubPoolsForNext.front();
        ret = ScheduleNext(subpoolkey);
        trytimes++;
        while((YWB_SUCCESS != ret) &&
                (trytimes < maxtrytimes) &&
                (Executor::S_stopping != exe->GetStatus()))
        {
            mDelayedSubPoolsForNext.pop_front();
            if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
            {
                mDelayedSubPoolsForNext.push_back(subpoolkey);
            }

            trytimes++;
            subpoolkey = mDelayedSubPoolsForNext.front();
            ret = ScheduleNext(subpoolkey);
        }

        if(YWB_SUCCESS == ret)
        {
            *scheduledsubpool = subpoolkey;
            mDelayedSubPoolsForNext.pop_front();
        }
        else
        {
            mDelayedSubPoolsForNext.pop_front();
            if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
            {
                mDelayedSubPoolsForNext.push_back(subpoolkey);
            }
        }
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::ScheduleDelayed()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannun = 0;
    ywb_bool_t success = false;
    SubPoolKey scheduledsubpool;

    ast_log_debug("PlanManager schedule delayed");
    ret = ScheduleFirstBatchForDelayed(&scheduledsubpool, &plannun);
    if(YWB_SUCCESS == ret)
    {
        ast_log_debug("Successfully schedule first batch for delayed on"
                << "subpool[" << scheduledsubpool.GetOss()
                << ", " << scheduledsubpool.GetPoolId()
                << ", " << scheduledsubpool.GetSubPoolId() << "]"
                << ", totally " << plannun << " plans are scheduled");
        success = true;
        SubmitFirstBatch(scheduledsubpool);
    }

    ret = ScheduleNextForDelayed(&scheduledsubpool);
    if(YWB_SUCCESS == ret)
    {
        ast_log_debug("Successfully schedule next for delayed on"
                << "subpool[" << scheduledsubpool.GetOss()
                << ", " << scheduledsubpool.GetPoolId()
                << ", " << scheduledsubpool.GetSubPoolId() << "]");
        success = true;
        SubmitNext(scheduledsubpool);
    }

    ret = ScheduleFirstBatchForTimedDelayed(&scheduledsubpool, &plannun);
    if(YWB_SUCCESS == ret)
    {
        ast_log_debug("Successfully schedule first batch for timed delayed on"
                << "subpool[" << scheduledsubpool.GetOss()
                << ", " << scheduledsubpool.GetPoolId()
                << ", " << scheduledsubpool.GetSubPoolId() << "]"
                << ", totally " << plannun << " plans are scheduled");
        success = true;
        SubmitFirstBatch(scheduledsubpool);
    }

    ret = ScheduleNextForTimedDelayed(&scheduledsubpool);
    if(YWB_SUCCESS == ret)
    {
        ast_log_debug("Successfully schedule next for timed delayed on"
                << " subpool[" << scheduledsubpool.GetOss()
                << ", " << scheduledsubpool.GetPoolId()
                << ", " << scheduledsubpool.GetSubPoolId() << "]");
        success = true;
        SubmitNext(scheduledsubpool);
    }

    if(false == success)
    {
        ret = YFS_ENODATA;
    }
    else
    {
        ret = YWB_SUCCESS;
    }

    return ret;
}

void
PlanManager::RemoveSubPoolFromDelayed(SubPoolKey& subpoolkey)
{
    list<SubPoolKey>::iterator iter1;
    list<SubPoolKey>::iterator iter2;
    map<TimeOut, SubPoolKey, TimeOutCmp>::iterator iter3;
    map<TimeOut, SubPoolKey, TimeOutCmp>::iterator iter4;

    iter1 = mDelayedSubPoolsForFirstBatch.begin();
    while(mDelayedSubPoolsForFirstBatch.end() != iter1)
    {
        if((iter1->GetOss() == subpoolkey.GetOss()) &&
            (iter1->GetPoolId() == subpoolkey.GetPoolId()) &&
            (iter1->GetSubPoolId() == subpoolkey.GetSubPoolId()))
        {
            mDelayedSubPoolsForFirstBatch.erase(iter1++);
        }
        else
        {
            iter1++;
        }
    }

    iter2 = mDelayedSubPoolsForNext.begin();
    while(mDelayedSubPoolsForNext.end() != iter2)
    {
        if((iter2->GetOss() == subpoolkey.GetOss()) &&
            (iter2->GetPoolId() == subpoolkey.GetPoolId()) &&
            (iter2->GetSubPoolId() == subpoolkey.GetSubPoolId()))
        {
            mDelayedSubPoolsForNext.erase(iter2++);
        }
        else
        {
            iter2++;
        }
    }

    iter3 = mTimedDelayedSubPoolsForFirstBatch.begin();
    while(mTimedDelayedSubPoolsForFirstBatch.end() != iter3)
    {
        if(((iter3->second).GetOss() == subpoolkey.GetOss()) &&
            ((iter3->second).GetPoolId() == subpoolkey.GetPoolId()) &&
            ((iter3->second).GetSubPoolId() == subpoolkey.GetSubPoolId()))
        {
            mTimedDelayedSubPoolsForFirstBatch.erase(iter3++);
        }
        else
        {
            iter3++;
        }
    }

    iter4 = mTimedDelayedSubPoolsForNext.begin();
    while(mTimedDelayedSubPoolsForNext.end() != iter4)
    {
        if(((iter4->second).GetOss() == subpoolkey.GetOss()) &&
            ((iter4->second).GetPoolId() == subpoolkey.GetPoolId()) &&
            ((iter4->second).GetSubPoolId() == subpoolkey.GetSubPoolId()))
        {
            mTimedDelayedSubPoolsForNext.erase(iter4++);
        }
        else
        {
            iter4++;
        }
    }
}

ywb_bool_t
PlanManager::CheckInTimedDelayedSubPoolsForFirstBatch(SubPoolKey& key)
{
    map<TimeOut, SubPoolKey, TimeOutCmp>::iterator iter;
    SubPoolKey iterkey;

    iter = mTimedDelayedSubPoolsForFirstBatch.begin();
    while(mTimedDelayedSubPoolsForFirstBatch.end() != iter)
    {
        iterkey = iter->second;
        if((iterkey.GetOss() == key.GetOss()) &&
                (iterkey.GetPoolId() == key.GetPoolId()) &&
                (iterkey.GetSubPoolId() == key.GetSubPoolId()))
        {
            return true;
        }
        else
        {
            iter++;
        }
    }

    return false;
}

ywb_bool_t
PlanManager::CheckInTimedDelayedSubPoolsForNext(SubPoolKey& key)
{
    map<TimeOut, SubPoolKey, TimeOutCmp>::iterator iter;
    SubPoolKey iterkey;

    iter = mTimedDelayedSubPoolsForNext.begin();
    while(mTimedDelayedSubPoolsForNext.end() != iter)
    {
        iterkey = iter->second;
        if((iterkey.GetOss() == key.GetOss()) &&
                (iterkey.GetPoolId() == key.GetPoolId()) &&
                (iterkey.GetSubPoolId() == key.GetSubPoolId()))
        {
            return true;
        }
        else
        {
            iter++;
        }
    }

    return false;
}

ywb_bool_t
PlanManager::CheckInDelayedSubPoolsForFirstBatch(SubPoolKey& key)
{
    list<SubPoolKey>::iterator iter;

    iter = mDelayedSubPoolsForFirstBatch.begin();
    while(mDelayedSubPoolsForFirstBatch.end() != iter)
    {
        if((iter->GetOss() == key.GetOss()) &&
                (iter->GetPoolId() == key.GetPoolId()) &&
                (iter->GetSubPoolId() == key.GetSubPoolId()))
        {
            return true;
        }
        else
        {
            iter++;
        }
    }

    return false;
}

ywb_bool_t
PlanManager::CheckInDelayedSubPoolsForNext(SubPoolKey& key)
{
    list<SubPoolKey>::iterator iter;

    iter = mDelayedSubPoolsForNext.begin();
    while(mDelayedSubPoolsForNext.end() != iter)
    {
        if((iter->GetOss() == key.GetOss()) &&
                (iter->GetPoolId() == key.GetPoolId()) &&
                (iter->GetSubPoolId() == key.GetSubPoolId()))
        {
            return true;
        }
        else
        {
            iter++;
        }
    }

    return false;
}

void
PlanManager::OnFailToSubmit(SubPoolPlans* subpoolplans, Plan* plan)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Error* err = NULL;
    LogicalConfig* config = NULL;
    OBJKey* objkey = NULL;
    ywb_uint64_t targetdiskid = 0;
    DiskKey sourcekey;
    DiskKey targetkey;
    SubPoolKey* subpoolkey = NULL;
    DiskScheStat* diskstat = NULL;

    YWB_ASSERT(subpoolplans != NULL);
    YWB_ASSERT(plan != NULL);
    GetAST(&ast);
    ast->GetError(&err);
    ast->GetLogicalConfig(&config);
    plan->GetOBJKey(&objkey);
    plan->GetSubPoolKey(&subpoolkey);
    targetdiskid = plan->GetTargetId();

    sourcekey.SetDiskId(objkey->GetStorageId());
    sourcekey.SetSubPoolId(subpoolkey->GetSubPoolId());
    targetkey.SetDiskId(targetdiskid);
    targetkey.SetSubPoolId(subpoolkey->GetSubPoolId());

    if(err->GetEXEErr() == Error::LCFE_subpool_not_exist)
    {
        /*
         * REMEMBER not to delete subpoolplans here, mark the subpool as
         * deleted will prevent new plan from generating, so no need to
         * set corresponding disks as unavailable
         **/
        subpoolplans->SetFlagComb(SubPoolPlans::SPF_subpool_deleted);
        ast_log_debug("Fail to submit plan for subpool already deleted");
    }
    else if(err->GetEXEErr() == Error::LCFE_source_disk_not_exist)
    {
        /*mark source disk as deleted*/
        ret = GetDiskScheStat(sourcekey, &diskstat);
        if((YWB_SUCCESS == ret) && (diskstat != NULL))
        {
            diskstat->SetFlagUniq(DiskScheStat::DSSS_deleted);
        }

        /*
         * now that source disk does not exist, it will not be available
         * anymore. It is moved from available set into unavailable set
         * when it is chosen, here we will not return it to the available
         * set even not remove it from unavailable set, by doing this,
         * the next invocation of ScheduleNext() will exclude this non-
         * existent disk regardless the available set is empty or not.
         *
         * DO NOT call RemoveUnavailDisk(sourcekey) here, otherwise if
         * available set is empty at next invocation of ScheduleNext(),
         * this non-existent disk will not be excluded from choosing.
         **/
        RemoveUnavailDisk(targetkey);
        AddAvailDisk(targetkey);
        ast_log_debug("Fail to submit plan for source disk does not exist");
    }
    else if(err->GetEXEErr() == Error::LCFE_target_disk_not_exist)
    {
        /*mark target disk as deleted*/
        ret = GetDiskScheStat(targetkey, &diskstat);
        if((YWB_SUCCESS == ret) && (diskstat != NULL))
        {
            diskstat->SetFlagUniq(DiskScheStat::DSSS_deleted);
        }

        /*ditto*/
        RemoveUnavailDisk(sourcekey);
        AddAvailDisk(sourcekey);
        ast_log_debug("Fail to submit plan for target disk does not exist");
    }

//    /*if subpool still exist, re-mark source and target disk as available*/
//    if(err->GetEXEErr() != Error::LCF_ESUBPOOLNOTEXIST)
//    {
//        RemoveUnavailDisk(sourcekey);
//        AddAvailDisk(sourcekey);
//        RemoveUnavailDisk(targetkey);
//        AddAvailDisk(targetkey);
//    }

//    if(plan)
//    {
//        delete plan;
//        plan = NULL;
//    }
}

ywb_status_t
PlanManager::SubmitFirstBatch(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t submitted = 0;
    AST* ast = NULL;
    Arbitrator* arb = NULL;
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;

    ast_log_debug("Submit first batch plans for subpool: ["
            << subpoolkey.GetOss()
            << ", " << subpoolkey.GetPoolId()
            << ", " << subpoolkey.GetSubPoolId()
            << "]");
    ret = GetSubPoolPlans(subpoolkey, &subpoolplans);
    if((YWB_SUCCESS == ret) && (subpoolplans != NULL))
    {
        GetAST(&ast);
        ast->GetArb(&arb);

        ret = subpoolplans->GetFirstPlan(&plan);
        while((YWB_SUCCESS == ret) && (plan != NULL))
        {
            ret = arb->ReceivePlan(subpoolkey, plan);
            /*
             * fail to submit the plan(maybe configuration change,
             * or out of memory, or reservation failure and so on)
             **/
            if((ret != YWB_SUCCESS))
            {
                OnFailToSubmit(subpoolplans, plan);
            }
            else
            {
                submitted++;
            }

            /*for safety consideration*/
            plan = NULL;
            if(!subpoolplans->TestFlag(SubPoolPlans::SPF_subpool_deleted))
            {
                ret = subpoolplans->GetNextPlan(&plan);
            }
        }
    }
    else
    {
        ast_log_debug("NO plans corresponding to subpool!");
        return YFS_EINVAL;
    }

    if(0 != submitted)
    {
        ret = YWB_SUCCESS;
    }
    else
    {
        /*FIXME: what shall be the best errno???*/
        ret = YFS_ENODATA;
    }

    return ret;
}

ywb_status_t
PlanManager::SubmitNext(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Error* err = NULL;
    LogicalConfig* config = NULL;
    Arbitrator* arb = NULL;
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;
    DiskKey sourcekey;

    ast_log_debug("Submit next plan for subpool: ["
            << "oss: " << subpoolkey.GetOss()
            << ", pool: " << subpoolkey.GetPoolId()
            << ", subpool: " << subpoolkey.GetSubPoolId()
            << "]");

    ret = GetSubPoolPlans(subpoolkey, &subpoolplans);
    if((YWB_SUCCESS == ret) && (subpoolplans != NULL))
    {
        GetAST(&ast);
        ast->GetError(&err);
        ast->GetLogicalConfig(&config);
        ast->GetArb(&arb);

        ret = subpoolplans->GetNextPlan(&plan);
        if((YWB_SUCCESS == ret) && (plan != NULL))
        {
            ret = arb->ReceivePlan(subpoolkey, plan);
            /*
             * fail to submit the plan(maybe configuration change,
             * or out of memory, or reservation failure and so on)
             **/
            if((ret != YWB_SUCCESS))
            {
                OnFailToSubmit(subpoolplans, plan);
            }
        }
    }
    else
    {
        ast_log_debug("NO plans corresponding to subpool!");
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
PlanManager::Complete(SubPoolKey& subpoolkey, DiskKey& sourcekey,
        DiskKey& targetkey, ywb_bool_t triggermore, ywb_status_t migstatus)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    Error* err = NULL;
    DiskScheStat* diskschestat = NULL;
    SubPoolPlans* subpoolplans = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ast->GetError(&err);
    err->SetPLNErr(Error::COM_success);

    ast_log_debug("Plan manager complete migration");
    /*
     * if the subpool does not exist, remove it from plan manager
     * and stop generating plan for it
     *
     * NOTE: here can not rely on SubPoolPlans::SPF_subpool_deleted,
     * which is set passively
     **/
    ret = GetSubPoolPlans(subpoolkey, &subpoolplans);
    if((YWB_SUCCESS == ret) && (subpoolplans != NULL) &&
            !subpoolplans->TestFlag(SubPoolPlans::SPF_subpool_deleted) &&
            config->CheckSubPoolExistence(subpoolkey))
    {
        /*
         * FIXME:
         * @migstatus may give some hint on disk's status which can be leveraged
         * */

        RemoveUnavailDisk(sourcekey);
        AddAvailDisk(sourcekey);
        RemoveUnavailDisk(targetkey);
        AddAvailDisk(targetkey);

        ret = GetDiskScheStat(sourcekey, &diskschestat);
        if((YWB_SUCCESS == ret) && (diskschestat != NULL))
        {
            /*mark the source as DSSS_none*/
            diskschestat->SetFlagUniq(DiskScheStat::DSSS_none);
        }

        ret = GetDiskScheStat(targetkey, &diskschestat);
        if((YWB_SUCCESS == ret) && (diskschestat != NULL))
        {
            /*mark the target as DSSS_none*/
            diskschestat->SetFlagUniq(DiskScheStat::DSSS_none);
        }

        if((subpoolplans->TestFlag(SubPoolPlans::SPF_no_more_plan)) ||
                (!triggermore))
        {
            return YWB_SUCCESS;
        }

        /*try those delayed scheduled subpools at first*/
        ScheduleDelayed();

        ret = ScheduleNext(subpoolkey);
        if(YWB_SUCCESS == ret)
        {
            /*submit the scheduled migration plan*/
            ret = SubmitNext(subpoolkey);
        }
        else
        {
            /*try those delayed scheduled subpools at first*/
            ScheduleDelayed();

            if((!subpoolplans->TestFlag(SubPoolPlans::SPF_subpool_deleted) &&
                                (config->CheckSubPoolExistence(subpoolkey))))
            {
                if(Error::PLNE_subpool_schedule_timed_delay == err->GetPLNErr())
                {
                    ast_log_debug("Timed-delay schedule next");
                    ScheduleFirstBatchTimedDelay(subpoolkey,
                            ((Constant::CYCLE_PERIOD) * 1000000));
                }
                else if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
                {
                    /*
                     * FIXME:
                     * should only delay schedule next plan if at least one plan
                     * can be scheduled later. But currently, we assume there
                     * is enough advise there.
                     * */
                    ast_log_debug("Delay schedule next");
                    ScheduleNextDelay(subpoolkey);
                }
            }
        }

        ret = YWB_SUCCESS;
    }
    else if((YWB_SUCCESS == ret) && (subpoolplans != NULL))
    {
        ast_log_debug("SubPool already been deleted!");
        RemoveSubPoolPlans(subpoolkey);
        RemoveSubPoolFromDelayed(subpoolkey);
        ret = YFS_ENOENT;
        /*
         * Leave the disk schedule statistics there until reset
         * at next decision cycle, now that the subpool is deleted,
         * those contained disks will never be accessed again
         **/
//        RemoveDiskScheStat(sourcekey);
//        RemoveDiskScheStat(targetkey);

        /*try schedule other SubPools instead*/
        ScheduleDelayed();
    }

    return ret;
}

ywb_status_t
PlanManager::Expire(SubPoolKey& subpoolkey, DiskKey& sourcekey,
        DiskKey& targetkey, ywb_bool_t triggermore)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    DiskScheStat* diskschestat = NULL;
    SubPoolPlans* subpoolplans = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);

    ast_log_debug("Plan manager expire migration");
    /*
     * if the subpool does not exist, remove it from plan manager
     * and stop generating plan for it
     *
     * NOTE: here can not rely on SubPoolPlans::SPF_subpool_deleted,
     * which is set passively
     **/
    ret = GetSubPoolPlans(subpoolkey, &subpoolplans);
    if((YWB_SUCCESS == ret) && (subpoolplans != NULL) &&
            !subpoolplans->TestFlag(SubPoolPlans::SPF_subpool_deleted) &&
            config->CheckSubPoolExistence(subpoolkey))
    {
        RemoveUnavailDisk(sourcekey);
        AddAvailDisk(sourcekey);
        RemoveUnavailDisk(targetkey);
        AddAvailDisk(targetkey);

        ret = GetDiskScheStat(sourcekey, &diskschestat);
        if((YWB_SUCCESS == ret) && (diskschestat != NULL))
        {
            /*mark the source as DSSS_none*/
            diskschestat->SetFlagUniq(DiskScheStat::DSSS_none);
        }

        ret = GetDiskScheStat(targetkey, &diskschestat);
        if((YWB_SUCCESS == ret) && (diskschestat != NULL))
        {
            /*mark the target as DSSS_none*/
            diskschestat->SetFlagUniq(DiskScheStat::DSSS_none);
        }

        if((subpoolplans->TestFlag(SubPoolPlans::SPF_no_more_plan)) ||
                (!triggermore))
        {
            return YWB_SUCCESS;
        }

#if 0
        ret = ScheduleNext(subpoolkey);
        if(YWB_SUCCESS == ret)
        {
            /*submit the scheduled migration plan*/
            ret = SubmitNext(subpoolkey);
        }
        else if((!subpoolplans->TestFlag(SubPoolPlans::SPF_subpool_deleted) &&
                (config->CheckSubPoolExistence(subpoolkey))))
        {
            /*try those delayed scheduled subpools at first*/
            ScheduleDelayed();

            /*
             * FIXME:
             * should only delay schedule next plan if at least one plan
             * can be scheduled later. But currently, we assume there
             * is enough advise there.
             * */
            ast_log_debug("Delay shcedule next");
            ScheduleNextDelay(subpoolkey);
        }
#endif
        /*try schedule other SubPools at first*/
        ScheduleDelayed();

        /*
         * wait for a while before scheduling next plan against the same
         * disk(for expiration maybe caused by disk IO pressure)
         */
        ScheduleNextTimedDelay(subpoolkey,
                Constant::TIMED_DELAY_INTERVAL);

        ret = YWB_SUCCESS;
    }
    else if((YWB_SUCCESS == ret) && (subpoolplans != NULL))
    {
        ast_log_debug("SubPool already been deleted!");
        RemoveSubPoolPlans(subpoolkey);
        RemoveSubPoolFromDelayed(subpoolkey);
        ret = YFS_ENOENT;
        /*
         * Leave the disk schedule statistics there until reset
         * at next decision cycle, now that the subpool is deleted,
         * those contained disks will never be accessed again
         **/
//        RemoveDiskScheStat(sourcekey);
//        RemoveDiskScheStat(targetkey);

        /*try schedule other SubPools instead*/
        ScheduleDelayed();
    }

    return ret;
}

ywb_status_t
PlanManager::Destroy(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskKey, DiskScheStat*, DiskKeyCmp>::iterator iter;
    DiskScheStat* diskschestat = NULL;

    ast_log_debug("Plan manager destroy shcedule stat for disk: ["
            << "subpool: " << diskkey.GetSubPoolId()
            << ", disk: " << diskkey.GetDiskId()
            << "]");
    iter = mDiskScheStat.find(diskkey);
    if(iter != mDiskScheStat.end())
    {
        diskschestat = iter->second;
        mDiskScheStat.erase(iter);
        delete diskschestat;
        diskschestat = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::Destroy(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t subpoolexist = false;
    ywb_bool_t diskexist = false;
    map<SubPoolKey, SubPoolPlans*, SubPoolKeyCmp>::iterator iter;
    map<DiskKey, DiskScheStat*, DiskKeyCmp>::iterator iter2;
    DiskKey diskkey;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskschestat = NULL;

    ast_log_debug("Plan manager destroy plans "
            << "corresponding to subpool: ["
            << "oss: " << subpoolkey.GetOss()
            << "pool: " << subpoolkey.GetPoolId()
            << "subpool: " << subpoolkey.GetSubPoolId()
            << "]");
    iter = mPlans.find(subpoolkey);
    if(iter != mPlans.end())
    {
        subpoolexist = true;
        subpoolplans = iter->second;
        mPlans.erase(iter);
        delete subpoolplans;
        subpoolplans = NULL;
    }

    iter2 = mDiskScheStat.begin();
    while(iter2 != mDiskScheStat.end())
    {
        diskkey = iter2->first;
        if(diskkey.GetSubPoolId() == subpoolkey.GetSubPoolId())
        {
            diskexist = true;
            diskschestat = iter2->second;
            mDiskScheStat.erase(iter2++);
            delete diskschestat;
            diskschestat = NULL;
        }
        else
        {
            iter2++;
        }
    }

    if((false == subpoolexist) && (false == diskexist))
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PlanManager::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolPlans*, SubPoolKeyCmp>::iterator iter1;
    map<DiskKey, DiskScheStat*, DiskKeyCmp>::iterator iter2;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskschestat = NULL;

    ast_log_debug("Plan manager destroy");
    iter1 = mPlans.begin();
    for(; iter1 != mPlans.end(); )
    {
        subpoolplans = iter1->second;
        mPlans.erase(iter1++);
        delete subpoolplans;
        subpoolplans = NULL;
    }

    iter2 = mDiskScheStat.begin();
    for(; iter2 != mDiskScheStat.end(); )
    {
        diskschestat = iter2->second;
        mDiskScheStat.erase(iter2++);
        delete diskschestat;
        diskschestat = NULL;
    }

    mTimedDelayedSubPoolsForFirstBatch.clear();
    mTimedDelayedSubPoolsForNext.clear();
    mDelayedSubPoolsForFirstBatch.clear();
    mDelayedSubPoolsForNext.clear();
    mUnavailDisks.Destroy();
    mAvailDisks.Destroy();
    mAst = NULL;

    return ret;
}

ywb_status_t
PlanManager::Reset(ywb_bool_t all)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolPlans*, SubPoolKeyCmp>::iterator iter1;
    map<DiskKey, DiskScheStat*, DiskKeyCmp>::iterator iter2;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskschestat = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    DiskKey diskkey;

    GetAST(&ast);
    YWB_ASSERT(ast != NULL);
    ast->GetLogicalConfig(&config);
    YWB_ASSERT(config != NULL);

    ast_log_debug("PlanManager reset");
    /*all plans will be discarded*/
    iter1 = mPlans.begin();
    for(; iter1 != mPlans.end(); )
    {
        subpoolplans = iter1->second;
        mPlans.erase(iter1++);
        delete subpoolplans;
        subpoolplans = NULL;
    }

    iter2 = mDiskScheStat.begin();
    if(ywb_false == all)
    {
        for(; iter2 != mDiskScheStat.end(); )
        {
            diskkey = iter2->first;
            diskschestat = iter2->second;
            /*
             * to be deleted disk must satisfy:
             * (1) the disk does not exist;
             * (2) the disk is not chosen.
             **/
            if(((diskschestat->TestFlag(DiskScheStat::DSSS_none)) &&
                    (!config->CheckDiskExistence(diskkey))) ||
                    (diskschestat->TestFlag(DiskScheStat::DSSS_deleted)))
            {
                mDiskScheStat.erase(iter2++);
                delete diskschestat;
                diskschestat = NULL;
            }
            else
            {
                iter2++;
            }
        }
    }
    else
    {
        for(; iter2 != mDiskScheStat.end(); )
        {
            diskschestat = iter2->second;
            mDiskScheStat.erase(iter2++);
            delete diskschestat;
            diskschestat = NULL;
        }
    }

    mTimedDelayedSubPoolsForFirstBatch.clear();
    mTimedDelayedSubPoolsForNext.clear();
    mDelayedSubPoolsForFirstBatch.clear();
    mDelayedSubPoolsForNext.clear();

    if(ywb_true == all)
    {
        mUnavailDisks.Reset();
        mAvailDisks.Reset();
    }

    return ret;
}

#if 0
ywb_status_t
PlanManager::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    ywb_uint32_t tiertype = Tier::TIER_cnt;
    map<SubPoolKey, SubPoolPlans*, SubPoolKeyCmp>::iterator iter1;
    map<DiskKey, DiskScheStat*, DiskKeyCmp>::iterator iter2;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskschestat = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);

    ast_log_debug("Plan manager reset");
    /*1. delete those not existed subpools*/
    ast_log_debug("Delete those not existed subpools");
    iter1 = mPlans.begin();
    for(; iter1 != mPlans.end(); )
    {
        subpoolkey = iter1->first;
        subpoolplans = iter1->second;
        if(!config->CheckSubPoolExistence(subpoolkey))
        {
            mPlans.erase(iter1++);
            delete subpoolplans;
            subpoolplans = NULL;
        }
        else
        {
            iter1++;
        }
    }

    /*2. add newly reported subpools*/
    ast_log_debug("Add newly reported subpools");
    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        iter1 = mPlans.find(subpoolkey);
        if(iter1 == mPlans.end())
        {
            subpoolplans = new SubPoolPlans();
            if(subpoolplans == NULL)
            {
                ast_log_debug("Out of memory!");
                ret = YFS_EOUTOFMEMORY;
            }
            else
            {
                mPlans.insert(make_pair(subpoolkey, subpoolplans));
            }
        }

        config->PutSubPool(subpool);
        if(YFS_EOUTOFMEMORY == ret)
        {
            break;
        }

        ret = config->GetNextSubPool(&subpool, &subpoolkey);
    }

    /*3. delete those not existed disks*/
    ast_log_debug("Delete those not existed disks");
    iter2 = mDiskScheStat.begin();
    for(; iter2 != mDiskScheStat.end(); )
    {
        diskkey = iter2->first;
        diskschestat = iter2->second;
        /*
         * to be deleted disk must satisfy:
         * (1) the disk does not exist and disk is not chosen
         * (2) the disk is marked as deleted
         **/
        if(((DiskScheStat::DSSS_none == diskschestat->GetDiskShceStatus()) &&
                (!config->CheckDiskExistence(diskkey))) ||
                (DiskScheStat::DSSS_deleted == diskschestat->GetDiskShceStatus()))
        {
            /*update unavailable/available disks set*/
            mUnavailDisks.RemoveDisk(diskkey);
            mAvailDisks.RemoveDisk(diskkey);

            mDiskScheStat.erase(iter2++);
            delete diskschestat;
            diskschestat = NULL;
        }
        else
        {
            iter2++;
        }
    }

    /*4. add newly reported disks*/
    ast_log_debug("Add newly reported disks");
    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        tiertype = Tier::TIER_0;
        for(; tiertype < Tier::TIER_cnt; tiertype++)
        {
            ret = subpool->GetTier(tiertype, &tier);
            if((YWB_SUCCESS == ret) && (tier != NULL))
            {
                ret = tier->GetFirstDisk(&disk, &diskkey);
                while((YWB_SUCCESS == ret) && (disk != NULL))
                {
                    iter2 = mDiskScheStat.find(diskkey);
                    if(iter2 == mDiskScheStat.end())
                    {
                        diskschestat = new DiskScheStat();
                        if(NULL == diskschestat)
                        {
                            ast_log_debug("Out of memory!");
                            ret = YFS_EOUTOFMEMORY;
                        }
                        else
                        {
                            /*update unavailable/available disks set*/
                            mUnavailDisks.AddDisk(diskkey);
                            mAvailDisks.AddDisk(diskkey);

                            mDiskScheStat.insert(make_pair(diskkey, diskschestat));
                        }
                    }

                    config->PutDisk(disk);
                    if(YFS_EOUTOFMEMORY == ret)
                    {
                        break;
                    }

                    ret = tier->GetNextDisk(&disk, &diskkey);
                }

                subpool->PutTier(tier);
                if(YFS_EOUTOFMEMORY == ret)
                {
                    break;
                }
            }
        }

        config->PutSubPool(subpool);
        if(YFS_EOUTOFMEMORY == ret)
        {
            break;
        }

        ret = config->GetNextSubPool(&subpool, &subpoolkey);
    }

    return ret;
}
#endif

ywb_status_t
PlanManager::Init()
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfManager* perfmgr = NULL;
    AdviceManager* advicemgr = NULL;
    SubPoolPlans* subpoolplanmgr = NULL;
    SubPoolAdvice* subpooladvise = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    list<SubPoolKey> subpoolkeys;
    list<SubPool*> subpools;
    list<SubPoolKey>::iterator subpoolkeyiter;
    list<SubPool*>::iterator subpooliter;
    SubPool* subpool = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    ast->GetAdviseManager(&advicemgr);
    config->GetKeysAndSubPools(subpoolkeys, subpools);
    YWB_ASSERT(subpoolkeys.size() == subpools.size());

    subpoolkeyiter = subpoolkeys.begin();
    subpooliter = subpools.begin();
    while(subpoolkeyiter != subpoolkeys.end())
    {
        subpool = *subpooliter;
        ret = perfmgr->GetSubPoolDiskAvailPerfRanking(
                *subpoolkeyiter, &subpoolperf);
        /*only subpools with advise and performance data are considered*/
        if((!(subpool->TestFlag(SubPool::SPF_no_advice))) &&
                ((YWB_SUCCESS == ret) && (subpoolperf != NULL)))
        {
            ret = advicemgr->GetSubPoolAdvice(*subpoolkeyiter, &subpooladvise);
            if((YWB_SUCCESS == ret) && (subpooladvise != NULL))
            {
                subpoolplanmgr = new SubPoolPlans(subpooladvise);
                if(subpoolplanmgr == NULL)
                {
                    ast_log_debug("Out of memory!");
                    ret = YFS_EOUTOFMEMORY;
                }
                else
                {
                    mPlans.insert(make_pair(*subpoolkeyiter, subpoolplanmgr));
                }

                advicemgr->PutSubPoolAdvice(subpooladvise);
            }
        }

        config->PutSubPool(subpool);
        if(YFS_EOUTOFMEMORY == ret)
        {
            ast_log_debug("Out of memory!");
            break;
        }

        subpoolkeyiter++;
        subpooliter++;
    }

    mTimedDelayedSubPoolsForFirstBatch.clear();
    mTimedDelayedSubPoolsForNext.clear();
    mDelayedSubPoolsForFirstBatch.clear();
    mDelayedSubPoolsForNext.clear();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
