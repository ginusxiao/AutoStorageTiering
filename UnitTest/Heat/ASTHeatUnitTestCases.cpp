#include "UnitTest/AST/Heat/ASTHeatUnitTests.hpp"

TEST_F(HeatBucketTest, CalculateOBJsAboveBoundary_SUCCESS)
{
    ywb_uint32_t abovenum = 0;
    HeatBucket* heatbucket = NULL;

    heatbucket = GetHeatBucket();
    abovenum = heatbucket->CalculateOBJsAboveBoundary();
    ASSERT_EQ(abovenum, 24);
}

TEST_F(HeatBucketTest, CalculateOBJsBelowBoundary_SUCCESS)
{
    ywb_uint32_t belownum = 0;
    HeatBucket* heatbucket = NULL;

    heatbucket = GetHeatBucket();
    belownum = heatbucket->CalculateOBJsBelowBoundary();
    ASSERT_EQ(belownum, 12);
}

TEST_F(HeatBucketTest, GetGreater_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;
    HeatCmp::Greater greater;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    ret = heatbucket->GetGreater(sortbase, &greater);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(HeatBucketTest, GetGreater_YFS_ENOENT)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;
    HeatCmp::Greater greater;

    sortbase = SortBase::SB_raw | SortBase::SB_random;
    heatbucket = GetHeatBucket();
    ret = heatbucket->GetGreater(sortbase, &greater);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(HeatBucketTest, GetLess_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;
    HeatCmp::Less less;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    ret = heatbucket->GetLess(sortbase, &less);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(HeatBucketTest, GetLess_YFS_ENOENT)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;
    HeatCmp::Less less;

    sortbase = SortBase::SB_raw | SortBase::SB_random;
    heatbucket = GetHeatBucket();
    ret = heatbucket->GetLess(sortbase, &less);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(HeatBucketTest, SortAbove_RequiredNumLargerThanTotalAbove_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    ywb_uint32_t totalabove = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    totalabove = heatbucket->GetOBJsAboveBoundary();
    ret = heatbucket->SortAbove(totalabove + 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), totalabove);
    ASSERT_EQ(heatbucket->GetLastSortedBucketAbove(), 4);
}

TEST_F(HeatBucketTest, SortAbove_RequiredNumLessThanTotalAbove_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    ret = heatbucket->SortAbove(16);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 18);
    ASSERT_EQ(heatbucket->GetLastSortedBucketAbove(), 6);
}

TEST_F(HeatBucketTest, SortAbove_RequiredNumIsZero_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    ret = heatbucket->SortAbove(0);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 0);
    ASSERT_EQ(heatbucket->GetLastSortedBucketAbove(),
            (ywb_uint32_t)Constant::BUCKET_NUM);
}

TEST_F(HeatBucketTest, SortBelow_RequiredNumLargerThanTotalBelow_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    ywb_uint32_t totalbelow = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    totalbelow = heatbucket->GetOBJsBelowBoundary();
    ret = heatbucket->SortBelow(totalbelow + 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), totalbelow);
    ASSERT_EQ(heatbucket->GetLastSortedBucketBelow(), 3);
}

TEST_F(HeatBucketTest, SortBelow_RequiredNumLessThanTotalBelow_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    ywb_uint32_t totalbelow = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    totalbelow = heatbucket->GetOBJsBelowBoundary();
    ret = heatbucket->SortBelow(totalbelow - 4);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 10);
    ASSERT_EQ(heatbucket->GetLastSortedBucketBelow(), 2);
}

TEST_F(HeatBucketTest, SortBelow_RequiredNumIsZero_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    ret = heatbucket->SortBelow(0);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 0);
    ASSERT_EQ(heatbucket->GetLastSortedBucketBelow(),
            (ywb_uint32_t)Constant::BUCKET_NUM);
}

TEST_F(HeatBucketTest, ExtendSortAboveBoundary_RequiredNumLargerThanTotalAbove_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    ywb_uint32_t totalabove = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    totalabove = heatbucket->GetOBJsAboveBoundary();
    heatbucket->SortAbove(8);
    heatbucket->ExtendSortAboveBoundary(totalabove + 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), totalabove);
    ASSERT_EQ(heatbucket->GetLastSortedBucketAbove(), 4);
}

TEST_F(HeatBucketTest, ExtendSortAboveBoundary_TargetNumLessThanTotalAboveButLargerThanPrevSorted_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    heatbucket->SortAbove(16);
    ret = heatbucket->ExtendSortAboveBoundary(20);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 20);
    ASSERT_EQ(heatbucket->GetLastSortedBucketAbove(), 5);
}

TEST_F(HeatBucketTest, ExtendSortAboveBoundary_TargetNumEqualToPrevSorted_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    heatbucket->SortAbove(16);
    ret = heatbucket->ExtendSortAboveBoundary(16);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 18);
    ASSERT_EQ(heatbucket->GetLastSortedBucketAbove(), 6);
}

TEST_F(HeatBucketTest, ExtendSortAboveBoundary_TargetNumLessThanPrevSorted_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    heatbucket->SortAbove(16);
    ret = heatbucket->ExtendSortAboveBoundary(14);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 18);
    ASSERT_EQ(heatbucket->GetLastSortedBucketAbove(), 6);
}

TEST_F(HeatBucketTest, ExtendSortAboveBoundary_DirectlyInvoke_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    ywb_uint32_t totalabove = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    totalabove = heatbucket->GetOBJsAboveBoundary();
    heatbucket->ExtendSortAboveBoundary(totalabove + 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), totalabove);
    ASSERT_EQ(heatbucket->GetLastSortedBucketAbove(), 4);
}

TEST_F(HeatBucketTest, ExtendSortBelowBoundary_RequiredNumLargerThanTotalAbove_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    ywb_uint32_t totalbelow = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    totalbelow = heatbucket->GetOBJsBelowBoundary();
    heatbucket->SortBelow(8);
    ret = heatbucket->ExtendSortBelowBoundary(totalbelow + 1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), totalbelow);
    ASSERT_EQ(heatbucket->GetLastSortedBucketBelow(), 3);
}

TEST_F(HeatBucketTest, ExtendSortBelowBoundary_TargetNumLessThanTotalAboveButLargerThanPrevSorted_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    heatbucket->SortBelow(8);
    ret = heatbucket->ExtendSortBelowBoundary(10);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 10);
    ASSERT_EQ(heatbucket->GetLastSortedBucketBelow(), 2);
}

TEST_F(HeatBucketTest, ExtendSortBelowBoundary_TargetNumEqualToPrevSorted_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    heatbucket->SortBelow(8);
    ret = heatbucket->ExtendSortBelowBoundary(8);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 10);
    ASSERT_EQ(heatbucket->GetLastSortedBucketBelow(), 2);
}

TEST_F(HeatBucketTest, ExtendSortBelowBoundary_TargetNumLessThanPrevSorted_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    heatbucket->SortBelow(8);
    ret = heatbucket->ExtendSortBelowBoundary(0);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 10);
    ASSERT_EQ(heatbucket->GetLastSortedBucketBelow(), 2);
}

TEST_F(HeatBucketTest, ExtendSortBelowBoundary_DirectlyInvoke_SUCCESS)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatBucket* heatbucket = NULL;

    sortbase = SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops;
    heatbucket = GetHeatBucket();
    heatbucket->SetSortBase(sortbase);
    ret = heatbucket->ExtendSortBelowBoundary(8);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 10);
    ASSERT_EQ(heatbucket->GetLastSortedBucketBelow(), 2);
}

TEST_F(HeatBucketTest, GetFirstHotOBJFromBucketIndex_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey* objkey = NULL;
    OBJVal* obj = NULL;
    HeatBucket* heatbucket = NULL;

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    /*simulate sort result*/
    heatbucket->SetLastSortedBucketAbove(5);
    ret = heatbucket->GetFirstHotOBJFromBucketIndexOn(9, &obj);
    ASSERT_EQ(ret, YWB_SUCCESS);
    obj->GetOBJKey(&objkey);
    ASSERT_EQ(objkey->GetInodeId(), 8);
    ASSERT_EQ(heatbucket->GetCurHotBucket(), 8);
}

TEST_F(HeatBucketTest, GetFirstHotOBJFromBucketIndex_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJVal* obj = NULL;
    HeatBucket* heatbucket = NULL;

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    heatbucket->SetLastSortedBucketAbove(Constant::BUCKET_NUM);
    ret = heatbucket->GetFirstHotOBJFromBucketIndexOn(9, &obj);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(heatbucket->GetCurHotBucket(), 9);
}

TEST_F(HeatBucketTest, GetFirstNextHotOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objnum = 0;
    OBJKey* objkey = NULL;
    OBJVal* obj = NULL;
    HeatBucket* heatbucket = NULL;
    ywb_uint64_t objinodeids[6] = {8, 18, 7, 17, 5, 15};

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    heatbucket->SetLastSortedBucketAbove(5);
    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJKey(&objkey);
        ASSERT_EQ(objkey->GetInodeId(), objinodeids[objnum++]);
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(heatbucket->GetCurHotBucket(), 4);
}

TEST_F(HeatBucketTest, GetFirstColdOBJFromBucketIndex_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey* objkey = NULL;
    OBJVal* obj = NULL;
    HeatBucket* heatbucket = NULL;

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    /*simulate sort result*/
    heatbucket->SetLastSortedBucketBelow(2);
    ret = heatbucket->GetFirstColdOBJFromBucketIndexOn(0, &obj);
    ASSERT_EQ(ret, YWB_SUCCESS);
    obj->GetOBJKey(&objkey);
    ASSERT_EQ(objkey->GetInodeId(), 1);
    ASSERT_EQ(heatbucket->GetCurColdBucket(), 1);
}

TEST_F(HeatBucketTest, GetFirstColdOBJFromBucketIndex_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJVal* obj = NULL;
    HeatBucket* heatbucket = NULL;

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    heatbucket->SetLastSortedBucketBelow(Constant::BUCKET_NUM);
    ret = heatbucket->GetFirstColdOBJFromBucketIndexOn(0, &obj);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(heatbucket->GetCurColdBucket(), 0);
}

TEST_F(HeatBucketTest, GetFirstNextColdOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objnum = 0;
    OBJKey* objkey = NULL;
    OBJVal* obj = NULL;
    HeatBucket* heatbucket = NULL;
    ywb_uint64_t objinodeids[6] = {1, 11, 2, 12};

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    heatbucket->SetLastSortedBucketBelow(2);
    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJKey(&objkey);
        ASSERT_EQ(objkey->GetInodeId(), objinodeids[objnum++]);
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(heatbucket->GetCurColdBucket(), 3);
}

TEST_F(HeatBucketTest, GetAllSatisfyingOBJs_SelectFromLargerButNotSortBeforeHand_SUCCESS)
{
    AdviceRule advicerule;
    HeatBucket* heatbucket = NULL;
    set<OBJVal*> objsabove;

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    advicerule.SetSelector(Selector::SEL_from_larger);
    heatbucket->GetAllSatisfyingOBJs(advicerule, &objsabove);
    ASSERT_EQ(objsabove.empty(), true);
}

TEST_F(HeatBucketTest, GetAllSatisfyingOBJs_SelectFromLarger_SUCCESS)
{
    AdviceRule advicerule;
    HeatBucket* heatbucket = NULL;
    set<OBJVal*> objsabove;

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    advicerule.SetSelector(Selector::SEL_from_larger);
    heatbucket->SetLastSortedBucketAbove(5);
    heatbucket->GetAllSatisfyingOBJs(advicerule, &objsabove);
    ASSERT_EQ(objsabove.empty(), false);
    ASSERT_EQ(objsabove.size(), 6);
}

TEST_F(HeatBucketTest, GetAllSatisfyingOBJs_SelectFromSmallerButNotSortBeforeHand_SUCCESS)
{
    AdviceRule advicerule;
    HeatBucket* heatbucket = NULL;
    set<OBJVal*> objsbelow;

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    advicerule.SetSelector(Selector::SEL_from_smaller);
    heatbucket->GetAllSatisfyingOBJs(advicerule, &objsbelow);
    ASSERT_EQ(objsbelow.empty(), true);
}

TEST_F(HeatBucketTest, GetAllSatisfyingOBJs_SelectFromSmaller_SUCCESS)
{
    AdviceRule advicerule;
    HeatBucket* heatbucket = NULL;
    set<OBJVal*> objsbelow;
    set<OBJVal*>::iterator objiter;

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    advicerule.SetSelector(Selector::SEL_from_smaller);
    heatbucket->SetLastSortedBucketBelow(2);
    heatbucket->GetAllSatisfyingOBJs(advicerule, &objsbelow);
    ASSERT_EQ(objsbelow.empty(), false);
    ASSERT_EQ(objsbelow.size(), 4);
}

TEST_F(HeatBucketTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;

    heatbucket = GetHeatBucket2();
    PrepareHeatBucket2();
    heatbucket->SetLastSortedBucketAbove(5);
    heatbucket->SetLastSortedBucketBelow(2);
    ret = heatbucket->Destroy();
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = heatbucket->GetFirstHotOBJFromBucketIndexOn(Constant::BUCKET_NUM - 1, &obj);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = heatbucket->GetFirstColdOBJFromBucketIndexOn(0, &obj);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(DiskHeatDistributionTest, GetFirstDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskHeatDistribution* diskheat = NULL;
    Disk* disk = NULL;

    diskheat = GetDiskHeatDistribution();
    ret = diskheat->GetFirstDisk(&disk);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(disk, GetDisk());
}

TEST_F(DiskHeatDistributionTest, GetNextDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskHeatDistribution* diskheat = NULL;
    Disk* disk = NULL;

    diskheat = GetDiskHeatDistribution();
    diskheat->GetFirstDisk(&disk);
    ret = diskheat->GetNextDisk(&disk);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(DiskHeatDistributionTest, SetupHeatRangeSummary_DiskHasNoOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatRange* range = NULL;

    diskheat = GetDiskHeatDistribution2();
    ret = diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ASSERT_EQ(ret, YWB_SUCCESS);
    summary->GetRndWriteCnt(&range);
    ASSERT_EQ(range->GetMax(), 0);
    summary->GetSeqAccumBW(&range);
    ASSERT_EQ(range->GetBucketCap(), YWB_UINT32_MAX);
    summary->GetRndSeqAccumRT(&range);
    ASSERT_EQ(range->GetMin(), YWB_UINT32_MAX);
}

TEST_F(DiskHeatDistributionTest, SetupHeatRangeSummary_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatRange* range = NULL;

    diskheat = GetDiskHeatDistribution();
    ret = diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ASSERT_EQ(ret, YWB_SUCCESS);

    summary->GetRndReadCnt(&range);
    ASSERT_EQ(range->GetBucketCap(), 1);
    summary->GetRndWriteCnt(&range);
    ASSERT_EQ(range->GetBucketCap(), 1);
    summary->GetRndAccumIOs(&range);
    ASSERT_EQ(range->GetBucketCap(), 2);
    summary->GetRndAccumRT(&range);
    ASSERT_EQ(range->GetBucketCap(), 6);
    summary->GetRndAccumBW(&range);
    ASSERT_EQ(range->GetBucketCap(), 9);
    summary->GetSeqReadCnt(&range);
    ASSERT_EQ(range->GetBucketCap(), 1);
    summary->GetSeqWriteCnt(&range);
    ASSERT_EQ(range->GetBucketCap(), 1);
    summary->GetSeqAccumIOs(&range);
    ASSERT_EQ(range->GetBucketCap(), 2);
    summary->GetSeqAccumRT(&range);
    ASSERT_EQ(range->GetBucketCap(), 6);
    summary->GetSeqAccumBW(&range);
    ASSERT_EQ(range->GetBucketCap(), 9);
    summary->GetRndSeqAccumIOs(&range);
    ASSERT_EQ(range->GetBucketCap(), 4);
    summary->GetRndSeqAccumBW(&range);
    ASSERT_EQ(range->GetBucketCap(), 19);
    summary->GetRndSeqAccumRT(&range);
    ASSERT_EQ(range->GetBucketCap(), 12);
}

TEST_F(DiskHeatDistributionTest, GetOBJBucketIndex_BaseOnRawRndIOPS_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops;
    disk = GetDisk();
    disk->GetFirstOBJ(&obj);

    diskheat = GetDiskHeatDistribution();
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = diskheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, 8);
}

TEST_F(DiskHeatDistributionTest, GetOBJBucketIndex_BaseOnRawSeqBW_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_sequential | PartitionBase::PB_bw;
    disk = GetDisk();
    disk->GetFirstOBJ(&obj);

    diskheat = GetDiskHeatDistribution();
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = diskheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, 7);
}

TEST_F(DiskHeatDistributionTest, GetOBJBucketIndex_BaseOnRawRndSeqRT_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_rnd_seq | PartitionBase::PB_rt;
    disk = GetDisk();
    disk->GetFirstOBJ(&obj);

    diskheat = GetDiskHeatDistribution();
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = diskheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, 6);
}

TEST_F(DiskHeatDistributionTest, GetOBJBucketIndex_PartitionBaseInvalid_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    partitionbase = PartitionBase::PB_raw | PartitionBase::PB_rt;
    disk = GetDisk();
    disk->GetFirstOBJ(&obj);

    diskheat = GetDiskHeatDistribution();
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = diskheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, (ywb_uint32_t)Constant::BUCKET_NUM);
}

TEST_F(DiskHeatDistributionTest, GetOBJBucketIndex_BucketCapIsYWB_UINT32_MAX_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    HeatRange heatrange;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops;
    disk = GetDisk();
    disk->GetFirstOBJ(&obj);

    diskheat = GetDiskHeatDistribution();
    summary = new HeatRangeSummary();
    YWB_ASSERT(summary != NULL);
    summary->SetRndAccumIOs(heatrange);
    bucketindex = diskheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, (ywb_uint32_t)Constant::BUCKET_NUM);
}

TEST_F(DiskHeatDistributionTest, GetOBJBucketIndex_BucketCapIs0_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    HeatRange heatrange;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops;
    disk = GetDisk();
    disk->GetFirstOBJ(&obj);

    diskheat = GetDiskHeatDistribution();
    summary = new HeatRangeSummary();
    YWB_ASSERT(summary != NULL);
    heatrange.SetBucketCap(0);
    summary->SetRndReadCnt(heatrange);
    bucketindex = diskheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, (ywb_uint32_t)Constant::BUCKET_NUM);
}

TEST_F(DiskHeatDistributionTest, GetOBJBucketIndex_CalculatedBucketIndexExceed_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    Disk* disk = NULL;
    OBJKey objkey;
    OBJ* obj = NULL;
    HeatRange heatrange;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops;
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);
    objkey.SetInodeId(5);

    disk = GetDisk();
    disk->GetOBJ(objkey, &obj);

    diskheat = GetDiskHeatDistribution();
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = diskheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, (ywb_uint32_t)(Constant::BUCKET_NUM - 1));
}

TEST_F(DiskHeatDistributionTest, Partition_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatBucket* heatbucket = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);

    diskheat = GetDiskHeatDistribution();
    heatbucket = GetHeatBucket();
    heatbucket->SetPartitionBase(advicerule.GetPartitionBase());
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ret = diskheat->Partition(advicerule, summary, heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetOBJsAboveBoundary(), 3);
    ASSERT_EQ(heatbucket->GetOBJsBelowBoundary(), 3);
}

TEST_F(DiskHeatDistributionTest, Partition_DiskHasNoOBJ_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatBucket* heatbucket = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);

    diskheat = GetDiskHeatDistribution2();
    heatbucket = GetHeatBucket2();
    heatbucket->SetPartitionBase(advicerule.GetPartitionBase());
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ret = diskheat->Partition(advicerule, summary, heatbucket);
    ASSERT_EQ(ret, YFS_ENODATA);
}

TEST_F(DiskHeatDistributionTest, Sort_SortAbove_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[6] = {13, 9, 5, 4, 2, 1};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    heatbucket = GetHeatBucket();
    heatbucket->SetPartitionBase(advicerule.GetPartitionBase());
    heatbucket->SetSortBase(advicerule.GetSortBase());
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    diskheat->Partition(advicerule, summary, heatbucket);
    ret = diskheat->Sort(advicerule, heatbucket, 5, 0);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 3);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 0);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 3);
}

TEST_F(DiskHeatDistributionTest, Sort_SortBelow_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[6] = {1, 2, 4, 5, 9, 13};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    heatbucket = GetHeatBucket();
    heatbucket->SetPartitionBase(advicerule.GetPartitionBase());
    heatbucket->SetSortBase(advicerule.GetSortBase());
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    diskheat->Partition(advicerule, summary, heatbucket);
    ret = diskheat->Sort(advicerule, heatbucket, 0, 3);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 0);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 3);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 3);
}

TEST_F(DiskHeatDistributionTest, Setup_DiskHasNoOBJ_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution2();
    ret = diskheat->Setup(advicerule, 3, 3);
    ASSERT_EQ(ret, YFS_ENODATA);
}

TEST_F(DiskHeatDistributionTest, Setup_HeatBucketNULLAndInvalidRule_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_random |
            PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    ret = diskheat->Setup(advicerule, 3, 3);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(DiskHeatDistributionTest, Setup_HeatBucketNULLAndHeatSummaryNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[6] = {13, 9, 5, 1, 2, 4};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    ret = diskheat->Setup(advicerule, 3, 3);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 3);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 3);
}

TEST_F(DiskHeatDistributionTest, Setup_HeatBucketNULLAndHeatSummaryNonNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[6] = {13, 9, 5, 1, 2, 4};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ret = diskheat->Setup(advicerule, 3, 3);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 3);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 3);
}

TEST_F(DiskHeatDistributionTest, Setup_HeatBucketNonNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[6] = {13, 9, 1, 2};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    /*Setup() will help us to create HeatBucket*/
    diskheat->Setup(advicerule, 2, 2);
    ret = diskheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    diskheat->Setup(advicerule, 0, 0);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 2);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 2);
}

TEST_F(DiskHeatDistributionTest, Setup_HeatBucketNonNuLLAndRequiredMoreThanSortedAbove_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[6] = {13, 9, 1};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    diskheat->Setup(advicerule, 1, 1);
    ret = diskheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    diskheat->Setup(advicerule, 2, 0);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 2);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 1);
}

TEST_F(DiskHeatDistributionTest, Setup_HeatBucketNonNuLLAndRequiredMoreThanSortedBelow_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[6] = {13, 1, 2, 4};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    diskheat->Setup(advicerule, 1, 1);
    ret = diskheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    diskheat->Setup(advicerule, 0, 3);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 1);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 3);
}

TEST_F(DiskHeatDistributionTest, GetHeatRangeSummary_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ret = diskheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(summary != NULL, true);
}

TEST_F(DiskHeatDistributionTest, GetHeatRangeSummary_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatRangeSummary* summary = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    diskheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ret = diskheat->GetHeatRangeSummary(IOStat::IOST_short_term_ema, &summary);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(DiskHeatDistributionTest, GetHeatBucket_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatBucket* bucket = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    diskheat->Setup(advicerule, 1, 1);
    ret = diskheat->GetHeatBucket(advicerule, &bucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(bucket != NULL, true);
}

TEST_F(DiskHeatDistributionTest, GetHeatBucket_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;
    HeatBucket* bucket = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    diskheat->Setup(advicerule, 1, 1);
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_rt);
    ret = diskheat->GetHeatBucket(advicerule, &bucket);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(DiskHeatDistributionTest, Destroy_ByGivenRule_SUCCESS)
{
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    diskheat->Setup(advicerule, 1, 1);
    /*
     * NOTE: should not call Destroy(AdviceRule) here directly,
     * for Destroy(AdviceRule) will be called automatically
     * at the point of @diskheat de-construction.
     **/
}

TEST_F(DiskHeatDistributionTest, Destroy_ByGivenRule_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    DiskHeatDistribution* diskheat = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    diskheat = GetDiskHeatDistribution();
    diskheat->Setup(advicerule, 1, 1);
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_rt);
    ret = diskheat->Destroy(advicerule);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(DiskHeatDistributionTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    AdviceRule advicerule2;
    DiskHeatDistribution* diskheat = NULL;
    HeatBucket* bucket = NULL;

    diskheat = GetDiskHeatDistribution();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    diskheat->Setup(advicerule, 1, 1);
    ret = diskheat->GetHeatBucket(advicerule, &bucket);
    ASSERT_EQ(ret, YWB_SUCCESS);

    advicerule2.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_rt);
    advicerule2.SetPartitionScope(PartitionScope::PS_disk_wise);
    advicerule2.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    diskheat->Setup(advicerule2, 4, 4);
    ret = diskheat->GetHeatBucket(advicerule2, &bucket);
    ASSERT_EQ(ret, YWB_SUCCESS);

    /*
     * NOTE: here we will not call destroy but instead called
     * automatically at the point of de-construction of @tierheat,
     * Destroy() is successfull only if this test case exits normally
     **/
}

TEST_F(TierWiseHeatDistributionTest, GetFirstNextDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierWiseHeatDistribution* tierheat = NULL;
    Disk* disk = NULL;

    tierheat = GetTierWiseHeatDistribution();
    ret = tierheat->GetFirstDisk(&disk);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tierheat->GetNextDisk(&disk);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tierheat->GetNextDisk(&disk);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tierheat->GetNextDisk(&disk);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierWiseHeatDistributionTest, GetFirstNextOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    TierWiseHeatDistribution* tierheat = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t objsrndreadios[2 * 6] =
            {9, 5, 1, 4, 13, 2, 12, 17, 3, 16, 15, 11};

    tierheat = GetTierWiseHeatDistribution();
    ret = tierheat->GetFirstOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), objsrndreadios[objsnum++]);
        ret = tierheat->GetNextOBJ(&obj);
    }
    ASSERT_EQ(objsnum, (ywb_uint32_t)(2 * 6));
}

TEST_F(TierWiseHeatDistributionTest, SetupHeatRangeSummary_TierHasNoDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatRange* range = NULL;
    Tier* tier = NULL;

    tierheat = GetTierWiseHeatDistribution2();
    tier = GetTier2();
    YWB_ASSERT(0 == tier->GetDiskCnt());

    ret = tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ASSERT_EQ(ret, YWB_SUCCESS);
    summary->GetRndWriteCnt(&range);
    ASSERT_EQ(range->GetMax(), 0);
    summary->GetSeqAccumBW(&range);
    ASSERT_EQ(range->GetBucketCap(), YWB_UINT32_MAX);
    summary->GetRndSeqAccumRT(&range);
    ASSERT_EQ(range->GetMin(), YWB_UINT32_MAX);

}

TEST_F(TierWiseHeatDistributionTest, SetupHeatRangeSummary_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatRange* range = NULL;

    tierheat = GetTierWiseHeatDistribution();
    ret = tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ASSERT_EQ(ret, YWB_SUCCESS);

    summary->GetRndReadCnt(&range);
    ASSERT_EQ(range->GetBucketCap(), 1);
    summary->GetRndWriteCnt(&range);
    ASSERT_EQ(range->GetBucketCap(), 1);
    summary->GetRndAccumIOs(&range);
    ASSERT_EQ(range->GetBucketCap(), 3);
    summary->GetRndAccumRT(&range);
    ASSERT_EQ(range->GetBucketCap(), 8);
    summary->GetRndAccumBW(&range);
    ASSERT_EQ(range->GetBucketCap(), 12);
    summary->GetSeqReadCnt(&range);
    ASSERT_EQ(range->GetBucketCap(), 1);
    summary->GetSeqWriteCnt(&range);
    ASSERT_EQ(range->GetBucketCap(), 1);
    summary->GetSeqAccumIOs(&range);
    ASSERT_EQ(range->GetBucketCap(), 3);
    summary->GetSeqAccumRT(&range);
    ASSERT_EQ(range->GetBucketCap(), 8);
    summary->GetSeqAccumBW(&range);
    ASSERT_EQ(range->GetBucketCap(), 12);
    summary->GetRndSeqAccumIOs(&range);
    ASSERT_EQ(range->GetBucketCap(), 6);
    summary->GetRndSeqAccumBW(&range);
    ASSERT_EQ(range->GetBucketCap(), 25);
    summary->GetRndSeqAccumRT(&range);
    ASSERT_EQ(range->GetBucketCap(), 16);
}

TEST_F(TierWiseHeatDistributionTest, GetOBJBucketIndex_BaseOnRawRndIOPS_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    OBJ* obj = NULL;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops;
    /*Get OBJ corresponding to Disk ID 4's sixth OBJ*/
    obj = GetOBJ(4, 24);
    YWB_ASSERT(obj != NULL);
    tierheat = GetTierWiseHeatDistribution();
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = tierheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, 6);
}

TEST_F(TierWiseHeatDistributionTest, GetOBJBucketIndex_BaseOnRawSeqBW_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    OBJ* obj = NULL;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_sequential | PartitionBase::PB_bw;
    obj = GetOBJ(4, 24);
    YWB_ASSERT(obj != NULL);
    tierheat = GetTierWiseHeatDistribution();
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = tierheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, 6);
}

TEST_F(TierWiseHeatDistributionTest, GetOBJBucketIndex_BaseOnRawRndSeqRT_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    OBJ* obj = NULL;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_rnd_seq | PartitionBase::PB_rt;
    obj = GetOBJ(4, 24);
    YWB_ASSERT(obj != NULL);
    tierheat = GetTierWiseHeatDistribution();
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = tierheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, 6);
}

TEST_F(TierWiseHeatDistributionTest, GetOBJBucketIndex_PartitionBaseInvalid_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    OBJ* obj = NULL;

    partitionbase = PartitionBase::PB_raw | PartitionBase::PB_rt;
    obj = GetOBJ(4, 24);
    YWB_ASSERT(obj != NULL);
    tierheat = GetTierWiseHeatDistribution();
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = tierheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, (ywb_uint32_t)Constant::BUCKET_NUM);
}

TEST_F(TierWiseHeatDistributionTest, GetOBJBucketIndex_BucketCapIsYWB_UINT32_MAX_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    OBJ* obj = NULL;
    HeatRange heatrange;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops;
    obj = GetOBJ(4, 24);
    YWB_ASSERT(obj != NULL);
    tierheat = GetTierWiseHeatDistribution();
    summary = new HeatRangeSummary();
    YWB_ASSERT(summary != NULL);
    summary->SetRndAccumIOs(heatrange);
    bucketindex = tierheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, (ywb_uint32_t)Constant::BUCKET_NUM);
}

TEST_F(TierWiseHeatDistributionTest, GetOBJBucketIndex_BucketCapIs0_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    OBJ* obj = NULL;
    HeatRange heatrange;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops;
    obj = GetOBJ(4, 24);
    YWB_ASSERT(obj != NULL);
    tierheat = GetTierWiseHeatDistribution();
    summary = new HeatRangeSummary();
    YWB_ASSERT(summary != NULL);
    heatrange.SetBucketCap(0);
    summary->SetRndReadCnt(heatrange);
    bucketindex = tierheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, (ywb_uint32_t)Constant::BUCKET_NUM);
}

TEST_F(TierWiseHeatDistributionTest, GetOBJBucketIndex_CalculatedBucketIndexExceed_SUCCESS)
{
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t partitionbase = 0;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    OBJ* obj = NULL;
    HeatRange heatrange;

    partitionbase = PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops;
    obj = GetOBJ(4, 22);
    YWB_ASSERT(obj != NULL);
    tierheat = GetTierWiseHeatDistribution();
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    bucketindex = tierheat->GetOBJBucketIndex(partitionbase, summary, obj);
    ASSERT_EQ(bucketindex, (ywb_uint32_t)(Constant::BUCKET_NUM - 1));
}

TEST_F(TierWiseHeatDistributionTest, Partition_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatBucket* heatbucket = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);

    tierheat = GetTierWiseHeatDistribution();
    heatbucket = GetHeatBucket();
    heatbucket->SetPartitionBase(advicerule.GetPartitionBase());
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ret = tierheat->Partition(advicerule, summary, heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetOBJsAboveBoundary(), 7);
    ASSERT_EQ(heatbucket->GetOBJsBelowBoundary(), 5);
}

TEST_F(TierWiseHeatDistributionTest, Partition_TierHasNoOBJ_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    Tier* tier = NULL;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatBucket* heatbucket = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);

    tierheat = GetTierWiseHeatDistribution2();
    tier = GetTier2();
    YWB_ASSERT(0 == tier->GetOBJCnt());
    heatbucket = new HeatBucket(tierheat);
    heatbucket->SetPartitionBase(advicerule.GetPartitionBase());
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ret = tierheat->Partition(advicerule, summary, heatbucket);
    ASSERT_EQ(ret, YFS_ENODATA);
}

TEST_F(TierWiseHeatDistributionTest, Sort_SortAbove_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    /*Tier 0 has 3 disks including one empty disk*/
    ywb_uint32_t rndreadios[2 * 6] = {17, 16, 15, 13, 12, 11, 9, 5, 4, 3, 2, 1};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    heatbucket = GetHeatBucket();
    heatbucket->SetPartitionBase(advicerule.GetPartitionBase());
    heatbucket->SetSortBase(advicerule.GetSortBase());
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    tierheat->Partition(advicerule, summary, heatbucket);
    ret = tierheat->Sort(advicerule, heatbucket, 7, 0);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 7);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 0);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 7);
}

TEST_F(TierWiseHeatDistributionTest, Sort_SortBelow_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[2 * 6] = {1, 2, 3, 4, 5, 9, 11, 12, 13, 15, 16, 17};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw | SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    heatbucket = GetHeatBucket();
    heatbucket->SetPartitionBase(advicerule.GetPartitionBase());
    heatbucket->SetSortBase(advicerule.GetSortBase());
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    tierheat->Partition(advicerule, summary, heatbucket);
    ret = tierheat->Sort(advicerule, heatbucket, 0, 6);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 0);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 5);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 5);
}

TEST_F(TierWiseHeatDistributionTest, Setup_TierHasNoOBJ_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    Tier* tier = NULL;
    TierWiseHeatDistribution* tierheat = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tier = GetTier2();
    YWB_ASSERT(0 == tier->GetOBJCnt());
    tierheat = GetTierWiseHeatDistribution2();
    ret = tierheat->Setup(advicerule, 7, 5);
    ASSERT_EQ(ret, YFS_ENODATA);
}

TEST_F(TierWiseHeatDistributionTest, Setup_HeatBucketNULLAndInvalidRule_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_random |
            PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    ret = tierheat->Setup(advicerule, 7, 5);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(TierWiseHeatDistributionTest, Setup_HeatBucketNULLAndHeatSummaryNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[2 * 6] =
            {17, 16, 15, 13, 12, 11, 9, 1, 2, 3, 4, 5};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    ret = tierheat->Setup(advicerule, 7, 5);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tierheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 7);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 5);
}

TEST_F(TierWiseHeatDistributionTest, Setup_HeatBucketNULLAndHeatSummaryNonNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[2 * 6] =
            {17, 16, 15, 13, 12, 11, 9, 1, 2, 3, 4, 5};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ret = tierheat->Setup(advicerule, 7, 5);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tierheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 7);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 5);
}

TEST_F(TierWiseHeatDistributionTest, Setup_HeatBucketNonNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[2 * 6] = {17, 16, 15, 1, 2, 3};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    /*Setup() will help us to create HeatBucket*/
    tierheat->Setup(advicerule, 3, 3);
    ret = tierheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    tierheat->Setup(advicerule, 0, 0);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 3);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 3);
}

TEST_F(TierWiseHeatDistributionTest, Setup_HeatBucketNonNuLLAndRequiredMoreThanSortedAbove_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[2 * 6] = {17, 16, 15, 13, 1, 2};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    tierheat->Setup(advicerule, 3, 2);
    ret = tierheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    tierheat->Setup(advicerule, 4, 0);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 4);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 2);
}

TEST_F(TierWiseHeatDistributionTest, Setup_HeatBucketNonNuLLAndRequiredMoreThanSortedBelow_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objsnum = 0;
    ywb_uint32_t objsabove = 0;
    ywb_uint32_t objsbelow = 0;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* heatbucket = NULL;
    OBJVal* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat* iostat = NULL;
    ywb_uint32_t rndreadios[2 * 6] = {17, 16, 15, 13, 1, 2, 3};

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    tierheat->Setup(advicerule, 4, 2);
    ret = tierheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    tierheat->Setup(advicerule, 0, 3);

    ret = heatbucket->GetFirstHotOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsabove++;
        ret = heatbucket->GetNextHotOBJ(&obj);
    }
    ASSERT_EQ(objsabove, 4);

    ret = heatbucket->GetFirstColdOBJ(&obj);
    while((YWB_SUCCESS == ret))
    {
        obj->GetOBJIO(&objio);
        objio->GetIOStat(IOStat::IOST_raw, &iostat);
        ASSERT_EQ(iostat->GetRndReadIOs(), rndreadios[objsnum++]);
        objsbelow++;
        ret = heatbucket->GetNextColdOBJ(&obj);
    }
    ASSERT_EQ(objsbelow, 3);
}

TEST_F(TierWiseHeatDistributionTest, GetHeatRangeSummary_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ret = tierheat->GetHeatRangeSummary(IOStat::IOST_raw, &summary);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(summary != NULL, true);
}

TEST_F(TierWiseHeatDistributionTest, GetHeatRangeSummary_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatRangeSummary* summary = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    tierheat->SetupHeatRangeSummary(IOStat::IOST_raw);
    ret = tierheat->GetHeatRangeSummary(IOStat::IOST_short_term_ema, &summary);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierWiseHeatDistributionTest, GetHeatBucket_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* bucket = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    tierheat->Setup(advicerule, 1, 1);
    ret = tierheat->GetHeatBucket(advicerule, &bucket);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(bucket != NULL, true);
}

TEST_F(TierWiseHeatDistributionTest, GetHeatBucket_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* bucket = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    tierheat->Setup(advicerule, 1, 1);
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_rt);
    ret = tierheat->GetHeatBucket(advicerule, &bucket);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierWiseHeatDistributionTest, Destroy_ByGivenRule_SUCCESS)
{
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    tierheat->Setup(advicerule, 1, 1);
    /*
     * NOTE: should not call Destroy(AdviceRule) here directly,
     * for Destroy(AdviceRule) will be called automatically
     * at the point of @diskheat de-construction.
     **/
}

TEST_F(TierWiseHeatDistributionTest, Destroy_ByGivenRule_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    TierWiseHeatDistribution* tierheat = NULL;

    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);

    tierheat = GetTierWiseHeatDistribution();
    tierheat->Setup(advicerule, 1, 1);
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_rt);
    ret = tierheat->Destroy(advicerule);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierWiseHeatDistributionTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    AdviceRule advicerule2;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* bucket = NULL;

    tierheat = GetTierWiseHeatDistribution();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    tierheat->Setup(advicerule, 1, 1);
    ret = tierheat->GetHeatBucket(advicerule, &bucket);
    ASSERT_EQ(ret, YWB_SUCCESS);

    advicerule2.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_rt);
    advicerule2.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule2.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    tierheat->Setup(advicerule2, 4, 4);
    ret = tierheat->GetHeatBucket(advicerule2, &bucket);
    ASSERT_EQ(ret, YWB_SUCCESS);

    /*
     * NOTE: here we will not call destroy but instead called
     * automatically at the point of de-construction of @tierheat,
     * Destroy() is successfull only if this test case exits normally
     **/
}

TEST_F(SubPoolHeatDistributionTest, SetupTierWiseHeatDistribution_TierNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    SubPoolHeatDistribution* subpoolheat = NULL;

    subpoolheat = GetSubPoolHeat();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    ret = subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_cnt, advicerule, 3, 3);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolHeatDistributionTest, SetupTierWiseHeatDistribution_TierHeatNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    SubPoolHeatDistribution* subpoolheat = NULL;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* heatbucket = NULL;

    subpoolheat = GetSubPoolHeat();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    ret = subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_1, advicerule, 10, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolheat->GetTierWiseHeatDistribution(Tier::TIER_1, &tierheat);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(tierheat != NULL, true);
    tierheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 10);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 2);
}

TEST_F(SubPoolHeatDistributionTest, SetupTierWiseHeatDistribution_TierHeatAlreadyExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    SubPoolHeatDistribution* subpoolheat = NULL;
    TierWiseHeatDistribution* tierheat = NULL;
    HeatBucket* heatbucket = NULL;

    subpoolheat = GetSubPoolHeat();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_1, advicerule, 6, 1);
    ret = subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_1, advicerule, 10, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    subpoolheat->GetTierWiseHeatDistribution(Tier::TIER_1, &tierheat);
    tierheat->GetHeatBucket(advicerule, &heatbucket);
    ASSERT_EQ(heatbucket->GetSortedAboveBoundary(), 10);
    ASSERT_EQ(heatbucket->GetSortedBelowBoundary(), 2);
}

TEST_F(SubPoolHeatDistributionTest, GetTierWiseHeatDistribution_TierHeatExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    SubPoolHeatDistribution* subpoolheat = NULL;
    TierWiseHeatDistribution* tierheat = NULL;

    subpoolheat = GetSubPoolHeat();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_0, advicerule, 6, 1);
    advicerule.SetPartitionBase(SortBase::SB_raw |
            SortBase::SB_sequential | SortBase::SB_bw);
    subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_1, advicerule, 7, 2);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_rnd_seq | SortBase::SB_rt);
    subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_2, advicerule, 8, 3);

    ret = subpoolheat->GetTierWiseHeatDistribution(Tier::TIER_0, &tierheat);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolheat->GetTierWiseHeatDistribution(Tier::TIER_1, &tierheat);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolheat->GetTierWiseHeatDistribution(Tier::TIER_2, &tierheat);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(SubPoolHeatDistributionTest, GetTierWiseHeatDistribution_TierHeatNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    SubPoolHeatDistribution* subpoolheat = NULL;
    TierWiseHeatDistribution* tierheat = NULL;

    subpoolheat = GetSubPoolHeat();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_1, advicerule, 6, 1);
    ret = subpoolheat->GetTierWiseHeatDistribution(Tier::TIER_2, &tierheat);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolHeatDistributionTest, DestroyTierWiseHeatDistribution_TierHeatNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    SubPoolHeatDistribution* subpoolheat = NULL;

    subpoolheat = GetSubPoolHeat();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    ret = subpoolheat->DestroyTierWiseHeatDistribution(Tier::TIER_1, advicerule);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = subpoolheat->DestroyTierWiseHeatDistribution(Tier::TIER_1);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolHeatDistributionTest, DestroyTierWiseHeatDistribution_AdviceRuleMismatch_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    SubPoolHeatDistribution* subpoolheat = NULL;

    subpoolheat = GetSubPoolHeat();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_0, advicerule, 6, 1);
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_sequential | PartitionBase::PB_bw);
    ret = subpoolheat->DestroyTierWiseHeatDistribution(Tier::TIER_0, advicerule);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolHeatDistributionTest, DestroyTierWiseHeatDistribution_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    SubPoolHeatDistribution* subpoolheat = NULL;
    TierWiseHeatDistribution* tierheat = NULL;

    subpoolheat = GetSubPoolHeat();
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_random | PartitionBase::PB_iops);
    advicerule.SetPartitionScope(PartitionScope::PS_tier_wise);
    advicerule.SetSortBase(SortBase::SB_raw |
            SortBase::SB_random | SortBase::SB_iops);
    subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_0, advicerule, 6, 1);
    advicerule.SetPartitionBase(PartitionBase::PB_raw |
            PartitionBase::PB_sequential | PartitionBase::PB_bw);
    subpoolheat->SetupTierWiseHeatDistribution(Tier::TIER_1, advicerule, 7, 2);
    ret = subpoolheat->DestroyTierWiseHeatDistribution(Tier::TIER_0);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolheat->GetTierWiseHeatDistribution(Tier::TIER_0, &tierheat);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = subpoolheat->DestroyTierWiseHeatDistribution(Tier::TIER_1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolheat->GetTierWiseHeatDistribution(Tier::TIER_1, &tierheat);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(HeatDistributionManagerTest, RegisterGreater_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatDistributionManager* heatmgr = NULL;
    HeatCmp::Greater greater;

    heatmgr = GetHeatManager();
    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_random | SortBase::SB_iops);
    heatmgr->RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMARndIOs);
    ret = heatmgr->GetGreater(sortbase, &greater);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(HeatDistributionManagerTest, RegisterLess_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatDistributionManager* heatmgr = NULL;
    HeatCmp::Less less;

    heatmgr = GetHeatManager();
    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_random | SortBase::SB_iops);
    heatmgr->RegisterLess(sortbase, HeatCmp::LessByLongTermEMARndIOs);
    ret = heatmgr->GetLess(sortbase, &less);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(HeatDistributionManagerTest, Initialize_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;
    HeatDistributionManager* heatmgr = NULL;
    HeatCmp::Greater greater;
    HeatCmp::Less less;

    heatmgr = GetHeatManager();
    heatmgr->Initialize();
    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
                SortBase::SB_random | SortBase::SB_bw);
    ret = heatmgr->GetGreater(sortbase, &greater);
    ASSERT_EQ(ret, YWB_SUCCESS);

    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_sequential | SortBase::SB_iops);
    ret = heatmgr->GetLess(sortbase, &less);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(HeatDistributionManagerTest, Init_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    SubPoolKey subpoolkey;
    HeatDistributionManager* heatmgr = NULL;
    SubPoolHeatDistribution* subpoolheat = NULL;

    heatmgr = GetHeatManager();
    heatmgr->Init();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        ret = heatmgr->GetSubPoolHeatDistribution(subpoolkey, &subpoolheat);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ASSERT_EQ(subpoolheat != NULL, true);
    }
}

TEST_F(HeatDistributionManagerTest, DestroySubPoolHeatDistribution_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    SubPoolKey subpoolkey;
    HeatDistributionManager* heatmgr = NULL;
    SubPoolHeatDistribution* subpoolheat = NULL;

    heatmgr = GetHeatManager();
    heatmgr->Init();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        ret = heatmgr->DestroySubPoolHeatDistribution(subpoolkey);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ret = heatmgr->GetSubPoolHeatDistribution(subpoolkey, &subpoolheat);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(HeatDistributionManagerTest, Reset_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    SubPoolKey subpoolkey;
    HeatDistributionManager* heatmgr = NULL;
    SubPoolHeatDistribution* subpoolheat = NULL;

    heatmgr = GetHeatManager();
    heatmgr->Init();
    heatmgr->Reset();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        ret = heatmgr->GetSubPoolHeatDistribution(subpoolkey, &subpoolheat);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

/* vim:set ts=4 sw=4 expandtab */
