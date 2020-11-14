/*
 * SwapOutTests.cpp
 *
 *  Created on: 2016年10月16日
 *      Author: Administrator
 */
#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/SwapOut/SwapOutTests.hpp"

void
SubPoolSwapOutTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_int32_t level = SwapOutManager::SL_max;
    ywb_int32_t direction = SwapOutManager::SD_max;
    ywb_int32_t val = 0;
    DiskKey diskkey(0, 1);

    /*swap up*/
    direction = SwapOutManager::SD_up;
    level = SwapOutManager::SL_high;
    val = SwapOutManager::GetCombinedLevelAndDirection(level, direction);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);

    level = SwapOutManager::SL_medium;
    val = SwapOutManager::GetCombinedLevelAndDirection(level, direction);
    diskkey.SetDiskId(1);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);
    diskkey.SetDiskId(2);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);

    level = SwapOutManager::SL_low;
    val = SwapOutManager::GetCombinedLevelAndDirection(level, direction);
    diskkey.SetDiskId(3);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);
    diskkey.SetDiskId(4);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);
    diskkey.SetDiskId(5);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);

    /*swap down*/
    direction = SwapOutManager::SD_down;
    level = SwapOutManager::SL_high;
    val = SwapOutManager::GetCombinedLevelAndDirection(level, direction);
    diskkey.SetDiskId(6);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);

    level = SwapOutManager::SL_medium;
    val = SwapOutManager::GetCombinedLevelAndDirection(level, direction);
    diskkey.SetDiskId(7);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);
    diskkey.SetDiskId(8);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);

    level = SwapOutManager::SL_low;
    val = SwapOutManager::GetCombinedLevelAndDirection(level, direction);
    diskkey.SetDiskId(9);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);
    diskkey.SetDiskId(10);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);
    diskkey.SetDiskId(11);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);
}

void
SubPoolSwapOutTest::TearDown()
{

}

void
SubPoolSwapOutTest::ScheduleNextSwapDiskTimes(ywb_int32_t swapdirection,
        ywb_int32_t times, std::vector<ywb_uint64_t>& diskidvec)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey(0, 1);
    ywb_int32_t index = 0;

    for(index = 0; index < times; index++)
    {
        ret = mSPS->ScheduleNextSwapDisk(swapdirection, diskkey);
        YWB_ASSERT(YWB_SUCCESS == ret);
        diskidvec.push_back(diskkey.GetDiskId());
    }
}

bool
SubPoolSwapOutTest::CheckScheduleNextSwapDiskTimes(ywb_int32_t swapdirection,
        ywb_int32_t times, std::vector<ywb_uint64_t>& diskidvec,
        std::vector<ywb_uint64_t>& diskidvec2)
{
    ywb_int32_t index = 0;

    for(index = 0; index < times; index++)
    {
        if(diskidvec[index] != diskidvec2[index])
        {
            return false;
        }
    }

    return true;
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

    testing::AddGlobalTestEnvironment(new SwapOutTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */


