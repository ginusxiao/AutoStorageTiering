/*
 * SwapOutTestCases.cpp
 *
 *  Created on: 2016年10月16日
 *      Author: Administrator
 */

#include "UnitTest/AST/SwapOut/SwapOutTests.hpp"

TEST_F(SubPoolSwapOutTest, AddSwapDisk_NewDisk_YWB_SUCCESS)
{
    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_up), 6);
    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_down), 6);

    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_up, SwapOutManager::SL_high), 1);
    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_down, SwapOutManager::SL_high), 1);

    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_up, SwapOutManager::SL_medium), 2);
    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_down, SwapOutManager::SL_medium), 2);

    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_up, SwapOutManager::SL_low), 3);
    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_down, SwapOutManager::SL_low), 3);
}

TEST_F(SubPoolSwapOutTest, AddSwapDisk_ExistedDiskWithDifferentSwapDireciton_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_int32_t level = SwapOutManager::SL_high;
    ywb_int32_t direction = SwapOutManager::SD_down;
    ywb_int32_t val = 0;
    DiskKey diskkey(0, 1);

    val = SwapOutManager::GetCombinedLevelAndDirection(level, direction);
    ret = mSPS->AddSwapDisk(diskkey, val);
    ASSERT_EQ(YWB_SUCCESS, ret);

    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_up, SwapOutManager::SL_high), 1);
    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_down, SwapOutManager::SL_high), 2);
}

TEST_F(SubPoolSwapOutTest, AddSwapDisk_ExistedDiskWithDifferentSwapLevel_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_int32_t level = SwapOutManager::SL_medium;
    ywb_int32_t direction = SwapOutManager::SD_up;
    ywb_int32_t val = 0;
    DiskKey diskkey(0, 1);

    val = SwapOutManager::GetCombinedLevelAndDirection(level, direction);
    ret = mSPS->AddSwapDisk(diskkey, val);
    YWB_ASSERT(YWB_SUCCESS == ret);

    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_up, SwapOutManager::SL_high), 0);
    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_up, SwapOutManager::SL_medium), 3);
}

TEST_F(SubPoolSwapOutTest, RemoveSwapDisk_YWB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_int64_t diskid = 0;
    DiskKey diskkey(0, 1);

    for(diskid = 0; diskid < 12; diskid++)
    {
        diskkey.SetDiskId(diskid);
        ret = mSPS->RemoveSwapDisk(diskkey);
        YWB_ASSERT(YWB_SUCCESS == ret);
    }

    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_up), 0);
    ASSERT_EQ(mSPS->GetSwapRequiredDiskNum(SwapOutManager::SD_down), 0);
}

TEST_F(SubPoolSwapOutTest, ScheduleNextSwapDisk_AllSwapLevelHasDisk_YWB_SUCCESS)
{
    /*
     * NOTE: test assertion should be updated whenever schedule ratio
     * of high/medium/low level in SubPoolSwapOut changes
     * */
    bool ret = false;
    std::vector<ywb_uint64_t> diskidvec;

    ywb_uint64_t diskidarray2[22] = {0, 1, 2, 3, 4, 5, 0, 1, 2, 0, 0,
            0, 1, 2, 3, 4, 5, 0, 1, 2, 0, 0};
    std::vector<ywb_uint64_t> diskidvec2(diskidarray2, diskidarray2 + 22);
    ScheduleNextSwapDiskTimes(SwapOutManager::SD_up, 22, diskidvec);
    ret = CheckScheduleNextSwapDiskTimes(SwapOutManager::SD_up, 22, diskidvec, diskidvec2);
    ASSERT_EQ(ret, true);

    ywb_uint64_t diskidarray3[22] = {6, 7, 8, 9, 10, 11, 6, 7, 8, 6, 6,
            6, 7, 8, 9, 10, 11, 6, 7, 8, 6, 6};
    std::vector<ywb_uint64_t> diskidvec3(diskidarray3, diskidarray3 + 22);
    diskidvec.clear();
    ScheduleNextSwapDiskTimes(SwapOutManager::SD_down, 22, diskidvec);
    ret = CheckScheduleNextSwapDiskTimes(SwapOutManager::SD_down, 22, diskidvec, diskidvec3);
    ASSERT_EQ(ret, true);
}

TEST_F(SubPoolSwapOutTest, ScheduleNextSwapDisk_SwapLevelHighHasNoDisk_YWB_SUCCESS)
{
    /*
     * NOTE: test assertion should be updated whenever schedule ratio
     * of high/medium/low level in SubPoolSwapOut changes
     * */
    bool ret = false;
    DiskKey diskkey(0, 1);
    std::vector<ywb_uint64_t> diskidvec;

    /*remove swap disk of high swap level from up swap direction*/
    diskkey.SetDiskId(0);
    mSPS->RemoveSwapDisk(diskkey);
    /*remove swap disk of high swap level from down swap direction*/
    diskkey.SetDiskId(6);
    mSPS->RemoveSwapDisk(diskkey);

    ywb_uint64_t diskidarray2[14] = {1, 2, 3, 4, 5, 1, 2,
            1, 2, 3, 4, 5, 1, 2};
    std::vector<ywb_uint64_t> diskidvec2(diskidarray2, diskidarray2 + 14);
    ScheduleNextSwapDiskTimes(SwapOutManager::SD_up, 14, diskidvec);
    ret = CheckScheduleNextSwapDiskTimes(SwapOutManager::SD_up, 14, diskidvec, diskidvec2);
    ASSERT_EQ(ret, true);

    ywb_uint64_t diskidarray3[14] = {7, 8, 9, 10, 11, 7, 8,
            7, 8, 9, 10, 11, 7, 8};
    std::vector<ywb_uint64_t> diskidvec3(diskidarray3, diskidarray3 + 14);
    diskidvec.clear();
    ScheduleNextSwapDiskTimes(SwapOutManager::SD_down, 14, diskidvec);
    ret = CheckScheduleNextSwapDiskTimes(SwapOutManager::SD_down, 14, diskidvec, diskidvec3);
    ASSERT_EQ(ret, true);
}

TEST_F(SubPoolSwapOutTest, ScheduleNextSwapDisk_SwapLevelMediumHasNoDisk_YWB_SUCCESS)
{
    /*
     * NOTE: test assertion should be updated whenever schedule ratio
     * of high/medium/low level in SubPoolSwapOut changes
     * */
    bool ret = false;
    DiskKey diskkey(0, 1);
    std::vector<ywb_uint64_t> diskidvec;

    /*remove swap disk of medium swap level from up swap direction*/
    diskkey.SetDiskId(1);
    mSPS->RemoveSwapDisk(diskkey);
    diskkey.SetDiskId(2);
    mSPS->RemoveSwapDisk(diskkey);
    /*remove swap disk of medium swap level from down swap direction*/
    diskkey.SetDiskId(7);
    mSPS->RemoveSwapDisk(diskkey);
    diskkey.SetDiskId(8);
    mSPS->RemoveSwapDisk(diskkey);

    ywb_uint64_t diskidarray2[14] = {0, 3, 4, 5, 0, 0, 0,
            0, 3, 4, 5, 0, 0, 0};
    std::vector<ywb_uint64_t> diskidvec2(diskidarray2, diskidarray2 + 14);
    ScheduleNextSwapDiskTimes(SwapOutManager::SD_up, 14, diskidvec);
    ret = CheckScheduleNextSwapDiskTimes(SwapOutManager::SD_up, 14, diskidvec, diskidvec2);
    ASSERT_EQ(ret, true);

    ywb_uint64_t diskidarray3[14] = {6, 9, 10, 11, 6, 6, 6,
            6, 9, 10, 11, 6, 6, 6};
    std::vector<ywb_uint64_t> diskidvec3(diskidarray3, diskidarray3 + 14);
    diskidvec.clear();
    ScheduleNextSwapDiskTimes(SwapOutManager::SD_down, 14, diskidvec);
    ret = CheckScheduleNextSwapDiskTimes(SwapOutManager::SD_down, 14, diskidvec, diskidvec3);
    ASSERT_EQ(ret, true);
}

TEST_F(SubPoolSwapOutTest, ScheduleNextSwapDisk_SwapLevelLowHasNoDisk_YWB_SUCCESS)
{
    /*
     * NOTE: test assertion should be updated whenever schedule ratio
     * of high/medium/low level in SubPoolSwapOut changes
     * */
    bool ret = false;
    DiskKey diskkey(0, 1);
    std::vector<ywb_uint64_t> diskidvec;

    /*remove swap disk of low swap level from up swap direction*/
    diskkey.SetDiskId(3);
    mSPS->RemoveSwapDisk(diskkey);
    diskkey.SetDiskId(4);
    mSPS->RemoveSwapDisk(diskkey);
    diskkey.SetDiskId(5);
    mSPS->RemoveSwapDisk(diskkey);
    /*remove swap disk of low swap level from down swap direction*/
    diskkey.SetDiskId(9);
    mSPS->RemoveSwapDisk(diskkey);
    diskkey.SetDiskId(10);
    mSPS->RemoveSwapDisk(diskkey);
    diskkey.SetDiskId(11);
    mSPS->RemoveSwapDisk(diskkey);

    ywb_uint64_t diskidarray2[16] = {0, 1, 2, 0, 1, 2, 0, 0,
            0, 1, 2, 0, 1, 2, 0, 0};
    std::vector<ywb_uint64_t> diskidvec2(diskidarray2, diskidarray2 + 16);
    ScheduleNextSwapDiskTimes(SwapOutManager::SD_up, 16, diskidvec);
    ret = CheckScheduleNextSwapDiskTimes(SwapOutManager::SD_up, 16, diskidvec, diskidvec2);
    ASSERT_EQ(ret, true);

    ywb_uint64_t diskidarray3[16] = {6, 7, 8, 6, 7, 8, 6, 6,
            6, 7, 8, 6, 7, 8, 6, 6};
    std::vector<ywb_uint64_t> diskidvec3(diskidarray3, diskidarray3 + 16);
    diskidvec.clear();
    ScheduleNextSwapDiskTimes(SwapOutManager::SD_down, 16, diskidvec);
    ret = CheckScheduleNextSwapDiskTimes(SwapOutManager::SD_down, 16, diskidvec, diskidvec3);
    ASSERT_EQ(ret, true);
}
