#ifndef __AST_IO_UNIT_TEST_HPP__
#define __AST_IO_UNIT_TEST_HPP__

#include <stdio.h>
#include "gtest/gtest.h"
#include "AST/ASTIO.hpp"
#include "AST/AST.hpp"
#include "AST/ASTTime.hpp"
#include "AST/MessageImpl.hpp"


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

class ASTIOTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class ASTOBJIOTest : public testing::Test
{
public:
    ASTOBJIOTest()
    {
        mObjIO = new OBJIO();
    }

    virtual ~ASTOBJIOTest()
    {
        if(mObjIO)
        {
            delete mObjIO;
            mObjIO = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    OBJIO* GetOBJIO();

private:
    OBJIO* mObjIO;
};

class ASTDiskIOTest : public testing::Test
{
public:
    ASTDiskIOTest()
    {
        mDiskIO = new DiskIO();
    }

    virtual ~ASTDiskIOTest()
    {
        ywb_uint32_t loop = 0;

        if(mDiskIO)
        {
            delete mDiskIO;
            mDiskIO = NULL;
        }

        for(loop = 0; loop < 6; loop++)
        {
            if(mObjIO[loop])
            {
                delete mObjIO[loop];
                mObjIO[loop] = NULL;
            }
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    DiskIO* GetDiskIO();
    OBJIO* GetOBJIO(ywb_uint32_t objindex);

private:
    OBJIO* mObjIO[6];
    DiskIO* mDiskIO;
};

class ASTIOManagerTest : public testing::Test
{
public:
    ASTIOManagerTest()
    {
        mAst = new AST();
        mCollectOBJResp = new CollectOBJResponse();
        /*NOTE: remember to increase reference against @mCollectOBJResp*/
        mCollectOBJResp->IncRef();
        mCollectOBJRespFrag[0] = new CollectOBJResponseFragment();
        mCollectOBJRespFrag[1] = new CollectOBJResponseFragment();
    }

    virtual ~ASTIOManagerTest()
    {
        if(mAst)
        {
            delete mAst;
            mAst = NULL;
        }

        if(mCollectOBJRespFrag[0] != NULL)
        {
            delete mCollectOBJRespFrag[0];
            mCollectOBJRespFrag[0] = NULL;
        }

        if(mCollectOBJRespFrag[1] != NULL)
        {
            delete mCollectOBJRespFrag[1];
            mCollectOBJRespFrag[1] = NULL;
        }

        if(mCollectOBJResp)
        {
            mCollectOBJResp->DecRef();
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    LogicalConfig* GetLogicalConfig();
    IOManager* GetIOManager();
    DecisionWindowManager* GetDecisionWindowManager(ywb_uint32_t windowindex);

    ywb_status_t GetStat(DiskIO* diskio, IOStat::Type stattype,
            Workload::Characteristics workload,
            ywb_uint32_t* rndread, ywb_uint32_t* seqread,
            ywb_uint32_t* rndwrite, ywb_uint32_t* seqwrite,
            ywb_uint32_t* read, ywb_uint32_t* write,
            ywb_uint32_t* rnd, ywb_uint32_t* seq);

    ywb_bool_t StatMatch(DiskIO* diskio, IOStat::Type stattype,
            Workload::Characteristics workload);

    ywb_status_t AddOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid);
    ywb_status_t AddDisk(ywb_uint32_t diskclass,
            ywb_uint64_t diskid, ywb_uint32_t subpoolid);

    ywb_status_t AddOBJIO(ywb_uint64_t diskid, ywb_uint64_t inodeid);
    ywb_status_t AddDiskIO(ywb_uint64_t diskid, ywb_uint32_t subpoolid);

    /*prepare environment for ResolveOBJIO*/
    void PrepareResolveOBJIOEnv();
    void BuildCollectOBJResponse();
    void BuildCollectOBJResponse2();
    ywb_status_t ResolveOBJIO();
    ywb_status_t CheckResolveOBJIOResult();

private:
    AST* mAst;
    CollectOBJResponse* mCollectOBJResp;
    /*
     * for simplicity, only two OBJ IO response fragments is
     * used which will accommodate all OBJs
     * */
    CollectOBJResponseFragment* mCollectOBJRespFrag[2];
};

#endif

/* vim:set ts=4 sw=4 expandtab */

