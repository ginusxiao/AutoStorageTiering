#include "UnitTest/AST/Plan/ASTPlanUnitTests.hpp"

TEST_F(SubPoolPlanDirStatTest, ExcludeDirectionFromAvailableDirections_Local_SUCCESS)
{
    ywb_uint32_t availplandirs = 0;
    SubPoolPlanDirStat* subpoolplandirstat = NULL;

    subpoolplandirstat = GetSubPoolPlanDirStat();
    availplandirs |= 1 << Plan::PD_ctr_promote;
    availplandirs |= 1 << Plan::PD_ctr_demote;

    availplandirs = subpoolplandirstat->ExcludeDirectionFromAvailableDirections(
            availplandirs, Plan::PD_ctr_demote);
    ASSERT_EQ(availplandirs, 1);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 2);
    availplandirs = subpoolplandirstat->ExcludeDirectionFromAvailableDirections(
            availplandirs, Plan::PD_ctr_demote);
    ASSERT_EQ(availplandirs, 1);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 2);
    availplandirs = subpoolplandirstat->ExcludeDirectionFromAvailableDirections(
            availplandirs, Plan::PD_ctr_promote);
    ASSERT_EQ(availplandirs, 0);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 2);
}

TEST_F(SubPoolPlanDirStatTest, ExcludeDirectionFromAvailableDirections_Local_DirectionOutOfBound_SUCCESS)
{
    ywb_uint32_t availplandirs = 0;
    SubPoolPlanDirStat* subpoolplandirstat = NULL;

    subpoolplandirstat = GetSubPoolPlanDirStat();
    availplandirs |= 1 << Plan::PD_ctr_promote;

    availplandirs = subpoolplandirstat->ExcludeDirectionFromAvailableDirections(
            availplandirs, Plan::PD_cnt);
    ASSERT_EQ(availplandirs, 1);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 2);
}

TEST_F(SubPoolPlanDirStatTest, ExcludeDirectionFromAvailableDirections_Global_SUCCESS)
{
    ywb_uint32_t availplandirs = 0;
    SubPoolPlanDirStat* subpoolplandirstat = NULL;

    subpoolplandirstat = GetSubPoolPlanDirStat();
    availplandirs = subpoolplandirstat->
            ExcludeDirectionFromAvailableDirections(Plan::PD_ctr_demote);
    ASSERT_EQ(availplandirs, 1);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 1);
    availplandirs = subpoolplandirstat->
            ExcludeDirectionFromAvailableDirections(Plan::PD_ctr_demote);
    ASSERT_EQ(availplandirs, 1);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 1);
    availplandirs = subpoolplandirstat->
            ExcludeDirectionFromAvailableDirections(Plan::PD_ctr_promote);
    ASSERT_EQ(availplandirs, 0);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 0);
}

TEST_F(SubPoolPlanDirStatTest, ExcludeDirectionFromAvailableDirections_Global_DirectionOutOfBound_SUCCESS)
{
    ywb_uint32_t availplandirs = 0;
    SubPoolPlanDirStat* subpoolplandirstat = NULL;

    subpoolplandirstat = GetSubPoolPlanDirStat();
    availplandirs = subpoolplandirstat->
            ExcludeDirectionFromAvailableDirections(Plan::PD_cnt);
    ASSERT_EQ(availplandirs, 3);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 2);
}

TEST_F(SubPoolPlanDirStatTest, GetPriorPlanDirectionFromAvailable_Local_SUCCESS)
{
    ywb_uint32_t availplandirs = 0;
    ywb_uint32_t direction = 0;
    SubPoolPlanDirStat* subpoolplandirstat = NULL;

    subpoolplandirstat = GetSubPoolPlanDirStat();
    availplandirs |= 1 << Plan::PD_ctr_promote;
    availplandirs |= 1 << Plan::PD_ctr_demote;

    direction = subpoolplandirstat->
            GetPriorPlanDirectionFromAvailable(availplandirs);
    ASSERT_EQ(direction, Plan::PD_ctr_promote);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 2);
}

TEST_F(SubPoolPlanDirStatTest, GetPriorPlanDirectionFromAvailable_LocalPromoteIsExcluded_SUCCESS)
{
    ywb_uint32_t availplandirs = 0;
    ywb_uint32_t direction = 0;
    SubPoolPlanDirStat* subpoolplandirstat = NULL;

    subpoolplandirstat = GetSubPoolPlanDirStat();
    availplandirs |= 1 << Plan::PD_ctr_promote;
    availplandirs |= 1 << Plan::PD_ctr_demote;

    subpoolplandirstat->
                ExcludeDirectionFromAvailableDirections(Plan::PD_ctr_promote);
    direction = subpoolplandirstat->
            GetPriorPlanDirectionFromAvailable(availplandirs);
    ASSERT_EQ(direction, Plan::PD_ctr_demote);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 1);
}

TEST_F(SubPoolPlanDirStatTest, GetPriorPlanDirectionFromAvailable_LocalPromoteHasBeenScheduledManyTimes_SUCCESS)
{
    ywb_uint32_t availplandirs = 0;
    ywb_uint32_t direction = 0;
    SubPoolPlanDirStat* subpoolplandirstat = NULL;

    subpoolplandirstat = GetSubPoolPlanDirStat();
    availplandirs |= 1 << Plan::PD_ctr_promote;
    availplandirs |= 1 << Plan::PD_ctr_demote;

    subpoolplandirstat->UpdatePlanDirScheTimes(Plan::PD_ctr_promote, 100, true);
    direction = subpoolplandirstat->
            GetPriorPlanDirectionFromAvailable(availplandirs);
    ASSERT_EQ(direction, Plan::PD_ctr_demote);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 2);
}

TEST_F(SubPoolPlanDirStatTest, GetPriorPlanDirectionFromAvailable_LocalPriorityQueueIsEmpty_SUCCESS)
{
    ywb_uint32_t availplandirs = 0;
    ywb_uint32_t direction = 0;
    SubPoolPlanDirStat* subpoolplandirstat = NULL;

    subpoolplandirstat = GetSubPoolPlanDirStat();
    availplandirs |= 1 << Plan::PD_ctr_promote;
    availplandirs |= 1 << Plan::PD_ctr_demote;

    subpoolplandirstat->
                ExcludeDirectionFromAvailableDirections(Plan::PD_ctr_promote);
    subpoolplandirstat->
                ExcludeDirectionFromAvailableDirections(Plan::PD_ctr_demote);
    direction = subpoolplandirstat->
            GetPriorPlanDirectionFromAvailable(availplandirs);
    ASSERT_EQ(direction, Plan::PD_cnt);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 0);
}

TEST_F(SubPoolPlanDirStatTest, GetPriorPlanDirectionFromAvailable_LocalPriorityQueueAndAvailableSetHasNoIntersection_SUCCESS)
{
    ywb_uint32_t availplandirs = 0;
    ywb_uint32_t direction = 0;
    SubPoolPlanDirStat* subpoolplandirstat = NULL;

    subpoolplandirstat = GetSubPoolPlanDirStat();
    availplandirs |= 1 << Plan::PD_ctr_demote;

    subpoolplandirstat->
                ExcludeDirectionFromAvailableDirections(Plan::PD_ctr_demote);
    direction = subpoolplandirstat->
            GetPriorPlanDirectionFromAvailable(availplandirs);
    ASSERT_EQ(direction, Plan::PD_cnt);
    ASSERT_EQ(subpoolplandirstat->GetPriorityQueueSize(), 1);
}

TEST_F(SubPoolPlansTest, AddPlan_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint64_t inodeids[6] = {1, 2, 3, 4, 5, 6};
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;
    OBJKey* objkey = NULL;

    subpoolplans = GetSubPoolPlans();
    ret = subpoolplans->GetFirstPlan(&plan);
    while((YWB_SUCCESS == ret) && (plan != NULL))
    {
        plan->GetOBJKey(&objkey);
        ASSERT_EQ(objkey->GetInodeId(), inodeids[plannum++]);
        ret = subpoolplans->GetNextPlan(&plan);
    }
    ASSERT_EQ(plannum, 6);
}

TEST_F(SubPoolPlansTest, AddPlan_AfterGetFirst_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint64_t inodeids[7] = {1, 2, 3, 4, 5, 6, 7};
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;
    OBJKey* objkey = NULL;

    subpoolplans = GetSubPoolPlans();
    subpoolplans->GetFirstPlan(&plan);
    AddPlan(7);
    while((YWB_SUCCESS == ret) && (plan != NULL))
    {
        plan->GetOBJKey(&objkey);
        ASSERT_EQ(objkey->GetInodeId(), inodeids[plannum++]);
        ret = subpoolplans->GetNextPlan(&plan);
    }
    ASSERT_EQ(plannum, 7);
}

TEST_F(SubPoolPlansTest, AddPlan_AfterGetNext_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint64_t inodeids[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;
    OBJKey* objkey = NULL;

    subpoolplans = GetSubPoolPlans();
    ret = subpoolplans->GetFirstPlan(&plan);
    while((YWB_SUCCESS == ret) && (plan != NULL))
    {
        if(2 == plannum)
        {
            AddPlan(7);
        }
        else if(4 == plannum)
        {
            AddPlan(8);
        }
        plan->GetOBJKey(&objkey);
        ASSERT_EQ(objkey->GetInodeId(), inodeids[plannum++]);
        ret = subpoolplans->GetNextPlan(&plan);
    }
    ASSERT_EQ(plannum, 8);
}

TEST_F(SubPoolPlansTest, AddPlan_AfterGetAllPlans_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint64_t inodeids[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;
    OBJKey* objkey = NULL;

    subpoolplans = GetSubPoolPlans();
    ret = subpoolplans->GetFirstPlan(&plan);
    while((YWB_SUCCESS == ret) && (plan != NULL))
    {
        plan->GetOBJKey(&objkey);
        ASSERT_EQ(objkey->GetInodeId(), inodeids[plannum++]);
        ret = subpoolplans->GetNextPlan(&plan);
    }
    ASSERT_EQ(plannum, 6);
    AddPlan(7);
    AddPlan(8);
    ret = subpoolplans->GetNextPlan(&plan);
    while((YWB_SUCCESS == ret) && (plan != NULL))
    {
        plan->GetOBJKey(&objkey);
        ASSERT_EQ(objkey->GetInodeId(), inodeids[plannum++]);
        ret = subpoolplans->GetNextPlan(&plan);
    }
    ASSERT_EQ(plannum, 8);
}

TEST_F(PlanManagerTest, GetFirstNextSubPoolPlans_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolnum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    ywb_uint32_t subpoolids[6] = {1, 2, 3, 7};

    planmgr = GetPlanManager();
    ret = planmgr->GetFirstSubPoolPlans(&subpoolplans, &subpoolkey);
    while((YWB_SUCCESS == ret))
    {
        ASSERT_EQ(subpoolkey.GetSubPoolId(), subpoolids[subpoolnum++]);
        ret = planmgr->GetNextSubPoolPlans(&subpoolplans, &subpoolkey);
    }
    ASSERT_EQ(subpoolnum, 4);
}

TEST_F(PlanManagerTest, GetFirstNextSubPoolPlans_EmptySet_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolnum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;

    planmgr = GetPlanManager();
    planmgr->Reset(ywb_false);
    ret = planmgr->GetFirstSubPoolPlans(&subpoolplans, &subpoolkey);
    while((YWB_SUCCESS == ret))
    {
        subpoolnum++;
        ret = planmgr->GetNextSubPoolPlans(&subpoolplans, &subpoolkey);
    }
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(subpoolnum, 0);
}

TEST_F(PlanManagerTest, GetSubPoolPlans_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolid = Constant::SUBPOOL_ID;

    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        if(!((3 == subpoolloop) || (4 == subpoolloop) || (5 == subpoolloop)))
        {
            ret = planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
            ASSERT_EQ(ret, YWB_SUCCESS);
            ASSERT_EQ(subpoolplans != NULL, true);
        }
    }
}

TEST_F(PlanManagerTest, GetSubPoolPlans_EmptySet_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolid = Constant::SUBPOOL_ID;
    planmgr->Reset(ywb_false);

    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        if(!((3 == subpoolloop) || (4 == subpoolloop) || (5 == subpoolloop)))
        {
            ret = planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
            ASSERT_EQ(ret, YFS_ENOENT);
        }
    }
}

TEST_F(PlanManagerTest, GetSubPoolPlans_NoGivenSubPool_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 100);
    ret = planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, RemoveSubPoolPlans_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolid = Constant::SUBPOOL_ID;

    for(subpoolloop = 0; subpoolloop < 7; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        if(!((3 == subpoolloop) || (4 == subpoolloop) || (5 == subpoolloop)))
        {
            ret = planmgr->RemoveSubPoolPlans(subpoolkey);
            ASSERT_EQ(ret, YWB_SUCCESS);
        }
    }

    ret = planmgr->GetFirstSubPoolPlans(&subpoolplans, &subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, ScheduleFirstBatch_NoAvailPlanDirs_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    subpoolplans->GetSubPoolPlanDirStat(&plandirstat);
    plandirstat->ExcludeDirectionFromAvailableDirections(Plan::PD_ctr_promote);
    plandirstat->ExcludeDirectionFromAvailableDirections(Plan::PD_ctr_demote);

    ret = planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ASSERT_EQ(ret, YFS_ENODATA);
    ASSERT_EQ(plannum, 0);
}

TEST_F(PlanManagerTest, ScheduleFirstBatch_SubPoolNoHotAdvice_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t curavailplandirs = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 2);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    subpoolplans->GetSubPoolPlanDirStat(&plandirstat);
    ret = planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(plannum, 1);
    curavailplandirs = plandirstat->GetCurrentAvailablePlanDirections();
    ASSERT_EQ(curavailplandirs, 2);
}

TEST_F(PlanManagerTest, ScheduleFirstBatch_SubPoolNoColdAdvice_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t curavailplandirs = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    subpoolplans->GetSubPoolPlanDirStat(&plandirstat);
    ret = planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(plannum, 1);
    curavailplandirs = plandirstat->GetCurrentAvailablePlanDirections();
    ASSERT_EQ(curavailplandirs, 1);
}

TEST_F(PlanManagerTest, ScheduleFirstBatch_SubPoolNoAdvice_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t curavailplandirs = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 1);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    subpoolplans->GetSubPoolPlanDirStat(&plandirstat);
    RemoveAllAdvise(subpoolkey, AdviceType::AT_ctr_hot);
    RemoveAllAdvise(subpoolkey, AdviceType::AT_ctr_cold);
    ret = planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ASSERT_EQ(ret, YFS_ENODATA);
    ASSERT_EQ(plannum, 0);
    curavailplandirs = plandirstat->GetCurrentAvailablePlanDirections();
    ASSERT_EQ(curavailplandirs, 0);
    set = subpoolplans->TestFlag(SubPoolPlans::SPF_no_more_plan);
    ASSERT_EQ(set, true);
}

TEST_F(PlanManagerTest, ScheduleFirstBatch_NoSatisfiedDisk_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t curavailplandirs = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    subpoolplans->GetSubPoolPlanDirStat(&plandirstat);
    DrainSubPoolWiseDiskAvailPerf(subpoolkey, IOStat::IOST_raw);
    ret = planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ASSERT_EQ(ret, YFS_ENODATA);
    ASSERT_EQ(plannum, 0);
    curavailplandirs = plandirstat->GetCurrentAvailablePlanDirections();
    ASSERT_EQ(curavailplandirs, 3);
    set = subpoolplans->TestFlag(SubPoolPlans::SPF_no_more_plan);
    ASSERT_EQ(set, true);
}

TEST_F(PlanManagerTest, ScheduleFirstBatch_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t curavailplandirs = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    subpoolplans->GetSubPoolPlanDirStat(&plandirstat);
    ret = planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(plannum, 1);
    curavailplandirs = plandirstat->GetCurrentAvailablePlanDirections();
    ASSERT_EQ(curavailplandirs, 3);
}

TEST_F(PlanManagerTest, ScheduleFirstBatch_MoreThanTwoPlansScheduled_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t curavailplandirs = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    SubPoolPlanDirStat* plandirstat = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    subpoolplans->GetSubPoolPlanDirStat(&plandirstat);
    ret = planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(plannum, 6);
    curavailplandirs = plandirstat->GetCurrentAvailablePlanDirections();
    ASSERT_EQ(curavailplandirs, 3);
}

TEST_F(PlanManagerTest, ScheduleNext_RetryUntilExceedsPresetTimes_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    ywb_uint64_t diskids[20] = {};

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    RemoveAllAdvise(subpoolkey, AdviceType::AT_ctr_hot);
    DrainSubPoolWiseDiskAvailPerf(subpoolkey, IOStat::IOST_raw);
    GetAllPlansTarget(subpoolkey, diskids, &disknum);
    AddAvailDisks(subpoolkey, diskids, disknum);
    ret = planmgr->ScheduleNext(subpoolkey);
    ASSERT_EQ(ret, YFS_ENODATA);
}

TEST_F(PlanManagerTest, ScheduleNext_RetryButNoAdvice_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    ywb_uint64_t diskids[20] = {};

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    RemoveAllAdvise(subpoolkey, AdviceType::AT_ctr_cold);
    DrainSubPoolWiseDiskAvailPerf(subpoolkey, IOStat::IOST_raw);
    GetAllPlansTarget(subpoolkey, diskids, &disknum);
    AddAvailDisks(subpoolkey, diskids, disknum);
    ret = planmgr->ScheduleNext(subpoolkey);
    ASSERT_EQ(ret, YFS_ENODATA);
}

TEST_F(PlanManagerTest, ScheduleNext_RetryButSPFSubPoolDeletedFlagSet_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    ywb_uint64_t diskids[20] = {};

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    DrainSubPoolWiseDiskAvailPerf(subpoolkey, IOStat::IOST_raw);
    GetAllPlansTarget(subpoolkey, diskids, &disknum);
    AddAvailDisks(subpoolkey, diskids, disknum);
    subpoolplans->SetFlagComb(SubPoolPlans::SPF_subpool_deleted);
    ret = planmgr->ScheduleNext(subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, ScheduleNext_RetryButSubPoolNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    ywb_uint64_t diskids[20] = {};

    config = GetConfig();
    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    DrainSubPoolWiseDiskAvailPerf(subpoolkey, IOStat::IOST_raw);
    GetAllPlansTarget(subpoolkey, diskids, &disknum);
    AddAvailDisks(subpoolkey, diskids, disknum);
    config->RemoveSubPool(subpoolkey);
    ret = planmgr->ScheduleNext(subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, ScheduleNext_NoAdvice_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    ywb_uint64_t diskids[20] = {};

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    RemoveAllAdvise(subpoolkey, AdviceType::AT_ctr_hot);
    RemoveAllAdvise(subpoolkey, AdviceType::AT_ctr_cold);
    GetAllPlansTarget(subpoolkey, diskids, &disknum);
    AddAvailDisks(subpoolkey, diskids, disknum);
    ret = planmgr->ScheduleNext(subpoolkey);
    ASSERT_EQ(ret, YFS_ENODATA);
}

TEST_F(PlanManagerTest, ScheduleNext_AvailDisksEmpty_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ret = planmgr->ScheduleNext(subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, ScheduleNext_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;
    ywb_uint64_t diskids[20] = {};

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    GetAllPlansTarget(subpoolkey, diskids, &disknum);
    AddAvailDisks(subpoolkey, diskids, disknum);
    ret = planmgr->ScheduleNext(subpoolkey);
    while((YWB_SUCCESS == ret))
    {
        ret = planmgr->ScheduleNext(subpoolkey);
    }

    plannum = 0;
    ret = subpoolplans->GetFirstPlan(&plan);
    while((YWB_SUCCESS == ret))
    {
        plannum++;
        ret = subpoolplans->GetNextPlan(&plan);
    }

    /*
     * if plannum equals to 12, we are sure ScheduleNext() has
     * been successfully invoked at least once, otherwise it
     * is impossible to have 12 plans generated
     * */
    ASSERT_EQ(plannum, 12);
}

TEST_F(PlanManagerTest, ScheduleFirstBatchForTimedDelayed_NoDelayedSubPool_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    SubPoolKey scheduledsubpool;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    ret = planmgr->ScheduleFirstBatchForTimedDelayed(
            &scheduledsubpool, &plannum);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(scheduledsubpool.GetSubPoolId(), YWB_UINT32_MAX);
}

TEST_F(PlanManagerTest, ScheduleFirstBatchForTimedDelayed_HaveSubpoolNotTimedOut_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey1;
    SubPoolKey subpoolkey2;
    SubPoolKey scheduledsubpool;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans1 = NULL;

    planmgr = GetPlanManager();
    subpoolkey1.SetOss(Constant::OSS_ID);
    subpoolkey1.SetPoolId(Constant::POOL_ID);
    subpoolkey1.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    subpoolkey2.SetOss(Constant::OSS_ID);
    subpoolkey2.SetPoolId(Constant::POOL_ID);
    subpoolkey2.SetSubPoolId(Constant::SUBPOOL_ID);

    planmgr->GetSubPoolPlans(subpoolkey1, &subpoolplans1);
    RemoveAllAdvise(subpoolkey1, AdviceType::AT_ctr_hot);
    RemoveAllAdvise(subpoolkey1, AdviceType::AT_ctr_cold);
    planmgr->ScheduleFirstBatchTimedDelay(subpoolkey1, (ywb_time_t)10);
    /*make sure subpool corresponding to subpoolkey2 not timedout*/
    planmgr->ScheduleFirstBatchTimedDelay(subpoolkey2, (ywb_time_t)100000000);

    /*make sure subpool corresponding to subpoolkey1 timedout*/
    usleep(20);
    ret = planmgr->ScheduleFirstBatchForTimedDelayed(
            &scheduledsubpool, &plannum);
    ASSERT_EQ(ret, YFS_ENODATA);
    ASSERT_EQ(scheduledsubpool.GetSubPoolId(), YWB_UINT32_MAX);
    set = subpoolplans1->TestFlag(SubPoolPlans::SPF_no_more_plan);
    ASSERT_EQ(set, true);
    set = planmgr->CheckInTimedDelayedSubPoolsForFirstBatch(subpoolkey1);
    ASSERT_EQ(set, false);
    set = planmgr->CheckInTimedDelayedSubPoolsForFirstBatch(subpoolkey2);
    ASSERT_EQ(set, true);
}

TEST_F(PlanManagerTest, ScheduleFirstBatchForTimedDelayed_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey1;
    SubPoolKey subpoolkey2;
    SubPoolKey scheduledsubpool;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans1 = NULL;

    planmgr = GetPlanManager();
    subpoolkey1.SetOss(Constant::OSS_ID);
    subpoolkey1.SetPoolId(Constant::POOL_ID);
    subpoolkey1.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    subpoolkey2.SetOss(Constant::OSS_ID);
    subpoolkey2.SetPoolId(Constant::POOL_ID);
    subpoolkey2.SetSubPoolId(Constant::SUBPOOL_ID);

    planmgr->GetSubPoolPlans(subpoolkey1, &subpoolplans1);
    RemoveAllAdvise(subpoolkey1, AdviceType::AT_ctr_hot);
    RemoveAllAdvise(subpoolkey1, AdviceType::AT_ctr_cold);
    planmgr->ScheduleFirstBatchTimedDelay(subpoolkey1, (ywb_time_t)10);
    planmgr->ScheduleFirstBatchTimedDelay(subpoolkey2, (ywb_time_t)100000);

    /*make sure both subpools timed out*/
    usleep(100000);
    ret = planmgr->ScheduleFirstBatchForTimedDelayed(
            &scheduledsubpool, &plannum);
    ASSERT_EQ(ret, YWB_SUCCESS);
    set = subpoolplans1->TestFlag(SubPoolPlans::SPF_no_more_plan);
    ASSERT_EQ(set, true);
    ASSERT_EQ(plannum, 1);
    ASSERT_EQ(scheduledsubpool.GetSubPoolId(), subpoolkey2.GetSubPoolId());
    set = planmgr->CheckInTimedDelayedSubPoolsForFirstBatch(subpoolkey1);
    ASSERT_EQ(set, false);
    set = planmgr->CheckInTimedDelayedSubPoolsForFirstBatch(subpoolkey2);
    ASSERT_EQ(set, false);
}

TEST_F(PlanManagerTest, ScheduleNextForTimedDelayed_NoDelayedSubPool_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey scheduledsubpool;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    ret = planmgr->ScheduleNextForTimedDelayed(&scheduledsubpool);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(scheduledsubpool.GetSubPoolId(), YWB_UINT32_MAX);
}

TEST_F(PlanManagerTest, ScheduleNextForTimedDelayed_HaveSubpoolNotTimedOut_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum1 = 0;
    ywb_uint32_t plannum2 = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey1;
    SubPoolKey subpoolkey2;
    SubPoolKey scheduledsubpool;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans1 = NULL;
    SubPoolPlans* subpoolplans2 = NULL;
    ywb_uint64_t diskids[20] = {};

    planmgr = GetPlanManager();
    subpoolkey1.SetOss(Constant::OSS_ID);
    subpoolkey1.SetPoolId(Constant::POOL_ID);
    subpoolkey1.SetSubPoolId(Constant::SUBPOOL_ID);
    subpoolkey2.SetOss(Constant::OSS_ID);
    subpoolkey2.SetPoolId(Constant::POOL_ID);
    subpoolkey2.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey1, &subpoolplans1);
    planmgr->GetSubPoolPlans(subpoolkey2, &subpoolplans2);
    planmgr->ScheduleFirstBatch(subpoolkey1, &plannum1);
    planmgr->ScheduleFirstBatch(subpoolkey2, &plannum2);
    DrainSubPoolWiseDiskAvailPerf(subpoolkey1, IOStat::IOST_raw);
    GetAllPlansTarget(subpoolkey2, diskids, &disknum);
    AddAvailDisks(subpoolkey2, diskids, disknum);
    planmgr->ScheduleNextTimedDelay(subpoolkey1, (ywb_time_t)10);
    /*make sure subpool corresponding to subpoolkey2 not timedout*/
    planmgr->ScheduleNextTimedDelay(subpoolkey2, (ywb_time_t)100000000);

    /*make sure subpool corresponding to subpoolkey1 timedout*/
    usleep(20);
    ret = planmgr->ScheduleNextForTimedDelayed(&scheduledsubpool);
    ASSERT_EQ(ret, YFS_ENODATA);
    ASSERT_EQ(scheduledsubpool.GetSubPoolId(), YWB_UINT32_MAX);

    set = planmgr->CheckInTimedDelayedSubPoolsForNext(subpoolkey1);
    ASSERT_EQ(set, true);
    set = planmgr->CheckInTimedDelayedSubPoolsForNext(subpoolkey2);
    ASSERT_EQ(set, true);
}

TEST_F(PlanManagerTest, ScheduleNextForTimedDelayed_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum1 = 0;
    ywb_uint32_t plannum2 = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey1;
    SubPoolKey subpoolkey2;
    SubPoolKey scheduledsubpool;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans1 = NULL;
    SubPoolPlans* subpoolplans2 = NULL;
    Plan* plan = NULL;
    ywb_uint64_t diskids[20] = {};

    planmgr = GetPlanManager();
    subpoolkey1.SetOss(Constant::OSS_ID);
    subpoolkey1.SetPoolId(Constant::POOL_ID);
    subpoolkey1.SetSubPoolId(Constant::SUBPOOL_ID);
    subpoolkey2.SetOss(Constant::OSS_ID);
    subpoolkey2.SetPoolId(Constant::POOL_ID);
    subpoolkey2.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey1, &subpoolplans1);
    planmgr->GetSubPoolPlans(subpoolkey2, &subpoolplans2);
    planmgr->ScheduleFirstBatch(subpoolkey1, &plannum1);
    planmgr->ScheduleFirstBatch(subpoolkey2, &plannum2);
    DrainSubPoolWiseDiskAvailPerf(subpoolkey1, IOStat::IOST_raw);
    GetAllPlansTarget(subpoolkey2, diskids, &disknum);
    AddAvailDisks(subpoolkey2, diskids, disknum);
    planmgr->ScheduleNextTimedDelay(subpoolkey1, (ywb_time_t)10);
    planmgr->ScheduleNextTimedDelay(subpoolkey2, (ywb_time_t)100000);

    /*make sure both subpools timed out*/
    usleep(100000);
    ret = planmgr->ScheduleNextForTimedDelayed(&scheduledsubpool);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(scheduledsubpool.GetSubPoolId(), subpoolkey2.GetSubPoolId());

    ASSERT_EQ(plannum1, 1);
    plannum1 = 0;
    ret = subpoolplans1->GetFirstPlan(&plan);
    while((YWB_SUCCESS == ret))
    {
        plannum1++;
        ret = subpoolplans1->GetNextPlan(&plan);
    }
    ASSERT_EQ(plannum1, 1);

    ASSERT_EQ(plannum2, 6);
    plannum2 = 0;
    ret = subpoolplans2->GetFirstPlan(&plan);
    while((YWB_SUCCESS == ret))
    {
        plannum2++;
        ret = subpoolplans2->GetNextPlan(&plan);
    }
    ASSERT_EQ(plannum2, 7);

    set = planmgr->CheckInTimedDelayedSubPoolsForNext(subpoolkey1);
    ASSERT_EQ(set, true);
    set = planmgr->CheckInTimedDelayedSubPoolsForNext(subpoolkey2);
    ASSERT_EQ(set, false);
}

TEST_F(PlanManagerTest, ScheduleFirstBatchForDelayed_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey1;
    SubPoolKey subpoolkey2;
    SubPoolKey scheduledsubpool;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans1 = NULL;

    planmgr = GetPlanManager();
    subpoolkey1.SetOss(Constant::OSS_ID);
    subpoolkey1.SetPoolId(Constant::POOL_ID);
    subpoolkey1.SetSubPoolId(Constant::SUBPOOL_ID + 1);
    subpoolkey2.SetOss(Constant::OSS_ID);
    subpoolkey2.SetPoolId(Constant::POOL_ID);
    subpoolkey2.SetSubPoolId(Constant::SUBPOOL_ID);

    planmgr->GetSubPoolPlans(subpoolkey1, &subpoolplans1);
    RemoveAllAdvise(subpoolkey1, AdviceType::AT_ctr_hot);
    RemoveAllAdvise(subpoolkey1, AdviceType::AT_ctr_cold);
    planmgr->ScheduleFirstBatchDelay(subpoolkey1);
    planmgr->ScheduleFirstBatchDelay(subpoolkey2);

    ret = planmgr->ScheduleFirstBatchForDelayed(
            &scheduledsubpool, &plannum);
    ASSERT_EQ(ret, YWB_SUCCESS);
    set = subpoolplans1->TestFlag(SubPoolPlans::SPF_no_more_plan);
    ASSERT_EQ(set, true);
    ASSERT_EQ(plannum, 1);
    ASSERT_EQ(scheduledsubpool.GetSubPoolId(), subpoolkey2.GetSubPoolId());

    set = planmgr->CheckInDelayedSubPoolsForFirstBatch(subpoolkey1);
    ASSERT_EQ(set, false);
    set = planmgr->CheckInDelayedSubPoolsForFirstBatch(subpoolkey2);
    ASSERT_EQ(set, false);
}

TEST_F(PlanManagerTest, ScheduleNextForDelayed_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum1 = 0;
    ywb_uint32_t plannum2 = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey1;
    SubPoolKey subpoolkey2;
    SubPoolKey scheduledsubpool;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans1 = NULL;
    SubPoolPlans* subpoolplans2 = NULL;
    Plan* plan = NULL;
    ywb_uint64_t diskids[20] = {};

    planmgr = GetPlanManager();
    subpoolkey1.SetOss(Constant::OSS_ID);
    subpoolkey1.SetPoolId(Constant::POOL_ID);
    subpoolkey1.SetSubPoolId(Constant::SUBPOOL_ID);
    subpoolkey2.SetOss(Constant::OSS_ID);
    subpoolkey2.SetPoolId(Constant::POOL_ID);
    subpoolkey2.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey1, &subpoolplans1);
    planmgr->GetSubPoolPlans(subpoolkey2, &subpoolplans2);
    planmgr->ScheduleFirstBatch(subpoolkey1, &plannum1);
    planmgr->ScheduleFirstBatch(subpoolkey2, &plannum2);
    DrainSubPoolWiseDiskAvailPerf(subpoolkey1, IOStat::IOST_raw);
    GetAllPlansTarget(subpoolkey2, diskids, &disknum);
    AddAvailDisks(subpoolkey2, diskids, disknum);
    planmgr->ScheduleNextDelay(subpoolkey1);
    planmgr->ScheduleNextDelay(subpoolkey2);

    ret = planmgr->ScheduleNextForDelayed(&scheduledsubpool);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(scheduledsubpool.GetSubPoolId(), subpoolkey2.GetSubPoolId());

    ASSERT_EQ(plannum1, 1);
    plannum1 = 0;
    ret = subpoolplans1->GetFirstPlan(&plan);
    while((YWB_SUCCESS == ret))
    {
        plannum1++;
        ret = subpoolplans1->GetNextPlan(&plan);
    }
    ASSERT_EQ(plannum1, 1);

    ASSERT_EQ(plannum2, 6);
    plannum2 = 0;
    ret = subpoolplans2->GetFirstPlan(&plan);
    while((YWB_SUCCESS == ret))
    {
        plannum2++;
        ret = subpoolplans2->GetNextPlan(&plan);
    }
    ASSERT_EQ(plannum2, 7);

    set = planmgr->CheckInDelayedSubPoolsForNext(subpoolkey1);
    ASSERT_EQ(set, true);
    set = planmgr->CheckInDelayedSubPoolsForNext(subpoolkey2);
    ASSERT_EQ(set, false);
}

TEST_F(PlanManagerTest, RemoveSubPoolFromDelayed_SUCCESS)
{
    ywb_bool_t ret = false;
    SubPoolKey subpoolkey1(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    SubPoolKey subpoolkey2(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 1);
    SubPoolKey subpoolkey3(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 2);
    SubPoolKey subpoolkey4(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 3);
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    planmgr->ScheduleFirstBatchTimedDelay(subpoolkey1, (ywb_time_t)10);
    planmgr->ScheduleFirstBatchTimedDelay(subpoolkey2, (ywb_time_t)20);
    planmgr->ScheduleNextTimedDelay(subpoolkey3, (ywb_time_t)30);
    planmgr->ScheduleNextTimedDelay(subpoolkey4, (ywb_time_t)40);
    planmgr->ScheduleFirstBatchDelay(subpoolkey1);
    planmgr->ScheduleFirstBatchDelay(subpoolkey2);
    planmgr->ScheduleNextDelay(subpoolkey3);
    planmgr->ScheduleNextDelay(subpoolkey4);

    planmgr->RemoveSubPoolFromDelayed(subpoolkey1);
    planmgr->RemoveSubPoolFromDelayed(subpoolkey4);
    ret = planmgr->CheckInTimedDelayedSubPoolsForFirstBatch(subpoolkey1);
    ASSERT_EQ(ret, false);
    ret = planmgr->CheckInDelayedSubPoolsForFirstBatch(subpoolkey1);
    ASSERT_EQ(ret, false);
    ret = planmgr->CheckInTimedDelayedSubPoolsForNext(subpoolkey4);
    ASSERT_EQ(ret, false);
    ret = planmgr->CheckInDelayedSubPoolsForNext(subpoolkey4);
    ASSERT_EQ(ret, false);
}

TEST_F(PlanManagerTest, OnFailToSubmit_SubPoolNotExist_SUCCESS)
{
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey;
    Error* err = NULL;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    Plan* plan = NULL;

    err = GetError();
    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    /*pretend fail to submit the first plan*/
    subpoolplans->GetFirstPlan(&plan);
    err->SetEXEErr(Error::LCFE_subpool_not_exist);
    planmgr->OnFailToSubmit(subpoolplans, plan);
    ASSERT_EQ(subpoolplans->TestFlag(
            SubPoolPlans::SPF_subpool_deleted), true);
}

TEST_F(PlanManagerTest, OnFailToSubmit_SourceDiskNotExist_SUCCESS)
{
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    OBJKey* objkey = NULL;
    Error* err = NULL;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskstat = NULL;
    Plan* plan = NULL;

    err = GetError();
    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    /*pretend fail to submit the first plan*/
    subpoolplans->GetFirstPlan(&plan);
    err->SetEXEErr(Error::LCFE_source_disk_not_exist);
    planmgr->OnFailToSubmit(subpoolplans, plan);

    plan->GetOBJKey(&objkey);
    sourcekey.SetDiskId(objkey->GetStorageId());
    sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
    planmgr->GetDiskScheStat(sourcekey, &diskstat);
    ASSERT_EQ(diskstat->TestFlag(DiskScheStat::DSSS_deleted), true);
}

TEST_F(PlanManagerTest, OnFailToSubmit_TargetDiskNotExist_SUCCESS)
{
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey;
    DiskKey targetkey;
    Error* err = NULL;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskstat = NULL;
    Plan* plan = NULL;

    err = GetError();
    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    /*pretend fail to submit the first plan*/
    subpoolplans->GetFirstPlan(&plan);
    err->SetEXEErr(Error::LCFE_target_disk_not_exist);
    planmgr->OnFailToSubmit(subpoolplans, plan);

    targetkey.SetDiskId(plan->GetTargetId());
    targetkey.SetSubPoolId(subpoolkey.GetSubPoolId());
    planmgr->GetDiskScheStat(targetkey, &diskstat);
    ASSERT_EQ(diskstat->TestFlag(DiskScheStat::DSSS_deleted), true);
}

TEST_F(PlanManagerTest, SubmitFirstBatch_NoGivenSubPoolPlans_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 100);

    ret = planmgr->SubmitFirstBatch(subpoolkey);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(PlanManagerTest, SubmitFirstBatch_SubPoolNoPlan_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    ret = planmgr->SubmitFirstBatch(subpoolkey);
    ASSERT_EQ(ret, YFS_ENODATA);
}

TEST_F(PlanManagerTest, SubmitFirstBatch_FailToSubmitPlan_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    LogicalConfig* config = NULL;
    PlanManager* planmgr = NULL;

    config = GetConfig();
    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    /*simulate all hot advise' source disk does not exist*/
    sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
    sourcekey.SetDiskId(3 * (subpoolid));
    config->RemoveDisk(sourcekey);
    ret = planmgr->SubmitFirstBatch(subpoolkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(PlanManagerTest, SubmitFirstBatch_FailToSubmitPlanForSubPoolNotExist_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;
    PlanManager* planmgr = NULL;

    config = GetConfig();
    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    /*simulate subpool does not exist*/
    config->RemoveSubPool(subpoolkey);
    ret = planmgr->SubmitFirstBatch(subpoolkey);
    ASSERT_EQ(ret, YFS_ENODATA);
}

TEST_F(PlanManagerTest, SubmitNext_NoGivenSubPoolPlans_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 100);

    ret = planmgr->SubmitNext(subpoolkey);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(PlanManagerTest, SubmitNext_SubPoolNoPlan_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    planmgr->SubmitFirstBatch(subpoolkey);
    ret = planmgr->SubmitNext(subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, SubmitNext_FailToSubmitPlan_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t disknum = 0;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    LogicalConfig* config = NULL;
    PlanManager* planmgr = NULL;
    ywb_uint64_t diskids[20] = {};

    config = GetConfig();
    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    GetAllPlansTarget(subpoolkey, diskids, &disknum);
    AddAvailDisks(subpoolkey, diskids, disknum);
    planmgr->ScheduleNext(subpoolkey);
    /*simulate all advise' source disk does not exist*/
    sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
    sourcekey.SetDiskId(3 * (subpoolid));
    config->RemoveDisk(sourcekey);
    sourcekey.SetDiskId(3 * (subpoolid) - 2);
    config->RemoveDisk(sourcekey);
    ret = planmgr->SubmitNext(subpoolkey);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(PlanManagerTest, SubmitNext_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t disknum = 0;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    PlanManager* planmgr = NULL;
    ywb_uint64_t diskids[20] = {};

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    GetAllPlansTarget(subpoolkey, diskids, &disknum);
    AddAvailDisks(subpoolkey, diskids, disknum);
    planmgr->ScheduleNext(subpoolkey);
    ret = planmgr->SubmitNext(subpoolkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(PlanManagerTest, Complete_NoGivenSubPoolPlans_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    DiskKey targetkey;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 100);

    ret = planmgr->Complete(subpoolkey, sourcekey, targetkey, true, YWB_SUCCESS);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, Complete_SubPoolPlansIsMarkedAsDeleted_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    DiskKey targetkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    subpoolplans->SetFlagComb(SubPoolPlans::SPF_subpool_deleted);
    ret = planmgr->Complete(subpoolkey, sourcekey, targetkey, true, YWB_SUCCESS);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, Complete_SubPoolNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    DiskKey targetkey;
    LogicalConfig* config = NULL;
    PlanManager* planmgr = NULL;

    config = GetConfig();
    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);
    config->RemoveSubPool(subpoolkey);
    ret = planmgr->Complete(subpoolkey, sourcekey, targetkey, true, YWB_SUCCESS);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, Complete_NoMorePlan_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    DiskKey targetkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* sourcedisk = NULL;
    DiskScheStat* targetdisk = NULL;

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    sourcekey.SetSubPoolId(subpoolid);
    sourcekey.SetDiskId(3 * (subpoolid) - 1);
    targetkey.SetSubPoolId(subpoolid);
    targetkey.SetDiskId(3 * (subpoolid) - 2);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    planmgr->SubmitFirstBatch(subpoolkey);
    planmgr->GetDiskScheStat(sourcekey, &sourcedisk);
    planmgr->GetDiskScheStat(targetkey, &targetdisk);
    subpoolplans->SetFlagComb(SubPoolPlans::SPF_no_more_plan);
    ret = planmgr->Complete(subpoolkey, sourcekey, targetkey, true, YWB_SUCCESS);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(sourcedisk->TestFlag(DiskScheStat::DSSS_none), true);
    ASSERT_EQ(targetdisk->TestFlag(DiskScheStat::DSSS_none), true);
}

TEST_F(PlanManagerTest, Complete_NoMoreTrigger_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    DiskKey targetkey;
    PlanManager* planmgr = NULL;
    DiskScheStat* sourcedisk = NULL;
    DiskScheStat* targetdisk = NULL;

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    sourcekey.SetSubPoolId(subpoolid);
    sourcekey.SetDiskId(3 * (subpoolid) - 1);
    targetkey.SetSubPoolId(subpoolid);
    targetkey.SetDiskId(3 * (subpoolid) - 2);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    planmgr->SubmitFirstBatch(subpoolkey);
    planmgr->GetDiskScheStat(sourcekey, &sourcedisk);
    planmgr->GetDiskScheStat(targetkey, &targetdisk);
    ret = planmgr->Complete(subpoolkey, sourcekey, targetkey, false, YWB_SUCCESS);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(sourcedisk->TestFlag(DiskScheStat::DSSS_none), true);
    ASSERT_EQ(targetdisk->TestFlag(DiskScheStat::DSSS_none), true);
}

TEST_F(PlanManagerTest, Complete_TriggerMore_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    DiskKey targetkey;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    sourcekey.SetSubPoolId(subpoolid);
    sourcekey.SetDiskId(3 * (subpoolid) - 1);
    targetkey.SetSubPoolId(subpoolid);
    targetkey.SetDiskId(3 * (subpoolid) - 2);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    planmgr->SubmitFirstBatch(subpoolkey);
    ret = planmgr->Complete(subpoolkey, sourcekey, targetkey, true, YWB_SUCCESS);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(PlanManagerTest, Complete_TriggerMoreButFailToScheduleNextPlan_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    DiskKey targetkey;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    sourcekey.SetSubPoolId(subpoolid);
    sourcekey.SetDiskId(3 * (subpoolid) - 1);
    targetkey.SetSubPoolId(subpoolid);
    targetkey.SetDiskId(3 * (subpoolid) - 2);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    planmgr->SubmitFirstBatch(subpoolkey);
    DrainSubPoolWiseDiskAvailPerf(subpoolkey, IOStat::IOST_raw);
    ret = planmgr->Complete(subpoolkey, sourcekey, targetkey, true, YWB_SUCCESS);
    ASSERT_EQ(ret, YWB_SUCCESS);
    set = planmgr->CheckInDelayedSubPoolsForNext(subpoolkey);
    ASSERT_EQ(set, true);
}

TEST_F(PlanManagerTest, Complete_TriggerMoreButNoMorePlan_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t set = false;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey sourcekey;
    DiskKey targetkey;
    PlanManager* planmgr = NULL;
    Error* err = NULL;

    planmgr = GetPlanManager();
    err = GetError();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    sourcekey.SetSubPoolId(subpoolid);
    sourcekey.SetDiskId(3 * (subpoolid) - 1);
    targetkey.SetSubPoolId(subpoolid);
    targetkey.SetDiskId(3 * (subpoolid) - 2);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    planmgr->SubmitFirstBatch(subpoolkey);
    err->SetPLNErr(Error::PLNE_subpool_no_more_plan);
    ret = planmgr->Complete(subpoolkey, sourcekey, targetkey, true, YWB_SUCCESS);
    ASSERT_EQ(ret, YWB_SUCCESS);
    /*clear error*/
    err->SetPLNErr(Error::COM_success);
    set = planmgr->CheckInDelayedSubPoolsForNext(subpoolkey);
    ASSERT_EQ(set, false);
}

TEST_F(PlanManagerTest, Destroy_ByDiskKey_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ret = planmgr->Destroy(diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, Destroy_ByDiskKey_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    PlanManager* planmgr = NULL;
    DiskScheStat* diskstat = NULL;

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(3 * subpoolid - 1);

    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    ret = planmgr->Destroy(diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = planmgr->GetDiskScheStat(diskkey, &diskstat);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, Destroy_BySubPoolKey_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    PlanManager* planmgr = NULL;

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 100);

    ret = planmgr->Destroy(subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, Destroy_BySubPoolKey_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskstat = NULL;
    ywb_uint64_t diskids[6] = {20, 22, 23, 24, 25, 26};

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    diskkey.SetSubPoolId(subpoolid);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);

    ret = planmgr->Destroy(subpoolkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    ASSERT_EQ(ret, YFS_ENOENT);

    while(diskloop < 6)
    {
        diskkey.SetDiskId(diskids[diskloop++]);
        ret = planmgr->GetDiskScheStat(diskkey, &diskstat);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(PlanManagerTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t plannum = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskstat = NULL;
    ywb_uint32_t subpoolids[4] = {1, 2, 3, 7};
    ywb_uint64_t diskids[6] = {20, 22, 23, 24, 25, 26};

    planmgr = GetPlanManager();
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);

    ret = planmgr->Destroy();
    ASSERT_EQ(ret, YWB_SUCCESS);

    while(subpoolloop < 4)
    {
        subpoolkey.SetSubPoolId(subpoolids[subpoolloop]);
        subpoolloop++;
        ret = planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
        ASSERT_EQ(ret, YFS_ENOENT);
    }

    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + 6);
    while(diskloop < 6)
    {
        diskkey.SetDiskId(diskids[diskloop++]);
        ret = planmgr->GetDiskScheStat(diskkey, &diskstat);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(PlanManagerTest, Reset_DiskNotChosen_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskstat = NULL;
    Plan* plan = NULL;

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(3 * subpoolid - 1);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    planmgr->GetDiskScheStat(diskkey, &diskstat);
    diskstat->SetFlagComb(DiskScheStat::DSSS_none);
    ret = planmgr->Reset(ywb_false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = planmgr->GetDiskScheStat(diskkey, &diskstat);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolplans->GetFirstPlan(&plan);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, Reset_DiskChosenButCurrentlyNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    LogicalConfig* config = NULL;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskstat = NULL;
    Plan* plan = NULL;

    config = GetConfig();
    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(3 * subpoolid - 1);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    config->RemoveDisk(diskkey);
    ret = planmgr->Reset(ywb_false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = planmgr->GetDiskScheStat(diskkey, &diskstat);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolplans->GetFirstPlan(&plan);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, Reset_DiskNotExistAndNotChosen_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    LogicalConfig* config = NULL;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskstat = NULL;
    Plan* plan = NULL;

    config = GetConfig();
    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(3 * subpoolid - 1);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    planmgr->GetDiskScheStat(diskkey, &diskstat);
    diskstat->SetFlagComb(DiskScheStat::DSSS_none);
    config->RemoveDisk(diskkey);
    ret = planmgr->Reset(ywb_false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = planmgr->GetDiskScheStat(diskkey, &diskstat);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = subpoolplans->GetFirstPlan(&plan);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(PlanManagerTest, Reset_DiskScheStatIsMarkedAsDeleted_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t plannum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    PlanManager* planmgr = NULL;
    SubPoolPlans* subpoolplans = NULL;
    DiskScheStat* diskstat = NULL;
    Plan* plan = NULL;

    planmgr = GetPlanManager();
    subpoolid = Constant::SUBPOOL_ID + 6;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);
    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(3 * subpoolid - 1);

    planmgr->GetSubPoolPlans(subpoolkey, &subpoolplans);
    planmgr->ScheduleFirstBatch(subpoolkey, &plannum);
    planmgr->GetDiskScheStat(diskkey, &diskstat);
    diskstat->SetFlagComb(DiskScheStat::DSSS_deleted);
    ret = planmgr->Reset(ywb_false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = planmgr->GetDiskScheStat(diskkey, &diskstat);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = subpoolplans->GetFirstPlan(&plan);
    ASSERT_EQ(ret, YFS_ENOENT);
}

/* vim:set ts=4 sw=4 expandtab */
