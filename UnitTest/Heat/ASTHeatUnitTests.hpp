#ifndef __AST_HEAT_UNIT_TEST_HPP__
#define __AST_HEAT_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTHeatDistribution.hpp"
#include "AST/ASTTime.hpp"

extern AST* gAst;

class ASTHeatTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

/*TODO:*/
class HeatCmpTest: public testing::Test
{
public:
    HeatCmpTest()
    {

    }

    virtual ~HeatCmpTest()
    {

    }

    virtual void SetUp();
    virtual void TearDown();

private:

};

class HeatBucketTest: public testing::Test
{
public:
    HeatBucketTest()
    {
        ywb_status_t ret = YWB_SUCCESS;
        LogicalConfig* config = NULL;
        SubPoolKey subpoolkey;

        gAst->GetLogicalConfig(&config);
        subpoolkey.SetOss(Constant::OSS_ID);
        subpoolkey.SetPoolId(Constant::POOL_ID);
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

        mHeatMgr = new HeatDistributionManager(gAst);
        YWB_ASSERT(mHeatMgr != NULL);
        ret = config->GetSubPool(subpoolkey, &mSubPool);
        YWB_ASSERT((YWB_SUCCESS == ret) && (mSubPool != NULL));
        mSubPoolHeat = new SubPoolHeatDistribution(mSubPool, mHeatMgr);
        YWB_ASSERT(mSubPoolHeat != NULL);
        mHeatDistribution = new HeatDistribution(mSubPoolHeat);
        YWB_ASSERT(mHeatDistribution != NULL);
        mHeatBucket = new HeatBucket(mHeatDistribution);
        YWB_ASSERT(mHeatBucket != NULL);
        mHeatBucket2 = new HeatBucket(mHeatDistribution);
        YWB_ASSERT(mHeatBucket2 != NULL);
    }

    virtual ~HeatBucketTest()
    {
        if(mHeatBucket)
        {
            delete mHeatBucket;
            mHeatBucket = NULL;
        }

        if(mHeatBucket2)
        {
            delete mHeatBucket2;
            mHeatBucket2 = NULL;
        }

        if(mHeatDistribution)
        {
            delete mHeatDistribution;
            mHeatDistribution = NULL;
        }

        if(mSubPoolHeat)
        {
            delete mSubPoolHeat;
            mSubPoolHeat = NULL;
        }

        if(mHeatMgr)
        {
            delete mHeatMgr;
            mHeatMgr = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    LogicalConfig* GetLogicalConfig();
    HeatBucket* GetHeatBucket();
    HeatBucket* GetHeatBucket2();
//    HeatDistribution* GetHeatDistribution();

    /*prepare @mHeatBucket2 for use*/
    void PrepareHeatBucket2();

private:
    HeatDistributionManager* mHeatMgr;
    SubPoolHeatDistribution* mSubPoolHeat;
    SubPool* mSubPool;
    HeatDistribution* mHeatDistribution;
    /*used for test suite*/
    HeatBucket* mHeatBucket;
    /*only used for GetFirst*** and GetNext*** logical*/
    HeatBucket* mHeatBucket2;
};

/*
 * here will only test those non-derived-class-specified methods,
 * all derived-class-specified methods will be tested in derived-
 * class individually
 **/
class HeatDistributionTest: public testing::Test
{
public:
    HeatDistributionTest()
    {

    }

    virtual ~HeatDistributionTest()
    {

    }

    virtual void SetUp();
    virtual void TearDown();

private:

};

class DiskHeatDistributionTest: public testing::Test
{
public:
    DiskHeatDistributionTest()
    {
        ywb_status_t ret = YWB_SUCCESS;
        LogicalConfig* config = NULL;
        SubPoolKey subpoolkey;
        DiskKey diskkey;

        gAst->GetLogicalConfig(&config);
        subpoolkey.SetOss(Constant::OSS_ID);
        subpoolkey.SetPoolId(Constant::POOL_ID);
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
        diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

        mHeatMgr = new HeatDistributionManager(gAst);
        YWB_ASSERT(mHeatMgr != NULL);
        ret = config->GetSubPool(subpoolkey, &mSubPool);
        YWB_ASSERT((YWB_SUCCESS == ret) && (mSubPool != NULL));
        mSubPoolHeat = new SubPoolHeatDistribution(mSubPool, mHeatMgr);
        YWB_ASSERT(mSubPoolHeat != NULL);
        diskkey.SetDiskId(Constant::DISK_ID);
        ret = mSubPool->GetDisk(diskkey, &mDisk);
        YWB_ASSERT((YWB_SUCCESS == ret) && (mDisk != NULL));
        /*please refer to ASTHeatTestEnvironment::SetUp()*/
        diskkey.SetDiskId(Constant::DISK_ID + 6);
        ret = mSubPool->GetDisk(diskkey, &mDisk2);
        YWB_ASSERT((YWB_SUCCESS == ret) && (mDisk2 != NULL));
        YWB_ASSERT(0 == mDisk2->GetOBJCnt());
        mDiskHeat = new DiskHeatDistribution(mDisk, mSubPoolHeat);
        YWB_ASSERT(mDiskHeat != NULL);
        mDiskHeat2 = new DiskHeatDistribution(mDisk2, mSubPoolHeat);
        YWB_ASSERT(mDiskHeat2 != NULL);
        /*NOTE: here we have not assign advice rule for bucket*/
        mHeatBucket = new HeatBucket(mDiskHeat);
        YWB_ASSERT(mHeatBucket != NULL);
        /*NOTE: here we have not assign advice rule for bucket*/
        mHeatBucket2 = new HeatBucket(mDiskHeat2);
        YWB_ASSERT(mHeatBucket2 != NULL);

        mSubPool->PutDisk(mDisk);
        mSubPool->PutDisk(mDisk2);
        config->PutSubPool(mSubPool);
    }

    virtual ~DiskHeatDistributionTest()
    {
        if(mHeatBucket)
        {
            delete mHeatBucket;
            mHeatBucket = NULL;
        }

        if(mHeatBucket2)
        {
            delete mHeatBucket2;
            mHeatBucket2 = NULL;
        }

        if(mDiskHeat)
        {
            delete mDiskHeat;
            mDiskHeat = NULL;
        }

        if(mDiskHeat2)
        {
            delete mDiskHeat2;
            mDiskHeat2 = NULL;
        }

        if(mSubPoolHeat)
        {
            delete mSubPoolHeat;
            mSubPoolHeat = NULL;
        }

        if(mHeatMgr)
        {
            delete mHeatMgr;
            mHeatMgr = NULL;
        }
    }

    virtual void SetUp() {};
    virtual void TearDown() {};

    Disk* GetDisk();
    Disk* GetDisk2();
    DiskHeatDistribution* GetDiskHeatDistribution();
    DiskHeatDistribution* GetDiskHeatDistribution2();
    HeatBucket* GetHeatBucket();
    HeatBucket* GetHeatBucket2();

//    ywb_bool_t CheckSetupResultAbove(HeatBucket* bucket);
//    ywb_bool_t CheckSetupResultBelow(HeatBucket* bucket);

private:
    HeatDistributionManager* mHeatMgr;
    SubPoolHeatDistribution* mSubPoolHeat;
    SubPool* mSubPool;
    Disk* mDisk;
    /*specially for empty disk test*/
    Disk* mDisk2;
    DiskHeatDistribution* mDiskHeat;
    /*dual to @mDisk2*/
    DiskHeatDistribution* mDiskHeat2;
    HeatBucket* mHeatBucket;
    /*dual to @mDisk2*/
    HeatBucket* mHeatBucket2;
};

/*TODO: currently TierInternalDiskWiseHeatDistribution is not supported*/
class TierInternalDiskWiseHeatDistributionTest: public testing::Test
{
public:
    TierInternalDiskWiseHeatDistributionTest()
    {

    }

    virtual ~TierInternalDiskWiseHeatDistributionTest()
    {

    }

    virtual void SetUp();
    virtual void TearDown();

private:

};

class TierWiseHeatDistributionTest: public testing::Test
{
public:
    TierWiseHeatDistributionTest()
    {
        ywb_status_t ret = YWB_SUCCESS;
        LogicalConfig* config = NULL;
        SubPoolKey subpoolkey;
        DiskKey diskkey;

        gAst->GetLogicalConfig(&config);
        subpoolkey.SetOss(Constant::OSS_ID);
        subpoolkey.SetPoolId(Constant::POOL_ID);
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
        diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

        mHeatMgr = new HeatDistributionManager(gAst);
        YWB_ASSERT(mHeatMgr != NULL);
        ret = config->GetSubPool(subpoolkey, &mSubPool);
        YWB_ASSERT((YWB_SUCCESS == ret) && (mSubPool != NULL));
        mSubPoolHeat = new SubPoolHeatDistribution(mSubPool, mHeatMgr);
        YWB_ASSERT(mSubPoolHeat != NULL);
        mSubPool->GetTier(Tier::TIER_0, &mTier);
        YWB_ASSERT(mTier != NULL);
        mTier2 = new Tier();
        YWB_ASSERT(mTier2 != NULL);
        mTierHeat = new TierWiseHeatDistribution(mTier, mSubPoolHeat);
        YWB_ASSERT(mTierHeat != NULL);
        mTierHeat2 = new TierWiseHeatDistribution(mTier2, mSubPoolHeat);
        YWB_ASSERT(mTierHeat2 != NULL);
        /*NOTE: we have not assign advice rule for bucket here*/
        mHeatBucket = new HeatBucket(mTierHeat);
        YWB_ASSERT(mHeatBucket != NULL);
        mHeatBucket2 = new HeatBucket(mTierHeat2);
        YWB_ASSERT(mHeatBucket2 != NULL);

        mSubPool->PutTier(mTier);
        config->PutSubPool(mSubPool);
    }

    virtual ~TierWiseHeatDistributionTest()
    {
        if(mHeatBucket)
        {
            delete mHeatBucket;
            mHeatBucket = NULL;
        }

        if(mHeatBucket2)
        {
            delete mHeatBucket2;
            mHeatBucket2 = NULL;
        }

        if(mTierHeat)
        {
            delete mTierHeat;
            mTierHeat = NULL;
        }

        if(mTierHeat2)
        {
            delete mTierHeat2;
            mTierHeat2 = NULL;
        }

        if(mSubPoolHeat)
        {
            delete mSubPoolHeat;
            mSubPoolHeat = NULL;
        }

        if(mHeatMgr)
        {
            delete mHeatMgr;
            mHeatMgr = NULL;
        }
    }

    virtual void SetUp() {};
    virtual void TearDown() {};

    Tier* GetTier();
    Tier* GetTier2();
    TierWiseHeatDistribution* GetTierWiseHeatDistribution();
    TierWiseHeatDistribution* GetTierWiseHeatDistribution2();
    HeatBucket* GetHeatBucket();
    HeatBucket* GetHeatBucket2();
    OBJ* GetOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid);
    void RemoveDisk(ywb_uint64_t diskid);

private:
    HeatDistributionManager* mHeatMgr;
    SubPoolHeatDistribution* mSubPoolHeat;
    SubPool* mSubPool;
    Tier* mTier;
    /*specially for empty tier test*/
    Tier* mTier2;
    TierWiseHeatDistribution* mTierHeat;
    /*dual to @mTier2*/
    TierWiseHeatDistribution* mTierHeat2;
    HeatBucket* mHeatBucket;
    /*dual to @mTier2*/
    HeatBucket* mHeatBucket2;
};

class SubPoolHeatDistributionTest: public testing::Test
{
public:
    SubPoolHeatDistributionTest()
    {
        ywb_status_t ret = YWB_SUCCESS;
        LogicalConfig* config = NULL;
        SubPool* subpool = NULL;
        SubPoolKey subpoolkey;
        DiskKey diskkey;

        gAst->GetLogicalConfig(&config);
        subpoolkey.SetOss(Constant::OSS_ID);
        subpoolkey.SetPoolId(Constant::POOL_ID);
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

        mHeatMgr = new HeatDistributionManager(gAst);
        YWB_ASSERT(mHeatMgr != NULL);
        ret = config->GetSubPool(subpoolkey, &subpool);
        YWB_ASSERT((YWB_SUCCESS == ret) && (subpool != NULL));
        mSubPoolHeat = new SubPoolHeatDistribution(subpool, mHeatMgr);
        YWB_ASSERT(mSubPoolHeat != NULL);
    }

    virtual ~SubPoolHeatDistributionTest()
    {
        if(mSubPoolHeat)
        {
            delete mSubPoolHeat;
            mSubPoolHeat = NULL;
        }

        if(mHeatMgr)
        {
            delete mHeatMgr;
            mHeatMgr = NULL;
        }
    }

    virtual void SetUp() {};
    virtual void TearDown() {};

    SubPoolHeatDistribution* GetSubPoolHeat();

private:
    HeatDistributionManager* mHeatMgr;
    SubPoolHeatDistribution* mSubPoolHeat;
};

class HeatDistributionManagerTest: public testing::Test
{
public:
    HeatDistributionManagerTest()
    {
        mHeatMgr = new HeatDistributionManager(gAst);
        YWB_ASSERT(mHeatMgr != NULL);
    }

    virtual ~HeatDistributionManagerTest()
    {
        if(mHeatMgr)
        {
            delete mHeatMgr;
            mHeatMgr = NULL;
        }
    }

    virtual void SetUp() {};
    virtual void TearDown() {};

    HeatDistributionManager* GetHeatManager();

private:
    HeatDistributionManager* mHeatMgr;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
