#include <stdio.h>
#include <time.h>
#include "OSSCommon/Logger.hpp"
#include "UnitTest/AST/OSSSimulator/OSSSimulatorUnitTests.hpp"

TEST_F(OSSSimulatorTest, None_SUCCESS)
{
    struct timeval now = { 0 };
    struct timespec timeout;

    gettimeofday(&now, NULL);
    /*wait for 5 cycles*/
    timeout.tv_sec = now.tv_sec + (11 * Constant::CYCLE_PERIOD);
    timeout.tv_nsec = now.tv_usec * 1000;
    /*
     * wait until the timer expires
     **/
    Lock();
    TimedWait(&timeout);
    Unlock();
    ast_log_debug("OSSSimulatorTest timed out");
}


/* vim:set ts=4 sw=4 expandtab */
