#ifndef __AST_ADVICE_UINT_TEST_HPP__
#define __AST_ADVICE_UINT_TEST_HPP__

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTAdvice.hpp"
#include "AST/ASTDiskUltimatePerfCache.hpp"
#include "AST/AST.hpp"

class ASTAdviceTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class AdviceSetTest: public testing::Test
{
public:
    AdviceSetTest()
    {
        mAdviceSet = new TierBasedAdviceSet();
        YWB_ASSERT(mAdviceSet != NULL);

        mAdviceSet2 = new TierBasedAdviceSet();
        YWB_ASSERT(mAdviceSet2 != NULL);

        mAdviceSet3 = new TierBasedAdviceSet();
        YWB_ASSERT(mAdviceSet3 != NULL);
    }

    virtual ~AdviceSetTest()
    {
        if(mAdviceSet)
        {
            delete mAdviceSet;
            mAdviceSet = NULL;
        }

        if(mAdviceSet2)
        {
            delete mAdviceSet2;
            mAdviceSet2 = NULL;
        }

        if(mAdviceSet3)
        {
            delete mAdviceSet3;
            mAdviceSet3 = NULL;
        }
    }

    /*all advice set will be initialized here*/
    virtual void SetUp();
    virtual void TearDown();

    TierBasedAdviceSet* GetAdviceSet();
    TierBasedAdviceSet* GetAdviceSet2();
    TierBasedAdviceSet* GetAdviceSet3();

private:
    /*Advice set with advise on tier 0 and tier 2*/
    TierBasedAdviceSet* mAdviceSet;
    /*Advice set with advise on only one tier*/
    TierBasedAdviceSet* mAdviceSet2;
    /*Advice set without any advice*/
    TierBasedAdviceSet* mAdviceSet3;
};

class SubPoolAdviceTest: public testing::Test
{
public:
    SubPoolAdviceTest()
    {
        mSubPoolAdvice = new SubPoolAdvice(SubPool::TC_ssd_sata);
    }

    virtual ~SubPoolAdviceTest()
    {
        if(mSubPoolAdvice)
        {
            delete mSubPoolAdvice;
            mSubPoolAdvice = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    SubPoolAdvice* GetSubPoolAdvice();

private:
    SubPoolAdvice* mSubPoolAdvice;
};

class AdviceManagerTest: public testing::Test
{
public:
    AdviceManagerTest()
    {
        mAst = new AST();
        YWB_ASSERT(mAst != NULL);
        mAst->GetAdviseManager(&mAdviceMgr);
    }

    virtual ~AdviceManagerTest()
    {
        if(mAst)
        {
            delete mAst;
            mAst = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    AST* GetAST();
    AdviceManager* GetAdviceManager();
    HeatBucket* GetHeatBucket(SubPoolKey& subpoolkey,
            ywb_uint32_t tier, AdviceRule advicerule);

    void BuildAdviceRule(AdviceRule& rule, ywb_uint32_t advicetype,
            ywb_uint32_t partitionscope, ywb_uint32_t partitionbase,
            ywb_uint32_t sortbase, ywb_uint32_t selector);

    void BuildDefaultHotAdviceRule(AdviceRule& rule);
    void BuildDefaultColdAdviceRule(AdviceRule& rule);
    void BuildDefaultHotFilterRule(AdviceRule& rule);

    /*
     * @sequence: sequence of advice feature value in heat bucket
     **/
    ywb_bool_t CheckHeatBucketAdviceSequence(SubPoolKey& subpoolkey,
            ywb_uint32_t tier, AdviceRule advicerule,
            ywb_uint64_t* sequence, ywb_uint32_t advicenum);
    /*
     * @sequence: sequence of advice feature value in SubPoolAdvice
     **/
    ywb_bool_t CheckSubPoolAdviceSequence(
            SubPoolKey& subpoolkey, AdviceRule advicerule,
            ywb_uint64_t* sequence, ywb_uint32_t advicenum);

    SubPool* BuildSubPoolWithoutTier();
    SubPool* BuildSubPoolWithTierButNoOBJ();
    SubPool* BuildSubPoolWithTierAndOBJs();
    void DestroySubPool(SubPool*);

private:
    AST* mAst;
    AdviceManager* mAdviceMgr;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
