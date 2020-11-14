/*
 * StoragePolicyTests.hpp
 *
 *  Created on: 2016年10月16日
 *      Author: Administrator
 */

#ifndef SRC_OSS_UNITTEST_AST_STORAGEPOLICY_STORAGEPOLICYTESTS_HPP_
#define SRC_OSS_UNITTEST_AST_STORAGEPOLICY_STORAGEPOLICYTESTS_HPP_

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTStoragePolicy.hpp"
#include "AST/AST.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "OSSCommon/ControlCenter.hpp"

extern ControlCenter* gCenter;
class StoragePolicyTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp()
    {
        mAST = GetSingleASTInstance();
        YWB_ASSERT(mAST != NULL);
        mCenter = new ControlCenter();
        YWB_ASSERT(mCenter != NULL);
        gCenter = mCenter;
    }

    virtual void TearDown()
    {
        if(mAST)
        {
            mAST->DecRef();
        }

        if(mCenter)
        {
            delete mCenter;
            mCenter = NULL;
        }

        gCenter = NULL;
    }

private:
    /*pls refer to ASTHUBTestEnvironment*/
    AST* mAST;
    /*pls refer to ASTHUBTestEnvironment*/
    ControlCenter* mCenter;
};

class PolicyManagerTest : public testing::Test
{
public:
    PolicyManagerTest()
    {
        /*mCtrlCenter = new ControlCenter();
        YWB_ASSERT(mCtrlCenter != NULL);
        mHUB = mCtrlCenter->GetHUB();*/
        mHUB = gCenter->GetHUB();
        YWB_ASSERT(mHUB != NULL);
        mPM = new PolicyManager(mHUB);
        YWB_ASSERT(mPM != NULL);
    }

    virtual ~PolicyManagerTest()
    {
        if (mPM != NULL) {
            delete mPM;
            mPM = NULL;
        }

        if (mHUB != NULL) {
            mHUB = NULL;
        }

        /*if (mCtrlCenter != NULL) {
            delete mCtrlCenter;
            mCtrlCenter = NULL;
        }*/
    }

    virtual void SetUp() {};
    virtual void TearDown() {};

    /*ControlCenter* mCtrlCenter;*/
    HUB* mHUB;
    PolicyManager* mPM;
};

class DiskCandidateTest : public testing::Test
{
public:
    DiskCandidateTest();
    virtual ~DiskCandidateTest();

    virtual void SetUp() {};
    virtual void TearDown() {};
};

class TierWiseDiskCandidatesTest : public testing::Test
{
public:
    TierWiseDiskCandidatesTest() : subpool(1, 1, 1) {
        /*mCtrlCenter = new ControlCenter();
        YWB_ASSERT(mCtrlCenter != NULL);
        mHUB = mCtrlCenter->GetHUB();*/
        mHUB = gCenter->GetHUB();
        YWB_ASSERT(mHUB != NULL);
        mPM = new PolicyManager(mHUB);
        YWB_ASSERT(mPM != NULL);
        mDS = new DiskSelector(subpool, mPM);
        YWB_ASSERT(mPM != NULL);
        mTWDC = new TierWiseDiskCandidates(mDS);
        YWB_ASSERT(mTWDC != NULL);
    }

    virtual ~TierWiseDiskCandidatesTest()
    {
        if (mTWDC != NULL) {
            delete mTWDC;
            mTWDC = NULL;
        }

        if (mDS != NULL) {
            delete mDS;
            mDS = NULL;
        }

        if (mPM != NULL) {
            delete mPM;
            mPM = NULL;
        }

        if (mHUB != NULL) {
            mHUB = NULL;
        }

        /*if (mCtrlCenter != NULL) {
            delete mCtrlCenter;
            mCtrlCenter = NULL;
        }*/
    }

    virtual void SetUp();
    virtual void TearDown();

    void PrepareForGetNextDiskCandidateDiskByDiskRR();
    void PrepareForGetNextDiskCandidateTicketByTicketRR();
    /*
     * return the number of successfully invocation of GetNextDiskCandidate()
     * and the sequence of disk ids got
     * */
    ywb_uint32_t GetSequenceOfNextDiskCandidateDiskByDiskRR(
            std::vector<ywb_uint64_t>& diskidvec);
    /*
     * return the number of successfully invocation of GetNextDiskCandidate()
     * and the sequence of disk ids got
     * */
    ywb_uint32_t GetSequenceOfNextDiskCandidateTicketByTicketRR(
            std::vector<ywb_uint64_t>& diskidvec);

    /*check whether of not the two vector has same element*/
    ywb_bool_t CheckVectorEqual(std::vector<ywb_uint64_t>& diskidvec1,
            std::vector<ywb_uint64_t>& diskidvec2);

    ywb_bool_t CheckSequenceOfGetNextDiskCandidateDiskByDiskRR(ywb_uint32_t,
            std::vector<ywb_uint64_t>& diskidvec);

    ywb_bool_t CheckSequenceOfGetNextDiskCandidateTicketByTicketRR(ywb_uint32_t,
            std::vector<ywb_uint64_t>& diskidvec);

    void Prepare_DSBCapProportional_AvailRatioReachHighThreshold();
    void Prepare_DSBCapProportional_AvailRatioReachMediumThreshold();
    void Prepare_DSBCapProportional_AvailRatioReachLowThreshold();
    void Prepare_DSBCapProportional_AvailRatioNotReachLowThreshold();
    void Prepare_DSBCapProportional_ReachTierThreshold();
    void Prepare_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgNotZero();
    void Prepare_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgIsZero();
    void Prepare_DSBCapProportional_NoDiskReachThreshold();
    void Prepare_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsApproximateEqual();
    void Prepare_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsNotApproximateEqual();
    void Prepare_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsApproximateEqual();
    void Prepare_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsNotApproximateEqual();
    void Prepare_DSBCapProportional_RemainingAvailCapReachWaterMark();
    void Prepare_DSBCapProportional_RemainingAvailCapNotReachWaterMark();

    bool Check_DSBCapProportional_AvailRatioReachHighThreshold();
    bool Check_DSBCapProportional_AvailRatioReachMediumThreshold();
    bool Check_DSBCapProportional_AvailRatioReachLowThreshold();
    bool Check_DSBCapProportional_AvailRatioNotReachLowThreshold();
    bool Check_DSBCapProportional_ReachTierThreshold();
    bool Check_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgNotZero();
    bool Check_DSBCapProportional_SomeDiskReachThreshold_OverloadAvailRatioAvgIsZero();
    bool Check_DSBCapProportional_NoDiskReachThreshold();
    bool Check_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsApproximateEqual();
    bool Check_DSBCapProportional_AllDiskReachThreshold_AllDiskAvailRatioIsNotApproximateEqual();
    bool Check_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsApproximateEqual();
    bool Check_DSBCapProportional_ReservedNotEnough_AllDiskAvailRatioIsNotApproximateEqual();
    bool Check_DSBCapProportional_RemainingAvailCapReachWaterMark();
    bool Check_DSBCapProportional_RemainingAvailCapNotReachWaterMark();

    SubPoolKey subpool;
    /*ControlCenter* mCtrlCenter;*/
    HUB* mHUB;
    PolicyManager* mPM;
    DiskSelector* mDS;
    TierWiseDiskCandidates* mTWDC;
};

class DiskSelectorTest : public testing::Test
{
public:
    DiskSelectorTest();
    virtual ~DiskSelectorTest();

};

class PerformancePreferredDriverTest : public testing::Test
{
public:
    PerformancePreferredDriverTest();
    virtual ~PerformancePreferredDriverTest();

    virtual void SetUp() {};
    virtual void TearDown() {};
};

class AutoDriverTest : public testing::Test
{
public:
    AutoDriverTest();
    virtual ~AutoDriverTest();

    virtual void SetUp() {};
    virtual void TearDown() {};
};

class PerformanceFirstAndThenAutoDriverTest : public testing::Test
{
public:
    PerformanceFirstAndThenAutoDriverTest();
    virtual ~PerformanceFirstAndThenAutoDriverTest();

    virtual void SetUp() {};
    virtual void TearDown() {};
};

class CostPreferredDriverTest : public testing::Test
{
public:
    CostPreferredDriverTest();
    virtual ~CostPreferredDriverTest();

    virtual void SetUp() {};
    virtual void TearDown() {};
};

class NoDataMovementDriverTest : public testing::Test
{
public:
    NoDataMovementDriverTest();
    virtual ~NoDataMovementDriverTest();

    virtual void SetUp() {};
    virtual void TearDown() {};
};

#endif /* SRC_OSS_UNITTEST_AST_STORAGEPOLICY_STORAGEPOLICYTESTS_HPP_ */
