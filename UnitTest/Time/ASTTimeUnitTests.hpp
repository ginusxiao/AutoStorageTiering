#ifndef __AST_TIME_UNIT_TEST_HPP__
#define __AST_TIME_UNIT_TEST_HPP__

#include <stdio.h>
#include "gtest/gtest.h"
#include "AST/ASTTime.hpp"

class ASTTimeTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class ASTCycleManagerTest: public testing::Test
{
public:
    ASTCycleManagerTest()
    {
        mCycleMgr = new CycleManager(2);
    }

    virtual ~ASTCycleManagerTest()
    {
        if(mCycleMgr)
        {
            delete mCycleMgr;
            mCycleMgr = NULL;
        }
    }

    virtual void SetUp() {}
    virtual void TearDown() {}

    CycleManager* GetCycleManager();

private:
    CycleManager* mCycleMgr;
};

class ASTDecisionWindowManagerTest: public testing::Test
{
public:
    ASTDecisionWindowManagerTest()
    {
        ywb_uint32_t cycleshare[2] = {0, 2};
        mDecisionWindowMgr = new DecisionWindowManager(2, cycleshare);
    }

    virtual ~ASTDecisionWindowManagerTest()
    {
        if(mDecisionWindowMgr)
        {
            delete mDecisionWindowMgr;
            mDecisionWindowMgr = NULL;
        }
    }

    virtual void SetUp() {}
    virtual void TearDown() {}

    DecisionWindowManager* GetDecisionWindowManager();

private:
    DecisionWindowManager* mDecisionWindowMgr;
};

/*FIXME: how to?*/
class ASTTimerTest: public testing::Test
{
public:
    ASTTimerTest()
    {

    }

    virtual ~ASTTimerTest()
    {
    }

    virtual void SetUp() {}
    virtual void TearDown() {}

private:

};

/*FIXME: how to?*/
class ASTTimeManagerTest: public testing::Test
{
public:
    ASTTimeManagerTest()
    {

    }

    virtual ~ASTTimeManagerTest()
    {
    }

    virtual void SetUp() {}
    virtual void TearDown() {}

private:

};

#endif

/* vim:set ts=4 sw=4 expandtab */

