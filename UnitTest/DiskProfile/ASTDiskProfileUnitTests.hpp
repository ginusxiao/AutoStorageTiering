#ifndef __AST_DISK_PROFILE_UNIT_TEST_HPP__
#define __AST_DISK_PROFILE_UNIT_TEST_HPP__

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "gtest/gtest.h"
#include "AST/ASTDiskProfile.hpp"

class ASTDiskProfileTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class ASTSubDiskProfileTest: public testing::Test
{
public:
    ASTSubDiskProfileTest()
    {
        mSubDiskProfile = new SubDiskProfile(20);
    }

    virtual ~ASTSubDiskProfileTest()
    {
        if(mSubDiskProfile)
        {
            delete mSubDiskProfile;
            mSubDiskProfile = NULL;
        }
    }

    virtual void SetUp() {}
    virtual void TearDown() {}

    ywb_status_t AddDiskProfileEntry(ywb_uint32_t iosize,
            ywb_uint32_t bw, ywb_uint32_t iops);
    ywb_status_t GetFirstProfileEntry(ywb_uint32_t* iosize,
            ywb_uint32_t* bw, ywb_uint32_t* iops);
    ywb_status_t GetNextProfileEntry(ywb_uint32_t* iosize,
            ywb_uint32_t* bw, ywb_uint32_t* iops);
    ywb_status_t GetFirstProfileEntryReverse(ywb_uint32_t* iosize,
            ywb_uint32_t* bw, ywb_uint32_t* iops);

    void Sort();

private:
    SubDiskProfile* mSubDiskProfile;
};

class ASTDiskProfileTest: public testing::Test
{
public:
    ASTDiskProfileTest()
    {
        mDiskPerf = new DiskPerfProfile();
    }

    virtual ~ASTDiskProfileTest()
    {
        if(mDiskPerf)
        {
            delete mDiskPerf;
            mDiskPerf = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    ywb_status_t GetDiskProfileEntry(ywb_uint32_t rwratio,
            ywb_uint32_t iosize, ywb_uint32_t* bw,
            ywb_uint32_t* ios, ywb_bool_t random);

    ywb_status_t GetFirstRWRatioLess(
            ywb_uint32_t rwratio, ywb_uint32_t* val, ywb_bool_t random);
    ywb_status_t GetFirstRWRatioGreater(
            ywb_uint32_t rwratio, ywb_uint32_t* val, ywb_bool_t random);
//    ywb_uint32_t LinearInterpolation(ywb_uint32_t pointax,
//            ywb_uint32_t pointay, ywb_uint32_t pointbx,
//            ywb_uint32_t pointby, ywb_uint32_t targetx);
//    ywb_uint32_t WeightedInterpolation(ywb_uint32_t weight1,
//            ywb_uint32_t value1, ywb_uint32_t weight2,
//            ywb_uint32_t value2);

    DiskPerfProfile* GetDiskPerfProfile();

private:
    DiskPerfProfile* mDiskPerf;
};

class ASTPerfProfileManagerTest : public testing::Test
{
public:
    ASTPerfProfileManagerTest()
    {
        char curdir[255] = {0};

        mPerfMgr = new PerfProfileManager(NULL);
        getcwd(curdir, sizeof(curdir));
        mDiskProfileForTest.assign(strcat(curdir,
                "/../../../src/OSS/AST/diskprofile_test.xml"));
    }

    virtual ~ASTPerfProfileManagerTest()
    {
        if(mPerfMgr)
        {
            delete mPerfMgr;
            mPerfMgr = NULL;
        }
    }

    virtual void SetUp() {}
    virtual void TearDown() {}

    inline string& GetDiskProfileForTest()
    {
        return mDiskProfileForTest;
    }

    PerfProfileManager* GetPerfProfileManager();
    ywb_status_t GetDiskProfileEntry(DiskBaseProp& prop,
            ywb_uint32_t rwratio, ywb_uint32_t iosize,
            ywb_uint32_t* bw, ywb_uint32_t* ios, ywb_bool_t random);

private:
    PerfProfileManager* mPerfMgr;
    /*path of disk profile for test*/
    string mDiskProfileForTest;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
