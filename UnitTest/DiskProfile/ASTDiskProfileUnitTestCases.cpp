#include "UnitTest/AST/DiskProfile/ASTDiskProfileUnitTests.hpp"

TEST_F(ASTSubDiskProfileTest, GetFirstEntryFromEmpty_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t iosize = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;

    ret = GetFirstProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(YFS_ENOENT, ret);
}

TEST_F(ASTSubDiskProfileTest, GetFirstEntryReverseFromEmpty_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t iosize = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;

    ret = GetFirstProfileEntryReverse(&iosize, &bw, &iops);
    ASSERT_EQ(YFS_ENOENT, ret);
}

TEST_F(ASTSubDiskProfileTest, TraverseEntries_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t iosize = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;

    AddDiskProfileEntry(4, 200, 10000);
    AddDiskProfileEntry(8, 400, 5000);
    AddDiskProfileEntry(16, 800, 2500);

    ret = GetFirstProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(iosize, 4);
    ASSERT_EQ(bw, 200);
    ASSERT_EQ(iops, 10000);
    ASSERT_EQ(YWB_SUCCESS, ret);

    ret = GetNextProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(iosize, 8);
    ASSERT_EQ(bw, 400);
    ASSERT_EQ(iops, 5000);
    ASSERT_EQ(YWB_SUCCESS, ret);

    ret = GetNextProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(iosize, 16);
    ASSERT_EQ(bw, 800);
    ASSERT_EQ(iops, 2500);
    ASSERT_EQ(YWB_SUCCESS, ret);

    ret = GetNextProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(YFS_ENOENT, ret);

    ret = GetFirstProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(iosize, 4);
    ASSERT_EQ(bw, 200);
    ASSERT_EQ(iops, 10000);
    ASSERT_EQ(YWB_SUCCESS, ret);
}

TEST_F(ASTSubDiskProfileTest, GetFirstEntryReverse_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t iosize = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;

    AddDiskProfileEntry(4, 200, 10000);
    AddDiskProfileEntry(8, 400, 5000);
    AddDiskProfileEntry(16, 800, 2500);

    ret = GetFirstProfileEntryReverse(&iosize, &bw, &iops);
    ASSERT_EQ(iosize, 16);
    ASSERT_EQ(bw, 800);
    ASSERT_EQ(iops, 2500);
    ASSERT_EQ(YWB_SUCCESS, ret);
}

TEST_F(ASTSubDiskProfileTest, SortEntries_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t iosize = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;

    AddDiskProfileEntry(8, 400, 5000);
    AddDiskProfileEntry(16, 800, 2500);
    AddDiskProfileEntry(4, 200, 10000);

    Sort();

    ret = GetFirstProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(iosize, 16);
    ASSERT_EQ(bw, 800);
    ASSERT_EQ(iops, 2500);
    ASSERT_EQ(YWB_SUCCESS, ret);

    ret = GetNextProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(iosize, 8);
    ASSERT_EQ(bw, 400);
    ASSERT_EQ(iops, 5000);
    ASSERT_EQ(YWB_SUCCESS, ret);

    ret = GetNextProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(iosize, 4);
    ASSERT_EQ(bw, 200);
    ASSERT_EQ(iops, 10000);
    ASSERT_EQ(YWB_SUCCESS, ret);

    ret = GetNextProfileEntry(&iosize, &bw, &iops);
    ASSERT_EQ(YFS_ENOENT, ret);
}

TEST_F(ASTDiskProfileTest, SetGetRndPerf_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* diskperf = NULL;
    SubDiskProfile* subprofile = NULL;

    diskperf = GetDiskPerfProfile();
    ASSERT_EQ(diskperf->RndPerfEmpty(), false);
    ASSERT_EQ(diskperf->SeqPerfEmpty(), true);

    subprofile = new SubDiskProfile(5);
    diskperf->SetRndPerf(5, subprofile);
    ret = diskperf->GetRndPerf(5, &subprofile);
    ASSERT_EQ(YWB_SUCCESS, ret);
    ASSERT_EQ(subprofile != NULL, true);
}

TEST_F(ASTDiskProfileTest, GetRndPerf_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* diskperf = NULL;
    SubDiskProfile* subprofile = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->GetRndPerf(15, &subprofile);
    ASSERT_EQ(YFS_ENOENT, ret);

    delete subprofile;
}

TEST_F(ASTDiskProfileTest, IsRWRatioFuzzyMatch_TRUE)
{
    ywb_bool_t ret = false;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->IsRWRatioFuzzyMatch(65, 64);
    ASSERT_EQ(ret, true);
}

TEST_F(ASTDiskProfileTest, IsRWRatioFuzzyMatch_FALSE)
{
    ywb_bool_t ret = false;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->IsRWRatioFuzzyMatch(65, 71);
    ASSERT_EQ(ret, false);
}

TEST_F(ASTDiskProfileTest, IsIOSizeFuzzyMatch_TRUE)
{
    ywb_bool_t ret = false;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->IsIOSizeFuzzyMatch(64, 62);
    ASSERT_EQ(ret, true);
}

TEST_F(ASTDiskProfileTest, IsIOSizeFuzzyMatch_FALSE)
{
    ywb_bool_t ret = false;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->IsIOSizeFuzzyMatch(64, 60);
    ASSERT_EQ(ret, false);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_DiskProfileEmpty_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* diskperf = NULL;
    ywb_uint32_t rwratio = 8;
    ywb_uint32_t iosize = 4;
    ywb_uint32_t bw = 0;
    ywb_uint32_t ios = 0;
    ywb_bool_t random = true;

    diskperf = new DiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rwratio, iosize, &bw, &ios, random);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_SubDiskProfileEmpty_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t rwratio = 8;
    ywb_uint32_t iosize = 4;
    ywb_uint32_t bw = 0;
    ywb_uint32_t ios = 0;
    ywb_bool_t random = true;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rwratio, iosize, &bw, &ios, random);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_SubDiskProfileHasNoEntry_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t rwratio = 30;
    ywb_uint32_t iosize = 4;
    ywb_uint32_t bw = 0;
    ywb_uint32_t ios = 0;
    ywb_bool_t random = true;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rwratio, iosize, &bw, &ios, random);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_SubDiskProfileOnlyOneEntry_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t rwratio = 60;
    ywb_uint32_t iosize = 25;
    ywb_uint32_t bw1 = 0;
    ywb_uint32_t ios1 = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;
    ywb_bool_t random = true;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rwratio, iosize, &bw1, &ios1, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetDiskProfileEntry(rwratio, 4, &bw2, &ios2, random);
    ASSERT_EQ(bw1, bw2);
    ASSERT_EQ(ios1, ios2);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_IOSizeLargerThanMaximum_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t rwratio = 10;
    ywb_uint32_t iosize = 1024;
    ywb_uint32_t bw1 = 0;
    ywb_uint32_t ios1 = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;
    bool random = true;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rwratio, iosize, &bw1, &ios1, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetDiskProfileEntry(rwratio, 16, &bw2, &ios2, random);
    ASSERT_EQ(bw1, bw2);
    ASSERT_EQ(ios1, ios2);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_IOSizeLessThanMinimum_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t rwratio = 10;
    ywb_uint32_t iosize = 1;
    ywb_uint32_t bw1 = 0;
    ywb_uint32_t ios1 = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;
    bool random = true;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rwratio, iosize, &bw1, &ios1, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetDiskProfileEntry(rwratio, 4, &bw2, &ios2, random);
    ASSERT_EQ(bw1, bw2);
    ASSERT_EQ(ios1, ios2);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_IOSizeExactMatch_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t rwratio = 10;
    ywb_uint32_t iosize = 8;
    ywb_uint32_t bw1 = 0;
    ywb_uint32_t ios1 = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;
    bool random = true;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rwratio, iosize, &bw1, &ios1, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetDiskProfileEntry(rwratio, 8, &bw2, &ios2, random);
    ASSERT_EQ(bw1, bw2);
    ASSERT_EQ(ios1, ios2);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_LinearInterpolation_SUCCESS)
{
    ywb_uint32_t pointax = 0;
    ywb_uint32_t pointay = 0;
    ywb_uint32_t pointbx = 0;
    ywb_uint32_t pointby = 0;
    ywb_uint32_t targetx = 0;
    ywb_uint32_t targety = 0;

    pointax = 1;
    pointay = 3;
    pointbx = 4;
    pointby = 6;
    targetx = 2;
    targety = DiskPerfProfile::LinearInterpolation(
            pointax, pointay, pointbx, pointby, targetx);
    ASSERT_EQ(targety, 4);

    pointax = 4;
    pointay = 6;
    pointbx = 1;
    pointby = 3;
    targetx = 2;
    targety = DiskPerfProfile::LinearInterpolation(
            pointax, pointay, pointbx, pointby, targetx);
    ASSERT_EQ(targety, 4);

    pointax = 1;
    pointay = 6;
    pointbx = 4;
    pointby = 3;
    targetx = 2;
    targety = DiskPerfProfile::LinearInterpolation(
            pointax, pointay, pointbx, pointby, targetx);
    ASSERT_EQ(targety, 5);

    pointax = 4;
    pointay = 3;
    pointbx = 1;
    pointby = 6;
    targetx = 2;
    targety = DiskPerfProfile::LinearInterpolation(
            pointax, pointay, pointbx, pointby, targetx);
    ASSERT_EQ(targety, 5);
}

//TEST_F(ASTDiskProfileTest, CalculateDiskPerf_WeightedInterpolation_SUCCESS)
//{
//
//}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_IOSizeNotExactMatch_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t rwratio = 10;
    ywb_uint32_t iosize = 6;
    ywb_uint32_t bw1 = 0;
    ywb_uint32_t ios1 = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;
    ywb_uint32_t bw3 = 0;
    ywb_uint32_t ios3 = 0;
    ywb_uint32_t bw4 = 0;
    ywb_uint32_t ios4 = 0;
    bool random = true;
    DiskPerfProfile* diskperf = NULL;

    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rwratio, iosize, &bw1, &ios1, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetDiskProfileEntry(rwratio, 4, &bw2, &ios2, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetDiskProfileEntry(rwratio, 8, &bw3, &ios3, random);
    ASSERT_EQ(ret, YWB_SUCCESS);

    bw4 = DiskPerfProfile::LinearInterpolation(4, bw2, 8, bw3, iosize);
    ios4 = DiskPerfProfile::LinearInterpolation(4, ios2, 8, ios3, iosize);
    ASSERT_EQ(bw1, bw4);
    ASSERT_EQ(ios1, ios4);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_RndSeqRatio0AndRWRatioFuzzyMatch100AndSeqPerfEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* diskperf = NULL;
    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;
    ywb_uint32_t rndbw = 0;
    ywb_uint32_t rndios = 0;
    ywb_uint32_t seqbw = 0;
    ywb_uint32_t seqios = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t ios = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;

    rndseqratio = 0;
    seqrwratio = 99;
    seqiosize = 4;
    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rndseqratio, rndrwratio,
            rndiosize, seqrwratio, seqiosize, &bw, &ios);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = diskperf->CalculateDiskPerf(100, seqiosize, &seqbw, &seqios, true);
    ASSERT_EQ(ret, YWB_SUCCESS);

    bw2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndbw, (100 - rndseqratio), seqbw);
    ios2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndios, (100 - rndseqratio), seqios);

    ASSERT_EQ(bw, bw2);
    ASSERT_EQ(ios, ios2);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_RndSeqRatio100AndRWRatioFuzzyMatch100AndSeqPerfEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* diskperf = NULL;
    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;
    ywb_uint32_t rndbw = 0;
    ywb_uint32_t rndios = 0;
    ywb_uint32_t seqbw = 0;
    ywb_uint32_t seqios = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t ios = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;

    rndseqratio = 100;
    rndrwratio = 99;
    rndiosize = 4;
    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rndseqratio, rndrwratio,
            rndiosize, seqrwratio, seqiosize, &bw, &ios);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = diskperf->CalculateDiskPerf(100, rndiosize, &rndbw, &rndios, true);
    ASSERT_EQ(ret, YWB_SUCCESS);

    bw2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndbw, (100 - rndseqratio), seqbw);
    ios2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndios, (100 - rndseqratio), seqios);

    ASSERT_EQ(bw, bw2);
    ASSERT_EQ(ios, ios2);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_RndSeqRatio0AndRWRatioExactMatchAndSeqPerfEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* diskperf = NULL;
    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;
    ywb_uint32_t rndbw = 0;
    ywb_uint32_t rndios = 0;
    ywb_uint32_t seqbw = 0;
    ywb_uint32_t seqios = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t ios = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;

    rndseqratio = 0;
    seqrwratio = 50;
    seqiosize = 8;
    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rndseqratio, rndrwratio,
            rndiosize, seqrwratio, seqiosize, &bw, &ios);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = diskperf->CalculateDiskPerf(seqrwratio, seqiosize, &seqbw, &seqios, true);
    ASSERT_EQ(ret, YWB_SUCCESS);

    bw2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndbw, (100 - rndseqratio), seqbw);
    ios2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndios, (100 - rndseqratio), seqios);

    ASSERT_EQ(bw, bw2);
    ASSERT_EQ(ios, ios2);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_RndSeqRatio100AndRWRatioExactMatchAndSeqPerfEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* diskperf = NULL;
    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;
    ywb_uint32_t rndbw = 0;
    ywb_uint32_t rndios = 0;
    ywb_uint32_t seqbw = 0;
    ywb_uint32_t seqios = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t ios = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;

    rndseqratio = 100;
    rndrwratio = 50;
    rndiosize = 4;
    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rndseqratio, rndrwratio,
            rndiosize, seqrwratio, seqiosize, &bw, &ios);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = diskperf->CalculateDiskPerf(rndrwratio, rndiosize, &rndbw, &rndios, true);
    ASSERT_EQ(ret, YWB_SUCCESS);

    bw2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndbw, (100 - rndseqratio), seqbw);
    ios2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndios, (100 - rndseqratio), seqios);

    ASSERT_EQ(bw, bw2);
    ASSERT_EQ(ios, ios2);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_RndSeqPerfWeightedInterpolationAndSeqPerfEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* diskperf = NULL;
    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;
    ywb_uint32_t rndbw = 0;
    ywb_uint32_t rndios = 0;
    ywb_uint32_t seqbw = 0;
    ywb_uint32_t seqios = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t ios = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;

    rndseqratio = 10;
    rndrwratio = 50;
    rndiosize = 4;
    seqrwratio = 50;
    seqiosize = 4;
    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rndseqratio, rndrwratio,
            rndiosize, seqrwratio, seqiosize, &bw, &ios);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = diskperf->CalculateDiskPerf(rndrwratio, rndiosize, &rndbw, &rndios, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskperf->CalculateDiskPerf(seqrwratio, seqiosize, &seqbw, &seqios, true);
    ASSERT_EQ(ret, YWB_SUCCESS);

    bw2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndbw, (100 - rndseqratio), seqbw);
    ios2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndios, (100 - rndseqratio), seqios);

    ASSERT_EQ(bw, bw2);
    ASSERT_EQ(ios, ios2);
}

TEST_F(ASTDiskProfileTest, CalculateDiskPerf_RWRatioInterpolationAndSeqPerfEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* diskperf = NULL;
    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio1 = 0;
    ywb_uint32_t rndrwratio2 = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio1 = 0;
    ywb_uint32_t seqrwratio2 = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;
    ywb_uint32_t rndbw1 = 0;
    ywb_uint32_t rndios1 = 0;
    ywb_uint32_t rndbw2 = 0;
    ywb_uint32_t rndios2 = 0;
    ywb_uint32_t seqbw1 = 0;
    ywb_uint32_t seqios1 = 0;
    ywb_uint32_t seqbw2 = 0;
    ywb_uint32_t seqios2 = 0;
    ywb_uint32_t rndbw = 0;
    ywb_uint32_t rndios = 0;
    ywb_uint32_t seqbw = 0;
    ywb_uint32_t seqios = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t ios = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t ios2 = 0;
    ywb_bool_t random = true;

    rndseqratio = 10;
    rndrwratio = 70;
    rndiosize = 4;
    seqrwratio = 50;
    seqiosize = 4;
    diskperf = GetDiskPerfProfile();
    ret = diskperf->CalculateDiskPerf(rndseqratio, rndrwratio,
            rndiosize, seqrwratio, seqiosize, &bw, &ios);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = GetFirstRWRatioLess(rndrwratio, &rndrwratio1, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskperf->CalculateDiskPerf(rndrwratio1, rndiosize, &rndbw1, &rndios1, true);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = GetFirstRWRatioGreater(rndrwratio, &rndrwratio2, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskperf->CalculateDiskPerf(rndrwratio2, rndiosize, &rndbw2, &rndios2, true);
    ASSERT_EQ(ret, YWB_SUCCESS);

    rndbw = DiskPerfProfile::LinearInterpolation(
            rndrwratio1, rndbw1, rndrwratio2, rndbw2, rndrwratio);
    rndios = DiskPerfProfile::LinearInterpolation(
            rndrwratio1, rndios1, rndrwratio2, rndios2, rndrwratio);

    ret = GetFirstRWRatioLess(seqrwratio, &seqrwratio1, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskperf->CalculateDiskPerf(seqrwratio1, seqiosize, &seqbw1, &seqios1, true);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = GetFirstRWRatioGreater(seqrwratio, &seqrwratio2, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskperf->CalculateDiskPerf(seqrwratio2, seqiosize, &seqbw2, &seqios2, true);
    ASSERT_EQ(ret, YWB_SUCCESS);

    seqbw = DiskPerfProfile::LinearInterpolation(
            seqrwratio1, seqbw1, seqrwratio2, seqbw2, seqrwratio);
    seqios = DiskPerfProfile::LinearInterpolation(
            seqrwratio1, seqios1, seqrwratio2, seqios2, seqrwratio);

    bw2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndbw, (100 - rndseqratio), seqbw);
    ios2 = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndios, (100 - rndseqratio), seqios);

    ASSERT_EQ(bw, bw2);
    ASSERT_EQ(ios, ios2);
}

TEST_F(ASTPerfProfileManagerTest, GetDiskPerfProfile_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp* diskprop = NULL;
    PerfProfileManager* perfmgr = NULL;
    DiskPerfProfile* diskperf = NULL;
    DiskPerfProfile* diskperf2 = NULL;

    diskprop = new DiskBaseProp(0, 1, 1, 15000, 2000);
    diskperf = new DiskPerfProfile();
    perfmgr = GetPerfProfileManager();
    perfmgr->AddDiskPerfProfile(*diskprop, diskperf);
    ret = perfmgr->GetDiskPerfProfile(*diskprop, &diskperf2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskperf2, diskperf);
}

TEST_F(ASTPerfProfileManagerTest, GetDiskPerfProfile_FuzzyMatch_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp* diskprop = NULL;
    DiskBaseProp* diskprop2 = NULL;
    PerfProfileManager* perfmgr = NULL;
    DiskPerfProfile* diskperf = NULL;
    DiskPerfProfile* diskperf2 = NULL;

    diskprop = new DiskBaseProp(0, 1, 1, 15000, 2000);
    diskprop2 = new DiskBaseProp(0, 1, 1, 15000, 2500);
    diskperf = new DiskPerfProfile();
    perfmgr = GetPerfProfileManager();
    perfmgr->AddDiskPerfProfile(*diskprop, diskperf);
    ret = perfmgr->GetDiskPerfProfile(*diskprop2, &diskperf2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskperf2, diskperf);
}

TEST_F(ASTPerfProfileManagerTest, GetDiskPerfProfile_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp* diskprop = NULL;
    DiskBaseProp* diskprop2 = NULL;
    PerfProfileManager* perfmgr = NULL;
    DiskPerfProfile* diskperf = NULL;
    DiskPerfProfile* diskperf2 = NULL;

    diskprop = new DiskBaseProp(0, 1, 1, 15000, 2000);
    diskprop2 = new DiskBaseProp(0, 1, 1, 10000, 2000);
    diskperf = new DiskPerfProfile();
    perfmgr = GetPerfProfileManager();
    perfmgr->AddDiskPerfProfile(*diskprop, diskperf);
    ret = perfmgr->GetDiskPerfProfile(*diskprop2, &diskperf2);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTPerfProfileManagerTest, BuildDiskPerfProfile_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    PerfProfileManager* perfmgr = NULL;
    DiskBaseProp* diskprop = NULL;
    ywb_uint32_t rwratio = 0;
    ywb_uint32_t iosize = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;
    ywb_bool_t random = true;
    string diskprofiletest;

    perfmgr = GetPerfProfileManager();
    diskprofiletest = GetDiskProfileForTest();
    ret = perfmgr->BuildDiskPerfProfile(diskprofiletest);
    ASSERT_EQ(ret, YWB_SUCCESS);

    diskprop = new DiskBaseProp(0, 1, 2, 10000, 128000);
    rwratio = 50;
    iosize = 8;
    ret = GetDiskProfileEntry(*diskprop, rwratio, iosize, &bw, &iops, random);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(bw, 173420000);
    ASSERT_EQ(iops, 21677000);

    rwratio = 40;
    ret = GetDiskProfileEntry(*diskprop, rwratio, iosize, &bw, &iops, random);
    ASSERT_EQ(ret, YFS_ENOENT);

    diskprop->SetRaidType(DiskBaseProp::DRT_raid0);
    ret = GetDiskProfileEntry(*diskprop, rwratio, iosize, &bw, &iops, random);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTPerfProfileManagerTest, CalculateDiskPerf_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    PerfProfileManager* perfmgr = NULL;
    DiskBaseProp* diskprop = NULL;
    DiskPerfProfile* diskperf = NULL;
    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t iops2 = 0;
    string diskprofiletest;

    perfmgr = GetPerfProfileManager();
    diskprofiletest = GetDiskProfileForTest();
    ret = perfmgr->BuildDiskPerfProfile(diskprofiletest);
    ASSERT_EQ(ret, YWB_SUCCESS);

    diskprop = new DiskBaseProp(0, 1, 2, 10000, 128000);
    ret = perfmgr->CalculateDiskPerf(*diskprop, rndseqratio,
            rndrwratio, rndiosize, seqrwratio, seqiosize, &bw, &iops);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ret = perfmgr->GetDiskPerfProfile(*diskprop, &diskperf);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = diskperf->CalculateDiskPerf(rndseqratio, rndrwratio,
            rndiosize, seqrwratio, seqiosize, &bw2, &iops2);
    ASSERT_EQ(ret, YWB_SUCCESS);

    ASSERT_EQ(bw, bw2);
    ASSERT_EQ(iops, iops2);
}

TEST_F(ASTPerfProfileManagerTest, CalculateDiskPerf_HardCoded_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    PerfProfileManager* perfmgr = NULL;
    DiskBaseProp* diskprop = NULL;
    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;
    string diskprofiletest;

    perfmgr = GetPerfProfileManager();
    diskprofiletest = GetDiskProfileForTest();
    ret = perfmgr->BuildDiskPerfProfile(diskprofiletest);
    ASSERT_EQ(ret, YWB_SUCCESS);

    diskprop = new DiskBaseProp(0, 1, 2, 5000, 128000);
    ret = perfmgr->CalculateDiskPerf(*diskprop, rndseqratio,
            rndrwratio, rndiosize, seqrwratio, seqiosize, &bw, &iops);
    ASSERT_EQ(ret, YWB_SUCCESS);
    /*use (Constant::DEFAULT_ENT_BW + 0) to avoid undefined reference issue*/
    ASSERT_EQ(bw, (ywb_uint32_t)Constant::DEFAULT_ENT_BW);
    ASSERT_EQ(iops, (ywb_uint32_t)Constant::DEFAULT_ENT_IOPS);
}

TEST_F(ASTPerfProfileManagerTest, CalculateDiskPerf_HardCoded_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    PerfProfileManager* perfmgr = NULL;
    DiskBaseProp* diskprop = NULL;
    ywb_uint32_t rndseqratio = 0;
    ywb_uint32_t rndrwratio = 0;
    ywb_uint32_t rndiosize = 0;
    ywb_uint32_t seqrwratio = 0;
    ywb_uint32_t seqiosize = 0;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;
    string diskprofiletest;

    perfmgr = GetPerfProfileManager();
    diskprofiletest = GetDiskProfileForTest();
    ret = perfmgr->BuildDiskPerfProfile(diskprofiletest);
    ASSERT_EQ(ret, YWB_SUCCESS);

    diskprop = new DiskBaseProp(0, 1, 100, 10000, 128000);
    ret = perfmgr->CalculateDiskPerf(*diskprop, rndseqratio,
            rndrwratio, rndiosize, seqrwratio, seqiosize, &bw, &iops);
    ASSERT_EQ(ret, YFS_EINVAL);
}

/* vim:set ts=4 sw=4 expandtab */
