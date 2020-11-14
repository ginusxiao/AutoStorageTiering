#ifndef __AST_TIME_INL_HPP__
#define __AST_TIME_INL_HPP__

#include "AST/ASTTime.hpp"

ywb_uint32_t
CycleManager::GetCyclePeriod()
{
    return mCyclePeriod;
}

ywb_uint64_t
CycleManager::GetCycleRelative()
{
    return mCurCycleRelative;
}

ywb_uint64_t
CycleManager::GetCycleAbsolute()
{
    return mCurCycleAbsolute;
}

ywb_uint64_t
CycleManager::GetCycleAstEnabled()
{
    return mCycleASTEnabled;
}

void
CycleManager::SetCyclePeriod(ywb_uint32_t val)
{
    mCyclePeriod = val;
}

void
CycleManager::SetCycleRelative(ywb_uint64_t val)
{
    mCurCycleRelative = val;
}

void
CycleManager::SetCycleAbsolute(ywb_uint64_t val)
{
    mCurCycleAbsolute = val;
}

void
CycleManager::SetCycleAstEnabled(ywb_uint64_t val)
{
    mCycleASTEnabled = val;
}

void
CycleManager::SetCycleFlagComb(ywb_uint32_t flag)
{
    mCycleFlag.SetFlagComb(flag);
}

void
CycleManager::SetCycleFlagUniq(ywb_uint32_t flag)
{
    mCycleFlag.SetFlagUniq(flag);
}

void
CycleManager::ClearCycleFlag(ywb_uint32_t flag)
{
    mCycleFlag.ClearFlag(flag);
}

bool
CycleManager::TestCycleFlag(ywb_uint32_t flag)
{
    return mCycleFlag.TestFlag(flag);
}

ywb_uint32_t
DecisionWindowManager::GetCycles()
{
    return mCycles;
}

ywb_uint32_t
DecisionWindowManager::GetCyclesPlusOneSquare()
{
    return mCyclesPlusOneSquare;
}

ywb_uint32_t
DecisionWindowManager::GetMultiplierOne()
{
    return mMultiplierOne;
}

ywb_uint32_t
DecisionWindowManager::GetMultiplierTwo()
{
    return mMultiplierTwo;
}

ywb_uint32_t
DecisionWindowManager::GetCurCycleType()
{
    return mCurCycleType;
}

ywb_bool_t
DecisionWindowManager::GetWindowEnabled()
{
    return mEnabled;
}

ywb_bool_t
DecisionWindowManager::IsNewWindowStarted()
{
    return mIsNewWindowStarted;
}

void
DecisionWindowManager::SetCycles(ywb_uint32_t val)
{
    mCycles = val;
    /*change following three fields accordingly*/
    mCyclesPlusOneSquare = ((mCycles + 1) * (mCycles + 1));
    mMultiplierOne = (mCyclesPlusOneSquare - 2 * (mCycles + 1));
    mMultiplierTwo = 2 * (mCycles + 1);
}

void
DecisionWindowManager::SetCycleShare(
        ywb_uint32_t *share, ywb_uint32_t arraysize)
{
    ywb_uint32_t loop = 0;
    ywb_uint32_t sum = 0;

    YWB_ASSERT(arraysize <= Constant::CYCLE_TYPE_CNT);
    for(loop = 0; loop < Constant::CYCLE_TYPE_CNT; loop++)
    {
        mCyclsShareOfEachCycleType[loop] = 0;
    }

    for(loop = 0; loop < arraysize; loop++)
    {
        mCyclsShareOfEachCycleType[loop] = share[loop];
        sum += share[loop];
    }

    YWB_ASSERT(sum == mCycles);
}

void
DecisionWindowManager::SetCurCycleType(ywb_uint32_t val)
{
    mCurCycleType = val;
}

void
DecisionWindowManager::SetWindowEnabled(ywb_bool_t val)
{
    mEnabled = val;
}

void
TimeManager::GetCycleManager(CycleManager** mgr)
{
    *mgr = mCycleManager;
}

void
TimeManager::GetDecisionWindow(ywb_uint32_t windowindex,
        DecisionWindowManager** mgr)
{
    *mgr = mDecisionWindowManagers[windowindex];
}

void
TimeManager::GetAST(AST** ast)
{
    *ast = mParent;
}

#endif

/* vim:set ts=4 sw=4 expandtab */
