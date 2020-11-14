#include "UnitTest/AST/Message/ASTMessageUnitTests.hpp"

TEST_F(ASTMessageManagerTest, RegisterTargetProxy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    MessageManager* msgmgr = NULL;
    TargetProxy* astproxy = NULL;
    TargetProxy* ossproxy = NULL;

    msgmgr = GetMessageManager();
    ret = msgmgr->GetProxy("ast", &astproxy);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(astproxy != NULL, true);
    ret = msgmgr->GetProxy("oss", &ossproxy);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(ossproxy != NULL, true);
}

TEST_F(ASTMessageManagerTest, DeRegisterTargetProxy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    MessageManager* msgmgr = NULL;
    TargetProxy* astproxy = NULL;

    msgmgr = GetMessageManager();
    ret = msgmgr->DeRegisterTargetProxy("ast");
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = msgmgr->GetProxy("ast", &astproxy);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTMessageManagerTest, StartTargetProxy_SUCCESS)
{
    MessageManager* msgmgr = NULL;
    TargetProxy* astproxy = NULL;

    msgmgr = GetMessageManager();
    msgmgr->GetProxy("ast", &astproxy);
    ASSERT_EQ(astproxy->GetStatus(), TargetProxy::S_running);
}

TEST_F(ASTMessageManagerTest, StopTargetProxy_SUCCESS)
{
    MessageManager* msgmgr = NULL;
    TargetProxy* astproxy = NULL;

    msgmgr = GetMessageManager();
    msgmgr->GetProxy("ast", &astproxy);
    msgmgr->StopTargetProxy("ast");
    ASSERT_EQ(astproxy->GetStatus(), TargetProxy::S_stopped);
}

TEST_F(ASTMessageManagerTest, AddSeqno2MsgEntry_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    MessageManager* msgmgr = NULL;
    Message* msg = NULL;
    Message* msg2 = NULL;

    msgmgr = GetMessageManager();
    msg = new Message("ast", Message::OP_enable_ast);
    YWB_ASSERT(msg != NULL);
    msgmgr->AddSeqno2MsgEntry(101, msg);
    ret = msgmgr->GetMsgBySeqno(101, &msg2);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(msg == msg2, true);
    delete msg;
    msg = NULL;
}

TEST_F(ASTMessageManagerTest, DeleteSeqno2MsgEntry_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    MessageManager* msgmgr = NULL;
    Message* msg = NULL;
    Message* msg2 = NULL;

    msgmgr = GetMessageManager();
    msg = new Message("ast", Message::OP_enable_ast);
    YWB_ASSERT(msg != NULL);
    msgmgr->AddSeqno2MsgEntry(101, msg);
    msgmgr->DeleteSeqno2MsgEntry(101);
    ret = msgmgr->GetMsgBySeqno(101, &msg2);
    ASSERT_EQ(ret, YFS_ENOENT);
    delete msg;
    msg = NULL;
}

TEST_F(CollectLogicalConfigResponseTest, AddDisk_SUCCESS)
{
    CollectLogicalConfigResponse* configresp = NULL;

    configresp = GetConfigResponse();
    ASSERT_EQ(configresp->IsSubPoolEmpty(), false);
}

TEST_F(CollectLogicalConfigResponseTest, AddSubPool_SUCCESS)
{
    CollectLogicalConfigResponse* configresp = NULL;

    configresp = GetConfigResponse();
    ASSERT_EQ(configresp->IsDiskEmpty(), false);
}

TEST_F(CollectLogicalConfigResponseTest, GetFirstNextSubPool_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolnum = 0;
    SubPoolKey subpoolkey;
    CollectLogicalConfigResponse* configresp = NULL;
    ywb_uint32_t subpoolidseqs[5] = {1, 2, 3, 4, 5};

    configresp = GetConfigResponse();
    ret = configresp->GetFirstSubPool(&subpoolkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(subpoolkey.GetSubPoolId(), subpoolidseqs[subpoolnum++]);
        ret = configresp->GetNextSubPool(&subpoolkey);
    }
    ASSERT_EQ(subpoolnum, (ywb_uint32_t)5);
}

TEST_F(CollectLogicalConfigResponseTest, GetFirstNextSubPool_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    CollectLogicalConfigResponse* configresp = NULL;

    configresp = GetConfigResponse2();
    ret = configresp->GetFirstSubPool(&subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(CollectLogicalConfigResponseTest, GetFirstNextDisk__SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t disknum = 0;
    DiskInfo diskinfo;
    CollectLogicalConfigResponse* configresp = NULL;
    DiskKey* diskkey = NULL;
    ywb_uint32_t diskidsseq[(5) * (3)] =
            {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

    configresp = GetConfigResponse();
    ret = configresp->GetFirstDisk(&diskinfo);
    while((YWB_SUCCESS == ret))
    {
        diskinfo.GetDiskKey(&diskkey);
        ASSERT_EQ(diskkey->GetDiskId(), diskidsseq[disknum++]);
        ret = configresp->GetNextDisk(&diskinfo);
    }
    ASSERT_EQ(disknum, (ywb_uint32_t)((5) * (3)));
}

TEST_F(CollectLogicalConfigResponseTest, GetFirstNextDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskInfo diskinfo;
    CollectLogicalConfigResponse* configresp = NULL;

    configresp = GetConfigResponse2();
    ret = configresp->GetFirstDisk(&diskinfo);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(CollectLogicalConfigResponseTest, IsSubPoolEmpty_TRUE)
{
    CollectLogicalConfigResponse* configresp = NULL;

    configresp = GetConfigResponse2();
    ASSERT_EQ(configresp->IsSubPoolEmpty(), true);
}

TEST_F(CollectLogicalConfigResponseTest, IsSubPoolEmpty_FALSE)
{
    CollectLogicalConfigResponse* configresp = NULL;

    configresp = GetConfigResponse();
    ASSERT_EQ(configresp->IsSubPoolEmpty(), false);
}

TEST_F(CollectLogicalConfigResponseTest, IsDiskEmpty_TRUE)
{
    CollectLogicalConfigResponse* configresp = NULL;

    configresp = GetConfigResponse2();
    ASSERT_EQ(configresp->IsDiskEmpty(), true);
}

TEST_F(CollectLogicalConfigResponseTest, IsDiskEmpty_FALSE)
{
    CollectLogicalConfigResponse* configresp = NULL;

    configresp = GetConfigResponse();
    ASSERT_EQ(configresp->IsDiskEmpty(), false);
}

TEST_F(CollectOBJResponseFragmentTest, AddOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJInfo objinfo;
    CollectOBJResponseFragment* objrespfrag = NULL;

    objrespfrag = GetOBJResponseFrag();
    ret = objrespfrag->GetFirstOBJ(&objinfo);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(CollectOBJResponseFragmentTest, GetFirstNextOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objnum = 0;
    OBJInfo objinfo;
    OBJKey* objkey = NULL;
    CollectOBJResponseFragment* objrespfrag = NULL;
    ywb_uint64_t inodeidsseq[6] = {1, 2, 3, 4, 5, 6};

    objrespfrag = GetOBJResponseFrag();
    ret = objrespfrag->GetFirstOBJ(&objinfo);
    while(YWB_SUCCESS == ret)
    {
        objinfo.GetOBJKey(&objkey);
        ASSERT_EQ(objkey->GetInodeId(), inodeidsseq[objnum++]);
        ret = objrespfrag->GetNextOBJ(&objinfo);
    }

    ASSERT_EQ(objnum, (ywb_uint32_t)6);
}

TEST_F(CollectOBJResponseFragmentTest, GetFirstNextOBJ_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJInfo objinfo;
    CollectOBJResponseFragment* objrespfrag = NULL;

    objrespfrag = GetOBJResponseFrag2();
    ret = objrespfrag->GetFirstOBJ(&objinfo);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ReportDeleteOBJMessageTest, AddOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    ReportDeleteOBJsMessage* objdeletionmsg = NULL;

    objdeletionmsg = GetOBJDeletionMsg();
    ret = objdeletionmsg->GetFirstOBJ(&objkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ReportDeleteOBJMessageTest, GetFirstNextOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objnum = 0;
    OBJKey objkey;
    ReportDeleteOBJsMessage* objdeletionmsg = NULL;
    ywb_uint64_t inodeidsseq[6] = {1, 2, 3, 4, 5, 6};

    objdeletionmsg = GetOBJDeletionMsg();
    ret = objdeletionmsg->GetFirstOBJ(&objkey);
    while(YWB_SUCCESS == ret)
    {
        ASSERT_EQ(objkey.GetInodeId(), inodeidsseq[objnum++]);
        ret = objdeletionmsg->GetNextOBJ(&objkey);
    }

    ASSERT_EQ(objnum, (ywb_uint32_t)6);
}

TEST_F(ReportDeleteOBJMessageTest, GetFirstNextOBJ_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    ReportDeleteOBJsMessage* objdeletionmsg = NULL;

    objdeletionmsg = GetOBJDeletionMsg2();
    ret = objdeletionmsg->GetFirstOBJ(&objkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ReportDeleteDiskMessageTest, AddDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    ReportDeleteDisksMessage* diskdeletionmsg = NULL;

    diskdeletionmsg = GetDiskDeletionMsg();
    ret = diskdeletionmsg->GetFirstDisk(&diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ReportDeleteDiskMessageTest, GetFirstNextDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t disknum = 0;
    DiskKey diskkey;
    ReportDeleteDisksMessage* diskdeletionmsg = NULL;
    ywb_uint64_t diskidsseq[3] = {1, 2, 3};

    diskdeletionmsg = GetDiskDeletionMsg();
    ret = diskdeletionmsg->GetFirstDisk(&diskkey);
    while(YWB_SUCCESS == ret)
    {
        ASSERT_EQ(diskkey.GetDiskId(), diskidsseq[disknum++]);
        ret = diskdeletionmsg->GetNextDisk(&diskkey);
    }

    ASSERT_EQ(disknum, (ywb_uint32_t)3);
}

TEST_F(ReportDeleteDiskMessageTest, GetFirstNextDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    ReportDeleteDisksMessage* diskdeletionmsg = NULL;

    diskdeletionmsg = GetDiskDeletionMsg2();
    ret = diskdeletionmsg->GetFirstDisk(&diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ReportDeleteSubPoolMessageTest, AddSubPool_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    ReportDeleteSubPoolsMessage* subpooldeletionmsg = NULL;

    subpooldeletionmsg = GetSubPoolDeletionMsg();
    ret = subpooldeletionmsg->GetFirstSubPool(&subpoolkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ReportDeleteSubPoolMessageTest, GetFirstNextSubPool_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolnum = 0;
    SubPoolKey subpoolkey;
    ReportDeleteSubPoolsMessage* subpooldeletionmsg = NULL;
    ywb_uint64_t subpoolidsseq[5] = {1, 2, 3, 4, 5};

    subpooldeletionmsg = GetSubPoolDeletionMsg();
    ret = subpooldeletionmsg->GetFirstSubPool(&subpoolkey);
    while(YWB_SUCCESS == ret)
    {
        ASSERT_EQ(subpoolkey.GetSubPoolId(), subpoolidsseq[subpoolnum++]);
        ret = subpooldeletionmsg->GetNextSubPool(&subpoolkey);
    }

    ASSERT_EQ(subpoolnum, (ywb_uint32_t)5);
}

TEST_F(ReportDeleteSubPoolMessageTest, GetFirstNextSubPool_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    ReportDeleteSubPoolsMessage* subpooldeletionmsg = NULL;

    subpooldeletionmsg = GetSubPoolDeletionMsg2();
    ret = subpooldeletionmsg->GetFirstSubPool(&subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

/* vim:set ts=4 sw=4 expandtab */
