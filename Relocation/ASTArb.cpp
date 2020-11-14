#include "AST/ASTTime.hpp"
#include "AST/ASTMigration.hpp"
#include "AST/ASTArb.hpp"

void
BaseArbitratee::OBJKey2ArbitrateeKey(OBJKey* objkey,
        ArbitrateeKey* arbitrateekey)
{
    *arbitrateekey = *objkey;
}

void
BaseArbitratee::ArbitrateeKey2OBJKey(
        ArbitrateeKey* arbitrateekey, OBJKey* objkey)
{
    *objkey = *arbitrateekey;
}

ywb_status_t
SubPoolArbitratee::GetArbitratee(ArbitrateeKey& key, Arbitratee** val)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter;

    iter = mItems.find(key);
    if(iter != mItems.end())
    {
        *val = iter->second;
    }
    else
    {
        *val = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolArbitratee::AddArbitratee(ArbitrateeKey& key, Arbitratee* val)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<ArbitrateeKey, Arbitratee*,
            ArbitrateeKeyCmp>::iterator, bool> pairret;

    pairret = mItems.insert(make_pair(key, val));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }
    else
    {
        mItems2.push_back(val);
    }

    return ret;
}

ywb_status_t
SubPoolArbitratee::RemoveArbitratee(ArbitrateeKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter;
    Arbitratee* arbitratee = NULL;

    iter = mItems.find(key);
    if(iter != mItems.end())
    {
        arbitratee = iter->second;
        mItems.erase(iter);
        mItems2.remove(arbitratee);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolArbitratee::GetFirstArbitratee(Arbitratee** val)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mItems2.empty())
    {
        mIt2 = mItems2.begin();
        *val = *mIt2;
        mIt2++;
    }
    else
    {
        *val = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolArbitratee::GetNextArbitratee(Arbitratee** val)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIt2 != mItems2.end())
    {
        *val = *mIt2;
        mIt2++;
    }
    else
    {
        *val = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_bool_t SubPoolArbitratee::Empty()
{
    return mItems.empty();
}

ywb_status_t
SubPoolArbitratee::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    Arbitratee* arbitratee = NULL;

    if(!mItems.empty())
    {
        mItems.clear();
    }

    mIt2 = mItems2.begin();
    for(; mIt2 != mItems2.end(); )
    {
        arbitratee = *mIt2;
        mIt2 = mItems2.erase(mIt2);
        delete arbitratee;
        arbitratee = NULL;
    }

    return ret;
}

ywb_status_t
Arbitrator::GetSubPoolArbitrateeFromIn(SubPoolKey& subpoolkey,
        SubPoolArbitratee** subpoolarbitratees)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator iter;

    iter = mItemsIn.find(subpoolkey);
    if(iter != mItemsIn.end())
    {
        *subpoolarbitratees = iter->second;
    }
    else
    {
        *subpoolarbitratees = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Arbitrator::GetSubPoolArbitrateeFromOut(SubPoolKey& subpoolkey,
        SubPoolArbitratee** subpoolarbitratees)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator iter;

    iter = mItemsOut.find(subpoolkey);
    if(iter != mItemsOut.end())
    {
        *subpoolarbitratees = iter->second;
    }
    else
    {
        *subpoolarbitratees = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Arbitrator::AddArbitrateeIn(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey, Arbitratee* val)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator iter;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    iter = mItemsIn.find(subpoolkey);
    if(iter != mItemsIn.end())
    {
        subpoolarbitratees = iter->second;
    }
    else
    {
        subpoolarbitratees = new SubPoolArbitratee();
        if(NULL == subpoolarbitratees)
        {
            ret = YFS_EOUTOFMEMORY;
            return ret;
        }

        mItemsIn.insert(make_pair(subpoolkey, subpoolarbitratees));
    }

    ret = subpoolarbitratees->AddArbitratee(arbitrateekey, val);
    return ret;
}

ywb_status_t
Arbitrator::RemoveArbitrateeIn(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator iter;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    iter = mItemsIn.find(subpoolkey);
    if(iter != mItemsIn.end())
    {
        subpoolarbitratees = iter->second;
        ret = subpoolarbitratees->RemoveArbitratee(arbitrateekey);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Arbitrator::RemoveSubPoolArbitrateeIn(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator iter;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    iter = mItemsIn.find(subpoolkey);
    if(iter != mItemsIn.end())
    {
        subpoolarbitratees = iter->second;
        mItemsIn.erase(iter);
        delete subpoolarbitratees;
        subpoolarbitratees = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Arbitrator::AddArbitrateeOut(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey, Arbitratee* val)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator iter;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    iter = mItemsOut.find(subpoolkey);
    if(iter != mItemsOut.end())
    {
        subpoolarbitratees = iter->second;
    }
    else
    {
        subpoolarbitratees = new SubPoolArbitratee();
        if(NULL == subpoolarbitratees)
        {
            ret = YFS_EOUTOFMEMORY;
            return ret;
        }

        mItemsOut.insert(make_pair(subpoolkey, subpoolarbitratees));
    }

    ret = subpoolarbitratees->AddArbitratee(arbitrateekey, val);
    return ret;
}

ywb_status_t
Arbitrator::RemoveArbitrateeOut(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator iter;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    iter = mItemsOut.find(subpoolkey);
    if(iter != mItemsOut.end())
    {
        subpoolarbitratees = iter->second;
        ret = subpoolarbitratees->RemoveArbitratee(arbitrateekey);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Arbitrator::RemoveSubPoolArbitrateeOut(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator iter;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    iter = mItemsOut.find(subpoolkey);
    if(iter != mItemsOut.end())
    {
        subpoolarbitratees = iter->second;
        mItemsOut.erase(iter);
        delete subpoolarbitratees;
        subpoolarbitratees = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

//ywb_status_t
//Arbitrator::AddNotCompletedMigratingItem(
//        ArbitrateeKey& key, Arbitratee* arbitratee)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    pair<map<ArbitrateeKey, Arbitratee*,
//        ArbitrateeKeyCmp>::iterator, bool> pairret;
//
//    YWB_ASSERT(arbitratee != NULL);
//    pairret = mNotCompletedMigratingItems.insert(make_pair(key, arbitratee));
//    if(!pairret.second)
//    {
//        ret = YFS_EEXIST;
//    }
//
//    return ret;
//}

//ywb_status_t
//Arbitrator::GetNotCompletedMigratingItem(
//        ArbitrateeKey& key, Arbitratee** arbitratee)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter;
//
//    iter = mNotCompletedMigratingItems.find(key);
//    if(iter != mNotCompletedMigratingItems.end())
//    {
//        *arbitratee = iter->second;
//    }
//    else
//    {
//        *arbitratee = NULL;
//        ret = YFS_ENOENT;
//    }
//
//    return ret;
//}

//ywb_status_t
//Arbitrator::RemoveNotCompletedMigratingItem(ArbitrateeKey& key)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter;
//    Arbitratee* arbitratee = NULL;
//
//    iter = mNotCompletedMigratingItems.find(key);
//    if(iter != mNotCompletedMigratingItems.end())
//    {
//        arbitratee = iter->second;
//        mNotCompletedMigratingItems.erase(iter);
//        delete arbitratee;
//        arbitratee = NULL;
//    }
//    else
//    {
//        ret = YFS_ENOENT;
//    }
//
//    return ret;
//}

ywb_bool_t
Arbitrator::CheckConfigChange(SubPoolKey& subpoolkey,
        Arbitratee* arbitratee)
{
    bool exist = false;
    AST* ast = NULL;
    Error* err = NULL;
    LogicalConfig* config = NULL;
    OBJKey* objkey = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;

    GetAST(&ast);
    ast->GetError(&err);
    ast->GetLogicalConfig(&config);

    err->SetEXEErr(Error::COM_success);
    exist = config->CheckSubPoolExistence(subpoolkey);
    /*subpool exist*/
    if(exist)
    {
//        config->GetSubPool(subpoolkey, &subpool);
//        YWB_ASSERT(subpool != NULL);
        /*
         * FIXME: if tier combination at the moment the advice is generated
         * and the one in current cycle are different, then the configuration
         * is taken as changed.
         **/
//        if((subpool->GetLastTierComb() != subpool->GetCurTierComb()))
//        {
//            return true;
//        }

        arbitratee->GetOBJKey(&objkey);
        sourcekey.SetDiskId(objkey->GetStorageId());
        sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
        targetkey.SetDiskId(arbitratee->GetTargetID());
        targetkey.SetSubPoolId(subpoolkey.GetSubPoolId());

        /*check existence of source disk*/
        if(!config->CheckDiskExistence(sourcekey))
        {
            err->SetEXEErr(Error::LCFE_source_disk_not_exist);
            return true;
        }

        /*check existence of target disk*/
        if(!config->CheckDiskExistence(targetkey))
        {
            err->SetEXEErr(Error::LCFE_target_disk_not_exist);
            return true;
        }

        /*
         * will not check the OBJ's existence here, offload the check
         * to migrator instead
         **/
//        if(!config->CheckOBJExistence(*objkey))
//        {
//            err->SetEXEErr(Error::LCF_EOBJNOTEXIST);
//            return true;
//        }
    }
    else
    {
        err->SetEXEErr(Error::LCFE_subpool_not_exist);
    }

    return !exist;
}

ywb_bool_t
Arbitrator::CheckConflict(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey, Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Error* err = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee2 = NULL;

    GetAST(&ast);
    ast->GetError(&err);

    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee2);
        /*find a conflicted arbitratee, then keep the one with higher priority*/
        if((YWB_SUCCESS == ret) && (arbitratee2 != NULL))
        {
            AST* ast = NULL;
            PlanManager* planmanager = NULL;
            SubPoolPlans* subpoolplans = NULL;
            SubPoolPlanDirStat* subpoolplandirstat = NULL;
            ywb_uint32_t direction = Plan::PD_cnt;
            ywb_uint32_t direction2 = Plan::PD_cnt;
            ChunkIOStat* stat = NULL;

            GetAST(&ast);
            YWB_ASSERT(ast != NULL);
            ast->GetPlanManager(&planmanager);
            YWB_ASSERT(planmanager != NULL);
            planmanager->GetSubPoolPlans(subpoolkey, &subpoolplans);
            YWB_ASSERT(subpoolplans != NULL);
            subpoolplans->GetSubPoolPlanDirStat(&subpoolplandirstat);
            YWB_ASSERT(subpoolplandirstat != NULL);
            direction = arbitratee->GetDirection();
            direction2 = arbitratee2->GetDirection();

            /*new arbitratee has higher priority*/
            if(direction == subpoolplandirstat->
                    GetHigherPlanDirection(direction, direction2))
            {
                arbitratee->GetIOStat(&stat);
                arbitratee2->SetTargetId(arbitratee->GetTargetID());
                arbitratee2->SetIOStat(*stat);
                arbitratee2->SetStatType(arbitratee->GetStatType());
                arbitratee2->SetDirection(arbitratee->GetDirection());
                arbitratee2->SetCycle(arbitratee->GetCycle());
                arbitratee2->SetEpoch(arbitratee->GetEpoch());
            }

            err->SetEXEErr(Error::ARBE_conflict);
            return true;
        }
    }

    return false;
}

ywb_bool_t
Arbitrator::CheckFrequentUpDown(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Error* err = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    GetAST(&ast);
    ast->GetError(&err);

    ret = GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        /*
         * an arbitratee with the same OBJ has been migrated, then check the
         * lease time, if lease expires, then new arbitratee can be migrated
         **/
        if((YWB_SUCCESS == ret) && (arbitratee != NULL))
        {
            AST* ast = NULL;
            TimeManager* timemanager = NULL;
            CycleManager* cyclemanager = NULL;
            ywb_uint64_t currentrelativecycle = 0;

            GetAST(&ast);
            ast->GetTimeManager(&timemanager);
            timemanager->GetCycleManager(&cyclemanager);
            currentrelativecycle = cyclemanager->GetCycleRelative();

            /*expires*/
            if(currentrelativecycle - arbitratee->GetCycle() >
                    GetLeaseTime(arbitratee->GetDirection()))
            {
                /*remove the expired arbitratee*/
                RemoveArbitrateeOut(subpoolkey, arbitrateekey);
                delete arbitratee;
                arbitratee = NULL;
                return false;
            }
            else
            {
                err->SetEXEErr(Error::ARBE_frequent_up_and_down);
                return true;
            }
        }
    }

    return false;
}

/*FIXME: there should be a more sophisticated algorithm*/
ywb_bool_t
Arbitrator::CheckBenefitOverCost(SubPoolKey& subpoolkey, ArbitrateeKey& arbitrateekey)
{
    return true;
}

ywb_status_t
Arbitrator::ReChooseMigrationTarget(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey, ywb_uint32_t perfType)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t currentrelativecycle = 0;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;
    AST* ast = NULL;
    TimeManager* timemanager = NULL;
    CycleManager* cyclemanager = NULL;

    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        if((YWB_SUCCESS == ret) && (arbitratee != NULL))
        {
            GetAST(&ast);
            YWB_ASSERT(ast != NULL);
            ast->GetTimeManager(&timemanager);
            YWB_ASSERT(timemanager != NULL);
            timemanager->GetCycleManager(&cyclemanager);
            YWB_ASSERT(cyclemanager != NULL);

            currentrelativecycle = cyclemanager->GetCycleRelative();
            if((currentrelativecycle - arbitratee->GetCycle()) >=
                    Constant::PLAN_BIND_DISK_TIME_LIMIT)
            {
                subpoolarbitratees->RemoveArbitratee(arbitrateekey);
                delete arbitratee;
                arbitratee = NULL;
                /*FIXME: what shall be the error code, YFS_ETIMEDOUT?*/
                ret = YFS_ENOENT;
            }
            else
            {
                ret = YWB_SUCCESS;
            }
        }
        else
        {
            ret = YFS_ENOENT;
        }
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Arbitrator::Reserve(SubPoolKey& subpoolkey, Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t stattype = IOStat::IOST_cnt;
    ywb_int32_t iosrequired = 0;
    ywb_int32_t bwrequired = 0;
    ywb_int32_t caprequired = 0;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfManager* perfmanager = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    OBJKey* objkey = NULL;
    ChunkIOStat* stat = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;

    YWB_ASSERT(arbitratee != NULL);
    if(CheckConfigChange(subpoolkey, arbitratee))
    {
        ast_log_debug("arbitratee@" << arbitratee << "'s config change");
        return YFS_EINVAL;
    }

    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        GetAST(&ast);
        ast->GetLogicalConfig(&config);
        ast->GetPerfManager(&perfmanager);
        arbitratee->GetOBJKey(&objkey);
        sourcekey.SetDiskId(objkey->GetStorageId());
        sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
        targetkey.SetDiskId(arbitratee->GetTargetID());
        targetkey.SetSubPoolId(subpoolkey.GetSubPoolId());
        perfmanager->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolperf);
        arbitratee->GetIOStat(&stat);
        stattype = arbitratee->GetStatType();
        iosrequired = stat->GetRndReadIOs() + stat->GetRndWriteIOs() +
                stat->GetSeqReadIOs() + stat->GetSeqWriteIOs();
        bwrequired = stat->GetRndReadBW() + stat->GetRndWriteBW() +
                stat->GetSeqReadBW() + stat->GetSeqWriteBW();
        caprequired = arbitratee->GetCap();
        /*reserve on source disk*/
        ret = subpoolperf->Reserve(sourcekey, stattype,
                iosrequired, bwrequired, (0 - caprequired));
        /*reserve on target disk*/
        ret = subpoolperf->Reserve(targetkey, stattype,
                iosrequired, bwrequired, caprequired);
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
Arbitrator::Reserve(SubPoolKey& subpoolkey, ArbitrateeKey& arbitrateekey)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t stattype = IOStat::IOST_cnt;
    ywb_int32_t iosrequired = 0;
    ywb_int32_t bwrequired = 0;
    ywb_int32_t caprequired = 0;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfManager* perfmanager = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;
    ChunkIOStat* stat = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;

    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        if((YWB_SUCCESS == ret) && (arbitratee != NULL))
        {
            GetAST(&ast);
            ast->GetLogicalConfig(&config);
            ast->GetPerfManager(&perfmanager);
            perfmanager->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolperf);

            if(CheckConfigChange(subpoolkey, arbitratee))
            {
                return YFS_EINVAL;
            }
            else
            {
                arbitratee->GetOBJKey(&objkey);
                sourcekey.SetDiskId(objkey->GetStorageId());
                sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
                targetkey.SetDiskId(arbitratee->GetTargetID());
                targetkey.SetSubPoolId(subpoolkey.GetSubPoolId());
                arbitratee->GetIOStat(&stat);
                stattype = arbitratee->GetStatType();
                iosrequired = stat->GetRndReadIOs() + stat->GetRndWriteIOs() +
                        stat->GetSeqReadIOs() + stat->GetSeqWriteIOs();
                bwrequired = stat->GetRndReadBW() + stat->GetRndWriteBW() +
                        stat->GetSeqReadBW() + stat->GetSeqWriteBW();
                caprequired = arbitratee->GetCap();
                /*reserve on source disk*/
                ret = subpoolperf->Reserve(sourcekey, stattype,
                        iosrequired, bwrequired, (0 - caprequired));
                /*reserve on target disk*/
                ret = subpoolperf->Reserve(targetkey, stattype,
                        iosrequired, bwrequired, caprequired);
            }
        }
        else
        {
            ret = YFS_ENOENT;
        }
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
Arbitrator::UnReserve(SubPoolKey& subpoolkey, Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t stattype = IOStat::IOST_cnt;
    ywb_uint32_t iosrequired = 0;
    ywb_uint32_t bwrequired = 0;
    ywb_uint32_t caprequired = 0;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    PerfManager* perfmanager = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    OBJKey* objkey = NULL;
    ChunkIOStat* stat = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;

    YWB_ASSERT(arbitratee != NULL);
    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        GetAST(&ast);
        ast->GetLogicalConfig(&config);
        ast->GetPerfManager(&perfmanager);
        ret = perfmanager->GetSubPoolDiskAvailPerfRanking(
                subpoolkey, &subpoolperf);
        if(YWB_SUCCESS != ret)
        {
            return ret;
        }

        arbitratee->GetOBJKey(&objkey);
        sourcekey.SetDiskId(objkey->GetStorageId());
        sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
        targetkey.SetDiskId(arbitratee->GetTargetID());
        targetkey.SetSubPoolId(subpoolkey.GetSubPoolId());
        arbitratee->GetIOStat(&stat);
        stattype = arbitratee->GetStatType();
        iosrequired = stat->GetRndReadIOs() + stat->GetRndWriteIOs() +
                stat->GetSeqReadIOs() + stat->GetSeqWriteIOs();
        bwrequired = stat->GetRndReadBW() + stat->GetRndWriteBW() +
                stat->GetSeqReadBW() + stat->GetSeqWriteBW();
        caprequired = arbitratee->GetCap();
        /*unreserve on source disk*/
        ret = subpoolperf->UnReserve(sourcekey, stattype,
                iosrequired, bwrequired, (0 - caprequired));
        /*unreserve on target disk*/
        ret = subpoolperf->UnReserve(targetkey, stattype,
                iosrequired, bwrequired, caprequired);
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
Arbitrator::UnReserve(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t stattype = IOStat::IOST_cnt;
    ywb_uint32_t iosrequired = 0;
    ywb_uint32_t bwrequired = 0;
    ywb_uint32_t caprequired = 0;
    AST* ast = NULL;
    PerfManager* perfmanager = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;
    ChunkIOStat* stat = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;

    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        if((YWB_SUCCESS == ret) && (arbitratee != NULL))
        {
            GetAST(&ast);
            ast->GetPerfManager(&perfmanager);
            ret = perfmanager->GetSubPoolDiskAvailPerfRanking(
                    subpoolkey, &subpoolperf);
            if(YWB_SUCCESS != ret)
            {
                return ret;
            }

            arbitratee->GetOBJKey(&objkey);
            sourcekey.SetDiskId(objkey->GetStorageId());
            sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
            targetkey.SetDiskId(arbitratee->GetTargetID());
            targetkey.SetSubPoolId(subpoolkey.GetSubPoolId());
            arbitratee->GetIOStat(&stat);
            stattype = arbitratee->GetStatType();
            iosrequired = stat->GetRndReadIOs() + stat->GetRndWriteIOs() +
                    stat->GetSeqReadIOs() + stat->GetSeqWriteIOs();
            bwrequired = stat->GetRndReadBW() + stat->GetRndWriteBW() +
                    stat->GetSeqReadBW() + stat->GetSeqWriteBW();
            caprequired = arbitratee->GetCap();
            /*unreserve on source disk*/
            ret = subpoolperf->UnReserve(sourcekey, stattype,
                    iosrequired, bwrequired, (0 - caprequired));
            /*unreserve on target disk*/
            ret = subpoolperf->UnReserve(targetkey, stattype,
                    iosrequired, bwrequired, caprequired);
        }
        else
        {
            ret = YFS_ENOENT;
        }
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
Arbitrator::Consume(SubPoolKey& subpoolkey, Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t stattype = IOStat::IOST_cnt;
    ywb_uint32_t iosrequired = 0;
    ywb_uint32_t bwrequired = 0;
    ywb_uint32_t caprequired = 0;
    AST* ast = NULL;
    PerfManager* perfmanager = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    OBJKey* objkey = NULL;
    ChunkIOStat* stat = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;

    YWB_ASSERT(arbitratee != NULL);
    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        GetAST(&ast);
        ast->GetPerfManager(&perfmanager);
        perfmanager->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolperf);
        arbitratee->GetOBJKey(&objkey);
        sourcekey.SetDiskId(objkey->GetStorageId());
        sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
        targetkey.SetDiskId(arbitratee->GetTargetID());
        targetkey.SetSubPoolId(subpoolkey.GetSubPoolId());
        arbitratee->GetIOStat(&stat);
        stattype = arbitratee->GetStatType();
        iosrequired = stat->GetRndReadIOs() + stat->GetRndWriteIOs() +
                stat->GetSeqReadIOs() + stat->GetSeqWriteIOs();
        bwrequired = stat->GetRndReadBW() + stat->GetRndWriteBW() +
                stat->GetSeqReadBW() + stat->GetSeqWriteBW();
        caprequired = arbitratee->GetCap();
        /*consume on source disk*/
        ret = subpoolperf->Consume(sourcekey, stattype,
                iosrequired, bwrequired, (0 - caprequired));
        /*consume on target disk*/
        ret = subpoolperf->Consume(targetkey, stattype,
                iosrequired, bwrequired, caprequired);
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
Arbitrator::Consume(SubPoolKey& subpoolkey, ArbitrateeKey& arbitrateekey)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t stattype = IOStat::IOST_cnt;
    ywb_uint32_t iosrequired = 0;
    ywb_uint32_t bwrequired = 0;
    ywb_uint32_t caprequired = 0;
    AST* ast = NULL;
    PerfManager* perfmanager = NULL;
    SubPoolDiskAvailPerfRanking* subpoolperf = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;
    ChunkIOStat* stat = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;

    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        if((YWB_SUCCESS == ret) && (arbitratee != NULL))
        {
            GetAST(&ast);
            ast->GetPerfManager(&perfmanager);
            perfmanager->GetSubPoolDiskAvailPerfRanking(subpoolkey, &subpoolperf);
            arbitratee->GetOBJKey(&objkey);
            sourcekey.SetDiskId(objkey->GetStorageId());
            sourcekey.SetSubPoolId(subpoolkey.GetSubPoolId());
            targetkey.SetDiskId(arbitratee->GetTargetID());
            targetkey.SetSubPoolId(subpoolkey.GetSubPoolId());
            arbitratee->GetIOStat(&stat);
            stattype = arbitratee->GetStatType();
            iosrequired = stat->GetRndReadIOs() + stat->GetRndWriteIOs() +
                    stat->GetSeqReadIOs() + stat->GetSeqWriteIOs();
            bwrequired = stat->GetRndReadBW() + stat->GetRndWriteBW() +
                    stat->GetSeqReadBW() + stat->GetSeqWriteBW();
            caprequired = arbitratee->GetCap();
            /*consume on source disk*/
            ret = subpoolperf->Consume(sourcekey, stattype,
                    iosrequired, bwrequired, (0 - caprequired));
            /*consume on target disk*/
            ret = subpoolperf->Consume(targetkey, stattype,
                    iosrequired, bwrequired, caprequired);
        }
        else
        {
            ret = YFS_ENOENT;
        }
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
Arbitrator::ReceivePlan(SubPoolKey& subpoolkey, Plan* plan)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t currentcycle = 0;
    AST* ast = NULL;
    Error* err = NULL;
    TimeManager* timemgr = NULL;
    CycleManager* cyclemgr = NULL;
    Arbitrator* arb = NULL;
    OBJKey* objkey = NULL;
    ArbitrateeKey arbitrateekey;
    ChunkIOStat* stat = NULL;
    Arbitratee* arbitratee = NULL;

    YWB_ASSERT(plan != NULL);
    GetAST(&ast);
    ast->GetError(&err);
    ast->GetArb(&arb);
    ast->GetTimeManager(&timemgr);
    timemgr->GetCycleManager(&cyclemgr);
    currentcycle = cyclemgr->GetCycleRelative();
    plan->GetOBJKey(&objkey);
    plan->GetChunkIOStat(&stat);

    arbitratee = new Arbitratee(*objkey, plan->GetTargetId(),
            subpoolkey, *stat, plan->GetStatType(), plan->GetDirection(),
            plan->GetCap(), currentcycle, plan->GetEpoch());
    if(NULL == arbitratee)
    {
        ret = YFS_EOUTOFMEMORY;
        return ret;
    }

    ast_log_debug("Receive plan@" << plan
            << " <-> arbitratee@" << arbitratee
            << ", obj:[" << objkey->GetStorageId()
            << ", " << objkey->GetInodeId()
            << ", " << objkey->GetChunkId()
            << ", " << objkey->GetChunkIndex()
            << ", " << objkey->GetChunkVersion()
            << "], targetdisk: "<< plan->GetTargetId()
            << ", direction: " << plan->GetDirection()
            << ", epoch: " << plan->GetEpoch());
    arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);
    /*if fail any of the check, the plan will be discarded*/
    if((CheckConfigChange(subpoolkey, arbitratee)) ||
            CheckConflict(subpoolkey, arbitrateekey, arbitratee) ||
            CheckFrequentUpDown(subpoolkey, arbitrateekey) ||
            !CheckBenefitOverCost(subpoolkey, arbitrateekey))
    {
        /*FIXME: what shall be the error code*/
        ret = YFS_EINVAL;

        /*subpool does not exist*/
        if(err->GetEXEErr() == Error::LCFE_subpool_not_exist)
        {
            RemoveSubPoolArbitrateeIn(subpoolkey);
            RemoveSubPoolArbitrateeOut(subpoolkey);
        }

        /*discard the plan*/
        delete arbitratee;
        arbitratee = NULL;

        ast_log_debug("arbitratee@" << arbitratee << "fails arbitrator check");
    }
    else
    {
        ret = arb->AddArbitrateeIn(subpoolkey, arbitrateekey, arbitratee);
        if(ret != YWB_SUCCESS)
        {
            delete arbitratee;
            arbitratee = NULL;

            ast_log_debug("arbitratee@" << arbitratee << "fails to be"
                    << "added into IN list, ret " << ret);
        }
        else
        {
            /*do reservation*/
            ret = Reserve(subpoolkey, arbitratee);
            if(YWB_SUCCESS == ret)
            {
                ast_log_debug("arbitratee@" << arbitratee << " successfully reserved");
                /*submit to migration module*/
                ret = Submit(subpoolkey, arbitratee);
                /*
                 * fail to submit(for configuration change, or out of
                 * memory and so on), then unreserve it
                 **/
                if((ret != YWB_SUCCESS))
                {
                    ast_log_debug("arbitratee@" << arbitratee << " unsuccessfully submitted");
                    UnReserve(subpoolkey, arbitratee);
                }
            }

            if(ret != YWB_SUCCESS)
            {
                arb->RemoveArbitrateeIn(subpoolkey, arbitrateekey);
                delete arbitratee;
                arbitratee = NULL;
            }
        }
    }

    return ret;
}

ywb_status_t
Arbitrator::Submit(SubPoolKey& subpoolkey, Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Migration* migration = NULL;

    YWB_ASSERT(arbitratee != NULL);
    GetAST(&ast);
    ast->GetMigration(&migration);

    /*mark it as migrating*/
    arbitratee->SetFlagUniq(BaseArbitratee::BAF_submitted);
    ret = migration->ReceiveArbitratee(arbitratee);
    return ret;
}

ywb_status_t
Arbitrator::Complete(SubPoolKey& subpoolkey, ArbitrateeKey& arbitrateekey,
        ywb_bool_t curwindow, ywb_status_t migstatus)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

//    if(curwindow)
//    {
    /*remove arbitratee from IN and add into OUT*/
    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        if((YWB_SUCCESS == ret) && (arbitratee != NULL))
        {
            ast_log_debug("arbitratee@" << arbitratee << " migration"
                    << "completed, migration status: " << migstatus);

            if(curwindow)
            {
                /*we are sure reservation has been done for arbitratee of current window*/
                ret = Consume(subpoolkey, arbitrateekey);
            }
            else
            {
                /*
                 * consume will decrease reservation amount internally,
                 * but for arbitratee of previous decision window, disk
                 * performance has been re-calculated at the start of
                 * current decision window, so must re-reserve before
                 * consuming.
                 * */
                ret = Reserve(subpoolkey, arbitrateekey);
                if(YWB_SUCCESS == ret)
                {
                    ret = Consume(subpoolkey, arbitrateekey);
                }
                else
                {
                    /*just ignore reservation failure*/
                    ret = YWB_SUCCESS;
                }
            }

            if(YWB_SUCCESS == migstatus)
            {
                arbitratee->SetFlagUniq(BaseArbitratee::BAF_migrated);
                RemoveArbitrateeIn(subpoolkey, arbitrateekey);
                AddArbitrateeOut(subpoolkey, arbitrateekey, arbitratee);
            }
            else
            {
                /*remove arbitratee from IN and de-construct it*/
                arbitratee->SetFlagUniq(BaseArbitratee::BAF_migration_failed);
                subpoolarbitratees->RemoveArbitratee(arbitrateekey);
                delete arbitratee;
                arbitratee = NULL;
            }
        }
    }
//    }
//    else
//    {
//        /*remove arbitratee from not-completed items*/
//        ret = GetNotCompletedMigratingItem(arbitrateekey, &arbitratee);
//        if(YWB_SUCCESS == ret)
//        {
//            arbitratee->SetFlagUniq(BaseArbitratee::BAF_migrated);
//            RemoveNotCompletedMigratingItem(arbitrateekey);
//            AddArbitrateeOut(subpoolkey, arbitrateekey, arbitratee);
//            ast_log_debug("arbitratee@" << arbitratee << "of previous window complete migration");
//        }
//    }

    return ret;
}

ywb_status_t
Arbitrator::Expire(SubPoolKey& subpoolkey,
        ArbitrateeKey& arbitrateekey, ywb_bool_t curwindow)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    /*remove arbitratee from IN and de-construct it*/
    ret = GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    if((YWB_SUCCESS == ret) && (subpoolarbitratees != NULL))
    {
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        if((YWB_SUCCESS == ret) && (arbitratee != NULL))
        {
            ast_log_debug("arbitratee@" << arbitratee << " migration expired");
            arbitratee->SetFlagUniq(BaseArbitratee::BAF_expired);
            subpoolarbitratees->RemoveArbitratee(arbitrateekey);
            if(curwindow)
            {
                UnReserve(subpoolkey, arbitrateekey);
            }

            delete arbitratee;
            arbitratee = NULL;
        }
    }

    return ret;
}

void Arbitrator::DrainAllArbitrateesIn()
{
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator initer;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    initer = mItemsIn.begin();
    for(; initer != mItemsIn.end(); )
    {
        subpoolarbitratees = initer->second;
        subpoolarbitratees->Destroy();
        mItemsIn.erase(initer++);
    }
}

void Arbitrator::DrainAllArbitrateesOut()
{
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator outiter;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    outiter = mItemsOut.begin();
    for(; outiter != mItemsOut.end(); )
    {
        subpoolarbitratees = outiter->second;
        subpoolarbitratees->Destroy();
        mItemsOut.erase(outiter++);
    }
}

void Arbitrator::DrainAllArbitratees()
{
    DrainAllArbitrateesIn();
    DrainAllArbitrateesOut();
}

ywb_status_t
Arbitrator::Reset(ywb_bool_t all)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    TimeManager* timemanager = NULL;
    CycleManager* cyclemanager = NULL;
    LogicalConfig* config = NULL;
    ywb_uint64_t currentrelativecycle = 0;
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp>::iterator iter;
    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter2;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;
    ArbitrateeKey arbitrateekey;
    SubPoolKey subpoolkey;

    ast_log_debug("Arbitrator reset");
    if(ywb_false == all)
    {
        GetAST(&ast);
        YWB_ASSERT(ast != NULL);
        ast->GetTimeManager(&timemanager);
        YWB_ASSERT(timemanager != NULL);
        timemanager->GetCycleManager(&cyclemanager);
        YWB_ASSERT(cyclemanager != NULL);
        ast->GetLogicalConfig(&config);
        YWB_ASSERT(config != NULL);
        currentrelativecycle = cyclemanager->GetCycleRelative();

        /*
         * completed/expired migration already been removed from @mItemsIn, so
         * the remaining arbitratees should be retained
         * */
        iter = mItemsIn.begin();
        for(; iter != mItemsIn.end(); )
        {
            subpoolkey = iter->first;
            subpoolarbitratees = iter->second;
            /*
             * clear subpool which does not exist and has no IN arbitratees
             *
             * must clear those not existed subpools if possible, otherwise they
             * will occupy the memory until the AST is destroyed.
             *
             * one subpool may not be cleared even if it is deleted from reported
             * configuration if there are some arbitratees in migrating state, so
             * in next "Reset", should check "existence" rather than "deletion",
             * for "deletion" is cycle-specific, which reflect the configuration
             * change in current cycle only, a deleted subpool in current cycle
             * will not be marked as deleted in next cycle.
             **/
            if((subpoolarbitratees->Empty()) &&
                    (!(config->CheckSubPoolExistence(subpoolkey))))
            {
                mItemsIn.erase(iter++);
                delete subpoolarbitratees;
                subpoolarbitratees = NULL;
            }
            else
            {
                iter++;
            }
        }

        iter = mItemsOut.begin();
        for(; iter != mItemsOut.end(); )
        {
            subpoolkey = iter->first;
            subpoolarbitratees = iter->second;
            ret = subpoolarbitratees->GetFirstArbitratee(&arbitratee);
            /*has OUT arbitratees*/
            while((YWB_SUCCESS == ret) && (arbitratee != NULL))
            {
                /*those arbitratees with lease expiration should be discarded*/
                if((currentrelativecycle - arbitratee->GetCycle()) >
                        GetLeaseTime(arbitratee->GetDirection()))
                {
                    arbitratee->GetOBJKey(&objkey);
                    arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);
                    subpoolarbitratees->RemoveArbitratee(arbitrateekey);
                    delete arbitratee;
                    arbitratee = NULL;
                }

                ret = subpoolarbitratees->GetNextArbitratee(&arbitratee);
            }

            /*subpool does not exist and has no OUT arbitratees*/
            if((subpoolarbitratees->Empty()) &&
                    (!(config->CheckSubPoolExistence(subpoolkey))))
            {
                mItemsOut.erase(iter++);
                delete subpoolarbitratees;
                subpoolarbitratees = NULL;
            }
            else
            {
                iter++;
            }

            ret = YWB_SUCCESS;
        }
    }
    else
    {
        iter = mItemsIn.begin();
        for(; iter != mItemsIn.end(); )
        {
            subpoolarbitratees = iter->second;
            mItemsIn.erase(iter++);
            delete subpoolarbitratees;
            subpoolarbitratees = NULL;
        }

        iter = mItemsOut.begin();
        for(; iter != mItemsOut.end(); )
        {
            subpoolarbitratees = iter->second;
            mItemsOut.erase(iter++);
            delete subpoolarbitratees;
            subpoolarbitratees = NULL;
        }

        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t
Arbitrator::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = Reset(ywb_true);
    mAst = NULL;
    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
