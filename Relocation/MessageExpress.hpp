#ifndef __MESSAGE_EXPRESS_HPP__
#define __MESSAGE_EXPRESS_HPP__

#include <pthread.h>
#include <vector>
#include <map>
#include "FsCommon/YfsThread.h"
#include "FsCommon/YfsMutex.hpp"
#include "FsCommon/YfsCond.h"
#include "common/FsAssert.h"
#include "Template/Queue.hpp"
#include "Communication/NetServer.h"
#include "FsCommon/Seqno.hpp"
#include "common/FsAtomic.h"

using namespace std;

/*
 * Base class for all messages, it is designed to be
 * intra-node only communication
 *
 * In AST, name sender of message the initiator, and
 * the receiver of message the target
 **/
class Message{
public:
//    typedef string InitiatorType;
    typedef string TargetType;

//    const static int MaxInitiatorTypeLen = 64;
    const static int MaxTargetTypeLen = 64;

    enum OpCode{
        OP_start = 0,

        /*OSS as target*/
        OP_ast_enabled,
        OP_ast_disabled,
        OP_collect_logical_config_req,
        OP_collect_OBJ_req,

        /*AST as target*/
        OP_enable_ast,
        OP_disable_ast,
        OP_collect_logical_config_resp,
        OP_collect_OBJ_resp,
        /*Deprecated*/
        OP_report_delete_OBJs,
        /*Deprecated*/
        OP_report_delete_disks,
        /*Deprecated*/
        OP_report_delete_subpools,
        OP_report_delete_OBJ,
        OP_report_delete_disk,
        OP_report_delete_subpool,
#ifdef WITH_INITIAL_PLACEMENT
        OP_notify_relocation_type,
        OP_notify_swap_requirement,
        OP_notify_swap_cancellation,
#endif

        /*Monitor as target*/

        /*MessageManager itself as target*/
        OP_stop,

        OP_cnt,
    };

public:
//    Message(InitiatorType initiator, TargetType target, OpCode op, ywb_bool_t sync);
    Message(TargetType target, OpCode op);
    virtual ~Message();

public:
    inline TargetType GetTarget();
    inline ywb_uint32_t GetOpCode();
    inline ywb_bool_t GetIsSync();
    inline ywb_status_t GetStatus();

    inline void SetIsSync(ywb_bool_t val);
    inline void SetStatus(ywb_status_t ret);

public:
//    /*pack message into request buffer*/
//    virtual void PackRequest(void* buffer);
//    /*unpack message from request buffer*/
//    virtual void UnpackRequest(void* buffer);
//    /*pack message into response buffer*/
//    virtual void PackResponse(void* buffer);
//    /*unpack message from response buffer*/
//    virtual void UnpackResponse(void* buffer);

    ywb_int32_t IncRef();
    ywb_int32_t DecRef();

private:
//    /*from which the message is triggered*/
//    InitiatorType mInitiator;
    /*at which the message is targeted*/
    TargetType mTarget;
    /*operation code*/
    OpCode mOpCode;
    /*is this a sync message*/
    ywb_bool_t mIsSync;
    /*return code*/
    ywb_status_t mStatus;
    /*reference count of this message*/
    fs_atomic_t mRef;
//    /*response message*/
//    Message* mRespMsg;
};

///*
// * For synchronous message
// **/
//class InitiatorSyncContext{
//public:
////    InitiatorSyncContext(ASTMessageManager*);
//    InitiatorSyncContext();
//
//    inline void GetCond(pthread_cond_t** cond);
//    inline void GetLock(pthread_mutex_t** lock);
//    inline void Wait();
//    inline void Wakeup();
//
//private:
//    /*set by calling thread, used for sync message*/
//    pthread_cond_t mCond;
//    /*set by calling thread, used for sync message*/
//    pthread_mutex_t mLock;
//};

/*
 * TargetProxy is designed to be the proxy of message target,
 * which acts as a abstraction layer between the initiator and
 * target, take message initiated by AST and targeted at OSS
 * as an example, message will go through AST, MessageManager,
 * OSSProxy, OSS respectively, AST send message to MessageManager
 * and MessageManager redirect it to OSSProxy without knowing
 * the existence of OSS.
 *
 * Like above stated, message initiated by OSS and targeted at
 * AST will go through OSS, MessageManager, ASTProxy, AST.
 **/
class MessageManager;
class TargetProxy{
public:
    enum Status{
        S_stopped,
        S_starting,
        S_running,
        S_stopping,
        S_status_cnt,
    };

    TargetProxy();
    virtual ~TargetProxy();

    inline Status GetStatus();
    inline void GetMsgMgr(MessageManager** mgr);
    inline void SetStatus(Status status);
    inline void SetMsgMgr(MessageManager* mgr);

    virtual ywb_status_t Start();
    virtual ywb_status_t Stop();
    /*message handler*/
    virtual ywb_status_t Handle(Message*);

private:
    Status mStatus;
    /*back reference to ASTMessageManager*/
    MessageManager* mMsgMgr;
};

/*
 * MessageManager is responsible for receiving messages and redirecting
 * them to the specified target.
 *
 * #SYNCHRONOUS/ASYNCHRONOUS MESSAGE#
 * MessageManager will support both synchronous message and asynchronous
 * message, for synchronous message, the initiator will wait for response
 * before carrying on work, for asynchronous message, the initiator can
 * go on its work without waiting for response.
 *
 * For asynchronous message, sequence will not be guaranteed.
 *
 * #TARGETPROXY#
 * MessageManager is designed to be targetproxy-pluggable, which means it
 * is the responsibility of user to register/deregister the targetproxy,
 * also the responsibility of user to start/stop the target proxy.
 **/
class AST;
class ControlCenter;
class MessageManager : public Thread{
public:
    enum Status{
        S_stopped,
        S_starting,
        S_running,
        S_stopping,
        S_failed,
        S_status_cnt,
    };

public:
    MessageManager(AST* ast, ControlCenter* center);
    ~MessageManager();

public:
    inline ywb_uint32_t AllocSeqno();

public:
    void* entry();
    /*start message service thread*/
    ywb_status_t Start();
    /*stop message service thread*/
    ywb_status_t Stop();

    /*start message service*/
    ywb_status_t StartMsgSvc();
    /*stop message service*/
    ywb_status_t StopMsgSvc();

    ywb_status_t CreateLogger();
    ywb_status_t DestroyLogger();

    void Enqueue(Message*);
    void Handle();

    ywb_status_t RegisterTargetProxy(
            const Message::TargetType& type,
            TargetProxy* context);
    ywb_status_t DeRegisterTargetProxy(
            const Message::TargetType& type);

    ywb_status_t StartTargetProxy(const Message::TargetType& type);
    ywb_status_t StopTargetProxy(const Message::TargetType& type);

    ywb_status_t StartTargetProxy(TargetProxy* proxy);
    ywb_status_t StopTargetProxy(TargetProxy* proxy);

    ywb_status_t GetProxy(const Message::TargetType type,
            TargetProxy** proxy);

    ywb_status_t AddSeqno2MsgEntry(ywb_uint32_t, Message*);
    ywb_status_t GetMsgBySeqno(ywb_uint32_t, Message**);
    ywb_status_t DeleteSeqno2MsgEntry(ywb_uint32_t);

    void Reset();

private:
    /*current status of message manager*/
    ywb_uint32_t mStatus;
    /*messages container*/
    Queue<Message> mMsgQueue;
    map<const Message::TargetType, TargetProxy*> mProxy;
    map<ywb_uint32_t, Message*> mSeqno2Msg;
    YfsSeqno mSeqno;
    /*message for stopping message manager*/
    Message* mStopMsg;
    /*reference to AST*/
    AST* mAST;
    /*reference to ControlCenter*/
    ControlCenter* mCtrlCenter;
    /*
     * the external concerns about the success or failure of starting/
     * stopping the message manager, so conditional wait is necessary.
     **/
    Mutex mMutex;
    Cond mCond;
};

#include "AST/MessageExpressInline.hpp"

#endif

/* vim:set ts=4 sw=4 expandtab */
