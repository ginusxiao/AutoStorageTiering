#include <stdio.h>
#include "FsInts.h"
#include "OSSCommon/Logger.hpp"
#include "UnitTest/AST/Time/ASTTimeUnitTests.hpp"

TEST_F(ASTCycleManagerTest, LaunchNewCycle_SUCCESS)
{
    CycleManager* cyclemgr = NULL;
    ywb_uint64_t absolutecycles1 = 0;
    ywb_uint64_t absolutecycles2 = 0;
    ywb_uint64_t relativecycles = 0;

    cyclemgr = GetCycleManager();
    absolutecycles1 = cyclemgr->GetCycleAbsolute();
    cyclemgr->LaunchNewCycle();
    absolutecycles2 = cyclemgr->GetCycleAbsolute();
    relativecycles = cyclemgr->GetCycleRelative();

    ASSERT_EQ((ywb_uint32_t)absolutecycles2,
            (ywb_uint32_t)(absolutecycles1 + 1));
    ASSERT_EQ((ywb_uint32_t)relativecycles, 1);
}

TEST_F(ASTDecisionWindowManagerTest, LaunchNewCycle_SUCCESS)
{
    DecisionWindowManager* windowmgr = NULL;
    ywb_uint32_t curcycletype = 0;
    ywb_uint32_t relativecycle = 0;
    ywb_uint32_t share[Constant::CYCLE_TYPE_CNT] = {0, 2};

    windowmgr = GetDecisionWindowManager();
    curcycletype = windowmgr->GetCurCycleType();
    ASSERT_EQ(curcycletype, 1);

    relativecycle += 1;
    windowmgr->LaunchNewCycle(relativecycle);
    curcycletype = windowmgr->GetCurCycleType();
    ASSERT_EQ(curcycletype, 1);

    relativecycle += 1;
    windowmgr->LaunchNewCycle(relativecycle);
    curcycletype = windowmgr->GetCurCycleType();
    ASSERT_EQ(curcycletype, 1);

    /*renew the share to be {2, 2}*/
    share[0] = 2;
    windowmgr->SetCycles(4);
    windowmgr->SetCycleShare(share, 2);
    relativecycle += 1;
    windowmgr->LaunchNewCycle(relativecycle);
    curcycletype = windowmgr->GetCurCycleType();
    ASSERT_EQ(curcycletype, 1);

    relativecycle += 1;
    windowmgr->LaunchNewCycle(relativecycle);
    curcycletype = windowmgr->GetCurCycleType();
    ASSERT_EQ(curcycletype, 0);

    relativecycle += 1;
    windowmgr->LaunchNewCycle(relativecycle);
    curcycletype = windowmgr->GetCurCycleType();
    ASSERT_EQ(curcycletype, 0);

    relativecycle += 1;
    windowmgr->LaunchNewCycle(relativecycle);
    curcycletype = windowmgr->GetCurCycleType();
    ASSERT_EQ(curcycletype, 1);
}


/* vim:set ts=4 sw=4 expandtab */
