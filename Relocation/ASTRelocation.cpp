#include "OSSCommon/Logger.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "AST/ASTSwapOut.hpp"
#include "AST/ASTHeatDistribution.hpp"
#include "AST/ASTAdvice.hpp"
#include "AST/ASTPerformance.hpp"
#include "AST/ASTPlan.hpp"
#include "AST/ASTArb.hpp"
#include "AST/ASTMigration.hpp"
#include "AST/AST.hpp"
#include "AST/MessageImpl.hpp"
#include "AST/ASTSwapOut.hpp"
#include "AST/ASTRelocation.hpp"

RelocationTask::RelocationTask(AST* ast)
{
    YWB_ASSERT(ast != NULL);
    mAst = ast;
}

RelocationTask::~RelocationTask()
{
    mAst = NULL;
}

ywb_status_t
RelocationTask::Prepare()
{
    return YWB_SUCCESS;
}

//ywb_status_t
//RelocationTask::DoCycleWork(ywb_uint32_t phase)
//{
//    return YWB_SUCCESS;
//}

ywb_status_t
RelocationTask::DoCycleWork()
{
    return YWB_SUCCESS;
}

RelocationLearning::RelocationLearning(AST* ast) :
        RelocationTask(ast), mAdviceAfterLearning(false)
{
    GetAST(&ast);
    ast->GetLogicalConfig(&mConfigManager);
    ast->GetIOManager(&mIOManager);
#ifdef WITH_INITIAL_PLACEMENT
    ast->GetSwapOutManager(&mSwapOutManager);
    YWB_ASSERT(mSwapOutManager != NULL);
#endif
    YWB_ASSERT(mConfigManager != NULL);
    YWB_ASSERT(mIOManager != NULL);
    mLearningEvent = NULL;
}

RelocationLearning::~RelocationLearning()
{
    mConfigManager = NULL;
    mIOManager = NULL;
    mLearningEvent = NULL;
#ifdef WITH_INITIAL_PLACEMENT
    mSwapOutManager = NULL;
#endif
}

ywb_status_t
RelocationLearning::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Learning task reset");
    mConfigManager->Reset();
    mIOManager->Reset();
#ifdef WITH_INITIAL_PLACEMENT
    mSwapOutManager->Reset();
#endif
    return ret;
}

ywb_status_t
RelocationLearning::MarkLogicalConfigAsSuspicious()
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    SubPoolKey subpoolkey;
    ywb_uint32_t tiertype = Tier::TIER_cnt;

    /*mark all subpools/disks/OBJs as suspicious*/
    ret = mConfigManager->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        subpool->SetFlagComb(SubPool::SPF_suspicious);
        tiertype = Tier::TIER_0;
        for(; tiertype < Tier::TIER_cnt; tiertype++)
        {
            ret = subpool->GetTier(tiertype, &tier);
            if((YWB_SUCCESS == ret) && (tier != NULL))
            {
                ret = tier->GetFirstDisk(&disk);
                while((YWB_SUCCESS == ret) && (disk != NULL))
                {
                    disk->SetFlagComb(Disk::DF_suspicious);
                    ret = disk->GetFirstOBJ(&obj);
                    while((YWB_SUCCESS == ret) && (obj != NULL))
                    {
                        obj->SetFlagComb(OBJ::OF_suspicious);
                        mConfigManager->PutOBJ(obj);
                        ret = disk->GetNextOBJ(&obj);
                    }

                    tier->PutDisk(disk);
                    ret = tier->GetNextDisk(&disk);
                }

                subpool->PutTier(tier);
            }
        }

        mConfigManager->PutSubPool(subpool);
        ret = mConfigManager->GetNextSubPool(&subpool, &subpoolkey);
    }

    return ret;
}

#if 0
ywb_status_t
RelocationLearning::RemoveUnConfirmedSuspect()
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;
    OBJVal* obj = NULL;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    OBJKey objkey;
    ywb_uint32_t tiertype = Tier::TIER_cnt;

    /*remove those subpools/disks/OBJs have not been confirmed*/
    ret = mConfigManager->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        if(subpool->TestFlag(SubPool::SPF_suspicious))
        {
            mConfigManager->RemoveSubPool(subpoolkey);
        }
        else
        {
            tiertype = Tier::TIER_0;
            for(; tiertype < Tier::TIER_cnt; tiertype++)
            {
                ret = subpool->GetTier(tiertype, &tier);
                if(!((YWB_SUCCESS == ret) && (tier != NULL)))
                {
                    continue;
                }

                ret = tier->GetFirstDisk(&disk, &diskkey);
                while((YWB_SUCCESS == ret) && (disk != NULL))
                {
                    if(disk->TestFlag(Disk::DF_suspicous))
                    {
                        subpool->RemoveDisk(diskkey);
                    }
                    else
                    {
                        ret = disk->GetFirstOBJ(&obj, &objkey);
                        while((YWB_SUCCESS == ret) && (obj != NULL))
                        {
                            if(obj->TestFlag(OBJ::OF_suspicious))
                            {
                                disk->RemoveOBJ(objkey);
                            }

                            mConfigManager->PutOBJ(obj);
                            ret = disk->GetNextOBJ(&obj, &objkey);
                        }
                    }

                    mConfigManager->PutDisk(disk);
                    ret = tier->GetNextDisk(&disk, &diskkey);
                }

                subpool->PutTier(tier);
            }
        }

        mConfigManager->PutSubPool(subpool);
        ret = mConfigManager->GetNextSubPool(&subpool, &subpoolkey);
    }

    return ret;
}
#endif

ywb_status_t
RelocationLearning::RemoveUnConfirmedSuspect()
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;
    OBJVal* obj = NULL;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    OBJKey objkey;
    ywb_uint32_t tiertype = Tier::TIER_cnt;

    /*remove those subpools/disks/OBJs have not been confirmed*/
    ret = mConfigManager->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        if(subpool->TestFlag(SubPool::SPF_suspicious))
        {
            mConfigManager->AddDeletedSubPool(subpoolkey);
        }
        else
        {
            tiertype = Tier::TIER_0;
            for(; tiertype < Tier::TIER_cnt; tiertype++)
            {
                ret = subpool->GetTier(tiertype, &tier);
                if(!((YWB_SUCCESS == ret) && (tier != NULL)))
                {
                    continue;
                }

                ret = tier->GetFirstDisk(&disk, &diskkey);
                while((YWB_SUCCESS == ret) && (disk != NULL))
                {
                    if(disk->TestFlag(Disk::DF_suspicious))
                    {
                        mConfigManager->AddDeletedDisk(diskkey);
                    }
                    else
                    {
                        ret = disk->GetFirstOBJ(&obj, &objkey);
                        while((YWB_SUCCESS == ret) && (obj != NULL))
                        {
                            if(obj->TestFlag(OBJ::OF_suspicious))
                            {
                                mConfigManager->AddDeletedOBJ(objkey);
                            }

                            mConfigManager->PutOBJ(obj);
                            ret = disk->GetNextOBJ(&obj, &objkey);
                        }
                    }

                    mConfigManager->PutDisk(disk);
                    ret = tier->GetNextDisk(&disk, &diskkey);
                }

                subpool->PutTier(tier);
            }
        }

        mConfigManager->PutSubPool(subpool);
        ret = mConfigManager->GetNextSubPool(&subpool, &subpoolkey);
    }

    return ret;
}

ywb_status_t
RelocationLearning::Consolidate()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Learning task consolidate");
    mConfigManager->Consolidate();
    mIOManager->Consolidate();
#ifdef WITH_INITIAL_PLACEMENT
    mSwapOutManager->Consolidate();
#endif

    return ret;
}

ywb_status_t
RelocationLearning::Prepare()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Learning task preparation");
    MarkLogicalConfigAsSuspicious();

    return ret;
}

ywb_status_t
RelocationLearning::DoCycleWork()
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Generator* gen = NULL;
    Executor* exe = NULL;
    GeneratorEventSet::LearningEvent* learningevent = NULL;
    void* resp = NULL;
    CollectOBJResponse* collectobjresponse = NULL;
    ywb_uint32_t flag = 0;

    learningevent = mLearningEvent;
    YWB_ASSERT(learningevent != NULL);
    GetAST(&ast);
    ast->GetGenerator(&gen);
    ast->GetExecutor(&exe);

    ast_log_debug("Learning task cyclic work, phase: "
            << learningevent->GetPhase()
            << "[P_collect_logical_config: 0,  P_resolve_logical_config: 1,"
            << " P_collect_OBJ_io: 2, P_resolve_OBJ_io : 3,"
            << " P_resolve_deleted_OBJs: 4, P_resolve_deleted_disks : 5,"
            << " P_resolve_deleted_subpools: 6]");

    switch(learningevent->GetPhase())
    {
    case Generator::P_collect_logical_config:
        /*prepare for collecting logical config and OBJ IO in new cycle*/
        Prepare();
        gen->SetStatus(Generator::S_learning);
        mAdviceAfterLearning = learningevent->GetAdviceAfterLearning();
        ret = mConfigManager->CollectLogicalConfig(Constant::OSS_ID);

        break;
    case Generator::P_resolve_logical_config:
        YWB_ASSERT(Generator::S_learning == gen->GetStatus());
        learningevent->GetResp(&resp);
        ret = mConfigManager->ResolveLogicalConfig(
                (CollectLogicalConfigResponse*)resp);
        if(YWB_SUCCESS == ret)
        {
            mIOManager->CollectOBJIOPrep(Constant::OSS_ID);
            /*directly call CollectOBJIO() instead of launch an event*/
            ret = mIOManager->CollectOBJIO(Constant::OSS_ID);
        }
        else
        {
            /*
             * FIXME: here will not call RemoveUnConfirmedSuspect, NOOP
             * is enough for both Generator and Executor
             * */
            gen->SetStatus(Generator::S_noop);
            exe->SetStatus(Executor::S_noop);
            gen->HandlePendingEvent();
        }

        break;
    case Generator::P_resolve_OBJ_io:
        YWB_ASSERT(Generator::S_learning == gen->GetStatus());
        learningevent->GetResp(&resp);
        collectobjresponse = (CollectOBJResponse*)resp;
        YWB_ASSERT(collectobjresponse != NULL);
        if((YWB_SUCCESS == collectobjresponse->GetStatus()) &&
                (Generator::S_noop != gen->GetStatus()))
        {
            flag = collectobjresponse->GetFlag();
            if(CollectOBJResponse::F_next_batch == flag)
            {
                mIOManager->CollectOBJIO(Constant::OSS_ID);
            }

            ret = mIOManager->ResolveOBJIO(collectobjresponse);
            if((YWB_SUCCESS == ret) && (CollectOBJResponse::F_end == flag))
            {
                mIOManager->CollectOBJIOPost(Constant::OSS_ID);
                RemoveUnConfirmedSuspect();

                if(true == mAdviceAfterLearning)
                {
                    gen->SetStatus(Generator::S_advising);
                    /*issue stall event*/
                    exe->LaunchStallEvent();
                }
                else
                {
                    gen->SetStatus(Generator::S_learning_done);
                    gen->HandlePendingEvent();
                }
            }
            else if(YWB_SUCCESS != ret)
            {
                mIOManager->CollectOBJIOPost(Constant::OSS_ID);
                /*
                 * FIXME: here will not call RemoveUnConfirmedSuspect, NOOP
                 * is enough for both Generator and Executor
                 * */
                gen->SetStatus(Generator::S_noop);
                exe->SetStatus(Executor::S_noop);
                gen->HandlePendingEvent();
            }
        }
        else
        {
            collectobjresponse->DecRef();
            if(Generator::S_noop != gen->GetStatus())
            {
                mIOManager->CollectOBJIOPost(Constant::OSS_ID);
            }
        }

        break;
    case Generator::P_resolve_deleted_OBJs:
        learningevent->GetResp(&resp);
        ret = mConfigManager->ResolveDeletedOBJs(
                (ReportDeleteOBJsMessage*)resp);

        break;
    case Generator::P_resolve_deleted_disks:
        learningevent->GetResp(&resp);
        ret = mConfigManager->ResolveDeletedDisks(
                (ReportDeleteDisksMessage*)resp);

        break;
    case Generator::P_resolve_deleted_subpools:
        learningevent->GetResp(&resp);
        ret = mConfigManager->ResolveDeletedSubPools(
                (ReportDeleteSubPoolsMessage*)resp);

        break;
    case Generator::P_resolve_deleted_OBJ:
        learningevent->GetResp(&resp);
        ret = mConfigManager->ResolveDeletedOBJ(
                (ReportDeleteOBJMessage*)resp);

        break;
    case Generator::P_resolve_deleted_disk:
        learningevent->GetResp(&resp);
        ret = mConfigManager->ResolveDeletedDisk(
                (ReportDeleteDiskMessage*)resp);

        break;
    case Generator::P_resolve_deleted_subpool:
        learningevent->GetResp(&resp);
        ret = mConfigManager->ResolveDeletedSubPool(
                (ReportDeleteSubPoolMessage*)resp);

        break;
#ifdef WITH_INITIAL_PLACEMENT
    case Generator::P_resolve_relocation_type:
        learningevent->GetResp(&resp);
        /*swap out manager resolves its own part only*/
        ret = mSwapOutManager->ResolveRelocationType(
                (NotifyRelocationType*)resp);
        /*logical config resolves its own part*/
        ret = mConfigManager->ResolveRelocationType(
                (NotifyRelocationType*)resp);

        break;
    case Generator::P_resolve_swap_requirement:
        learningevent->GetResp(&resp);
        ret = mSwapOutManager->ResolveSwapRequirement(
                (NotifySwapRequirement*)resp);

        break;
    case Generator::P_resolve_swap_cancellation:
        learningevent->GetResp(&resp);
        ret = mSwapOutManager->ResolveSwapCancellation(
                (NotifySwapCancellation*)resp);

        break;
#endif
    default:
        YWB_ASSERT(0);
        break;
    }

    return ret;
}

RelocationCTR::RelocationCTR(AST* ast) : RelocationTask(ast)
{
    GetAST(&ast);
    ast->GetAdviseManager(&mAdviceManager);
    ast->GetHeatDistributionManager(&mHeatDistributionManager);
    YWB_ASSERT(mAdviceManager != NULL);
    YWB_ASSERT(mHeatDistributionManager != NULL);
}

RelocationCTR::~RelocationCTR()
{
    mAdviceManager = NULL;
    mHeatDistributionManager = NULL;
}

ywb_status_t
RelocationCTR::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("CTR task reset");
    mAdviceManager->Reset();
    mHeatDistributionManager->Reset();

    return ret;
}

ywb_status_t
RelocationCTR::Init()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("CTR task init");
    mAdviceManager->Init();
    mHeatDistributionManager->Init();

    return ret;
}

ywb_status_t
RelocationCTR::Prepare()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("CTR task prepare");
    Reset();
    Init();

    return ret;
}

ywb_status_t
RelocationCTR::DoCycleWork()
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SwapOutManager* swapoutmgr = NULL;
    SubPool* subpool = NULL;
    SubPoolKey subpoolkey;
    AdviceRule* hotadvicerule = NULL;
    AdviceRule* coldadvicerule = NULL;
    AdviceRule* hotfilterrule = NULL;
    AdviceRule* coldfilterrule = NULL;
#ifdef WITH_INITIAL_PLACEMENT
    AdviceRule* swapuprule = NULL;
    AdviceRule* swapdownrule = NULL;
    AdviceRule* swapupfilterrule = NULL;
    AdviceRule* swapdownfilterrule = NULL;
#endif

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
#ifdef WITH_INITIAL_PLACEMENT
    ast->GetSwapOutManager(&swapoutmgr);
#endif

    ast_log_debug("CTR task cyclic work");
    /*rule for CTR hot advice*/
    hotadvicerule = new AdviceRule(AdviceType::AT_ctr_hot,
            PartitionScope::PS_tier_wise,
            PartitionBase::PB_long_term | PartitionBase::PB_ema |
            PartitionBase::PB_random | PartitionBase::PB_rt,
            SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_iops,
            Selector::SEL_from_larger);
    /*rule for CTR cold advice*/
    coldadvicerule = new AdviceRule(AdviceType::AT_ctr_cold,
            PartitionScope::PS_tier_wise,
            PartitionBase::PB_long_term | PartitionBase::PB_ema |
            PartitionBase::PB_sequential | PartitionBase::PB_rt,
            SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_iops,
            Selector::SEL_from_larger);
#if 0
    /*filter rule for CTR cold advice*/
    coldfilterrule = new AdviceRule(AdviceType::AT_ctr_cold,
            PartitionScope::PS_tier_wise,
            PartitionBase::PB_long_term | PartitionBase::PB_ema |
            PartitionBase::PB_random | PartitionBase::PB_rt,
            SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_iops,
            Selector::SEL_from_smaller);
#endif
    /*rule for swap up advice*/
    swapuprule = new AdviceRule(AdviceType::AT_swap_up,
            PartitionScope::PS_tier_internal_disk_wise,
            PartitionBase::PB_long_term | PartitionBase::PB_ema |
            PartitionBase::PB_random | PartitionBase::PB_rt,
            SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_iops,
            Selector::SEL_from_larger);
    swapdownrule = new AdviceRule(AdviceType::AT_swap_down,
            PartitionScope::PS_tier_internal_disk_wise,
            PartitionBase::PB_long_term | PartitionBase::PB_ema |
            PartitionBase::PB_sequential | PartitionBase::PB_rt,
            SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_iops,
            Selector::SEL_from_smaller);

    YWB_ASSERT(hotadvicerule);
    YWB_ASSERT(coldadvicerule);
    /*YWB_ASSERT(coldfilterrule);*/
    YWB_ASSERT(swapuprule);
    YWB_ASSERT(swapdownrule);

    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        ret = mAdviceManager->PreGenerateCheck(subpool);
        if(YWB_SUCCESS == ret)
        {
#ifdef WITH_INITIAL_PLACEMENT
            if(ywb_true == swapoutmgr->GetSwapRequired(subpoolkey))
            {
                ast_log_debug("CTR generate SWAPUP advice for subpool: ["
                        << subpoolkey.GetOss()
                        << ", " << subpoolkey.GetPoolId()
                        << ", " << subpoolkey.GetSubPoolId()
                        << "]");
                mAdviceManager->GenerateAdvice(
                        subpoolkey, *swapuprule, swapupfilterrule, 0);

                ast_log_debug("CTR generate SWAPDOWN advice for subpool: ["
                        << subpoolkey.GetOss()
                        << ", " << subpoolkey.GetPoolId()
                        << ", " << subpoolkey.GetSubPoolId()
                        << "]");
                mAdviceManager->GenerateAdvice(
                        subpoolkey, *swapdownrule, swapdownfilterrule, 0);
            }
            else
#endif
            {
                ast_log_debug("CTR generate HOT advice for subpool: ["
                        << subpoolkey.GetOss()
                        << ", " << subpoolkey.GetPoolId()
                        << ", " << subpoolkey.GetSubPoolId()
                        << "]");
                mAdviceManager->GenerateAdvice(
                        subpoolkey, *hotadvicerule, hotfilterrule, 0);

                ast_log_debug("CTR generate COLD advice for subpool: ["
                        << subpoolkey.GetOss()
                        << ", " << subpoolkey.GetPoolId()
                        << ", " << subpoolkey.GetSubPoolId()
                        << "]");
    //            mAdviceManager->GenerateAdvice(
    //                    subpoolkey, *coldadvicerule, coldfilterrule, 1);
                mAdviceManager->GenerateAdvice(
                        subpoolkey, *coldadvicerule, coldfilterrule, 0);
            }
        }

        config->PutSubPool(subpool);
        ret = config->GetNextSubPool(&subpool, &subpoolkey);
    }

    if(hotadvicerule)
    {
        delete hotadvicerule;
        hotadvicerule = NULL;
    }

    if(coldadvicerule)
    {
        delete coldadvicerule;
        coldadvicerule = NULL;
    }

    if(hotfilterrule)
    {
        delete hotfilterrule;
        hotfilterrule = NULL;
    }

    if(coldfilterrule)
    {
        delete coldfilterrule;
        coldfilterrule = NULL;
    }

    if(swapuprule)
    {
        delete swapuprule;
        swapuprule = NULL;
    }

    if(swapupfilterrule)
    {
        delete swapupfilterrule;
        swapupfilterrule = NULL;
    }

    if(swapdownrule)
    {
        delete swapdownrule;
        swapdownrule = NULL;
    }

    if(swapdownfilterrule)
    {
        delete swapdownfilterrule;
        swapdownfilterrule = NULL;
    }

    return ret;
}

RelocationITR::RelocationITR(AST* ast) : RelocationTask(ast)
{
    GetAST(&ast);
    ast->GetAdviseManager(&mAdviceManager);
    ast->GetHeatDistributionManager(&mHeatDistributionManager);
    YWB_ASSERT(mAdviceManager != NULL);
    YWB_ASSERT(mHeatDistributionManager != NULL);
}

RelocationITR::~RelocationITR()
{
    mAdviceManager = NULL;
    mHeatDistributionManager = NULL;
}

/*currently ITR is not supported*/
ywb_status_t
RelocationITR::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("ITR task reset");
    return ret;
}

ywb_status_t
RelocationITR::Init()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("ITR task init");
    return ret;
}

ywb_status_t
RelocationITR::Prepare()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("ITR task prepare");
    Reset();
    Init();

    return ret;
}

/*currently ITR is not supported*/
ywb_status_t
RelocationITR::DoCycleWork()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("ITR task cyclic work");
    return ret;
}

RelocationPlan::RelocationPlan(AST* ast) : RelocationTask(ast)
{
    GetAST(&ast);
    ast->GetPerfManager(&mDiskPerfManager);
    ast->GetPlanManager(&mPlanManager);
    YWB_ASSERT(mDiskPerfManager != NULL);
    YWB_ASSERT(mPlanManager != NULL);
}

RelocationPlan::~RelocationPlan()
{
    mDiskPerfManager = NULL;
    mPlanManager = NULL;
}

ywb_status_t
RelocationPlan::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Plan task reset");
    mPlanManager->Reset(ywb_false);
    mDiskPerfManager->Reset();

    return ret;
}

ywb_status_t
RelocationPlan::Init()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Plan task init");
    mDiskPerfManager->Init();
    mPlanManager->Init();

    return ret;
}

ywb_status_t
RelocationPlan::HandleCompleted(SubPoolKey& subpoolkey,
        DiskKey& sourcekey, DiskKey& targetkey,
        ywb_bool_t triggermore, ywb_status_t migstatus)
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Plan task handle completion notification");
    ret = mPlanManager->Complete(subpoolkey, sourcekey,
            targetkey, triggermore, migstatus);

    return ret;
}

ywb_status_t
RelocationPlan::HandleExpired(SubPoolKey& subpoolkey,
            DiskKey& sourcekey, DiskKey& targetkey,
            ywb_bool_t triggermore)
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Plan task handle expiration notification");
    ret = mPlanManager->Expire(subpoolkey, sourcekey, targetkey, triggermore);

    return ret;
}

ywb_status_t
RelocationPlan::Prepare()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("Plan task prepare");
    Reset();
    Init();

    return ret;
}

ywb_status_t
RelocationPlan::DoCycleWork()
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PlanManager* planmgr = NULL;
    Error* err = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolKey subpoolkey;
    ywb_uint32_t numplans = 0;
    ywb_uint32_t scheduledsubpools = 0;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    ast->GetPlanManager(&planmgr);
    ast->GetError(&err);

    ast_log_debug("Plan task cyclic work");
    ret = planmgr->GetFirstSubPoolPlans(&subpoolplans, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpoolplans != NULL))
    {
        numplans = 0;
        /*generate plan*/
        ret = mPlanManager->ScheduleFirstBatch(subpoolkey, &numplans);
        if((YWB_SUCCESS == ret))
        {
            mPlanManager->SubmitFirstBatch(subpoolkey);
        }
        else if(Error::PLNE_subpool_schedule_timed_delay == err->GetPLNErr())
        {
            ast_log_debug("Timed-delay schedule first batch");
            mPlanManager->ScheduleFirstBatchTimedDelay(subpoolkey,
                    ((Constant::CYCLE_PERIOD) * YWB_USEC_PER_SEC));
        }
        else if(Error::PLNE_subpool_no_more_plan != err->GetPLNErr())
        {
            ast_log_debug("Timed-delay schedule first batch");
            mPlanManager->ScheduleFirstBatchDelay(subpoolkey);
        }

        scheduledsubpools++;
        ret = planmgr->GetNextSubPoolPlans(&subpoolplans, &subpoolkey);
    }

    if((scheduledsubpools > 0) && (YFS_ENOENT == ret))
    {
        ret = YWB_SUCCESS;
    }

    return ret;
}

RelocationARB::RelocationARB(AST* ast) : RelocationTask(ast)
{
    GetAST(&ast);
    ast->GetArb(&mArb);
    YWB_ASSERT(mArb != NULL);
}

RelocationARB::~RelocationARB()
{
    mArb = NULL;
}

ywb_status_t
RelocationARB::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("ARB task reset");
    ret = mArb->Reset(ywb_false);

    return ret;
}

ywb_status_t
RelocationARB::HandleCompleted(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey, ywb_bool_t curwindow,
        ywb_status_t migstatus)
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("ARB task handle completion notification");
    ret = mArb->Complete(subpoolkey, arbitrateekey, curwindow, migstatus);

    return ret;
}

ywb_status_t
RelocationARB::HandleExpired(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, ywb_bool_t curwindow)
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("ARB task handle expiration notification");
    ret = mArb->Expire(subpoolkey, arbitrateekey, curwindow);

    return ret;
}

ywb_status_t
RelocationARB::Prepare()
{
    ast_log_debug("ARB task prepare");
    return Reset();
}

ywb_status_t
RelocationARB::DoCycleWork()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("ARB task cyclic work");
    /*nothing to do at current*/
    return ret;
}

RelocationMIG::RelocationMIG(AST* ast) : RelocationTask(ast)
{
    GetAST(&ast);
    ast->GetMigration(&mMigrationManager);
    YWB_ASSERT(mMigrationManager != NULL);
}

RelocationMIG::~RelocationMIG()
{
    mMigrationManager = NULL;
}

ywb_status_t
RelocationMIG::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("MIG task reset");
    mMigrationManager->Reset(ywb_false);

    return ret;
}

ywb_status_t
RelocationMIG::Init()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("MIG task init");
    mMigrationManager->Init();

    return ret;
}

ywb_status_t
RelocationMIG::HandleCompleted(DiskKey& diskkey,
        ArbitrateeKey& arbitrateekey, ywb_status_t migerr,
        ywb_bool_t triggermore)
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("MIG task handle completion notification");
    ret = mMigrationManager->Complete(diskkey,
            arbitrateekey, migerr, triggermore);

    return ret;
}

ywb_status_t
RelocationMIG::HandleExpired(DiskKey& diskkey,
        ArbitrateeKey& arbitrateekey,
        ywb_bool_t triggermore)
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("MIG task handle expiration notification");
    ret = mMigrationManager->Expire(diskkey, arbitrateekey, triggermore);

    return ret;
}

ywb_status_t
RelocationMIG::RemoveInflight(ArbitrateeKey& arbitrateekey)
{
    return mMigrationManager->RemoveInflight(arbitrateekey);
}

ywb_uint32_t
RelocationMIG::GetInflightEmpty()
{
    return mMigrationManager->GetInflightEmpty();
}

ywb_status_t
RelocationMIG::Prepare()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("MIG task prepare");
    Reset();
    Init();

    return ret;
}

ywb_status_t
RelocationMIG::DoCycleWork()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("MIG task cyclic work");
    /*nothing to do at current*/
    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
