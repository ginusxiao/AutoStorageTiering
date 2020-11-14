/*
 * StoragePolicyTests.cpp
 *
 *  Created on: 2016年10月16日
 *      Author: Administrator
 */
#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/StoragePolicy/StoragePolicyTests.hpp"

ControlCenter* gCenter = NULL;

void
TierWiseDiskCandidatesTest::SetUp()
{
    ywb_uint32_t diskid = 0;
    DiskKey diskkey(0, subpool.GetSubPoolId());

    for (diskid = 0; diskid < 5; diskid++) {
        diskkey.SetDiskId(diskid);
        mTWDC->AddDiskCandidate(diskkey);
    }
}

void
TierWiseDiskCandidatesTest::TearDown()
{
    ywb_uint32_t diskid = 0;
    DiskKey diskkey(0, subpool.GetSubPoolId());

    for (diskid = 0; diskid < 10; diskid++) {
        diskkey.SetDiskId(diskid);
        mTWDC->RemoveDiskCandidate(diskkey);
    }
}

void
TierWiseDiskCandidatesTest::PrepareForGetNextDiskCandidateDiskByDiskRR()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(3);
    mTWDC->AddDiskCandidateVec(dc);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(2);
    mTWDC->AddDiskCandidateVec(dc);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(2);
    mTWDC->AddDiskCandidateVec(dc);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(3);
    mTWDC->AddDiskCandidateVec(dc);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(1);
    mTWDC->AddDiskCandidateVec(dc);

    mTWDC->SetAvailTicketsNum(3+2+2+3+1);
}

void
TierWiseDiskCandidatesTest::PrepareForGetNextDiskCandidateTicketByTicketRR()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(3);
    mTWDC->AddDiskCandidateVec(dc);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(2);
    mTWDC->AddDiskCandidateVec(dc);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(2);
    mTWDC->AddDiskCandidateVec(dc);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(3);
    mTWDC->AddDiskCandidateVec(dc);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetAvailTickets(1);
    mTWDC->AddDiskCandidateVec(dc);

    mTWDC->SetAvailTicketsNum(3+2+2+3+1);
}

ywb_uint32_t
TierWiseDiskCandidatesTest::GetSequenceOfNextDiskCandidateDiskByDiskRR(
        std::vector<ywb_uint64_t>& diskidvec)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    ywb_int32_t mode = DiskSelector::DSM_disk_by_disk_round_robin;

    ret = mTWDC->GetNextDiskCandidate(mode, diskkey);
    while (YWB_SUCCESS == ret) {
        diskidvec.push_back(diskkey.GetDiskId());
        ret = mTWDC->GetNextDiskCandidate(mode, diskkey);
    }

    return diskidvec.size();
}

ywb_uint32_t
TierWiseDiskCandidatesTest::GetSequenceOfNextDiskCandidateTicketByTicketRR(
        std::vector<ywb_uint64_t>& diskidvec)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    ywb_int32_t mode = DiskSelector::DSM_ticket_by_ticket_round_robin;

    ret = mTWDC->GetNextDiskCandidate(mode, diskkey);
    while (YWB_SUCCESS == ret) {
        diskidvec.push_back(diskkey.GetDiskId());
        ret = mTWDC->GetNextDiskCandidate(mode, diskkey);
    }

    return diskidvec.size();
}

ywb_bool_t
TierWiseDiskCandidatesTest::CheckVectorEqual(
        std::vector<ywb_uint64_t>& diskidvec1,
        std::vector<ywb_uint64_t>& diskidvec2)
{
    ywb_uint32_t index = 0;

    if (diskidvec1.size() == diskidvec2.size()) {
        for (index = 0; index < diskidvec1.size(); index++) {
            if (diskidvec1[index] != diskidvec2[index]) {
                return ywb_false;
            }
        }
    }

    return ywb_true;
}

ywb_bool_t
TierWiseDiskCandidatesTest::CheckSequenceOfGetNextDiskCandidateDiskByDiskRR(
        ywb_uint32_t sequencenum, std::vector<ywb_uint64_t>& diskidvec)
{
    ywb_uint64_t diskid = 0;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    ywb_uint64_t diskidarray[11] = {0, 0, 0, 1, 1, 2, 2, 3, 3, 3, 4};
    std::vector<ywb_uint64_t> diskidvec2(diskidarray, diskidarray + 11);
    DiskCandidate* dc = NULL;

    YWB_ASSERT(sequencenum == 11);
    YWB_ASSERT(CheckVectorEqual(diskidvec, diskidvec2) == ywb_true);
    YWB_ASSERT(mTWDC->GetAvailTicketsNum() == 0);
    for (diskid = 0; diskid < 5; diskid++) {
        diskkey.SetDiskId(diskid);
        mTWDC->GetDiskCandidate(diskkey, &dc);
        YWB_ASSERT(dc->GetAvailTickets() == 0);
    }

    return ywb_true;
}

ywb_bool_t
TierWiseDiskCandidatesTest::CheckSequenceOfGetNextDiskCandidateTicketByTicketRR(
        ywb_uint32_t sequencenum, std::vector<ywb_uint64_t>& diskidvec)
{
    ywb_uint64_t diskid = 0;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    ywb_uint64_t diskidarray[11] = {0, 1, 2, 3, 4, 0, 1, 2, 3, 0, 3};
    std::vector<ywb_uint64_t> diskidvec2(diskidarray, diskidarray + 11);
    DiskCandidate* dc = NULL;

    YWB_ASSERT(sequencenum == 11);
    YWB_ASSERT(CheckVectorEqual(diskidvec, diskidvec2) == ywb_true);
    YWB_ASSERT(mTWDC->GetAvailTicketsNum() == 0);
    for (diskid = 0; diskid < 5; diskid++) {
        diskkey.SetDiskId(diskid);
        mTWDC->GetDiskCandidate(diskkey, &dc);
        YWB_ASSERT(dc->GetAvailTickets() == 0);
    }

    return ywb_true;
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_AvailRatioReachHighThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(1);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_AvailRatioReachMediumThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(8);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_AvailRatioReachLowThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(18);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_AvailRatioNotReachLowThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(1);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(1);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(1);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(100);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_ReachTierThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(22);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(22);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(22);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(22);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(22);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgNotZero()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(16);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(60);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(60);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(60);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgIsZero()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(0);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(0);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(60);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(60);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(60);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_NoDiskReachThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(800);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(700);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(700);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(600);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(700);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsApproximateEqual()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsNotApproximateEqual()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(2);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsApproximateEqual()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(70);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(71);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(70);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(70);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(71);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsNotApproximateEqual()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(70);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(80);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(70);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(70);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(60);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_RemainingAvailCapReachWaterMark()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(100);
    dc->SetAvailCap(10);
}

void
TierWiseDiskCandidatesTest::Prepare_DSBCapProportional_RemainingAvailCapNotReachWaterMark()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(900);

    diskkey.SetDiskId(1);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(700);

    diskkey.SetDiskId(2);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(700);

    diskkey.SetDiskId(3);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(500);

    diskkey.SetDiskId(4);
    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    dc->SetTotalCap(1000);
    dc->SetAvailCap(700);
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_AvailRatioReachHighThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    YWB_ASSERT(true == dc->TestFlag(DiskCandidate::DCF_swap_high));

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_AvailRatioReachMediumThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(1, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    YWB_ASSERT(true == dc->TestFlag(DiskCandidate::DCF_swap_medium));

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_AvailRatioReachLowThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(2, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    YWB_ASSERT(true == dc->TestFlag(DiskCandidate::DCF_swap_low));

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_AvailRatioNotReachLowThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(3, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    ret = mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(ret == YWB_SUCCESS);
    YWB_ASSERT((false == dc->TestFlag(DiskCandidate::DCF_swap_high)) &&
            (false == dc->TestFlag(DiskCandidate::DCF_swap_medium)) &&
                    (false == dc->TestFlag(DiskCandidate::DCF_swap_low)));

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_ReachTierThreshold()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    for (diskid = 0; diskid < 5; diskid++)
    {
        diskkey.SetDiskId(diskid);
        ret = mTWDC->GetDiskCandidate(diskkey, &dc);
        YWB_ASSERT(ret == YWB_SUCCESS);
        YWB_ASSERT(true == dc->TestFlag(DiskCandidate::DCF_swap_low));
    }

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgNotZero()
{
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    diskkey.SetDiskId(0);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    diskkey.SetDiskId(1);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    diskkey.SetDiskId(2);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(47 == dc->GetAvailTickets());

    diskkey.SetDiskId(3);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(47 == dc->GetAvailTickets());

    diskkey.SetDiskId(4);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(47 == dc->GetAvailTickets());

    YWB_ASSERT(141 == mTWDC->GetAvailTicketsNum());

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgIsZero()
{
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    diskkey.SetDiskId(0);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    diskkey.SetDiskId(1);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    diskkey.SetDiskId(2);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(40 == dc->GetAvailTickets());

    diskkey.SetDiskId(3);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(40 == dc->GetAvailTickets());

    diskkey.SetDiskId(4);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(40 == dc->GetAvailTickets());

    YWB_ASSERT(120 == mTWDC->GetAvailTicketsNum());

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_NoDiskReachThreshold()
{
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    diskkey.SetDiskId(0);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(100 == dc->GetAvailTickets());

    diskkey.SetDiskId(1);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    diskkey.SetDiskId(2);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    diskkey.SetDiskId(3);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    diskkey.SetDiskId(4);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    YWB_ASSERT(100 == mTWDC->GetAvailTicketsNum());

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsApproximateEqual()
{
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    diskkey.SetDiskId(0);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(10 == dc->GetAvailTickets());

    diskkey.SetDiskId(1);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(10 == dc->GetAvailTickets());

    diskkey.SetDiskId(2);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(10 == dc->GetAvailTickets());

    diskkey.SetDiskId(3);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(10 == dc->GetAvailTickets());

    diskkey.SetDiskId(4);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(10 == dc->GetAvailTickets());

    YWB_ASSERT(50 == mTWDC->GetAvailTicketsNum());

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsNotApproximateEqual()
{
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    diskkey.SetDiskId(0);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(8 == dc->GetAvailTickets());

    diskkey.SetDiskId(1);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    diskkey.SetDiskId(2);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(8 == dc->GetAvailTickets());

    diskkey.SetDiskId(3);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(8 == dc->GetAvailTickets());

    diskkey.SetDiskId(4);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(8 == dc->GetAvailTickets());

    YWB_ASSERT(32 == mTWDC->GetAvailTicketsNum());

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsApproximateEqual()
{
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    diskkey.SetDiskId(0);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(70 == dc->GetAvailTickets());

    diskkey.SetDiskId(1);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(71 == dc->GetAvailTickets());

    diskkey.SetDiskId(2);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(70 == dc->GetAvailTickets());

    diskkey.SetDiskId(3);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(70 == dc->GetAvailTickets());

    diskkey.SetDiskId(4);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(71 == dc->GetAvailTickets());

    YWB_ASSERT(352 == mTWDC->GetAvailTicketsNum());

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsNotApproximateEqual()
{
    DiskKey diskkey(0, subpool.GetSubPoolId());
    DiskCandidate* dc = NULL;

    diskkey.SetDiskId(0);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(10 == dc->GetAvailTickets());

    diskkey.SetDiskId(1);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(20 == dc->GetAvailTickets());

    diskkey.SetDiskId(2);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(10 == dc->GetAvailTickets());

    diskkey.SetDiskId(3);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(10 == dc->GetAvailTickets());

    diskkey.SetDiskId(4);
    mTWDC->GetDiskCandidate(diskkey, &dc);
    YWB_ASSERT(0 == dc->GetAvailTickets());

    YWB_ASSERT(50 == mTWDC->GetAvailTicketsNum());

    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_RemainingAvailCapReachWaterMark()
{
    YWB_ASSERT(true == mTWDC->TestFlag(TierWiseDiskCandidates::TC_exhausted));
    return true;
}

bool
TierWiseDiskCandidatesTest::Check_DSBCapProportional_RemainingAvailCapNotReachWaterMark()
{
    YWB_ASSERT(false == mTWDC->TestFlag(TierWiseDiskCandidates::TC_exhausted));
    return true;
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

    testing::AddGlobalTestEnvironment(new StoragePolicyTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */

