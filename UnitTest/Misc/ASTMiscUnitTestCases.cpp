#include "UnitTest/AST/Misc/ASTMiscUnitTests.hpp"

TEST_F(FlagTest, SetFlagComb_SUCCESS)
{
    Flag flag;

    ASSERT_EQ(flag.Empty(), true);
    flag.SetFlagComb(FlagTest::F_c);
    flag.SetFlagComb(FlagTest::F_a);
    ASSERT_EQ(flag.TestFlag(FlagTest::F_a), true);
    ASSERT_EQ(flag.TestFlag(FlagTest::F_b), false);
    ASSERT_EQ(flag.TestFlag(FlagTest::F_c), true);
}

TEST_F(FlagTest, SetFlagUniq_SUCCESS)
{
    Flag flag;

    ASSERT_EQ(flag.Empty(), true);
    flag.SetFlagComb(FlagTest::F_c);
    flag.SetFlagComb(FlagTest::F_a);
    flag.SetFlagUniq(FlagTest::F_b);
    ASSERT_EQ(flag.TestFlag(FlagTest::F_a), false);
    ASSERT_EQ(flag.TestFlag(FlagTest::F_b), true);
    ASSERT_EQ(flag.TestFlag(FlagTest::F_c), false);
}

/* vim:set ts=4 sw=4 expandtab */
