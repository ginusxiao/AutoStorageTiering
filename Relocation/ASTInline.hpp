#ifndef __AST_INLINE_HPP__
#define __AST_INLINE_HPP__

#include "AST/AST.hpp"

void
AST::IncRef()
{
    fs_atomic_inc(&mRef);
}

void
AST::DecRef()
{
    if(fs_atomic_dec_and_test(&mRef))
    {
        delete this;
    }
}

void
AST::GetError(Error** err)
{
    *err = mErr;
}

void
AST::GetConfigManager(ConfigManager** configmgr)
{
    *configmgr = mConfig;
}

void
AST::GetTimeManager(TimeManager** timemgr)
{
    *timemgr = mTime;
}

void
AST::GetPerformanceProfile(PerfProfileManager** profile)
{
    *profile = mProfile;
}

void
AST::GetLogicalConfig(LogicalConfig** logicalconfig)
{
    *logicalconfig = mLogicalConfig;
}

void
AST::GetIOManager(IOManager** iomgr)
{
    *iomgr = mIO;
}

#ifdef WITH_INITIAL_PLACEMENT
void
AST::GetSwapOutManager(SwapOutManager** swapout)
{
    *swapout = mSwapOut;
}
#endif

void
AST::GetHeatDistributionManager(
        HeatDistributionManager** heatdistribution)
{
    *heatdistribution = mHeatDistribution;
}

void
AST::GetDiskUltimatePerfCache(DiskUltimatePerfCache** cache)
{
    *cache = mDiskUltimatePerfCache;
}

void
AST::GetAdviseManager(AdviceManager** advise)
{
    *advise = mAdvise;
}

void
AST::GetPerfManager(PerfManager** perf)
{
    *perf = mPerf;
}

void
AST::GetPlanManager(PlanManager** plan)
{
    *plan = mPlan;
}

void
AST::GetArb(Arbitrator**arb)
{
    *arb = mArb;
}

void
AST::GetMigration(Migration** migration)
{
    *migration = mMigration;
}

ywb_status_t
AST::GetRelocationTask(ywb_uint32_t tasktype,
        RelocationTask** reloaction)
{
    ywb_status_t ret = YWB_SUCCESS;

    *reloaction = mRelocationTasks[tasktype];
    return ret;
}

//void AST::GetHUB(HUB** hub)
//{
//    *hub = mHUB;
//}

void AST::GetControlCenter(ControlCenter** center)
{
    *center = mCtrlCenter;
}

void AST::GetMessageManager(MessageManager** msgmgr)
{
    *msgmgr = mMsgMgr;
}

void AST::GetGenerator(Generator** gen)
{
    *gen = mGen;
}

void AST::GetExecutor(Executor** exe)
{
    *exe = mExe;
}

ywb_uint32_t AST::GetState()
{
    return mState;
}

void
AST::SetConfigManager(ConfigManager*& config)
{
    mConfig = config;
}

void
AST::SetTimeManager(TimeManager*& time)
{
    mTime = time;
}

void
AST::SetPerformanceProfile(PerfProfileManager*& profile)
{
    mProfile = profile;
}

void
AST::SetLogicalConfig(LogicalConfig*& logicalconfig)
{
    mLogicalConfig = logicalconfig;
}

void
AST::SetIOManager(IOManager*& io)
{
    mIO = io;
}

#ifdef WITH_INITIAL_PLACEMENT
void
AST::SetSwapOutManager(SwapOutManager*& swapout)
{
    mSwapOut = swapout;
}
#endif

void
AST::SetHeatDistributionManager(
        HeatDistributionManager*& heatdistribution)
{
    mHeatDistribution = heatdistribution;
}

void
AST::SetDiskUltimatePerfCache(DiskUltimatePerfCache* cache)
{
    mDiskUltimatePerfCache = cache;
}

void
AST::SetAdviseManager(AdviceManager*& advise)
{
    mAdvise = advise;
}

void
AST::SetPerfManager(PerfManager*& perf)
{
    mPerf = perf;
}

void
AST::SetPlanManager(PlanManager*& plan)
{
    mPlan = plan;
}

void
AST::SetArb(Arbitrator*& arb)
{
    mArb = arb;
}

void
AST::SetMigration(Migration*& migration)
{
    mMigration = migration;
}

//void
//AST::SetHUB(HUB* hub)
//{
//    mHUB = hub;
//}

void
AST::SetControlCenter(ControlCenter* center)
{
    mCtrlCenter = center;
}

void
AST::SetMessageManager(MessageManager* msgmgr)
{
    mMsgMgr = msgmgr;
}

void AST::SetState(ywb_uint32_t state)
{
    mState = state;
}

#endif

/* vim:set ts=4 sw=4 expandtab */
