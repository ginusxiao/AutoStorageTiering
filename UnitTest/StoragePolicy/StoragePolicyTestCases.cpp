/*
 * StoragePolicyTestCases.hpp
 *
 *  Created on: 2016年10月16日
 *      Author: Administrator
 */

#include "UnitTest/AST/StoragePolicy/StoragePolicyTests.hpp"

TEST_F(PolicyManagerTest, SetStoragePolicy_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    StoragePolicy policy;
    StoragePolicy* policyp = NULL;

    policy.SetExpectedPolicy(StoragePolicy::SP_initial_placement,
            InitialPlacementType::IP_performance_preferred);
    policy.SetRealPolicy(StoragePolicy::SP_initial_placement,
            InitialPlacementType::IP_performance_preferred);
    ret = mPM->SetStoragePolicy(subpoolkey, policy, ywb_false);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ret = mPM->GetStoragePolicy(subpoolkey, &policyp);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ASSERT_EQ(policyp->GetExpectedPolicy(StoragePolicy::SP_initial_placement),
            (ywb_int32_t)InitialPlacementType::IP_performance_preferred);
    ASSERT_EQ(policyp->GetRealPolicy(StoragePolicy::SP_initial_placement),
            (ywb_int32_t)InitialPlacementType::IP_performance_preferred);
}

TEST_F(PolicyManagerTest, SetStoragePolicy_Overlap_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    StoragePolicy policy1;
    StoragePolicy policy2;
    StoragePolicy* policyp = NULL;

    mPM->SetStoragePolicy(subpoolkey, policy1, ywb_true);
    policy2.SetExpectedPolicy(StoragePolicy::SP_initial_placement,
            InitialPlacementType::IP_performance_preferred);
    policy2.SetRealPolicy(StoragePolicy::SP_initial_placement,
            InitialPlacementType::IP_performance_preferred);
    ret = mPM->SetStoragePolicy(subpoolkey, policy2, ywb_true);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ret = mPM->GetStoragePolicy(subpoolkey, &policyp);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ASSERT_EQ(policyp->GetExpectedPolicy(StoragePolicy::SP_initial_placement),
            (ywb_int32_t)InitialPlacementType::IP_performance_preferred);
    ASSERT_EQ(policyp->GetRealPolicy(StoragePolicy::SP_initial_placement),
            (ywb_int32_t)InitialPlacementType::IP_performance_preferred);
}

TEST_F(PolicyManagerTest, SetStoragePolicy_NoOverlap_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    StoragePolicy policy1;
    StoragePolicy policy2;
    StoragePolicy* policyp = NULL;

    policy1.SetExpectedPolicy(StoragePolicy::SP_initial_placement,
            InitialPlacementType::IP_performance_preferred);
    policy1.SetRealPolicy(StoragePolicy::SP_initial_placement,
            InitialPlacementType::IP_performance_preferred);
    mPM->SetStoragePolicy(subpoolkey, policy1, ywb_true);
    policy2.SetExpectedPolicy(StoragePolicy::SP_initial_placement,
            InitialPlacementType::IP_cost_preferred);
    policy2.SetRealPolicy(StoragePolicy::SP_initial_placement,
            InitialPlacementType::IP_cost_preferred);
    ret = mPM->SetStoragePolicy(subpoolkey, policy2, ywb_false);
    ASSERT_EQ(YFS_EEXIST, ret);
    ret = mPM->GetStoragePolicy(subpoolkey, &policyp);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ASSERT_EQ(policyp->GetExpectedPolicy(StoragePolicy::SP_initial_placement),
            (ywb_int32_t)InitialPlacementType::IP_performance_preferred);
    ASSERT_EQ(policyp->GetRealPolicy(StoragePolicy::SP_initial_placement),
            (ywb_int32_t)InitialPlacementType::IP_performance_preferred);
}

TEST_F(PolicyManagerTest, SetDefaultStoragePolicy_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    StoragePolicy policy;
    StoragePolicy* policyp = NULL;

    ret = mPM->SetDefaultStoragePolicy(subpoolkey);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ret = mPM->GetStoragePolicy(subpoolkey, &policyp);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ASSERT_EQ(policyp->GetExpectedPolicy(StoragePolicy::SP_initial_placement),
            (ywb_int32_t)InitialPlacementType::IP_auto);
    ASSERT_EQ(policyp->GetRealPolicy(StoragePolicy::SP_initial_placement),
            (ywb_int32_t)InitialPlacementType::IP_auto);
}

TEST_F(PolicyManagerTest, RemoveStoragePolicy_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    StoragePolicy policy;
    StoragePolicy* policyp = NULL;

    mPM->SetDefaultStoragePolicy(subpoolkey);
    mPM->GetStoragePolicy(subpoolkey, &policyp);
    ret = mPM->RemoveStoragePolicy(subpoolkey);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ret = mPM->GetStoragePolicy(subpoolkey, &policyp);
    ASSERT_EQ(YFS_ENOENT, ret);
}

TEST_F(PolicyManagerTest, AddDiskSelector_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    DiskSelector* ds = NULL;

    ds = new DiskSelector(subpoolkey, DiskSelector::DSB_performance_based_v1,
            DiskSelector::DSM_ticket_by_ticket_random, mPM);
    ret = mPM->AddDiskSelector(subpoolkey, ds);
    ASSERT_EQ(YWB_SUCCESS, ret);
}

TEST_F(PolicyManagerTest, AddDiskSelector_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    DiskSelector* ds = NULL;

    ds = new DiskSelector(subpoolkey, DiskSelector::DSB_performance_based_v1,
            DiskSelector::DSM_ticket_by_ticket_random, mPM);
    mPM->AddDiskSelector(subpoolkey, ds);
    ret = mPM->AddDiskSelector(subpoolkey, ds);
    ASSERT_EQ(YFS_EEXIST, ret);
}

TEST_F(PolicyManagerTest, GetDiskSelector_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    DiskSelector* ds = NULL;

    ret = mPM->GetDiskSelector(subpoolkey, &ds);
    ASSERT_EQ(YFS_ENOENT, ret);
}

TEST_F(PolicyManagerTest, GetDiskSelector_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    DiskSelector* ds = NULL;
    DiskSelector* ds2 = NULL;

    ds = new DiskSelector(subpoolkey, DiskSelector::DSB_performance_based_v1,
            DiskSelector::DSM_ticket_by_ticket_random, mPM);
    mPM->AddDiskSelector(subpoolkey, ds);
    ret = mPM->GetDiskSelector(subpoolkey, &ds2);
    ASSERT_EQ(YWB_SUCCESS, ret);
}

TEST_F(PolicyManagerTest, RemoveDiskSelector_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);
    DiskSelector* ds = NULL;
    DiskSelector* ds2 = NULL;

    ds = new DiskSelector(subpoolkey, DiskSelector::DSB_performance_based_v1,
            DiskSelector::DSM_ticket_by_ticket_random, mPM);
    mPM->AddDiskSelector(subpoolkey, ds);
    ret = mPM->RemoveDiskSelector(subpoolkey);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ret = mPM->GetDiskSelector(subpoolkey, &ds2);
    ASSERT_EQ(YFS_ENOENT, ret);
}

TEST_F(PolicyManagerTest, RemoveDiskSelector_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey(1, 1, 1);

    ret = mPM->RemoveDiskSelector(subpoolkey);
    ASSERT_EQ(YFS_ENOENT, ret);
}

TEST_F(TierWiseDiskCandidatesTest, GetNextDiskCandidate_DiskByDiskRR)
{
    ywb_uint32_t ret = 0;
    std::vector<ywb_uint64_t> diskidvec;

    PrepareForGetNextDiskCandidateDiskByDiskRR();
    ret = GetSequenceOfNextDiskCandidateDiskByDiskRR(diskidvec);
    ASSERT_EQ(CheckSequenceOfGetNextDiskCandidateDiskByDiskRR(ret, diskidvec), ywb_true);
}

TEST_F(TierWiseDiskCandidatesTest, GetNextDiskCandidate_TicketByTicketRR)
{
    ywb_uint32_t ret = 0;
    std::vector<ywb_uint64_t> diskidvec;

    PrepareForGetNextDiskCandidateTicketByTicketRR();
    ret = GetSequenceOfNextDiskCandidateTicketByTicketRR(diskidvec);
    ASSERT_EQ(CheckSequenceOfGetNextDiskCandidateTicketByTicketRR(ret, diskidvec), ywb_true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_NoDiskCandidate)
{
    ywb_uint32_t ret = 0;

    ret = mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(ret, 0);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_AvailRatioReachHighThreshold)
{
    Prepare_DSBCapProportional_AvailRatioReachHighThreshold();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_AvailRatioReachHighThreshold(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_AvailRatioReachMediumThreshold)
{
    Prepare_DSBCapProportional_AvailRatioReachMediumThreshold();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_AvailRatioReachMediumThreshold(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_AvailRatioReachLowThreshold)
{
    Prepare_DSBCapProportional_AvailRatioReachLowThreshold();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_AvailRatioReachLowThreshold(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_AvailRatioNotReachLowThreshold)
{
    Prepare_DSBCapProportional_AvailRatioNotReachLowThreshold();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_AvailRatioNotReachLowThreshold(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_ReachTierThreshold)
{
    Prepare_DSBCapProportional_ReachTierThreshold();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_ReachTierThreshold(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgNotZero)
{
    Prepare_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgNotZero();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgNotZero(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgIsZero)
{
    Prepare_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgIsZero();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgIsZero(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_NoDiskReachThreshold)
{
    Prepare_DSBCapProportional_NoDiskReachThreshold();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_NoDiskReachThreshold(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsApproximateEqual)
{
    Prepare_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsApproximateEqual();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsApproximateEqual(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsNotApproximateEqual)
{
    Prepare_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsApproximateEqual();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsApproximateEqual(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsApproximateEqual)
{
    Prepare_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsApproximateEqual();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsApproximateEqual(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsNotApproximateEqual)
{
    Prepare_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsNotApproximateEqual();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsNotApproximateEqual(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_RemainingAvailCapReachWaterMark)
{
    Prepare_DSBCapProportional_RemainingAvailCapReachWaterMark();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_RemainingAvailCapReachWaterMark(), true);
}

TEST_F(TierWiseDiskCandidatesTest, Update_DSBCapProportional_RemainingAvailCapNotReachWaterMark)
{
    Prepare_DSBCapProportional_RemainingAvailCapNotReachWaterMark();
    mTWDC->Update(DiskSelector::DSB_capacity_proportional);
    ASSERT_EQ(Check_DSBCapProportional_RemainingAvailCapNotReachWaterMark(), true);
}

/* vim:set ts=4 sw=4 expandtab */
