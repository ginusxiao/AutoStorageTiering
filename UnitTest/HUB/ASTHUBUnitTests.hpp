#ifndef __AST_MISC_UNIT_TEST_HPP__
#define __AST_MISC_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "FsCommon/YfsMutex.hpp"
#include "OSSCommon/ControlCenter.hpp"
#include "AST/ASTMigration.hpp"
#include "AST/ASTConstant.hpp"
#include "AST/ASTHUB.hpp"

extern ControlCenter* gCenter;
class ControlCenterMock : public ControlCenter
{
public:
    ControlCenterMock() : ControlCenter(), mMigReqCnt(0)
    {

    }

    virtual ~ControlCenterMock() {}

    inline ywb_uint32_t GetMigReqCnt()
    {
        return mMigReqCnt;
    }

    void
    ForwardToObjectLayout(OpCtx *ctx)
    {
        mMigReqCnt++;
    }

private:
    ywb_uint32_t mMigReqCnt;
};

class ASTHUBTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp()
    {
        mAST = GetSingleASTInstance();
        YWB_ASSERT(mAST != NULL);
        mCenter = new ControlCenterMock();
        YWB_ASSERT(mCenter != NULL);
        gCenter = mCenter;
    }

    virtual void TearDown()
    {
        if(mAST)
        {
            mAST->DecRef();
        }

        if(mCenter)
        {
            delete mCenter;
            mCenter = NULL;
        }

        gCenter = NULL;
    }

private:
    /*
     * HUB related test requires instance of AST which is
     * got by GetSingleASTInstance(), and all test cases
     * will be executed in the same thread which will lead
     * to all test cases have the same AST instance, but
     * precedent test case may de-construct AST instance at
     * the end of test, leading to successive test have
     * mal-constructed AST instance, eg:
     * TEST_F(testcase1)
     * {
     *
     * }
     *
     * TEST_F(testcase2)
     * {
     *
     * }
     *
     * for context of testcase1:
     * AST instance @ast will be got at the very begining,
     * which is assigned by unise::Singleton::_instance
     *
     * AST instance @ast will be de-constructed at the end,
     * so does @ast's fields, so does unise::Singleton::
     * _instance's fields, even @ast is set to NULL,
     * unise::Singleton::_instance will not be changed
     *
     * for context of testcase2:
     * AST instance @ast will be got at the very begining,
     * which is assigned by unise::Singleton::_instance, so
     * have the same value as @ast in testcase1, but @ast's
     * fields are not in initialization state as constructed,
     * instead all fields have the same state as it in de-
     * construction at the end of testcase1
     *
     * AST instance @ast will be used, but all its fields are
     * mal-initialized, leading to errors!!!
     *
     * So the solution will be have a global AST for all test
     * cases, which will not be de-constructed at the end of
     * any test case, but REMEMBER to reset AST at the very
     * beginning of each test cases.
     * */
    AST* mAST;
    /*
     * global control center, all test suites SHOULD use this one
     * instead of their own one
     * */
    ControlCenter* mCenter;
};

class TimeScheduleConfigTest : public testing::Test
{
public:
    ywb_status_t AddTimeRanges(vector<TimeRange> ranges);
    void SetTimeScheduleConfig(TimeScheduleConfig& config);
    ywb_bool_t CheckTimeScheduleConfig(
            vector<TimeRange>& timeranges, ywb_uint32_t weekdays);

private:
    TimeScheduleConfig mConfig;
};

class OBJViewTest: public testing::Test
{
public:
    OBJViewTest()
    {
        mObjView = new OBJView();
        YWB_ASSERT(mObjView != NULL);
        /*mCenter = new ControlCenter();
        YWB_ASSERT(mCenter != NULL);*/
    }

    virtual ~OBJViewTest()
    {
        if(mObjView)
        {
            delete mObjView;
            mObjView = NULL;
        }

        /*if(mCenter)
        {
            delete mCenter;
            mCenter = NULL;
        }*/
    }

    OBJView* GetOBJView()
    {
        return mObjView;
    }

private:
    OBJView* mObjView;
    /*just for logger usage*/
    /*ControlCenter* mCenter;*/
};

class DiskViewTest : public testing::Test
{
public:
    DiskViewTest()
    {
        DiskBaseProp diskprop;

        diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
        diskprop.SetDiskRPM(10000);
        diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
        diskprop.SetRaidWidth(1);
        diskprop.SetDiskCap(128000);
        mDiskView = new DiskView(diskprop);
        YWB_ASSERT(mDiskView != NULL);
        /*mCenter = new ControlCenter();
        YWB_ASSERT(mCenter != NULL);*/
    }

    virtual ~DiskViewTest()
    {
        if(mDiskView != NULL)
        {
            delete mDiskView;
            mDiskView = NULL;
        }

        /*if(mCenter)
        {
            delete mCenter;
            mCenter = NULL;
        }*/
    }

    virtual void SetUp();
    virtual void TearDown();

    inline ywb_uint32_t GetOBJCnt()
    {
        return mDiskView->GetOBJCnt();
    }

    ywb_status_t AddOBJView(OBJKey& objkey, OBJView** objview);
    ywb_status_t GetOBJView(OBJKey& objkey, OBJView** objview);
    ywb_status_t DeleteOBJView(OBJKey& objkey);

    DiskView* GetDiskView();

private:
    DiskView* mDiskView;
    /*just for logger usage*/
    /*ControlCenter* mCenter;*/
};

class SubPoolViewTest : public testing::Test
{
public:
    SubPoolViewTest() : mNonEmptyDiskCnt(0), mOBJCnt(0)
    {
        mSubPoolView = new SubPoolView();
        YWB_ASSERT(mSubPoolView != NULL);
        mSubPoolView2 = new SubPoolView();
        YWB_ASSERT(mSubPoolView2 != NULL);
        /*mCenter = new ControlCenter();
        YWB_ASSERT(mCenter != NULL);*/
    }

    virtual ~SubPoolViewTest()
    {
        if(mSubPoolView)
        {
            delete mSubPoolView;
            mSubPoolView = NULL;
        }

        if(mSubPoolView2)
        {
            delete mSubPoolView2;
            mSubPoolView2 = NULL;
        }

        /*if(mCenter)
        {
            delete mCenter;
            mCenter = NULL;
        }*/
    }

    virtual void SetUp();
    virtual void TearDown();

    inline ywb_uint32_t GetDiskCnt()
    {
        return mSubPoolView->GetDiskCnt();
    }

    inline ywb_uint32_t GetNonEmptyDiskCnt()
    {
        return mNonEmptyDiskCnt;
    }

    inline ywb_uint32_t GetOBJCnt()
    {
        return mOBJCnt;
    }

    inline void SetDefaultDiskBaseProp(DiskBaseProp& diskprop)
    {
        diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
        diskprop.SetDiskRPM(10000);
        diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
        diskprop.SetRaidWidth(1);
        diskprop.SetDiskCap(128000);
    }

    ywb_status_t AddDiskView(DiskKey& diskkey);
    ywb_status_t GetDiskView(DiskKey& diskkey, DiskView** diskview);
    ywb_status_t RemoveDiskView(DiskKey& diskkey);

    ywb_status_t AddOBJView(DiskKey& diskkey, OBJKey& objkey);
    ywb_status_t GetOBJView(DiskKey& diskkey,
            OBJKey& objkey, OBJView** objview);
    ywb_status_t DeleteOBJView(DiskKey& diskkey, OBJKey& objkey);

    SubPoolView* GetSubPoolView();
    SubPoolView* GetSubPoolView2();

private:
    ywb_uint32_t mNonEmptyDiskCnt;
    ywb_uint32_t mOBJCnt;
    SubPoolView* mSubPoolView;
    /*for empty subpool test*/
    SubPoolView* mSubPoolView2;
    /*just for logger usage*/
    /*ControlCenter* mCenter;*/
};

class HUBMock : public HUB
{
public:
    HUBMock(ControlCenter* center) : HUB(center)
    {
        SetSelfServiceId(center->GetSelfServiceId());
        SetEnableOrDisableASTSync(ywb_true);
    }

    virtual ~HUBMock() {}

private:
};

class ConfigViewTest : public testing::Test
{
public:
    ConfigViewTest() : mNonEmptySubPoolCnt(0),
    mNonEmptyDiskCnt(0), mDiskCnt(0), mTotalOBJCnt(0),
    mTotalOBJCntOnSubPoolWithASTEnabled(0)
    {
        /*mControlCenter = new ControlCenter();
        YWB_ASSERT(mControlCenter != NULL);*/
        YWB_ASSERT(gCenter != NULL);
        mHUB = new HUBMock(gCenter);
        YWB_ASSERT(mHUB != NULL);
        /*mConfigView = new ConfigView(mHUB);*/
        mHUB->GetConfigView(&mConfigView);
        YWB_ASSERT(mConfigView != NULL);
    }

    virtual ~ConfigViewTest()
    {
        if(mHUB)
        {
            delete mHUB;
            mHUB = NULL;
        }

        /*if(mControlCenter)
        {
            delete mControlCenter;
            mControlCenter = NULL;
        }*/
    }

    ConfigView* GetConfigView()
    {
        return mConfigView;
    }

    ConfigView* GetEmptyConfigView()
    {
        mConfigView->Reset();
        return mConfigView;
    }

    virtual void SetUp();
    virtual void TearDown();

    inline ywb_uint32_t GetOSSID()
    {
        return mHUB->GetSelfServiceId();
    }

    inline ywb_uint32_t GetSubPoolCnt()
    {
        return mConfigView->GetSubPoolCnt();
    }

    inline ywb_uint32_t GetNonEmptySubPoolCnt()
    {
        return mNonEmptySubPoolCnt;
    }

    inline ywb_uint32_t GetNonEmptyDiskCnt()
    {
        return mNonEmptyDiskCnt;
    }

    inline ywb_uint32_t GetDiskCnt()
    {
        return mDiskCnt;
    }

    inline ywb_uint32_t GetTotalOBJCnt()
    {
        return mTotalOBJCnt;
    }

    inline ywb_uint32_t GetTotalOBJCntOnSubPoolWithASTEnabled()
    {
        return mTotalOBJCntOnSubPoolWithASTEnabled;
    }

    inline void SetDefaultDiskBaseProp(DiskBaseProp& diskprop)
    {
        diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
        diskprop.SetDiskRPM(10000);
        diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
        diskprop.SetRaidWidth(1);
        diskprop.SetDiskCap(128000);
    }

    ywb_status_t SetSubPoolASTStatus(ywb_uint32_t subpoolid,
            ywb_uint32_t aststatus);
    ywb_status_t SetSubPoolTimeScheduleConfig(
            ywb_uint32_t subpoolid, TimeScheduleConfig& schedule);
//    ywb_status_t AddSubPoolView(SubPoolKey& subpoolkey);
    ywb_status_t GetSubPoolView(SubPoolKey& subpoolkey,
            SubPoolView** subpoolview);
//    ywb_status_t RemoveSubPoolView(SubPoolKey& subpoolkey);

    ywb_status_t AddDiskView(DiskKey& diskkey, DiskBaseProp& diskprop);
    ywb_status_t GetDiskView(DiskKey& diskkey, DiskView** diskview);
    ywb_status_t RemoveDiskView(DiskKey& diskkey);

    ywb_status_t AddOBJView(OBJKey& objkey);
    ywb_status_t GetOBJView(OBJKey& objkey, OBJView** objinfo);
    ywb_status_t DeleteOBJView(OBJKey& objkey);

private:
    ywb_uint32_t mNonEmptySubPoolCnt;
    ywb_uint32_t mNonEmptyDiskCnt;
    ywb_uint32_t mDiskCnt;
    ywb_uint32_t mTotalOBJCnt;
    /*total number of OBJs on subpools with AST enabled*/
    ywb_uint32_t mTotalOBJCntOnSubPoolWithASTEnabled;
    ConfigView* mConfigView;
    HUBMock* mHUB;
    /*use @gCenter instead*/
    /*ControlCenter* mControlCenter;*/
};

//class MigratorMock : public Migrator
//{
//public:
//    MigratorMock(Migration* migration) :
//        Migrator(migration), mMigratedCnt(0)
//    {
//
//    }
//
//    virtual ~MigratorMock() {}
//
//    inline ywb_uint32_t GetMigratedCnt()
//    {
//        return mMigratedCnt;
//    }
//
//    ywb_status_t Start()
//    {
//        return YWB_SUCCESS;
//    }
//
//    ywb_status_t Stop(ywb_bool_t immediate)
//    {
//        return YWB_SUCCESS;
//    }
//
//    void Submit(Arbitratee* item);
//    ywb_status_t PrepareMigration(Arbitratee* item)
//    {
//        return YWB_SUCCESS;
//    }
//
//    ywb_status_t DoMigration(Arbitratee* item)
//    {
//        return YWB_SUCCESS;
//    }
//
//    ywb_status_t CompleteMigration(Arbitratee* item)
//    {
//        return YWB_SUCCESS;
//    }
//
//    void LaunchCompleteMigrationEvent(OBJKey objkey)
//    {
//        mMigratedCnt++;
//    }
//
//    ywb_status_t StatPerformance(ywb_uint32_t readPerf,
//            ywb_uint32_t writePerf)
//    {
//        return YWB_SUCCESS;
//    }
//
//private:
//    ywb_uint32_t mMigratedCnt;
//};

class HUBMock2 : public HUB
{
public:
    HUBMock2(ControlCenter* center) : HUB(center),
    mRemovedSubPoolCnt(0), mRemovedDiskCnt(0), mDeletedOBJCnt(0),
    mConfigViewCollectedTimes(0), mIOCollectedTimes(0),
    mLock("HUBMockLock")
    {
        SetEnableOrDisableASTSync(ywb_true);
    }

    virtual ~HUBMock2() {}

    inline ywb_uint32_t GetRemovedSubPoolCnt()
    {
        return mRemovedSubPoolCnt;
    }

    inline ywb_uint32_t GetRemovedDiskCnt()
    {
        return mRemovedDiskCnt;
    }

    inline ywb_uint32_t GetRemovedOBJCnt()
    {
        return mDeletedOBJCnt;
    }

    inline ywb_uint32_t GetConfigViewCollectedTimes()
    {
        return mConfigViewCollectedTimes;
    }

    inline ywb_uint32_t GetIOCollectedTimes()
    {
        return mIOCollectedTimes;
    }

    inline OBJKey GetMigratingOBJ()
    {
        return mMigratingOBJ;
    }

    inline OBJKey GetLastMigratedOBJ()
    {
        return mLastMigratedOBJ;
    }

    inline Mutex* GetLock()
    {
        return &mLock;
    }

    inline Cond* GetCond()
    {
        return &mCond;
    }

    void Wait()
    {
        mLock.Lock();
        mCond.Wait(mLock);
        mLock.Unlock();
    }

    void Signal()
    {
        mLock.Lock();
        mCond.Signal();
        mLock.Unlock();
    }

    ywb_status_t AddDisk(DiskKey& diskkey, DiskBaseProp& diskprop);
    ywb_status_t RemoveDisk(DiskKey& diskkey);
    ywb_status_t AddOBJ(OBJKey& objkey);
    ywb_status_t DeleteOBJ(OBJKey& objkey);
    ywb_status_t AccessOBJ(OBJKey& objkey, off_t offset,
            size_t len, ywb_uint32_t ios, ywb_uint32_t bw,
            ywb_uint32_t rt, ywb_bool_t read);
    ywb_status_t Migrate(OBJKey& objkey, DiskKey& tgtdisk);
    ywb_status_t CompleteMigrate(OBJKey& objkey, ywb_status_t status);

    void SendRemovedSubPool();
    void SendRemovedDisk();
    void SendRemovedOBJ();
    void SendConfig();
    void SendIO();

    void ResetAST();

private:
    ywb_uint32_t mRemovedSubPoolCnt;
    ywb_uint32_t mRemovedDiskCnt;
    ywb_uint32_t mDeletedOBJCnt;
    ywb_uint32_t mConfigViewCollectedTimes;
    ywb_uint32_t mIOCollectedTimes;
    OBJKey mMigratingOBJ;
    OBJKey mLastMigratedOBJ;
    Mutex mLock;
    Cond mCond;
};

class HUBTest : public testing::Test
{
public:
    HUBTest() : mRemovedSubPoolCnt(0), mRemovedDiskCnt(0),
    mDeletedOBJCnt(0), mConfigViewCollectedTimes(0),
    mIOCollectedTimes(0), mRawHUB(NULL)
    {
        /*mCenter = new ControlCenterMock();
        YWB_ASSERT(mCenter != NULL);*/
        YWB_ASSERT(gCenter != NULL);
        /*
         * NOTE: @mHUB and @mRawHUB will share the same AST instance,
         * but AST instance has reference count
         * */
        mHUB = new HUBMock2(gCenter);
        YWB_ASSERT(mHUB != NULL);
    }

    virtual ~HUBTest()
    {
        if(mHUB)
        {
            delete mHUB;
            mHUB = NULL;
        }

        /*if(mCenter)
        {
            delete mCenter;
            mCenter = NULL;
        }*/

        mRawHUB = NULL;
    }

    virtual void SetUp();
    virtual void TearDown();

    ywb_uint32_t GetASTStatus();

    ywb_status_t StartHUB();
    ywb_status_t StopHUB(ywb_bool_t immediate);
    ywb_status_t StartAST();
    ywb_status_t StopAST(ywb_bool_t immediate);
    ywb_status_t ResetAST();

    void WaitForHUBSignal();
    /*split the synchronous wait primitive into three phases*/
    void Lock();
    void Wait();
    void Unlock();

    void SetDiskKey(DiskKey& diskkey, ywb_uint64_t diskid);
    void SetDefaultDiskBaseProp(DiskBaseProp& diskprop);
    void SetOBJKey(OBJKey& objkey, ywb_uint64_t diskid, ywb_uint64_t inodeid);

    /*@directly: whether to add disk directly or via event*/
    void AddOneDisk(ywb_uint64_t diskid, ywb_bool_t directly);
    /*@directly: whether to add disk directly or via event*/
    void RemoveOneDisk(ywb_uint64_t diskid, ywb_bool_t directly);
    /*@directly: whether to add disk directly or via event*/
    void AddOneOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid, ywb_bool_t directly);
    /*@directly: whether to add disk directly or via event*/
    void DeleteOneOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid, ywb_bool_t directly);
    /*@directly: whether to add disk directly or via event*/
    void AccessOneOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid,
            off_t offset, size_t len, ywb_uint32_t ios,
            ywb_uint32_t bw, ywb_uint32_t rt, ywb_bool_t read, ywb_bool_t directly);

    void AddDisksDirectly(ywb_uint64_t firstdiskid, ywb_uint32_t disknum);
    void AddOBJsDirectly(ywb_uint64_t diskid,
            ywb_uint64_t firstinodeid, ywb_uint32_t objnum);

    ywb_status_t GetSubPoolKeyFromDiskID(ywb_uint64_t diskid, SubPoolKey& subpoolkey);
    ywb_status_t GetDiskView(ywb_uint64_t diskid, DiskView** diskview);
    ywb_status_t GetOBJView(ywb_uint64_t diskid,
            ywb_uint64_t inodeid, OBJView** objview);

    void PrepareConfigView();
    void CollectConfigView();
    ywb_bool_t CheckConfigView();

    void PrepareIO();
    void CollectIO();
    ywb_bool_t CheckIO();

    void PrepareMigrate();
    void Migrate(ywb_uint64_t srcdiskid, ywb_uint64_t inodeid,
            ywb_uint64_t tgtdiskid);
    ywb_bool_t CheckMigrate();

    void CompleteMigrate(ywb_uint64_t diskid, ywb_uint64_t inodeid);
    ywb_bool_t CheckCompleteMigrate();

    HUBMock2* GetHUBMock2();

private:
    ywb_uint32_t mRemovedSubPoolCnt;
    ywb_uint32_t mRemovedDiskCnt;
    ywb_uint32_t mDeletedOBJCnt;
    ywb_uint32_t mConfigViewCollectedTimes;
    ywb_uint32_t mIOCollectedTimes;
    OBJKey mMigratingOBJ;
    OBJKey mLastMigratedOBJ;

    HUBMock2* mHUB;
    /*raw HUB set internally for ControlCenter*/
    HUB* mRawHUB;
    /*use @gCenter instead*/
    /*ControlCenterMock* mCenter;*/
};

#endif

/* vim:set ts=4 sw=4 expandtab */
