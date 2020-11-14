#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/Plan/ASTPlanUnitTests.hpp"

//Migration* gMigration = NULL;
void ASTPlanTestEnvironment::SetUp()
{
//    gMigration = new Migration(NULL);
//    YWB_ASSERT(gMigration != NULL);
//    gMigration->Initialize();
//    gMigration->StartMigrator("default");
}

void ASTPlanTestEnvironment::TearDown()
{
//    gMigration->StopMigrator();
//    gMigration->Finalize();
//    if(gMigration != NULL)
//    {
//        delete gMigration;
//        gMigration = NULL;
//    }
}

DiskScheStat* DiskScheStatTest::GetDiskScheStat()
{
    return mDiskScheStat;
}

SubPoolPlanDirStat* SubPoolPlanDirStatTest::GetSubPoolPlanDirStat()
{
    return mSubPoolPlanDirStat;
}

void SubPoolPlansTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t planloop = 0;
    ywb_uint64_t inodeid = 0;
    ywb_uint64_t tgtid = 0;
    ywb_uint32_t factors = 0;
    SubPoolKey subpoolkey;
    OBJKey objkey;
    ChunkIOStat rawstat;
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;

    inodeid = Constant::DEFAULT_INODE;
    subpoolplans = mSubPoolPlans;
    tgtid = Constant::DISK_ID + 1;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    for(planloop = 0; planloop < 6; planloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        factors = inodeid;
        rawstat.SetRndReadIOs(factors);
        rawstat.SetRndReadBW(factors * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndWriteIOs(factors);
        rawstat.SetRndWriteBW(factors * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndTotalRT((factors * (Constant::META_RT_READ)) +
                (factors * (Constant::META_RT_WRITE)));
        rawstat.SetSeqReadIOs(factors);
        rawstat.SetSeqReadBW(factors * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqWriteIOs(factors);
        rawstat.SetSeqWriteBW(factors * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqTotalRT((factors * (Constant::META_RT_READ)) +
                (factors * (Constant::META_RT_WRITE)));

        plan = new Plan(objkey, tgtid, subpoolkey, rawstat, IOStat::IOST_raw,
                /*Constant::DEFAULT_CAP*/1, Plan::PD_ctr_promote, 1001);
        YWB_ASSERT(plan != NULL);
        ret = subpoolplans->AddPlan(plan);
        YWB_ASSERT(YWB_SUCCESS == ret);
    }
}

void SubPoolPlansTest::TearDown()
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolAdvice* subpooladvice = NULL;

    subpoolplans = mSubPoolPlans;
    subpoolplans->GetSubPoolAdvise(&subpooladvice);
    /*Increase SubPoolAdvice reference to avoid double free issue*/
    subpooladvice->IncRef();
    ret = subpoolplans->Destroy();
    YWB_ASSERT(YWB_SUCCESS == ret);
}

SubPoolPlans* SubPoolPlansTest::GetSubPoolPlans()
{
    return mSubPoolPlans;
}

ywb_status_t SubPoolPlansTest::AddPlan(ywb_uint64_t inodeid)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtid = 0;
    ywb_uint32_t factors = 0;
    SubPoolKey subpoolkey;
    OBJKey objkey;
    ChunkIOStat rawstat;
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;

    subpoolplans = mSubPoolPlans;
    tgtid = Constant::DISK_ID + 1;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    objkey.SetInodeId(inodeid);
    factors = inodeid;
    rawstat.SetRndReadIOs(factors);
    rawstat.SetRndReadBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetRndWriteIOs(factors);
    rawstat.SetRndWriteBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetRndTotalRT((factors * (Constant::META_RT_READ)) +
            (factors * (Constant::META_RT_WRITE)));
    rawstat.SetSeqReadIOs(factors);
    rawstat.SetSeqReadBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetSeqWriteIOs(factors);
    rawstat.SetSeqWriteBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetSeqTotalRT((factors * (Constant::META_RT_READ)) +
            (factors * (Constant::META_RT_WRITE)));

    plan = new Plan(objkey, tgtid, subpoolkey, rawstat, IOStat::IOST_raw,
            /*Constant::DEFAULT_CAP*/1, Plan::PD_ctr_promote, 1001);
    YWB_ASSERT(plan != NULL);
    ret = subpoolplans->AddPlan(plan);
    YWB_ASSERT(YWB_SUCCESS == ret);

    return YWB_SUCCESS;
}

void PlanManagerTest::SetUp()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subdiskperfloop = 0;
    ywb_uint32_t diskentryloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint32_t diskid = 0;
    ywb_uint32_t inodeid = 0;
    ywb_uint32_t inodeid2 = 0;
    ywb_uint32_t factors = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    DiskBaseProp diskprop;
    OBJKey objkey;
    ChunkIOStat rawstat;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    PerfProfileManager* profilemgr = NULL;
    DiskPerfProfile* diskperfprofile = NULL;
    SubDiskProfile* subdiskprofile = NULL;
    DiskProfileEntry* diskprofileentry = NULL;
    AdviceManager* advicemgr = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;
    PerfManager* perfmgr = NULL;
    PlanManager* planmgr = NULL;
    Executor* exe = NULL;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};
    ywb_uint32_t diskrpm[3] = {15000, 10000, 7500};
    ywb_uint32_t subdiskrwratio[3] = {0, 50, 100};
    ywb_uint32_t diskentryiosize[2] = {4, 8};
    ywb_uint32_t bws[3][3][2] = {
            /*rwratio:0     |       50     |      100       */
            /*iosize:4k 8k  |    4k     8k |     4k      8k */
            {{40000, 64000}, {24000, 40000}, {60000, 100000}},/*disk rpm 7500*/
            {{48000, 80000}, {28000, 48000}, {72000, 120000}},/*disk rpm 10000*/
            {{80000, 128000}, {48000, 80000}, {120000, 200000}}/*disk rpm 15000*/
    };
    ywb_uint32_t ios[3][3][2] = {
            /*rwratio:0     |       50     |      100       */
            /*iosize:4k 8k  |    4k     8k |     4k      8k */
            {{10000, 8000}, {6000, 5000}, {15000, 12500}},/*disk rpm 7500*/
            {{12000, 10000}, {7000, 6000}, {18000, 15000}},/*disk rpm 10000*/
            {{20000, 16000}, {12000, 10000}, {30000, 25000}}/*disk rpm 15000*/
    };

    config = mConfig;
    advicemgr = mAdviceMgr;
    perfmgr = mPerfMgr;
    profilemgr = mProfileMgr;
    planmgr = mPlanMgr;
    exe = mExe;

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    inodeid2 = Constant::DEFAULT_INODE;

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);

    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(Constant::DEFAULT_RAID_WIDTH);
    diskprop.SetDiskCap(128000);

    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    factors = 10;
    rawstat.SetRndReadIOs(factors);
    rawstat.SetRndReadBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetRndWriteIOs(factors);
    rawstat.SetRndWriteBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetRndTotalRT((factors * (Constant::META_RT_READ)) +
            (factors * (Constant::META_RT_WRITE)));
    rawstat.SetSeqReadIOs(factors);
    rawstat.SetSeqReadBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetSeqWriteIOs(factors);
    rawstat.SetSeqWriteBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetSeqTotalRT((factors * (Constant::META_RT_READ)) +
            (factors * (Constant::META_RT_WRITE)));

    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        subpool = new SubPool(subpoolkey);
        YWB_ASSERT(subpool != NULL);
        /*
         * AST will clear this flag only if it can do generate
         * advise against this subpool under real environment,
         * here clear it to just for simulating, otherwise there
         * will be no any advice against all subpools
         **/
        subpool->ClearFlag(SubPool::SPF_no_advice);
        config->AddSubPool(subpoolkey, subpool);

        /*Logical configuration related*/
        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            diskkey.SetSubPoolId(subpoolid);
            diskkey.SetDiskId(diskid);
            diskprop.SetDiskRPM(diskrpm[diskloop]);
            diskprop.SetDiskClass(diskclass[diskloop]);
            disk = new Disk(diskprop);
            YWB_ASSERT(disk != NULL);
            config->AddDisk(diskkey, disk);

            for(objloop = 0; objloop < 6; objloop++, inodeid++)
            {
                objkey.SetStorageId(diskid);
                objkey.SetInodeId(inodeid);
                obj = new OBJ(objkey);
                YWB_ASSERT(obj != NULL);
                objio = new OBJIO();
                YWB_ASSERT(objio != NULL);
                /*simulate the OBJ has been monitored for a long period*/
                objio->SetStatCycles(Constant::SHORT_TERM_DECISION_WINDOW);
                objio->SetRawIO(rawstat);
                objio->SetOBJ(obj);
                obj->SetOBJIO(objio);
                config->AddOBJ(objkey, obj);
            }

            disk->SummarizeDiskIO();
            diskperfprofile = new DiskPerfProfile();
            YWB_ASSERT(diskperfprofile != NULL);
            for(subdiskperfloop = 0; subdiskperfloop < 3; subdiskperfloop++)
            {
                subdiskprofile = new SubDiskProfile(subdiskrwratio[subdiskperfloop]);
                YWB_ASSERT(subdiskprofile != NULL);

                for(diskentryloop = 0; diskentryloop < 2; diskentryloop++)
                {
                    diskprofileentry = new DiskProfileEntry(
                            subdiskrwratio[subdiskperfloop],
                            diskentryiosize[diskentryloop],
                            bws[diskloop][subdiskperfloop][diskentryloop],
                            ios[diskloop][subdiskperfloop][diskentryloop]);
                    YWB_ASSERT(diskprofileentry != NULL);
                    subdiskprofile->AddDiskProfileEntry(diskprofileentry);
                }

                diskperfprofile->SetRndPerf(subdiskrwratio[subdiskperfloop], subdiskprofile);
            }

            profilemgr->AddDiskPerfProfile(diskprop, diskperfprofile);
        }

        /*Advice related*/
        if(5 != subpoolloop)
        {
            subpooladvice = new SubPoolAdvice(SubPool::TC_ssd_ent_sata);
            YWB_ASSERT(subpooladvice != NULL);
            advicemgr->AddSubPoolAdvice(subpoolkey, subpooladvice);
        }

        if(0 == subpoolloop)
        {
            inodeid2 = Constant::DEFAULT_INODE + (2 * 6);
            /*subpool with both hot and cold advise*/
            for(objloop = 0; objloop < 6; objloop++, inodeid2++)
            {
                /*hot advise*/
                objkey.SetStorageId(diskid - 1);
                objkey.SetInodeId(inodeid2);

                advice = new Advice(objkey, Tier::TIER_2, Tier::TIER_1,
                        rawstat, IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
                subpooladvice->AddAdvice(Tier::TIER_2, AdviceType::AT_ctr_hot, advice);
            }

            inodeid2 = Constant::DEFAULT_INODE;
            for(objloop = 0; objloop < 6; objloop++, inodeid2++)
            {
                /*cold advise*/
                objkey.SetStorageId(diskid - 3);
                objkey.SetInodeId(inodeid2);

                advice = new Advice(objkey, Tier::TIER_0, Tier::TIER_1,
                        rawstat, IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
                subpooladvice->AddAdvice(Tier::TIER_0, AdviceType::AT_ctr_cold, advice);
            }
        }
        else if(1 == subpoolloop)
        {
            inodeid2 = Constant::DEFAULT_INODE + (5 * 6);
            /*subpool with only hot advise*/
            for(objloop = 0; objloop < 6; objloop++, inodeid2++)
            {
                /*hot advise*/
                objkey.SetStorageId(diskid - 1);
                objkey.SetInodeId(inodeid2);

                advice = new Advice(objkey, Tier::TIER_2, Tier::TIER_1,
                        rawstat, IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
                subpooladvice->AddAdvice(Tier::TIER_2, AdviceType::AT_ctr_hot, advice);
            }
        }
        else if(2 == subpoolloop)
        {
            inodeid2 = Constant::DEFAULT_INODE + (6 * 6);
            /*subpool with only cold advise*/
            for(objloop = 0; objloop < 6; objloop++, inodeid2++)
            {
                /*cold advise*/
                objkey.SetStorageId(diskid - 3);
                objkey.SetInodeId(inodeid2);

                advice = new Advice(objkey, Tier::TIER_0, Tier::TIER_1,
                        rawstat, IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
                subpooladvice->AddAdvice(Tier::TIER_0, AdviceType::AT_ctr_cold, advice);
            }
        }
        else if(3 == subpoolloop)
        {
            /*subpool with SPF_no_advice set*/
            subpool->SetFlagComb(SubPool::SPF_no_advice);
        }
        else if(4 == subpoolloop)
        {
            inodeid2 = Constant::DEFAULT_INODE + (14 * 6);
            /*subpool has advice but no corresponding SubPoolPerf*/
            for(objloop = 0; objloop < 6; objloop++, inodeid2++)
            {
                /*hot advise*/
                objkey.SetStorageId(diskid - 1);
                objkey.SetInodeId(inodeid2);

                advice = new Advice(objkey, Tier::TIER_2, Tier::TIER_1,
                        rawstat, IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
                subpooladvice->AddAdvice(Tier::TIER_2, AdviceType::AT_ctr_hot, advice);
            }

            inodeid2 = Constant::DEFAULT_INODE + (12 * 6);
            for(objloop = 0; objloop < 6; objloop++, inodeid2++)
            {
                /*cold advise*/
                objkey.SetStorageId(diskid - 3);
                objkey.SetInodeId(inodeid2);

                advice = new Advice(objkey, Tier::TIER_0, Tier::TIER_1,
                        rawstat, IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
                subpooladvice->AddAdvice(Tier::TIER_0, AdviceType::AT_ctr_cold, advice);
            }
        }
        else if(5 == subpoolloop)
        {
            /*subpool without SubPoolAdvice*/
        }
        else if(6 == subpoolloop)
        {
            /*subpool with 8 disks on tier 1, with both hot and cold advise*/
            inodeid2 = Constant::DEFAULT_INODE + (20 * 6);
            /*subpool with both hot and cold advise*/
            for(objloop = 0; objloop < 6; objloop++, inodeid2++)
            {
                /*hot advise*/
                objkey.SetStorageId(diskid - 1);
                objkey.SetInodeId(inodeid2);

                advice = new Advice(objkey, Tier::TIER_2, Tier::TIER_1,
                        rawstat, IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
                subpooladvice->AddAdvice(Tier::TIER_2, AdviceType::AT_ctr_hot, advice);
            }

            inodeid2 = Constant::DEFAULT_INODE + (18 * 6);
            for(objloop = 0; objloop < 6; objloop++, inodeid2++)
            {
                /*cold advise*/
                objkey.SetStorageId(diskid - 3);
                objkey.SetInodeId(inodeid2);

                advice = new Advice(objkey, Tier::TIER_0, Tier::TIER_1,
                        rawstat, IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
                subpooladvice->AddAdvice(Tier::TIER_0, AdviceType::AT_ctr_cold, advice);
            }

            diskprop.SetDiskRPM(10000);
            diskprop.SetDiskClass(DiskBaseProp::DCT_ent);

            /*add 5 more disk on tier 1*/
            for(diskloop = 0; diskloop < 5; diskloop++, diskid++)
            {
                diskkey.SetSubPoolId(subpoolid);
                diskkey.SetDiskId(diskid);

                disk = new Disk(diskprop);
                YWB_ASSERT(disk != NULL);
                config->AddDisk(diskkey, disk);

                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    objkey.SetStorageId(diskid);
                    objkey.SetInodeId(inodeid);
                    obj = new OBJ(objkey);
                    YWB_ASSERT(obj != NULL);
                    objio = new OBJIO();
                    YWB_ASSERT(objio != NULL);
                    /*simulate the OBJ has been monitored for a long period*/
                    objio->SetStatCycles(Constant::SHORT_TERM_DECISION_WINDOW);
                    objio->SetRawIO(rawstat);
                    objio->SetOBJ(obj);
                    obj->SetOBJIO(objio);
                    config->AddOBJ(objkey, obj);
                }

                disk->SummarizeDiskIO();
            }
        }

        /*performance related*/
        if(4 != subpoolloop)
        {
            perfmgr->SetupSubPoolDiskAvailPerfRanking(subpoolkey,
                    DiskAvailPerfRankingCmp::DiskAvailPerfRankingGreater);
        }
    }

    planmgr->Init();
    exe->SetEpoch(1001);
}

void PlanManagerTest::TearDown()
{
    LogicalConfig* config = NULL;
    AdviceManager* advicemgr = NULL;
    PerfManager* perfmgr = NULL;

    config = mConfig;
    advicemgr = mAdviceMgr;
    perfmgr = mPerfMgr;

    perfmgr->Reset();
    advicemgr->Reset();
    config->Reset();
}

Error* PlanManagerTest::GetError()
{
    return mAst->mErr;
}

PlanManager* PlanManagerTest::GetPlanManager()
{
    return mPlanMgr;
}

LogicalConfig* PlanManagerTest::GetConfig()
{
    return mConfig;
}

PerfManager* PlanManagerTest::GetPerfManager()
{
    return mPerfMgr;
}

void PlanManagerTest::MakeAllAdviseRequiresHuge(
        SubPoolKey subpoolkey, ywb_uint32_t advicetype)
{
    ywb_status_t ret = YWB_SUCCESS;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;

    planmgr = mPlanMgr;
    ret = planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    YWB_ASSERT(YWB_SUCCESS == ret);
    subpoolplans->GetSubPoolAdvise(&subpooladvice);
    ret = subpooladvice->GetFirstAdvice(advicetype, &advice);
    while((YWB_SUCCESS == ret))
    {
        advice->SetCap(YWB_UINT32_MAX);
        subpooladvice->PutAdvice(advice);
        ret = subpooladvice->GetNextAdvice(advicetype, &advice);
    }
}

void PlanManagerTest::DrainTierWiseDiskAvailPerf(
        SubPoolKey subpoolkey, ywb_uint32_t tier, ywb_uint32_t perftype)
{
    ywb_status_t ret = YWB_SUCCESS;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;
    vector<DiskKey> diskvec;
    vector<DiskKey>::iterator diskiter;

    perfmgr = mPerfMgr;
    ret = perfmgr->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolavailperf);
    if(YWB_SUCCESS == ret)
    {
        ret = subpoolavailperf->GetTierDiskAvailPerfRanking(tier, &tieravailperf);
        if(YWB_SUCCESS == ret)
        {
            tieravailperf->GetSortedDisks(perftype, &diskvec);
            diskiter = diskvec.begin();
            while(diskiter != diskvec.end())
            {
                ret = tieravailperf->GetDiskAvailPerfRankingUnit(
                        *diskiter, perftype, &diskavailperf);
                if(YWB_SUCCESS == ret)
                {
                    /*drain the available performance*/
                    diskavailperf->Reserve(
                            (diskavailperf->GetIOAvail() - 1),
                            (diskavailperf->GetBWAvail() - 1),
                            (diskavailperf->GetCapAvail() - 1));
                }

                diskiter++;
            }

            diskvec.clear();
        }
    }
}

void PlanManagerTest::DrainSubPoolWiseDiskAvailPerf(
        SubPoolKey subpoolkey, ywb_uint32_t perftype)
{
    ywb_uint32_t tier = 0;

    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        DrainTierWiseDiskAvailPerf(subpoolkey, tier, perftype);
    }
}

void PlanManagerTest::RemoveAllAdvise(
        SubPoolKey subpoolkey, ywb_uint32_t advicetype)
{
    ywb_status_t ret = YWB_SUCCESS;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolAdvice* subpooladvice = NULL;

    planmgr = mPlanMgr;
    ret = planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    YWB_ASSERT(YWB_SUCCESS == ret);
    subpoolplans->GetSubPoolAdvise(&subpooladvice);
    YWB_ASSERT(subpooladvice != NULL);
    subpooladvice->Destroy(advicetype);
}

//void PlanManagerTest::AddDisk(SubPoolKey subpoolkey, ywb_uint64_t diskid,
//        ywb_uint32_t diskclass, ywb_uint32_t diskrpm)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    DiskBaseProp diskprop;
//    DiskKey diskkey;
//    AST* ast = NULL;
//    LogicalConfig* config = NULL;
//    Disk* disk = NULL;
//
//    ast = mAst;
//    ast->GetLogicalConfig(&config);
//
//    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
//    diskprop.SetRaidWidth(Constant::DEFAULT_RAID_WIDTH);
//    diskprop.SetDiskCap(128000);
//    diskprop.SetDiskClass(diskclass);
//    diskprop.SetDiskRPM(diskrpm);
//
//    diskkey.SetSubPoolId(subpoolkey.GetSubPoolId());
//    diskkey.SetDiskId(diskid);
//    disk = new Disk(diskprop);
//    YWB_ASSERT(disk != NULL);
//    ret = config->AddDisk(diskkey, disk);
//    YWB_ASSERT(YWB_SUCCESS == ret);
//}

void PlanManagerTest::GetAllPlansTarget(SubPoolKey subpoolkey,
        ywb_uint64_t* diskids, ywb_uint32_t* disknum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    DiskKey diskkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;

    planmgr = mPlanMgr;
    ret = planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    if((YWB_SUCCESS == ret))
    {
        ret = subpoolplans->GetFirstPlan(&plan);
        while((YWB_SUCCESS == ret))
        {
            diskids[plannum++] = plan->GetTargetId();
            ret = subpoolplans->GetNextPlan(&plan);
        }
    }

    *disknum = plannum;
}

void PlanManagerTest::AddAvailDisks(SubPoolKey subpoolkey,
        ywb_uint64_t* diskids, ywb_uint32_t disknum)
{
    ywb_uint32_t diskloop = 0;
    DiskKey diskkey;
    PlanManager* planmgr = NULL;

    planmgr = mPlanMgr;
    for(diskloop = 0; diskloop < disknum; diskloop++)
    {
        diskkey.SetSubPoolId(subpoolkey.GetSubPoolId());
        diskkey.SetDiskId(diskids[diskloop]);
        planmgr->AddAvailDisk(diskkey);
    }
}

int main(int argc, char *argv[])
{
    int ret = 0;
    std::string logfile =
            YfsDirectorySetting::GetLogDirectory() + Constant::LOG_PATH;

    char ch;
    bool daemon = false;
    bool ignore = false;
    opterr = 0;

    while ((ch = getopt(argc, argv, "gd")) != -1) {
        if (ch == '?') {
            ignore = true;
            continue;
        }
        daemon = true;
    }

    if (daemon && !ignore) {
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    create_logger(logfile, "trace");

    testing::AddGlobalTestEnvironment(new ASTPlanTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
