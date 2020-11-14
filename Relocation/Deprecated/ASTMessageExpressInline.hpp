#ifndef __AST_MESSAGE_EXPRESS_INLINE_HPP
#define __AST_MESSAGE_EXPRESS_INLINE_HPP

#include "AST/Deprecated/ASTMessageExpress.hpp"

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
TargetProxy::GetAstMsgMgr(ASTMessageManager** mgr)
{
    *mgr = mAstMsgMgr;
}

void
TargetProxy::SetStatus(Status status)
{
    mStatus = status;
}

void
TargetProxy::SetAstMsgMgr(ASTMessageManager* mgr)
{
    mAstMsgMgr = mgr;
}

ywb_uint32_t
ASTMessageManager::AllocSeqno()
{
    return mSeqno.GetSeqno();
}

void
ASTMessageManager::GetStatusLock(pthread_mutex_t** lock)
{
    *lock = &mStatusLock;
}

void
ASTMessageManager::GetStatusCond(pthread_cond_t** cond)
{
    *cond = &mStatusCond;
}

#endif

/* vim:set ts=4 sw=4 expandtab */
