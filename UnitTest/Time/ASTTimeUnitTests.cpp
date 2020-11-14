#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/Time/ASTTimeUnitTests.hpp"

CycleManager*
ASTCycleManagerTest::GetCycleManager()
{
    return mCycleMgr;
}

DecisionWindowManager*
ASTDecisionWindowManagerTest::GetDecisionWindowManager()
{
    return mDecisionWindowMgr;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    std::string logfile =
            YfsDirectorySetting::GetLogDirectory() + Constant::LOG_PATH;

    char ch;
    bool daemon = false;
    bool ignore = false;
    opterr = 0;

    while ((ch = getopt(argc, argv, "gd")) != -1) {
        if (ch == '?') {
            ignore = true;
            continue;
        }
        daemon = true;
    }

    if (daemon && !ignore) {
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    create_logger(logfile, "trace");

    testing::AddGlobalTestEnvironment(new ASTTimeTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
