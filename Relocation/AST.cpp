#include "OSSCommon/Logger.hpp"
#include "AST/ASTError.hpp"
#include "AST/ASTConfig.hpp"
#include "AST/ASTTime.hpp"
#include "AST/ASTDiskProfile.hpp"
#include "AST/MessageExpress.hpp"
#include "AST/MessageImpl.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "AST/ASTSwapOut.hpp"
#include "AST/ASTHeatDistribution.hpp"
#include "AST/ASTDiskUltimatePerfCache.hpp"
#include "AST/ASTAdvice.hpp"
#include "AST/ASTPerformance.hpp"
#include "AST/ASTPlan.hpp"
#include "AST/ASTArb.hpp"
#include "AST/ASTMigration.hpp"
#include "AST/ASTRelocation.hpp"
#include "AST/ASTEvent.hpp"
#include "AST/ASTHUB.hpp"
#include "AST/AST.hpp"


Generator::Generator(AST* ast) : mStatus(S_stopped),
        mMutex("GeneratorLock"), mAst(ast)
{

}

Generator::~Generator()
{
    Event* event = NULL;
//    Event* stopevent = NULL;

    while((event = mEventContainer.DequeueNoWait()) != NULL)
    {
        event->DecRef();
    }

    while((event = mPendingEventContainer.DequeueNoWait()) != NULL)
    {
        event->DecRef();
    }
}

ywb_uint32_t Generator::GetStatus()
{
    ywb_uint32_t ret = 0;

    mMutex.Lock();
    ret = mStatus;
    mMutex.Unlock();

    return ret;
}

ywb_uint32_t Generator::GetStatusLocked()
{
    return mStatus;
}

void Generator::SetStatus(ywb_uint32_t status)
{
    mMutex.Lock();
    mStatus = status;
    mMutex.Unlock();
}

void Generator::SetStatusLocked(ywb_uint32_t status)
{
    mStatus = status;
}

void* Generator::entry()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t exit = ywb_false;

    mMutex.Lock();
    mStatus = Generator::S_starting;

    /*
     * any Generator specific initialization work should be
     * executed here, and the parent thread will be notified
     * about execution result.
     *
     * currently no generator specific initialization work!
     **/

    /*notify the parent thread about the initialization result*/
    if(YWB_SUCCESS != ret)
    {
        mStatus = Generator::S_failed;
        exit = ywb_true;
    }
    else
    {
        mStatus = Generator::S_running;
    }

    /*notify the AST about generator thread start status*/
    ast_log_debug("Notify generator start status");
    mCond.Signal();
    mMutex.Unlock();

    if(ywb_false == exit)
    {
        Handle();
    }

    return NULL;
}

ywb_status_t Generator::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    mMutex.Lock();
    if(Generator::S_stopped == mStatus)
    {
        create();

        ast_log_debug("Waiting for Generator's start status");
        mCond.Wait(mMutex);

        if(Generator::S_failed == mStatus)
        {
            ast_log_debug("Generator fail to start");
            mMutex.Unlock();
            /*join();*/
            ret = YFS_EAGAIN;
        }
        else
        {
            ast_log_debug("Generator successfully start");
            YWB_ASSERT(Generator::S_running == mStatus);
            mMutex.Unlock();
        }
    }
    else
    {
        mMutex.Unlock();
        ast_log_debug("Start generator in invalid state: " << mStatus);
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t Generator::Stop(ywb_bool_t immediate)
{
    ywb_status_t ret = YWB_SUCCESS;

    mMutex.Lock();
    if(Generator::S_starting == mStatus ||
            Generator::S_running == mStatus ||
            Generator::S_learning == mStatus ||
            Generator::S_learning_done == mStatus ||
            Generator::S_advising == mStatus ||
            Generator::S_advising_done == mStatus ||
            Generator::S_failed == mStatus)
    {
        if(Generator::S_running == mStatus ||
                Generator::S_learning == mStatus ||
                Generator::S_learning_done == mStatus ||
                Generator::S_advising == mStatus ||
                Generator::S_advising_done == mStatus)
        {
            mStatus = Generator::S_stopping;

            /*notify the generator to stop*/
            LaunchStopEvent(immediate);

            /*
             * wait for generator stopped(stop learning and generating
             * advise, while the thread is still alive)
             **/
            ast_log_debug("Wait for generator stopping");
            mCond.Wait(mMutex);
        }

        join();
        mStatus = Generator::S_stopped;
    }
    else if(Generator::S_stopped != mStatus)
    {
        ast_log_debug("Stop generator in invalid state: " << mStatus);
        ret = YFS_EINVAL;
    }

    mMutex.Unlock();

    return ret;
}

void Generator::LaunchCollectConfigEvent(ywb_bool_t adviceafterlearning)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch collect configuration event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_collect_logical_config, NULL);
    event->SetAdviceAfterLearning(adviceafterlearning);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchCollectOBJIOEvent()
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch collect OBJ IO event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_collect_OBJ_io, NULL);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveConfigEvent(CollectLogicalConfigResponse* resp)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve configuration event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_logical_config, resp);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveOBJIOEvent(CollectOBJResponse* resp)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve OBJ IO event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_OBJ_io, resp);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveDeletedSubPoolsEvent(ReportDeleteSubPoolsMessage* resp)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve deleted subpools event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_deleted_subpools, resp);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveDeletedDisksEvent(ReportDeleteDisksMessage* resp)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve deleted disks event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_deleted_disks, resp);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveDeletedOBJsEvent(ReportDeleteOBJsMessage* resp)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve deleted OBJs event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_deleted_OBJs, resp);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveDeletedSubPoolEvent(ReportDeleteSubPoolMessage* resp)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve deleted subpool event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_deleted_subpool, resp);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveDeletedDiskEvent(ReportDeleteDiskMessage* resp)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve deleted disk event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_deleted_disk, resp);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveDeletedOBJEvent(ReportDeleteOBJMessage* resp)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve deleted OBJ event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_deleted_OBJ, resp);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveRelocationType(
        NotifyRelocationType* relocationtypes)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve relocation type notification event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_relocation_type, relocationtypes);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveSwapRequirementEvent(
        NotifySwapRequirement* swaprequirement)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve swap requirement event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_swap_requirement, swaprequirement);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchResolveSwapCancellationEvent(
        NotifySwapCancellation* swapcancellation)
{
    GeneratorEventSet::LearningEvent* event = NULL;

    ast_log_debug("Launch resolve swap cancellation event against Generator");
    event = new GeneratorEventSet::LearningEvent(
            Generator::P_resolve_swap_cancellation, swapcancellation);
    YWB_ASSERT(event != NULL);
    Enqueue(event);
}

void Generator::LaunchAdviseEvent()
{
    GeneratorEventSet::AdviceEvent* adviceevent = NULL;

    ast_log_debug("Launch advice event against Generator");
    adviceevent = new GeneratorEventSet::AdviceEvent();
    YWB_ASSERT(adviceevent != NULL);
    Enqueue(adviceevent);
}

void Generator::LaunchStopEvent(ywb_bool_t immediate)
{
    GeneratorEventSet::StopEvent* stopevent = NULL;

    ast_log_debug("Launch stop event against Generator");
    stopevent = new GeneratorEventSet::StopEvent();
    YWB_ASSERT(stopevent != NULL);
//    Enqueue(stopevent);
    if(true == immediate)
    {
        /*
         * enqueue stop event into both pending and non-pending queue
         * so that the stop event can be handled as soon as possible
         **/
        EnqueueNonPendingFront(stopevent);
        EnqueuePendingFront(stopevent);
    }
    else
    {
        EnqueueNonPending(stopevent);
    }

}

ywb_bool_t
Generator::ShouldEventPend(Event* event)
{
    ywb_bool_t pend = ywb_false;
    ywb_uint32_t status = 0;
    ywb_uint32_t phase = 0;
    GeneratorEventSet::LearningEvent* learningevent = NULL;

    YWB_ASSERT(event != NULL);
    status = GetStatus();
    if(Event::ET_gen_stop == event->GetType())
    {
        pend = ywb_false;
    }
    else if(Generator::S_learning == status)
    {
        /*
         * Any event other than learning event will be pended if
         * generator is in learning status
         **/
        if(Event::ET_gen_learning == event->GetType())
        {
            learningevent = (GeneratorEventSet::LearningEvent*)event;
            phase = learningevent->GetPhase();
            if(!(Generator::P_resolve_logical_config == phase ||
                    Generator::P_collect_OBJ_io == phase ||
                    Generator::P_resolve_OBJ_io == phase))
            {
                pend = ywb_true;
            }
        }
        else
        {
            pend = ywb_true;
        }
    }
    else if(Generator::S_advising == status)
    {
        /*
         * Any event other than advice event will be pended if
         * generator is in advising status
         **/
        if(Event::ET_gen_advise != event->GetType())
        {
            pend = ywb_true;
        }
    }
    else if(Generator::S_noop == status)
    {
        pend = ywb_false;
    }

    return pend;
}

void Generator::Enqueue(Event* event)
{
    YWB_ASSERT(event);
//    /*
//     * put the coming event into pending queue if generator satisfy:
//     * 1. in learning state and the coming event is not learning
//     *    related(collect logical configuration response or collect
//     *    OBJ IO response)
//     * 2. the pending event queue is not empty
//     **/
//    if(((ShouldEventPend(event))) ||
//            !mPendingEventContainer.empty())
//    {
//        ast_log_debug("Generator enqueue event into pending queue: "
//                << event->GetType() << "[ET_none: 0, ET_gen_learning: 1,"
//                << "ET_gen_advice: 2, ET_gen_stop: 3]");
//        mPendingEventContainer.Enqueue(event);
//    }
//    else
//    {
        EnqueueNonPending(event);
//    }
}

void Generator::EnqueueNonPending(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("Enqueue event@" << event << " into Generator's"
            << " non-pending queue, type:" << event->GetType());
    event->IncRef();
    mEventContainer.Enqueue(event);
}

void Generator::EnqueueNonPending(vector<Event*> eventvec)
{
    vector<Event*>::iterator iter;
    Event* event = NULL;

    iter = eventvec.begin();
    ast_log_debug("Enqueue event vector into Generator's non-pending"
            " queue, vector has " << eventvec.size() << " elements: ");
    for(; iter != eventvec.end(); iter++)
    {
        event = *iter;
        event->IncRef();
        ast_log_debug("event@" << event << ", type: " << event->GetType());
    }
    mEventContainer.Enqueue(eventvec);
}

void Generator::EnqueueNonPendingFront(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("EnqueueFront event@" << event << " into Generator's"
            << " non-pending queue, type:" << event->GetType());
    event->IncRef();
    mEventContainer.Enqueue_front(event);
}

void Generator::EnqueueNonPendingFront(vector<Event*> eventvec)
{
    vector<Event*>::iterator iter;
    Event* event = NULL;

    iter = eventvec.begin();
    ast_log_debug("EnqueueFront event vector into "
            << "Generator's non-pending queue, vector has "
            << eventvec.size() << " elements: ");
    for(; iter != eventvec.end(); iter++)
    {
        event = *iter;
        event->IncRef();
        ast_log_debug("event@" << event << ", type: " << event->GetType());
    }

    mEventContainer.Enqueue_front(eventvec);
}

void Generator::EnqueuePending(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("Enqueue event@" << event << " into Generator's"
            << " pending queue, type:" << event->GetType());
    event->IncRef();
    mPendingEventContainer.Enqueue(event);
}

void Generator::EnqueuePending(vector<Event*> eventvec)
{
    vector<Event*>::iterator iter;
    Event* event = NULL;

    iter = eventvec.begin();
    ast_log_debug("Enqueue event vector into Generator's pending"
            " queue, vector is: ");
    for(; iter != eventvec.end(); iter++)
    {
        event = *iter;
        event->IncRef();
        ast_log_debug("event@" << event << ", type: " << event->GetType());
    }

    mPendingEventContainer.Enqueue(eventvec);
}

void Generator::EnqueuePendingFront(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("EnqueueFront event@" << event << " into Generator's"
            << " pending queue, type:" << event->GetType());
    event->IncRef();
    mPendingEventContainer.Enqueue_front(event);
}

void Generator::Handle()
{
    AST* ast = NULL;
    Event* event = NULL;
    GeneratorEventSet::LearningEvent* learningevent = NULL;
    RelocationLearning* learningtask = NULL;
    RelocationTask* tasklearning = NULL;
    RelocationTask* taskctr = NULL;
    RelocationTask* taskitr = NULL;
    Executor* exe = NULL;

    GetAST(&ast);
    for (event = mEventContainer.Dequeue();
            event != NULL;
            event = mEventContainer.Dequeue())
    {
        ast_log_debug("Generator handle event@" << event
                << ", type: " << event->GetType());

//        /*
//         * if current event should be pended, then all its successive
//         * events should be pended too
//         **/
        if(ShouldEventPend(event))
        {
//            ast_log_debug("Current event should pend, so as well its"
//                    " successive events");
//            vector<Event*> eventvec;
            ast_log_debug("Current event@" << event << " should pend");
            EnqueuePending(event);
            event->DecRef();
//            mEventContainer.DequeueNoWait(eventvec);
//            if(!eventvec.empty())
//            {
//                EnqueuePending(eventvec);
//            }
//            eventvec.clear();
        }
        else
        {
            switch(event->GetType())
            {
            case Event::ET_gen_learning:
                ast->GetRelocationTask(
                        RelocationTaskType::RTT_learning, &tasklearning);
                learningevent = (GeneratorEventSet::LearningEvent*)event;
                learningtask = (RelocationLearning*)tasklearning;

                learningtask->SetLearningEvent(learningevent);
                tasklearning->DoCycleWork();
                learningtask->SetLearningEvent(NULL);

                break;
            case Event::ET_gen_advise:
                ast->GetRelocationTask(RelocationTaskType::RTT_ctr, &taskctr);
                ast->GetRelocationTask(RelocationTaskType::RTT_itr, &taskitr);
                ast->GetRelocationTask(
                        RelocationTaskType::RTT_learning, &tasklearning);
                learningtask = (RelocationLearning*)tasklearning;

                /*consolidate logical configuration and summarize disk IO*/
                learningtask->Consolidate();
                /*prepare for generating new advise*/
                taskctr->Prepare();
                taskitr->Prepare();

                /*generate new advise*/
                taskctr->DoCycleWork();
                taskitr->DoCycleWork();

                SetStatus(Generator::S_advising_done);
                HandlePendingEvent();

                /*notify the executor to start planning*/
                ast->GetExecutor(&exe);
                YWB_ASSERT(exe != NULL);
                exe->LaunchPlanEvent();

                break;
            case Event::ET_gen_stop:
                /*
                 * FIXME: whether to destroy learning and advice related
                 * control structures, they will be destroyed when AST
                 * is destroyed if not here.
                 **/

                /*
                 * executor not necessary in stopping state, say the external
                 * tell the AST to stop, which will further signal Generator
                 * to stop(when will set in in Generator::S_stopping status),
                 * but the Generator thread itself may be at Generator::
                 * P_collect_logical_config phase, when Generator thread comes
                 * back, will change its status to Generator::S_learning
                 * */
                /*YWB_ASSERT(Generator::S_stopping == GetStatus());*/

                /*
                 * finalization work against generator should be executed
                 * before notifying caller thread, if any
                 *
                 * currently, no finalization work required!
                 **/
                mMutex.Lock();
                ast_log_debug("Notify generator stopped");
                mCond.Signal();
                mMutex.Unlock();

//                /*
//                 * As we will put the event into both pending and non-pending
//                 * queue(refer to LaunchStopEvent), we are not sure in which
//                 * queue the stop event will be handled firstly, so leave this
//                 * until de-construction of the Generator.
//                 **/
//                /*delete event;
//                event = NULL;*/
                event->DecRef();

                return;
            default:
                YWB_ASSERT(0);
                break;
            }

            event->DecRef();
//            delete event;
//            event = NULL;
        }
    }
}

void Generator::HandlePendingEvent()
{
    vector<Event*> eventvec;
    vector<Event*> eventvec2;
    vector<Event*>::iterator iter;
    Event* event = NULL;

    /*
     * for simplicity, put all events in pending queue into
     * non-pending event queue
     **/
    if(!mPendingEventContainer.empty())
    {
        ast_log_debug("Handle pending event in pending queue");
        /*YWB_ASSERT(mEventContainer.empty());*/
        mPendingEventContainer.DequeueNoWait(eventvec);
        eventvec2.assign(eventvec.rbegin(), eventvec.rend());
        /*enqueue into front of non-pending queue*/
        EnqueueNonPendingFront(eventvec2);
        /*
         * MUST decrease event's reference after calling
         * EnqueueNonPendingFront, otherwise event will
         * be de-constructed
         **/
        for(iter = eventvec2.begin(); iter != eventvec2.end(); )
        {
            event = *iter;
            iter++;
            event->DecRef();
        }

        eventvec.clear();
        eventvec2.clear();
    }
}

//void Generator::SendSync(Message* msg)
//{
//    AST* ast = NULL;
//    ASTMessageManager* msgmgr = NULL;
//    InitiatorSyncContext* synccontext = NULL;
//    pthread_mutex_t* synclock;
//    pthread_cond_t* synccond;
//
//    GetAST(&ast);
//    GetSyncContext(&synccontext);
//    ast->GetMessageManager(&msgmgr);
//    synccontext->GetLock(&synclock);
//    synccontext->GetCond(&synccond);
//
//    msg->SetIsSync(true);
//    pthread_mutex_lock(synclock);
//    msgmgr->Enqueue(msg);
//    pthread_cond_wait(synccond, synclock);
//    pthread_mutex_unlock(synclock);
//}

void Generator::Send(Message* msg)
{
    AST* ast = NULL;
    MessageManager* msgmgr = NULL;

    GetAST(&ast);
    ast->GetMessageManager(&msgmgr);
    msgmgr->Enqueue(msg);
}

//void Generator::WaitOnSyncContext()
//{
//    mSyncContext->Wait();
//}
//
//void Generator::WakeupSyncContext()
//{
//    mSyncContext->Wakeup();
//}

void Generator::Reset()
{
    vector<Event*> eventvec;
    vector<Event*>::iterator iter;
    Event* event = NULL;

    ast_log_debug("Generator reset");
    mStatus = S_stopped;
    if(!mEventContainer.empty())
    {
        mEventContainer.DequeueNoWait(eventvec);
        for(iter = eventvec.begin(); iter != eventvec.end();)
        {
            event = *iter;
            iter++;
            event->DecRef();
        }

        eventvec.clear();
    }

    if(!mPendingEventContainer.empty())
    {
        mPendingEventContainer.DequeueNoWait(eventvec);
        for(iter = eventvec.begin(); iter != eventvec.end();)
        {
            event = *iter;
            iter++;
            event->DecRef();
        }

        eventvec.clear();
    }
}

Executor::Executor(AST* ast) : mStatus(S_stopped), mEpoch(0),
        mMutex("ExecutorLock"), mAst(ast)
{

}

Executor::~Executor()
{
    Event* event = NULL;

    while((event = mEventContainer.DequeueNoWait()) != NULL)
    {
        event->DecRef();
    }

    while((event = mDelayedEventContainer.DequeueNoWait()) != NULL)
    {
        event->DecRef();
    }
}

void* Executor::entry()
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Generator* gen = NULL;
    ywb_bool_t exit = ywb_false;

    GetAST(&ast);
    ast->GetGenerator(&gen);

    mMutex.Lock();
    mStatus = Executor::S_starting;

    /*
     * any Executor specific initialization work should be
     * executed here, and the parent thread will be notified
     * about execution result.
     *
     * currently no Executor specific initialization work!
     **/

    /*notify the parent thread about the initialization result*/
    if(YWB_SUCCESS != ret)
    {
        mStatus = Executor::S_failed;
        exit = ywb_true;
    }
    else
    {
        mStatus = Executor::S_running;
    }

    /*notify the AST about executor thread start status*/
    ast_log_debug("Notify executor start status");
    mCond.Signal();
    mMutex.Unlock();

    if(ywb_false == exit)
    {
        Handle();
    }

    return NULL;
}

ywb_status_t Executor::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    mMutex.Lock();
    if(Executor::S_stopped == mStatus)
    {
        create();

        ast_log_debug("Waiting for Executor's start status");
        mCond.Wait(mMutex);

        if(Executor::S_failed == mStatus)
        {
            ast_log_debug("Executor fail to start");
            mMutex.Unlock();
            /*join();*/
            ret = YWB_EAGAIN;
        }
        else
        {
            ast_log_debug("Executor successfully start");
            YWB_ASSERT(Executor::S_running == mStatus);
            mMutex.Unlock();
        }
    }
    else
    {
        mMutex.Unlock();
        ast_log_debug("Start executor in invalid state");
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t Executor::Stop(ywb_bool_t immediate)
{
    ywb_status_t ret = YWB_SUCCESS;

    mMutex.Lock();
    if(Executor::S_starting == mStatus ||
            Executor::S_running == mStatus ||
            Executor::S_stall == mStatus ||
            Executor::S_failed == mStatus)
    {
        if(Executor::S_running == mStatus ||
                Executor::S_stall == mStatus)
        {
            mStatus = Executor::S_stopping;

            /*notify the executor to stop*/
            LaunchStopEvent(immediate);
            /*
             * wait for executor stop(stop plan/arb/mig while the
             * thread is still alive)
             **/
            ast_log_debug("Wait for executor stopping");
            mCond.Wait(mMutex);
        }

        join();
        mStatus = Executor::S_stopped;
    }
    else if(Executor::S_stopped != mStatus)
    {
        ast_log_debug("Stop executor in invalid state: " << mStatus);
        ret = YFS_EINVAL;
    }

    mMutex.Unlock();

    return ret;
}

ywb_uint32_t Executor::GetStatus()
{
    ywb_uint32_t status = 0;

    mMutex.Lock();
    status = mStatus;
    mMutex.Unlock();

    return status;
}

ywb_uint32_t Executor::GetStatusLocked()
{
    return mStatus;
}

void Executor::SetStatus(ywb_uint32_t status)
{
    mMutex.Lock();
    mStatus = status;
    mMutex.Unlock();
}

void Executor::SetStatusLocked(ywb_uint32_t status)
{
    mStatus = status;
}

ywb_uint64_t Executor::GetEpoch()
{
    return mEpoch;
}

void Executor::SetEpoch(ywb_uint64_t epoch)
{
    mEpoch = epoch;
}

void Executor::LaunchPlanEvent()
{
    ExecutorEventSet::PlanningEvent* planevent = NULL;

    ast_log_debug("Launch plan event against Executor");
    planevent = new ExecutorEventSet::PlanningEvent();
    YWB_ASSERT(planevent != NULL);
    Enqueue(planevent);
}

void Executor::LaunchMigrationCompletedEvent(Event* event)
{
    YWB_ASSERT(event != NULL);
    ast_log_debug("Launch migration completed event against Executor");
    Enqueue(event);
}

void Executor::LaunchMigrationExpiredEvent(Event* event)
{
    YWB_ASSERT(event != NULL);
    ast_log_debug("Launch migration expired event against Executor");
    Enqueue(event);
}

//void Executor::LaunchTimedoutEvent()
//{
//    Enqueue(mTimedoutEvent);
//}

void Executor::LaunchStallEvent()
{
    ExecutorEventSet::StallEvent* stallevent = NULL;

    ast_log_debug("Launch stall event against Executor");
    stallevent = new ExecutorEventSet::StallEvent();
    YWB_ASSERT(stallevent != NULL);
    EnqueueFront(stallevent);
}

void Executor::LaunchStopEvent(ywb_bool_t immediate)
{
    ExecutorEventSet::StopEvent* stopevent = NULL;

    ast_log_debug("Launch stop event against Executor");
    stopevent = new ExecutorEventSet::StopEvent();
    YWB_ASSERT(stopevent != NULL);
    if(true == immediate)
    {
        /*
         * enqueue stop event into both delayed and non-delayed queue
         * so that the stop event can be handled as soon as possible
         **/
        EnqueueFront(stopevent);
        EnqueueDelayedFront(stopevent);
    }
    else
    {
        Enqueue(stopevent);
    }
}

ywb_bool_t Executor::ShouldEventDelayed(Event* event)
{
    ywb_bool_t delay = ywb_false;;
    ywb_uint32_t status = 0;

    YWB_ASSERT(event);
    status = GetStatus();
    if(Executor::S_stall == status)
    {
        if((Event::ET_exe_migration_completed == event->GetType()) ||
                (Event::ET_exe_migration_expired == event->GetType()))
        {
            delay = ywb_true;
        }
    }

    return delay;
}

void Executor::Enqueue(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("Enqueue event@" << event
            << " against Executor, type: " << event->GetType());
    event->IncRef();
    mEventContainer.Enqueue(event);
}

void Executor::Enqueue(vector<Event*> eventvec)
{
    vector<Event*>::iterator iter;
    Event* event = NULL;

    iter = eventvec.begin();
    ast_log_debug("Enqueue event vector into Executor's queue, vector has "
            << eventvec.size() << "elements: ");
    for(; iter != eventvec.end(); iter++)
    {
        event = *iter;
        event->IncRef();
        ast_log_debug("event@" << event << ", type: " << event->GetType());
    }
    mEventContainer.Enqueue(eventvec);
}

void Executor::EnqueueFront(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("EnqueueFront event@" << event
            << " against Executor, type: " << event->GetType());
    event->IncRef();
    mEventContainer.Enqueue_front(event);
}

void Executor::EnqueueFront(vector<Event*> eventvec)
{
    vector<Event*>::iterator iter;
    Event* event = NULL;

    iter = eventvec.begin();
    ast_log_debug("EnqueueFront event vector into Executor's queue, vector has "
            << eventvec.size() << " elements: ");
    for(; iter != eventvec.end(); iter++)
    {
        event = *iter;
        event->IncRef();
        ast_log_debug("event@" << event << ", type: " << event->GetType());
    }
    mEventContainer.Enqueue_front(eventvec);
}

void Executor::EnqueueDelayed(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("Enqueue delayed event@" << event
            << " against Executor, type: " << event->GetType());
    event->IncRef();
    mDelayedEventContainer.Enqueue(event);
}

void Executor::EnqueueDelayedFront(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("EnqueueFront delayed event@" << event
            << " against Executor, type: " << event->GetType());
    event->IncRef();
    mDelayedEventContainer.Enqueue_front(event);
}

void Executor::Handle()
{
    ywb_bool_t triggermore = false;
    ywb_bool_t curwindow = true;
    AST* ast = NULL;
    Event* event = NULL;
    RelocationTask* taskplan = NULL;
    RelocationTask* taskarb = NULL;
    RelocationTask* taskmig = NULL;
    RelocationMIG* migtask = NULL;
    RelocationARB* arbtask = NULL;
    RelocationPlan* plantask = NULL;
    ExecutorEventSet::MigrationCompletedEvent* completedevent = NULL;
    ExecutorEventSet::MigrationExpiredEvent* expiredevent = NULL;
    SubPoolKey* subpoolkey = NULL;
    DiskKey* sourcekey = NULL;
    DiskKey* targetkey = NULL;
    ArbitrateeKey* arbitrateekey = NULL;
    Generator* gen = NULL;

    GetAST(&ast);
    ast->GetRelocationTask(RelocationTaskType::RTT_plan, &taskplan);
    ast->GetRelocationTask(RelocationTaskType::RTT_arb, &taskarb);
    ast->GetRelocationTask(RelocationTaskType::RTT_mig, &taskmig);

    for (event = mEventContainer.Dequeue();
            event != NULL;
            event = mEventContainer.Dequeue())
    {
        ast_log_debug("Executor Handle event@" << event
                << ", type: " << event->GetType());

        if(ShouldEventDelayed(event))
        {
            ast_log_debug("Current event @" << event << " should be delayed");
            EnqueueDelayed(event);
            event->DecRef();
        }
        else
        {
            switch(event->GetType())
            {
            case Event::ET_exe_planning:
                SetStatus(Executor::S_running);

                mEpoch++;

                taskplan->Prepare();
                taskarb->Prepare();
                taskmig->Prepare();

                HandleDelayedEvent();

                /*generate first batch of plans for each subpool*/
                taskplan->DoCycleWork();
                taskarb->DoCycleWork();
                taskmig->DoCycleWork();

                break;
            case Event::ET_exe_migration_completed:
                triggermore = true;
                curwindow = true;
                migtask = (RelocationMIG*)taskmig;
                arbtask = (RelocationARB*)taskarb;
                plantask = (RelocationPlan*)taskplan;

                /*extract subpoolkey, diskkey, arbitrateekey*/
                completedevent = (ExecutorEventSet::MigrationCompletedEvent*)event;
                completedevent->GetSubPoolKey(&subpoolkey);
                completedevent->GetSourceKey(&sourcekey);
                completedevent->GetTargetKey(&targetkey);
                completedevent->GetArbitrateeKey(&arbitrateekey);

                if((Executor::S_stopping == GetStatus()) ||
                        (Executor::S_noop == GetStatus()) ||
                        (YFS_ENOSPACE == completedevent->GetErrCode()) ||
                        (YFS_ENOTDONE == completedevent->GetErrCode()))
                {
                    triggermore = false;
                }

//                mMutex.Lock();
//                if(Executor::S_running == mStatus)
//                {
//                    mMutex.Unlock();
                    /*completion event of previous epoch*/
                    if(completedevent->GetEpoch() != mEpoch)
                    {
                        curwindow = false;
                        ast_log_debug("Handle migration completed event @" << event
                                << " of previous decision window");
                    }
                    else
                    {
                        ast_log_debug("Handle migration completed event @" << event
                                << " of current decision window");
                    }

                    /*
                     * dealing of following two migration failure case is as below:
                     *
                     * (1) when migration request from AST reaches HUB, HUB find
                     *     the corresponding SubPool is not AST-enabled or not
                     *     scheduled for the time being, and HUB tell AST that
                     *     it discards this migration.
                     *
                     *     A. if AST decides not schedule for this SubPool again,
                     *     then how about the SubPool comes back again later soon?
                     *     B. if AST ignore this situation, then AST will generate
                     *     another new plan against this SubPool, but it may be
                     *     discarded again and again if the SubPool never comes back
                     *
                     *     currently will adopt choice A which may not generate plan
                     *     and migration request against this SubPool at least for
                     *     current decision window. When next planning cycle comes,
                     *     all SubPools will be re-scheduled!
                     *
                     * (2) when complete migration notification reaches Migrator,
                     *     Migrator finds that corresponding SubPool does not exist,
                     *     then Migrator tells AST that case.
                     *
                     *        for MIG, don't drive successive migration requests of
                     *        this SubPool any more;
                     *        for ARB, nothing specially done;
                     *        for Plan, don't generate successive against this SubPool
                     *        any more;
                     * */
                    migtask->HandleCompleted(*targetkey, *arbitrateekey,
                            completedevent->GetErrCode(), triggermore);
                    arbtask->HandleCompleted(*subpoolkey, *arbitrateekey,
                            curwindow, completedevent->GetErrCode());
                    plantask->HandleCompleted(*subpoolkey, *sourcekey,
                            *targetkey, triggermore, completedevent->GetErrCode());
//                }
//                else
//                {
//                    /*
//                     * executor not necessary in stopping state, see comments
//                     * above in Generator::Handle()
//                     * */
//                    /*YWB_ASSERT(Executor::S_stopping == mStatus);*/
//
//                    /*remove the item from in-flight*/
//                    migtask->RemoveInflight(*arbitrateekey);
//                    arbtask->HandleComplete(*subpoolkey, *arbitrateekey);
//                    /*it's time to notify AST if there is no more in flight item*/
//                    if(migtask->GetInflightEmpty())
//                    {
//                        ast_log_debug("Notifying AST no more in-flight item");
//                        mCond.Signal();
//                    }
//
//                    mMutex.Unlock();
//                }

                break;
    //        case Event::ET_exe_timedout:
    //            break;
            case Event::ET_exe_migration_expired:
                triggermore = true;
                curwindow = true;
                migtask = (RelocationMIG*)taskmig;
                arbtask = (RelocationARB*)taskarb;
                plantask = (RelocationPlan*)taskplan;

                /*extract subpoolkey, diskkey, arbitrateekey*/
                expiredevent = (ExecutorEventSet::MigrationExpiredEvent*)event;
                expiredevent->GetSubPoolKey(&subpoolkey);
                expiredevent->GetSourceKey(&sourcekey);
                expiredevent->GetTargetKey(&targetkey);
                expiredevent->GetArbitrateeKey(&arbitrateekey);

                /*
                 * S_noop is set by Generator when Generator fail to collect
                 * configuration or IO for all Pools are AST-disabled or not
                 * scheduled for the time being.
                 * */
                if((Executor::S_stopping == GetStatus()) ||
                        (Executor::S_noop == GetStatus()))
                {
                    triggermore = false;
                }

                if(expiredevent->GetEpoch() != mEpoch)
                {
                    curwindow = false;
                    ast_log_debug("Handle migration expired event @" << event
                            << " of previous decision window");
                }
                else
                {
                    ast_log_debug("Handle migration expired event @" << event
                            << " of current decision window");
                }

                migtask->HandleExpired(*targetkey,
                        *arbitrateekey, triggermore);
                arbtask->HandleExpired(*subpoolkey,
                        *arbitrateekey, curwindow);
                plantask->HandleExpired(*subpoolkey,
                        *sourcekey, *targetkey, triggermore);

                break;
            case Event::ET_exe_stall:
                ast->GetGenerator(&gen);
                SetStatus(Executor::S_stall);
                gen->LaunchAdviseEvent();

                break;
            case Event::ET_exe_stop:
                migtask = (RelocationMIG*)taskmig;
                mMutex.Lock();
                /*
                 * executor not necessary in stopping state, say comments
                 * above in Generator::Handle()
                 * */
                /*YWB_ASSERT(Executor::S_stopping == mStatus);*/

//                /*
//                 * there is no in flight items, notify the AST without
//                 * hesitation, otherwise wait for the completion event
//                 * regarding all those in-flight items
//                 **/
//                if(migtask->GetInflightEmpty())
//                {
//                    ast_log_debug("Notify AST no more in-flight item");
                    ast_log_debug("Notify executor stopped");
                    mCond.Signal();
//                }
                mMutex.Unlock();

//                /*
//                 * As we will put the event into both delayed and non-delayed
//                 * queue(refer to LaunchStopEvent), we are not sure in which
//                 * queue the stop event will be handled firstly, so leave this
//                 * until de-construction of the Executor.
//                 **/
//                /*delete event;
//                event = NULL;*/
                event->DecRef();

                return;
            default:
                YWB_ASSERT(0);
                break;
            }

            event->DecRef();
//            delete event;
//            event = NULL;
        }
    }
}

void Executor::HandleDelayedEvent()
{
    vector<Event*> eventvec;
    vector<Event*> eventvec2;
    vector<Event*>::iterator iter;
    Event* event = NULL;

    if(!mDelayedEventContainer.empty())
    {
        ast_log_debug("Handle event in delayed event queue");
        mDelayedEventContainer.DequeueNoWait(eventvec);
        eventvec2.assign(eventvec.rbegin(), eventvec.rend());
        /*enqueue into front of non-delayed queue*/
//        Enqueue(eventvec2);
        EnqueueFront(eventvec2);
        /*
         * MUST decrease event's reference after calling
         * EnqueueFront, otherwise event will be de-constructed
         **/
        for(iter = eventvec2.begin(); iter != eventvec2.end(); )
        {
            event = *iter;
            iter++;
            event->DecRef();
        }

        eventvec.clear();
        eventvec2.clear();
    }
}

void Executor::Reset()
{
    vector<Event*> eventvec;
    vector<Event*>::iterator iter;
    Event* event = NULL;

    ast_log_debug("Executor reset");
    mStatus = S_stopped;
    mEpoch = 0;
    if(!mEventContainer.empty())
    {
        mEventContainer.DequeueNoWait(eventvec);
        for(iter = eventvec.begin(); iter != eventvec.end();)
        {
            event = *iter;
            iter++;
            event->DecRef();
        }

        eventvec.clear();
    }

    if(!mDelayedEventContainer.empty())
    {
        mDelayedEventContainer.DequeueNoWait(eventvec);
        for(iter = eventvec.begin(); iter != eventvec.end();)
        {
            event = *iter;
            iter++;
            event->DecRef();
        }

        eventvec.clear();
    }
}

AST::AST() : mState(AST::ST_none), mRef(0)
{
    Initialize();
}

AST::~AST()
{
    Finalize();
}

ywb_status_t AST::EnableAST()
{
    ywb_status_t ret = YWB_SUCCESS;
    string* diskprofilepath;

    ast_log_debug("Enable Auto Storage Tiering");
    if(AST::ST_enabling == mState ||
            AST::ST_enabled == mState ||
            AST::ST_disabling == mState)
    {
        ast_log_debug("Enable AST in invalid state");
        ret = YFS_EINVAL;
    }
    else
    {
        if(AST::ST_disabled == mState)
        {
            /*REMEMBER to reset current memory state*/
            Reset();
        }

        mState = AST::ST_enabling;
        do {
            /*1. read the configuration file*/
            ret = mConfig->ConfigParseFromFile(Constant::CONFIG_PATH);
            if(YWB_SUCCESS != ret)
            {
                ast_log_debug("fail to parse config file from "
                        << Constant::CONFIG_PATH);
                break;
            }

            /*2. read disk performance profile*/
            mConfig->GetDiskProfile(&diskprofilepath);
            ret = mProfile->BuildDiskPerfProfile(*diskprofilepath);
            if(YWB_SUCCESS != ret)
            {
                ast_log_debug("fail to build disk performance profile from "
                        << *diskprofilepath);
                break;
            }

            /*3. start timer*/
            ret = mTime->StartMotor();
            if(YWB_SUCCESS != ret)
            {
                ast_log_debug("fail to start time manager");
                break;
            }

            /*4. start generator*/
            ret = mGen->Start();
            if(ret != YWB_SUCCESS)
            {
                ast_log_debug("fail to start Generator");
                DisableAST();
                break;
            }

            /*5. start executor*/
            ret = mExe->Start();
            if(ret != YWB_SUCCESS)
            {
                ast_log_debug("fail to start Executor");
                DisableAST();
                break;
            }

            /*6. start migrator*/
            ret = mMigration->Initialize();
            if(YWB_SUCCESS != ret)
            {
                ast_log_debug("fail to initialize migration manager");
                DisableAST();
                break;
            }

            /*NOTE: register and start any other migrator here if necessary*/
            ret = mMigration->StartMigrator("default");
            if(YWB_SUCCESS != ret)
            {
                ast_log_debug("fail to start default migrator");
                DisableAST();
                break;
            }
        } while (0);

        if(YWB_SUCCESS == ret)
        {
            mState = AST::ST_enabled;
        }
        else
        {
            /*DisableAST will mark it as AST::ST_disabled*/
        }
    }

    return ret;
}

ywb_status_t AST::DisableAST()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Disable Auto Storage Tiering");
    if(/*AST::ST_enabling == mState ||*/
            AST::ST_disabling == mState ||
            AST::ST_disabled == mState)
    {
        ast_log_debug("Disable AST in invalid state");
        ret = YFS_EINVAL;
    }
    else
    {
        mState = AST::ST_disabling;

        /*1. stop timer*/
        mTime->StopMotor();

        /*2. stop generator and executor*/
        mGen->Stop(true);
        mExe->Stop(true);

        /*3. stop migrator*/
        mMigration->StopMigrator(true);
        /*NOTE: deregister and stop any other migrator here if necessary*/
        mMigration->Finalize();

        mState = AST::ST_disabled;
    }

    return ret;
}

void AST::HandleEnableAST()
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = EnableAST();
    NotifyAboutEnablingASTResult(ret);
    if(YWB_SUCCESS == ret)
    {
        /*notify the generator to start learning*/
        mGen->LaunchCollectConfigEvent(false);
    }
}

void AST::HandleDisableAST()
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = DisableAST();
    NotifyAboutDisablingASTResult(ret);
}

void AST::NotifyAboutEnablingASTResult(ywb_status_t status)
{
    ast_log_debug("Notify OSS about enabling AST status " << status);
    mASTEnabledMsg->SetStatus(status);
    mMsgMgr->Enqueue(mASTEnabledMsg);
}

void AST::NotifyAboutDisablingASTResult(ywb_status_t status)
{
    ast_log_debug("Notify OSS about disabling AST status " << status);
    mASTDisabledMsg->SetStatus(status);
    mMsgMgr->Enqueue(mASTDisabledMsg);
}

void AST::Initialize()
{
    mConstant = new Constant();
    YWB_ASSERT(mConstant != NULL);
    mErr = new Error(this);
    YWB_ASSERT(mErr != NULL);
    mConfig = new ConfigManager(this);
    YWB_ASSERT(mConfig != NULL);
    mTime = new TimeManager(this);
    YWB_ASSERT(mTime != NULL);
    mProfile = new PerfProfileManager(this);
    YWB_ASSERT(mProfile != NULL);
    mLogicalConfig = new LogicalConfig(this);
    YWB_ASSERT(mLogicalConfig != NULL);
    mIO = new IOManager(this);
    YWB_ASSERT(mIO != NULL);
#ifdef WITH_INITIAL_PLACEMENT
    mSwapOut = new SwapOutManager(this);
    YWB_ASSERT(mSwapOut != NULL);
#endif
    mHeatDistribution = new HeatDistributionManager(this);
    YWB_ASSERT(mHeatDistribution != NULL);
    mDiskUltimatePerfCache = new DiskUltimatePerfCache(this);
    YWB_ASSERT(mDiskUltimatePerfCache != NULL);
    mAdvise = new AdviceManager(this);
    YWB_ASSERT(mAdvise != NULL);
    mPerf = new PerfManager(this);
    YWB_ASSERT(mPerf != NULL);
    mPlan = new PlanManager(this);
    YWB_ASSERT(mPlan != NULL);
    mArb = new Arbitrator(this);
    YWB_ASSERT(mArb != NULL);
    mMigration = new Migration(this);
    YWB_ASSERT(mMigration != NULL);

    mRelocationTasks[RelocationTaskType::RTT_learning] = new RelocationLearning(this);
    YWB_ASSERT(mRelocationTasks[RelocationTaskType::RTT_learning] != NULL);
    mRelocationTasks[RelocationTaskType::RTT_ctr] = new RelocationCTR(this);
    YWB_ASSERT(mRelocationTasks[RelocationTaskType::RTT_ctr] != NULL);
    mRelocationTasks[RelocationTaskType::RTT_itr] = new RelocationITR(this);
    YWB_ASSERT(mRelocationTasks[RelocationTaskType::RTT_itr] != NULL);
    mRelocationTasks[RelocationTaskType::RTT_plan] = new RelocationPlan(this);
    YWB_ASSERT(mRelocationTasks[RelocationTaskType::RTT_plan] != NULL);
    mRelocationTasks[RelocationTaskType::RTT_arb] = new RelocationARB(this);
    YWB_ASSERT(mRelocationTasks[RelocationTaskType::RTT_arb] != NULL);
    mRelocationTasks[RelocationTaskType::RTT_mig] = new RelocationMIG(this);
    YWB_ASSERT(mRelocationTasks[RelocationTaskType::RTT_mig] != NULL);

//    mHUB = NULL;
    mCtrlCenter = NULL;
    mMsgMgr = NULL;
//    mMsgMgr = new ASTMessageManager();

    mGen = new Generator(this);
    YWB_ASSERT(mGen != NULL);
    mExe = new Executor(this);
    YWB_ASSERT(mExe != NULL);

    mASTEnabledMsg = new Message("oss", Message::OP_ast_enabled);
    YWB_ASSERT(mASTEnabledMsg != NULL);
    mASTDisabledMsg = new Message("oss", Message::OP_ast_disabled);
    YWB_ASSERT(mASTDisabledMsg != NULL);
}

void AST::Finalize()
{
    ywb_uint32_t relocationtype = 0;

    if(mConstant != NULL)
    {
        delete mConstant;
        mConstant = NULL;
    }

    if(mErr != NULL)
    {
        delete mErr;
        mErr = NULL;
    }

    if(mConfig != NULL)
    {
        delete mConfig;
        mConfig = NULL;
    }

    if(mTime != NULL)
    {
        delete mTime;
        mTime = NULL;
    }

    if(mProfile != NULL)
    {
        delete mProfile;
        mProfile = NULL;
    }

    if(mLogicalConfig != NULL)
    {
        delete mLogicalConfig;
        mLogicalConfig = NULL;
    }

    if(mIO != NULL)
    {
        delete mIO;
        mIO = NULL;
    }

#ifdef WITH_INITIAL_PLACEMENT
    if(mSwapOut != NULL)
    {
        delete mSwapOut;
        mSwapOut = NULL;
    }
#endif

    if(mHeatDistribution != NULL)
    {
        delete mHeatDistribution;
        mHeatDistribution = NULL;
    }

    if(mDiskUltimatePerfCache != NULL)
    {
        delete mDiskUltimatePerfCache;
        mDiskUltimatePerfCache = NULL;
    }

    if(mAdvise != NULL)
    {
        delete mAdvise;
        mAdvise = NULL;
    }

    if(mPerf != NULL)
    {
        delete mPerf;
        mPerf = NULL;
    }

    if(mPlan != NULL)
    {
        delete mPlan;
        mPlan = NULL;
    }

    if(mArb != NULL)
    {
        delete mArb;
        mArb = NULL;
    }

    if(mMigration != NULL)
    {
        delete mMigration;
        mMigration = NULL;
    }

    for(; relocationtype < Constant::RELOCATION_TASK_CNT; relocationtype++)
    {
        if(mRelocationTasks[relocationtype] != NULL)
        {
            delete mRelocationTasks[relocationtype];
            mRelocationTasks[relocationtype] = NULL;
        }
    }

//    if(mMsgMgr)
//    {
//        delete mMsgMgr;
//        mMsgMgr = NULL;
//    }
    mMsgMgr = NULL;

    if(mGen)
    {
        delete mGen;
        mGen = NULL;
    }

    if(mExe)
    {
        delete mExe;
        mExe = NULL;
    }

    if(mASTEnabledMsg)
    {
        delete mASTEnabledMsg;
        mASTEnabledMsg = NULL;
    }

    if(mASTDisabledMsg)
    {
        delete mASTDisabledMsg;
        mASTDisabledMsg = NULL;
    }

    mState = AST::ST_none;
}

void AST::Reset()
{
    ast_log_debug("AST reset");
    YWB_ASSERT(mConstant);
    YWB_ASSERT(mErr);
    mErr->Reset();

    YWB_ASSERT(mConfig);
    mConfig->Reset();

    YWB_ASSERT(mTime);
    mTime->Reset();

    YWB_ASSERT(mProfile);
    mProfile->Reset();

    YWB_ASSERT(mLogicalConfig);
    mLogicalConfig->Reset();

    YWB_ASSERT(mIO);
    mIO->Reset();

#ifdef WITH_INITIAL_PLACEMENT
    YWB_ASSERT(mSwapOut);
    mSwapOut->Reset();
#endif

    YWB_ASSERT(mHeatDistribution);
    mHeatDistribution->Reset();

    YWB_ASSERT(mDiskUltimatePerfCache);
    mDiskUltimatePerfCache->Reset();

    YWB_ASSERT(mAdvise);
    mAdvise->Reset();

    YWB_ASSERT(mPerf);
    mPerf->Reset();

    YWB_ASSERT(mPlan);
    mPlan->Reset(ywb_true);

    YWB_ASSERT(mArb);
    mArb->Reset(ywb_true);

    YWB_ASSERT(mMigration);
    mMigration->Reset(ywb_true);

    YWB_ASSERT(mGen);
    mGen->Reset();

    YWB_ASSERT(mExe);
    mExe->Reset();

    YWB_ASSERT(mASTEnabledMsg);
    mASTEnabledMsg->SetStatus(YWB_SUCCESS);

    YWB_ASSERT(mASTDisabledMsg);
    mASTDisabledMsg->SetStatus(YWB_SUCCESS);

    mRef = 0;
}

AST* GetSingleASTInstance()
{
    AST* ast = NULL;

    ast = Singleton<AST>::get();
    ast->IncRef();

    return ast;
}

/* vim:set ts=4 sw=4 expandtab */
