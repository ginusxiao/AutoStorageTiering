#ifndef __AST_PLAN_HPP__
#define __AST_PLAN_HPP__

#include <list>
#include <map>
#include <queue>
#include <set>
#include "common/FsInts.h"
#include "common/FsAssert.h"
#include "common/FsStatus.h"
#include "FsCommon/YfsTime.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTUtil.hpp"
#include "OSSCommon/Logger.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "AST/ASTAdvice.hpp"
#include "AST/ASTPerformance.hpp"

using namespace std;

/*migration plan*/
class Plan {
public:
    /*migration direction*/
    enum Direction{
        /*promote to upper tier*/
        PD_ctr_promote = 0,
        /*demote to lower tier*/
        PD_ctr_demote,
        /*swap up*/
        PD_swap_up,
        /*swap down*/
        PD_swap_down,

        /*remember to update Constant::PLAN_DIR_CNT accordingly*/
        PD_cnt,
    };

    Plan() : mTargetId(YWB_UINT64_MAX), mStatType(IOStat::IOST_cnt),
            mCap(0), mDir(PD_cnt), mEpoch(0) {}
    Plan(OBJKey& objkey, ywb_uint64_t tgtid, SubPoolKey& subpoolkey,
            ChunkIOStat& iostat, ywb_uint32_t stattype, ywb_uint32_t cap,
            ywb_uint32_t direction, ywb_uint64_t epoch) : mKey(objkey),
                    mTargetId(tgtid), mSubPoolKey(subpoolkey),
                    mStat(iostat), mStatType(stattype), mCap(cap),
                    mDir(direction), mEpoch(epoch) {}

    inline void GetOBJKey(OBJKey** objkey)
    {
        *objkey = &mKey;
    }

    inline ywb_uint64_t GetTargetId()
    {
        return mTargetId;
    }

    inline void GetSubPoolKey(SubPoolKey** subpoolkey)
    {
        *subpoolkey = &mSubPoolKey;
    }

    inline void GetChunkIOStat(ChunkIOStat** stat)
    {
        *stat = &mStat;
    }

    inline ywb_uint32_t GetStatType()
    {
        return mStatType;
    }

    inline ywb_uint32_t GetCap()
    {
        return mCap;
    }

    inline ywb_uint32_t GetDirection()
    {
        return mDir;
    }

    inline ywb_uint64_t GetEpoch()
    {
        return mEpoch;
    }

    inline void SetOBJKey(OBJKey& objkey)
    {
        mKey = objkey;
    }

    inline void SetTargetId(ywb_uint64_t tgtid)
    {
        mTargetId = tgtid;
    }

    inline void SetSubPoolKey(SubPoolKey& subpoolkey)
    {
        mSubPoolKey = subpoolkey;
    }

    inline void SetChunkIOStat(ChunkIOStat& stat)
    {
        mStat = stat;
    }

    inline void SetStatType(ywb_uint32_t type)
    {
        mStatType = type;
    }

    inline void SetCap(ywb_uint32_t cap)
    {
        mCap = cap;
    }

    inline void SetDirection(ywb_uint32_t direction)
    {
        mDir = direction;
    }

    inline void SetEpoch(ywb_uint64_t epoch)
    {
        mEpoch = epoch;
    }

private:
    /*key*/
    OBJKey mKey;
    /*
     * target disk id for migration
     * can get target disk key via subpool key and target disk id
     * can get source disk key via subpool key and OBJ key
     **/
    ywb_uint64_t mTargetId;
    /*subpool key*/
    SubPoolKey mSubPoolKey;
    /*IO statistics, derived from Advice*/
    ChunkIOStat mStat;
    /*of which IOStat::Type the @mStat is, derived from Advice*/
    ywb_uint32_t mStatType;
    /*capacity requirement*/
    ywb_uint32_t mCap;
    /*plan direction*/
    ywb_uint32_t mDir;
    /*to which epoch this plan belongs*/
    ywb_uint64_t mEpoch;
};

class DiskScheStat{
public:
    enum Status{
        /*disk not scheduled*/
        DSSS_none = 0x0000,
        /*disk is chosen*/
        DSSS_chosen = 0x0001,
        /*if disk is unhealthy, it is excluded from being chosen*/
        DSSS_excluded = 0x0002,
        /*if disk is deleted in logical configuration*/
        DSSS_deleted = 0x0004,
    };

    DiskScheStat() : mDiskScheTimes(0) {
        mDiskScheStatus.SetFlagUniq(DiskScheStat::DSSS_none);
    }

    inline ywb_uint32_t GetDiskScheTimes()
    {
        return mDiskScheTimes;
    }

//    inline ywb_uint32_t GetDiskShceStatus()
//    {
//        return mDiskScheStatus;
//    }

    inline void SetDiskScheTimes(ywb_uint32_t val)
    {
        mDiskScheTimes = val;
    }

//    inline void SetDiskScheStatus(ywb_uint32_t val)
//    {
//        mDiskScheStatus |= val;
//    }
//
//    inline void ClearDiskScheStatus(ywb_uint32_t val)
//    {
//        mDiskScheStatus &= (~val);
//    }

    inline void SetFlagComb(ywb_uint32_t flag)
    {
        mDiskScheStatus.SetFlagComb(flag);
    }

    inline void SetFlagUniq(ywb_uint32_t flag)
    {
        mDiskScheStatus.SetFlagUniq(flag);
    }

    inline void ClearFlag(ywb_uint32_t flag)
    {
        mDiskScheStatus.ClearFlag(flag);
    }

    inline bool TestFlag(ywb_uint32_t flag)
    {
        return mDiskScheStatus.TestFlag(flag);
    }

    /*update disk's schedule times*/
    void UpdateDiskScheTimesByNum(ywb_uint32_t num, bool increase);

private:
    ywb_uint32_t mDiskScheTimes;
    Flag mDiskScheStatus;
};

class Node
{
public:
    friend class NodeCmp;
    Node(ywb_uint32_t direction, ywb_uint32_t priority) :
        mPlanDir(direction), mDynamicPriority(0) {}

    inline ywb_uint32_t GetPlanDirection()
    {
        return mPlanDir;
    }

    inline void SetPriority(ywb_uint32_t priority)
    {
        mDynamicPriority = priority;
    }

private:
    ywb_uint32_t mPlanDir;
    ywb_uint32_t mDynamicPriority;
};

class NodeCmp
{
public:
    bool operator()(const Node* nodea, const Node* nodeb) const;
};

class SubPoolPlanDirStat{
public:
    SubPoolPlanDirStat();
    virtual ~SubPoolPlanDirStat();

    inline ywb_uint32_t GetPlanDirStaticPriority(ywb_uint32_t direction)
    {
        YWB_ASSERT(0 <= direction && direction < Plan::PD_cnt);
        return mPlanDirStaticPriority[direction];
    }

    /*get current available plan types*/
    inline ywb_uint32_t GetCurrentAvailablePlanDirections()
    {
        return mCurAvailPlanDirs;
    }

    inline ywb_uint32_t GetLastMissedPlanDir()
    {
        return mLastMissedPlanDir;
    }

    /*
     * called whenever current plan direction hit or misses plan
     * if hit, call this routine with @direction set to Plan::PD_cnt,
     * otherwise, call this routine with the missed direction
     * */
    inline void SetLastMissedPlanDir(ywb_uint32_t direction)
    {
        ywb_uint32_t dir = 0;

        YWB_ASSERT(0 <= direction && direction <= Plan::PD_cnt);
        /*continuous missed plan direction*/
        if((mLastMissedPlanDir == direction) && (Plan::PD_cnt != direction))
        {
            mPlanDirPenalty[direction] += 1;
        }
        else
        {
            for(dir = 0; dir < Plan::PD_cnt; dir++)
            {
                (direction != dir) ? (mPlanDirPenalty[dir] = 0) : (direction);
            }
        }

        mLastMissedPlanDir = direction;
    }

    /*for test only*/
    inline ywb_uint32_t GetPriorityQueueSize()
    {
        return mPlanDirDynamicPriority.size();
    }

    /*initialize dynamic plan priority*/
    ywb_status_t InitDynamicPlanPriority();

    /*update plan direction schedule times*/
    void UpdatePlanDirScheTimes(ywb_uint32_t direction,
            ywb_uint32_t num, bool increase);

    /*update plan direction penanlty*/
    void UpdatePlanDirPenalty(ywb_uint32_t direction,
            ywb_uint32_t num, bool increase);

    /*called when current plan direction is satisfied*/
    void ResetLastMissedPlanDir();

    /*calculate dynamic priority*/
    ywb_uint32_t CalculateDynamicPriority(ywb_uint32_t direction);
    /*get the one with higher priority*/
    ywb_uint32_t GetHigherPlanDirection(
            ywb_uint32_t direction, ywb_uint32_t direction2);

    /*
     * AST will distinguish between global and local available
     * plan directions, so called global is SubPoolPlanDirStat::
     * mCurAvailPlanDirs, and local stands for available plan
     * directions within one schedule procedure, the local comes
     * from global at the beginning of one schedule, and update
     * during the schedule without updating the global one for
     * schedule will take disk availability into consideration,
     * one disk is not available at current schedule may become
     * available at successive schedule. Local is within one
     * schedule context, and global is within subpool wise overall
     * schedule context.
     *
     * temporarily exclude one plan direction from (local)
     * available plan directions
     * IN:
     *      availableset - available plan directions
     *      direction - to be excluded plan direction bit
     * OUT:
     *      remaining available plan directions
     **/
    ywb_uint32_t ExcludeDirectionFromAvailableDirections(
            ywb_uint32_t availableset, ywb_uint32_t direction);

    /*
     * permanently exclude one plan direction from (global)
     * available plan directions
     * IN:
     *      direction - to be excluded plan direction bit
     * OUT:
     *      remaining available plan directions
     **/
    ywb_uint32_t ExcludeDirectionFromAvailableDirections(
            ywb_uint32_t direction);

    /*get prior plan direction from given available plan directions*/
    virtual ywb_uint32_t GetPriorPlanDirectionFromAvailable(
            ywb_uint32_t availableset);
    /*get prior plan direction from current available plan directions*/
    virtual ywb_uint32_t GetPriorPlanDirectionFromAvailable();

    /*get advice type from given plan direction*/
    virtual ywb_uint32_t GetAdviceTypeFromPlanDirection(ywb_uint32_t direction);

    ywb_status_t Destroy();

private:
    /*how many times one plan direction has been scheduled*/
    ywb_uint32_t mPlanDirScheTimes[Constant::PLAN_DIR_CNT];
    /*
     * static priority of all plan directions, the smaller
     * the priority value, the higher the priority
     **/
    ywb_uint32_t mPlanDirStaticPriority[Constant::PLAN_DIR_CNT];
    /*
     * penalty is adopted when one plan direction missed, penalty rule
     * is as below:
     * a. penalty has same unit with schedule times, that is times
     * b. continuous missed plan direction will have more penalty, say
     *    previous penalty is 1, then current penalty will be 2
     * c. discontinuous missed plan direction will have penalty of 1
     **/
    ywb_uint32_t mPlanDirPenalty[Constant::PLAN_DIR_CNT];
//    /*is the plan direction exhausted*/
//    bool mPlanDirExhausted[Constant::PLAN_DIR_CNT];
    /*current available plan directions*/
    ywb_uint32_t mCurAvailPlanDirs;
    /*
     * last missed plan direction, REMEMBER to update it to Plan::PD_cnt
     * even there is no missing by invoking ResetLastMissedPlanDir
     **/
    ywb_uint32_t mLastMissedPlanDir;

    /*priority queue*/
    priority_queue<Node*, vector<Node*>, NodeCmp> mPlanDirDynamicPriority;
};

class SubPoolPlans {
public:
    enum {
        /*no more plans against this subpool*/
        SPF_no_more_plan = 0,
        /*
         * subpool in logical configuration already been deleted, BUT
         * remember that SubPoolPlans is not consistent with SubPool
         * in logical configuration, SubPoolPlans may not be deleted
         * though subpool in logical configuration has been deleted.
         **/
        SPF_subpool_deleted,
    };

    SubPoolPlans(SubPoolAdvice* subpooladvise) :
#if 0
        mAutoTieringScheduled(ywb_false), mSwapScheduled(ywb_false),
#endif
        mSubPoolAdvise(subpooladvise)
    {
        YWB_ASSERT(mSubPoolAdvise != NULL);
        mSubPoolAdvise->IncRef();
        mFlag.Reset();
    }

    ~SubPoolPlans()
    {
        Destroy();
    }

#if 0
    inline ywb_bool_t GetAutoTieringScheduled()
    {
        return mAutoTieringScheduled;
    }

    inline ywb_bool_t GetSwapScheduled()
    {
        return mSwapScheduled;
    }
#endif

    inline void GetSubPoolPlanDirStat(SubPoolPlanDirStat** stat)
    {
        *stat = &mPlanDirStat;
    }

    inline void GetSubPoolAdvise(SubPoolAdvice** subpooladvise)
    {
        *subpooladvise = mSubPoolAdvise;
    }

#if 0
    inline void SetAutoTieringScheduled(ywb_bool_t val)
    {
        mAutoTieringScheduled = val;
    }

    inline void SetSwapScheduled(ywb_bool_t val)
    {
        mSwapScheduled = val;
    }
#endif

    inline void SetFlagComb(ywb_uint32_t flag)
    {
        mFlag.SetFlagComb(flag);
    }

    inline void SetFlagUniq(ywb_uint32_t flag)
    {
        mFlag.SetFlagUniq(flag);
    }

    inline void ClearFlag(ywb_uint32_t flag)
    {
        mFlag.ClearFlag(flag);
    }

    inline bool TestFlag(ywb_uint32_t flag)
    {
        return mFlag.TestFlag(flag);
    }

    ywb_status_t AddPlan(Plan* plan);

    /*
     * NOTE:
     * Even in single thread environment, GetFirstPlan and GetNextPlan
     * have the same problem mentioned in SubPoolArbitratee(ASTArb.hpp)
     * , but here we can not ignore point (1) problem for plans are
     * dynamically added, so must restrict the usage of these two
     * functions: no deletion operation between GetFirstPlan and
     * GetNextPlan.
     **/
    ywb_status_t GetFirstPlan(Plan** plan);
    ywb_status_t GetNextPlan(Plan** plan);

    ywb_status_t Destroy();

private:
    /*flags*/
    Flag mFlag;
#if 0
    /*whether or not auto-tiering has been scheduled before*/
    ywb_bool_t mAutoTieringScheduled;
    /*whether or not swap has been scheduled before*/
    ywb_bool_t mSwapScheduled;
#endif
    /*list of all generated plans*/
    list<Plan*> mPlans;
    /*iterator used for traversing list of plans*/
    list<Plan*>::iterator mIt;
    /*plan direction statistics*/
    SubPoolPlanDirStat mPlanDirStat;
    /*
     * reference to advise
     *
     * SubPoolAdvice will be accessed every time scheduling a new plan,
     * but SubPoolAdvice will be reset before generating new SubPoolAdvice
     * for coming new decision window, so must make sure reset will not
     * de-construct SubPoolAdvice.
     *
     * SubPoolPlans will increase reference to SubPoolAdvice, and decrease
     * reference to it when plan manager reset SubPoolPlans.
     **/
    SubPoolAdvice* mSubPoolAdvise;
};

/*utility for time-based schedule*/
class TimeOut
{
public:
    friend class TimeOutCmp;
    TimeOut() : mStart(0), mInterval(0) {}

    TimeOut(ywb_time_t start, ywb_time_t interval) :
        mStart(start), mInterval(interval) {}

    TimeOut(const TimeOut& to)
    {
        mStart = to.mStart;
        mInterval = to.mInterval;
    }

    TimeOut& operator= (const TimeOut& to)
    {
        if(this == &to)
        {
            return *this;
        }

        mStart = to.mStart;
        mInterval = to.mInterval;

        return *this;
    }

    virtual ~TimeOut() {}

    inline void SetStart(ywb_time_t start)
    {
        mStart = start;
    }

    inline void SetInterval(ywb_time_t interval)
    {
        mInterval = interval;
    }

    ywb_bool_t IsTimedOut();

private:
    ywb_time_t mStart;
    ywb_time_t mInterval;
};

class TimeOutCmp {
public:
    bool operator()(const TimeOut& key1, const TimeOut& key2) const;
};

class PlanManager {
public:
    PlanManager(AST* ast)
    {
        YWB_ASSERT(ast != NULL);
        mAst = ast;
    }

    virtual ~PlanManager()
    {
        Destroy();
    }

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    ywb_status_t GetFirstSubPoolPlans(
            SubPoolPlans** subpool, SubPoolKey* key);
    ywb_status_t GetNextSubPoolPlans(
            SubPoolPlans** subpool, SubPoolKey* key);

    /*get subpool plans manager of given subpool*/
    ywb_status_t GetSubPoolPlans(SubPoolKey& subpoolkey,
            SubPoolPlans** subpool);
    /*remove subpool plans' manager*/
    ywb_status_t RemoveSubPoolPlans(SubPoolKey& subpoolkey);

    /*get disk schedule statistics of given disk*/
    ywb_status_t GetDiskScheStat(DiskKey& diskkey,
            DiskScheStat** diskstat);
    /*remove disk schedule statistics*/
    ywb_status_t RemoveDiskScheStat(DiskKey& diskkey);

    /*add chosen disk into unavailable disks*/
    ywb_status_t AddUnavailDisk(DiskKey& diskkey);
    /*remove free disk from unavailable disks*/
    ywb_status_t RemoveUnavailDisk(DiskKey& diskkey);

    /*add chosen disk into available disks set*/
    ywb_status_t AddAvailDisk(DiskKey& diskkey);
    /*remove specified disk from available disks set*/
    ywb_status_t RemoveAvailDisk(DiskKey& diskkey);

    /*
     * Choose first batch of plans of specified subpool.
     *
     * From perspective of efficiency, each disk should be involved
     * in at most one plan in this batch and the most "valuable"
     * extent be migrated.
     *
     * From perspective of fairness, all disks/plan types should
     * be involved.
     *
     * Take both disk parallism and disk availability into consideration,
     * from the point of disk parallism, choose as much disk as possible,
     * from the point of disk availability, one (source/target)disk pair
     * is involved in at most one plan.
     *
     * NOTE: will not check existence of generated plans internally, but
     * instead check in Arb module.
     **/
    virtual ywb_status_t ScheduleFirstBatch(
            SubPoolKey& subpoolkey, ywb_uint32_t* numofplan);
    /*auxiliary functions for ScheduleFirstBatch*/
    virtual ywb_status_t ScheduleNextFromSpecifiedPlanDirs(
            SubPoolKey& subpoolkey, ywb_uint32_t availplandirs);
    /*
     * schedule a new plan in given subpool, will not check the existence
     * of generated plans internally, but instead check in Arb module.
     **/
    virtual ywb_status_t ScheduleNext(SubPoolKey& subpoolkey);
//    /*schedule one specified subpool targeting at given disk*/
//    virtual ywb_status_t ScheduleNext(
//            SubPoolKey& subpoolkey, DiskKey& diskkey);

    /*time-based schedule first batch on specified subpool*/
    virtual ywb_status_t ScheduleFirstBatchTimedDelay(
            SubPoolKey& subpoolkey, ywb_time_t interval);
    /*time-based schedule next on specified subpool*/
    virtual ywb_status_t ScheduleNextTimedDelay(
            SubPoolKey& subpoolkey, ywb_time_t interval);
//    /*time-based schedule on specified subpool targeting at given disk*/
//    virtual ywb_status_t ScheduleNext(SubPoolKey& subpoolkey,
//            const DiskKey& diskkey, ywb_uint32_t timeout);

    /*
     * Currently ScheduleFirstBatch relies on disk's performance which is
     * variable, so if ScheduleFirstBatch fails, then try to schedule it
     * again at other time(when certain events happened), please refter to
     * DelayScheduleNext() for more about the mechanism
     **/
    virtual ywb_status_t ScheduleFirstBatchDelay(SubPoolKey& subpoolkey);
    /*
     * if ScheduleNext fails, then try to schedule it again at other
     * time(when certain events happened)
     *
     * this method should only be called when there is available advise
     * from which plans can be generated, now that there is available
     * advise, we are sure ScheduleNext fails for no favorite disk at
     * current, so DelayScheduleNext should only be "Waked Up" when there
     * is another completion event against PlanManager
     **/
    virtual ywb_status_t ScheduleNextDelay(SubPoolKey& subpoolkey);

    /*
     * schedule first batch of plans for time-based-delay-scheduled subpool
     * return the successfully scheduled subpool's key
     * */
    virtual ywb_status_t ScheduleFirstBatchForTimedDelayed(
            SubPoolKey* subpoolkey, ywb_uint32_t* numofplan);
    /*
     * schedule next plan for time-based-delay-scheduled subpool
     * return the successfully scheduled subpool's key
     * */
    virtual ywb_status_t ScheduleNextForTimedDelayed(SubPoolKey* subpoolkey);
    /*
     * schedule first batch of plans for delay-scheduled subpool
     * return the successfully scheduled subpool's key
     * */
    virtual ywb_status_t ScheduleFirstBatchForDelayed(
            SubPoolKey* subpoolkey, ywb_uint32_t* numofplan);
    /*
     * schedule next plan for delay-scheduled subpool
     * return the successfully scheduled subpool's key
     * */
    virtual ywb_status_t ScheduleNextForDelayed(SubPoolKey* subpoolkey);

    /*schedule first-batch/next plan for delayed/time-based-delayed subpools*/
    virtual ywb_status_t ScheduleDelayed();
    /*remove given subpool from delayed/timed-delayed subpools*/
    void RemoveSubPoolFromDelayed(SubPoolKey& subpoolkey);

    /*
     * check whether given subpool in time-based delayed-subpools-
     * for-first-batch set, for test only
     * */
    ywb_bool_t CheckInTimedDelayedSubPoolsForFirstBatch(SubPoolKey& key);
    /*
     * check whether given subpool in time-based delayed-subpools-
     * for-next set, for test only
     * */
    ywb_bool_t CheckInTimedDelayedSubPoolsForNext(SubPoolKey& key);
    /*
     * check whether given subpool in delayed-subpools-for-first-batch
     * set, for test only
     * */
    ywb_bool_t CheckInDelayedSubPoolsForFirstBatch(SubPoolKey& key);
    /*
     * check whether given subpool in delayed-subpools-for-next set,
     * for test only
     * */
    ywb_bool_t CheckInDelayedSubPoolsForNext(SubPoolKey& key);

    /*
     * called when fail to submit one plan.
     * DO NOT delete the failed plan here.
     **/
    void OnFailToSubmit(SubPoolPlans* subpoolplans, Plan* plan);
    /*submit first batch of plan to ARB*/
    ywb_status_t SubmitFirstBatch(SubPoolKey& subpoolkey);
    /*submit next plan to ARB*/
    ywb_status_t SubmitNext(SubPoolKey& subpoolkey);

    /*
     * called upon one plan corresponding to given subpool and disk is
     * successfully migrated.
     **/
    ywb_status_t Complete(SubPoolKey& subpoolkey, DiskKey& sourcekey,
            DiskKey& targetkey, ywb_bool_t triggermore, ywb_status_t migstatus);
    /*
     * called when one plan expired
     * */
    ywb_status_t Expire(SubPoolKey& subpoolkey, DiskKey& sourcekey,
                DiskKey& targetkey, ywb_bool_t triggermore);

    /*destroy given disk's disk schedule statistics*/
    ywb_status_t Destroy(DiskKey& diskkey);
    /*destroy give subpool's plans as well as related disk schedule statistics*/
    ywb_status_t Destroy(SubPoolKey& subpoolkey);
    /*destroy whole*/
    ywb_status_t Destroy();

    /*
     * when new decision cycle comes, new advice/plan(for current decision
     * window) will be generated, so current plans(for last decision window)
     * in @mPlans will be discarded.
     *
     * @all:
     *      reset all fields if set to ywb_true;
     *      retain some fields for future use otherwise;
     **/
    ywb_status_t Reset(ywb_bool_t all);
    /*prepare for the new plans generation*/
    ywb_status_t Init();

private:
    /*manage all the generated plans*/
    map<SubPoolKey, SubPoolPlans*, SubPoolKeyCmp> mPlans;
    /*iterator used for traversing subpool plans*/
    map<SubPoolKey, SubPoolPlans*, SubPoolKeyCmp>::iterator mIt;
    /*shchedule statistics of all disks*/
    map<DiskKey, DiskScheStat*, DiskKeyCmp> mDiskScheStat;
    /*accomodate all time-based-delay-schedule-first-batch subpools*/
    map<TimeOut, SubPoolKey, TimeOutCmp> mTimedDelayedSubPoolsForFirstBatch;
    /*accomodate all time-based-delay-schedule-next subpools*/
    map<TimeOut, SubPoolKey, TimeOutCmp> mTimedDelayedSubPoolsForNext;
    /*accomodate all delay-schedule-first-batch subpools*/
    list<SubPoolKey> mDelayedSubPoolsForFirstBatch;
    /*accomodate all delay-schedule-next subpools*/
    list<SubPoolKey> mDelayedSubPoolsForNext;
    /*
     * manage all chosen/excluded disks, chosen disks may
     * become free and available again, excluded disks
     * will be permanently unavailable
     **/
    BlackList mUnavailDisks;
    /*
     * manage all free disks, free disks my be moved out
     * if it is chosen for migration again
     *
     * available disks should be initialized to all disks
     * other than those excluded disks at the beginning of
     * Plan Module, BUT for simplicity, one disk will be
     * added into available disks set when a migration
     * complete.
     **/
    WhiteList mAvailDisks;
    /*reference to AST*/
    AST* mAst;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
