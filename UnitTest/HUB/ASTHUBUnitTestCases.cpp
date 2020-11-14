#include "UnitTest/AST/HUB/ASTHUBUnitTests.hpp"

TEST_F(TimeScheduleConfigTest, VerifyCopyConstructor_SUCCESS)
{
    TimeRange range;
    vector<TimeRange> timeranges;
    TimeScheduleConfig config;

    range.SetStart(10000);
    range.SetEnd(20000);
    timeranges.push_back(range);
    range.SetStart(30000);
    range.SetEnd(40000);
    timeranges.push_back(range);
    config.AddTimeRanges(timeranges);
    config.SetWeekDays(239);
    SetTimeScheduleConfig(config);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 239), true);
}

TEST_F(TimeScheduleConfigTest, CheckDefaultTimeScheduleConfig_SUCCESS)
{
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(0);
    range.SetEnd(86401);
    timeranges.push_back(range);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(TimeScheduleConfigTest, SetNewTimeScheduleConfigOnDefaultOne_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(100);
    range.SetEnd(75000);
    timeranges.push_back(range);
    ret = AddTimeRanges(timeranges);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(TimeScheduleConfigTest, SetNewTimeScheduleConfigAdjustCase1_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(10000);
    range.SetEnd(15000);
    timeranges.push_back(range);
    range.SetStart(10000);
    range.SetEnd(15000);
    timeranges.push_back(range);
    ret = AddTimeRanges(timeranges);
    ASSERT_EQ(ret, YWB_SUCCESS);

    timeranges.clear();
    range.SetStart(10000);
    range.SetEnd(15000);
    timeranges.push_back(range);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(TimeScheduleConfigTest, SetNewTimeScheduleConfigAdjustCase2_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(20000);
    range.SetEnd(25000);
    timeranges.push_back(range);
    range.SetStart(10000);
    range.SetEnd(15000);
    timeranges.push_back(range);
    ret = AddTimeRanges(timeranges);

    timeranges.clear();
    range.SetStart(10000);
    range.SetEnd(15000);
    timeranges.push_back(range);
    range.SetStart(20000);
    range.SetEnd(25000);
    timeranges.push_back(range);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(TimeScheduleConfigTest, SetNewTimeScheduleConfigAdjustCase3_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(20000);
    range.SetEnd(25000);
    timeranges.push_back(range);
    range.SetStart(10000);
    range.SetEnd(21000);
    timeranges.push_back(range);
    ret = AddTimeRanges(timeranges);
    ASSERT_EQ(ret, YWB_SUCCESS);

    timeranges.clear();
    range.SetStart(10000);
    range.SetEnd(25000);
    timeranges.push_back(range);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(TimeScheduleConfigTest, SetNewTimeScheduleConfigAdjustCase4_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(20000);
    range.SetEnd(25000);
    timeranges.push_back(range);
    range.SetStart(10000);
    range.SetEnd(30000);
    timeranges.push_back(range);
    ret = AddTimeRanges(timeranges);
    ASSERT_EQ(ret, YWB_SUCCESS);

    timeranges.clear();
    range.SetStart(10000);
    range.SetEnd(30000);
    timeranges.push_back(range);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(TimeScheduleConfigTest, SetNewTimeScheduleConfigAdjustCase5_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(10000);
    range.SetEnd(25000);
    timeranges.push_back(range);
    range.SetStart(15000);
    range.SetEnd(20000);
    timeranges.push_back(range);
    ret = AddTimeRanges(timeranges);
    ASSERT_EQ(ret, YWB_SUCCESS);

    timeranges.clear();
    range.SetStart(10000);
    range.SetEnd(25000);
    timeranges.push_back(range);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(TimeScheduleConfigTest, SetNewTimeScheduleConfigAdjustCase6_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(10000);
    range.SetEnd(25000);
    timeranges.push_back(range);
    range.SetStart(15000);
    range.SetEnd(30000);
    timeranges.push_back(range);
    ret = AddTimeRanges(timeranges);
    ASSERT_EQ(ret, YWB_SUCCESS);

    timeranges.clear();
    range.SetStart(10000);
    range.SetEnd(30000);
    timeranges.push_back(range);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(TimeScheduleConfigTest, SetNewTimeScheduleConfigAdjustCase7_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(10000);
    range.SetEnd(25000);
    timeranges.push_back(range);
    range.SetStart(30000);
    range.SetEnd(40000);
    timeranges.push_back(range);
    ret = AddTimeRanges(timeranges);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(TimeScheduleConfigTest, SetNewTimeScheduleConfigMerge_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TimeRange range;
    vector<TimeRange> timeranges;

    range.SetStart(60000);
    range.SetEnd(70000);
    timeranges.push_back(range);
    range.SetStart(30000);
    range.SetEnd(40000);
    timeranges.push_back(range);
    range.SetStart(10000);
    range.SetEnd(20000);
    timeranges.push_back(range);
    range.SetStart(15000);
    range.SetEnd(35000);
    timeranges.push_back(range);
    ret = AddTimeRanges(timeranges);
    ASSERT_EQ(ret, YWB_SUCCESS);

    timeranges.clear();
    range.SetStart(10000);
    range.SetEnd(40000);
    timeranges.push_back(range);
    range.SetStart(60000);
    range.SetEnd(70000);
    timeranges.push_back(range);
    ASSERT_EQ(CheckTimeScheduleConfig(timeranges, 255), true);
}

TEST_F(OBJViewTest, Construct_SUCCESS)
{
    ywb_uint32_t remainingrt = 0;
    OBJView* objview = NULL;
    AccessTrack* track = NULL;

    objview = GetOBJView();
    objview->GetAccessTrack(&track);
    remainingrt = objview->GetRemainingRT();

    ASSERT_EQ(track->GetLastOffset(), 0);
    ASSERT_EQ(track->GetLastLen(), 0);
    ASSERT_EQ(remainingrt, 0);
}

TEST_F(OBJViewTest, Update_SUCCESS)
{
    ywb_uint32_t remainingrt = 0;
    OBJView* objview = NULL;
    AccessTrack* track = NULL;
    ChunkIOStat* iostat = NULL;

    objview = GetOBJView();
    objview->GetAccessTrack(&track);
    objview->GetChunkIOStat(&iostat);
    objview->Update(AccessModel::AM_rnd | AccessModel::AM_read, 0, 10, 1, 10, 4503);
    remainingrt = objview->GetRemainingRT();
    ASSERT_EQ(track->GetLastOffset(), 0);
    ASSERT_EQ(track->GetLastLen(), 10);
    ASSERT_EQ(iostat->GetRndReadIOs(), 1);
    ASSERT_EQ(iostat->GetRndReadBW(), 10);
    ASSERT_EQ(iostat->GetRndTotalRT(), 4);
    ASSERT_EQ(remainingrt, 503);

    objview->Update(AccessModel::AM_rnd | AccessModel::AM_write, 20, 10, 1, 10, 5503);
    remainingrt = objview->GetRemainingRT();
    ASSERT_EQ(track->GetLastOffset(), 20);
    ASSERT_EQ(track->GetLastLen(), 10);
    ASSERT_EQ(iostat->GetRndReadIOs(), 1);
    ASSERT_EQ(iostat->GetRndReadBW(), 10);
    ASSERT_EQ(iostat->GetRndTotalRT(), 10);
    ASSERT_EQ(remainingrt, 6);

    objview->Update(AccessModel::AM_seq | AccessModel::AM_read, 30, 10, 1, 10, 5503);
    remainingrt = objview->GetRemainingRT();
    ASSERT_EQ(track->GetLastOffset(), 30);
    ASSERT_EQ(track->GetLastLen(), 10);
    ASSERT_EQ(iostat->GetSeqReadIOs(), 1);
    ASSERT_EQ(iostat->GetSeqReadBW(), 10);
    ASSERT_EQ(iostat->GetSeqTotalRT(), 5);
    ASSERT_EQ(remainingrt, 509);

    objview->Update(AccessModel::AM_seq | AccessModel::AM_write, 40, 10, 1, 10, 1503);
    remainingrt = objview->GetRemainingRT();
    ASSERT_EQ(track->GetLastOffset(), 40);
    ASSERT_EQ(track->GetLastLen(), 10);
    ASSERT_EQ(iostat->GetSeqReadIOs(), 1);
    ASSERT_EQ(iostat->GetSeqReadBW(), 10);
    ASSERT_EQ(iostat->GetSeqTotalRT(), 7);
    ASSERT_EQ(remainingrt, 12);
}

TEST_F(OBJViewTest, Reset_SUCCESS)
{
    ywb_uint32_t remainingrt = 0;
    OBJView* objview = NULL;
    AccessTrack* track = NULL;
    ChunkIOStat* iostat = NULL;

    objview = GetOBJView();
    objview->GetAccessTrack(&track);
    objview->GetChunkIOStat(&iostat);
    objview->Update(AccessModel::AM_rnd | AccessModel::AM_read, 0, 10, 1, 10, 4503);
    objview->Update(AccessModel::AM_rnd | AccessModel::AM_write, 20, 10, 1, 10, 5503);
    objview->Update(AccessModel::AM_seq | AccessModel::AM_read, 30, 10, 1, 10, 5503);
    objview->Update(AccessModel::AM_seq | AccessModel::AM_write, 40, 10, 1, 10, 1503);
    objview->Reset();

    ASSERT_EQ(track->GetLastOffset(), 0);
    ASSERT_EQ(track->GetLastLen(), 0);
    ASSERT_EQ(iostat->GetSeqReadIOs(), 0);
    ASSERT_EQ(iostat->GetSeqReadBW(), 0);
    ASSERT_EQ(iostat->GetSeqTotalRT(), 0);
    ASSERT_EQ(track->GetLastLen(), 0);
    ASSERT_EQ(iostat->GetRndReadIOs(), 0);
    ASSERT_EQ(iostat->GetRndReadBW(), 0);
    ASSERT_EQ(iostat->GetRndTotalRT(), 0);
    ASSERT_EQ(remainingrt, 0);
}

TEST_F(DiskViewTest, AddOBJView_SUCCESS)
{
    ASSERT_EQ(GetOBJCnt(), 6);
}

TEST_F(DiskViewTest, AddOBJView_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    OBJView* objview = NULL;

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 1);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + 1);
    ret = AddOBJView(objkey, &objview);
    ASSERT_EQ(ret, YFS_EEXIST);
    delete objview;
    objview = NULL;
}

TEST_F(DiskViewTest, GetOBJView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objloop = 0;
    OBJKey objkey;
    OBJView* objview = NULL;

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    for(objloop = 0; objloop < GetOBJCnt(); objloop++)
    {
        objkey.SetInodeId(Constant::DEFAULT_INODE + objloop);
        objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + objloop);
        ret = GetOBJView(objkey, &objview);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }
}

TEST_F(DiskViewTest, GetOBJView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    OBJView* objview = NULL;

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 10000);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + 10000);
    ret = GetOBJView(objkey, &objview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(DiskViewTest, DeleteOBJView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    OBJView* objview = NULL;

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 1);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + 1);
    ret = GetOBJView(objkey, &objview);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = DeleteOBJView(objkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetOBJView(objkey, &objview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(DiskViewTest, GetFirstNextOBJView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objloop = 0;
    OBJKey objkey;
    OBJView* objview = NULL;
    DiskView* diskview = NULL;

    diskview = GetDiskView();
    YWB_ASSERT(diskview != NULL);
    ret = diskview->GetFirstOBJView(&objview, &objkey);
    while(YWB_SUCCESS == ret)
    {
        ASSERT_EQ(objkey.GetInodeId(), Constant::DEFAULT_INODE + objloop);
        objloop++;
        ret = diskview ->GetNextOBJView(&objview, &objkey);
    }

    ASSERT_EQ(objloop, GetOBJCnt());
}

TEST_F(SubPoolViewTest, AddDiskView_SUCCESS)
{
    ASSERT_EQ(GetDiskCnt(), 6);
}

TEST_F(SubPoolViewTest, AddDiskView_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    ret = AddDiskView(diskkey);
    ASSERT_EQ(ret, YFS_EEXIST);
}

TEST_F(SubPoolViewTest, GetDiskView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    DiskKey diskkey;
    DiskBaseProp *diskprop = NULL;
    DiskView* diskview = NULL;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    for(diskloop = 0; diskloop < 6; diskloop++)
    {
        diskkey.SetDiskId(Constant::DISK_ID + diskloop);
        ret = GetDiskView(diskkey, &diskview);
        ASSERT_EQ(ret, YWB_SUCCESS);
        diskview->GetDiskBaseProp(&diskprop);
        ASSERT_EQ(diskprop->GetDiskClass(), DiskBaseProp::DCT_ent);
    }
}

TEST_F(SubPoolViewTest, GetDiskView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    DiskView* diskview = NULL;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(1000);
    ret = GetDiskView(diskkey, &diskview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolViewTest, RemoveDiskView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    DiskView* diskview = NULL;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    ret = GetDiskView(diskkey, &diskview);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = RemoveDiskView(diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetDiskView(diskkey, &diskview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolViewTest, AddOBJView_SUCCESS)
{
    ywb_uint32_t diskloop = 0;
    DiskKey diskkey;
    DiskView* diskview = NULL;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    for(diskloop = 0; diskloop < GetDiskCnt(); diskloop++)
    {
        if((diskloop % 2) == 1)
        {
            diskkey.SetDiskId(Constant::DISK_ID + diskloop);
            GetDiskView(diskkey, &diskview);
            ASSERT_EQ(diskview->GetOBJCnt(), 6);
        }
    }
}

TEST_F(SubPoolViewTest, AddOBJView_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    OBJKey objkey;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    objkey.SetStorageId(Constant::DISK_ID + 1);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    objkey.SetInodeId(1);
    objkey.SetChunkId(1);

    ret = AddOBJView(diskkey, objkey);
    ASSERT_EQ(ret, YFS_EEXIST);
}

TEST_F(SubPoolViewTest, GetOBJView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint64_t inodeid = 0;
    DiskKey diskkey;
    OBJKey objkey;
    OBJView* objview = NULL;

    inodeid = Constant::DEFAULT_INODE;
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    for(diskloop = 0; diskloop < 6; diskloop++)
    {
        if((diskloop % 2) == 1)
        {
            diskkey.SetDiskId(Constant::DISK_ID + diskloop);
            objkey.SetStorageId(Constant::DISK_ID + diskloop);
            for(objloop = 0; objloop < 6; objloop++, inodeid++)
            {
                objkey.SetInodeId(inodeid);
                objkey.SetChunkId(inodeid);

                ret = GetOBJView(diskkey, objkey, &objview);
                ASSERT_EQ(ret, YWB_SUCCESS);
            }
        }
    }
}

TEST_F(SubPoolViewTest, GetOBJView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    OBJKey objkey;
    OBJView* objview = NULL;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    objkey.SetStorageId(Constant::DISK_ID + 100);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 10000);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    ret = GetOBJView(diskkey, objkey, &objview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolViewTest, DeleteOBJView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    OBJKey objkey;
    OBJView* objview = NULL;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    objkey.SetStorageId(Constant::DISK_ID + 1);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 1);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + 1);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    ret = DeleteOBJView(diskkey, objkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetOBJView(diskkey, objkey, &objview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolViewTest, GetFirstNextDiskView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    DiskKey diskkey;
    SubPoolView* subpoolview = NULL;
    DiskView* diskview = NULL;

    subpoolview = GetSubPoolView();
    ret = subpoolview->GetFirstDiskView(&diskview, &diskkey);
    while(YWB_SUCCESS == ret)
    {
        ASSERT_EQ(diskkey.GetDiskId(), Constant::DISK_ID + diskloop);
        diskloop++;
        ret = subpoolview->GetNextDiskView(&diskview, &diskkey);
    }

    ASSERT_EQ(diskloop, GetDiskCnt());
}

TEST_F(SubPoolViewTest, GetFirstDiskView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolView* subpoolview = NULL;
    DiskView* diskview = NULL;

    subpoolview = GetSubPoolView2();
    ret = subpoolview->GetFirstDiskView(&diskview, &diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolViewTest, GetFirstNextOBJView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objloop = 0;
    OBJKey objkey;
    SubPoolView* subpoolview = NULL;
    OBJView* objview = NULL;

    subpoolview = GetSubPoolView();
    ret = subpoolview->GetFirstOBJView(&objview, &objkey);
    while(YWB_SUCCESS == ret)
    {
        ASSERT_EQ(objkey.GetInodeId(), Constant::DEFAULT_INODE + objloop);
        objloop++;
        ret = subpoolview->GetNextOBJView(&objview, &objkey);
    }

    ASSERT_EQ(objloop, GetOBJCnt());
}

TEST_F(SubPoolViewTest, GetFirstOBJView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    SubPoolView* subpoolview = NULL;
    OBJView* objview = NULL;

    subpoolview = GetSubPoolView2();
    ret = subpoolview->GetFirstOBJView(&objview, &objkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ConfigViewTest, GetSubPoolView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolcnt = 0;
    ywb_uint32_t subpoolid = Constant::SUBPOOL_ID;
    SubPoolKey subpoolkey;
    SubPoolView* subpoolview = NULL;

    subpoolkey.SetOss(GetOSSID());
    for(subpoolloop = 0; subpoolloop < 6; subpoolloop++, subpoolid++)
    {
        subpoolcnt++;
        subpoolkey.SetPoolId(subpoolid);
        subpoolkey.SetSubPoolId(subpoolid);
        ret = GetSubPoolView(subpoolkey, &subpoolview);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }

    ASSERT_EQ(subpoolcnt, GetSubPoolCnt());
}

TEST_F(ConfigViewTest, GetSubPoolView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    SubPoolView* subpoolview = NULL;

    subpoolkey.SetOss(GetOSSID());
    subpoolkey.SetPoolId(Constant::SUBPOOL_ID + 100);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
    ret = GetSubPoolView(subpoolkey, &subpoolview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ConfigViewTest, AddDiskView_SUCCESS)
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    SubPoolView* subpoolview = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    subpoolkey.SetOss(GetOSSID());

    ASSERT_EQ(GetSubPoolCnt(), 6);
    for(subpoolloop = 0; subpoolloop < 6; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetPoolId(subpoolid);
        subpoolkey.SetSubPoolId(subpoolid);
        GetSubPoolView(subpoolkey, &subpoolview);
        ASSERT_EQ(subpoolview->GetDiskCnt(), 6);
    }
}

TEST_F(ConfigViewTest, AddDiskView_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    DiskBaseProp diskprop;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID);
    SetDefaultDiskBaseProp(diskprop);
    ret = AddDiskView(diskkey, diskprop);
    ASSERT_EQ(ret, YFS_EEXIST);
}

TEST_F(ConfigViewTest, GetDiskView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;
    DiskView* diskview = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    for(subpoolloop = 0; subpoolloop < 6; subpoolloop++, subpoolid++)
    {
        for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
        {
            diskkey.SetSubPoolId(subpoolid);
            diskkey.SetDiskId(diskid);
            ret = GetDiskView(diskkey, &diskview);
            ASSERT_EQ(ret, YWB_SUCCESS);
        }
    }
}

TEST_F(ConfigViewTest, GetDiskView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    DiskView* diskview = NULL;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);
    ret = GetDiskView(diskkey, &diskview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ConfigViewTest, RemoveDiskView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    DiskView* diskview = NULL;

    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    ret = RemoveDiskView(diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetDiskView(diskkey, &diskview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ConfigViewTest, AddOBJView_SUCCESS)
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;
    DiskView* diskview = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    for(subpoolloop = 0; subpoolloop < 6; subpoolloop++, subpoolid++)
    {
        diskkey.SetSubPoolId(subpoolid);
        for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
        {
            diskkey.SetDiskId(diskid);
            if((diskloop % 2) == 1)
            {
                GetDiskView(diskkey, &diskview);
                ASSERT_EQ(diskview->GetOBJCnt(), 6);
            }
        }
    }
}

TEST_F(ConfigViewTest, AddOBJView_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;

    objkey.SetStorageId(Constant::DISK_ID + 1);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    objkey.SetChunkId(Constant::DEFAULT_INODE);

    ret = AddOBJView(objkey);
    ASSERT_EQ(ret, YFS_EEXIST);
}

TEST_F(ConfigViewTest, GetOBJView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;
    OBJView* objview = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    for(subpoolloop = 0; subpoolloop < 6; subpoolloop++, subpoolid++)
    {
        for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
        {
            if((diskloop % 2) == 1)
            {
                objkey.SetStorageId(diskid);
                objkey.SetChunkIndex(1);
                objkey.SetChunkVersion(1);
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    objkey.SetInodeId(inodeid);
                    objkey.SetChunkId(inodeid);
                    ret = GetOBJView(objkey, &objview);
                    ASSERT_EQ(ret, YWB_SUCCESS);
                }
            }
        }
    }
}

TEST_F(ConfigViewTest, GetOBJView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    OBJView* objview = NULL;

    objkey.SetStorageId(Constant::DISK_ID + 1);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 10000);
    objkey.SetChunkId(Constant::DEFAULT_INODE + 10000);

    ret = GetOBJView(objkey, &objview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ConfigViewTest, DeleteOBJView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    OBJView* objview = NULL;

    objkey.SetStorageId(Constant::DISK_ID + 1);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    objkey.SetChunkId(Constant::DEFAULT_INODE);

    ret = DeleteOBJView(objkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetOBJView(objkey, &objview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ConfigViewTest, GetFirstNextSubPoolView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolcnt = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    ConfigView* configview = NULL;
    SubPoolView* subpoolview = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    configview = GetConfigView();
    ret = configview->GetFirstSubPoolView(&subpoolview, &subpoolkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(subpoolkey.GetSubPoolId(), subpoolid++);
        subpoolcnt++;
        ret = configview->GetNextSubPoolView(&subpoolview, &subpoolkey);
    }

    ASSERT_EQ(subpoolcnt, GetSubPoolCnt());
}

TEST_F(ConfigViewTest, GetFirstSubPoolView_EmptyConfigView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    ConfigView* configview = NULL;
    SubPoolView* subpoolview = NULL;

    configview = GetEmptyConfigView();
    ret = configview->GetFirstSubPoolView(&subpoolview, &subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ConfigViewTest, GetFirstNextDiskView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskcnt = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;
    ConfigView* configview = NULL;
    DiskView* diskview = NULL;

    diskid = Constant::DISK_ID;
    configview = GetConfigView();
    ret = configview->GetFirstDiskView(&diskview, &diskkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(diskkey.GetDiskId(), diskid++);
        diskcnt++;
        ret = configview->GetNextDiskView(&diskview, &diskkey);
    }

    ASSERT_EQ(diskcnt, GetDiskCnt());
}

TEST_F(ConfigViewTest, GetFirstDiskView_EmptyConfigView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    ConfigView* configview = NULL;
    DiskView* diskview = NULL;

    configview = GetEmptyConfigView();
    ret = configview->GetFirstDiskView(&diskview, &diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ConfigViewTest, GetFirstNextOBJView_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objcnt = 0;
    OBJKey objkey;
    ConfigView* configview = NULL;
    OBJView* objview = NULL;

    configview = GetConfigView();
    ret = configview->GetFirstOBJView(&objview, &objkey);
    while((YWB_SUCCESS == ret))
    {
        objcnt++;
        ret = configview->GetNextOBJView(&objview, &objkey);
    }

    ASSERT_EQ(objcnt, GetTotalOBJCntOnSubPoolWithASTEnabled());
}

TEST_F(ConfigViewTest, GetFirstOBJView_EmptyConfigView_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    ConfigView* configview = NULL;
    OBJView* objview = NULL;

    configview = GetEmptyConfigView();
    ret = configview->GetFirstOBJView(&objview, &objkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(HUBTest, StartStopAST_SUCCESS)
{
//    ywb_uint32_t startwaittimes = 0;
//    ywb_uint32_t stopwaittimes = 0;
//
//    StartAST();
//    /*wait until the AST is really enabled*/
//    while((startwaittimes < 50) && (GetASTStatus() != HUB::AS_enabled))
//    {
//        startwaittimes++;
//        sleep(1);
//    }
//
//    if(startwaittimes >= 50)
//    {
//        ast_log_fatal("fail to wait until AST started");
//    }
//    else
//    {
//        ast_log_debug("wait until AST started for " << startwaittimes << " times");
//    }
//
//    StopAST(true);
//    /*wait until the AST is really disabled*/
//    while((stopwaittimes < 50) && (GetASTStatus() != HUB::AS_disabled))
//    {
//        stopwaittimes++;
//        sleep(1);
//    }
//
//    if(stopwaittimes >= 50)
//    {
//        ast_log_fatal("fail to wait until AST stopped");
//    }
//    else
//    {
//        ast_log_debug("wait until AST stopped for " << stopwaittimes << " times");
//    }
    StartAST();
    StopAST(true);
    /*clear the state of AST*/
    ResetAST();
}

TEST_F(HUBTest, AddDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    DiskBaseProp diskprop;
    DiskView* diskview = NULL;

    AddOneDisk(Constant::DISK_ID, false);
    WaitForHUBSignal();
    ret = GetDiskView(Constant::DISK_ID, &diskview);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskview != NULL, true);
}

TEST_F(HUBTest, RemoveDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskView* diskview = NULL;

    AddOneDisk(Constant::DISK_ID, true);
    AddOneDisk(Constant::DISK_ID + 1, true);
    RemoveOneDisk(Constant::DISK_ID, false);
    WaitForHUBSignal();
    ret = GetDiskView(Constant::DISK_ID, &diskview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(HUBTest, AddOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJView* objview = NULL;

    AddOneDisk(Constant::DISK_ID, true);
    AddOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE, false);
    WaitForHUBSignal();
    ret = GetOBJView(Constant::DISK_ID, Constant::DEFAULT_INODE, &objview);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(objview != NULL, true);
}

TEST_F(HUBTest, DeleteOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJView* objview = NULL;

    AddOneDisk(Constant::DISK_ID, true);
    AddOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE, true);
    DeleteOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE, false);
    WaitForHUBSignal();
    ret = GetOBJView(Constant::DISK_ID, Constant::DEFAULT_INODE, &objview);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(HUBTest, AccessOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJView* objview = NULL;

    AddOneDisk(Constant::DISK_ID, true);
    AddOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE, true);
    AccessOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE,
            2, 4, 1, 4, 305, true, false);
    WaitForHUBSignal();
    ret = GetOBJView(Constant::DISK_ID, Constant::DEFAULT_INODE, &objview);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(objview->GetRemainingRT(), 305);
}

TEST_F(HUBTest, CollectConfigView_SUCCESS)
{
    ywb_bool_t val = false;

    PrepareConfigView();
    Lock();
    CollectConfigView();
    Wait();
    Unlock();
    val = CheckConfigView();
    ASSERT_EQ(val, true);
}

TEST_F(HUBTest, CollectIO_SUCCESS)
{
    ywb_bool_t val = false;

    PrepareIO();
    Lock();
    CollectIO();
    Wait();
    Unlock();
    val = CheckIO();
    ASSERT_EQ(val, true);
}

TEST_F(HUBTest, Migrate_SUCCESS)
{
    ywb_bool_t val = false;

    PrepareMigrate();
    Lock();
    Migrate(Constant::DISK_ID, Constant::DEFAULT_INODE, Constant::DISK_ID + 1);
    Wait();
    Unlock();
    val = CheckMigrate();
    ASSERT_EQ(val, true);
}

TEST_F(HUBTest, MigrationCompleted_SUCCESS)
{
    ywb_bool_t val = false;

    CompleteMigrate(Constant::DISK_ID, Constant::DEFAULT_INODE);
    val = CheckCompleteMigrate();
    ASSERT_EQ(val, true);
}

/* vim:set ts=4 sw=4 expandtab */
