#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/DiskPerf/ASTDiskPerfUnitTests.hpp"

AST* gAst = NULL;

void ASTDiskPerfTestEnvironment::SetUp()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subdiskperfloop = 0;
    ywb_uint32_t diskentryloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfProfileManager* profilemgr = NULL;
    /*PerfManager* perfmgr = NULL;*/
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    DiskPerfProfile* diskperfprofile = NULL;
    SubDiskProfile* subdiskprofile = NULL;
    DiskProfileEntry* diskprofileentry = NULL;
    ChunkIOStat rawstat;
    ywb_uint32_t factors = 0;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};
    ywb_uint32_t diskrpm[3] = {15000, 10000, 7500};
    ywb_uint32_t diskbitmap[3][3] = {
            {1, 1, 1},
            {1, 0, 1},
            {0, 0, 0}
    };
    ywb_uint32_t metafactors[3][6] =
    {
                    {90, 50, 10, 40, 130, 20},/*Tier 0*/
                    {60, 140, 70, 100, 0, 80},/*Tier 1*/
                    {120, 170, 30, 160, 150, 110},/*Tier 2*/
    };
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

    gAst = GetSingleASTInstance();
    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerformanceProfile(&profilemgr);
    /*ast->GetPerfManager(&perfmgr);*/
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);

    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        subpool = new SubPool(subpoolkey);
        YWB_ASSERT(subpool != NULL);
        /*
         * clear SubPool::SPF_no_advice for setting up
         * disk performance for this SubPool
         **/
        subpool->ClearFlag(SubPool::SPF_no_advice);
        config->AddSubPool(subpoolkey, subpool);

        diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
        diskprop.SetRaidWidth(1);
        diskprop.SetDiskCap(128000);
        diskkey.SetSubPoolId(subpoolid);

        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            if(0 != diskbitmap[subpoolloop][diskloop])
            {
                diskprop.SetDiskClass(diskclass[diskloop % ((sizeof(diskclass) / sizeof(ywb_uint32_t)))]);
                diskprop.SetDiskRPM(diskrpm[diskloop % ((sizeof(diskrpm) / sizeof(ywb_uint32_t)))]);
                diskkey.SetDiskId(diskid);
                disk = new Disk(diskprop);
                YWB_ASSERT(disk != NULL);
                config->AddDisk(diskkey, disk);

                objkey.SetStorageId(diskid);
                objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
                objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
                objkey.SetChunkVersion(1);

                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    factors = metafactors[diskloop][objloop];
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
            else
            {
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    ;
                }
            }
        }
    }

    /*perfmgr->Init();*/
}

void ASTDiskPerfTestEnvironment::TearDown()
{
    if(gAst)
    {
        delete gAst;
        gAst = NULL;
    }
}

void BlackListTest::SetUp()
{
    ywb_uint32_t diskloop = 0;
    DiskKey diskkey;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    for(diskloop = 0; diskloop < 3; diskloop++)
    {
        diskkey.SetDiskId(Constant::DISK_ID + diskloop);
        mBlackList->AddDisk(diskkey);
    }

    for(diskloop = 0; diskloop < 3; diskloop++)
    {
        diskkey.SetDiskId(Constant::DISK_ID + 1 + diskloop);
        mBlackList2->AddDisk(diskkey);
    }

    for(diskloop = 0; diskloop < 3; diskloop++)
    {
        diskkey.SetDiskId(Constant::DISK_ID + 1000 + diskloop);
        mBlackList3->AddDisk(diskkey);
    }
}

void BlackListTest::TearDown()
{

}

BlackList* BlackListTest::GetBlackList()
{
    return mBlackList;
}

BlackList* BlackListTest::GetBlackList2()
{
    return mBlackList2;
}

BlackList* BlackListTest::GetBlackList3()
{
    return mBlackList3;
}

BlackList* BlackListTest::GetBlackList4()
{
    return mBlackList4;
}

void DiskPerfTest::SetUp()
{
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;

    ast = gAst;
    ast->GetPerfManager(&perfmgr);
    perfmgr->Init();
}

void DiskPerfTest::TearDown()
{
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;

    ast = gAst;
    ast->GetPerfManager(&perfmgr);
    perfmgr->Reset();
}

DiskPerf* DiskPerfTest::GetDiskPerfInternal()
{
    return mDiskPerf;
}

DiskPerf* DiskPerfTest::GetDiskPerf(
        ywb_uint32_t subpoolid, ywb_uint64_t diskid)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = Tier::TIER_0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperfranking = NULL;
    TierDiskAvailPerfRanking* tierperfranking = NULL;
    DiskAvailPerfRankingUnit* diskperfranking = NULL;
    Disk* disk = NULL;
    DiskPerf* diskperf = NULL;

    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    ret = perfmgr->GetSubPoolDiskAvailPerfRanking(
            subpoolkey, &subpoolperfranking);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(diskid);
    ret = config->GetDisk(diskkey, &disk);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    tier = disk->GetTier();
    ret = subpoolperfranking->GetTierDiskAvailPerfRanking(
            tier, &tierperfranking);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    ret = tierperfranking->GetDiskAvailPerfRankingUnit(diskkey,
            IOStat::IOST_raw, &diskperfranking);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    diskperfranking->GetDiskPerf(&diskperf);
    return diskperf;
}

void DiskAvailPerfRankingUnitTest::SetUp()
{
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;

    ast = gAst;
    ast->GetPerfManager(&perfmgr);
    perfmgr->Init();
}

void DiskAvailPerfRankingUnitTest::TearDown()
{
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;

    ast = gAst;
    ast->GetPerfManager(&perfmgr);
    perfmgr->Reset();
}

DiskAvailPerfRankingUnit*
DiskAvailPerfRankingUnitTest::GetDiskAvailPerf(
        ywb_uint32_t subpoolid, ywb_uint64_t diskid)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = Tier::TIER_0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperfranking = NULL;
    TierDiskAvailPerfRanking* tierperfranking = NULL;
    DiskAvailPerfRankingUnit* diskperfranking = NULL;
    Disk* disk = NULL;

    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    ret = perfmgr->GetSubPoolDiskAvailPerfRanking(
            subpoolkey, &subpoolperfranking);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(diskid);
    ret = config->GetDisk(diskkey, &disk);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    tier = disk->GetTier();
    ret = subpoolperfranking->GetTierDiskAvailPerfRanking(
            tier, &tierperfranking);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    tierperfranking->GetDiskAvailPerfRankingUnit(diskkey,
            IOStat::IOST_raw, &diskperfranking);

    return diskperfranking;
}

void TierDiskAvailPerfRankingTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    AST* ast = NULL;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    LogicalConfig* config = NULL;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat rawstat;
    ywb_uint32_t factors = 0;
    ywb_uint32_t metafactors[6] = {90, 50, 10, 40, 130, 20};
    ywb_uint32_t metafactors2[6][6] = {
            {90, 50, 60, 40, 130, 30},/*400*/
            {60, 50, 10, 40, 30, 10},/*200*/
            {50, 150, 80, 90, 110, 120},/*600*/
            {30, 40, 10, 50, 80, 90},/*300*/
            {100, 20, 60, 80, 50, 90},/*400*/
            {150, 40, 90, 70, 60, 90},/*500*/
    };

    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    perfmgr->Init();

    /*=========config tier 0 on SubPool: Constant::SUBPOOL_ID start=========*/
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID + 1000;
    inodeid = Constant::DEFAULT_INODE + 10000;

    diskprop.SetDiskClass(DiskBaseProp::DCT_ssd);
    diskprop.SetDiskRPM(15000);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);

    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(diskid);
    disk = new Disk(diskprop);
    YWB_ASSERT(disk != NULL);
    disk->DetermineDiskTier();
    ret = config->AddDisk(diskkey, disk);
    YWB_ASSERT(YWB_SUCCESS == ret);

    objkey.SetStorageId(diskkey.GetDiskId());
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    for(objloop = 0; objloop < 6; objloop++, inodeid++)
    {
        factors = metafactors[objloop];
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
    /*=========config tier 0 on SubPool: Constant::SUBPOOL_ID end=========*/

    /*=======config tier 1 on SubPool: Constant::SUBPOOL_ID+1 start=======*/
    subpoolid = Constant::SUBPOOL_ID + 1;
    diskid = Constant::DISK_ID + 1100;
    inodeid = Constant::DEFAULT_INODE + 11000;

    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskprop.SetDiskRPM(10000);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);

    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    /*add 6 disks on tier 1*/
    for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
    {
        diskkey.SetSubPoolId(subpoolid);
        diskkey.SetDiskId(diskid);
        disk = new Disk(diskprop);
        YWB_ASSERT(disk != NULL);
        disk->DetermineDiskTier();
        ret = config->AddDisk(diskkey, disk);
        YWB_ASSERT(YWB_SUCCESS == ret);

        objkey.SetStorageId(diskkey.GetDiskId());
        for(objloop = 0; objloop < 6; objloop++, inodeid++)
        {
            factors = metafactors2[diskloop][objloop];
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
    /*=======config tier 1 on SubPool: Constant::SUBPOOL_ID+1 end=======*/

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID + 1);
    YWB_ASSERT(subpoolavailperf != NULL);
    ret = subpoolavailperf->SetupTierDiskAvailPerfRanking(Tier::TIER_1,
            DiskAvailPerfRankingCmp::DiskAvailPerfRankingGreater);
    YWB_ASSERT(YWB_SUCCESS == ret);
}

void TierDiskAvailPerfRankingTest::TearDown()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint32_t diskid = 0;
    DiskKey diskkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfManager* perfmgr = NULL;
    Disk* disk = NULL;

    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    perfmgr->Reset();

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID + 1000;
    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(diskid);
    ret = config->RemoveDisk(diskkey);
    YWB_ASSERT(YWB_SUCCESS == ret);

    subpoolid = Constant::SUBPOOL_ID + 1;
    diskid = Constant::DISK_ID + 1100;
    diskkey.SetSubPoolId(subpoolid);
    for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
    {
        diskkey.SetDiskId(diskid);
        ret = config->GetDisk(diskkey, &disk);
        if((YWB_SUCCESS == ret) && (disk != NULL))
        {
            ret = config->RemoveDisk(diskkey);
            YWB_ASSERT(YWB_SUCCESS == ret);
        }
    }
}

TierDiskAvailPerfRanking*
TierDiskAvailPerfRankingTest::GetTierAvailPerf(
        ywb_uint32_t subpoolid, ywb_uint32_t tier)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperfranking = NULL;
    TierDiskAvailPerfRanking* tierperfranking = NULL;

    ast = gAst;
    ast->GetPerfManager(&perfmgr);
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    ret = perfmgr->GetSubPoolDiskAvailPerfRanking(
            subpoolkey, &subpoolperfranking);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    subpoolperfranking->GetTierDiskAvailPerfRanking(
            tier, &tierperfranking);

    return tierperfranking;
}

SubPoolDiskAvailPerfRanking*
TierDiskAvailPerfRankingTest::GetSubPoolAvailPerf(
        ywb_uint32_t subpoolid)
{
    SubPoolKey subpoolkey;
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperfranking = NULL;

    ast = gAst;
    ast->GetPerfManager(&perfmgr);
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    perfmgr->GetSubPoolDiskAvailPerfRanking(
            subpoolkey, &subpoolperfranking);

    return subpoolperfranking;
}

void SubPoolDiskAvailPerfRankingTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    AST* ast = NULL;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    LogicalConfig* config = NULL;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat rawstat;
    ywb_uint32_t factors = 0;
    ywb_uint32_t metafactors[6][6] = {
            {90, 50, 60, 40, 130, 30},/*400*/
            {60, 50, 10, 40, 30, 10},/*200*/
            {50, 150, 80, 90, 110, 120},/*600*/
            {30, 40, 10, 50, 80, 90},/*300*/
            {100, 20, 60, 80, 50, 90},/*400*/
            {150, 40, 90, 70, 60, 90},/*500*/
    };

    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    perfmgr->Init();

    /*=======config tier 1 on SubPool: Constant::SUBPOOL_ID+1 start=======*/
    subpoolid = Constant::SUBPOOL_ID + 1;
    diskid = Constant::DISK_ID + 1100;
    inodeid = Constant::DEFAULT_INODE + 11000;

    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskprop.SetDiskRPM(10000);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);

    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    /*add 6 disks on tier 1*/
    for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
    {
        diskkey.SetSubPoolId(subpoolid);
        diskkey.SetDiskId(diskid);
        disk = new Disk(diskprop);
        YWB_ASSERT(disk != NULL);
        disk->DetermineDiskTier();
        ret = config->AddDisk(diskkey, disk);
        YWB_ASSERT(YWB_SUCCESS == ret);

        objkey.SetStorageId(diskkey.GetDiskId());
        for(objloop = 0; objloop < 6; objloop++, inodeid++)
        {
            factors = metafactors[diskloop][objloop];
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
    /*=======config tier 1 on SubPool: Constant::SUBPOOL_ID+1 end=======*/

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID + 1);
    YWB_ASSERT(subpoolavailperf != NULL);
    ret = subpoolavailperf->SetupTierDiskAvailPerfRanking(Tier::TIER_1,
            DiskAvailPerfRankingCmp::DiskAvailPerfRankingGreater);
    YWB_ASSERT(YWB_SUCCESS == ret);
}

void SubPoolDiskAvailPerfRankingTest::TearDown()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint32_t diskid = 0;
    DiskKey diskkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfManager* perfmgr = NULL;
    Disk* disk = NULL;

    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    perfmgr->Reset();

    subpoolid = Constant::SUBPOOL_ID + 1;
    diskid = Constant::DISK_ID + 1100;
    diskkey.SetSubPoolId(subpoolid);
    for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
    {
        diskkey.SetDiskId(diskid);
        ret = config->GetDisk(diskkey, &disk);
        if((YWB_SUCCESS == ret) && (disk != NULL))
        {
            ret = config->RemoveDisk(diskkey);
            YWB_ASSERT(YWB_SUCCESS == ret);
        }
    }
}

SubPoolDiskAvailPerfRanking*
SubPoolDiskAvailPerfRankingTest::GetSubPoolAvailPerf(
        ywb_uint32_t subpoolid)
{
    SubPoolKey subpoolkey;
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperfranking = NULL;

    ast = gAst;
    ast->GetPerfManager(&perfmgr);
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    perfmgr->GetSubPoolDiskAvailPerfRanking(
            subpoolkey, &subpoolperfranking);

    return subpoolperfranking;
}

void PerfManagerTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    AST* ast = NULL;
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    LogicalConfig* config = NULL;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat rawstat;
    ywb_uint32_t factors = 0;
    ywb_uint32_t metafactors[6][6] = {
            {90, 50, 60, 40, 130, 30},/*400*/
            {60, 50, 10, 40, 30, 10},/*200*/
            {50, 150, 80, 90, 110, 120},/*600*/
            {30, 40, 10, 50, 80, 90},/*300*/
            {100, 20, 60, 80, 50, 90},/*400*/
            {150, 40, 90, 70, 60, 90},/*500*/
    };

    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    perfmgr->Init();

    /*=======config tier 1 on SubPool: Constant::SUBPOOL_ID+1 start=======*/
    subpoolid = Constant::SUBPOOL_ID + 1;
    diskid = Constant::DISK_ID + 1100;
    inodeid = Constant::DEFAULT_INODE + 11000;

    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskprop.SetDiskRPM(10000);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);

    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    /*add 6 disks on tier 1*/
    for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
    {
        diskkey.SetSubPoolId(subpoolid);
        diskkey.SetDiskId(diskid);
        disk = new Disk(diskprop);
        YWB_ASSERT(disk != NULL);
        disk->DetermineDiskTier();
        ret = config->AddDisk(diskkey, disk);
        YWB_ASSERT(YWB_SUCCESS == ret);

        objkey.SetStorageId(diskkey.GetDiskId());
        for(objloop = 0; objloop < 6; objloop++, inodeid++)
        {
            factors = metafactors[diskloop][objloop];
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
    /*=======config tier 1 on SubPool: Constant::SUBPOOL_ID+1 end=======*/

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    perfmgr->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolavailperf);
    YWB_ASSERT(subpoolavailperf != NULL);
    ret = subpoolavailperf->SetupTierDiskAvailPerfRanking(Tier::TIER_1,
            DiskAvailPerfRankingCmp::DiskAvailPerfRankingGreater);
    YWB_ASSERT(YWB_SUCCESS == ret);
}

void PerfManagerTest::TearDown()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint32_t diskid = 0;
    DiskKey diskkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfManager* perfmgr = NULL;
    Disk* disk = NULL;

    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmgr);
    perfmgr->Reset();

    subpoolid = Constant::SUBPOOL_ID + 1;
    diskid = Constant::DISK_ID + 1100;
    diskkey.SetSubPoolId(subpoolid);
    for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
    {
        diskkey.SetDiskId(diskid);
        ret = config->GetDisk(diskkey, &disk);
        if((YWB_SUCCESS == ret) && (disk != NULL))
        {
            ret = config->RemoveDisk(diskkey);
            YWB_ASSERT(YWB_SUCCESS == ret);
        }
    }
}

PerfManager* PerfManagerTest::GetPerfManager()
{
    AST* ast = NULL;
    PerfManager* perfmgr = NULL;

    ast = gAst;
    ast->GetPerfManager(&perfmgr);

    return perfmgr;
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

    testing::AddGlobalTestEnvironment(new ASTDiskPerfTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
