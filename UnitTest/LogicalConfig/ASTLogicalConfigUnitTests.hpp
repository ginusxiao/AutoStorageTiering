#ifndef __AST_LOGICAL_CONFIG_UNIT_TEST_HPP__
#define __AST_LOGICAL_CONFIG_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTIO.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/MessageImpl.hpp"

class ASTLogicalConfigTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class Workload{
public:
    enum Characteristics{
        /*io*/
        WC_io = 0,
        /*bandwidth*/
        WC_bw,
        /*response time*/
        WC_rt,
        WC_cnt,
    };
};

/*nothing to do at current*/
class OBJTest: public testing::Test
{
public:
    OBJTest() {}
    virtual ~OBJTest() {}

    virtual void SetUp();
    virtual void TearDown();

private:

};

class DiskTest: public testing::Test
{
public:
    DiskTest()
    {
        mDisk = new Disk();
    }

    virtual ~DiskTest()
    {
        if(mDisk)
        {
            delete mDisk;
            mDisk = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    Disk* GetDisk();

    /*
     * @stattype: comes from IOStat::Type
     * @workload: comes from DiskTest::WorkloadCharacteristics
     **/
    ywb_status_t GetStat(IOStat::Type stattype,
            Workload::Characteristics workload,
            ywb_uint32_t* rndread, ywb_uint32_t* seqread,
            ywb_uint32_t* rndwrite, ywb_uint32_t* seqwrite,
            ywb_uint32_t* read, ywb_uint32_t* write,
            ywb_uint32_t* rnd, ywb_uint32_t* seq);
    ywb_bool_t StatMatch(IOStat::Type stattype,
            Workload::Characteristics workload);
    /*
     * @stattype: comes from IOStat::Type
     * @workload: comes from DiskTest::WorkloadCharacteristics
     **/
    ywb_status_t GetRange(IOStat::Type stattype,
            Workload::Characteristics workload,
            ywb_uint32_t* rndmin, ywb_uint32_t* rndmax,
            ywb_uint32_t* rndreadmin, ywb_uint32_t* rndreadmax,
            ywb_uint32_t* rndwritemin, ywb_uint32_t* rndwritemax,
            ywb_uint32_t* seqmin, ywb_uint32_t* seqmax,
            ywb_uint32_t* seqreadmin, ywb_uint32_t* seqreadmax,
            ywb_uint32_t* seqwritemin, ywb_uint32_t* seqwritemax,
            ywb_uint32_t* rndseqmin, ywb_uint32_t* rndseqmax);
    ywb_bool_t RangeMatch(IOStat::Type stattype,
            Workload::Characteristics workload);

private:
    Disk* mDisk;
};

class TierTest: public testing::Test
{
public:
    TierTest()
    {
        mTier = new Tier();
    }

    virtual ~TierTest()
    {
        if(mTier)
        {
            delete mTier;
            mTier = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    Tier* GetTier();
    /*
     * same as SetUp in functionality, but we can have full control
     * of where to use it and where not
     **/
    void SetUpByOurselves();
    ywb_uint32_t AddDisks(ywb_uint32_t subpoolid,
            ywb_uint64_t firstdiskid, ywb_uint32_t disknum);
    /*return number of successfully added OBJs*/
    ywb_uint32_t AddOBJs(ywb_uint32_t subpoolid, ywb_uint64_t diskid,
            ywb_uint64_t firstinodeid, ywb_uint32_t objnum);

private:
    Tier* mTier;
};

class SubPoolTest: public testing::Test
{
public:
    SubPoolTest()
    {
        SubPoolKey subpoolkey;

        subpoolkey.SetOss(Constant::OSS_ID);
        subpoolkey.SetPoolId(Constant::POOL_ID);
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
        mSubPool = new SubPool(subpoolkey);
    }

    SubPoolTest(ywb_uint32_t ossid,
            ywb_uint32_t poolid, ywb_uint32_t subpoolid)
    {
        SubPoolKey subpoolkey;

        subpoolkey.SetOss(ossid);
        subpoolkey.SetPoolId(poolid);
        subpoolkey.SetSubPoolId(subpoolid);
        mSubPool = new SubPool(subpoolkey);
    }

    virtual ~SubPoolTest()
    {
        if(mSubPool)
        {
            delete mSubPool;
            mSubPool = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    SubPool* GetSubPool();
    ywb_status_t AddDisk(ywb_uint32_t diskclass,
            ywb_uint64_t diskid, ywb_uint32_t subpoolid);
    /*return number of successfully added OBJs*/
    ywb_uint32_t AddOBJs(ywb_uint64_t diskid, ywb_uint32_t subpoolid,
            ywb_uint64_t firstinodeid, ywb_uint32_t objnum);
    /*
     * @stattype: comes from IOStat::Type
     * @workload: comes from DiskTest::WorkloadCharacteristics
     **/
    ywb_status_t GetStat(ywb_uint64_t diskid,
            ywb_uint32_t subpoolid, IOStat::Type stattype,
            Workload::Characteristics workload,
            ywb_uint32_t* rndread, ywb_uint32_t* seqread,
            ywb_uint32_t* rndwrite, ywb_uint32_t* seqwrite,
            ywb_uint32_t* read, ywb_uint32_t* write,
            ywb_uint32_t* rnd, ywb_uint32_t* seq);
    ywb_bool_t StatMatch(ywb_uint64_t diskid,
            ywb_uint32_t subpoolid, IOStat::Type stattype,
            Workload::Characteristics workload);

private:
    SubPool* mSubPool;
};

class LogicalConfigTest: public testing::Test
{
public:
    LogicalConfigTest()
    {
        mLogicalConfig = new LogicalConfig(NULL);
        /*NOTE: remember to increase reference against following four*/
        mCollectResp = new CollectLogicalConfigResponse();
        mCollectResp->IncRef();
        mDeletedOBJMsg = new ReportDeleteOBJsMessage();
        mDeletedOBJMsg->IncRef();
        mDeletedDiskMsg = new ReportDeleteDisksMessage();
        mDeletedDiskMsg->IncRef();
        mDeletedSubPoolMsg = new ReportDeleteSubPoolsMessage();
        mDeletedSubPoolMsg->IncRef();
    }

    virtual ~LogicalConfigTest()
    {
        if(mLogicalConfig)
        {
            delete mLogicalConfig;
            mLogicalConfig = NULL;
        }

        if(mCollectResp)
        {
            mCollectResp->DecRef();
        }

        if(mDeletedOBJMsg)
        {
            mDeletedOBJMsg->DecRef();
        }

        if(mDeletedDiskMsg)
        {
            mDeletedDiskMsg->DecRef();
        }

        if(mDeletedSubPoolMsg)
        {
            mDeletedSubPoolMsg->DecRef();
        }
    }

    virtual void SetUp() {};
    virtual void TearDown() {};

    LogicalConfig* GetLogicalConfig();
    ywb_status_t AddSubPool(ywb_uint32_t ossid,
            ywb_uint32_t poolid, ywb_uint32_t subpoolid);
    ywb_uint32_t AddSubPools(ywb_uint32_t ossid, ywb_uint32_t poolid,
            ywb_uint32_t firstsubpoolid, ywb_uint32_t subpoolnum);
    ywb_status_t AddDisk(ywb_uint32_t diskclass,
            ywb_uint64_t diskid, ywb_uint32_t subpoolid);
    ywb_uint32_t AddDisks(ywb_uint32_t diskclass, ywb_uint64_t firstdiskid,
            ywb_uint32_t subpoolid, ywb_uint32_t disknum);
    /*return number of successfully added OBJs*/
    ywb_uint32_t AddOBJs(ywb_uint64_t diskid,
            ywb_uint64_t firstinodeid, ywb_uint32_t objnum);
    /*return success or not*/
    ywb_bool_t AddOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid);

    SubPool* GetSubPool(ywb_uint32_t ossid,
            ywb_uint32_t poolid, ywb_uint32_t subpoolid);
    Disk* GetDisk(ywb_uint64_t diskid, ywb_uint32_t subpoolid);
    OBJ* GetOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid);

    ywb_status_t RemoveSubPool(ywb_uint32_t ossid,
            ywb_uint32_t poolid, ywb_uint32_t subpoolid);
    ywb_status_t RemoveDisk(ywb_uint64_t diskid, ywb_uint32_t subpoolid);
    ywb_status_t RemoveOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid);

    ywb_status_t AddDeletedSubPool(ywb_uint32_t ossid,
            ywb_uint32_t poolid, ywb_uint32_t subpoolid);
    ywb_status_t AddDeletedDisk(ywb_uint64_t diskid, ywb_uint32_t subpoolid);
    ywb_status_t AddDeletedOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid);

    ywb_bool_t CheckSubPoolDeleted(ywb_uint32_t ossid,
            ywb_uint32_t poolid, ywb_uint32_t subpoolid);
    ywb_bool_t CheckDiskDeleted(ywb_uint64_t diskid, ywb_uint32_t subpoolid);
    ywb_bool_t CheckOBJDeleted(ywb_uint64_t diskid, ywb_uint64_t inodeid);

    ywb_bool_t CheckSubPoolExistence(ywb_uint32_t ossid,
            ywb_uint32_t poolid, ywb_uint32_t subpoolid);
    ywb_bool_t CheckDiskExistence(ywb_uint64_t diskid, ywb_uint32_t subpoolid);
    ywb_bool_t CheckOBJExistence(ywb_uint64_t diskid, ywb_uint64_t inodeid);

    /*prepare environment for ResolveLogicalConfig*/
    void PrepareResolveLogicalConfigEnv();
    void BuildCollectLogicalConfigResponse();
    ywb_status_t ResolveLogicalConfig();
    ywb_status_t CheckResolveLogicalConfigResult();

    /*prepare logical configuration for deletion of Subpool/Disk/OBJ*/
    void PrepareLogicalConfigForDeleteMessage();
    void BuildReportDeleteOBJMessage();
    ywb_status_t ResolveDeletedOBJs();
    ywb_status_t CheckResolveDeletedOBJsResult();

    void BuildReportDeleteDiskMessage();
    ywb_status_t ResolveDeletedDisks();
    ywb_status_t CheckResolveDeletedDisksResult();

    void BuildReportDeleteSubPoolMessage();
    ywb_status_t ResolveDeletedSubPools();
    ywb_status_t CheckResolveDeletedSubPoolsResult();

private:
    LogicalConfig* mLogicalConfig;
    CollectLogicalConfigResponse* mCollectResp;
    ReportDeleteOBJsMessage* mDeletedOBJMsg;
    ReportDeleteDisksMessage* mDeletedDiskMsg;
    ReportDeleteSubPoolsMessage* mDeletedSubPoolMsg;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
