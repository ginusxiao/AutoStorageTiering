/*
 * ASTStoragePolicy.cpp
 *
 *  Created on: 2016年6月6日
 *      Author: Administrator
 */

#include "AST/ASTStoragePolicy.hpp"
#include "AST/ASTSwapOut.hpp"
#include "AST/ASTRelocation.hpp"

ywb_status_t PolicyManager::SetStoragePolicy(SubPoolKey& subpoolkey,
        StoragePolicy& policy, ywb_bool_t overlap)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::pair<std::map<SubPoolKey, StoragePolicy*, SubPoolKeyCmp>::iterator, bool> pairret;
    StoragePolicy* policyp = NULL;
    HUB* hub = NULL;

    hub = GetHUB();
    policyp = new StoragePolicy(policy);
    YWB_ASSERT(policyp != NULL);
    pairret = mPolicies.insert(std::make_pair(subpoolkey, policyp));
    if(true != pairret.second)
    {
        /*modify the storage policy with new one*/
        if(ywb_true == overlap)
        {
            delete (pairret.first)->second;
            pairret.first->second = policyp;
            hub->LaunchUpdateRelocationTypeEvent(subpoolkey,
                    GetRelocationType(subpoolkey));
        }
        else
        {
            delete policyp;
            policyp = NULL;
            ret = YFS_EEXIST;
        }
    }
    else
    {
        hub->LaunchUpdateRelocationTypeEvent(subpoolkey,
                GetRelocationType(subpoolkey));
    }

    return ret;
}

ywb_status_t PolicyManager::SetDefaultStoragePolicy(SubPoolKey& subpoolkey)
{
    StoragePolicy storpolicy;
    return SetStoragePolicy(subpoolkey, storpolicy, ywb_false);
}

ywb_status_t PolicyManager::GetStoragePolicy(
        SubPoolKey& subpoolkey, StoragePolicy** policy)
{
    ywb_status_t ret = YFS_ENOENT;
    std::map<SubPoolKey, StoragePolicy*, SubPoolKeyCmp>::iterator iter;

    *policy = NULL;
    iter = mPolicies.find(subpoolkey);
    if(mPolicies.end() != iter)
    {
        *policy = iter->second;
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t PolicyManager::RemoveStoragePolicy(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YFS_ENOENT;
    std::map<SubPoolKey, StoragePolicy*, SubPoolKeyCmp>::iterator iter;

    iter = mPolicies.find(subpoolkey);
    if(mPolicies.end() != iter)
    {
        mPolicies.erase(iter);
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t PolicyManager::AddDiskSelector(
        SubPoolKey& subpoolkey, DiskSelector* selector)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::pair<std::map<SubPoolKey, DiskSelector*, SubPoolKeyCmp>::iterator, bool> pairret;

    pairret = mDiskSelectors.insert(std::make_pair(subpoolkey, selector));
    if(true != pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t PolicyManager::GetDiskSelector(
        SubPoolKey& subpoolkey, DiskSelector** selector)
{
    ywb_status_t ret = YFS_ENOENT;
    std::map<SubPoolKey, DiskSelector*, SubPoolKeyCmp>::iterator iter;

    iter = mDiskSelectors.find(subpoolkey);
    if(mDiskSelectors.end() != iter)
    {
        *selector = iter->second;
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t PolicyManager::RemoveDiskSelector(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YFS_ENOENT;
    std::map<SubPoolKey, DiskSelector*, SubPoolKeyCmp>::iterator iter;

    iter = mDiskSelectors.find(subpoolkey);
    if(mDiskSelectors.end() != iter)
    {
        mDiskSelectors.erase(iter);
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t PolicyManager::UpdateDiskSelector(SubPoolKey& subpoolkey,
        ywb_int32_t tier, ywb_int32_t signal)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskSelector* diskselector = NULL;

    ret = GetDiskSelector(subpoolkey, &diskselector);
    if(YFS_ENOENT == ret)
    {
        diskselector = new DiskSelector(subpoolkey, this);
        YWB_ASSERT(diskselector != NULL);
        AddDiskSelector(subpoolkey, diskselector);
    }

    return diskselector->Update(tier, signal);
}

ywb_status_t PolicyManager::UpdateDiskSelector(SubPoolKey& subpoolkey,
        DiskKey& diskkey, ywb_int32_t signal)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskSelector* diskselector = NULL;

    ret = GetDiskSelector(subpoolkey, &diskselector);
    if(YFS_ENOENT == ret)
    {
        diskselector = new DiskSelector(subpoolkey, this);
        YWB_ASSERT(diskselector != NULL);
        AddDiskSelector(subpoolkey, diskselector);
    }

    return diskselector->Update(diskkey, signal);
}

ywb_status_t PolicyManager::Adjust(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    StoragePolicy* storpolicy = NULL;
    ywb_int32_t expectedplacement = InitialPlacementType::IP_performance_preferred;
    ywb_int32_t realplacement = InitialPlacementType::IP_performance_preferred;

    ret = GetStoragePolicy(subpoolkey, &storpolicy);
    if(YFS_ENOENT == ret)
    {
        /*use default storage policy if not specified explicitly*/
        SetDefaultStoragePolicy(subpoolkey);
        GetStoragePolicy(subpoolkey, &storpolicy);
    }

    expectedplacement = storpolicy->GetExpectedPolicy(StoragePolicy::SP_initial_placement);
    ret = mPlacementDrvs[expectedplacement]->Adjust(subpoolkey, realplacement);
    storpolicy->SetRealPolicy(StoragePolicy::SP_initial_placement, realplacement);

    return ret;
}

/*
 * FIXME: we assume that DiskSelector has been updated by UpdateDiskSelector
 * before this method is invoked, so here we will not check the existence of
 * DiskSelector
 * */
ywb_status_t PolicyManager::ChooseAppropriateDisk(
        SubPoolKey& subpoolkey, DiskKey& disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    StoragePolicy* storpolicy = NULL;
    ywb_int32_t realplacement = InitialPlacementType::IP_performance_preferred;

    ret = GetStoragePolicy(subpoolkey, &storpolicy);
    if(YFS_ENOENT == ret)
    {
        /*use default storage policy if not specified explicitly*/
        SetDefaultStoragePolicy(subpoolkey);
        GetStoragePolicy(subpoolkey, &storpolicy);
    }

    realplacement = storpolicy->GetRealPolicy(StoragePolicy::SP_initial_placement);
    ret = mPlacementDrvs[realplacement]->ChooseAppropriateDisk(subpoolkey, disk);

    return ret;
}

ywb_int32_t PolicyManager::GetSwapDirection(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    StoragePolicy* sp = NULL;
    ywb_int32_t realplacement = InitialPlacementType::IP_performance_preferred;
    ywb_int32_t direction = SwapOutManager::SD_max;

    ret = GetStoragePolicy(subpoolkey, &sp);
    if(YWB_SUCCESS == ret)
    {
        realplacement = sp->GetRealPolicy(StoragePolicy::SP_initial_placement);
        direction = mPlacementDrvs[realplacement]->GetSwapDirection();
        return direction;
    }

    return SwapOutManager::SD_max;
}

ywb_int32_t PolicyManager::GetRelocationType(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    StoragePolicy* sp = NULL;
    ywb_int32_t realplacement = InitialPlacementType::IP_performance_preferred;

    ret = GetStoragePolicy(subpoolkey, &sp);
    if(YWB_SUCCESS == ret)
    {
        realplacement = sp->GetRealPolicy(StoragePolicy::SP_initial_placement);
        return mPlacementDrvs[realplacement]->GetRelocationType();
    }

    return RelocationType::RT_auto_tiering;
}


