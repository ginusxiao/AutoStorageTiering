#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/DiskProfile/ASTDiskProfileUnitTests.hpp"

ywb_status_t
ASTSubDiskProfileTest::AddDiskProfileEntry(ywb_uint32_t iosize,
        ywb_uint32_t bw, ywb_uint32_t iops)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskProfileEntry* diskentry = NULL;

    diskentry = new DiskProfileEntry(
            mSubDiskProfile->GetRWRatio(), iosize, bw, iops);
    if(NULL == diskentry)
    {
        ast_log_debug("Out of memory!");
        return YFS_EOUTOFMEMORY;
    }
    else
    {
        mSubDiskProfile->AddDiskProfileEntry(diskentry);
    }

    return ret;
}

ywb_status_t
ASTSubDiskProfileTest::GetFirstProfileEntry(ywb_uint32_t* iosize,
        ywb_uint32_t* bw, ywb_uint32_t* iops)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskProfileEntry* diskentry = NULL;

    ret = mSubDiskProfile->GetFirstEntry(&diskentry);
    if(YWB_SUCCESS == ret)
    {
        *iosize = diskentry->GetIOSize();
        *bw = diskentry->GetBW();
        *iops = diskentry->GetIOPS();
    }

    return ret;
}

ywb_status_t
ASTSubDiskProfileTest::GetNextProfileEntry(ywb_uint32_t* iosize,
        ywb_uint32_t* bw, ywb_uint32_t* iops)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskProfileEntry* diskentry = NULL;

    ret = mSubDiskProfile->GetNextEntry(&diskentry);
    if(YWB_SUCCESS == ret)
    {
        *iosize = diskentry->GetIOSize();
        *bw = diskentry->GetBW();
        *iops = diskentry->GetIOPS();
    }

    return ret;
}

ywb_status_t
ASTSubDiskProfileTest::GetFirstProfileEntryReverse(ywb_uint32_t* iosize,
        ywb_uint32_t* bw, ywb_uint32_t* iops)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskProfileEntry* diskentry = NULL;

    ret = mSubDiskProfile->GetFirstEntryReverse(&diskentry);
    if(YWB_SUCCESS == ret)
    {
        *iosize = diskentry->GetIOSize();
        *bw = diskentry->GetBW();
        *iops = diskentry->GetIOPS();
    }

    return ret;
}

void
ASTSubDiskProfileTest::Sort()
{
    mSubDiskProfile->Sort();
}

void
ASTDiskProfileTest::SetUp()
{
    ywb_uint32_t loop = 0;
    ywb_uint32_t rwratio = 0;
    DiskProfileEntry* entry1 = NULL;
    DiskProfileEntry* entry2 = NULL;
    DiskProfileEntry* entry3 = NULL;
    SubDiskProfile* subprofile = NULL;

    for(loop = 0; loop < 11; loop++)
    {
        rwratio = loop * 10;
        subprofile = new SubDiskProfile(rwratio);

        if(loop == 3)
        {
            /*rwratio of 30 has no disk profile entry*/
        }
        else if(loop == 6)
        {
            /*rwratio of 60 has only one disk profile entry*/
            entry1 = new DiskProfileEntry(rwratio, 4, (200 + rwratio), (20000 - 10 * rwratio));
            subprofile->AddDiskProfileEntry(entry1);
        }
        else
        {
            entry1 = new DiskProfileEntry(rwratio, 4, (200 + rwratio), (20000 - 10 * rwratio));
            entry2 = new DiskProfileEntry(rwratio, 8, (400 + 2 * rwratio), (10000 - 20 * rwratio));
            entry3 = new DiskProfileEntry(rwratio, 16, (800 + 4 * rwratio), (5000 - 40 * rwratio));
            subprofile->AddDiskProfileEntry(entry1);
            subprofile->AddDiskProfileEntry(entry2);
            subprofile->AddDiskProfileEntry(entry3);
        }

        subprofile->Sort();
        /*currently only test random performance*/
        mDiskPerf->SetRndPerf(rwratio, subprofile);
    }
}

void
ASTDiskProfileTest::TearDown()
{

}

ywb_status_t
ASTDiskProfileTest::GetDiskProfileEntry(ywb_uint32_t rwratio,
        ywb_uint32_t iosize, ywb_uint32_t* bw,
        ywb_uint32_t* ios, ywb_bool_t random)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubDiskProfile* subprofile = NULL;
    DiskProfileEntry* diskentry = NULL;

    if(random)
    {
        ret = mDiskPerf->GetRndPerf(rwratio, &subprofile);
    }
    else
    {
        ret = mDiskPerf->GetSeqPerf(rwratio, &subprofile);
    }

    if((YWB_SUCCESS == ret) && (subprofile != NULL))
    {
       ret = subprofile->GetFirstEntry(&diskentry);
       while((YWB_SUCCESS == ret) && (diskentry != NULL))
       {
           if(diskentry->GetIOSize() == iosize)
           {
               *bw = diskentry->GetBW();
               *ios = diskentry->GetIOPS();
               return YWB_SUCCESS;
           }

           ret = subprofile->GetNextEntry(&diskentry);
       }
    }

    return YFS_ENOENT;
}

ywb_status_t
ASTDiskProfileTest::GetFirstRWRatioLess(
        ywb_uint32_t rwratio, ywb_uint32_t* val, ywb_bool_t random)
{
    return mDiskPerf->GetFirstRWRatioLess(rwratio, val, random);
}

ywb_status_t
ASTDiskProfileTest::GetFirstRWRatioGreater(
        ywb_uint32_t rwratio, ywb_uint32_t* val, ywb_bool_t random)
{
    return mDiskPerf->GetFirstRWRatioGreater(rwratio, val, random);
}

//ywb_uint32_t
//ASTDiskProfileTest::LinearInterpolation(ywb_uint32_t pointax,
//        ywb_uint32_t pointay, ywb_uint32_t pointbx,
//        ywb_uint32_t pointby, ywb_uint32_t targetx)
//{
//    ywb_uint32_t targety = 0;
//
//    if((pointax > targetx) && (targetx > pointbx))
//    {
//        if(pointby < pointay)
//        {
//            targety = (((pointay - pointby) * (targetx - pointbx)) /
//                    (pointax - pointbx)) + pointby;
//        }
//        else
//        {
//            targety = pointby - (((pointby - pointay) * (targetx - pointbx)) /
//                    (pointax - pointbx));
//        }
//    }
//    else if((pointbx > targetx) && (targetx > pointax))
//    {
//        if(pointay < pointby)
//        {
//            targety = (((pointby - pointay) * (targetx - pointax)) /
//                    (pointbx - pointax)) + pointay;
//        }
//        else
//        {
//            targety = pointay - (((pointay - pointby) * (targetx - pointax)) /
//                    (pointbx - pointax));
//        }
//    }
//
//    return targety;
//}
//
//ywb_uint32_t
//ASTDiskProfileTest::WeightedInterpolation(ywb_uint32_t weight1,
//        ywb_uint32_t value1, ywb_uint32_t weight2, ywb_uint32_t value2)
//{
//    ywb_uint32_t weight = 0;
//    ywb_uint32_t ratio1 = 0;
//    ywb_uint32_t ratio2 = 0;
//    ywb_uint32_t value = 0;
//
//    weight = weight1 + weight2;
//    ratio1 = (weight == 0) ? 0 : ((100 * weight1) / weight);
//    ratio2 = 100 - ratio1;
//
//    if((ratio1 == 0) || (value1 == 0))
//    {
//        value = value2;
//    }
//    else if((ratio2 == 0) || (value2 == 0))
//    {
//        value = value1;
//    }
//    else if(ratio1 * value2 + ratio2 * value1 != 0)
//    {
//        value = ((value1 * value2) / (ratio1 * value2 + ratio2 * value1)) * 100;
//    }
//
//    return value;
//}

DiskPerfProfile*
ASTDiskProfileTest::GetDiskPerfProfile()
{
    return mDiskPerf;
}

PerfProfileManager*
ASTPerfProfileManagerTest::GetPerfProfileManager()
{
    return mPerfMgr;
}

ywb_status_t
ASTPerfProfileManagerTest::GetDiskProfileEntry(DiskBaseProp& prop,
        ywb_uint32_t rwratio, ywb_uint32_t iosize,
        ywb_uint32_t* bw, ywb_uint32_t* ios, ywb_bool_t random)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskPerfProfile* profile = NULL;
    SubDiskProfile* subprofile = NULL;
    DiskProfileEntry* diskentry = NULL;

    ret = mPerfMgr->GetDiskPerfProfile(prop, &profile);
    if((YWB_SUCCESS == ret) && (profile != NULL))
    {
        if(random)
        {
            ret = profile->GetRndPerf(rwratio, &subprofile);
        }
        else
        {
            ret = profile->GetSeqPerf(rwratio, &subprofile);
        }

        if((YWB_SUCCESS == ret) && (subprofile != NULL))
        {
           ret = subprofile->GetFirstEntry(&diskentry);
           while((YWB_SUCCESS == ret) && (diskentry != NULL))
           {
               if(diskentry->GetIOSize() == iosize)
               {
                   *bw = diskentry->GetBW();
                   *ios = diskentry->GetIOPS();
                   return YWB_SUCCESS;
               }

               ret = subprofile->GetNextEntry(&diskentry);
           }
        }
    }

    return YFS_ENOENT;
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

    testing::AddGlobalTestEnvironment(new ASTDiskProfileTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
