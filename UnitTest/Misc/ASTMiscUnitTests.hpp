#ifndef __AST_MISC_UNIT_TEST_HPP__
#define __AST_MISC_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTUtil.hpp"

class ASTMiscTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class FlagTest: public testing::Test
{
public:
    enum {
        F_a,
        F_b,
        F_c,
    };
};

#endif

/* vim:set ts=4 sw=4 expandtab */
