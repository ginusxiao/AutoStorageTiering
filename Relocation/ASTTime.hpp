#ifndef __AST_TIME_HPP__
#define __AST_TIME_HPP__

#include <time.h>
#include <event2/event.h>
#include "common/FsInts.h"
#include "common/FsStatus.h"
#include "FsCommon/YfsTimerArg.h"
#include "FsCommon/YfsThread.h"
#include "OSSCommon/Logger.hpp"
#include "Communication/NetServer.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTUtil.hpp"
#include "AST/AST.hpp"

class CycleManager {
public:
    enum {
        CF_alive = 0,
        CF_abort,
        CF_reset_heat,
    };

    enum CycleType{
        CT_learning = 0x00000000,
        CT_decision = 0x00000001,
        CT_cnt,
    };

public:
    CycleManager(ywb_uint32_t period);

    inline ywb_uint32_t GetCyclePeriod();
    inline ywb_uint64_t GetCycleRelative();
    inline ywb_uint64_t GetCycleAbsolute();
    inline ywb_uint64_t GetCycleAstEnabled();

    inline void SetCyclePeriod(ywb_uint32_t val);
    inline void SetCycleRelative(ywb_uint64_t val);
    inline void SetCycleAbsolute(ywb_uint64_t val);
    inline void SetCycleAstEnabled(ywb_uint64_t val);

    inline void SetCycleFlagComb(ywb_uint32_t flag);
    inline void SetCycleFlagUniq(ywb_uint32_t flag);
    inline void ClearCycleFlag(ywb_uint32_t flag);
    inline bool TestCycleFlag(ywb_uint32_t flag);

    void LaunchNewCycle();
    void Reset();

private:
    /*current cycle flags*/
    Flag mCycleFlag;
    /*how long a cycle lasts for(in seconds)*/
    ywb_uint32_t mCyclePeriod;
    /*relative to enable cycle*/
    ywb_uint64_t mCurCycleRelative;
    /*calculated cycle number directly from NTP time*/
    ywb_uint64_t mCurCycleAbsolute;
    /*in which (absolute) cycle the AST is enabled*/
    ywb_uint64_t mCycleASTEnabled;
};

class DecisionWindowManager {
public:
    enum DecisionWindow{
        DW_short_term = 0,
        DW_long_term,
        DW_cnt,
    };

    /*
     * @cycleshare specify how much cycles one cycle type owns,
     * it must contain exactly Constant::CYCLE_TYPE_CNT elements
     **/
    DecisionWindowManager(ywb_uint32_t cycles, ywb_uint32_t *cycleshare);

    inline ywb_uint32_t GetCycles();
    inline ywb_uint32_t GetCyclesPlusOneSquare();
    inline ywb_uint32_t GetMultiplierOne();
    inline ywb_uint32_t GetMultiplierTwo();
    inline ywb_uint32_t GetCurCycleType();
    inline ywb_bool_t GetWindowEnabled();
    inline ywb_bool_t IsNewWindowStarted();
    /*
     * SetCycles and SetCycleShare are used for change the
     * setting of one decision window, pay attention to
     * following two points when using them:
     * 1. they should be used together;
     * 2. they can only be used after initializing TimeManager
     *    but before TimeManager running;
     **/
    inline void SetCycles(ywb_uint32_t val);
    inline void SetCycleShare(ywb_uint32_t *share, ywb_uint32_t arraysize);
    inline void SetCurCycleType(ywb_uint32_t val);
    inline void SetWindowEnabled(ywb_bool_t val);

    ywb_uint32_t FindNextCaredCycleType(ywb_uint32_t from);
    void LaunchNewCycle(ywb_uint32_t relativecycle);
    /*re-initialization*/
    void Reset(ywb_uint32_t cycles, ywb_uint32_t* cycleshare);

private:
    /*how many cycles one decision window lasts for*/
    ywb_uint32_t mCycles;
    /*constants used for EMA calculation*/
    ywb_uint32_t mCyclesPlusOneSquare;
    ywb_uint32_t mMultiplierOne;
    ywb_uint32_t mMultiplierTwo;

    /*
     * how many cycles shared by each cycle type, if one cycle
     * type's share equals to 0, it means this cycle type is not
     * supported by this decision window
     **/
    ywb_uint32_t mCyclsShareOfEachCycleType[Constant::CYCLE_TYPE_CNT];
    /*current cycle type*/
    ywb_uint32_t mCurCycleType;
    /*
     * this window is enabled or disabled, decision window is
     * enabled only if it is registered
     **/
    ywb_bool_t mEnabled;
    /*whether current cycle is the first cycle of newly started window*/
    ywb_bool_t mIsNewWindowStarted;
};

class TimeManager;
class Timer : public Server{
public:
    enum {
        TIMER_NONE = 0, TIMER_CYCLE_LAUNCHER,
    };

    typedef void (*timeHandler)(int fd, short what, void* arg);

    Timer(TimeManager* timemanager, ywb_uint64_t intervalsec,
            ywb_uint64_t intervalusec);
    virtual ~Timer();

    ywb_status_t Start();
    ywb_status_t Stop();

    /* Inherit from Server*/
    ywb_status_t initialize();
    /* Inherit from Server*/
    void handleevent();
    void Reset();

private:
    struct event* mInterest;
    /*back reference to TimeManager*/
    TimeManager* mTimeManager;
    ywb_uint64_t mCycleIntervalSec;
    ywb_uint64_t mCycleIntervalUsec;
    /*used for indicating the timer should run or stop*/
    ywb_bool_t mRunning;
};

class TimeManager : public Thread {
public:
    TimeManager(AST* ast);
    /*TimeManager(AST* ast, ywb_uint32_t cycleperiod);*/
    virtual ~TimeManager();

    inline void GetCycleManager(CycleManager** mgr);
    inline void GetDecisionWindow(ywb_uint32_t windowindex,
            DecisionWindowManager** mgr);
    inline void GetAST(AST** ast);

//    /*
//     * register specified decision window
//     * @cycleshare specify how many cycles one cycle type shares
//     **/
//    ywb_status_t RegisterDecisionWindow(ywb_uint32_t windowindex,
//            ywb_uint32_t cycles, ywb_uint32_t *cycleshare);

    void* entry();
    ywb_status_t StartMotor();
    ywb_status_t StopMotor();

    virtual void LaunchNewCycle();
    void Reset();

private:
    CycleManager* mCycleManager;
    DecisionWindowManager* mDecisionWindowManagers[Constant::DECISION_WINDOW_CNT];
    Timer* mCycleMotor;
    AST* mParent;
};

#include "AST/ASTTimeInline.hpp"

#endif

/* vim:set ts=4 sw=4 expandtab */
