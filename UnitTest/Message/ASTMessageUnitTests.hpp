#ifndef __AST_MESSAGE_UNIT_TEST_HPP__
#define __AST_MESSAGE_UNIT_TEST_HPP__

#include "gtest/gtest.h"
#include "OSSCommon/ServiceMonClient.hpp"
#include "AST/ASTConstant.hpp"
#include "AST/MessageExpress.hpp"
#include "AST/MessageImpl.hpp"

/*forward reference*/
class ASTMock;
class OSSMock;
class ASTMessageManagerTest;

extern ASTMock* gAst;
extern OSSMock* gOss;
class ASTMessageTestEnvironment: public testing::Environment
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

class ASTMock : public AST
{
public:
    ASTMock();
    ~ASTMock();

private:

};

class StorageServiceMonClientMock : public StorageServiceMonClient
{
public:
    StorageServiceMonClientMock(OSSMock*);
    ~StorageServiceMonClientMock();

    ywb_status_t Init();
    ywb_status_t initialize();

private:
    OSSMock* mOss;
};

class OSSMock : public ControlCenter
{
public:
    OSSMock();
    ~OSSMock();

private:
    StorageServiceMonClientMock* mClient;
};

class ASTMessageManagerTest: public testing::Test
{
public:
    ASTMessageManagerTest();
    ~ASTMessageManagerTest();

    virtual void SetUp();
    virtual void TearDown();

    MessageManager* GetMessageManager();

private:
//    ASTProxy* mAstProxy;
//    OSSProxy* mOssProxy;
    MessageManager* mAstMsgMgr;
};

class CollectLogicalConfigResponseTest: public testing::Test
{
public:
    CollectLogicalConfigResponseTest()
    {
        mConfigResponse = new CollectLogicalConfigResponse();
        YWB_ASSERT(mConfigResponse != NULL);
        mConfigResponse2 = new CollectLogicalConfigResponse();
        YWB_ASSERT(mConfigResponse2 != NULL);
    }

    ~CollectLogicalConfigResponseTest()
    {
        if(mConfigResponse)
        {
            delete mConfigResponse;
            mConfigResponse = NULL;
        }

        if(mConfigResponse2)
        {
            delete mConfigResponse2;
            mConfigResponse2 = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    CollectLogicalConfigResponse* GetConfigResponse();
    CollectLogicalConfigResponse* GetConfigResponse2();

private:
    CollectLogicalConfigResponse* mConfigResponse;
    /*speically for empty test*/
    CollectLogicalConfigResponse* mConfigResponse2;
};

class CollectOBJResponseFragmentTest: public testing::Test
{
public:
    CollectOBJResponseFragmentTest()
    {
        mOBJResponseFrag = new CollectOBJResponseFragment();
        YWB_ASSERT(mOBJResponseFrag != NULL);
        mOBJResponseFrag2 = new CollectOBJResponseFragment();
        YWB_ASSERT(mOBJResponseFrag2 != NULL);
    }

    ~CollectOBJResponseFragmentTest()
    {
        if(mOBJResponseFrag != NULL)
        {
            delete mOBJResponseFrag;
            mOBJResponseFrag = NULL;
        }

        if(mOBJResponseFrag2 != NULL)
        {
            delete mOBJResponseFrag2;
            mOBJResponseFrag2 = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    CollectOBJResponseFragment* GetOBJResponseFrag();
    CollectOBJResponseFragment* GetOBJResponseFrag2();

private:
    CollectOBJResponseFragment* mOBJResponseFrag;
    /*specially for empty test*/
    CollectOBJResponseFragment* mOBJResponseFrag2;
};

class ReportDeleteOBJMessageTest: public testing::Test
{
public:
    ReportDeleteOBJMessageTest()
    {
        mOBJDeletionMsg = new ReportDeleteOBJsMessage();
        YWB_ASSERT(mOBJDeletionMsg != NULL);
        mOBJDeletionMsg2 = new ReportDeleteOBJsMessage();
        YWB_ASSERT(mOBJDeletionMsg2 != NULL);
    }

    ~ReportDeleteOBJMessageTest()
    {
        if(mOBJDeletionMsg)
        {
            delete mOBJDeletionMsg;
            mOBJDeletionMsg = NULL;
        }

        if(mOBJDeletionMsg2)
        {
            delete mOBJDeletionMsg2;
            mOBJDeletionMsg2 = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    ReportDeleteOBJsMessage* GetOBJDeletionMsg();
    ReportDeleteOBJsMessage* GetOBJDeletionMsg2();

private:
    ReportDeleteOBJsMessage* mOBJDeletionMsg;
    /*specially for empty test*/
    ReportDeleteOBJsMessage* mOBJDeletionMsg2;
};

class ReportDeleteDiskMessageTest: public testing::Test
{
public:
    ReportDeleteDiskMessageTest()
    {
        mDiskDeletionMsg = new ReportDeleteDisksMessage();
        YWB_ASSERT(mDiskDeletionMsg != NULL);
        mDiskDeletionMsg2 = new ReportDeleteDisksMessage();
        YWB_ASSERT(mDiskDeletionMsg2 != NULL);
    }

    ~ReportDeleteDiskMessageTest()
    {
        if(mDiskDeletionMsg)
        {
            delete mDiskDeletionMsg;
            mDiskDeletionMsg = NULL;
        }

        if(mDiskDeletionMsg2)
        {
            delete mDiskDeletionMsg2;
            mDiskDeletionMsg2 = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    ReportDeleteDisksMessage* GetDiskDeletionMsg();
    ReportDeleteDisksMessage* GetDiskDeletionMsg2();

private:
    ReportDeleteDisksMessage* mDiskDeletionMsg;
    /*specially for empty test*/
    ReportDeleteDisksMessage* mDiskDeletionMsg2;
};

class ReportDeleteSubPoolMessageTest: public testing::Test
{
public:
    ReportDeleteSubPoolMessageTest()
    {
        mSubPoolDeletionMsg = new ReportDeleteSubPoolsMessage();
        YWB_ASSERT(mSubPoolDeletionMsg != NULL);
        mSubPoolDeletionMsg2 = new ReportDeleteSubPoolsMessage();
        YWB_ASSERT(mSubPoolDeletionMsg2 != NULL);
    }

    ~ReportDeleteSubPoolMessageTest()
    {
        if(mSubPoolDeletionMsg)
        {
            delete mSubPoolDeletionMsg;
            mSubPoolDeletionMsg = NULL;
        }

        if(mSubPoolDeletionMsg2)
        {
            delete mSubPoolDeletionMsg2;
            mSubPoolDeletionMsg2 = NULL;
        }
    }

    virtual void SetUp();
    virtual void TearDown();

    ReportDeleteSubPoolsMessage* GetSubPoolDeletionMsg();
    ReportDeleteSubPoolsMessage* GetSubPoolDeletionMsg2();

private:
    ReportDeleteSubPoolsMessage* mSubPoolDeletionMsg;
    /*specially for empty test*/
    ReportDeleteSubPoolsMessage* mSubPoolDeletionMsg2;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
