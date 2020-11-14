#include <algorithm>
#include "AST/ASTTime.hpp"
#include "AST/ASTPerformance.hpp"

bool
BlackList::Empty()
{
    return mDisks.empty();
}

ywb_status_t
BlackList::AddDisk(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    mDisks.insert(diskkey);
    return ret;
}

ywb_status_t
BlackList::RemoveDisk(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    set<DiskKey, DiskKeyCmp>::iterator iter;

    iter = mDisks.find(diskkey);
    if(iter != mDisks.end())
    {
        mDisks.erase(iter);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
BlackList::GetFirstDisk(DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    mIt = mDisks.begin();
    if(mIt != mDisks.end())
    {
        *diskkey = *mIt;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
BlackList::GetNextDisk(DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIt != mDisks.end())
    {
        *diskkey = *mIt;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
BlackList::Find(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    set<DiskKey, DiskKeyCmp>::iterator iter;

    iter = mDisks.find(diskkey);
    if(iter == mDisks.end())
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
BlackList::GetDifferenceSet(BlackList* listb)
{
    ywb_status_t ret = YWB_SUCCESS;
    set<DiskKey, DiskKeyCmp>::iterator iter;
    DiskKey diskkey;

    if((NULL == listb) || (listb->Empty()))
    {
        return YWB_SUCCESS;
    }

    mIt = mDisks.begin();
    while(mIt != mDisks.end())
    {
        diskkey = *mIt;
        iter = listb->mDisks.find(diskkey);
        if(iter != listb->mDisks.end())
        {
            mDisks.erase(mIt++);
        }
        else
        {
            mIt++;
        }
    }

    return ret;
}

ywb_status_t
BlackList::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;

    mDisks.clear();
    return ret;
}

ywb_status_t
BlackList::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;

    mDisks.clear();
    return ret;
}

ywb_status_t
DiskPerf::CalculateDiskPerf(ywb_uint32_t perftype)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskIOStat* iosstat = NULL;
    DiskIOStat* bwstat = NULL;
    DiskBaseProp* prop = NULL;

    ywb_uint32_t rndiosstat = 0;
    ywb_uint32_t seqiosstat = 0;
    ywb_uint32_t rndreadios = 0;
    ywb_uint32_t rndwriteios = 0;
    ywb_uint32_t seqreadios = 0;
    ywb_uint32_t seqwriteios = 0;
    ywb_uint32_t rndbwstat = 0;
    ywb_uint32_t seqbwstat = 0;

    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;

    mDiskIO->GetStatIOs(IOStat::Uint2Type(perftype), &iosstat);
    mDiskIO->GetStatBW(IOStat::Uint2Type(perftype), &bwstat);

    rndiosstat = iosstat->GetRndStat();
    seqiosstat = iosstat->GetSeqStat();
    rndreadios = iosstat->GetRndReadStat();
    rndwriteios = iosstat->GetRndWriteStat();
    seqreadios = iosstat->GetSeqReadStat();
    seqwriteios = iosstat->GetSeqWriteStat();
    rndbwstat = bwstat->GetRndStat();
    seqbwstat = bwstat->GetSeqStat();

    rndseqratio = ((rndiosstat + seqiosstat) == 0) ?
            0 : ((100 * rndiosstat) / (rndiosstat + seqiosstat));
    rndrwratio = ((rndreadios + rndwriteios) == 0) ?
            0 : ((100 * rndreadios) / (rndreadios + rndwriteios));
    rndiosize = (rndiosstat == 0) ?
            (Constant::DEFAULT_IOSIZE) : (rndbwstat / rndiosstat);
    seqrwratio = ((seqreadios + seqwriteios) == 0) ?
            0 : ((100 * seqreadios) / (seqreadios + seqwriteios));
    seqiosize = (seqiosstat == 0) ?
            (Constant::DEFAULT_IOSIZE) : (seqbwstat / seqiosstat);

    if(mDiskPerfProfile)
    {
        ret = mDiskPerfProfile->CalculateDiskPerf(rndseqratio, rndrwratio, rndiosize,
                seqrwratio, seqiosize, &mBWThreshold, &mIOThreshold);
    }

    if((NULL == mDiskPerfProfile) || (YWB_SUCCESS != ret))
    {
        /*
         * use hard coded value instead
         *
         * FIXME: find a more sophisticated algorithm with
         * rwratio and iosize considered
         **/
        switch(mDisk->GetTier())
        {
        case Tier::TIER_0:
            mBWThreshold = Constant::DEFAULT_SSD_BW;
            mIOThreshold = Constant::DEFAULT_SSD_IOPS;
            break;
        case Tier::TIER_1:
            mBWThreshold = Constant::DEFAULT_ENT_BW;
            mIOThreshold = Constant::DEFAULT_ENT_IOPS;
            break;
        case Tier::TIER_2:
            mBWThreshold = Constant::DEFAULT_SATA_BW;
            mIOThreshold = Constant::DEFAULT_SATA_IOPS;
            break;
        default:
            YWB_ASSERT(0);
            break;
        }
    }
    else if((YWB_SUCCESS == ret) && ((0 == mBWThreshold) || (0 == mIOThreshold)))
    {
        switch(mDisk->GetTier())
        {
        case Tier::TIER_0:
            if(0 == mBWThreshold)
            {
                mBWThreshold = Constant::DEFAULT_SSD_BW;
            }

            if(0 == mIOThreshold)
            {
                mIOThreshold = Constant::DEFAULT_SSD_IOPS;
            }

            break;
        case Tier::TIER_1:
            if(0 == mBWThreshold)
            {
                mBWThreshold = Constant::DEFAULT_ENT_BW;
            }

            if(0 == mIOThreshold)
            {
                mIOThreshold = Constant::DEFAULT_ENT_IOPS;
            }

            break;
        case Tier::TIER_2:
            if(0 == mBWThreshold)
            {
                mBWThreshold = Constant::DEFAULT_SATA_BW;
            }

            if(0 == mIOThreshold)
            {
                mIOThreshold = Constant::DEFAULT_SATA_IOPS;
            }

            break;
        default:
            YWB_ASSERT(0);
            break;
        }
    }

    /*firstly translate BW threshold from MB to KB*/
    mBWThreshold *= (1024);

    /*
     * then translate IO/BW threshold per second to total cycle threshold
     *
     * FIXME:
     * if @mCyclePeriod is more than one hour, and @mBWThreshold is GB level
     * , it is possible that the calculated @mBWThreshold will overflow
     * */
    mIOThreshold *= (mCyclePeriod);
    mBWThreshold *= (mCyclePeriod);

    mIOConsumed = rndiosstat + seqiosstat;
    mBWConsumed = rndbwstat + seqbwstat;
    mDisk->GetBaseProp(&prop);
    mCapTotal = prop->GetDiskCap();
    mCapConsumed = mDisk->GetOBJCnt();

    ast_log_debug("Calculated disk performance: ["
            << "rndseqratio: " << rndseqratio
            << ", rndrwratio: " << rndrwratio
            << ", rndiosize: " << rndiosize
            << ", seqrwratio: " << seqrwratio
            << ", seqiosize: " << seqiosize
            << ", BWThreshold: " << mBWThreshold
            << ", IOThreshold: " << mIOThreshold
            << ", CapTotal: " << mCapTotal
            << ", BWConsumed: " << mBWConsumed
            << ", IOConsumed: " << mIOConsumed
            << ", CapConsumed: " << mCapConsumed
            << "], performance type: "<< perftype
            << "[IOST_raw: 0, IOST_short_term_ema:"
            << " 1, IOST_long_term_ema: 2]");

    return ret;
}

void
DiskAvailPerfRankingUnit::CalculateDiskAvailPerf()
{
    ywb_uint32_t iothresh = 0;
    ywb_uint32_t ioconsumed = 0;
    ywb_uint32_t bwthresh = 0;
    ywb_uint32_t bwconsumed = 0;
    ywb_int32_t capthresh = 0;
    ywb_int32_t capconsumed = 0;

    iothresh = mDiskPerf->GetIOThreshold();
    ioconsumed = mDiskPerf->GetIOConsumed();
    bwthresh = mDiskPerf->GetBWThreshold();
    bwconsumed = mDiskPerf->GetBWConsumed();
    capthresh = mDiskPerf->GetCapTotal();
    capconsumed = mDiskPerf->GetCapConsumed();

    mIOAvail = (iothresh > (ioconsumed + mIOReserved)) ?
            (iothresh - (ioconsumed + mIOReserved)) : 0;
    mBWAvail = (bwthresh > (bwconsumed + mBWReserved)) ?
            (bwthresh - (bwconsumed + mBWReserved)) : 0;
    mCapAvail = (capthresh > (capconsumed + mCapReserved)) ?
            (capthresh - (capconsumed + mCapReserved)) : 0;

    ast_log_debug("Calculated disk available performance: ["
            << "IOAvail: " << mIOAvail
            << ", BWAvail: " << mBWAvail
            << ", CapAval: " << mCapAvail
            << "]");
}

ywb_status_t
DiskAvailPerfRankingUnit::Reserve(ywb_int32_t iosrequired,
    ywb_int32_t bwrequired, ywb_int32_t caprequired)
{
    ywb_status_t ret = YWB_SUCCESS;

    /*for demotion, both @iosrequired and @bwrequired can be 0*/
    YWB_ASSERT(iosrequired >= 0);
    YWB_ASSERT(bwrequired >= 0);
    if((mIOAvail > (ywb_uint32_t)iosrequired) &&
            (mBWAvail > (ywb_uint32_t)bwrequired) &&
            (mCapAvail > caprequired))
    {
        mIOAvail -= (ywb_uint32_t)iosrequired;
        mBWAvail -= (ywb_uint32_t)bwrequired;

        mIOReserved += (ywb_uint32_t)iosrequired;
        mBWReserved += (ywb_uint32_t)bwrequired;

        mCapAvail -= caprequired;
        mCapReserved += caprequired;
    }
    else
    {
        ret = YFS_ENOSPACE;
    }

    return ret;
}

ywb_status_t
DiskAvailPerfRankingUnit::UnReserve(ywb_int32_t iosrequired,
    ywb_int32_t bwrequired, ywb_int32_t caprequired)
{
    ywb_status_t ret = YWB_SUCCESS;

    YWB_ASSERT(iosrequired > 0);
    YWB_ASSERT(bwrequired > 0);

    mIOAvail += iosrequired;
    mBWAvail += bwrequired;
    mCapAvail += caprequired;

    mIOReserved -= iosrequired;
    mBWReserved -= bwrequired;
    mCapReserved -= caprequired;

    return ret;
}

ywb_status_t
DiskAvailPerfRankingUnit::Consume(ywb_int32_t iosrequired,
    ywb_int32_t bwrequired, ywb_int32_t caprequired)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t ioconsumed = 0;
    ywb_uint32_t bwconsumed = 0;
    ywb_int32_t capconsumed = 0;

    mIOReserved -= iosrequired;
    mBWReserved -= bwrequired;
    mCapReserved -= caprequired;

    ioconsumed = mDiskPerf->GetIOConsumed();
    bwconsumed = mDiskPerf->GetBWConsumed();
    capconsumed = mDiskPerf->GetCapConsumed();

    mDiskPerf->SetIOConsumed(ioconsumed + iosrequired);
    mDiskPerf->SetBWConsumed(bwconsumed + bwrequired);
    mDiskPerf->SetCapConsumed(capconsumed + caprequired);

    return ret;
}

ywb_status_t
TierDiskAvailPerfRanking::SetupDiskAvailPerfRankingUnit(DiskKey& key,
        ywb_uint32_t perftype)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    PerfManager* perfmgr = NULL;
    AST* ast = NULL;
    PerfProfileManager* profilemgr = NULL;
    DiskPerfProfile* profile = NULL;
    LogicalConfig* config = NULL;
    TimeManager* timemgr = NULL;
    CycleManager* cyclemgr = NULL;
    Disk* disk = NULL;
    DiskBaseProp* prop = NULL;
    DiskIO* diskio = NULL;
    DiskPerf* diskperf = NULL;
    DiskAvailPerfRankingUnit* diskunit = NULL;

    if(perftype < 0 || perftype >= IOStat::IOST_cnt)
    {
        return YFS_EINVAL;
    }

    subpoolperf = mSubPoolPerf;
    subpoolperf->GetPerfManager(&perfmgr);
    perfmgr->GetAST(&ast);
    ast->GetPerformanceProfile(&profilemgr);
    ast->GetLogicalConfig(&config);
    ast->GetTimeManager(&timemgr);
    timemgr->GetCycleManager(&cyclemgr);

    ret = config->GetDisk(key, &disk);
    if((YWB_SUCCESS != ret))
    {
        return ret;
    }

    disk->GetDiskIO(&diskio);
    disk->GetBaseProp(&prop);
    profilemgr->GetDiskPerfProfile(*prop, &profile);
    diskperf = new DiskPerf(cyclemgr->GetCyclePeriod(), profile, diskio, disk);
    config->PutDisk(disk);
    if(NULL == diskperf)
    {
        ret = YFS_EOUTOFMEMORY;
        return ret;
    }

    diskunit = new DiskAvailPerfRankingUnit(key, diskperf, this);
    if(NULL == diskunit)
    {
        delete diskperf;
        diskperf = NULL;

        ret = YFS_EOUTOFMEMORY;
        return ret;
    }

    ast_log_debug("Setup available performance ranking for disk: ["
            << key.GetSubPoolId() << ", " << key.GetDiskId()
            << "], perftype: " << perftype
            << "[IOST_raw: 0, IOST_short_term_ema:"
            << " 1, IOST_long_term_ema: 2]");
    diskperf->CalculateDiskPerf(perftype);
    diskunit->CalculateDiskAvailPerf();
    mIndexing[perftype].insert(make_pair(key, diskunit));
    mDiskRanking[perftype].push_back(diskunit);

    return ret;
}

ywb_status_t
TierDiskAvailPerfRanking::SetupDiskAvailPerfRankingUnit(DiskKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t perftype = IOStat::IOST_raw;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    PerfManager* perfmgr = NULL;
    AST* ast = NULL;
    PerfProfileManager* profilemgr = NULL;
    DiskPerfProfile* profile = NULL;
    LogicalConfig* config = NULL;
    TimeManager* timemgr = NULL;
    CycleManager* cyclemgr = NULL;
    Disk* disk = NULL;
    DiskBaseProp* prop = NULL;
    DiskIO* diskio = NULL;
    DiskPerf* diskperf = NULL;
    DiskAvailPerfRankingUnit* diskunit = NULL;

    ast_log_debug("Setup available performance ranking for disk: ["
            << key.GetSubPoolId() << ", " << key.GetDiskId() << "]");
    subpoolperf = mSubPoolPerf;
    subpoolperf->GetPerfManager(&perfmgr);
    perfmgr->GetAST(&ast);
    ast->GetPerformanceProfile(&profilemgr);
    ast->GetLogicalConfig(&config);
    ast->GetTimeManager(&timemgr);
    timemgr->GetCycleManager(&cyclemgr);

    ret = config->GetDisk(key, &disk);
    if((YWB_SUCCESS == ret) && (disk != NULL))
    {
        disk->GetDiskIO(&diskio);
        disk->GetBaseProp(&prop);

        /*
         * if not get a matched(exactly or fuzzy) disk performance
         * profile, we will use hard coded value instead, so feel
         * free to ignore the return value(even if @profile is NULL)
         **/
        profilemgr->GetDiskPerfProfile(*prop, &profile);
        for(; perftype < IOStat::IOST_cnt; perftype++)
        {
            diskperf = new DiskPerf(cyclemgr->GetCyclePeriod(), profile, diskio, disk);
            if(NULL == diskperf)
            {
                ast_log_debug("Out of memory!");
                config->PutDisk(disk);
                ret = YFS_EOUTOFMEMORY;
                return ret;
            }

            diskunit = new DiskAvailPerfRankingUnit(key, diskperf, this);
            if(NULL == diskunit)
            {
                ast_log_debug("Out of memory!");
                config->PutDisk(disk);

                delete diskperf;
                diskperf = NULL;

                ret = YFS_EOUTOFMEMORY;
                return ret;
            }

            diskperf->CalculateDiskPerf(perftype);
            diskunit->CalculateDiskAvailPerf();
            mIndexing[perftype].insert(make_pair(key, diskunit));
            mDiskRanking[perftype].push_back(diskunit);
        }

        config->PutDisk(disk);
    }

    return ret;
}

ywb_status_t
TierDiskAvailPerfRanking::GetDiskAvailPerfRankingUnit(DiskKey& key,
        ywb_uint32_t perftype, DiskAvailPerfRankingUnit** diskunit)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskKey, DiskAvailPerfRankingUnit*, DiskKeyCmp>::iterator iter;

    if(perftype < 0 || perftype >= IOStat::IOST_cnt)
    {
        return YFS_EINVAL;
    }

    iter = mIndexing[perftype].find(key);
    if(iter != mIndexing[perftype].end())
    {
        *diskunit = iter->second;
    }
    else
    {
        ret = YFS_ENOENT;
        *diskunit = NULL;
    }

    return ret;
}

ywb_status_t
TierDiskAvailPerfRanking::RemoveDiskAvailPerfRankingUnit(DiskKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t found = false;
    ywb_uint32_t perftype = IOStat::IOST_raw;
    ywb_uint32_t noentryperfs = 0;
    DiskKey* diskkey = NULL;
    map<DiskKey, DiskAvailPerfRankingUnit*, DiskKeyCmp>::iterator mapiter;
    vector<DiskAvailPerfRankingUnit*>::iterator veciter;
    DiskAvailPerfRankingUnit* diskunit = NULL;

    ast_log_debug("Remove available performance ranking for disk: ["
            << "subpool: " << key.GetSubPoolId()
            << "disk: " << key.GetDiskId());

    for(; perftype < IOStat::IOST_cnt; perftype++)
    {
        mapiter = mIndexing[perftype].find(key);
        if(mapiter != mIndexing[perftype].end())
        {
            diskunit = mapiter->second;
            mIndexing[perftype].erase(mapiter);

            found = false;
            veciter = mDiskRanking[perftype].begin();
            while(veciter != mDiskRanking[perftype].end())
            {
                diskunit = *veciter;
                diskunit->GetDiskKey(&diskkey);
                if(*diskkey == key)
                {
                    found = true;
                    mDiskRanking[perftype].erase(veciter);
                    delete diskunit;
                    diskunit = NULL;
                    break;
                }
                else
                {
                    veciter++;
                }
            }

            YWB_ASSERT(true == found);
        }
        else
        {
            noentryperfs++;
        }
    }

    if(IOStat::IOST_cnt == noentryperfs)
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

void
TierDiskAvailPerfRanking::SortDisks(ywb_uint32_t perftype)
{
    YWB_ASSERT((0 <= perftype) && (perftype < IOStat::IOST_cnt));
    if(!mDiskRanking[perftype].empty())
    {
        sort(mDiskRanking[perftype].begin(),
                mDiskRanking[perftype].end(), mGreater);
        mCurFavoritePos[perftype] = 0;
    }

    ast_log_debug("Sort disks according to performance type: " << perftype);
}

void
TierDiskAvailPerfRanking::SortDisks()
{
    ywb_uint32_t perftype = IOStat::IOST_cnt;

    ast_log_debug("Sort disks");
    for(perftype = IOStat::IOST_raw; perftype < IOStat::IOST_cnt; perftype++)
    {
        SortDisks(perftype);
    }
}

void
TierDiskAvailPerfRanking::GetSortedDisks(
        ywb_uint32_t perftype, vector<DiskKey>* diskvec)
{
    vector<DiskAvailPerfRankingUnit*>::iterator diskiter;
    DiskAvailPerfRankingUnit* diskunit = NULL;
    DiskKey* diskkey = NULL;

    YWB_ASSERT((0 <= perftype) && (perftype < IOStat::IOST_cnt));
    YWB_ASSERT(diskvec != NULL);

    if(!mDiskRanking[perftype].empty())
    {
        diskiter = mDiskRanking[perftype].begin();
        while(diskiter != mDiskRanking[perftype].end())
        {
            diskunit = *diskiter;
            diskunit->GetDiskKey(&diskkey);
            diskvec->push_back(*diskkey);
            diskiter++;
        }
    }
}

ywb_status_t
TierDiskAvailPerfRanking::Reserve(
        DiskKey& key, ywb_uint32_t perftype,
        ywb_int32_t iosrequired,
        ywb_int32_t bwrequired,
        ywb_int32_t caprequired)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskunit = NULL;

    if(perftype < 0 || perftype >= IOStat::IOST_cnt)
    {
        return YFS_EINVAL;
    }

    ret = GetDiskAvailPerfRankingUnit(key, perftype, &diskunit);
    if(YWB_SUCCESS == ret && diskunit != NULL)
    {
        ret = diskunit->Reserve(iosrequired, bwrequired, caprequired);
    }

    ast_log_debug("Reserve on disk: [subpool: " << key.GetSubPoolId()
            << ", disk: " << key.GetDiskId()
            << "], peformance type: " << perftype
            << ", iosrequired: " << iosrequired
            << ", bwrequired: " << bwrequired
            << ", caprequired: " << caprequired
            << ", ret: " << ret);

    return ret;
}

ywb_status_t
TierDiskAvailPerfRanking::UnReserve(
        DiskKey& key, ywb_uint32_t perftype,
        ywb_int32_t iosrequired,
        ywb_int32_t bwrequired,
        ywb_int32_t caprequired)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskunit = NULL;

    if(perftype < 0 || perftype >= IOStat::IOST_cnt)
    {
        return YFS_EINVAL;
    }

    ret = GetDiskAvailPerfRankingUnit(key, perftype, &diskunit);
    if(YWB_SUCCESS == ret && diskunit != NULL)
    {
        ret = diskunit->UnReserve(iosrequired, bwrequired, caprequired);
    }

    ast_log_debug("Reserve on disk: [subpool: " << key.GetSubPoolId()
            << ", disk: " << key.GetDiskId()
            << "], peformance type: " << perftype
            << ", iosrequired: " << iosrequired
            << ", bwrequired: " << bwrequired
            << ", caprequired: " << caprequired
            << ", ret: " << ret);

    return ret;
}

ywb_status_t
TierDiskAvailPerfRanking::Consume(
        DiskKey& key, ywb_uint32_t perftype,
        ywb_int32_t iosrequired,
        ywb_int32_t bwrequired,
        ywb_int32_t caprequired)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskunit = NULL;

    if(perftype < 0 || perftype >= IOStat::IOST_cnt)
    {
        return YFS_EINVAL;
    }

    ret = GetDiskAvailPerfRankingUnit(key, perftype, &diskunit);
    if(YWB_SUCCESS == ret && diskunit != NULL)
    {
        ret = diskunit->Consume(iosrequired, bwrequired, caprequired);
    }

    ast_log_debug("Consume on disk: [subpool: " << key.GetSubPoolId()
            << ", disk: " << key.GetDiskId()
            << "], peformance type: " << perftype
            << ", iosrequired: " << iosrequired
            << ", bwrequired: " << bwrequired
            << ", caprequired: " << caprequired
            << ", ret: " << ret);

    return ret;
}

ywb_status_t
TierDiskAvailPerfRanking::GetFavoriteDisk(
        ywb_uint32_t perftype, ywb_uint32_t iopsrequired,
        ywb_uint32_t bwrequired, ywb_int32_t caprequired,
        BlackList* blacklist, WhiteList* whitelist, DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    map<DiskKey, DiskAvailPerfRankingUnit*, DiskKeyCmp>::iterator iter1;
    vector<DiskAvailPerfRankingUnit*>::iterator iter2;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;
    WhiteList* whitedisks = NULL;
    DiskKey whiteitemkey;
    DiskKey* diskkey2 = NULL;
    ywb_uint32_t curindex = 0;

    if(perftype < 0 || perftype >= IOStat::IOST_cnt)
    {
        return YFS_EINVAL;
    }

    whitedisks = whitelist;
    if((NULL == whitedisks) || (whitedisks->Empty()))
    {
        /*
         * if whitelist is not assigned, or it is empty,
         * then assign it by ourselves
         **/
        whitedisks = new WhiteList();
        if(NULL == whitedisks)
        {
            ast_log_debug("Out of memory!");
            ret = YFS_EOUTOFMEMORY;
            return ret;
        }

        iter1 = mIndexing[perftype].begin();
        for(; iter1 != mIndexing[perftype].end(); iter1++)
        {
            whiteitemkey = iter1->first;
            whitedisks->AddDisk(whiteitemkey);
        }

        if((!whitedisks->Empty()) && (blacklist != NULL))
        {
            /*
             * if blacklist is assigned, then update whitelist by removing
             * intersection items of the two
             **/
            whitedisks->GetDifferenceSet(blacklist);
        }
    }

    /*
     * NOTE: should not update whitelist by removing intersection
     * items with blacklist if the whitelist is assigned externally
     **/
    if((whitedisks != NULL) && (!whitedisks->Empty()))
    {
        GetAST(&ast);
        ast->GetLogicalConfig(&config);

        /*figure out those not existed disks and remove them*/
        iter2 = mDiskRanking[perftype].begin();
        for(; iter2 != mDiskRanking[perftype].end(); )
        {
            diskavailperf = *iter2;
            diskavailperf->GetDiskKey(&diskkey2);
            if(!config->CheckDiskExistence(*diskkey2))
            {
                iter2 = mDiskRanking[perftype].erase(iter2);
                iter1 = mIndexing[perftype].find(*diskkey2);
                YWB_ASSERT(iter1 != mIndexing[perftype].end());
                mIndexing[perftype].erase(iter1);
            }
            else
            {
                iter2++;
            }
        }

        /*this is the so-called "on-demand" sorting*/
        SortDisks(perftype);

        /*figure out the first satisfied disks in whitelist*/
        curindex = 0;
        while(curindex < mDiskRanking[perftype].size())
        {
            diskavailperf = mDiskRanking[perftype].at(curindex);
            diskavailperf->GetDiskKey(&diskkey2);
            /*disk is in whitelist*/
            ret = whitedisks->Find(*diskkey2);
            if((YWB_SUCCESS == ret))
            {
                break;
            }
            else
            {
                curindex++;
            }
        }

        /*no sorted disks in whitelist*/
        if(curindex == mDiskRanking[perftype].size())
        {
            ret = YFS_ENOENT;
        }
        else
        {
            if((diskavailperf->GetIOAvail() < iopsrequired) ||
                    (diskavailperf->GetBWAvail() < bwrequired) ||
                    (diskavailperf->GetCapAvail() < caprequired))
            {
                ret = YFS_ENOSPACE;
            }
            else
            {
                diskavailperf->GetDiskKey(&diskkey2);
                *diskkey = *diskkey2;
                ret = YWB_SUCCESS;
            }
        }
    }
    else
    {
        ret = YFS_ENOENT;
    }

    if(((NULL == whitelist) || (whitelist->Empty())) && (whitedisks != NULL))
    {
        delete whitedisks;
        whitedisks = NULL;
    }

    ast_log_debug("Get favorite disk for performance type: " << perftype
            << ", requirement: [iosrequired: " << iopsrequired
            << ", bwrequired: " << bwrequired
            << ", caprequired: " << caprequired
            << "], retval: " << ret);

    return ret;
}

ywb_status_t
TierDiskAvailPerfRanking::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    vector<DiskAvailPerfRankingUnit*>::iterator veciter;
    DiskAvailPerfRankingUnit* unit = NULL;
    ywb_uint32_t perftype = 0;

    for(; perftype < Constant::DISK_PERF_TYPE_CNT; perftype++)
    {
        if(!mIndexing[perftype].empty())
        {
            mIndexing[perftype].clear();
        }

        veciter = mDiskRanking[perftype].begin();
        for(; veciter != mDiskRanking[perftype].end(); )
        {
            unit = *veciter;
            veciter = mDiskRanking[perftype].erase(veciter);
            delete unit;
            unit = NULL;
        }
    }

    mGreater = NULL;
    mSubPoolPerf = NULL;

    return ret;
}

ywb_status_t
SubPoolDiskAvailPerfRanking::SetupTierDiskAvailPerfRanking(
        ywb_uint32_t tiertype, DiskAvailPerfRankingCmp::Greater greaterthan)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tierperf = NULL;
    Tier* tier = NULL;
    list<DiskKey> diskkeys;
    list<Disk*> disks;
    list<DiskKey>::iterator diskkeyiter;
    list<Disk*>::iterator diskiter;
    Disk* disk = NULL;
    ywb_uint32_t diskunitnum = 0;

    ast_log_debug("Setup tier wise disk available performance ranking"
            << " for tier: " << tiertype);
    ret = mSubPool->GetTier(tiertype, &tier);
    YWB_ASSERT(tier != NULL);
    tierperf = new TierDiskAvailPerfRanking(this, greaterthan);
    if(tierperf == NULL)
    {
        ast_log_debug("Out of memory!");
        mSubPool->PutTier(tier);
        ret = YFS_EOUTOFMEMORY;
        return ret;
    }

    tier->GetKeysAndDisks(diskkeys, disks);
    YWB_ASSERT(diskkeys.size() == disks.size());
    diskkeyiter = diskkeys.begin();
    diskiter = disks.begin();
    while(diskkeyiter != diskkeys.end())
    {
        disk = *diskiter;
        ret = tierperf->SetupDiskAvailPerfRankingUnit(*diskkeyiter);
        tier->PutDisk(disk);
        if((YFS_EOUTOFMEMORY == ret))
        {
            ast_log_debug("Out of memory!");
            break;
        }

        diskunitnum++;
        diskkeyiter++;
        diskiter++;
    }

    mSubPool->PutTier(tier);
    if((diskunitnum > 0))
    {
        mTierDiskRanking[tiertype] = tierperf;
        ret = YWB_SUCCESS;
    }
    else
    {
        delete tierperf;
        tierperf = NULL;
    }

    return ret;
}

ywb_status_t
SubPoolDiskAvailPerfRanking::GetTierDiskAvailPerfRanking(ywb_uint32_t tier,
            TierDiskAvailPerfRanking** tieravail)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(tier < 0 || tier >= Tier::TIER_cnt)
    {
        return YFS_EINVAL;
    }

    *tieravail = mTierDiskRanking[tier];
    if(NULL  == mTierDiskRanking[tier])
    {
        ret = YFS_ENOENT;
        *tieravail = NULL;
    }

    return ret;
}

ywb_status_t
SubPoolDiskAvailPerfRanking::Reserve(DiskKey& key,
        ywb_uint32_t perftype, ywb_int32_t iosrequired,
        ywb_int32_t bwrequired, ywb_int32_t caprequired)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = Tier::TIER_cnt;
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;
    LogicalConfig* config = NULL;
    TierDiskAvailPerfRanking* tierperf = NULL;
    Disk* disk = NULL;

    GetPerfManager(&perfmgr);
    perfmgr->GetAST(&ast);
    ast->GetLogicalConfig(&config);
    YWB_ASSERT(mSubPool != NULL);
    if(!config->CheckDiskExistence(key))
    {
        return YFS_ENOENT;
    }

    mSubPool->GetDisk(key, &disk);
    YWB_ASSERT(disk != NULL);
    tier = disk->GetTier();
    tierperf = mTierDiskRanking[tier];
    YWB_ASSERT(tierperf != NULL);
    mSubPool->PutDisk(disk);

    ret = tierperf->Reserve(key, perftype,
            iosrequired, bwrequired, caprequired);
    return ret;
}

ywb_status_t
SubPoolDiskAvailPerfRanking::UnReserve(DiskKey& key,
        ywb_uint32_t perftype, ywb_int32_t iosrequired,
        ywb_int32_t bwrequired, ywb_int32_t caprequired)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = Tier::TIER_cnt;
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;
    LogicalConfig* config = NULL;
    TierDiskAvailPerfRanking* tierperf = NULL;
    Disk* disk = NULL;

    GetPerfManager(&perfmgr);
    perfmgr->GetAST(&ast);
    ast->GetLogicalConfig(&config);
    YWB_ASSERT(mSubPool != NULL);
    if(!config->CheckDiskExistence(key))
    {
        return YFS_ENOENT;
    }

    mSubPool->GetDisk(key, &disk);
    YWB_ASSERT(disk != NULL);
    tier = disk->GetTier();
    tierperf = mTierDiskRanking[tier];
    YWB_ASSERT(tierperf != NULL);
    mSubPool->PutDisk(disk);

    ret = tierperf->UnReserve(key, perftype,
            iosrequired, bwrequired, caprequired);
    return ret;
}

ywb_status_t
SubPoolDiskAvailPerfRanking::Consume(
        DiskKey& key, ywb_uint32_t perftype,
        ywb_int32_t iosrequired, ywb_int32_t bwrequired,
        ywb_int32_t caprequired)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = Tier::TIER_cnt;
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;
    LogicalConfig* config = NULL;
    TierDiskAvailPerfRanking* tierperf = NULL;
    Disk* disk = NULL;

    GetPerfManager(&perfmgr);
    perfmgr->GetAST(&ast);
    ast->GetLogicalConfig(&config);
    YWB_ASSERT(mSubPool != NULL);
    if(!config->CheckDiskExistence(key))
    {
        return YFS_ENOENT;
    }

    mSubPool->GetDisk(key, &disk);
    YWB_ASSERT(disk != NULL);
    tier = disk->GetTier();
    tierperf = mTierDiskRanking[tier];
    YWB_ASSERT(tierperf != NULL);
    mSubPool->PutDisk(disk);

    ret = tierperf->Consume(key, perftype,
            iosrequired, bwrequired, caprequired);
    return ret;
}

ywb_status_t
SubPoolDiskAvailPerfRanking::GetFavoriteDisk(
        ywb_uint32_t tier, ywb_uint32_t perftype,
        ywb_uint32_t iopsrequired, ywb_uint32_t bwrequired,
        ywb_uint32_t caprequired, BlackList* blacklist,
        WhiteList* whitelist, DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tierperf = NULL;

    ret = GetTierDiskAvailPerfRanking(tier, &tierperf);
    if((YWB_SUCCESS == ret) && (tierperf != NULL))
    {
        ret = tierperf->GetFavoriteDisk(perftype, iopsrequired,
                bwrequired, caprequired, blacklist, whitelist, diskkey);
    }

    return ret;
}

ywb_status_t
SubPoolDiskAvailPerfRanking::Destroy(ywb_uint32_t tier)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(tier < 0 || tier >= Tier::TIER_cnt)
    {
        return YFS_EINVAL;
    }

    if(mTierDiskRanking[tier] != NULL)
    {
        delete mTierDiskRanking[tier];
        mTierDiskRanking[tier] = NULL;
    }

    return ret;
}

ywb_status_t
SubPoolDiskAvailPerfRanking::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = 0;

    for(; tier < Constant::TIER_NUM; tier++)
    {
        if(mTierDiskRanking[tier] != NULL)
        {
            delete mTierDiskRanking[tier];
            mTierDiskRanking[tier] = NULL;
        }
    }

    if(mSubPool)
    {
        mSubPool->DecRef();
        mSubPool = NULL;
    }

    mPerfMgr = NULL;
    return ret;
}

ywb_status_t
PerfManager::SetupSubPoolDiskAvailPerfRanking(SubPoolKey& key,
        DiskAvailPerfRankingCmp::Greater greaterthan)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tiertype = Tier::TIER_0;
    AST* ast = NULL;
    LogicalConfig *config = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;

    ast_log_debug("Setup subpool wise available performance ranking"
            << " for subpool: [" << key.GetOss()
            << ", " << key.GetPoolId()
            << ", " << key.GetSubPoolId() << "]");
    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ret = config->GetSubPool(key, &subpool);
    if((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        subpoolperf = new SubPoolDiskAvailPerfRanking(subpool, this);
        if(NULL == subpoolperf)
        {
            ast_log_debug("Out of memory!");
            config->PutSubPool(subpool);
            ret = YFS_EOUTOFMEMORY;
            return ret;
        }

        for(; tiertype < Tier::TIER_cnt; tiertype++)
        {
            ret = subpool->GetTier(tiertype, &tier);
            if((YWB_SUCCESS == ret) && (tier != NULL))
            {
                ret = subpoolperf->SetupTierDiskAvailPerfRanking(
                        tiertype, greaterthan);
                subpool->PutTier(tier);
                if(ret != YWB_SUCCESS)
                {
                    break;
                }
            }
        }

        config->PutSubPool(subpool);
        mSubPoolAvailPerfRanking.insert(make_pair(key, subpoolperf));
    }
    else
    {
        ast_log_debug("No subpool corresponding to key: ["
                << key.GetOss()
                << ", " << key.GetPoolId()
                << ", " << key.GetSubPoolId()
                << "]");
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PerfManager::GetSubPoolDiskAvailPerfRanking(SubPoolKey& key,
        SubPoolDiskAvailPerfRanking** subpoolavail)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolDiskAvailPerfRanking*, SubPoolKeyCmp>::iterator iter;

    iter = mSubPoolAvailPerfRanking.find(key);
    if(iter != mSubPoolAvailPerfRanking.end())
    {
        *subpoolavail = iter->second;
    }
    else
    {
        ret = YFS_ENOENT;
        *subpoolavail = NULL;
    }

    return ret;
}

ywb_status_t
PerfManager::GetFavoriteDisk(SubPoolKey& key, ywb_uint32_t tier,
        ywb_uint32_t perftype, ywb_uint32_t iopsrequired,
        ywb_uint32_t bwrequired, ywb_uint32_t caprequired,
        BlackList* blacklist, WhiteList* whitelist, DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;

    ret = GetSubPoolDiskAvailPerfRanking(key, &subpoolperf);
    if((YWB_SUCCESS == ret) && (subpoolperf != NULL))
    {
        ret = subpoolperf->GetFavoriteDisk(tier, perftype, iopsrequired,
                bwrequired, caprequired, blacklist, whitelist, diskkey);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PerfManager::Destroy(SubPoolKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolDiskAvailPerfRanking*, SubPoolKeyCmp>::iterator iter;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;

    iter = mSubPoolAvailPerfRanking.find(key);
    if(iter != mSubPoolAvailPerfRanking.end())
    {
        subpoolperf = iter->second;
        mSubPoolAvailPerfRanking.erase(iter);
        delete subpoolperf;
        subpoolperf = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
PerfManager::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;

    Reset();
    mAst = NULL;
    return ret;
}

ywb_status_t
PerfManager::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolDiskAvailPerfRanking*, SubPoolKeyCmp>::iterator iter;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;

    ast_log_debug("PerfManager reset");
    iter = mSubPoolAvailPerfRanking.begin();
    for(; iter != mSubPoolAvailPerfRanking.end(); )
    {
        subpoolperf = iter->second;
        mSubPoolAvailPerfRanking.erase(iter++);
        delete subpoolperf;
        subpoolperf = NULL;
    }

    return ret;
}

ywb_status_t
PerfManager::Init()
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    list<SubPoolKey> subpoolkeys;
    list<SubPool*> subpools;
    list<SubPoolKey>::iterator subpoolkeyiter;
    list<SubPool*>::iterator subpooliter;
    SubPool* subpool = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    config->GetKeysAndSubPools(subpoolkeys, subpools);
    YWB_ASSERT(subpoolkeys.size() == subpools.size());

    subpoolkeyiter = subpoolkeys.begin();
    subpooliter = subpools.begin();
    while(subpoolkeyiter != subpoolkeys.end())
    {
        subpool = *subpooliter;
        /*no advice against this subpool*/
        if(!(subpool->TestFlag(SubPool::SPF_no_advice)))
        {
            ret = SetupSubPoolDiskAvailPerfRanking(*subpoolkeyiter,
                    DiskAvailPerfRankingCmp::DiskAvailPerfRankingGreater);
        }

        config->PutSubPool(subpool);
        if((YFS_EOUTOFMEMORY == ret))
        {
            ast_log_debug("Out of memory!");
            break;
        }

        subpoolkeyiter++;
        subpooliter++;
    }

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
