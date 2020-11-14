#include "OSSCommon/Logger.hpp"
#include "AST/MessageImpl.hpp"

//Message::Message(InitiatorType initiator, TargetType target, OpCode op, ywb_bool_t sync)
Message::Message(TargetType target, OpCode op) :
        mTarget(target), mOpCode(op), mIsSync(false),
        mStatus(YWB_SUCCESS), mRef(1) {}

Message::~Message() {}

ywb_int32_t
Message::IncRef()
{
    return fs_atomic_inc(&mRef);
}

ywb_int32_t
Message::DecRef()
{
    ywb_uint32_t ret = 0;

    ret = fs_atomic_dec(&mRef);
    if(ret == 0)
    {
        delete this;
    }

    return ret;
}

////InitiatorSyncContext::InitiatorSyncContext(ASTMessageManager* mgr)
//InitiatorSyncContext::InitiatorSyncContext()
//{
//    pthread_cond_init(&mCond, NULL);
//    pthread_mutex_init(&mLock, NULL);
////    mAstMsgMgr = mgr;
//}

TargetProxy::TargetProxy()
{
    mStatus = TargetProxy::S_stopped;
    mMsgMgr = NULL;
}

TargetProxy::~TargetProxy()
{
    mMsgMgr = NULL;
}

ywb_status_t
TargetProxy::Start()
{
    return YWB_SUCCESS;
}

ywb_status_t
TargetProxy::Stop()
{
    return YWB_SUCCESS;
}

ywb_status_t
TargetProxy::Handle(Message*)
{
    return YWB_SUCCESS;
}

MessageManager::MessageManager(AST* ast, ControlCenter* center) :
        mStatus(MessageManager::S_stopped), mAST(ast),
        mCtrlCenter(center), mMutex("MessageManagerLock")
{
    mStopMsg = new Message("msgmgr", Message::OP_stop);
    YWB_ASSERT(mStopMsg != NULL);
}

MessageManager::~MessageManager()
{
    Message* msg = NULL;

    YWB_ASSERT(MessageManager::S_stopped == mStatus);

    /*user is responsible for garbaging the target proxy*/
    mProxy.clear();
    mSeqno2Msg.clear();

    while((msg = mMsgQueue.DequeueNoWait()) != NULL)
    {
        delete msg;
        msg = NULL;
    }

    if(mStopMsg != NULL)
    {
        delete mStopMsg;
        mStopMsg = NULL;
    }

    mAST = NULL;
    mCtrlCenter = NULL;
}

/*
 * target proxy will not be started automatically here, for the message
 * manager is designed to be pluggable which means new target proxy may
 * be plugged in after starting the message manager
 **/
//void*
//ASTMessageManager::Entry(void* arg)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    ASTMessageManager* manager = (ASTMessageManager*)arg;
//    pthread_mutex_t* statuslock = NULL;
//    pthread_cond_t* statuscond = NULL;
//
//    manager->GetStatusLock(&statuslock);
//    manager->GetStatusCond(&statuscond);
//
//    ast_log_trace("Entering 1----------\n");
//    pthread_mutex_lock(statuslock);
//    ast_log_trace("Entering 2----------\n");
//    manager->mStatus = ASTMessageManager::S_starting;
//    ret = manager->start();
//    if(YWB_SUCCESS != ret)
//    {
//        manager->mStatus = ASTMessageManager::S_failed;
//    }
//    else
//    {
//        manager->mStatus = ASTMessageManager::S_running;
//    }
//
//    /*notify the calling thread about the executing status*/
//    pthread_cond_signal(statuscond);
//    ast_log_trace("Entering 3----------\n");
//    pthread_mutex_unlock(statuslock);
//    ast_log_trace("Entering 4----------\n");
//
//    return NULL;
//}

void* MessageManager::entry()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t exit = ywb_false;

    mMutex.Lock();
    mStatus = MessageManager::S_starting;

    if(YWB_SUCCESS != ret)
    {
        mStatus = MessageManager::S_failed;
        exit = ywb_true;
    }
    else
    {
        mStatus = MessageManager::S_running;
    }

    /*notify the calling thread about the executing status*/
    mCond.Signal();
    ast_log_debug("Notify MessageManager start status");
    mMutex.Unlock();

    if(ywb_false == exit)
    {
        Handle();
    }

    return NULL;
}

ywb_status_t
MessageManager::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    mMutex.Lock();
    if(MessageManager::S_stopped == mStatus)
    {
        create();
        ast_log_debug("Waiting for MessageManager's start status");
        mCond.Wait(mMutex);

        if(MessageManager::S_failed == mStatus)
        {
            mMutex.Unlock();
            ret = YFS_EAGAIN;
            /*join();*/
            ast_log_debug("MessageManager fail to start");
        }
        else
        {
            YWB_ASSERT(MessageManager::S_running == mStatus);
            mMutex.Unlock();
            ast_log_debug("MessageManager is successfully started");
        }
    }
    else
    {
        mMutex.Unlock();
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
MessageManager::Stop()
{
    ywb_status_t ret = YWB_SUCCESS;

    mMutex.Lock();
    if(MessageManager::S_starting == mStatus ||
            MessageManager::S_running == mStatus ||
            MessageManager::S_failed == mStatus)
    {
        if(MessageManager::S_running == mStatus)
        {
            mStatus = MessageManager::S_stopping;
            YWB_ASSERT(mStopMsg != NULL);
            mMsgQueue.Enqueue(mStopMsg);
            ast_log_debug("Wait for MessageManager stopping");
            mCond.Wait(mMutex);
        }

        join();
        mStatus = MessageManager::S_stopped;
        mMutex.Unlock();
    }
    else /*if(MessageManager::S_stopped != mStatus)*/
    {
        mMutex.Unlock();
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t MessageManager::StartMsgSvc()
{
    ywb_status_t ret = YWB_SUCCESS;
    ASTProxy* astproxy = NULL;
    OSSProxy* ossproxy = NULL;

    ast_log_debug("Start message service");
    do {
        ret = Start();
        if(ret != YWB_SUCCESS)
        {
            /*StopMsgSvc();
            return ret;*/
            break;
        }

        YWB_ASSERT(mAST != NULL);
        astproxy = new ASTProxy(mAST);
        YWB_ASSERT(astproxy != NULL);
        YWB_ASSERT(mCtrlCenter != NULL);
        ossproxy = new OSSProxy(mCtrlCenter);
        YWB_ASSERT(ossproxy != NULL);

        ret = RegisterTargetProxy("oss", ossproxy);
        if(ret != YWB_SUCCESS)
        {
            break;
        }

        ret = RegisterTargetProxy("ast", astproxy);
        if(ret != YWB_SUCCESS)
        {
            break;
        }

        ret = StartTargetProxy("oss");
        if(ret != YWB_SUCCESS)
        {
            break;
        }

        ret = StartTargetProxy("ast");
        if(ret != YWB_SUCCESS)
        {
            break;
        }
    } while(0);

    return ret;
}

ywb_status_t MessageManager::StopMsgSvc()
{
    ywb_status_t ret = YWB_SUCCESS;
    TargetProxy* astproxy = NULL;
    TargetProxy* ossproxy = NULL;

    ast_log_debug("Stop message service");
    GetProxy("oss", &ossproxy);
    GetProxy("ast", &astproxy);

    /*
     * DO NOT de-register proxy before stop, otherwise messages
     * before stop will not be processed
     **/
    Stop();
    DeRegisterTargetProxy("oss");
    DeRegisterTargetProxy("ast");
    StopTargetProxy("oss");
    StopTargetProxy("ast");

    if(ossproxy)
    {
        delete ossproxy;
        ossproxy = NULL;
    }

    if(astproxy)
    {
        delete astproxy;
        astproxy = NULL;
    }

    return ret;
}

ywb_status_t
MessageManager::CreateLogger()
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_status_t
MessageManager::DestroyLogger()
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

void
MessageManager::Enqueue(Message* msg)
{
    YWB_ASSERT(msg);
    YWB_ASSERT(!(msg->GetTarget().empty()));
    YWB_ASSERT(msg->GetTarget().length() < Message::MaxTargetTypeLen);

    ast_log_debug("Enqueue message@" << msg
            << ", target: " << msg->GetTarget()
            << ", Opcode: " << msg->GetOpCode());
    msg->IncRef();
    mMsgQueue.Enqueue(msg);
}

void
MessageManager::Handle()
{
    Message* msg = NULL;
    TargetProxy* proxy = NULL;
    map<const Message::TargetType, TargetProxy*>::iterator iter;

    for (msg = mMsgQueue.Dequeue();
            msg != NULL;
            msg = mMsgQueue.Dequeue())
    {
        if((msg->GetOpCode() != Message::OP_stop))
        {
            iter = mProxy.find(msg->GetTarget());
            YWB_ASSERT(iter != mProxy.end());
            proxy = iter->second;
            YWB_ASSERT(proxy);
            proxy->Handle(msg);
        }
        else
        {
            break;
        }
    }

    mMutex.Lock();
    ast_log_debug("Notifying MessageManager stopped");
    mCond.Signal();
    mMutex.Unlock();
}

ywb_status_t
MessageManager::RegisterTargetProxy(
        const Message::TargetType& type,
        TargetProxy* proxy)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<const Message::TargetType, TargetProxy*>::iterator, bool> pairret;

    YWB_ASSERT(!type.empty());
    YWB_ASSERT(type.length() < Message::MaxTargetTypeLen);

    proxy->SetMsgMgr(this);
    pairret = mProxy.insert(std::make_pair(type, proxy));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t
MessageManager::DeRegisterTargetProxy(
        const Message::TargetType& type)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<const Message::TargetType, TargetProxy*>::iterator iter;
    TargetProxy* proxy = NULL;

    YWB_ASSERT(!type.empty());
    YWB_ASSERT(type.length() < Message::MaxTargetTypeLen);

    if(mProxy.empty())
    {
        ret = YFS_ENOENT;
    }
    else
    {
        iter = mProxy.find(type);
        if(mProxy.end() != iter)
        {
            /*here will not delete target proxy*/
            proxy = iter->second;
            proxy->SetMsgMgr(NULL);
            mProxy.erase(iter);
        }
        else
        {
            ret = YFS_ENOENT;
        }
    }

    return ret;
}

ywb_status_t
MessageManager::StartTargetProxy(const Message::TargetType& type)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<const Message::TargetType, TargetProxy*>::iterator iter;
    TargetProxy* proxy = NULL;

    YWB_ASSERT(!type.empty());
    YWB_ASSERT(type.length() < Message::MaxTargetTypeLen);

    if(mProxy.empty())
    {
        ret = YFS_ENOENT;
    }
    else
    {
        iter = mProxy.find(type);
        if(mProxy.end() != iter)
        {
            proxy = iter->second;
            if(TargetProxy::S_stopped == proxy->GetStatus())
            {
                ret = proxy->Start();
            }
        }
        else
        {
            ret = YFS_ENOENT;
        }
    }

    return ret;
}

ywb_status_t
MessageManager::StopTargetProxy(const Message::TargetType& type)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<const Message::TargetType, TargetProxy*>::iterator iter;
    TargetProxy* proxy = NULL;

    YWB_ASSERT(!type.empty());
    YWB_ASSERT(type.length() < Message::MaxTargetTypeLen);

    if(mProxy.empty())
    {
        ret = YFS_ENOENT;
    }
    else
    {
        iter = mProxy.find(type);
        if(mProxy.end() != iter)
        {
            proxy = iter->second;
            if(TargetProxy::S_running == proxy->GetStatus())
            {
                ret = proxy->Stop();
            }
        }
        else
        {
            ret = YFS_ENOENT;
        }
    }

    return ret;
}

ywb_status_t
MessageManager::StartTargetProxy(TargetProxy* proxy)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(proxy != NULL)
    {
        ret = proxy->Start();
    }

    return ret;
}

ywb_status_t
MessageManager::StopTargetProxy(TargetProxy* proxy)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(proxy != NULL)
    {
        ret = proxy->Stop();
    }

    return ret;
}

ywb_status_t
MessageManager::GetProxy(const Message::TargetType type,
        TargetProxy** proxy)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<const Message::TargetType, TargetProxy*>::iterator iter;

    YWB_ASSERT(!type.empty());
    YWB_ASSERT(type.length() < Message::MaxTargetTypeLen);

    if(mProxy.empty())
    {
        ret = YFS_ENOENT;
        *proxy = NULL;
    }
    else
    {
        iter = mProxy.find(type);
        if(mProxy.end() != iter)
        {
            *proxy = iter->second;
        }
        else
        {
            ret = YFS_ENOENT;
            *proxy = NULL;
        }
    }

    return ret;
}

ywb_status_t
MessageManager::AddSeqno2MsgEntry(ywb_uint32_t seqno, Message* msg)
{
    pair<map<ywb_uint32_t, Message*>::iterator, bool> ret =
            mSeqno2Msg.insert(std::make_pair(seqno, msg));

    if(ret.second)
    {
        return YWB_SUCCESS;
    }
    else
    {
        /*FIXME: what shall be returned*/
        return YFS_EEXIST;
    }
}

ywb_status_t
MessageManager::GetMsgBySeqno(ywb_uint32_t seqno, Message** msg)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    map<ywb_uint32_t, Message*>::iterator iter;

    iter = mSeqno2Msg.find(seqno);
    if(iter == mSeqno2Msg.end())
    {
        ret = YFS_ENOENT;
    }
    else
    {
        *msg = iter->second;
    }

    return ret;
}

ywb_status_t
MessageManager::DeleteSeqno2MsgEntry(ywb_uint32_t seqno)
{
    ywb_uint32_t ret = YWB_SUCCESS;
    map<ywb_uint32_t, Message*>::iterator iter;

    iter = mSeqno2Msg.find(seqno);
    if(iter == mSeqno2Msg.end())
    {
        ret = YFS_ENOENT;
    }
    else
    {
        mSeqno2Msg.erase(iter);
        ret = YWB_SUCCESS;
    }

    return ret;
}

void
MessageManager::Reset()
{
    vector<Message*> msgvec;
    vector<Message*>::iterator iter;
    Message* msg = NULL;

    ast_log_debug("MessageManager reset");
    mStatus = MessageManager::S_stopped;
    mMsgQueue.DequeueNoWait(msgvec);
    iter = msgvec.begin();
    while(iter != msgvec.end())
    {
        msg = *iter;
        iter++;
        delete msg;
        msg = NULL;
    }

    mSeqno2Msg.clear();
    mSeqno.Reset();
    mStatus = MessageManager::S_running;
}

/* vim:set ts=4 sw=4 expandtab */
