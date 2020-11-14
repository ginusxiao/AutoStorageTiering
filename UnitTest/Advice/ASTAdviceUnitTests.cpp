#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/Advice/ASTAdviceUnitTests.hpp"

void AdviceSetTest::SetUp()
{
    ywb_uint32_t adviceloop = 0;
    ywb_uint32_t factors = 0;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;
    ChunkIOStat rawstat;
    TierBasedAdviceSet* adviceset = NULL;
    Advice* advice = NULL;
    ywb_uint32_t metafactors2[Constant::DEFAULT_ADVISE_NUM] =
        {10, 13, 9, 8, 11, 15};
    ywb_uint32_t metafactors1[Constant::DEFAULT_ADVISE_NUM] =
        {7, 4, 5, 8, 3, 6};
    ywb_uint32_t metafactors0[Constant::DEFAULT_ADVISE_NUM] =
        {1, 6, 4, 2, 5, 3};

    adviceset = mAdviceSet;
    inodeid = Constant::DEFAULT_INODE;
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);
    /*simulate advise of tier2, where exactly Constant::DEFAULT_ADVISE_NUM advise*/
    for(adviceloop = 0; adviceloop < Constant::DEFAULT_ADVISE_NUM; adviceloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        factors = metafactors2[inodeid];
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

        advice = new Advice(objkey, 2, 1, rawstat,
                IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
        YWB_ASSERT(advice != NULL);
        adviceset->AddAdvice(2, advice);
    }

    objkey.SetStorageId(Constant::DISK_ID + 2);
    inodeid = Constant::DEFAULT_INODE + (2 * Constant::DEFAULT_ADVISE_NUM);
    /*simulate advise of tier0, where only Constant::DEFAULT_ADVISE_NUM - 2 advise*/
    for(adviceloop = 0; adviceloop < Constant::DEFAULT_ADVISE_NUM - 2; adviceloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        factors = metafactors0[inodeid - Constant::DEFAULT_ADVISE_NUM];
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

        advice = new Advice(objkey, 0, 1, rawstat,
                IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
        YWB_ASSERT(advice != NULL);
        adviceset->AddAdvice(0, advice);
    }

    adviceset = mAdviceSet2;
    inodeid = Constant::DEFAULT_INODE + (3 * Constant::DEFAULT_ADVISE_NUM);
    objkey.SetStorageId(Constant::DISK_ID + 3);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);
    for(adviceloop = 0; adviceloop < Constant::DEFAULT_ADVISE_NUM; adviceloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        factors = metafactors1[inodeid - 3 * Constant::DEFAULT_ADVISE_NUM];
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

        advice = new Advice(objkey, 1, 0, rawstat,
                IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
        YWB_ASSERT(advice != NULL);
        adviceset->AddAdvice(1, advice);
    }
}

void AdviceSetTest::TearDown()
{
    TierBasedAdviceSet* adviceset = NULL;

    adviceset = mAdviceSet;
    adviceset->Destroy();
}

TierBasedAdviceSet* AdviceSetTest::GetAdviceSet()
{
    return mAdviceSet;
}

TierBasedAdviceSet* AdviceSetTest::GetAdviceSet2()
{
    return mAdviceSet2;
}

TierBasedAdviceSet* AdviceSetTest::GetAdviceSet3()
{
    return mAdviceSet3;
}

SubPoolAdvice* SubPoolAdviceTest::GetSubPoolAdvice()
{
    return mSubPoolAdvice;
}

void SubPoolAdviceTest::SetUp()
{
    ywb_uint32_t adviceloop = 0;
    ywb_uint32_t factors = 0;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;
    ChunkIOStat rawstat;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;
    ywb_uint32_t hotmetafactors2[Constant::DEFAULT_ADVISE_NUM] =
        {17, 14, 25, 18, 23, 16};
    ywb_uint32_t hotmetafactors1[Constant::DEFAULT_ADVISE_NUM] =
        {10, 13, 9, 8, 11, 15};
    ywb_uint32_t coldmetafactors0[Constant::DEFAULT_ADVISE_NUM] =
        {7, 4, 5, 8, 3, 6};
    ywb_uint32_t coldmetafactors1[Constant::DEFAULT_ADVISE_NUM] =
        {1, 6, 4, 2, 5, 3};

    subpooladvice = mSubPoolAdvice;
    /*simulate hot advice of tier 2, exactly Constant::DEFAULT_ADVISE_NUM advise*/
    inodeid = Constant::DEFAULT_INODE;
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);
    for(adviceloop = 0; adviceloop < Constant::DEFAULT_ADVISE_NUM; adviceloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        factors = hotmetafactors2[inodeid];
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

        advice = new Advice(objkey, 2, 1, rawstat,
                IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
        YWB_ASSERT(advice != NULL);
        subpooladvice->AddAdvice(Tier::TIER_2, AdviceType::AT_ctr_hot, advice);
    }

    /*simulate hot advise of tier 1, where only Constant::DEFAULT_ADVISE_NUM - 2 advise*/
    objkey.SetStorageId(Constant::DISK_ID + 1);
    inodeid = Constant::DEFAULT_INODE + (1 * Constant::DEFAULT_ADVISE_NUM);
    for(adviceloop = 0; adviceloop < Constant::DEFAULT_ADVISE_NUM - 2; adviceloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        factors = hotmetafactors1[inodeid - (1 * Constant::DEFAULT_ADVISE_NUM)];
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

        advice = new Advice(objkey, 0, 1, rawstat,
                IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
        YWB_ASSERT(advice != NULL);
        subpooladvice->AddAdvice(Tier::TIER_1, AdviceType::AT_ctr_hot, advice);
    }

    /*simulate cold advice of tier 1, only Constant::DEFAULT_ADVISE_NUM - 2 advise*/
    inodeid = Constant::DEFAULT_INODE + (2 * Constant::DEFAULT_ADVISE_NUM);
    objkey.SetStorageId(Constant::DISK_ID + 2);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);
    for(adviceloop = 0; adviceloop < Constant::DEFAULT_ADVISE_NUM - 2; adviceloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        factors = coldmetafactors1[inodeid - (2 * Constant::DEFAULT_ADVISE_NUM)];
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

        advice = new Advice(objkey, 1, 0, rawstat,
                IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
        YWB_ASSERT(advice != NULL);
        subpooladvice->AddAdvice(Tier::TIER_1, AdviceType::AT_ctr_cold, advice);
    }

    /*simulate cold advice of tier 0, exactly Constant::DEFAULT_ADVISE_NUM advise*/
    inodeid = Constant::DEFAULT_INODE + (3 * Constant::DEFAULT_ADVISE_NUM);
    objkey.SetStorageId(Constant::DISK_ID + 3);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);
    for(adviceloop = 0; adviceloop < Constant::DEFAULT_ADVISE_NUM; adviceloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        factors = coldmetafactors0[inodeid - (3 * Constant::DEFAULT_ADVISE_NUM)];
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

        advice = new Advice(objkey, 1, 0, rawstat,
                IOStat::IOST_raw, /*Constant::DEFAULT_OBJ_SIZE*/1);
        YWB_ASSERT(advice != NULL);
        subpooladvice->AddAdvice(Tier::TIER_0, AdviceType::AT_ctr_cold, advice);
    }
}

void SubPoolAdviceTest::TearDown()
{
    SubPoolAdvice* subpooladvice = NULL;

    subpooladvice = GetSubPoolAdvice();
    subpooladvice->Destroy();
}

void AdviceManagerTest::SetUp()
{
    LogicalConfig* config = NULL;
    HeatDistributionManager* heatmgr = NULL;
    DiskUltimatePerfCache* ultimateperfmgr = NULL;
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
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat rawstat;
    ywb_uint32_t factors = 0;
    /*totally 7(SubPool::TC_cnt) kinds of subpools*/
    ywb_uint32_t diskbitmap[7][3] = {
            {1, 1, 1},/*ssd + ent + sata*/
            {1, 1, 0},/*ssd + ent*/
            {1, 0, 1},/*ssd + sata*/
            {0, 1, 1},/*ent + sata*/
            {1, 0, 0},/*ssd*/
            {0, 1, 0},/*ent*/
            {0, 0, 1},/*sata*/
    };
    ywb_uint32_t rndmetafactor[6] =
            {10, 20, 30, 40, 50, 60};
    ywb_uint32_t seqmetafactor[6] =
            {60, 50, 20, 10, 40, 30};
    ywb_uint64_t ultimatereadbw = 0;
    ywb_uint64_t ultimatewritebw = 0;
    ywb_uint64_t ultimatereadios = 0;
    ywb_uint64_t ultimatewriteios = 0;

    mAst->GetLogicalConfig(&config);
    mAst->GetHeatDistributionManager(&heatmgr);
    mAst->GetDiskUltimatePerfCache(&ultimateperfmgr);

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++, subpoolid++)
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

        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            if(0 == diskbitmap[subpoolloop][diskloop])
            {
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    ;
                }

                continue;
            }

            if(0 == diskloop)
            {
                diskprop.SetDiskClass(DiskBaseProp::DCT_ssd);
                ultimatereadbw = (Constant::DEFAULT_SSD_BW * 100);
                ultimatewritebw = (Constant::DEFAULT_SSD_BW * 100);
                ultimatereadios = (Constant::DEFAULT_SSD_IOPS * 100);
                ultimatewriteios = (Constant::DEFAULT_SSD_IOPS * 100);
            }
            else if(1 == diskloop)
            {
                diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
                ultimatereadbw = (Constant::DEFAULT_ENT_BW * 100);
                ultimatewritebw = (Constant::DEFAULT_ENT_BW * 100);
                ultimatereadios = (Constant::DEFAULT_ENT_IOPS * 100);
                ultimatewriteios = (Constant::DEFAULT_ENT_IOPS * 100);
            }
            else if(2 == diskloop)
            {
                diskprop.SetDiskClass(DiskBaseProp::DCT_sata);
                ultimatereadbw = (Constant::DEFAULT_SATA_BW * 100);
                ultimatewritebw = (Constant::DEFAULT_SATA_BW * 100);
                ultimatereadios = (Constant::DEFAULT_SATA_IOPS * 100);
                ultimatewriteios = (Constant::DEFAULT_SATA_IOPS * 100);
            }

            /*
             * make sure the DiskUltimatePerfCache will use ultimate disk perf
             * set by ourselves here
             * */
            ultimateperfmgr->SetUltimatePerfForTest(diskprop, ultimatereadbw,
                    ultimatewritebw, ultimatereadios, ultimatewriteios);
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
                factors = rndmetafactor[objloop];
                rawstat.SetRndReadIOs(factors);
                rawstat.SetRndReadBW(factors * (Constant::DEFAULT_IOSIZE));
                rawstat.SetRndWriteIOs(factors);
                rawstat.SetRndWriteBW(factors * (Constant::DEFAULT_IOSIZE));
                rawstat.SetRndTotalRT((factors * (Constant::META_RT_READ)) +
                        (factors * (Constant::META_RT_WRITE)));

                factors = seqmetafactor[objloop];
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
    }

    heatmgr->Init();
    mAdviceMgr->Init();
}

void AdviceManagerTest::TearDown()
{

}

AST* AdviceManagerTest::GetAST()
{
    return mAst;
}

AdviceManager* AdviceManagerTest::GetAdviceManager()
{
    return mAdviceMgr;
}

HeatBucket* AdviceManagerTest::GetHeatBucket(SubPoolKey& subpoolkey,
        ywb_uint32_t tier, AdviceRule advicerule)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    HeatDistributionManager* heatmgr = NULL;
    SubPoolHeatDistribution* subpoolheat = NULL;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* heatbucket = NULL;

    ast = mAst;
    ast->GetHeatDistributionManager(&heatmgr);
    ret = heatmgr->GetSubPoolHeatDistribution(subpoolkey, &subpoolheat);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    ret = subpoolheat->GetTierWiseHeatDistribution(tier, &tierheat);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }

    ret = tierheat->GetHeatBucket(advicerule, &heatbucket);
    if(YWB_SUCCESS != ret)
    {
        return NULL;
    }
    else
    {
        return heatbucket;
    }
}

void AdviceManagerTest::BuildAdviceRule(
        AdviceRule& rule, ywb_uint32_t advicetype,
        ywb_uint32_t partitionscope, ywb_uint32_t partitionbase,
        ywb_uint32_t sortbase, ywb_uint32_t selector)
{
    rule.SetAdviceType(advicetype);
    rule.SetPartitionScope(partitionscope);
    rule.SetPartitionBase(partitionbase);
    rule.SetSortBase(sortbase);
    rule.SetSelector(selector);
}

void AdviceManagerTest::BuildDefaultHotAdviceRule(AdviceRule& rule)
{
    rule.SetAdviceType(AdviceType::AT_ctr_hot);
    rule.SetPartitionScope(PartitionScope::PS_tier_wise);
    rule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    rule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    rule.SetSelector(Selector::SEL_from_larger);
}

void AdviceManagerTest::BuildDefaultColdAdviceRule(AdviceRule& rule)
{
    rule.SetAdviceType(AdviceType::AT_ctr_cold);
    rule.SetPartitionScope(PartitionScope::PS_tier_wise);
    rule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_sequential | PartitionBase::PB_iops);
    rule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_sequential | SortBase::SB_iops);
    rule.SetSelector(Selector::SEL_from_smaller);
}

void AdviceManagerTest::BuildDefaultHotFilterRule(AdviceRule& rule)
{
    rule.SetAdviceType(AdviceType::AT_ctr_hot);
    rule.SetPartitionScope(PartitionScope::PS_tier_wise);
    rule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_sequential | PartitionBase::PB_iops);
    rule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_sequential | SortBase::SB_iops);
    rule.SetSelector(Selector::SEL_from_smaller);
}

ywb_bool_t AdviceManagerTest::CheckHeatBucketAdviceSequence(SubPoolKey& subpoolkey,
        ywb_uint32_t tier, AdviceRule advicerule,
        ywb_uint64_t* sequence, ywb_uint32_t advicenum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = true;
    ywb_uint32_t objnum = 0;
    HeatBucket* heatbucket = NULL;
    OBJ* obj = NULL;
    OBJKey* objkey = NULL;

    heatbucket = GetHeatBucket(subpoolkey, tier, advicerule);
    if(0 == advicenum)
    {
        if(NULL != heatbucket)
        {
            match = false;
        }
    }
    else
    {
        if(NULL != heatbucket)
        {
            if(AdviceType::AT_ctr_hot == advicerule.GetAdviceType())
            {
                ret = heatbucket->GetFirstHotOBJ(&obj);
                while((YWB_SUCCESS == ret) && (obj != NULL))
                {
                    obj->GetOBJKey(&objkey);
                    if(objkey->GetInodeId() != sequence[objnum++])
                    {
                        match = false;
                        break;
                    }

                    ret = heatbucket->GetNextHotOBJ(&obj);
                }
            }
            else if(AdviceType::AT_ctr_cold == advicerule.GetAdviceType())
            {
                ret = heatbucket->GetFirstColdOBJ(&obj);
                while((YWB_SUCCESS == ret) && (obj != NULL))
                {
                    obj->GetOBJKey(&objkey);
                    if(objkey->GetInodeId() != sequence[objnum++])
                    {
                        match = false;
                        break;
                    }

                    ret = heatbucket->GetNextColdOBJ(&obj);
                }
            }
        }
        else
        {
            match = false;
        }
    }

    return match;
}

ywb_bool_t AdviceManagerTest::CheckSubPoolAdviceSequence(
        SubPoolKey& subpoolkey, AdviceRule advicerule,
        ywb_uint64_t* sequence, ywb_uint32_t advicenum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = true;
    ywb_uint32_t objnum = 0;
    AdviceManager* advicemgr = NULL;
    SubPoolAdvice* subpooladvice = NULL;
    Advice* advice = NULL;
    OBJKey objkey;

    advicemgr = GetAdviceManager();
    advicemgr->GetSubPoolAdvice(subpoolkey, &subpooladvice);
    ret = subpooladvice->GetFirstAdvice(advicerule.GetAdviceType(), &advice);
    while((YWB_SUCCESS == ret) && (advice != NULL))
    {
        advice->GetOBJKey(&objkey);
        if(objkey.GetInodeId() != sequence[objnum++])
        {
            match = false;
            break;
        }

        ret = subpooladvice->GetNextAdvice(advicerule.GetAdviceType(), &advice);
    }

    if(objnum != advicenum)
    {
        match = false;
    }

    return match;
}

SubPool* AdviceManagerTest::BuildSubPoolWithoutTier()
{
    SubPoolKey subpoolkey;
    SubPool* subpool = NULL;

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
    subpool = new SubPool(subpoolkey);
    YWB_ASSERT(subpool != NULL);

    return subpool;
}

SubPool* AdviceManagerTest::BuildSubPoolWithTierButNoOBJ()
{
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
    subpool = new SubPool(subpoolkey);
    YWB_ASSERT(subpool != NULL);

    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    disk = new Disk(diskprop);
    YWB_ASSERT(disk != NULL);
    subpool->AddDisk(diskkey, disk);

    return subpool;
}

SubPool* AdviceManagerTest::BuildSubPoolWithTierAndOBJs()
{
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
    subpool = new SubPool(subpoolkey);
    YWB_ASSERT(subpool != NULL);

    diskprop.SetDiskClass(DiskBaseProp::DCT_ssd);
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    disk = new Disk(diskprop);
    YWB_ASSERT(disk != NULL);
    subpool->AddDisk(diskkey, disk);

    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    disk = new Disk(diskprop);
    YWB_ASSERT(disk != NULL);
    subpool->AddDisk(diskkey, disk);

    diskprop.SetDiskClass(DiskBaseProp::DCT_sata);
    diskkey.SetDiskId(Constant::DISK_ID + 2);
    disk = new Disk(diskprop);
    YWB_ASSERT(disk != NULL);
    subpool->AddDisk(diskkey, disk);

    diskprop.SetDiskClass(DiskBaseProp::DCT_sata);
    diskkey.SetDiskId(Constant::DISK_ID + 3);
    disk = new Disk(diskprop);
    YWB_ASSERT(disk != NULL);
    subpool->AddDisk(diskkey, disk);

    objkey.SetStorageId(Constant::DISK_ID + 3);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    obj = new OBJ(objkey);
    YWB_ASSERT(obj != NULL);
    disk->AddOBJ(objkey, obj);

    return subpool;
}

void AdviceManagerTest::DestroySubPool(SubPool* subpool)
{
    YWB_ASSERT(subpool != NULL);
    delete subpool;
    subpool = NULL;
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

    testing::AddGlobalTestEnvironment(new ASTAdviceTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
