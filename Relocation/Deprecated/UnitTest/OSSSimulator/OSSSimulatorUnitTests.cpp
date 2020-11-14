#include "AST/ASTConstant.hpp"
#include "AST/ASTLogger.hpp"
#include "YfsDirectorySetting.hpp"
#include "UnitTest/AST/OSSSimulator/OSSSimulatorUnitTests.hpp"

void OSSSimulatorTest::SetUp()
{
    mOSSSim->Initialize();
    mOSSSim->Start();
}

void OSSSimulatorTest::TearDown()
{
    mOSSSim->Stop();
}

void OSSSimulatorTest::Lock()
{
    pthread_mutex_lock(&lock);
}

void OSSSimulatorTest::Unlock()
{
    pthread_mutex_unlock(&lock);
}

void OSSSimulatorTest::TimedWait(struct timespec* timeout)
{
    pthread_cond_timedwait(&cond, &lock, timeout);
}

int main(int argc, char *argv[])
{
    int ret = 0;

    /*ControlCenter will create log for us*/
#if 0
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

    create_logger(logfile, "debug", true);
#endif

    testing::AddGlobalTestEnvironment(new OSSSimulatorTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
