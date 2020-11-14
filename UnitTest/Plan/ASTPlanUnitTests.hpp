#ifndef __AST_PLAN_UNIT_TEST_HPP__
#define __AST_PLAN_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTPlan.hpp"
#include "AST/ASTError.hpp"
#include "AST/ASTMigration.hpp"

//extern Migration* gMigration;

class ASTPlanTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

class DiskScheStatTest: public testing::Test
{
public:
    DiskScheStatTest()
    {
        mDiskScheStat = new DiskScheStat();
        YWB_ASSERT(mDiskScheStat != NULL);
    }

    virtual ~DiskScheStatTest()
    {
        if(mDiskScheStat)
        {
            delete mDiskScheStat;
            mDiskScheStat = NULL;
        }
    }

    virtual void SetUp() {};
    virtual void TearDown() {};

    DiskScheStat* GetDiskScheStat();

private:
    DiskScheStat* mDiskScheStat;
};

class SubPoolPlanDirStatTest: public testing::Test
{
public:
    SubPoolPlanDirStatTest()
    {
        mSubPoolPlanDirStat = new SubPoolPlanDirStat();
        YWB_ASSERT(mSubPoolPlanDirStat);
    }

    virtual ~SubPoolPlanDirStatTest()
    {
        if(mSubPoolPlanDirStat)
        {
            delete mSubPoolPlanDirStat;
            mSubPoolPlanDirStat = NULL;
        }
    }

    virtual void SetUp() {};
    virtual void TearDown() {};

    SubPoolPlanDirStat* GetSubPoolPlanDirStat();

private:
    SubPoolPlanDirStat* mSubPoolPlanDirStat;
};

class SubPoolPlansTest: public testing::Test
{
public:
    SubPoolPlansTest()
    {
        mSubPoolAdvise = new SubPoolAdvice(SubPool::TC_ent);
        YWB_ASSERT(mSubPoolAdvise != NULL);
        mSubPoolPlans = new SubPoolPlans(mSubPoolAdvise);
        YWB_ASSERT(mSubPoolPlans != NULL);
    }

    virtual ~SubPoolPlansTest()
    {
        if(mSubPoolPlans)
        {
            delete mSubPoolPlans;
            mSubPoolPlans = NULL;
        }
        /*
         * @mSubPoolAdvise will be de-constructed as
         * deconstruction of @mSubPoolPlans
         **/
    }

    virtual void SetUp();
    virtual void TearDown();

    SubPoolPlans* GetSubPoolPlans();
    ywb_status_t AddPlan(ywb_uint64_t inodeid);

private:
    SubPoolAdvice* mSubPoolAdvise;
    SubPoolPlans* mSubPoolPlans;
};

class FakeMigrator : public Migrator
{
public:
    FakeMigrator(Migration* migration) : Migrator(migration)
    {

    }

    void Submit(BaseArbitratee* item) {}
    void SubmitFront(BaseArbitratee* item) {}

    ywb_status_t PrepareMigration(Arbitratee* item)
    {
        return YWB_SUCCESS;
    }

    ywb_status_t DoMigration(Arbitratee* item)
    {
        return YWB_SUCCESS;
    }

    ywb_status_t CompleteMigration(Arbitratee* item, ywb_status_t migstatus)
    {
        return YWB_SUCCESS;
    }

    void LaunchCompleteMigrationEvent(OBJKey objkey, ywb_status_t err) {}
    void LaunchCheckExpiredEvent() {}

    ywb_status_t StatPerformance(ywb_uint32_t readPerf,
            ywb_uint32_t writePerf)
    {
        return YWB_SUCCESS;
    }
};

class PlanManagerTest: public testing::Test
{
public:
    PlanManagerTest()
    {
        /*
         * DO NOT use GetAST(), otherwise ~PlanManagerTest()
         * will de-construct it and leave all its internal
         * fields to be NULL
         **/
        mAst = new AST();
        YWB_ASSERT(mAst != NULL);
        mAst->GetLogicalConfig(&mConfig);
        mAst->GetPerformanceProfile(&mProfileMgr);
        mAst->GetAdviseManager(&mAdviceMgr);
        mAst->GetPerfManager(&mPerfMgr);
        mAst->GetPlanManager(&mPlanMgr);
        mAst->GetExecutor(&mExe);
        mAst->GetMigration(&mMigration);
        mAst->GetArb(&mArb);

        mDefaultMigrator = new FakeMigrator(mMigration);
        YWB_ASSERT(mDefaultMigrator != NULL);
        mMigration->SetCurrentMigrator(mDefaultMigrator);
    }

    virtual ~PlanManagerTest()
    {
        mMigration->DrainAllInflight();
        mArb->DrainAllArbitratees();
        mMigration->SetCurrentMigrator(NULL);

        if(mDefaultMigrator)
        {
            delete mDefaultMigrator;
            mDefaultMigrator = NULL;
        }

        if(mAst)
        {
            delete mAst;
            mAst = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    Error* GetError();
    PlanManager* GetPlanManager();
    LogicalConfig* GetConfig();
    PerfManager* GetPerfManager();

    /*
     * Deprecated.
     * Use DrainAllDiskAvailPerf instead if wish to simulate the
     * situation where the available disks can not satisfy the
     * requirement of advice.
     *
     * This routine use GetFirstAdvice and GetNextAdvice which may
     * be used by other code at the same time, but GetFirstAdvice
     * and GetNextAdvice can not guarantee this concurrency
     **/
    void MakeAllAdviseRequiresHuge(
            SubPoolKey subpoolkey, ywb_uint32_t advicetype);
    void DrainTierWiseDiskAvailPerf(SubPoolKey subpoolkey,
            ywb_uint32_t tier, ywb_uint32_t perftype);
    void DrainSubPoolWiseDiskAvailPerf(
            SubPoolKey subpoolkey, ywb_uint32_t perftype);

    void RemoveAllAdvise(SubPoolKey subpoolkey,
            ywb_uint32_t advicetype);
    void AddDisk(SubPoolKey subpoolkey, ywb_uint64_t diskid,
            ywb_uint32_t diskclass, ywb_uint32_t diskrpm);

    void GetAllPlansTarget(SubPoolKey subpoolkey,
            ywb_uint64_t* diskids, ywb_uint32_t* disknum);
    void AddAvailDisks(SubPoolKey subpoolkey,
            ywb_uint64_t* diskids, ywb_uint32_t disknum);

private:
    AST* mAst;
    LogicalConfig* mConfig;
    PerfProfileManager* mProfileMgr;
    AdviceManager* mAdviceMgr;
    PerfManager* mPerfMgr;
    PlanManager* mPlanMgr;
    Executor* mExe;
    Migration* mMigration;
    Migrator* mDefaultMigrator;
    Arbitrator* mArb;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
