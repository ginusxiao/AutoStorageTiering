#ifndef __MESSAGE_EXPRESS_INLINE_HPP
#define __MESSAGE_EXPRESS_INLINE_HPP

#include "AST/MessageExpress.hpp"

Message::TargetType
Message::GetTarget()
{
    return mTarget;
}

ywb_uint32_t
Message::GetOpCode()
{
    return mOpCode;
}

ywb_bool_t
Message::GetIsSync(){
    return mIsSync;
}

ywb_status_t
Message::GetStatus(){
    return mStatus;
}

void
Message::SetIsSync(ywb_bool_t val)
{
    mIsSync = val;
}

void
Message::SetStatus(ywb_status_t ret){
    mStatus = ret;
}

//void InitiatorSyncContext::GetCond(pthread_cond_t** cond)
//{
//    *cond = &mCond;
//}
//
//void InitiatorSyncContext::GetLock(pthread_mutex_t** lock)
//{
//    *lock = &mLock;
//}
//
//void InitiatorSyncContext::Wait()
//{
//    pthread_mutex_lock(&mLock);
//    pthread_cond_wait(&mCond, &mLock);
//    pthread_mutex_unlock(&mLock);
//}
//
//void InitiatorSyncContext::Wakeup()
//{
//    pthread_mutex_lock(&mLock);
//    pthread_cond_signal(&mCond);
//    pthread_mutex_unlock(&mLock);
//}

TargetProxy::Status
TargetProxy::GetStatus()
{
    return mStatus;
}

void
TargetProxy::GetMsgMgr(MessageManager** mgr)
{
    *mgr = mMsgMgr;
}

void
TargetProxy::SetStatus(Status status)
{
    mStatus = status;
}

void
TargetProxy::SetMsgMgr(MessageManager* mgr)
{
    mMsgMgr = mgr;
}

ywb_uint32_t
MessageManager::AllocSeqno()
{
    return mSeqno.GetSeqno();
}

#endif

/* vim:set ts=4 sw=4 expandtab */
