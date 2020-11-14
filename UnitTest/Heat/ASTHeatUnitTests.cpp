#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/Heat/ASTHeatUnitTests.hpp"

AST* gAst = NULL;

/*
 * NOTE:
 * DO REMEMBER to modify all the test suites if modify this routine.
 **/
void ASTHeatTestEnvironment::SetUp()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    TimeManager* timemgr = NULL;
    DecisionWindowManager* shortwindowmgr = NULL;
    DecisionWindowManager* longwindowmgr = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat rawstat;
    ywb_uint32_t factors = 0;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};
    ywb_uint32_t cycleshares[Constant::CYCLE_TYPE_CNT] = {0, 0};
    /*NOTE: REMEMBER to update HeatBucketTest::SetUp() if modify @metafactors*/
    ywb_uint32_t metafactors[5][3 * 3][6] =
    {
            /*                              Tier0                   Tier1                   Tier2       */
            {
                    /*Disk ID: 1 - 3*/{9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11},
                    /*Disk ID: 4 - 6*/{12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8},
                    /*Disk ID: 7 - 9*/{6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2},
            },
            {
                    /*Disk ID: 1 - 3*/{9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11},
                    /*Disk ID: 4 - 6*/{12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8},
                    /*Disk ID: 7 - 9*/{6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2},
            },
            {
                    /*Disk ID: 1 - 3*/{9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11},
                    /*Disk ID: 4 - 6*/{12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8},
                    /*Disk ID: 7 - 9*/{6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2},
            },
            {
                    /*Disk ID: 1 - 3*/{9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11},
                    /*Disk ID: 4 - 6*/{12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8},
                    /*Disk ID: 7 - 9*/{6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2},
            },
            {
                    /*Disk ID: 1 - 3*/{9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11},
                    /*Disk ID: 4 - 6*/{12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2}, {6, 14, 7, 10, 0, 8},
                    /*Disk ID: 7 - 9*/{6, 14, 7, 10, 0, 8}, {12, 17, 3, 16, 15, 11}, {9, 5, 1, 4, 13, 2},
            },
    };

    gAst = new AST();
    ast = gAst;
    ast->GetLogicalConfig(&config);
    ast->GetTimeManager(&timemgr);
    timemgr->GetDecisionWindow(DecisionWindowManager::DW_short_term, &shortwindowmgr);
    timemgr->GetDecisionWindow(DecisionWindowManager::DW_long_term, &longwindowmgr);
    cycleshares[0] = 11;
    cycleshares[1] = 1;
    shortwindowmgr->SetCycles(12);
    shortwindowmgr->SetCycleShare(cycleshares, Constant::CYCLE_TYPE_CNT);
    cycleshares[0] = 25;
    cycleshares[1] = 1;
    longwindowmgr->SetCycles(26);
    longwindowmgr->SetCycleShare(cycleshares, Constant::CYCLE_TYPE_CNT);

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        subpool = new SubPool(subpoolkey);
        YWB_ASSERT(subpool != NULL);
        config->AddSubPool(subpoolkey, subpool);

        diskprop.SetDiskRPM(10000);
        diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
        diskprop.SetRaidWidth(1);
        diskprop.SetDiskCap(1280000);
        diskkey.SetSubPoolId(subpoolid);

        for(diskloop = 0; diskloop < (3 * 3); diskloop++, diskid++)
        {
            diskprop.SetDiskClass(diskclass[diskloop % ((sizeof(diskclass) / sizeof(ywb_uint32_t)))]);
            diskkey.SetDiskId(diskid);
            disk = new Disk(diskprop);
            YWB_ASSERT(disk != NULL);
            config->AddDisk(diskkey, disk);

            /*
             * each tier has three disks, two of them have OBJs, but
             * the other one has no OBJ, to be specific:
             * Tier         Disks(loop)     Disks(ID)   Empty one
             *  0           0, 3, 6          1, 4, 7        6
             *  1           1, 4, 7          2, 5, 8        4
             *  2           2, 5, 8          3, 6, 9        2
             **/
            if(!((2 == diskloop) || (4 == diskloop) || (6 == diskloop)))
            {
                objkey.SetStorageId(diskid);
                objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
                objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
                objkey.SetChunkVersion(1);

                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    factors = metafactors[subpoolloop][diskloop][objloop];
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
            }
            else
            {
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    ;
                }
            }

            disk->SummarizeDiskIO();
        }
    }
}

void ASTHeatTestEnvironment::TearDown()
{
    if(gAst)
    {
        delete gAst;
        gAst = NULL;
    }
}

void HeatBucketTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    ywb_uint32_t heatmin = 0;
    ywb_uint32_t heatmax = 0;
    ywb_uint32_t bucketcap = 0;
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t objnumabove = 0;
    ywb_uint32_t objnumbelow = 0;
    SubPoolKey subpoolkey;
    OBJKey objkey;
    LogicalConfig* config = NULL;
    HeatBucket* heatbucket = NULL;
    SubPool* subpool = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;

    config = GetLogicalConfig();
    heatbucket = mHeatBucket;
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;

    /*
     * one subpool has 36 OBJs(three tiers with each has three
     * disks respectively including one empty disk, and each
     * disk has 6 OBJs)
     *
     * here take raw random IOs as heat measure(please
     * refer to ASTHeatTestEnvironment::SetUp()), and
     * the heat corresponding distribution will be:
     * bucket   OBJs Heat(raw random IOs, in OBJ adding order)
     *    0         2, 0, 2, 0,
     *    1         4, 4,
     *    2         8, 6, 8, 6,
     *    3         10, 10,
     *    4         12, 14, 12, 14,
     *    5         16, 16,
     *    6         18, 20, 18, 20,
     *    7         22, 22,
     *    8         26, 24, 26, 24,
     *    9         28, 34, 32, 30, 28, 34, 32, 30,
     *
     * NOTE:
     * 1. @heatmin and @heatmax should be updated as
     * modification of ASTHeatTestEnvironment::SetUp()
     *
     * 2. @heatmin and @heatmax should be updated if change
     *  heat measure(currently use raw random IOs)
     **/
    heatmin = 0;
    heatmax = 34;
    bucketcap = (heatmax - heatmin) / Constant::BUCKET_NUM;

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = config->GetSubPool(subpoolkey, &subpool);
        YWB_ASSERT((YWB_SUCCESS == ret) && (subpool != NULL));
        if(mSubPool != subpool)
        {
            /*nothing to do, only loop index proceeds*/
            for(diskloop = 0; diskloop < (3); diskloop++, diskid++)
            {
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    ;
                }
            }

            continue;
        }

        for(diskloop = 0; diskloop < (3 * 3); diskloop++, diskid++)
        {
            objkey.SetStorageId(diskid);
            objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
            objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
            objkey.SetChunkVersion(1);

            if(!((2 == diskloop) || (4 == diskloop) || (6 == diskloop)))
            {
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    objkey.SetInodeId(inodeid);
                    ret = subpool->GetOBJ(objkey, &obj);
                    YWB_ASSERT((YWB_SUCCESS == ret) && (obj != NULL));
                    obj->GetOBJIO(&objio);
                    YWB_ASSERT(objio != NULL);
                    objio->GetIOStat(IOStat::IOST_raw, &iostat);
                    YWB_ASSERT(iostat != NULL);

                    bucketindex = (iostat->GetRndReadIOs() +
                            iostat->GetRndWriteIOs() - heatmin) / bucketcap;
                    if(bucketindex < 0)
                    {
                        bucketindex = 0;
                    }
                    else if(bucketindex >= Constant::BUCKET_NUM)
                    {
                        bucketindex = Constant::BUCKET_NUM - 1;
                    }

                    heatbucket->AddOBJ(bucketindex, obj);
                    subpool->PutOBJ(obj);
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

    heatbucket->DeterminBucketBoundary();
    /*calculate number of OBJs above and below the boundary*/
    objnumabove = heatbucket->CalculateOBJsAboveBoundary();
    objnumbelow = heatbucket->CalculateOBJsBelowBoundary();
    heatbucket->SetOBJsAboveBoundary(objnumabove);
    heatbucket->SetOBJsBelowBoundary(objnumbelow);
}

void HeatBucketTest::TearDown()
{

}

LogicalConfig* HeatBucketTest::GetLogicalConfig()
{
    LogicalConfig* config = NULL;

    gAst->GetLogicalConfig(&config);
    YWB_ASSERT(config != NULL);
    return config;
}

HeatBucket* HeatBucketTest::GetHeatBucket()
{
    return mHeatBucket;
}

HeatBucket* HeatBucketTest::GetHeatBucket2()
{
    return mHeatBucket2;
}

//HeatDistribution* HeatBucketTest::GetHeatDistribution()
//{
//    return mHeatDistribution;
//}

void HeatBucketTest::PrepareHeatBucket2()
{
    ywb_uint32_t objloop = 0;
    ywb_uint64_t inodeid = 0;
    ywb_uint32_t bucketindex = 0;
    OBJKey objkey;
    HeatBucket* heatbucket = NULL;
    OBJ* obj = NULL;

    heatbucket = mHeatBucket2;
    inodeid = Constant::DEFAULT_INODE;
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    for(objloop = 0; objloop < 20; objloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        /*
         * bucket[0], bucket[6] and bucket[9] has no OBJ
         * and OBJ distribution is:
         * bucket   OBJ's inode(== objloop + 1)
         *  0
         *  1           1, 11
         *  2           2, 12
         *  3           3, 13
         *  4           4, 14
         *  5           5, 15
         *  6
         *  7           7, 17,
         *  8           8, 18
         *  9
         **/
        if(!((5 == objloop) || (8 == objloop) || (9 == objloop) ||
                (15 == objloop) || (18 == objloop) || (19 == objloop)))
        {
            bucketindex = inodeid % (Constant::BUCKET_NUM);
            obj = new OBJ(objkey);
            YWB_ASSERT(obj != NULL);
            heatbucket->AddOBJ(bucketindex, obj);
        }
    }
}


Disk* DiskHeatDistributionTest::GetDisk()
{
    return mDisk;
}

Disk* DiskHeatDistributionTest::GetDisk2()
{
    return mDisk2;
}

DiskHeatDistribution*
DiskHeatDistributionTest::GetDiskHeatDistribution()
{
    return mDiskHeat;
}

DiskHeatDistribution*
DiskHeatDistributionTest::GetDiskHeatDistribution2()
{
    return mDiskHeat2;
}

HeatBucket* DiskHeatDistributionTest::GetHeatBucket()
{
    return mHeatBucket;
}

HeatBucket* DiskHeatDistributionTest::GetHeatBucket2()
{
    return mHeatBucket2;
}

Tier* TierWiseHeatDistributionTest::GetTier()
{
    return mTier;
}

Tier* TierWiseHeatDistributionTest::GetTier2()
{
    return mTier2;
}

TierWiseHeatDistribution*
TierWiseHeatDistributionTest::GetTierWiseHeatDistribution()
{
    return mTierHeat;
}

TierWiseHeatDistribution*
TierWiseHeatDistributionTest::GetTierWiseHeatDistribution2()
{
    return mTierHeat2;
}

HeatBucket* TierWiseHeatDistributionTest::GetHeatBucket()
{
    return mHeatBucket;
}

HeatBucket* TierWiseHeatDistributionTest::GetHeatBucket2()
{
    return mHeatBucket2;
}

OBJ*
TierWiseHeatDistributionTest::GetOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    OBJKey objkey;
    SubPoolKey* subpoolkey = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    mSubPool->GetSubPoolKey(&subpoolkey);
    diskkey.SetSubPoolId(subpoolkey->GetSubPoolId());
    diskkey.SetDiskId(diskid);
    ret = mTier->GetDisk(diskkey, &disk);
    if((YWB_SUCCESS == ret) && (disk != NULL))
    {
        objkey.SetStorageId(diskid);
        objkey.SetInodeId(inodeid);
        objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
        objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
        objkey.SetChunkVersion(1);
        ret = disk->GetOBJ(objkey, &obj);
        if((YWB_SUCCESS == ret) && (obj != NULL))
        {
            return obj;
        }
    }

    return NULL;
}

void TierWiseHeatDistributionTest::RemoveDisk(ywb_uint64_t diskid)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    SubPoolKey* subpoolkey = NULL;
    DiskKey diskkey;

    subpool = mSubPool;
    tier = mTier;
    subpool->GetSubPoolKey(&subpoolkey);
    diskkey.SetSubPoolId(subpoolkey->GetSubPoolId());
    diskkey.SetDiskId(diskid);
    ret = tier->RemoveDisk(diskkey);
    YWB_ASSERT(YWB_SUCCESS == ret);
}

SubPoolHeatDistribution*
SubPoolHeatDistributionTest::GetSubPoolHeat()
{
    return mSubPoolHeat;
}

HeatDistributionManager*
HeatDistributionManagerTest::GetHeatManager()
{
    return mHeatMgr;
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

    testing::AddGlobalTestEnvironment(new ASTHeatTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
