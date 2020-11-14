#ifndef __AST_HPP__
#define __AST_HPP__

#include <pthread.h>
#include "common/FsStatus.h"
#include "FsCommon/YfsThread.h"
#include "FsCommon/YfsMutex.hpp"
#include "FsCommon/YfsCond.h"
#include "common/FsAssert.h"
#include "Template/Queue.hpp"
#include "Communication/NetServer.h"
#include "AST/ASTConstant.hpp"
#include "OSSCommon/Logger.hpp"
#include "OSSCommon/Globals.hpp"

/*forward reference*/
class HUB;
class ControlCenter;
class ConfigManager;
class TimeManager;
class PerfProfileManager;
class LogicalConfig;
class IOManager;
class SwapOutManager;
class HeatDistributionManager;
class DiskUltimatePerfCache;
class AdviceManager;
class PerfManager;
class PlanManager;
class Arbitrator;
class Migration;
class RelocationTask;
class Error;
class AST;
class Message;
class InitiatorSyncContext;
class MessageManager;
class Event;
class CollectLogicalConfigResponse;
class CollectOBJResponse;
class ReportDeleteSubPoolsMessage;
class ReportDeleteDisksMessage;
class ReportDeleteOBJsMessage;
class ReportDeleteSubPoolMessage;
class ReportDeleteDiskMessage;
class ReportDeleteOBJMessage;
#ifdef WITH_INITIAL_PLACEMENT
class NotifyRelocationType;
class NotifySwapRequirement;
class NotifySwapCancellation;
#endif

/*generator is responsible for generating advise*/
class Generator : public Thread {
public:
    enum Status
    {
        S_stopped = 0,
        S_starting,
        S_running,
        S_learning,
        S_learning_done,
        S_advising,
        S_advising_done,
        S_noop,
        S_stopping,
        S_failed,
    };

    enum Phase{
        P_collect_logical_config,
        P_resolve_logical_config,
        P_collect_OBJ_io,
        P_resolve_OBJ_io,
        /*Deprecated*/
        P_resolve_deleted_OBJs,
        /*Deprecated*/
        P_resolve_deleted_disks,
        /*Deprecated*/
        P_resolve_deleted_subpools,
        P_resolve_deleted_OBJ,
        P_resolve_deleted_disk,
        P_resolve_deleted_subpool,
        P_resolve_relocation_type,
        P_resolve_swap_requirement,
        P_resolve_swap_cancellation,
        P_cnt,
    };

    /*
     * put definition of constructor and de-constructor in .cpp
     * file to avoid cross reference issue
     **/
    Generator(AST* ast);
    ~Generator();

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    ywb_uint32_t GetStatus();
    ywb_uint32_t GetStatusLocked();
    void SetStatus(ywb_uint32_t status);
    void SetStatusLocked(ywb_uint32_t status);

    void* entry();
    ywb_status_t Start();
    /*@immediate: whether to stop generator immediately*/
    ywb_status_t Stop(ywb_bool_t immediate);

    /*issue learning related event*/
    void LaunchCollectConfigEvent(ywb_bool_t adviceafterlearning);
    void LaunchCollectOBJIOEvent();
    void LaunchResolveConfigEvent(CollectLogicalConfigResponse* resp);
    void LaunchResolveOBJIOEvent(CollectOBJResponse* resp);
    void LaunchResolveDeletedSubPoolsEvent(ReportDeleteSubPoolsMessage* resp);
    void LaunchResolveDeletedDisksEvent(ReportDeleteDisksMessage* resp);
    void LaunchResolveDeletedOBJsEvent(ReportDeleteOBJsMessage* resp);
    void LaunchResolveDeletedSubPoolEvent(ReportDeleteSubPoolMessage* resp);
    void LaunchResolveDeletedDiskEvent(ReportDeleteDiskMessage* resp);
    void LaunchResolveDeletedOBJEvent(ReportDeleteOBJMessage* resp);
    void LaunchResolveRelocationType(NotifyRelocationType* relocationtypes);
    void LaunchResolveSwapRequirementEvent(NotifySwapRequirement* swaprequirement);
    void LaunchResolveSwapCancellationEvent(NotifySwapCancellation* swapcancellation);
    /*issue advise event*/
    void LaunchAdviseEvent();
    /*
     * issue stop event
     * @immediate: whether to stop generator immediately
     **/
    void LaunchStopEvent(ywb_bool_t immediate);

    /*check whether or not the given event is learning related*/
    ywb_bool_t ShouldEventPend(Event* event);
    /*currently equals to EnqueueNonPending(Event*)*/
    void Enqueue(Event*);
//    void EnqueueFront(Event*);
    /*enqueue one event into non-pending queue*/
    void EnqueueNonPending(Event*);
    /*enqueue event vector into non-pending queue*/
    void EnqueueNonPending(vector<Event*> eventvec);
    /*enqueue one event into front of non-pending queue*/
    void EnqueueNonPendingFront(Event*);
    /*enqueue event vector into front of non-pending queue*/
    void EnqueueNonPendingFront(vector<Event*> eventvec);
    /*enqueue one event into pending queue*/
    void EnqueuePending(Event*);
    /*enqueue event vector into pending queue*/
    void EnqueuePending(vector<Event*> eventvec);
    /*enqueue one event into front of pending queue*/
    void EnqueuePendingFront(Event*);

    void Handle();
    void HandlePendingEvent();

//    /*for synchronous message*/
//    void SendSync(Message*);
    /*for non-synchronous message*/
    void Send(Message*);

//    /*wait for synchronous message comes back*/
//    void WaitOnSyncContext();
//    /*wake up waiting thread*/
//    void WakeupSyncContext();
    void Reset();

private:
//    /*whether generate advise after learning, set externally*/
//    bool mAdviseAfterLearning;
    /*current status*/
    ywb_uint32_t mStatus;
//    /*
//     * indicates new coming event will be on @mPendingEvnetContainer,
//     * if set to non-0, otherwise on @mEventContainer
//     **/
//    ywb_uint32_t mPendComingEvent;

    /*container of event*/
    Queue<Event> mEventContainer;
    /*
     * pending events are those launched before synchronous events
     * completed
     **/
    Queue<Event> mPendingEventContainer;

//    /*learning event, reusable*/
//    LearningEvent* mLearningEvent;
//    /*generate advise, reusable*/
//    Event* mAdviseEvent;
//    /*stop generator*/
//    Event* mStopEvent;
//    /*
//     * #Synchronous Context#
//     * Some event such as LearningEvent requires synchronous processing,
//     * LearningEvent has following phases(refer to RelocationLearning::
//     * Phase):
//     * (1) P_collect_logical_config,
//     * (2) P_resolve_logical_config,
//     * (3) P_collect_OBJ_io,
//     * (4) P_resolve_OBJ_io,
//     * (5) P_resolve_deleted_OBJs,
//     * (6) P_resolve_deleted_disks,
//     * (7) P_resolve_deleted_subpools
//     *
//     * (1), (2), (3) and (4) must be executed sequentially. Synchronous
//     * context is used to guarantee this sequentiality.
//     **/
//    InitiatorSyncContext* mSyncContext;

    /*
     * AST is responsible for starting generator and executor threads
     * in order, generator and executor thread must notify AST about
     * its status(such as success or fail) to AST so that AST can
     * determine what to do, so conditional wait is necessary
     *
     * AST must notify the generator and executor to stop when disable
     * AST is received, generator and executor must notify AST about
     * their successful stop, so that the thread resources can be
     * released. For the latter, either libevent or pthread condition/
     * mutex should be adopted. Generator and executor both use libevent
     * to handle inter-thread notification, but here use pthread instead.
     **/
    Mutex mMutex;
    Cond mCond;

    /*back reference to AST*/
    AST* mAst;
};

/*executor is responsible for migration*/
class Executor : public Thread {
public:
    enum Status
    {
        S_stopped = 0,
        S_starting,
        S_running,
        S_stall,
        S_noop,
        S_stopping,
        S_failed,
    };

    /*
     * put definition of constructor and de-constructor in .cpp
     * file to avoid cross reference issue
     **/
    Executor(AST* ast);
    ~Executor();

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    void* entry();
    ywb_status_t Start();
    /*@immediate: whether to stop executor immediately*/
    ywb_status_t Stop(ywb_bool_t immediate);

    ywb_uint32_t GetStatus();
    ywb_uint32_t GetStatusLocked();
    void SetStatus(ywb_uint32_t status);
    void SetStatusLocked(ywb_uint32_t status);
    ywb_uint64_t GetEpoch();
    void SetEpoch(ywb_uint64_t epoch);

    /*issue plan event*/
    void LaunchPlanEvent();
//    /*issue timedout event*/
//    void LaunchTimedoutEvent();
    /*issue migration completion event*/
    void LaunchMigrationCompletedEvent(Event* event);
    void LaunchMigrationExpiredEvent(Event* event);
    void LaunchStallEvent();
    /*
     * issue stop event
     * @immediate: whether to stop executor immediately
     **/
    void LaunchStopEvent(ywb_bool_t immediate);

    /*check whether or not the given event is learning related*/
    ywb_bool_t ShouldEventDelayed(Event* event);
    void Enqueue(Event*);
    void Enqueue(vector<Event*>);
    void EnqueueFront(Event*);
    void EnqueueFront(vector<Event*>);
    void EnqueueDelayed(Event*);
    void EnqueueDelayedFront(Event*);

    void Handle();
    void HandleDelayedEvent();
    void Reset();

private:
    /*current status*/
    ywb_uint32_t mStatus;
    /*one epoch corresponding to one (short-term) decision window*/
    ywb_uint64_t mEpoch;
    /*container of event*/
    Queue<Event> mEventContainer;
    /*
     * Currently Event::ET_exe_completion comes after Event::ET_stall
     * and before Event::ET_exe_planning must be delayed to beginning
     * of next decision window.
     **/
    Queue<Event> mDelayedEventContainer;

//    /*plan event*/
//    Event* mPlanEvent;
//    /*timedout event*/
//    Event* mTimedoutEvent;
//    /*stop event*/
//    Event* mStopEvent;

    /*
     * AST is responsible for starting generator and executor threads
     * in order, generator and executor thread must notify AST about
     * its status(such as success or fail) to AST so that AST can
     * determine what to do, so conditional wait is necessary
     *
     * AST must notify the generator and executor to stop when disable
     * AST is received, generator and executor must notify AST about
     * their successful stop, so that the thread resources can be
     * released. For the latter, either libevent or pthread condition/
     * mutex should be adopted. Generator and executor both use libevent
     * to handle inter-thread notification, but here use pthread instead.
     **/
    Mutex mMutex;
    Cond mCond;

    /*back reference to AST*/
    AST* mAst;
};

class AST {
public:
    enum State
    {
        ST_none = 0,
        ST_enabling,
        ST_enabled,
        ST_disabling,
        ST_disabled,
        ST_failed,
    };

    AST();
    virtual ~AST();

    inline void IncRef();
    inline void DecRef();

    inline void GetError(Error** err);
    inline void GetConfigManager(ConfigManager** configmgr);
    inline void GetTimeManager(TimeManager** timemgr);
    inline void GetPerformanceProfile(PerfProfileManager** profile);
    inline void GetLogicalConfig(LogicalConfig** logicalconfig);
    inline void GetIOManager(IOManager** iomgr);
#ifdef WITH_INITIAL_PLACEMENT
    inline void GetSwapOutManager(SwapOutManager** swapout);
#endif
    inline void GetHeatDistributionManager(
            HeatDistributionManager** heatdistribution);
    inline void GetDiskUltimatePerfCache(DiskUltimatePerfCache** cache);
    inline void GetAdviseManager(AdviceManager** advise);
    inline void GetPerfManager(PerfManager** perf);
    inline void GetPlanManager(PlanManager** plan);
    inline void GetArb(Arbitrator** arb);
    inline void GetMigration(Migration** migration);
    inline ywb_status_t GetRelocationTask(
            ywb_uint32_t taskType, RelocationTask** reloaction);
//    inline void GetHUB(HUB** hub);
    inline void GetControlCenter(ControlCenter** center);
    inline void GetMessageManager(MessageManager** msgmgr);
    inline void GetGenerator(Generator** gen);
    inline void GetExecutor(Executor** exe);
    inline ywb_uint32_t GetState();

    inline void SetConfigManager(ConfigManager*& configmgr);
    inline void SetTimeManager(TimeManager*& timemgr);
    inline void SetPerformanceProfile(PerfProfileManager*& profile);
    inline void SetLogicalConfig(LogicalConfig*& logicalconfig);
    inline void SetIOManager(IOManager*& iomgr);
#ifdef WITH_INITIAL_PLACEMENT
    inline void SetSwapOutManager(SwapOutManager*& swapout);
#endif
    inline void SetHeatDistributionManager(
            HeatDistributionManager*& heatdistribution);
    inline void SetDiskUltimatePerfCache(DiskUltimatePerfCache* cache);
    inline void SetAdviseManager(AdviceManager*& advise);
    inline void SetPerfManager(PerfManager*& perf);
    inline void SetPlanManager(PlanManager*& plan);
    inline void SetArb(Arbitrator*& arb);
    inline void SetMigration(Migration*& migration);
//    inline void SetHUB(HUB* hub);
    inline void SetControlCenter(ControlCenter* center);
    inline void SetMessageManager(MessageManager* msgmgr);
    inline void SetState(ywb_uint32_t state);

//    /*must be called before enable AST*/
//    ywb_status_t StartMsgSvc();
//    /*called only if AST is disabled*/
//    ywb_status_t StopMsgSvc();

//    /*wait for generator thread start/stop status*/
//    void WaitForGenStatus();
//    /*notify generator thread start/stop status*/
//    void NotifyGenStatus();
//    /*wait for executor thread start/stop status*/
//    void WaitForExeStatus();
//    /*notify executor thread start/stop status*/
//    void NotifyExeStatus();

    /*enable auto storage tiering*/
    ywb_status_t EnableAST();
    /*disable auto storage tiering*/
    ywb_status_t DisableAST();

    void HandleEnableAST();
    void HandleDisableAST();

    /*notify the external about result of Enabling AST*/
    void NotifyAboutEnablingASTResult(ywb_status_t status);
    /*notify the external about result of Disabling AST*/
    void NotifyAboutDisablingASTResult(ywb_status_t status);

    void Initialize();
    void Finalize();
    /*
     * Reset current memory state, but retain the allocated memory structures.
     *
     * Theoretically it can be called anywhere, but MUST pay attention that
     * some module may have different activity regarding the time when it is
     * reset, take TimeManager(which will initialize the first cycle when reset)
     * as an example:
     * if called at the end of DisableAST, then many cycles may already elapsed
     * before next invocation of EnableAST, which leads to error;
     * if called at the begin of EnableAST, TimeManager will work as expected
     *
     * Meanwhile, if called at the end of DisableAST, configuration may change
     * before next invocation of EnableAST, which calls for re-initialization
     * of those components relied on configuration.
     *
     * So, it is better to call Reset at the begin of EnableAST
     * */
    void Reset();

public:
    /*constant management*/
    Constant* mConstant;
    /*error management*/
    Error* mErr;
    /*configuration options management*/
    ConfigManager* mConfig;
    /*time management*/
    TimeManager* mTime;
    /*disk performance profile*/
    PerfProfileManager* mProfile;

    /*logical configuration*/
    LogicalConfig* mLogicalConfig;
//    /*
//     * snapshot of logical configuration, for Executor
//     * internal use only
//     **/
//    LogicalConfig* mLogicalConfigSnapshot;
    /*IO*/
    IOManager* mIO;
#ifdef WITH_INITIAL_PLACEMENT
    SwapOutManager* mSwapOut;
#endif
    /*heat distribution*/
    HeatDistributionManager* mHeatDistribution;
    /*ultimate disk performance cache*/
    DiskUltimatePerfCache* mDiskUltimatePerfCache;
    /*advice*/
    AdviceManager* mAdvise;
    /*performance*/
    PerfManager* mPerf;
    /*plan*/
    PlanManager* mPlan;
    /*arbitrator*/
    Arbitrator* mArb;
    /*migration*/
    Migration* mMigration;

    /*relocation task*/
    RelocationTask* mRelocationTasks[Constant::RELOCATION_TASK_CNT];

//    /*back reference to HUB*/
//    HUB* mHUB;
    /*back reference to control center*/
    ControlCenter* mCtrlCenter;
    MessageManager* mMsgMgr;

    /*responsible for learning configuration and generating advise*/
    Generator* mGen;
    /*responsible for plan/arb/migration*/
    Executor* mExe;

    /*for notifying the external about EnableAST result*/
    Message* mASTEnabledMsg;
    /*for notifying the external about DisableAST result*/
    Message* mASTDisabledMsg;

    /*AST state*/
    ywb_uint32_t mState;
    /*
     * AST is got by GetSingleASTInstance() which means multiple threads
     * may refer to the same AST instance, so reference count is required
     **/
    fs_atomic_t mRef;
};

/*
 * thread-safe
 * currently mainly for two purpose:
 * (1) Control Center start AST
 * (2) AST internal logical(only if one entity can not get AST directly
 *     from its context, then use this global one - singleton version)
 **/
extern AST* GetSingleASTInstance();

#include "AST/ASTInline.hpp"

#endif

/* vim:set ts=4 sw=4 expandtab */
