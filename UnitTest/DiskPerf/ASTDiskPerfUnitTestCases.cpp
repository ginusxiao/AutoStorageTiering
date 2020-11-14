#include <vector>
#include "UnitTest/AST/DiskPerf/ASTDiskPerfUnitTests.hpp"

TEST_F(BlackListTest, AddDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t disknum = 0;
    DiskKey diskkey;
    BlackList* blacklist = NULL;
    ywb_uint64_t diskids[3] = {1, 2, 3};

    blacklist = GetBlackList();
    ret = blacklist->GetFirstDisk(&diskkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(diskkey.GetDiskId(), diskids[disknum++]);
        ret = blacklist->GetNextDisk(&diskkey);
    }
    ASSERT_EQ(disknum, 3);
}

TEST_F(BlackListTest, RemoveDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t disknum = 0;
    DiskKey diskkey;
    BlackList* blacklist = NULL;
    ywb_uint64_t diskids[3] = {1, 3};

    blacklist = GetBlackList();
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    ret = blacklist->RemoveDisk(diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = blacklist->GetFirstDisk(&diskkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(diskkey.GetDiskId(), diskids[disknum++]);
        ret = blacklist->GetNextDisk(&diskkey);
    }
    ASSERT_EQ(disknum, 2);
}

TEST_F(BlackListTest, RemoveDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t disknum = 0;
    DiskKey diskkey;
    BlackList* blacklist = NULL;
    ywb_uint64_t diskids[3] = {1, 2, 3};

    blacklist = GetBlackList();
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 10000);
    ret = blacklist->RemoveDisk(diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = blacklist->GetFirstDisk(&diskkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(diskkey.GetDiskId(), diskids[disknum++]);
        ret = blacklist->GetNextDisk(&diskkey);
    }
    ASSERT_EQ(disknum, 3);
}

TEST_F(BlackListTest, Find_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    DiskKey diskkey;
    BlackList* blacklist = NULL;

    blacklist = GetBlackList();
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    for(diskloop = 0; diskloop < 3; diskloop++)
    {
        diskkey.SetDiskId(Constant::DISK_ID + diskloop);
        ret = blacklist->Find(diskkey);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }
}

TEST_F(BlackListTest, Find_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    BlackList* blacklist = NULL;

    blacklist = GetBlackList();
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 10000);
    ret = blacklist->Find(diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(BlackListTest, GetDifferenceSet_BlackListAAndBHasIntersection_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    BlackList* blacklista = NULL;
    BlackList* blacklistb = NULL;
    DiskKey diskkey;
    ywb_uint32_t disknum = 0;
    ywb_uint64_t diskids[3] = {1};

    blacklista = GetBlackList();
    blacklistb = GetBlackList2();
    ret = blacklista->GetDifferenceSet(blacklistb);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = blacklista->GetFirstDisk(&diskkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(diskkey.GetDiskId(), diskids[disknum++]);
        ret = blacklista->GetNextDisk(&diskkey);
    }
    ASSERT_EQ(disknum, 1);
}

TEST_F(BlackListTest, GetDifferenceSet_BlackListAAndBHasNoIntersection_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    BlackList* blacklista = NULL;
    BlackList* blacklistb = NULL;
    DiskKey diskkey;
    ywb_uint32_t disknum = 0;
    ywb_uint64_t diskids[3] = {1, 2, 3};

    blacklista = GetBlackList();
    blacklistb = GetBlackList3();
    ret = blacklista->GetDifferenceSet(blacklistb);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = blacklista->GetFirstDisk(&diskkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(diskkey.GetDiskId(), diskids[disknum++]);
        ret = blacklista->GetNextDisk(&diskkey);
    }
    ASSERT_EQ(disknum, 3);
}

TEST_F(BlackListTest, GetDifferenceSet_BlackListBEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    BlackList* blacklista = NULL;
    BlackList* blacklistb = NULL;
    DiskKey diskkey;
    ywb_uint32_t disknum = 0;
    ywb_uint64_t diskids[3] = {1, 2, 3};

    blacklista = GetBlackList();
    blacklistb = GetBlackList4();
    ret = blacklista->GetDifferenceSet(blacklistb);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = blacklista->GetFirstDisk(&diskkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(diskkey.GetDiskId(), diskids[disknum++]);
        ret = blacklista->GetNextDisk(&diskkey);
    }
    ASSERT_EQ(disknum, 3);
}

TEST_F(BlackListTest, Reset_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t disknum = 0;
    DiskKey diskkey;
    BlackList* blacklista = NULL;

    blacklista = GetBlackList();
    ret = blacklista->Reset();
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = blacklista->GetFirstDisk(&diskkey);
    while((YWB_SUCCESS == ret))
    {
        disknum++;
        ret = blacklista->GetNextDisk(&diskkey);
    }
    ASSERT_EQ(disknum, 0);
}

TEST_F(BlackListTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t disknum = 0;
    DiskKey diskkey;
    BlackList* blacklista = NULL;

    blacklista = GetBlackList();
    ret = blacklista->Destroy();
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = blacklista->GetFirstDisk(&diskkey);
    while((YWB_SUCCESS == ret))
    {
        disknum++;
        ret = blacklista->GetNextDisk(&diskkey);
    }
    ASSERT_EQ(disknum, 0);
}

TEST_F(DiskPerfTest, CalculateDiskPerf_DiskPerfProfileNonNull_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerf* diskperf = NULL;

    diskperf = GetDiskPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskperf != NULL);
    ret = diskperf->CalculateDiskPerf(IOStat::IOST_raw);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskperf->GetBWThreshold(), 737280000);
    ASSERT_EQ(diskperf->GetIOThreshold(), 180000);
    ASSERT_EQ(diskperf->GetCapTotal(), 128000);
    ASSERT_EQ(diskperf->GetBWConsumed(), 5440);
    ASSERT_EQ(diskperf->GetIOConsumed(), 1360);
    ASSERT_EQ(diskperf->GetCapConsumed(), 6);
}

TEST_F(DiskPerfTest, CalculateDiskPerf_DiskPerfProfileNull_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerf* diskperf = NULL;

    diskperf = GetDiskPerfInternal();
    ret = diskperf->CalculateDiskPerf(IOStat::IOST_raw);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskperf->GetBWThreshold(), (ywb_uint32_t)(
            (Constant::DEFAULT_ENT_BW) * (Constant::CYCLE_PERIOD) * 1024));
    ASSERT_EQ(diskperf->GetIOThreshold(), (ywb_uint32_t)(
            (Constant::DEFAULT_ENT_IOPS) * (Constant::CYCLE_PERIOD)));
    ASSERT_EQ(diskperf->GetCapTotal(), 128000);
    ASSERT_EQ(diskperf->GetBWConsumed(), 5440);
    ASSERT_EQ(diskperf->GetIOConsumed(), 1360);
    ASSERT_EQ(diskperf->GetCapConsumed(), 6);
}

TEST_F(DiskAvailPerfRankingUnitTest, CalculateDiskAvailPerf_SUCCESS)
{
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    diskavailperf = GetDiskAvailPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskavailperf != NULL);
    diskavailperf->CalculateDiskAvailPerf();
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178640);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 0);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274560);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 0);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127994);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 0);
}

TEST_F(DiskAvailPerfRankingUnitTest, CalculateDiskAvailPerf_ThresholdExceed_SUCCESS)
{
    DiskAvailPerfRankingUnit* diskavailperf = NULL;
    DiskPerf* diskperf = NULL;

    diskavailperf = GetDiskAvailPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskavailperf != NULL);
    diskavailperf->GetDiskPerf(&diskperf);
    diskperf->SetIOConsumed(180000);
    diskperf->SetBWConsumed(800000000);
    diskperf->SetCapConsumed(200000);
    diskavailperf->CalculateDiskAvailPerf();
    ASSERT_EQ(diskavailperf->GetIOAvail(), 0);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 0);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 0);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 0);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 0);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 0);
}

TEST_F(DiskAvailPerfRankingUnitTest, Reserve_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    diskavailperf = GetDiskAvailPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskavailperf != NULL);
    diskavailperf->CalculateDiskAvailPerf();
    ret = diskavailperf->Reserve(40, 160, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178600);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 40);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274400);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 160);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127992);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 2);
}

TEST_F(DiskAvailPerfRankingUnitTest, Reserve_NegativeCap_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    diskavailperf = GetDiskAvailPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskavailperf != NULL);
    diskavailperf->CalculateDiskAvailPerf();
    ret = diskavailperf->Reserve(40, 160, -2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178600);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 40);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274400);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 160);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127996);
    ASSERT_EQ(diskavailperf->GetCapReserved(), -2);
}

TEST_F(DiskAvailPerfRankingUnitTest, Reserve_BWExceed_YFS_ENOSPACE)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    diskavailperf = GetDiskAvailPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskavailperf != NULL);
    diskavailperf->CalculateDiskAvailPerf();
    ret = diskavailperf->Reserve(4000, 800000000, 200);
    ASSERT_EQ(ret, YFS_ENOSPACE);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178640);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 0);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274560);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 0);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127994);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 0);
}

TEST_F(DiskAvailPerfRankingUnitTest, UnReserve_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    diskavailperf = GetDiskAvailPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskavailperf != NULL);
    diskavailperf->CalculateDiskAvailPerf();
    diskavailperf->Reserve(40, 160, 2);
    ret = diskavailperf->UnReserve(40, 160, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178640);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 0);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274560);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 0);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127994);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 0);
}

TEST_F(DiskAvailPerfRankingUnitTest, UnReserve_NegativeCap_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    diskavailperf = GetDiskAvailPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskavailperf != NULL);
    diskavailperf->CalculateDiskAvailPerf();
    diskavailperf->Reserve(40, 160, -2);
    ret = diskavailperf->UnReserve(40, 160, -2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178640);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 0);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274560);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 0);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127994);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 0);
}

TEST_F(DiskAvailPerfRankingUnitTest, Consume_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;
    DiskPerf* diskperf = NULL;

    diskavailperf = GetDiskAvailPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskavailperf != NULL);
    diskavailperf->CalculateDiskAvailPerf();
    diskavailperf->Reserve(40, 160, 2);
    diskavailperf->Reserve(100, 400, 5);
    ret = diskavailperf->Consume(40, 160, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178500);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 100);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274000);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 400);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127987);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 5);
    diskavailperf->GetDiskPerf(&diskperf);
    ASSERT_EQ(diskperf->GetIOConsumed(), 1400);
    ASSERT_EQ(diskperf->GetBWConsumed(), 5600);
    ASSERT_EQ(diskperf->GetCapConsumed(), 8);
}

TEST_F(DiskAvailPerfRankingUnitTest, Consume_NegativeCap_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;
    DiskPerf* diskperf = NULL;

    diskavailperf = GetDiskAvailPerf(Constant::SUBPOOL_ID, Constant::DISK_ID);
    YWB_ASSERT(diskavailperf != NULL);
    diskavailperf->CalculateDiskAvailPerf();
    diskavailperf->Reserve(40, 160, -2);
    diskavailperf->Reserve(100, 400, 5);
    ret = diskavailperf->Consume(40, 160, -2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178500);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 100);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274000);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 400);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127991);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 5);
    diskavailperf->GetDiskPerf(&diskperf);
    ASSERT_EQ(diskperf->GetIOConsumed(), 1400);
    ASSERT_EQ(diskperf->GetBWConsumed(), 5600);
    ASSERT_EQ(diskperf->GetCapConsumed(), 4);
}

TEST_F(TierDiskAvailPerfRankingTest, SetupDiskAvailPerfRankingUnit_WithPerfType_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID, Tier::TIER_0);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);
    ret = tieravailperf->SetupDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &diskavailperf);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178640);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 0);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274560);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 0);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127994);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 0);
}

TEST_F(TierDiskAvailPerfRankingTest, SetupDiskAvailPerfRankingUnit_WithDiskKeyOnly_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* rawdiskavailperf = NULL;
    DiskAvailPerfRankingUnit* shortdiskavailperf = NULL;
    DiskAvailPerfRankingUnit* longdiskavailperf = NULL;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID, Tier::TIER_0);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);
    ret = tieravailperf->SetupDiskAvailPerfRankingUnit(diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &rawdiskavailperf);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_short_term_ema, &shortdiskavailperf);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_long_term_ema, &longdiskavailperf);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(rawdiskavailperf->GetIOAvail(), 178640);
    ASSERT_EQ(rawdiskavailperf->GetIOReserved(), 0);
    ASSERT_EQ(rawdiskavailperf->GetBWAvail(), 737274560);
    ASSERT_EQ(rawdiskavailperf->GetBWReserved(), 0);
    ASSERT_EQ(rawdiskavailperf->GetCapAvail(), 127994);
    ASSERT_EQ(rawdiskavailperf->GetCapReserved(), 0);
}

TEST_F(TierDiskAvailPerfRankingTest, SetupDiskAvailPerfRankingUnit_WithPerfTypeButDiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID, Tier::TIER_0);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1500);
    ret = tieravailperf->SetupDiskAvailPerfRankingUnit(diskkey, IOStat::IOST_long_term_ema);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierDiskAvailPerfRankingTest, SetupDiskAvailPerfRankingUnit_WithDiskKeyOnlyButDiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID, Tier::TIER_0);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1500);
    ret = tieravailperf->SetupDiskAvailPerfRankingUnit(diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierDiskAvailPerfRankingTest, GetDiskAvailPerfRankingUnit_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint32_t diskid = 0;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    subpoolid = Constant::SUBPOOL_ID + 1;
    diskid = Constant::DISK_ID + 1100;
    diskkey.SetSubPoolId(subpoolid);
    for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
    {
        diskkey.SetDiskId(diskid);
        ret = tieravailperf->GetDiskAvailPerfRankingUnit(
                diskkey, IOStat::IOST_raw, &diskavailperf);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ret = tieravailperf->GetDiskAvailPerfRankingUnit(
                diskkey, IOStat::IOST_long_term_ema, &diskavailperf);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }
}

TEST_F(TierDiskAvailPerfRankingTest, GetDiskAvailPerfRankingUnit_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1200);
    ret = tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &diskavailperf);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierDiskAvailPerfRankingTest, GetDiskAvailPerfRankingUnit_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1100);
    ret = tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_cnt, &diskavailperf);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(TierDiskAvailPerfRankingTest, RemoveDiskAvailPerfRankingUnit_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1100);
    ret = tieravailperf->RemoveDiskAvailPerfRankingUnit(diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &diskavailperf);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_short_term_ema, &diskavailperf);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_long_term_ema, &diskavailperf);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierDiskAvailPerfRankingTest, RemoveDiskAvailPerfRankingUnit_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1200);
    ret = tieravailperf->RemoveDiskAvailPerfRankingUnit(diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierDiskAvailPerfRankingTest, SortDisks_WithSpecifiedPerfType_SUCCESS)
{
    ywb_uint32_t disknum = 0;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    vector<DiskKey> diskvec;
    vector<DiskKey>::iterator diskiter;
    ywb_uint64_t diskids[6] = {1102, 1104, 1101, 1105, 1106, 1103};

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    tieravailperf->SortDisks(IOStat::IOST_raw);
    tieravailperf->GetSortedDisks(IOStat::IOST_raw, &diskvec);
    diskiter = diskvec.begin();
    for(; diskiter != diskvec.end(); diskiter++)
    {
        diskkey = *diskiter;
        ASSERT_EQ(diskkey.GetDiskId(), diskids[disknum++]);
    }
}

TEST_F(TierDiskAvailPerfRankingTest, SortDisks_SUCCESS)
{
    ywb_uint32_t disknum = 0;
    DiskKey diskkey;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    vector<DiskKey> diskvec;
    vector<DiskKey>::iterator diskiter;
    ywb_uint64_t diskids[6] = {1102, 1104, 1101, 1105, 1106, 1103};

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    tieravailperf->SortDisks();
    tieravailperf->GetSortedDisks(IOStat::IOST_raw, &diskvec);
    diskiter = diskvec.begin();
    for(; diskiter != diskvec.end(); diskiter++)
    {
        diskkey = *diskiter;
        ASSERT_EQ(diskkey.GetDiskId(), diskids[disknum++]);
    }
}

TEST_F(TierDiskAvailPerfRankingTest, Reserve_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID, Tier::TIER_0);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID);
    tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &diskavailperf);
    diskavailperf->CalculateDiskAvailPerf();
    ret = tieravailperf->Reserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178600);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 40);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274400);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 160);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127992);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 2);
}

TEST_F(TierDiskAvailPerfRankingTest, UnReserve_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID, Tier::TIER_0);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID);
    tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &diskavailperf);
    diskavailperf->CalculateDiskAvailPerf();
    tieravailperf->Reserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ret = tieravailperf->UnReserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178640);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 0);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274560);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 0);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127994);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 0);
}

TEST_F(TierDiskAvailPerfRankingTest, Consume_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;
    DiskPerf* diskperf = NULL;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID, Tier::TIER_0);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID);
    tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &diskavailperf);
    diskavailperf->CalculateDiskAvailPerf();
    tieravailperf->Reserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    tieravailperf->Reserve(diskkey, IOStat::IOST_raw, 100, 400, 5);
    ret = tieravailperf->Consume(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178500);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 100);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274000);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 400);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127987);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 5);
    diskavailperf->GetDiskPerf(&diskperf);
    ASSERT_EQ(diskperf->GetIOConsumed(), 1400);
    ASSERT_EQ(diskperf->GetBWConsumed(), 5600);
    ASSERT_EQ(diskperf->GetCapConsumed(), 8);
}

TEST_F(TierDiskAvailPerfRankingTest, GetFavoriteDisk_WhiteListNULLBlackListNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    ret = tieravailperf->GetFavoriteDisk(
            IOStat::IOST_raw, 40, 160, 2, NULL, NULL, &diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskkey.GetDiskId(), 1102);
}

TEST_F(TierDiskAvailPerfRankingTest, GetFavoriteDisk_WhiteListNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    BlackList blacklist;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 1);
    blacklist.AddDisk(diskkey);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 3);
    blacklist.AddDisk(diskkey);
    ret = tieravailperf->GetFavoriteDisk(
            IOStat::IOST_raw, 40, 160, 2, &blacklist, NULL, &diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskkey.GetDiskId(), 1101);
}

TEST_F(TierDiskAvailPerfRankingTest, GetFavoriteDisk_BlackListNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    WhiteList whitelist;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 4);
    whitelist.AddDisk(diskkey);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 5);
    whitelist.AddDisk(diskkey);
    ret = tieravailperf->GetFavoriteDisk(
            IOStat::IOST_raw, 40, 160, 2, NULL, &whitelist, &diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskkey.GetDiskId(), 1105);
}

TEST_F(TierDiskAvailPerfRankingTest, GetFavoriteDisk_WhiteListAndBlackListNonNULL_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    BlackList blacklist;
    WhiteList whitelist;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 4);
    blacklist.AddDisk(diskkey);
    whitelist.AddDisk(diskkey);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 5);
    whitelist.AddDisk(diskkey);
    ret = tieravailperf->GetFavoriteDisk(
            IOStat::IOST_raw, 40, 160, 2, &blacklist, &whitelist, &diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskkey.GetDiskId(), 1105);
}

TEST_F(TierDiskAvailPerfRankingTest, GetFavoriteDisk_WhiteListNonNullAndOneDiskNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    WhiteList whitelist;
    DiskKey diskkey;

    ast = gAst;
    ast->GetLogicalConfig(&config);
    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 4);
    config->RemoveDisk(diskkey);
    whitelist.AddDisk(diskkey);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 5);
    whitelist.AddDisk(diskkey);
    ret = tieravailperf->GetFavoriteDisk(
            IOStat::IOST_raw, 40, 160, 2, NULL, &whitelist, &diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskkey.GetDiskId(), 1106);
}

TEST_F(TierDiskAvailPerfRankingTest, GetFavoriteDisk_BWExceed_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    WhiteList whitelist;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 4);
    whitelist.AddDisk(diskkey);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 5);
    whitelist.AddDisk(diskkey);
    ret = tieravailperf->GetFavoriteDisk(
            IOStat::IOST_raw, 4000, 900000000, 400, NULL, &whitelist, &diskkey);
    ASSERT_EQ(ret, YFS_ENOSPACE);
}

TEST_F(TierDiskAvailPerfRankingTest, GetFavoriteDisk_TierHasNoDisks_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    WhiteList whitelist;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    for(diskloop = 0; diskloop < 6; diskloop++)
    {
        diskkey.SetDiskId(Constant::DISK_ID + 1100 + diskloop);
        tieravailperf->RemoveDiskAvailPerfRankingUnit(diskkey);
    }

    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 5);
    whitelist.AddDisk(diskkey);
    ret = tieravailperf->GetFavoriteDisk(
            IOStat::IOST_raw, 40, 160, 2, NULL, &whitelist, &diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierDiskAvailPerfRankingTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t perftype = 0;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskunit = NULL;
    DiskKey diskkey;

    tieravailperf = GetTierAvailPerf(Constant::SUBPOOL_ID + 1, Tier::TIER_1);
    tieravailperf->Destroy();
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    for(diskloop = 0; diskloop < 6; diskloop++)
    {
        diskkey.SetDiskId(Constant::DISK_ID + 1100 + diskloop);
        for(perftype = IOStat::IOST_raw; perftype < IOStat::IOST_cnt; perftype++)
        {
            ret = tieravailperf->GetDiskAvailPerfRankingUnit(
                    diskkey, perftype, &diskunit);
            ASSERT_EQ(ret, YFS_ENOENT);
        }
    }
}

TEST_F(SubPoolDiskAvailPerfRankingTest, SetupTierDiskAvailPerfRanking_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskunit = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID);
    diskid = Constant::DISK_ID;
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        ret = subpoolavailperf->GetTierDiskAvailPerfRanking(tier, &tieravailperf);
        ASSERT_EQ(ret, YWB_SUCCESS);
        diskkey.SetDiskId(diskid++);
        ret = tieravailperf->GetDiskAvailPerfRankingUnit(
                diskkey, IOStat::IOST_raw, &diskunit);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ASSERT_EQ(diskunit != NULL, true);
    }
}

TEST_F(SubPoolDiskAvailPerfRankingTest, GetTierDiskAvailPerfRanking_NoGivenTier_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID + 2);
    ret = subpoolavailperf->GetTierDiskAvailPerfRanking(Tier::TIER_1, &tieravailperf);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, GetTierDiskAvailPerfRanking_TierOutOfBoundary_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID + 1);
    ret = subpoolavailperf->GetTierDiskAvailPerfRanking(Tier::TIER_cnt, &tieravailperf);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, Reserve_DiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);
    ret = subpoolavailperf->Reserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, Reserve_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID);
    subpoolavailperf->GetTierDiskAvailPerfRanking(Tier::TIER_0, &tieravailperf);
    tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &diskavailperf);
    diskavailperf->CalculateDiskAvailPerf();
    ret = subpoolavailperf->Reserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178600);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 40);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274400);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 160);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127992);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 2);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, UnReserve_DiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);
    ret = subpoolavailperf->UnReserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, UnReserve_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID);
    subpoolavailperf->GetTierDiskAvailPerfRanking(Tier::TIER_0, &tieravailperf);
    tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &diskavailperf);
    diskavailperf->CalculateDiskAvailPerf();
    subpoolavailperf->Reserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ret = subpoolavailperf->UnReserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178640);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 0);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274560);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 0);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127994);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 0);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, Consume_DiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);
    ret = subpoolavailperf->Consume(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, Consume_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;
    DiskAvailPerfRankingUnit* diskavailperf = NULL;
    DiskPerf* diskperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID);
    ret = subpoolavailperf->GetTierDiskAvailPerfRanking(Tier::TIER_0, &tieravailperf);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID);
    subpoolavailperf->GetTierDiskAvailPerfRanking(Tier::TIER_0, &tieravailperf);
    tieravailperf->GetDiskAvailPerfRankingUnit(
            diskkey, IOStat::IOST_raw, &diskavailperf);
    diskavailperf->CalculateDiskAvailPerf();
    subpoolavailperf->Reserve(diskkey, IOStat::IOST_raw, 40, 160, 2);
    subpoolavailperf->Reserve(diskkey, IOStat::IOST_raw, 100, 400, 5);
    ret = subpoolavailperf->Consume(diskkey, IOStat::IOST_raw, 40, 160, 2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskavailperf->GetIOAvail(), 178500);
    ASSERT_EQ(diskavailperf->GetIOReserved(), 100);
    ASSERT_EQ(diskavailperf->GetBWAvail(), 737274000);
    ASSERT_EQ(diskavailperf->GetBWReserved(), 400);
    ASSERT_EQ(diskavailperf->GetCapAvail(), 127987);
    ASSERT_EQ(diskavailperf->GetCapReserved(), 5);
    diskavailperf->GetDiskPerf(&diskperf);
    ASSERT_EQ(diskperf->GetIOConsumed(), 1400);
    ASSERT_EQ(diskperf->GetBWConsumed(), 5600);
    ASSERT_EQ(diskperf->GetCapConsumed(), 8);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, GetFavoriteDisk_FailToGetDiskAvailPerf_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID + 2);
    ret = subpoolavailperf->GetFavoriteDisk(Tier::TIER_0,
            IOStat::IOST_raw, 40, 160, 2, NULL, NULL, &diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, GetFavoriteDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    BlackList blacklist;
    WhiteList whitelist;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID + 1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 4);
    blacklist.AddDisk(diskkey);
    whitelist.AddDisk(diskkey);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 5);
    whitelist.AddDisk(diskkey);
    ret = subpoolavailperf->GetFavoriteDisk(Tier::TIER_1,
            IOStat::IOST_raw, 40, 160, 2, &blacklist, &whitelist, &diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskkey.GetDiskId(), 1105);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, Destroy_ByTier_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID);
    ret = subpoolavailperf->Destroy(Tier::TIER_1);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolavailperf->GetTierDiskAvailPerfRanking(Tier::TIER_1, &tieravailperf);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = subpoolavailperf->GetTierDiskAvailPerfRanking(Tier::TIER_2, &tieravailperf);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(SubPoolDiskAvailPerfRankingTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = 0;
    DiskKey diskkey;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;
    TierDiskAvailPerfRanking* tieravailperf = NULL;

    subpoolavailperf = GetSubPoolAvailPerf(Constant::SUBPOOL_ID);
    ret = subpoolavailperf->Destroy();
    ASSERT_EQ(ret, YWB_SUCCESS);
    for(tier = Tier::TIER_0; tier < Tier::TIER_cnt; tier++)
    {
        ret = subpoolavailperf->GetTierDiskAvailPerfRanking(Tier::TIER_1, &tieravailperf);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(PerfManagerTest, SetupSubPoolDiskAvailPerfRanking_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;

    perfmgr = GetPerfManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolid = Constant::SUBPOOL_ID;

    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = perfmgr->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolavailperf);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }
}

TEST_F(PerfManagerTest, GetSubPoolDiskAvailPerfRanking_NoSuchSubPoolAvailPerf_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;

    perfmgr = GetPerfManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 1000);

    ret = perfmgr->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolavailperf);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PerfManagerTest, GetFavoriteDisk_NoSuchSubPoolAvailPerf_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    PerfManager* perfmgr = NULL;

    perfmgr = GetPerfManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 1000);

    ret = perfmgr->GetFavoriteDisk(subpoolkey, Tier::TIER_1,
            IOStat::IOST_raw, 40, 160, 2, NULL, NULL, &diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PerfManagerTest, GetFavoriteDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    BlackList blacklist;
    WhiteList whitelist;
    PerfManager* perfmgr = NULL;

    perfmgr = GetPerfManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 4);
    blacklist.AddDisk(diskkey);
    whitelist.AddDisk(diskkey);
    diskkey.SetDiskId(Constant::DISK_ID + 1100 + 5);
    whitelist.AddDisk(diskkey);
    ret = perfmgr->GetFavoriteDisk(subpoolkey, Tier::TIER_1,
            IOStat::IOST_raw, 40, 160, 2, &blacklist, &whitelist, &diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskkey.GetDiskId(), 1105);
}

TEST_F(PerfManagerTest, Destroy_BySubPool_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;

    perfmgr = GetPerfManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    ret = perfmgr->Destroy(subpoolkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = perfmgr->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolavailperf);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PerfManagerTest, Destroy_NoGivenSubPool_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    PerfManager* perfmgr = NULL;

    perfmgr = GetPerfManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 1000);
    ret = perfmgr->Destroy(subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PerfManagerTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    PerfManager* perfmgr = NULL;
    SubPoolDiskAvailPerfRanking* subpoolavailperf = NULL;

    perfmgr = GetPerfManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolid = Constant::SUBPOOL_ID;
    ret = perfmgr->Destroy();
    ASSERT_EQ(ret, YWB_SUCCESS);
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = perfmgr->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolavailperf);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

/* vim:set ts=4 sw=4 expandtab */
