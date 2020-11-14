#include "FsCommon/YfsThread.h"
#include "AST/ASTConfig.hpp"
#include "AST/ASTMigration.hpp"
#include "AST/ASTTime.hpp"

CycleManager::CycleManager(ywb_uint32_t period) : mCyclePeriod(period)
{
    time_t now;

    YWB_ASSERT(period != 0);
    now = time(NULL);
    mCycleFlag.SetFlag(CycleManager::CF_alive);
    mCurCycleAbsolute = (now + period - 1) / period;
    mCurCycleRelative = 0;
    mCycleASTEnabled = mCurCycleAbsolute;
}

void
CycleManager::LaunchNewCycle()
{
    ast_log_debug("Cycle manager launch new cycle");
    mCurCycleAbsolute++;
    mCurCycleRelative++;
}

void
CycleManager::Reset()
{
    time_t now;

    now = time(NULL);
    mCycleFlag.Reset();
    mCycleFlag.SetFlag(CycleManager::CF_alive);
    /*DO NOT touch @mCyclePeriod, follow the previously set value*/
    mCurCycleAbsolute = (now + mCyclePeriod - 1) / mCyclePeriod;
    mCurCycleRelative = 0;
    mCycleASTEnabled = mCurCycleAbsolute;
}

DecisionWindowManager::DecisionWindowManager(
        ywb_uint32_t cycles, ywb_uint32_t *cycleshare) : mCycles(cycles)
{
    ywb_uint32_t index = 0;
    ywb_uint32_t sum = 0;

    YWB_ASSERT(cycleshare != NULL);
    mCyclesPlusOneSquare = ((cycles + 1) * (cycles + 1));
    mMultiplierOne = (mCyclesPlusOneSquare - 2 * (cycles + 1));
    mMultiplierTwo = 2 * (cycles + 1);

    for(; index < Constant::CYCLE_TYPE_CNT; index++)
    {
        mCyclsShareOfEachCycleType[index] = cycleshare[index];
        sum += cycleshare[index];
    }

    YWB_ASSERT(cycles == sum);
    mCurCycleType = FindNextCaredCycleType(CycleManager::CT_cnt);
    mEnabled = false;
    mIsNewWindowStarted = false;
}

ywb_uint32_t DecisionWindowManager::FindNextCaredCycleType(ywb_uint32_t from)
{
    ywb_uint32_t next = 0;

    next = from + 1;

    /*search from @from onward*/
    while(next < Constant::CYCLE_TYPE_CNT)
    {
        if(mCyclsShareOfEachCycleType[next] != 0)
        {
            return next;
        }

        next++;
    }

    /*search from the very beginning*/
    if(Constant::CYCLE_TYPE_CNT <= next)
    {
        next = CycleManager::CT_learning;
        while(next < from)
        {
            if(mCyclsShareOfEachCycleType[next] != 0)
            {
                return next;
            }

            next++;
        }
    }

    return from;
}

void
DecisionWindowManager::LaunchNewCycle(ywb_uint32_t relativecycle)
{
    ywb_uint32_t type = 0;
    ywb_uint32_t sum = 0;
    ywb_uint32_t relativeinwindow = 0;

    relativeinwindow = relativecycle % mCycles;
    if(0 == relativeinwindow)
    {
        mIsNewWindowStarted = true;
        mCurCycleType = FindNextCaredCycleType(CycleManager::CT_cnt);
        YWB_ASSERT(CycleManager::CT_cnt != mCurCycleType);
    }
    else
    {
        mIsNewWindowStarted = false;
        for(; (type < (mCurCycleType + 1)) &&
                (type < Constant::CYCLE_TYPE_CNT); type++)
        {
            sum += mCyclsShareOfEachCycleType[type];
        }

        if(sum == relativeinwindow)
        {
            mCurCycleType = FindNextCaredCycleType(mCurCycleType);
            YWB_ASSERT(CycleManager::CT_cnt != mCurCycleType);
        }
    }
}

void
DecisionWindowManager::Reset(ywb_uint32_t cycles, ywb_uint32_t* cycleshare)
{
    YWB_ASSERT(cycleshare != NULL);
    SetCycles(cycles);
    SetCycleShare(cycleshare, Constant::CYCLE_TYPE_CNT);
    mCurCycleType = FindNextCaredCycleType(CycleManager::CT_cnt);
    mIsNewWindowStarted = false;
}

static void
CycleLauncher(int fd, short what, void* arg) {
    TimerArg* param = NULL;
    TimeManager* timemanager = NULL;
    int event = 0;

    param = (TimerArg*)arg;
    timemanager = (TimeManager*)param->m_handler;
    event = param->m_event;

    if(Timer::TIMER_CYCLE_LAUNCHER == event)
    {
        timemanager->LaunchNewCycle();
    }
}

Timer::Timer(TimeManager* timemanager, ywb_uint64_t intervalsec,
        ywb_uint64_t intervalusec) : mInterest(NULL),
        mTimeManager(timemanager), mCycleIntervalSec(intervalsec),
        mCycleIntervalUsec(intervalusec), mRunning(false)
{

}

Timer::~Timer()
{
    if(mInterest) {
        ::event_del(mInterest);
        ::event_free(mInterest);
        mInterest = NULL;
    }

    mTimeManager = NULL;
}

ywb_status_t Timer::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    mRunning = true;
    ret = Server::start();
    if(YWB_SUCCESS != ret)
    {
        ast_log_debug("Fail to start timer");
    }

    return ret;
}

ywb_status_t Timer::Stop()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Wait for Timer stopping");
    mRunning = false;
    wakeup();

    return ret;
}

ywb_status_t Timer::initialize()
{
    struct timeval timeout = { 0 };
    TimerArg* timearg;

    timearg = new TimerArg(mTimeManager, Timer::TIMER_CYCLE_LAUNCHER);
    mInterest = ::event_new(base(), -1, EV_TIMEOUT | EV_PERSIST,
            CycleLauncher, timearg);
    timeout.tv_sec = mCycleIntervalSec;
    timeout.tv_usec = mCycleIntervalUsec;
    ::event_add(mInterest, &timeout);
    PushTimerArg(timearg);

    return YWB_SUCCESS;
}

void Timer::handleevent()
{
    if(!mRunning)
    {
        Server::stop();
    }
}

void Timer::Reset()
{

}

TimeManager::TimeManager(AST* ast)
{
    ConfigManager* configmgr = NULL;
    ywb_uint32_t cycles = 0;
    ywb_uint32_t cyclesshare[Constant::CYCLE_TYPE_CNT] = {0, 0};
    ywb_uint32_t cycletype = 0;

    YWB_ASSERT(ast != NULL);
    ast->GetConfigManager(&configmgr);
    YWB_ASSERT(configmgr != NULL);

    mCycleManager = new CycleManager(configmgr->GetCycleInterval());
    if(NULL == mCycleManager)
    {
        ast_log_fatal("Out of Memory!");
    }

    for(cycletype = 0; cycletype < Constant::CYCLE_TYPE_CNT; cycletype++)
    {
        cyclesshare[cycletype] = 0;
    }

    cycles = configmgr->GetShortTermWindowCycles();
    cyclesshare[CycleManager::CT_learning] = cycles - 1;
    cyclesshare[CycleManager::CT_decision] = 1;
    mDecisionWindowManagers[DecisionWindowManager::DW_short_term] =
            new DecisionWindowManager(cycles, cyclesshare);
    if(NULL == mDecisionWindowManagers[DecisionWindowManager::DW_short_term])
    {
        ast_log_fatal("Out of Memory!");
    }

    mDecisionWindowManagers[DecisionWindowManager::DW_short_term]->SetWindowEnabled(ywb_true);
    for(cycletype = 0; cycletype < Constant::CYCLE_TYPE_CNT; cycletype++)
    {
        cyclesshare[cycletype] = 0;
    }

    cycles = configmgr->GetLongTermWindowCycles();
    cyclesshare[CycleManager::CT_learning] = cycles;
    cyclesshare[CycleManager::CT_decision] = 0;
    mDecisionWindowManagers[DecisionWindowManager::DW_long_term] =
            new DecisionWindowManager(cycles, cyclesshare);
    if(NULL == mDecisionWindowManagers[DecisionWindowManager::DW_long_term])
    {
        ast_log_fatal("Out of Memory!");
    }

    mDecisionWindowManagers[DecisionWindowManager::DW_long_term]->SetWindowEnabled(ywb_true);
    mCycleMotor = NULL;
    mParent = ast;
}

TimeManager::~TimeManager() {
    ywb_uint32_t windowindex = 0;

    if (mCycleManager) {
        delete mCycleManager;
        mCycleManager = NULL;
    }

    for (; windowindex < Constant::DECISION_WINDOW_CNT; windowindex++) {
        if (mDecisionWindowManagers[windowindex]) {
            delete mDecisionWindowManagers[windowindex];
            mDecisionWindowManagers[windowindex] = NULL;
        }
    }

    mParent = NULL;
}

//ywb_status_t
//TimeManager::RegisterDecisionWindow(ywb_uint32_t windowindex,
//        ywb_uint32_t cycles,  ywb_uint32_t *cycleshare)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//
//    YWB_ASSERT(windowindex < Constant::DECISION_WINDOW_CNT);
//    mDecisionWindowManagers[windowindex] =
//            new DecisionWindowManager(cycles, cycleshare);
//
//    YWB_ASSERT(NULL != mDecisionWindowManagers[windowindex]);
//    mDecisionWindowManagers[windowindex]->SetWindowEnabled(true);
//
//    return ret;
//}

void* TimeManager::entry()
{
    mCycleMotor->Start();
    return NULL;
}

ywb_status_t
TimeManager::StartMotor()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Time manager start motor");
    mCycleMotor = new Timer(this, (ywb_uint64_t)(mCycleManager->GetCyclePeriod()),
            (ywb_uint64_t)0);
    if(NULL == mCycleMotor)
    {
        ret = YFS_EOUTOFMEMORY;
        ast_log_fatal("Out of Memory!");
        return ret;
    }

    ret = create();
    return ret;
}

ywb_status_t
TimeManager::StopMotor()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Time manager stop cycle motor");
    if (mCycleMotor) {
        mCycleMotor->Stop();
        join();
        delete mCycleMotor;
        mCycleMotor = NULL;
    }

    return ret;
}

void
TimeManager::LaunchNewCycle()
{
    ywb_uint32_t windowindex = 0;
    ywb_uint32_t relative = 0;
    DecisionWindowManager* manager = NULL;
    AST* ast = NULL;
    Generator* gen = NULL;
    Executor* exe = NULL;
    Migration* mig = NULL;
    Migrator* curmigrator = NULL;

    ast_log_debug("Timer manager launch new cycle");
    mCycleManager->LaunchNewCycle();
    relative = mCycleManager->GetCycleRelative();

    for (; windowindex < Constant::DECISION_WINDOW_CNT; windowindex++) {
        if (mDecisionWindowManagers[windowindex]) {
            manager = mDecisionWindowManagers[windowindex];
            manager->LaunchNewCycle(relative);
            ast_log_debug("Decision window: " << windowindex
                    << " launch new cycle, current cycle type: "
                    << manager->GetCurCycleType());
        }
    }

    /*notify the external about decision cycle of short-term window*/
    manager = mDecisionWindowManagers[DecisionWindowManager::DW_short_term];
    if(manager)
    {
        GetAST(&ast);
        YWB_ASSERT(ast != NULL);
        ast->GetGenerator(&gen);
        YWB_ASSERT(gen != NULL);
        ast->GetExecutor(&exe);
        YWB_ASSERT(exe != NULL);
        ast->GetMigration(&mig);
        YWB_ASSERT(mig != NULL);
        mig->GetCurrentMigrator(&curmigrator);
        YWB_ASSERT(curmigrator != NULL);

        if((CycleManager::CT_decision ==  manager->GetCurCycleType()))
        {
            /*
             * re-collect logical configuration at first, and generate
             * advice after learning
             **/
            gen->LaunchCollectConfigEvent(true);
            /*notify the migrator to check any expired arbitratees*/
            curmigrator->LaunchCheckExpiredEvent();
        }
        else
        {
            gen->LaunchCollectConfigEvent(false);
            /*notify the migrator to check any expired arbitratees*/
            curmigrator->LaunchCheckExpiredEvent();
        }
    }
}

void
TimeManager::Reset()
{
    AST* ast = NULL;
    ConfigManager* configmgr = NULL;
    ywb_uint32_t cycles = 0;
    ywb_uint32_t cyclesshare[Constant::CYCLE_TYPE_CNT] = {0, 0};
    ywb_uint32_t cycletype = 0;

    GetAST(&ast);
    YWB_ASSERT(ast != NULL);
    ast->GetConfigManager(&configmgr);
    YWB_ASSERT(configmgr != NULL);

    ast_log_debug("TimeManager reset");
    YWB_ASSERT(mCycleManager != NULL);
    mCycleManager->SetCyclePeriod(configmgr->GetCycleInterval());
    mCycleManager->Reset();

    for(cycletype = 0; cycletype < Constant::CYCLE_TYPE_CNT; cycletype++)
    {
        cyclesshare[cycletype] = 0;
    }

    cycles = configmgr->GetShortTermWindowCycles();
    cyclesshare[CycleManager::CT_learning] = cycles - 1;
    cyclesshare[CycleManager::CT_decision] = 1;
    YWB_ASSERT(mDecisionWindowManagers[DecisionWindowManager::DW_short_term]);
    mDecisionWindowManagers[DecisionWindowManager::DW_short_term]->
        Reset(cycles, cyclesshare);

    for(cycletype = 0; cycletype < Constant::CYCLE_TYPE_CNT; cycletype++)
    {
        cyclesshare[cycletype] = 0;
    }

    cycles = configmgr->GetLongTermWindowCycles();
    cyclesshare[CycleManager::CT_learning] = cycles;
    cyclesshare[CycleManager::CT_decision] = 0;
    YWB_ASSERT(mDecisionWindowManagers[DecisionWindowManager::DW_long_term]);
    mDecisionWindowManagers[DecisionWindowManager::DW_long_term]->
        Reset(cycles, cyclesshare);
}

/* vim:set ts=4 sw=4 expandtab */
