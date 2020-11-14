#ifndef __AST_DISK_PERF_UNIT_TEST_HPP__
#define __AST_DISK_PERF_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTPerformance.hpp"
#include "AST/ASTTime.hpp"

extern AST* gAst;

class ASTDiskPerfTestEnvironment: public testing::Environment
{
public:
    /*
     * NOTE:
     * SetUp() of ASTDiskPerfTestEnvironment will called once
     * at the very beginning of all test suites, and precedent
     * test cases will affect successive test cases.
     **/
    virtual void SetUp();
    virtual void TearDown();
};

class BlackListTest: public testing::Test
{
public:
    BlackListTest()
    {
        mBlackList = new BlackList();
        YWB_ASSERT(mBlackList != NULL);
        mBlackList2 = new BlackList();
        YWB_ASSERT(mBlackList2 != NULL);
        mBlackList3 = new BlackList();
        YWB_ASSERT(mBlackList3 != NULL);
        mBlackList4 = new BlackList();
        YWB_ASSERT(mBlackList4 != NULL);
    }

    virtual ~BlackListTest()
    {
        if(mBlackList)
        {
            delete mBlackList;
            mBlackList = NULL;
        }

        if(mBlackList2)
        {
            delete mBlackList2;
            mBlackList2 = NULL;
        }

        if(mBlackList3)
        {
            delete mBlackList3;
            mBlackList3 = NULL;
        }

        if(mBlackList4)
        {
            delete mBlackList4;
            mBlackList4 = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    BlackList* GetBlackList();
    BlackList* GetBlackList2();
    BlackList* GetBlackList3();
    BlackList* GetBlackList4();

private:
    BlackList* mBlackList;
    /*has intersection with @mBlackList*/
    BlackList* mBlackList2;
    /*has no intersection with @mBlackList*/
    BlackList* mBlackList3;
    /*empty BlackList*/
    BlackList* mBlackList4;
};

class DiskPerfTest: public testing::Test
{
public:
    DiskPerfTest()
    {
        ywb_status_t ret = YWB_SUCCESS;
        DiskKey diskkey;
        AST* ast = NULL;
        LogicalConfig* config = NULL;
        TimeManager* timemgr = NULL;
        CycleManager* cyclemgr = NULL;
        Disk* disk = NULL;
        DiskIO* diskio = NULL;

        ast = gAst;
        ast->GetLogicalConfig(&config);
        ast->GetTimeManager(&timemgr);
        timemgr->GetCycleManager(&cyclemgr);

        diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
        diskkey.SetDiskId(Constant::DISK_ID);
        ret = config->GetDisk(diskkey, &disk);
        YWB_ASSERT(YWB_SUCCESS == ret);
        disk->GetDiskIO(&diskio);
        YWB_ASSERT(diskio != NULL);
        mDiskPerf = new DiskPerf(cyclemgr->GetCyclePeriod(), NULL, diskio, disk);
        YWB_ASSERT(mDiskPerf != NULL);
    }

    virtual ~DiskPerfTest()
    {
        if(mDiskPerf)
        {
            delete mDiskPerf;
            mDiskPerf = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    DiskPerf* GetDiskPerfInternal();
    DiskPerf* GetDiskPerf(ywb_uint32_t subpoolid, ywb_uint64_t diskid);

private:
    /*
     * disk performance with no matched(exactly or fuzzy)
     * disk performance profile
     **/
    DiskPerf* mDiskPerf;
};

class DiskAvailPerfRankingUnitTest: public testing::Test
{
public:
    DiskAvailPerfRankingUnitTest()
    {

    }

    virtual ~DiskAvailPerfRankingUnitTest()
    {

    }

    virtual void SetUp();
    virtual void TearDown();

    DiskAvailPerfRankingUnit* GetDiskAvailPerf(
            ywb_uint32_t subpoolid, ywb_uint64_t diskid);

private:

};

class TierDiskAvailPerfRankingTest: public testing::Test
{
public:
    TierDiskAvailPerfRankingTest()
    {

    }

    virtual ~TierDiskAvailPerfRankingTest()
    {

    }

    virtual void SetUp();
    virtual void TearDown();

    TierDiskAvailPerfRanking* GetTierAvailPerf(
            ywb_uint32_t subpoolid, ywb_uint32_t tier);
    SubPoolDiskAvailPerfRanking* GetSubPoolAvailPerf(ywb_uint32_t subpoolid);

private:

};

class SubPoolDiskAvailPerfRankingTest: public testing::Test
{
public:
    SubPoolDiskAvailPerfRankingTest()
    {

    }

    virtual ~SubPoolDiskAvailPerfRankingTest()
    {

    }

    virtual void SetUp();
    virtual void TearDown();

    SubPoolDiskAvailPerfRanking* GetSubPoolAvailPerf(ywb_uint32_t subpoolid);

private:

};

class PerfManagerTest: public testing::Test
{
public:
    PerfManagerTest()
    {

    }

    virtual ~PerfManagerTest()
    {

    }

    virtual void SetUp();
    virtual void TearDown();

    PerfManager* GetPerfManager();

private:

};

#endif

/* vim:set ts=4 sw=4 expandtab */
