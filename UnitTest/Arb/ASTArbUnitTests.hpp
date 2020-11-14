#ifndef __AST_ARB_UNIT_TEST_HPP__
#define __AST_ARB_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTArb.hpp"
#include "AST/ASTTime.hpp"
#include "AST/ASTMigration.hpp"

//extern AST* gAst;
class ASTArbTestEnvironment: public testing::Environment
{
public:
    /*
     * NOTE:
     * This is global setup which will only be called once
     * before first test suite, and precedent test cases
     * may affect successive test cases, more attention
     * should be paid.
     **/
    virtual void SetUp();
    virtual void TearDown();
};

class ArbitrateeTest: public testing::Test
{
public:
    ArbitrateeTest()
    {

    }

    virtual ~ArbitrateeTest()
    {

    }

    virtual void SetUp();
    virtual void TearDown();

private:

};

class SubPoolArbitrateeTest: public testing::Test
{
public:
    SubPoolArbitrateeTest()
    {
        mSubPoolArbitratee = new SubPoolArbitratee();
        YWB_ASSERT(mSubPoolArbitratee != NULL);
    }

    virtual ~SubPoolArbitrateeTest()
    {
        if(mSubPoolArbitratee)
        {
            delete mSubPoolArbitratee;
            mSubPoolArbitratee = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    void SetSubPoolKey(SubPoolKey& subpoolkey, ywb_uint32_t oss,
            ywb_uint32_t pool, ywb_uint32_t subpool);
    void SetArbitrateeKey(ArbitrateeKey& arbitrateekey, ywb_uint64_t disk,
            ywb_uint64_t inode, ywb_uint64_t chunk);

    SubPoolArbitratee* GetSubPoolArbitratee();

private:
    SubPoolArbitratee* mSubPoolArbitratee;
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

class ArbitratorTest: public testing::Test
{
public:
    ArbitratorTest()
    {
        Migration* migration = NULL;

        mAst = new AST();
        YWB_ASSERT(mAst != NULL);
        mAst->GetMigration(&migration);
        mFakeDefaultMigrator = new FakeMigrator(migration);
        YWB_ASSERT(mFakeDefaultMigrator != NULL);
        migration->SetCurrentMigrator(mFakeDefaultMigrator);
    }

    virtual ~ArbitratorTest()
    {
        Migration* migration = NULL;
        Arbitrator* arb = NULL;

        mAst->GetMigration(&migration);
        mAst->GetArb(&arb);
        migration->DrainAllInflight();
        arb->DrainAllArbitratees();
        migration->SetCurrentMigrator(NULL);

        if(mFakeDefaultMigrator)
        {
            delete mFakeDefaultMigrator;
            mFakeDefaultMigrator = NULL;
        }

        if(mAst)
        {
            delete mAst;
            mAst = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    void SetSubPoolKey(SubPoolKey& subpoolkey, ywb_uint32_t oss,
            ywb_uint32_t pool, ywb_uint32_t subpool);
    void SetArbitrateeKey(ArbitrateeKey& arbitrateekey, ywb_uint64_t disk,
            ywb_uint64_t inode, ywb_uint64_t chunk);

    AST* GetAST();
    Arbitrator* GetArbitrator();
    LogicalConfig* GetConfig();
    TimeManager* GetTimeManager();
    CycleManager* GetCycleManager();

    ywb_status_t GetReserved(SubPoolKey& subpoolkey, DiskKey& diskkey,
            ywb_uint32_t perftype, ywb_uint32_t* ios,
            ywb_uint32_t* bw, ywb_uint32_t* cap);
    ywb_status_t GetConsumed(SubPoolKey& subpoolkey, DiskKey& diskkey,
            ywb_uint32_t perftype, ywb_uint32_t* ios,
            ywb_uint32_t* bw, ywb_uint32_t* cap);
    ywb_status_t GetArbitrateeFromIn(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, Arbitratee** arbitratee);
    ywb_status_t GetArbitrateeFromOut(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, Arbitratee** arbitratee);
    ywb_status_t CheckDiskPerf(SubPoolKey& subpoolkey,
            ywb_uint64_t tgtdiskid, ywb_uint32_t perftype,
            ywb_uint32_t ioreserved, ywb_uint32_t bwreserved,
            ywb_uint32_t capreserved, ywb_uint32_t ioconsumed,
            ywb_uint32_t bwconsumed, ywb_uint32_t capconsumed);

    Plan* GenerateNewPlan(ywb_uint32_t subpoolid, ywb_uint64_t srcdiskid,
            ywb_uint64_t tgtdiskid, ywb_uint64_t inodeid, ywb_uint32_t cap,
            ywb_uint32_t direction, ywb_uint64_t epoch);
    void DestroyPlan(Plan* plan);

private:
    /*assigned by ArbitratorTest::SetUp()*/
    AST* mAst;
    /*
     * simulate default migrator to substitute the default migrator
     * in Migration module(which will start new thread)
     **/
    Migrator* mFakeDefaultMigrator;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
