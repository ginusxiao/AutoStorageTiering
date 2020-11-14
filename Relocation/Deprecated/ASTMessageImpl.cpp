#include <vector>
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "AST/Deprecated/ASTMessageExpress.hpp"
#include "AST/Deprecated/ASTMessageImpl.hpp"

void CollectLogicalConfigResponse::AddDisk(DiskInfo& disk)
{
    mDisks.push_back(disk);
}

void CollectLogicalConfigResponse::AddSubPool(SubPoolKey& subpool)
{
    mSubPools.push_back(subpool);
}

ywb_status_t
CollectLogicalConfigResponse::GetFirstSubPool(SubPoolKey* subpool)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mSubPools.empty())
    {
        mSubPoolIter = mSubPools.begin();
        *subpool = *mSubPoolIter;
        mSubPoolIter++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
CollectLogicalConfigResponse::GetNextSubPool(SubPoolKey* subpool)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mSubPoolIter != mSubPools.end())
    {
        *subpool = *mSubPoolIter;
        mSubPoolIter++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
CollectLogicalConfigResponse::GetFirstDisk(DiskInfo* disk)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mDisks.empty())
    {
        mDiskIter = mDisks.begin();
        *disk = *mDiskIter;
        mDiskIter++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
CollectLogicalConfigResponse::GetNextDisk(DiskInfo* disk)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mDiskIter != mDisks.end())
    {
        *disk = *mDiskIter;
        mDiskIter++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

bool CollectLogicalConfigResponse::IsSubPoolEmpty()
{
    return mSubPools.empty();
}

bool CollectLogicalConfigResponse::IsDiskEmpty()
{
    return mDisks.empty();
}

void CollectLogicalConfigResponse::Reset()
{
    mSubPools.clear();
    mDisks.clear();
}

void CollectOBJResponse::AddOBJ(OBJInfo& obj)
{
    mOBJs.push_back(obj);
}

ywb_status_t
CollectOBJResponse::GetFirstOBJ(OBJInfo* obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mOBJs.empty())
    {
        mOBJIter = mOBJs.begin();
        *obj = *mOBJIter;
        mOBJIter++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
CollectOBJResponse::GetNextOBJ(OBJInfo* obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mOBJIter != mOBJs.end())
    {
        *obj = *mOBJIter;
        mOBJIter++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

void CollectOBJResponse::Reset()
{
    mOBJs.clear();
}

void ReportDeleteOBJsMessage::AddOBJ(OBJKey& key)
{
    mObjs.push_back(key);
}

ywb_status_t ReportDeleteOBJsMessage::GetFirstOBJ(OBJKey* obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    mIt = mObjs.begin();
    if(mIt != mObjs.end())
    {
        *obj = *mIt;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ReportDeleteOBJsMessage::GetNextOBJ(OBJKey* obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIt != mObjs.end())
    {
        *obj = *mIt;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

void ReportDeleteOBJsMessage::Reset()
{
    mObjs.clear();
}

void ReportDeleteDisksMessage::AddDisk(DiskKey& key)
{
    mDisks.push_back(key);
}

ywb_status_t ReportDeleteDisksMessage::GetFirstDisk(DiskKey* obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    mIt = mDisks.begin();
    if(mIt != mDisks.end())
    {
        *obj = *mIt;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ReportDeleteDisksMessage::GetNextDisk(DiskKey* obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIt != mDisks.end())
    {
        *obj = *mIt;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

void ReportDeleteDisksMessage::Reset()
{
    mDisks.clear();
}

void ReportDeleteSubPoolsMessage::AddSubPool(SubPoolKey& key)
{
    mSubPools.push_back(key);
}

ywb_status_t ReportDeleteSubPoolsMessage::GetFirstSubPool(SubPoolKey* obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    mIt = mSubPools.begin();
    if(mIt != mSubPools.end())
    {
        *obj = *mIt;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ReportDeleteSubPoolsMessage::GetNextSubPool(SubPoolKey* obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIt != mSubPools.end())
    {
        *obj = *mIt;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

void ReportDeleteSubPoolsMessage::Reset()
{
    mSubPools.clear();
}

ASTProxy::ASTProxy(AST* ast)
{
    YWB_ASSERT(ast != NULL);
    mAst = ast;
    mAst->GetLogicalConfig(&mConfig);
    mAst->GetIOManager(&mIO);
}

ASTProxy::~ASTProxy()
{
    mAst = NULL;
    mConfig = NULL;
    mIO = NULL;
}

ywb_status_t
ASTProxy::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    SetStatus(TargetProxy::S_running);
    return ret;
}

ywb_status_t
ASTProxy::Stop()
{
    ywb_status_t ret = YWB_SUCCESS;

    SetStatus(TargetProxy::S_stopped);
    return ret;
}

ywb_status_t
ASTProxy::Handle(Message* msg)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Generator* gen = NULL;
    ASTMessageManager* manager = NULL;
    LogicalConfig* config = NULL;
    IOManager* io = NULL;
    Message* reqmsg = NULL;
    ywb_uint32_t seqno = 0;

    GetAST(&ast);
    ast->GetGenerator(&gen);

    switch(msg->GetOpCode())
    {
        case Message::OP_enable_ast:
        {
            ast->EnableAST();
            msg->DecRef();
            break;
        }
        case Message::OP_disable_ast:
        {
            ast->DisableAST();
            msg->DecRef();
            break;
        }
        case Message::OP_collect_logical_config_resp:
        {
            CollectLogicalConfigResponse* respmsg = NULL;

            GetLogicalConfig(&config);
            GetAstMsgMgr(&manager);
            respmsg = (CollectLogicalConfigResponse*)msg;
            seqno = respmsg->GetSeqNo();
            manager->GetMsgBySeqno(seqno, &reqmsg);
            manager->DeleteSeqno2MsgEntry(seqno);

//            config->SetCollectLogicalConfigResponse(respmsg);
//            /*
//             * remember to wakeup before launching resolving logical config
//             * event, otherwise the generator may be blocked for waiting
//             * incoming event. Image the following scenario:
//             * (1) Generator is waiting on synchronization context;
//             * (2) Launch resolve config event, which will not wake up generator;
//             * (3) Call WakeupSyncContext which will wake up generator;
//             * (4) Generator waiting for incoming event, but the resolve config
//             *     event already been launched before, so generator thread
//             *     is blocked!
//             **/
//            gen->WakeupSyncContext();
            gen->LaunchResolveConfigEvent(respmsg);
            break;
        }
        case Message::OP_collect_OBJ_resp:
        {
            CollectOBJResponse* respmsg = NULL;

            GetIOManager(&io);
            GetAstMsgMgr(&manager);
            respmsg = (CollectOBJResponse*)msg;
            seqno = respmsg->GetSeqNo();
            manager->GetMsgBySeqno(seqno, &reqmsg);
            manager->DeleteSeqno2MsgEntry(seqno);

//            io->SetCollectOBJResponse(respmsg);
//            gen->WakeupSyncContext();
            gen->LaunchResolveOBJIOEvent(respmsg);
            break;
        }
        case Message::OP_report_delete_OBJs:
        {
            ReportDeleteOBJsMessage* deleteobjmsg = NULL;

            GetLogicalConfig(&config);
            deleteobjmsg = (ReportDeleteOBJsMessage*)msg;
//            config->SetDeleteOBJMessage(deleteobjmsg);
            gen->LaunchResolveDeletedOBJEvent(deleteobjmsg);
            break;
        }
        case Message::OP_report_delete_disks:
        {
            ReportDeleteDisksMessage* deletediskmsg = NULL;

            GetLogicalConfig(&config);
            deletediskmsg = (ReportDeleteDisksMessage*)msg;
//            config->SetDeleteDiskMessage(deletediskmsg);
            gen->LaunchResolveDeletedDiskEvent(deletediskmsg);
            break;
        }
        case Message::OP_report_delete_subpools:
        {
            ReportDeleteSubPoolsMessage* deletesubpoolmsg = NULL;

            GetLogicalConfig(&config);
            deletesubpoolmsg = (ReportDeleteSubPoolsMessage*)msg;
//            config->SetDeleteSubPoolMessage(deletesubpoolmsg);
            gen->LaunchResolveDeletedSubPoolEvent(deletesubpoolmsg);
            break;
        }
        default:
            YWB_ASSERT(0);
            break;
    }

    return ret;
}

OSSProxy::OSSProxy(ControlCenter* center) : mCenter(center)
{
    mMonClnt = center->GetMonClient();
}

OSSProxy::~OSSProxy()
{
    mCenter = NULL;
    mMonClnt = NULL;
}

ywb_status_t OSSProxy::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    SetStatus(TargetProxy::S_running);
    return ret;
}

ywb_status_t OSSProxy::Stop()
{
    ywb_status_t ret = YWB_SUCCESS;

    SetStatus(TargetProxy::S_stopped);
    return ret;
}

ywb_status_t OSSProxy::Handle(Message* msg)
{
    ywb_status_t ret = YWB_SUCCESS;
//    StorageServiceMonClient* monclient = NULL;
    ControlCenter* center = NULL;
    ASTMessageManager* messagemanager = NULL;
    ywb_uint32_t seqno = 0;

//    GetMonClnt(&monclient);
//    YWB_ASSERT(NULL != monclient);
    GetControlCenter(&center);
    YWB_ASSERT(NULL != center);
    GetAstMsgMgr(&messagemanager);
    YWB_ASSERT(NULL != messagemanager);

    switch(msg->GetOpCode())
    {
        case Message::OP_collect_logical_config_req:
        {
            CollectLogicalConfigRequest* configMsg = NULL;

            configMsg = (CollectLogicalConfigRequest*)msg;
            seqno = messagemanager->AllocSeqno();
            messagemanager->AddSeqno2MsgEntry(seqno, msg);
            msg->DecRef();
//            ret = monclient->RetrieveLogicalConfig(seqno, configMsg->GetOss());
            ret = center->RetrieveOBJ(seqno, configMsg->GetOss());
            break;
        }
        case Message::OP_collect_OBJ_req:
        {
            CollectOBJRequest* objmsg = NULL;

            objmsg = (CollectOBJRequest*)msg;
            seqno = messagemanager->AllocSeqno();
            messagemanager->AddSeqno2MsgEntry(seqno, msg);
            msg->DecRef();
            ret = center->RetrieveOBJ(seqno, objmsg->GetOss());

            break;
        }
        default:
            YWB_ASSERT(0);
            break;
    }

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
