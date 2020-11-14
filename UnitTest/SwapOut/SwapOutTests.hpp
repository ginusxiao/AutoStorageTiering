/*
 * SwapOutTests.hpp
 *
 *  Created on: 2016年10月16日
 *      Author: Administrator
 */

#ifndef SRC_OSS_UNITTEST_AST_SWAPOUT_SWAPOUTTESTS_HPP_
#define SRC_OSS_UNITTEST_AST_SWAPOUT_SWAPOUTTESTS_HPP_

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTSwapOut.hpp"

class SwapOutTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class SubPoolSwapOutTest : public testing::Test
{
public:
    SubPoolSwapOutTest()
    {
        mSPS = new SubPoolSwapOut();
        YWB_ASSERT(mSPS != NULL);
    }

    virtual ~SubPoolSwapOutTest()
    {
        if(mSPS != NULL)
        {
            delete mSPS;
            mSPS = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    void ScheduleNextSwapDiskTimes(ywb_int32_t swapdirection, ywb_int32_t times,
            std::vector<ywb_uint64_t>& diskidvec);
    bool CheckScheduleNextSwapDiskTimes(ywb_int32_t swapdirection, ywb_int32_t times,
            std::vector<ywb_uint64_t>& diskidvec, std::vector<ywb_uint64_t>& diskidvec2);

    SubPoolSwapOut *mSPS;
};

class SwapOutManagerTest : public testing::Test
{
public:
    SwapOutManagerTest()
    {
    }

    virtual ~SwapOutManagerTest()
    {
    }

    virtual void SetUp();
    virtual void TearDown();
};



#endif /* SRC_OSS_UNITTEST_AST_SWAPOUT_SWAPOUTTESTS_HPP_ */
