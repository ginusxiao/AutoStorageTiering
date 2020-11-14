#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/Arb/ASTArbUnitTests.hpp"

//AST* gAst = NULL;

void ASTArbTestEnvironment::SetUp()
{
#if 0
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subdiskperfloop = 0;
    ywb_uint32_t diskentryloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    ywb_uint32_t factors = 0;
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    ChunkIOStat rawstat;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfProfileManager* profilemgr = NULL;
    PerfManager* perfmgr = NULL;
    Arbitrator* arb = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    DiskPerfProfile* diskperfprofile = NULL;
    SubDiskProfile* subdiskprofile = NULL;
    DiskProfileEntry* diskprofileentry = NULL;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};
    ywb_uint32_t diskrpm[3] = {15000, 10000, 7500};
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

    gAst = GetAST();
    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetPerformanceProfile(&profilemgr);
    ast->GetPerfManager(&perfmgr);
    ast->GetArb(&arb);
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

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
            diskprop.SetDiskClass(diskclass[diskloop % ((sizeof(diskclass) / sizeof(ywb_uint32_t)))]);
            diskprop.SetDiskRPM(diskrpm[diskloop % ((sizeof(diskrpm) / sizeof(ywb_uint32_t)))]);
            diskkey.SetDiskId(diskid);
            disk = new Disk(diskprop);
            YWB_ASSERT(disk != NULL);
            config->AddDisk(diskkey, disk);

            objkey.SetStorageId(diskid);
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
    }

    /*init performance module*/
    perfmgr->Init();
#endif
}

void ASTArbTestEnvironment::TearDown()
{
#if 0
    if(gAst)
    {
        delete gAst;
        gAst = NULL;
    }
#endif
}

void SubPoolArbitrateeTest::SetUp()
{
    ywb_uint32_t arbitrateeloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    ywb_uint32_t factors = 0;
    SubPoolKey subpoolkey;
    ChunkIOStat rawstat;
    SubPoolArbitratee* subpoolarbitratee = NULL;
    ArbitrateeKey arbitrateekey;
    Arbitratee* arbitratee = NULL;

    subpoolarbitratee = mSubPoolArbitratee;
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);

    arbitrateekey.SetStorageId(diskid);
    arbitrateekey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    arbitrateekey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    arbitrateekey.SetChunkVersion(1);

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

    for(arbitrateeloop = 0; arbitrateeloop < 6; arbitrateeloop++, inodeid++)
    {
        arbitrateekey.SetInodeId(inodeid);
        arbitratee = new Arbitratee(arbitrateekey, diskid + 1,
                subpoolkey, rawstat, IOStat::IOST_raw,
                Plan::PD_ctr_demote, Constant::DEFAULT_OBJ_CAP, 100, 101);
        YWB_ASSERT(arbitratee != NULL);
        subpoolarbitratee->AddArbitratee(arbitrateekey, arbitratee);
    }
}

void SubPoolArbitrateeTest::TearDown()
{
    ywb_status_t ret = YWB_SUCCESS;
    ArbitrateeKey arbitrateekey;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;

    subpoolarbitratees = mSubPoolArbitratee;
    ret = subpoolarbitratees->GetFirstArbitratee(&arbitratee);
    while((YWB_SUCCESS == ret) && (arbitratee != NULL))
    {
        arbitratee->GetOBJKey(&objkey);
        arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);
        subpoolarbitratees->RemoveArbitratee(arbitrateekey);
        delete arbitratee;
        arbitratee = NULL;

        ret = subpoolarbitratees->GetNextArbitratee(&arbitratee);
    }
}

void SubPoolArbitrateeTest::SetSubPoolKey(SubPoolKey& subpoolkey,
        ywb_uint32_t oss, ywb_uint32_t pool, ywb_uint32_t subpool)
{
    subpoolkey.SetOss(oss);
    subpoolkey.SetPoolId(pool);
    subpoolkey.SetSubPoolId(subpool);
}

void SubPoolArbitrateeTest::SetArbitrateeKey(ArbitrateeKey& arbitrateekey,
        ywb_uint64_t disk, ywb_uint64_t inode, ywb_uint64_t chunk)
{
    arbitrateekey.SetStorageId(disk);
    arbitrateekey.SetInodeId(inode);
    arbitrateekey.SetChunkId(chunk);
    arbitrateekey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    arbitrateekey.SetChunkVersion(1);
}

SubPoolArbitratee*
SubPoolArbitrateeTest::GetSubPoolArbitratee()
{
    return mSubPoolArbitratee;
}

void ArbitratorTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subdiskperfloop = 0;
    ywb_uint32_t diskentryloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    ywb_uint32_t factors = 0;
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    ArbitrateeKey arbitrateekey;
    ChunkIOStat rawstat;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfProfileManager* profilemgr = NULL;
    AdviceManager* advicemgr = NULL;
    PerfManager* perfmgr = NULL;
    PlanManager* planmgr = NULL;
    Arbitrator* arb = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* rawstat2;
    Arbitratee* arbitratee = NULL;
    DiskPerfProfile* diskperfprofile = NULL;
    SubDiskProfile* subdiskprofile = NULL;
    DiskProfileEntry* diskprofileentry = NULL;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};
    ywb_uint32_t diskrpm[3] = {15000, 10000, 7500};
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

    ast = GetAST();
    ast->GetLogicalConfig(&config);
    ast->GetPerformanceProfile(&profilemgr);
    ast->GetAdviseManager(&advicemgr);
    ast->GetPerfManager(&perfmgr);
    ast->GetPlanManager(&planmgr);
    ast->GetArb(&arb);
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);
    arbitrateekey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    arbitrateekey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    arbitrateekey.SetChunkVersion(1);

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
            diskprop.SetDiskClass(diskclass[diskloop % ((sizeof(diskclass) / sizeof(ywb_uint32_t)))]);
            diskprop.SetDiskRPM(diskrpm[diskloop % ((sizeof(diskrpm) / sizeof(ywb_uint32_t)))]);
            diskkey.SetDiskId(diskid);
            disk = new Disk(diskprop);
            YWB_ASSERT(disk != NULL);
            config->AddDisk(diskkey, disk);

            objkey.SetStorageId(diskid);
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
    }

    advicemgr->Init();
    /*init performance module*/
    perfmgr->Init();
    /*init plan module*/
    planmgr->Init();

    /*generate advise*/
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            if((1 == diskloop))
            {
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    objkey.SetStorageId(diskid);
                    objkey.SetInodeId(inodeid);
                    arbitrateekey.SetStorageId(diskid);
                    arbitrateekey.SetInodeId(inodeid);

                    config->GetOBJ(objkey, &obj);
                    obj->GetOBJIO(&objio);
                    objio->GetIOStat(IOStat::IOST_raw, &rawstat2);

                    if(objloop <= 3)
                    {
                        arbitratee = new Arbitratee(arbitrateekey, diskid + 1,
                                subpoolkey, *rawstat2, IOStat::IOST_raw,
                                Plan::PD_ctr_demote, Constant::DEFAULT_OBJ_CAP, 100, 101);
                        YWB_ASSERT(arbitratee != NULL);
                        ret = arb->AddArbitrateeIn(subpoolkey, arbitrateekey, arbitratee);
                        YWB_ASSERT(YWB_SUCCESS == ret);
                    }

                    /*arbitratee with objloop == 3 will be both in IN and OUT*/
                    if(objloop >= 3)
                    {
                        arbitratee = new Arbitratee(arbitrateekey, diskid - 1,
                                subpoolkey, *rawstat2, IOStat::IOST_raw,
                                Plan::PD_ctr_promote, Constant::DEFAULT_OBJ_CAP, 100, 101);
                        YWB_ASSERT(arbitratee != NULL);
                        ret = arb->AddArbitrateeOut(subpoolkey, arbitrateekey, arbitratee);
                        YWB_ASSERT(YWB_SUCCESS == ret);
                    }
                }
            }
            else if(((2 == subpoolloop)) && (2 == diskloop))
            {
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    objkey.SetStorageId(diskid);
                    objkey.SetInodeId(inodeid);
                    arbitrateekey.SetStorageId(diskid);
                    arbitrateekey.SetInodeId(inodeid);

                    config->GetOBJ(objkey, &obj);
                    obj->GetOBJIO(&objio);
                    objio->GetIOStat(IOStat::IOST_raw, &rawstat2);

                    arbitratee = new Arbitratee(arbitrateekey, diskid - 1,
                            subpoolkey, *rawstat2, IOStat::IOST_raw,
                            Plan::PD_ctr_promote, Constant::DEFAULT_OBJ_CAP, 100, 101);
                    YWB_ASSERT(arbitratee != NULL);
                    ret = arb->AddArbitrateeIn(subpoolkey, arbitrateekey, arbitratee);
                    YWB_ASSERT(YWB_SUCCESS == ret);
                }
            }
            else if(((2 == subpoolloop)) && (0 == diskloop))
            {
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    objkey.SetStorageId(diskid);
                    objkey.SetInodeId(inodeid);
                    arbitrateekey.SetStorageId(diskid);
                    arbitrateekey.SetInodeId(inodeid);

                    config->GetOBJ(objkey, &obj);
                    obj->GetOBJIO(&objio);
                    objio->GetIOStat(IOStat::IOST_raw, &rawstat2);

                    arbitratee = new Arbitratee(arbitrateekey, diskid + 1,
                            subpoolkey, *rawstat2, IOStat::IOST_raw,
                            Plan::PD_ctr_demote, Constant::DEFAULT_OBJ_CAP, 100, 101);
                    YWB_ASSERT(arbitratee != NULL);
                    ret = arb->AddArbitrateeOut(subpoolkey, arbitrateekey, arbitratee);
                    YWB_ASSERT(YWB_SUCCESS == ret);
                }
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
}

void ArbitratorTest::TearDown()
{

}

void ArbitratorTest::SetSubPoolKey(SubPoolKey& subpoolkey,
        ywb_uint32_t oss, ywb_uint32_t pool, ywb_uint32_t subpool)
{
    subpoolkey.SetOss(oss);
    subpoolkey.SetPoolId(pool);
    subpoolkey.SetSubPoolId(subpool);
}

void ArbitratorTest::SetArbitrateeKey(ArbitrateeKey& arbitrateekey,
        ywb_uint64_t disk, ywb_uint64_t inode, ywb_uint64_t chunk)
{
    arbitrateekey.SetStorageId(disk);
    arbitrateekey.SetInodeId(inode);
    arbitrateekey.SetChunkId(chunk);
    arbitrateekey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    arbitrateekey.SetChunkVersion(1);
}

AST* ArbitratorTest::GetAST()
{
    YWB_ASSERT(mAst != NULL);
    return mAst;
}

Arbitrator* ArbitratorTest::GetArbitrator()
{
    AST* ast = NULL;
    Arbitrator* arb = NULL;

    ast = GetAST();
    ast->GetArb(&arb);
    YWB_ASSERT(arb != NULL);

    return arb;
}

LogicalConfig* ArbitratorTest::GetConfig()
{
    AST* ast = NULL;
    LogicalConfig* config = NULL;

    ast = GetAST();
    ast->GetLogicalConfig(&config);
    YWB_ASSERT(config != NULL);

    return config;
}

TimeManager* ArbitratorTest::GetTimeManager()
{
    AST* ast = NULL;
    TimeManager* timemgr = NULL;

    ast = GetAST();
    ast->GetTimeManager(&timemgr);

    return timemgr;
}

CycleManager* ArbitratorTest::GetCycleManager()
{
    AST* ast = NULL;
    TimeManager* timemgr = NULL;
    CycleManager* cyclemgr = NULL;

    ast = GetAST();
    ast->GetTimeManager(&timemgr);
    timemgr->GetCycleManager(&cyclemgr);

    return cyclemgr;
}

ywb_status_t ArbitratorTest::GetReserved(
        SubPoolKey& subpoolkey, DiskKey& diskkey,
        ywb_uint32_t perftype, ywb_uint32_t* ios,
        ywb_uint32_t* bw, ywb_uint32_t* cap)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    Disk* disk = NULL;
    PerfManager* perfmanager = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    TierDiskAvailPerfRanking* tierperf = NULL;
    DiskAvailPerfRankingUnit* diskunit = NULL;

    *ios = 0;
    *bw = 0;
    *cap = 0;

    ast = GetAST();
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmanager);
    ret = perfmanager->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolperf);
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = config->GetDisk(diskkey, &disk);
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = subpoolperf->GetTierDiskAvailPerfRanking(disk->GetTier(), &tierperf);
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = tierperf->GetDiskAvailPerfRankingUnit(diskkey, perftype, &diskunit);
    YWB_ASSERT(YWB_SUCCESS == ret);

    *ios = diskunit->GetIOReserved();
    *bw = diskunit->GetBWReserved();
    *cap = diskunit->GetCapReserved();

    return ret;
}

ywb_status_t ArbitratorTest::GetConsumed(
        SubPoolKey& subpoolkey, DiskKey& diskkey,
        ywb_uint32_t perftype, ywb_uint32_t* ios,
        ywb_uint32_t* bw, ywb_uint32_t* cap)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    Disk* disk = NULL;
    PerfManager* perfmanager = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    TierDiskAvailPerfRanking* tierperf = NULL;
    DiskAvailPerfRankingUnit* diskunit = NULL;
    DiskPerf* diskperf = NULL;

    *ios = 0;
    *bw = 0;
    *cap = 0;

    ast = GetAST();
    ast->GetLogicalConfig(&config);
    ast->GetPerfManager(&perfmanager);
    ret = perfmanager->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolperf);
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = config->GetDisk(diskkey, &disk);
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = subpoolperf->GetTierDiskAvailPerfRanking(disk->GetTier(), &tierperf);
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = tierperf->GetDiskAvailPerfRankingUnit(diskkey, perftype, &diskunit);
    YWB_ASSERT(YWB_SUCCESS == ret);
    diskunit->GetDiskPerf(&diskperf);

    *ios = diskperf->GetIOConsumed();
    *bw = diskperf->GetBWConsumed();
    *cap = diskperf->GetCapConsumed();

    return ret;
}

ywb_status_t ArbitratorTest::GetArbitrateeFromIn(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey, Arbitratee** arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    arb = GetArbitrator();
    ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, arbitratee);
        if(!(YWB_SUCCESS == ret))
        {
            *arbitratee = NULL;
        }
    }

    return ret;
}

ywb_status_t ArbitratorTest::GetArbitrateeFromOut(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey, Arbitratee** arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    arb = GetArbitrator();
    ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, arbitratee);
        if(!(YWB_SUCCESS == ret))
        {
            *arbitratee = NULL;
        }
    }

    return ret;
}

ywb_status_t ArbitratorTest::CheckDiskPerf(SubPoolKey& subpoolkey,
        ywb_uint64_t tgtdiskid, ywb_uint32_t perftype,
        ywb_uint32_t ioreserved, ywb_uint32_t bwreserved,
        ywb_uint32_t capreserved, ywb_uint32_t ioconsumed,
        ywb_uint32_t bwconsumed, ywb_uint32_t capconsumed)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey tgtdiskkey;
    ywb_uint32_t ios = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t cap = 0;

    tgtdiskkey.SetSubPoolId(subpoolkey.GetSubPoolId());
    tgtdiskkey.SetDiskId(tgtdiskid);
    GetReserved(subpoolkey, tgtdiskkey, IOStat::IOST_raw, &ios, &bw, &cap);
    YWB_ASSERT(ios == ioreserved);
    YWB_ASSERT(bw == bwreserved);
    YWB_ASSERT(cap == capreserved);
    GetConsumed(subpoolkey, tgtdiskkey, IOStat::IOST_raw, &ios, &bw, &cap);
    YWB_ASSERT(ios == ioconsumed);
    YWB_ASSERT(bw == bwconsumed);
    YWB_ASSERT(cap == capconsumed);

    return ret;
}

Plan* ArbitratorTest::GenerateNewPlan(ywb_uint32_t subpoolid,
        ywb_uint64_t srcdiskid, ywb_uint64_t tgtdiskid,
        ywb_uint64_t inodeid, ywb_uint32_t cap,
        ywb_uint32_t direction, ywb_uint64_t epoch)
{
    ywb_uint32_t factors = 0;
    SubPoolKey subpoolkey;
    OBJKey objkey;
    ChunkIOStat rawstat;
    Plan* plan = NULL;

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);

    objkey.SetStorageId(srcdiskid);
    objkey.SetInodeId(inodeid);
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

    plan = new Plan(objkey, tgtdiskid, subpoolkey, rawstat,
            IOStat::IOST_raw, cap, direction, epoch);
    YWB_ASSERT(plan != NULL);

    return plan;
}

void ArbitratorTest::DestroyPlan(Plan* plan)
{
    if(plan)
    {
        delete plan;
        plan = NULL;
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

    testing::AddGlobalTestEnvironment(new ASTArbTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
