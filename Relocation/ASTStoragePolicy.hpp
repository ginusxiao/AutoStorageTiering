/*
 * ASTStoragePolicy.hpp
 *
 *  Created on: 2016年6月6日
 *      Author: Administrator
 */

#ifndef SRC_OSS_AST_ASTSTORAGEPOLICY_HPP_
#define SRC_OSS_AST_ASTSTORAGEPOLICY_HPP_

#include <string>
#include "common/FsAssert.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTUtil.hpp"
#include "AST/ASTPlacement.hpp"
#include "AST/ASTHUB.hpp"

using namespace std;

class StoragePolicy
{
public:
    enum PolicyType{
        /*initial placement*/
        SP_initial_placement = 0,

        /*
         * any other storage policies can be added here, BUT make sure
         * SP_cnt less than or equal to Constant::POLICY_KINDS(if not,
         * update Constant::POLICY_KINDS is necessary)
         * */
        SP_cnt,
    };

    StoragePolicy()
    {
        /*default storage policy will be adopted*/
        mExpectedPolicies[SP_initial_placement] =
                InitialPlacementType::IP_auto;
        mRealPolicies[SP_initial_placement] =
                InitialPlacementType::IP_auto;
    }

    StoragePolicy(const StoragePolicy& policy)
    {
        ywb_int32_t type = SP_initial_placement;

        for(; type < SP_cnt; type++)
        {
            mExpectedPolicies[type] = policy.mExpectedPolicies[type];
            mRealPolicies[type] = policy.mRealPolicies[type];
        }
    }

    StoragePolicy& operator= (const StoragePolicy& policy)
    {
        ywb_int32_t type = SP_initial_placement;

        if(this == &policy)
            return *this;

        for(; type < SP_cnt; type++)
        {
            mExpectedPolicies[type] = policy.mExpectedPolicies[type];
            mRealPolicies[type] = policy.mRealPolicies[type];
        }

        return *this;
    }

    virtual ~StoragePolicy() {}

    inline ywb_int32_t GetExpectedPolicy(ywb_int32_t type)
    {
        YWB_ASSERT((StoragePolicy::SP_initial_placement <= type) &&
                (type < StoragePolicy::SP_cnt));
        return mExpectedPolicies[type];
    }

    inline ywb_int32_t GetRealPolicy(ywb_int32_t type)
    {
        YWB_ASSERT((StoragePolicy::SP_initial_placement <= type) &&
                (type < StoragePolicy::SP_cnt));
        return mRealPolicies[type];
    }

    inline void SetExpectedPolicy(ywb_int32_t type, ywb_int32_t policy)
    {
        YWB_ASSERT((StoragePolicy::SP_initial_placement <= type) &&
                (type < StoragePolicy::SP_cnt));
        mExpectedPolicies[type] = policy;
    }

    inline void SetRealPolicy(ywb_int32_t type, ywb_int32_t policy)
    {
        YWB_ASSERT((StoragePolicy::SP_initial_placement <= type) &&
                (type < StoragePolicy::SP_cnt));
        mRealPolicies[type] = policy;
    }

private:
    /*expected policy set by user, comes from InitialPlacementType*/
    ywb_int32_t mExpectedPolicies[Constant::POLICY_KINDS];
    /*currently real policy applied internally*/
    ywb_int32_t mRealPolicies[Constant::POLICY_KINDS];
};

class PolicyManager
{
public:
    PolicyManager(HUB* hub) : mHUB(hub)
    {
        YWB_ASSERT(hub != NULL);

        mPlacementDrvs[InitialPlacementType::IP_performance_preferred] =
                new PerformancePreferredDriver(this);
        YWB_ASSERT(mPlacementDrvs[InitialPlacementType::IP_performance_preferred] != NULL);
        mPlacementDrvs[InitialPlacementType::IP_auto] =
                new PerformancePreferredDriver(this);
        YWB_ASSERT(mPlacementDrvs[InitialPlacementType::IP_auto] != NULL);
        mPlacementDrvs[InitialPlacementType::IP_performance_first_and_then_auto] =
                new PerformancePreferredDriver(this);
        YWB_ASSERT(mPlacementDrvs[InitialPlacementType::IP_performance_first_and_then_auto] != NULL);
        mPlacementDrvs[InitialPlacementType::IP_cost_preferred] =
                new PerformancePreferredDriver(this);
        YWB_ASSERT(mPlacementDrvs[InitialPlacementType::IP_cost_preferred] != NULL);
        mPlacementDrvs[InitialPlacementType::IP_no_migration] =
                new PerformancePreferredDriver(this);
        YWB_ASSERT(mPlacementDrvs[InitialPlacementType::IP_no_migration] != NULL);
    }

    virtual ~PolicyManager()
    {
        std::map<SubPoolKey, StoragePolicy*, SubPoolKeyCmp>::iterator iter;
        std::map<SubPoolKey, DiskSelector*, SubPoolKeyCmp>::iterator iter2;
        StoragePolicy* policy = NULL;
        DiskSelector* selector = NULL;
        ywb_int32_t type = 0;

        iter = mPolicies.begin();
        while(mPolicies.end() != iter)
        {
            policy = iter->second;
            iter++;
            delete policy;
            policy = NULL;
        }

        iter2 = mDiskSelectors.begin();
        while(mDiskSelectors.end() != iter2)
        {
            selector = iter2->second;
            iter2++;
            delete selector;
            selector = NULL;
        }

        for(type = InitialPlacementType::IP_performance_preferred;
                type < InitialPlacementType::IP_placement_cnt; type++)
        {
            if(mPlacementDrvs[type] != NULL)
            {
                delete mPlacementDrvs[type];
                mPlacementDrvs[type] = NULL;
            }
        }

        mPolicies.clear();
        mDiskSelectors.clear();
        mHUB = NULL;
    }

    inline HUB* GetHUB()
    {
        return mHUB;
    }

    /*
     * set specified @policy against given @subpoolkey
     * @overlap: whether or not overlap existed policy if already specified before
     * */
    ywb_status_t SetStoragePolicy(SubPoolKey& subpoolkey,
            StoragePolicy& policy, ywb_bool_t overlap);
    ywb_status_t SetDefaultStoragePolicy(SubPoolKey&);
    ywb_status_t GetStoragePolicy(SubPoolKey&, StoragePolicy**);
    ywb_status_t RemoveStoragePolicy(SubPoolKey&);

    ywb_status_t AddDiskSelector(SubPoolKey&, DiskSelector*);
    ywb_status_t GetDiskSelector(SubPoolKey&, DiskSelector**);
    ywb_status_t RemoveDiskSelector(SubPoolKey&);

    ywb_status_t UpdateDiskSelector(SubPoolKey& subpoolkey,
            ywb_int32_t tier, ywb_int32_t signal);
    ywb_status_t UpdateDiskSelector(SubPoolKey& subpoolkey,
            DiskKey& diskkey, ywb_int32_t signal);
    /*
     * adjust the internally used policy according to the user-defined policy and
     * the external configuration change, called before applying it for the very
     * first time and whenever configuration changes
     * @subpoolkey: the entity against which this policy is
     * */
    ywb_status_t Adjust(SubPoolKey& subpoolkey);
    /*
     * choose appropriate disk from given subpool for initial placement
     * @subpool: the entity against which this policy is
     * @disk: the chosen appropriate disk
     * */
    ywb_status_t ChooseAppropriateDisk(SubPoolKey& subpool, DiskKey& disk);
    ywb_int32_t GetSwapDirection(SubPoolKey& subpoolkey);
    ywb_int32_t GetRelocationType(SubPoolKey& subpoolkey);

private:
    std::map<SubPoolKey, StoragePolicy*, SubPoolKeyCmp> mPolicies;
    std::map<SubPoolKey, DiskSelector*, SubPoolKeyCmp> mDiskSelectors;
    InitialPlacementDriver* mPlacementDrvs[Constant::INITIAL_PLACEMENT_POLICIES_NUM];
    /*back reference to HUB*/
    HUB* mHUB;
};

#endif /* SRC_OSS_AST_ASTSTORAGEPOLICY_HPP_ */
