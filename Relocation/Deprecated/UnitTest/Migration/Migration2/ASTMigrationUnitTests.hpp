#ifndef __AST_MIGRATION_UNIT_TEST_HPP__
#define __AST_MIGRATION_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTMigration.hpp"

class ASTMigrationTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class MigrationDiskTest: public testing::Test
{
public:
    MigrationDiskTest()
    {
        mMigrationDisk = new MigrationDisk();
        YWB_ASSERT(mMigrationDisk != NULL);
        mMigrationDisk2 = new MigrationDisk();
        YWB_ASSERT(mMigrationDisk2 != NULL);
    }

    virtual ~MigrationDiskTest()
    {
        if(mMigrationDisk)
        {
            delete mMigrationDisk;
            mMigrationDisk = NULL;
        }

        if(mMigrationDisk2)
        {
            delete mMigrationDisk2;
            mMigrationDisk2 = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    MigrationDisk* GetMigrationDisk();
    MigrationDisk* GetMigrationDisk2();

private:
    MigrationDisk* mMigrationDisk;
    /*specially for empty MigrationDisk test*/
    MigrationDisk* mMigrationDisk2;
};

class HUBMock : public HUB
{
public:
    HUBMock(ControlCenter* cc, AST* ast) : HUB(cc)
    {
        YWB_ASSERT(ast != NULL);
        mAST = ast;
        SetEnableOrDisableASTSync(ywb_true);
    }

    void LaunchMigrateEvent(OBJKey objkey, DiskKey tgtdisk)
    {
        Migration* migrationmgr = NULL;
        Migrator* curmigrator = NULL;

        mAST->GetMigration(&migrationmgr);
        migrationmgr->GetCurrentMigrator(&curmigrator);
        YWB_ASSERT(curmigrator != NULL);
        curmigrator->LaunchCompleteMigrationEvent(objkey, YWB_SUCCESS);
    }

private:
    AST* mAST;
};

class ControlCenterMock : public ControlCenter
{
public:
    ControlCenterMock(AST* ast) : ControlCenter()
    {
        mCurHUB = new HUBMock(this, ast);
        YWB_ASSERT(mCurHUB != NULL);
        mRawHUB = NULL;
    }

    ~ControlCenterMock()
    {
        if(mCurHUB)
        {
            delete mCurHUB;
            mCurHUB = NULL;
        }
    }

    ywb_status_t Start()
    {
        ywb_status_t ret = YWB_SUCCESS;

        mRawHUB = GetHUB();
        SetHUB(mCurHUB);
        /*to match with Stop() in ControlCenter::Finalize()*/
        mRawHUB->Start();
        ret = mCurHUB->Start();

        a
        return ret;
    }

    ywb_status_t Stop()
    {
        ywb_status_t ret = YWB_SUCCESS;

        ret = mCurHUB->Stop(true);
        SetHUB(mRawHUB);

        return ret;
    }

private:
    /*for saving the previously set HUB*/
    HUB* mRawHUB;
    HUB* mCurHUB;
};

class DefaultMigratorMock : public DefaultMigrator
{
public:
    DefaultMigratorMock(Migration* migration) :
        DefaultMigrator(migration)
    {
        mSubmittedNum = 0;
        mSubmittedFrontNum = 0;
        mPreparedNum = 0;
        mDoneNum = 0;
        mCompletedNum = 0;
        mStopImmediate = false;
    }

    void* entry();
    ywb_status_t Start();
    ywb_status_t Stop();

    void Submit(BaseArbitratee* item);
    void SubmitFront(BaseArbitratee* item);
    ywb_status_t PrepareMigration(Arbitratee* item);
    ywb_status_t DoMigration(Arbitratee* item);
    ywb_status_t CompleteMigration(Arbitratee* item, ywb_status_t migstatus);
    void LaunchCompleteMigrationEvent(OBJKey objkey, ywb_status_t err);

    ywb_uint32_t GetSubmittedNum();
    ywb_uint32_t GetSubmittedFrontNum();
    ywb_uint32_t GetPreparedNum();
    ywb_uint32_t GetDoneNum();
    ywb_uint32_t GetCompletedNum();

    ywb_bool_t GetStopImmediate();
    void SetStopImmediate(ywb_bool_t stopimmediate);

    void CleanUp();

private:
    ywb_uint32_t mSubmittedNum;
    ywb_uint32_t mSubmittedFrontNum;
    ywb_uint32_t mPreparedNum;
    ywb_uint32_t mDoneNum;
    ywb_uint32_t mCompletedNum;
    /*stop DefaultMigratorMock immediately or not*/
    ywb_bool_t mStopImmediate;
    vector<BaseArbitratee*> mArbitrateeVec;
};

class DefaultMigratorTest: public testing::Test
{
public:
    DefaultMigratorTest()
    {
        Migration* migration = NULL;

        mAst = new AST();
        YWB_ASSERT(mAst != NULL);
        mControlCenterMock = new ControlCenterMock(mAst);
        YWB_ASSERT(mControlCenterMock != NULL);
        mAst->SetControlCenter(mControlCenterMock);
        mAst->GetMigration(&migration);
        mDefaultMigratorMock = new DefaultMigratorMock(migration);
        YWB_ASSERT(mDefaultMigratorMock != NULL);
        migration->SetCurrentMigrator(mDefaultMigratorMock);
    }

    virtual ~DefaultMigratorTest()
    {
        Migration* migration = NULL;

        mAst->GetMigration(&migration);
        migration->SetCurrentMigrator(NULL);

        if(mControlCenterMock)
        {
            delete mControlCenterMock;
            mControlCenterMock = NULL;
        }

        if(mDefaultMigratorMock)
        {
            delete mDefaultMigratorMock;
            mDefaultMigratorMock = NULL;
        }

        if(mAst)
        {
            delete mAst;
            mAst = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    void Submit(Arbitratee* item);
    void SubmitFront(Arbitratee* item);

    DefaultMigratorMock* GetDefaultMigratorMock();
    ywb_uint32_t GetStatus();
    ywb_uint32_t GetSubmittedNum();
    ywb_uint32_t GetSubmittedFrontNum();
    ywb_uint32_t GetPreparedNum();
    ywb_uint32_t GetDoneNum();
    ywb_uint32_t GetCompletedNum();

    /*
     * functionally, CleanUp() equals to TearDown(), but we can
     * get the result of CleanUp() rather than TearDown()
     *
     * @stopimmediate: whether to stop it immediately or not
     **/
    void CleanUp(ywb_bool_t stopimmediate);

private:
    AST* mAst;
    ControlCenterMock* mControlCenterMock;
    DefaultMigratorMock* mDefaultMigratorMock;
};

class FakeMigrator : public Migrator
{
public:
    FakeMigrator(Migration* migration) : Migrator(migration)
    {

    }

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

    void LaunchCompleteMigrationEvent(OBJKey objkey, ywb_status_t err)
    {
    }

    ywb_status_t StatPerformance(ywb_uint32_t readPerf,
            ywb_uint32_t writePerf)
    {
        return YWB_SUCCESS;
    }
};

class MigrationTest: public testing::Test
{
public:
    MigrationTest()
    {
        mAst = new AST();
        YWB_ASSERT(mAst != NULL);
        mControlCenterMock = new ControlCenterMock(mAst);
        YWB_ASSERT(mControlCenterMock != NULL);
        mAst->SetControlCenter(mControlCenterMock);
    }

    virtual ~MigrationTest()
    {
        if(mAst)
        {
            delete mAst;
            mAst = NULL;
        }

        if(mControlCenterMock)
        {
            delete mControlCenterMock;
            mControlCenterMock = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    LogicalConfig* GetConfig();
    Migration* GetMigration();

    void RegisterMigrators();
    void DeRegisterMigrators();

    ywb_status_t AddArbitratee(ywb_uint64_t inodeid, Arbitratee* arbitratee);
    ywb_status_t GetArbitratee(ywb_uint64_t inodeid, Arbitratee** arbitratee);
    void RemoveArbitratees();

    Arbitratee* GenerateNewArbitratee(
            ywb_uint32_t subpoolid, ywb_uint64_t srcdiskid,
            ywb_uint64_t tgtdiskid, ywb_uint64_t inodeid,
            ywb_uint32_t cap, ywb_uint32_t direction,
            ywb_uint64_t cycle, ywb_uint64_t epoch);

    ywb_status_t ReceiveArbitratee(
            ywb_uint32_t subpoolid, ywb_uint64_t srcdiskid,
            ywb_uint64_t tgtdiskid, ywb_uint64_t inodeid,
            ywb_uint32_t cap, ywb_uint32_t direction,
            ywb_uint64_t cycle, ywb_uint64_t epoch);
    ywb_status_t Complete(ywb_uint64_t inodeid, ywb_bool_t more);

private:
    AST* mAst;
    ControlCenterMock* mControlCenterMock;
    /*inodeid to Arbitratee mapping table*/
    map<ywb_uint64_t, Arbitratee*> mArbitratees;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
