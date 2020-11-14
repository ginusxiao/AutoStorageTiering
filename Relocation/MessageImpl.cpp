#include <vector>
#include "OSSCommon/ServiceMonClient.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "AST/AST.hpp"
#include "AST/ASTHUB.hpp"
#include "AST/MessageImpl.hpp"

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

void CollectOBJResponseFragment::AddOBJ(OBJInfo& obj)
{
    mOBJs.push_back(obj);
}

ywb_status_t
CollectOBJResponseFragment::GetFirstOBJ(OBJInfo* obj)
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
CollectOBJResponseFragment::GetNextOBJ(OBJInfo* obj)
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

void CollectOBJResponseFragment::Reset()
{
    mOBJs.clear();
    mStatus = YFS_ENODATA;
}

void CollectOBJResponse::Reset()
{
    mFlag = CollectOBJResponse::F_max;
    mStatus = YFS_ENODATA;
    mSeqno = YWB_UINT32_MAX;
    mFragmentsNum = 0;
    mFragments = NULL;
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

ywb_status_t NotifyRelocationType::AddRelocationType(
        SubPoolKey& subpoolkey, ywb_int32_t val)
{
    std::pair<map<SubPoolKey, ywb_int32_t, SubPoolKeyCmp>::iterator, bool> pairret;

    pairret = mRelocationTypes.insert(std::make_pair(subpoolkey, val));
    if(false == pairret.second)
    {
        return YFS_EEXIST;
    }

    return YWB_SUCCESS;
}

ywb_status_t NotifyRelocationType::Assign(map<SubPoolKey,
        ywb_int32_t, SubPoolKeyCmp>* relocationtypes)
{
    if(relocationtypes != NULL)
    {
        mRelocationTypes.insert(relocationtypes->begin(), relocationtypes->end());
    }

    return YWB_SUCCESS;
}

ywb_status_t NotifyRelocationType::GetFirstRelocationType(
        SubPoolKey& subpoolkey, ywb_int32_t& val)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mRelocationTypes.empty())
    {
        mIt = mRelocationTypes.begin();
        subpoolkey = mIt->first;
        val = mIt->second;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t NotifyRelocationType::GetNextRelocationType(
        SubPoolKey& subpoolkey, ywb_int32_t& val)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIt != mRelocationTypes.end())
    {
        subpoolkey = mIt->first;
        val = mIt->second;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_bool_t NotifyRelocationType::Empty()
{
    return mRelocationTypes.empty();
}

void NotifyRelocationType::Reset()
{
    mRelocationTypes.clear();
}

ywb_status_t NotifySwapRequirement::AddRequireSwapDisk(
        DiskKey& diskkey, ywb_int32_t val)
{
    std::pair<map<DiskKey, ywb_int32_t, DiskKeyCmp>::iterator, bool> pairret;

    pairret = mSwapRequiredDisks.insert(std::make_pair(diskkey, val));
    if(false == pairret.second)
    {
        return YFS_EEXIST;
    }

    return YWB_SUCCESS;
}

ywb_status_t NotifySwapRequirement::Assign(map<DiskKey,
        ywb_int32_t, DiskKeyCmp>* disks)
{
    if(disks != NULL)
    {
        mSwapRequiredDisks.insert(disks->begin(), disks->end());
    }

    return YWB_SUCCESS;
}

ywb_status_t NotifySwapRequirement::GetFirstSwapDisk(
        DiskKey& diskkey, ywb_int32_t& val)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mSwapRequiredDisks.empty())
    {
        mIt = mSwapRequiredDisks.begin();
        diskkey = mIt->first;
        val = mIt->second;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t NotifySwapRequirement::GetNextSwapDisk(
        DiskKey& diskkey, ywb_int32_t& val)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIt != mSwapRequiredDisks.end())
    {
        diskkey = mIt->first;
        val = mIt->second;
        mIt++;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_bool_t NotifySwapRequirement::Empty()
{
    return mSwapRequiredDisks.empty();
}

void NotifySwapRequirement::Reset()
{
    mSwapRequiredDisks.clear();
}

ywb_status_t NotifySwapCancellation::AddCancelSwapDisk(DiskKey& diskkey)
{
    mSwapCancelledDisks.push_back(diskkey);
    return YWB_SUCCESS;
}

ywb_status_t NotifySwapCancellation::Assign(list<DiskKey>* disks)
{
    if(disks != NULL)
    {
        mSwapCancelledDisks.assign(disks->begin(), disks->end());
    }

    return YWB_SUCCESS;
}

ywb_status_t NotifySwapCancellation::GetSwapCancelledDisks(list<DiskKey>* disks)
{
    YWB_ASSERT(disks != NULL);
    disks->assign(mSwapCancelledDisks.begin(), mSwapCancelledDisks.end());

    return YWB_SUCCESS;
}

ywb_bool_t NotifySwapCancellation::Empty()
{
    return mSwapCancelledDisks.empty();
}

void NotifySwapCancellation::Reset()
{
    mSwapCancelledDisks.clear();
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
    MessageManager* manager = NULL;
    LogicalConfig* config = NULL;
    IOManager* io = NULL;
    Message* reqmsg = NULL;
    ywb_uint32_t seqno = 0;

    GetAST(&ast);
    ast->GetGenerator(&gen);

    ast_log_debug("handle message@" << msg
            << " against AST, opcode: " << msg->GetOpCode());
    switch(msg->GetOpCode())
    {
        case Message::OP_enable_ast:
        {
            ast->HandleEnableAST();
            msg->DecRef();
            break;
        }
        case Message::OP_disable_ast:
        {
            ast->HandleDisableAST();
            msg->DecRef();
            break;
        }
        case Message::OP_collect_logical_config_resp:
        {
            CollectLogicalConfigResponse* respmsg = NULL;

            GetLogicalConfig(&config);
            GetMsgMgr(&manager);
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
            GetMsgMgr(&manager);
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
            gen->LaunchResolveDeletedOBJsEvent(deleteobjmsg);
            break;
        }
        case Message::OP_report_delete_disks:
        {
            ReportDeleteDisksMessage* deletediskmsg = NULL;

            GetLogicalConfig(&config);
            deletediskmsg = (ReportDeleteDisksMessage*)msg;
//            config->SetDeleteDiskMessage(deletediskmsg);
            gen->LaunchResolveDeletedDisksEvent(deletediskmsg);
            break;
        }
        case Message::OP_report_delete_subpools:
        {
            ReportDeleteSubPoolsMessage* deletesubpoolmsg = NULL;

            GetLogicalConfig(&config);
            deletesubpoolmsg = (ReportDeleteSubPoolsMessage*)msg;
//            config->SetDeleteSubPoolMessage(deletesubpoolmsg);
            gen->LaunchResolveDeletedSubPoolsEvent(deletesubpoolmsg);
            break;
        }
        case Message::OP_report_delete_OBJ:
        {
            ReportDeleteOBJMessage* deleteobjmsg = NULL;

            GetLogicalConfig(&config);
            deleteobjmsg = (ReportDeleteOBJMessage*)msg;
            gen->LaunchResolveDeletedOBJEvent(deleteobjmsg);
            break;
        }
        case Message::OP_report_delete_disk:
        {
            ReportDeleteDiskMessage* deletediskmsg = NULL;

            GetLogicalConfig(&config);
            deletediskmsg = (ReportDeleteDiskMessage*)msg;
            gen->LaunchResolveDeletedDiskEvent(deletediskmsg);
            break;
        }
        case Message::OP_report_delete_subpool:
        {
            ReportDeleteSubPoolMessage* deletesubpoolmsg = NULL;

            GetLogicalConfig(&config);
            deletesubpoolmsg = (ReportDeleteSubPoolMessage*)msg;
            gen->LaunchResolveDeletedSubPoolEvent(deletesubpoolmsg);
            break;
        }
#ifdef WITH_INITIAL_PLACEMENT
        case Message::OP_notify_relocation_type:
        {
            NotifyRelocationType* relocationtypes = NULL;

            relocationtypes = (NotifyRelocationType*)msg;
            gen->LaunchResolveRelocationType(relocationtypes);
            break;
        }
        case Message::OP_notify_swap_requirement:
        {
            NotifySwapRequirement* swaprequirement = NULL;

            swaprequirement = (NotifySwapRequirement*)msg;
            gen->LaunchResolveSwapRequirementEvent(swaprequirement);
            break;
        }
        case Message::OP_notify_swap_cancellation:
        {
            NotifySwapCancellation* swapcancellation = NULL;

            swapcancellation = (NotifySwapCancellation*)msg;
            gen->LaunchResolveSwapCancellationEvent(swapcancellation);
            break;
        }
#endif
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
    MessageManager* messagemanager = NULL;
    ywb_uint32_t seqno = 0;

//    GetMonClnt(&monclient);
//    YWB_ASSERT(NULL != monclient);
    GetControlCenter(&center);
    YWB_ASSERT(NULL != center);
    GetMsgMgr(&messagemanager);
    YWB_ASSERT(NULL != messagemanager);

    ast_log_debug("handle message@" << msg
            << " against OSS, opcode: " << msg->GetOpCode());
    switch(msg->GetOpCode())
    {
    case Message::OP_ast_enabled:
    {
        HUB* hub = NULL;

        hub = center->GetHUB();
        YWB_ASSERT(hub != NULL);
        hub->NotifyEnableASTResult(msg->GetStatus());
        msg->DecRef();
        break;
    }
    case Message::OP_ast_disabled:
    {
        HUB* hub = NULL;

        hub = center->GetHUB();
        YWB_ASSERT(hub != NULL);
        hub->NotifyDisableASTResult(msg->GetStatus());
        msg->DecRef();
        break;
    }
    case Message::OP_collect_logical_config_req:
    {
        CollectLogicalConfigRequest* configMsg = NULL;

        configMsg = (CollectLogicalConfigRequest*)msg;
        seqno = messagemanager->AllocSeqno();
        messagemanager->AddSeqno2MsgEntry(seqno, msg);
        msg->DecRef();
//            ret = monclient->RetrieveLogicalConfig(seqno, configMsg->GetOss());
        ret = center->RetrieveLogicalConfig(seqno, configMsg->GetOss());
        break;
    }
    case Message::OP_collect_OBJ_req:
    {
        CollectOBJRequest* objmsg = NULL;

        objmsg = (CollectOBJRequest*)msg;
        seqno = messagemanager->AllocSeqno();
        msg->DecRef();
        if(CollectOBJRequest::F_prepare == objmsg->GetFlag())
        {
            ret = center->RetrieveOBJPrep(seqno, objmsg->GetOss());
        }
        else if(CollectOBJRequest::F_normal == objmsg->GetFlag())
        {
            messagemanager->AddSeqno2MsgEntry(seqno, msg);
            ret = center->RetrieveOBJ(seqno, objmsg->GetOss());
        }
        else if(CollectOBJRequest::F_post == objmsg->GetFlag())
        {
            ret = center->RetrieveOBJPost(seqno, objmsg->GetOss());
        }
        else
        {
            YWB_ASSERT(0);
        }

        break;
    }
    default:
        YWB_ASSERT(0);
        break;
    }

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
