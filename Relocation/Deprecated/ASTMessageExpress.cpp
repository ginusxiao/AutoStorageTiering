#include "OSSCommon/Logger.hpp"
#include "AST/Deprecated/ASTMessageExpress.hpp"

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
    mAstMsgMgr = NULL;
}

TargetProxy::~TargetProxy()
{
    mAstMsgMgr = NULL;
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

ASTMessageManager::ASTMessageManager() :
        mStatus(ASTMessageManager::S_stopped)
{
    mStopMsg = new Message("msgmgr", Message::OP_stop);
    YWB_ASSERT(mStopMsg != NULL);
    pthread_mutex_init(&mStatusLock, NULL);
    pthread_cond_init(&mStatusCond, NULL);
}

ASTMessageManager::~ASTMessageManager()
{
    Message* msg = NULL;

    YWB_ASSERT(ASTMessageManager::S_stopped == mStatus);

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

void* ASTMessageManager::entry()
{
    ywb_status_t ret = YWB_SUCCESS;

    pthread_mutex_lock(&mStatusLock);
    mStatus = ASTMessageManager::S_starting;

    if(YWB_SUCCESS != ret)
    {
        mStatus = ASTMessageManager::S_failed;
    }
    else
    {
        mStatus = ASTMessageManager::S_running;
    }

    /*notify the calling thread about the executing status*/
    pthread_cond_signal(&mStatusCond);
    pthread_mutex_unlock(&mStatusLock);

    Handle();
    return NULL;
}

ywb_status_t
ASTMessageManager::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    pthread_mutex_lock(&mStatusLock);
    if(ASTMessageManager::S_stopped == mStatus)
    {
        pthread_mutex_unlock(&mStatusLock);
        create();
        /*
         * if the newly created thread returns prior to execution
         * of this code segment, no need to wait at all, otherwise
         * must wait before knowing the newly created thread's status
         *
         * message manager is in stopped status, means the newly
         * created thread has not been executed
         **/
        pthread_mutex_lock(&mStatusLock);
        if(ASTMessageManager::S_stopped == mStatus)
        {
            pthread_cond_wait(&mStatusCond, &mStatusLock);
        }

        if(ASTMessageManager::S_failed == mStatus)
        {
            pthread_mutex_unlock(&mStatusLock);
            ret = YFS_EAGAIN;
        }
        else
        {
            YWB_ASSERT(ASTMessageManager::S_running == mStatus);
            pthread_mutex_unlock(&mStatusLock);
        }
    }
    else
    {
        pthread_mutex_unlock(&mStatusLock);
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
ASTMessageManager::Stop()
{
    ywb_status_t ret = YWB_SUCCESS;

    pthread_mutex_lock(&mStatusLock);
    if(ASTMessageManager::S_starting == mStatus ||
            ASTMessageManager::S_running == mStatus ||
            ASTMessageManager::S_failed == mStatus)
    {
        if(ASTMessageManager::S_running == mStatus)
        {
            mStatus = ASTMessageManager::S_stopping;
            YWB_ASSERT(mStopMsg != NULL);
            mMsgQueue.Enqueue(mStopMsg);
            pthread_cond_wait(&mStatusCond, &mStatusLock);
            pthread_mutex_unlock(&mStatusLock);
        }

        join();
        mStatus = ASTMessageManager::S_stopped;
    }
    else if(ASTMessageManager::S_stopped != mStatus)
    {
        pthread_mutex_unlock(&mStatusLock);
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
ASTMessageManager::CreateLogger()
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_status_t
ASTMessageManager::DestroyLogger()
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

void
ASTMessageManager::Enqueue(Message* msg)
{
    YWB_ASSERT(msg);
    YWB_ASSERT(!(msg->GetTarget().empty()));
    YWB_ASSERT(msg->GetTarget().length() < Message::MaxTargetTypeLen);

    ast_log_debug("Enqueue message, target: " << msg->GetTarget()
            << ", Opcode: " << msg->GetOpCode());
    msg->IncRef();
    mMsgQueue.Enqueue(msg);
}

void
ASTMessageManager::Handle()
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

    pthread_mutex_lock(&mStatusLock);
    pthread_cond_signal(&mStatusCond);
    pthread_mutex_unlock(&mStatusLock);
}

ywb_status_t
ASTMessageManager::RegisterTargetProxy(
        const Message::TargetType& type,
        TargetProxy* proxy)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<const Message::TargetType, TargetProxy*>::iterator, bool> pairret;

    YWB_ASSERT(!type.empty());
    YWB_ASSERT(type.length() < Message::MaxTargetTypeLen);

    proxy->SetAstMsgMgr(this);
    pairret = mProxy.insert(std::make_pair(type, proxy));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t
ASTMessageManager::DeRegisterTargetProxy(
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
            proxy->SetAstMsgMgr(NULL);
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
ASTMessageManager::StartTargetProxy(const Message::TargetType& type)
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
ASTMessageManager::StopTargetProxy(const Message::TargetType& type)
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
ASTMessageManager::StartTargetProxy(TargetProxy* proxy)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(proxy != NULL)
    {
        ret = proxy->Start();
    }

    return ret;
}

ywb_status_t
ASTMessageManager::StopTargetProxy(TargetProxy* proxy)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(proxy != NULL)
    {
        ret = proxy->Stop();
    }

    return ret;
}

ywb_status_t
ASTMessageManager::GetProxy(const Message::TargetType type,
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
ASTMessageManager::AddSeqno2MsgEntry(ywb_uint32_t seqno, Message* msg)
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
ASTMessageManager::GetMsgBySeqno(ywb_uint32_t seqno, Message** msg)
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
ASTMessageManager::DeleteSeqno2MsgEntry(ywb_uint32_t seqno)
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

void ASTMessageManager::WaitForStatus()
{
    pthread_mutex_lock(&mStatusLock);
    pthread_cond_wait(&mStatusCond, &mStatusLock);
    pthread_mutex_unlock(&mStatusLock);
}

void ASTMessageManager::NotifyAboutStatus()
{
    pthread_mutex_lock(&mStatusLock);
    pthread_cond_signal(&mStatusCond);
    pthread_mutex_unlock(&mStatusLock);
}

/* vim:set ts=4 sw=4 expandtab */
