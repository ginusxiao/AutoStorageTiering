/*
 * ASTPlacementSelector.hpp
 *
 *  Created on: 2016年6月11日
 *      Author: Administrator
 */

#ifndef SRC_OSS_AST_ASTPLACEMENT_HPP_
#define SRC_OSS_AST_ASTPLACEMENT_HPP_

#include <string>
#include "common/FsAssert.h"
#include "FsCommon/YfsMutex.hpp"
#include "AST/ASTConstant.hpp"
#include "AST/ASTUtil.hpp"
#include "AST/ASTLogicalConfig.hpp"

using namespace std;

/*
 * Initial placement must be supported by HUB instead of AST currently,
 * for AST is not necessarily enabled as the start of storage system,
 * and even so, AST will not have any data about storage system until
 * collecting configuration and OBJ IO in the very first cycle
 * */
class InitialPlacementType
{
public:
    enum {
        /*
         * set OBJ's initial placement to the highest available tier, if the
         * highest tier reach its capacity/performance limit, then next highest
         * tier will be used, meanwhile cold OBJs in higher tier will be swapped
         * out to lower tier if there is available space
         * */
        IP_performance_preferred = 0,
        /*use current "MDS-centralized" disk selection algorithm*/
        IP_auto,
        /*
         * set OBJ's initial placement to the highest available tier, if the
         * highest tier reach its capacity/performance limit, then next highest
         * tier will be used
         * */
        IP_performance_first_and_then_auto,
        /*
         * set OBJ's initial placement to the lowest available tier, if the
         * lowest tier reach its capacity/performance limit, then next lowest
         * tier will be used, meanwhile hot OBJs in lower tier will be swapped
         * out to higher tier if there is available space
         * */
        IP_cost_preferred,
        /*this policy is used for monitoring IO statistics only*/
        IP_no_migration,
        /*
         * make sure IP_placement_cnt <= Constant::INITIAL_PLACEMENT_POLICIES_NUM,
         * otherwise update Constant::INITIAL_PLACEMENT_POLICIES_NUM is necessary
         * */
        IP_placement_cnt,
    };
};

class TierWiseDiskCandidates;
class DiskSelector;
class DiskCandidate
{
public:
    enum {
        DCF_removing = 0,
        DCF_removed,
        DCF_medium_err,
        DCF_busy,
        DCF_suspecious,
        /*little swap will be OK*/
        DCF_swap_low,
        /*lots of swap out will be OK*/
        DCF_swap_medium,
        /*large amount of swap out will be OK*/
        DCF_swap_high,

        DCF_cnt,
    };

    DiskCandidate(DiskKey& diskkey) : mDiskKey(diskkey),
            mTotalCap(YWB_UINT32_MAX), mReservedCapForHotSwap(0),
            mReservedCapForColdSwap(0), mAvailCap(YWB_UINT32_MAX),
            mRecentlyRemovedOBJNum(0), mIOs(YWB_UINT32_MAX),
            mBWs(YWB_UINT32_MAX), mAvailTickets(0)
    {
        mFlag.Reset();
    }

    DiskCandidate(DiskKey& diskkey, ywb_uint32_t totalcap,
            ywb_uint32_t availcap, ywb_uint32_t IOs, ywb_uint32_t BWs) :
                mDiskKey(diskkey), mTotalCap(totalcap),
                mReservedCapForHotSwap(0), mReservedCapForColdSwap(0),
                mAvailCap(availcap), mRecentlyRemovedOBJNum(0),
                mIOs(IOs), mBWs(BWs), mAvailTickets(0)
    {
        mFlag.Reset();
    }

    virtual ~DiskCandidate()
    {
        Reset();
    }

    inline DiskKey GetDiskKey()
    {
        return mDiskKey;
    }

    inline ywb_uint32_t GetTotalCap()
    {
        return mTotalCap;
    }

    inline ywb_uint32_t GetReservedCapForHotSwap()
    {
        return mReservedCapForHotSwap;
    }

    inline ywb_uint32_t GetReservedCapForColdSwap()
    {
        return mReservedCapForColdSwap;
    }

    inline ywb_uint32_t GetAvailCap()
    {
        return mAvailCap;
    }

    inline ywb_uint32_t GetRecentlyRemovedOBJNum()
    {
        return mRecentlyRemovedOBJNum;
    }

    inline ywb_uint32_t GetIOs()
    {
        return mIOs;
    }

    inline ywb_uint32_t GetBWs()
    {
        return mBWs;
    }

    inline ywb_uint32_t GetAvailTickets()
    {
        return mAvailTickets;
    }

    inline void SetTotalCap(ywb_uint32_t val)
    {
        mTotalCap = val;
    }

    inline void IncreaseReservedCapForHotSwap()
    {
        mReservedCapForHotSwap++;
    }

    inline void DecreaseReservedCapForHotSwap()
    {
        mReservedCapForHotSwap = (mReservedCapForHotSwap > 1) ?
                (mReservedCapForHotSwap - 1) : 0;
    }

    inline void IncreaseReservedCapForColdSwap()
    {
        mReservedCapForColdSwap++;
    }

    inline void DecreaseReservedCapForColdSwap()
    {
        mReservedCapForColdSwap = (mReservedCapForColdSwap > 1) ?
                (mReservedCapForColdSwap - 1) : 0;
    }

    inline void SetAvailCap(ywb_uint32_t val)
    {
        mAvailCap = val;
    }

    inline void SetRecentlyRemovedOBJNum(ywb_uint32_t val)
    {
        mRecentlyRemovedOBJNum = val;
    }

    inline void SetIOs(ywb_uint32_t val)
    {
        mIOs = val;
    }

    inline void SetBWs(ywb_uint32_t val)
    {
        mBWs = val;
    }

    inline void SetAvailTickets(ywb_uint32_t val)
    {
        mAvailTickets = val;
    }

    inline void DecreaseAvailTickets(ywb_uint32_t val)
    {
        mAvailTickets = (mAvailTickets > val) ? (mAvailTickets - val) : 0;
    }

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

    void Reset();

private:
    DiskKey mDiskKey;
    /*disk total capacity*/
    ywb_uint32_t mTotalCap;
    /*currently reserved capacity for hot swap, in measure of OBJ num*/
    ywb_uint32_t mReservedCapForHotSwap;
    /*currently reserved capacity for cold swap, in measure of OBJ num*/
    ywb_uint32_t mReservedCapForColdSwap;
    /*disk available capacity*/
    ywb_uint32_t mAvailCap;
    /*record currently remove OBJ num*/
    ywb_uint32_t mRecentlyRemovedOBJNum;
    /*IOs stated*/
    ywb_uint32_t mIOs;
    /*BWs stated*/
    ywb_uint32_t mBWs;
    /*currently available tickets*/
    ywb_uint32_t mAvailTickets;
    Flag mFlag;
};

class TierWiseDiskCandidates
{
public:
    enum {
        /*tier has no disk at all*/
        TC_no_candidate = 0,
        /*tier has disk but has no available disk candidate*/
        TC_no_avail_candidate,
        TC_exhausted,
        TC_cnt,
    };

    TierWiseDiskCandidates(DiskSelector* ds) : mAvailTicketsNum(0),
            mCurIndex(0), mRecentlyRemovedOBJNum(0), mDiskSelector(ds)
    {
        mFlag.Reset();
        YWB_ASSERT(ds != NULL);
    }

    virtual ~TierWiseDiskCandidates()
    {
        Reset();
    }

    inline ywb_uint32_t GetAvailTicketsNum()
    {
        return mAvailTicketsNum;
    }

    inline ywb_uint32_t GetRecentlyRemovedOBJNum()
    {
        return mRecentlyRemovedOBJNum;
    }

    inline DiskSelector* GetDiskSelector()
    {
        return mDiskSelector;
    }

    inline void SetAvailTicketsNum(ywb_uint32_t num)
    {
        mAvailTicketsNum = num;
    }

    inline void DecreaseAvailTicketNum()
    {
        mAvailTicketsNum = ((mAvailTicketsNum > 1) ? (mAvailTicketsNum - 1) : 0);
    }

    inline void SetRecentlyRemovedOBJNum(ywb_uint32_t val)
    {
        mRecentlyRemovedOBJNum = val;
    }

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

    ywb_status_t AddDiskCandidate(DiskKey& diskkey);
    ywb_status_t GetDiskCandidate(DiskKey& diskkey, DiskCandidate** candidate);
    ywb_status_t RemoveDiskCandidate(DiskKey& diskkey);
    /*for test only*/
    ywb_status_t AddDiskCandidateVec(DiskCandidate*);

    void MarkCandidatesAsSuspect();
    void RemoveUnConfirmedSuspect();
    ywb_bool_t ShouldExcludeDiskCandidate(DiskKey& diskkey);
    ywb_bool_t ShouldExcludeDiskCandidate(DiskCandidate* candidate);

    ywb_bool_t HasAnyDiskCandidate();
    ywb_bool_t HasDiskCandidate(DiskKey& diskkey);
    ywb_bool_t FrequentlyRemovedRecently();
    void UpdateRemovedOBJNum(DiskKey& diskkey, ywb_int32_t val);
    /*
     * get next candidate by given selection mode
     *
     * theoretically, we should exclude those removing/removed/medium_err/busy
     * disk from candidate set, but for simplicity, here will not figure out
     * these disk candidate which will cost a lot for GetNextDiskCandidate() is
     * frequently accessed, instead, when there are removing/removed/medium_err/
     * busy disk, then a new signal will be launched and Update() is invoked
     * and new reservation will be generated
     * */
    ywb_status_t GetNextDiskCandidate(ywb_int32_t mode, DiskKey& diskkey);
    /*
     * (1) update tier wise disk candidates(when there is no enough available
     * tickets or the configuration changes)
     *
     * (2) reserve tickets for future create IO, return the reserved tickets
     * number, it will figure out those disks called for swap, but whether or
     * not swap will be executed is dependent on external condition(such as the
     * initial placement policy)
     * */
    ywb_uint32_t Update(ywb_int32_t base);

    void Reset();

protected:
    ywb_bool_t ApproximateEqual(ywb_uint32_t vala,
            ywb_uint32_t valb, ywb_uint32_t ratio);
    ywb_status_t GetNextCandidateDiskByDiskRR(DiskKey& diskkey);
    ywb_status_t GetNextCandidateDiskByDiskRandom(DiskKey& diskkey);
    ywb_status_t GetNextCandidateTicketByTicketRR(DiskKey& diskkey);
    ywb_status_t GetNextCandidateTicketByTicketRandom(DiskKey& diskkey);
    /*get swap direction of given disk*/
    ywb_int32_t GetSwapDirection(DiskKey& diskkey);

private:
    std::map<DiskKey, DiskCandidate*, DiskKeyCmp> mDisksMap;
    /*should be rebuild when calling Update()*/
    std::vector<DiskCandidate*> mDisksVec;
    /*currently available tickets num, in measure of OBJ num*/
    ywb_uint32_t mAvailTicketsNum;
    /*record current index of the chosen disk candidates*/
    ywb_uint32_t mCurIndex;
    /*record number of OBJs removed recently*/
    ywb_uint32_t mRecentlyRemovedOBJNum;
    Flag mFlag;
    /*back reference to DiskSelector*/
    DiskSelector* mDiskSelector;
};

class PolicyManager;
class DiskSelector
{
public:
    /*selection base*/
    enum Base{
        /*select on disk capacity proportionally*/
        DSB_capacity_proportional,
        /*select on disk IOs proportionally*/
        DSB_io_proportional,
        /*select on disk BWs proportionally*/
        DSB_bw_proportional,
        /*IOs first, then BWs and last capacity*/
        DSB_performance_based_v1,
        DSB_cnt,
    };

    /*selection mode*/
    enum Mode{
        /*choose the same disk until its tickets are used up, then next in order*/
        DSM_disk_by_disk_round_robin = 0,
        /*choose the same disk until its tickets are used up, then next randomly*/
        DSM_disk_by_disk_random,
        /*
         * choose a ticket from one disk and then another ticket from another disk,
         * and select disk in round robin mode
         * */
        DSM_ticket_by_ticket_round_robin,
        /*
         * choose a ticket from one disk and then another ticket from another disk,
         * and select disk randomly
         * */
        DSM_ticket_by_ticket_random,
        DSM_cnt,
    };

    enum Signal{
        DSE_add_disk = 0,
        DSE_remove_disk,
        DSE_disk_medium_err,
        DSE_disk_busy,
        DSE_add_obj,
        DSE_remove_obj,
        DSE_lack_of_ticket,
        DSE_cnt,
    };

    /*sort(in descending order) methods corresponding to different selection base*/
    class Sort
    {
    public:
        static bool SortByCapacityProportional(const DiskCandidate* disk1,
                const DiskCandidate* disk2);
        static bool SortByIOProportional(const DiskCandidate* disk1,
                const DiskCandidate* disk2);
        static bool SortByBWProportional(const DiskCandidate* disk1,
                const DiskCandidate* disk2);
        static bool SortByPerformanceV1(const DiskCandidate* disk1,
                const DiskCandidate* disk2);
    };

    DiskSelector(SubPoolKey& subpoolkey, ywb_int32_t base,
            ywb_int32_t mode, PolicyManager* pm) :
                mSubPoolKey(subpoolkey), mBase(base),
                mMode(mode), mPolicyMgr(pm)
    {
        ywb_int32_t tiertype = Tier::TIER_0;
        TierWiseDiskCandidates* twdc = NULL;
        Mutex* lock = NULL;

        YWB_ASSERT(pm != NULL);
        for(; tiertype < (ywb_int32_t)Constant::TIER_NUM; tiertype++)
        {
            twdc = new TierWiseDiskCandidates(this);
            YWB_ASSERT(twdc != NULL);
            mCandidates.push_back(twdc);
            lock = new Mutex("TierWiseCandidatesLock");
            YWB_ASSERT(lock != NULL);
            mLocks.push_back(lock);
        }
    }

    /*default Base and Mode will be adopted*/
    DiskSelector(SubPoolKey& subpoolkey, PolicyManager* pm) :
        mSubPoolKey(subpoolkey), mBase(DSB_capacity_proportional),
        mMode(DSM_ticket_by_ticket_round_robin), mPolicyMgr(pm)
    {
        ywb_int32_t tiertype = Tier::TIER_0;
        TierWiseDiskCandidates* twdc = NULL;
        Mutex* lock = NULL;

        YWB_ASSERT(pm != NULL);
        mCandidates.reserve(Constant::TIER_NUM);
        mLocks.reserve(Constant::TIER_NUM);
        for(; tiertype < (ywb_int32_t)Constant::TIER_NUM; tiertype++)
        {
            twdc = new TierWiseDiskCandidates(this);
            YWB_ASSERT(twdc != NULL);
            mCandidates.push_back(twdc);
            lock = new Mutex("TierWiseCandidatesLock");
            YWB_ASSERT(lock != NULL);
            mLocks.push_back(lock);
        }
    }

    virtual ~DiskSelector()
    {
        ywb_int32_t tiertype = Tier::TIER_0;

        for(; tiertype < (ywb_int32_t)Constant::TIER_NUM; tiertype++)
        {
            if(mCandidates[tiertype] != NULL)
            {
                delete mCandidates[tiertype];
                mCandidates[tiertype] = NULL;
            }

            if(mLocks[tiertype] != NULL)
            {
                delete mLocks[tiertype];
                mLocks[tiertype] = NULL;
            }
        }

        mPolicyMgr = NULL;
    }

    inline SubPoolKey GetSubPoolKey()
    {
        return mSubPoolKey;
    }

    inline PolicyManager* GetPolicyManager()
    {
        return mPolicyMgr;
    }

    /*get next candidate from specified tier*/
    ywb_status_t GetNextDiskCandidate(ywb_int32_t tier, DiskKey& diskkey);
    /*
     * update disk candidates set whenever necessary(when reserved tickets
     * are to be used up, or when configuration change)
     * */
    ywb_status_t Update(ywb_int32_t tier, ywb_int32_t signal);
    ywb_status_t UpdateLocked(ywb_int32_t tier, ywb_int32_t signal);
    /*update the tier associated with given @diskkey*/
    ywb_status_t Update(DiskKey& diskkey, ywb_int32_t signal);
    /*get swap direction of given tier*/
    ywb_int32_t GetSwapDirection(TierWiseDiskCandidates* tdc);

protected:
    /*@val: the number of newly removed OBJ*/
    void UpdateRemovedOBJNumLocked(ywb_int32_t tier, ywb_int32_t val);
    void UpdateRemovedOBJNum(ywb_int32_t tier, ywb_int32_t val);
    void UpdateRemovedOBJNumLocked(ywb_int32_t tier,
            DiskKey& diskkey, ywb_int32_t val);
    void UpdateRemovedOBJNum(DiskKey& diskkey, ywb_int32_t val);

private:
    /*corresponding subpool*/
    SubPoolKey mSubPoolKey;
    /*selection base, comes from DiskSelector::Base*/
    ywb_int32_t mBase;
    /*selection mode, comes from DiskSelector::Mode*/
    ywb_int32_t mMode;
    /*for concurrent access protection, one for each tier*/
    std::vector<Mutex*> mLocks;
    /*tier wise disk candidates for each tier*/
    std::vector<TierWiseDiskCandidates*> mCandidates;
    /*back reference to policy manager*/
    PolicyManager* mPolicyMgr;
};

/*base driver class for initial placement*/
class InitialPlacementDriver
{
public:
    InitialPlacementDriver(PolicyManager* pm);

    virtual ~InitialPlacementDriver();

    inline PolicyManager* GetPolicyManager()
    {
        return mPolicyMgr;
    }

    /*
     * adjust the internally used policy according to the user-defined policy and
     * the external configuration change, called before applying it for the very
     * first time and whenever configuration changes
     * @subpoolkey: the entity against which this policy is
     * @applied: currently applied policy, INOUT
     * */
    virtual ywb_status_t Adjust(SubPoolKey& subpoolkey, ywb_int32_t& applied) = 0;
    /*
     * choose appropriate disk from given subpool
     * @subpool: the entity against which this policy is
     * @disk: the chosen appropriate disk
     * */
    virtual ywb_status_t ChooseAppropriateDisk(SubPoolKey& subpool, DiskKey& disk) = 0;
    virtual ywb_int32_t GetSwapDirection();
    virtual ywb_int32_t GetRelocationType() = 0;

private:
    /*back reference to policy manager*/
    PolicyManager* mPolicyMgr;
};

class PerformancePreferredDriver : public InitialPlacementDriver
{
public:
    PerformancePreferredDriver(PolicyManager* pm) : InitialPlacementDriver(pm),
    mReservedCapForHotSwap(Constant::RESERVED_CAP_FOR_HOT_SWAP)
    {

    }

    virtual ~PerformancePreferredDriver()
    {

    }

    ywb_status_t Adjust(SubPoolKey& subpoolkey, ywb_int32_t& applied);
    ywb_status_t ChooseAppropriateDisk(SubPoolKey& subpool, DiskKey& disk);
    ywb_int32_t GetSwapDirection();
    ywb_int32_t GetRelocationType();

private:
    /*driver specified fields*/
    /*
     * "Performance Preferred" policy may swap in some hot OBJ from lower tier,
     * so reserve some space for it, in measure of percentage
     * */
    ywb_uint32_t mReservedCapForHotSwap;
};

class AutoDriver : public InitialPlacementDriver
{
public:
    AutoDriver(PolicyManager* pm) : InitialPlacementDriver(pm)
    {

    }

    virtual ~AutoDriver()
    {

    }

    ywb_status_t Adjust(SubPoolKey& subpoolkey, ywb_int32_t& applied);
    ywb_status_t ChooseAppropriateDisk(SubPoolKey& subpool, DiskKey& disk);
    ywb_int32_t GetRelocationType();

private:
    /*driver specified fields*/
};

class PerformanceFirstAndThenAutoDriver : public InitialPlacementDriver
{
public:
    PerformanceFirstAndThenAutoDriver(PolicyManager* pm) :
        InitialPlacementDriver(pm),
        mReservedCapForHotSwap(Constant::RESERVED_CAP_FOR_HOT_SWAP),
        mReservedCapForColdSwap(Constant::RESERVED_CAP_FOR_COLD_SWAP)
    {

    }

    virtual ~PerformanceFirstAndThenAutoDriver()
    {

    }

    ywb_status_t Adjust(SubPoolKey& subpoolkey, ywb_int32_t& applied);
    ywb_status_t ChooseAppropriateDisk(SubPoolKey& subpool, DiskKey& disk);
    ywb_int32_t GetRelocationType();

private:
    /*driver specified fields*/
    /*
     * "Performance First And Then Auto" policy may swap in some hot OBJs from
     * lower tier, so reserve some space for it, in measure of percentage
     * */
    ywb_uint32_t mReservedCapForHotSwap;
    /*
     * "Performance First And Then Auto" policy may swap in some cold OBJs from
     * higher tier, so reserve some space for it, in measure of percentage
     * */
    ywb_uint32_t mReservedCapForColdSwap;
};

class CostPreferredDriver : public InitialPlacementDriver
{
public:
    CostPreferredDriver(PolicyManager* pm) : InitialPlacementDriver(pm),
    mReservedCapForColdSwap(Constant::RESERVED_CAP_FOR_COLD_SWAP)
    {

    }

    virtual ~CostPreferredDriver()
    {

    }


    ywb_status_t Adjust(SubPoolKey& subpoolkey, ywb_int32_t& applied);
    ywb_status_t ChooseAppropriateDisk(SubPoolKey& subpool, DiskKey& disk);
    ywb_int32_t GetSwapDirection();
    ywb_int32_t GetRelocationType();

private:
    /*driver specified fields*/
    /*
     * "Cost Preferred" policy may swap in some cold OBJ from higher tier,
     * so reserve some space for it, in measure of percentage
     * */
    ywb_uint32_t mReservedCapForColdSwap;
};

class NoDataMovementDriver : public InitialPlacementDriver
{
public:
    NoDataMovementDriver(PolicyManager* pm) : InitialPlacementDriver(pm)
    {

    }

    virtual ~NoDataMovementDriver()
    {

    }

    ywb_status_t Adjust(SubPoolKey& subpoolkey, ywb_int32_t& applied);
    ywb_status_t ChooseAppropriateDisk(SubPoolKey& subpool, DiskKey& disk);
    ywb_int32_t GetRelocationType();

private:
    /*driver specified fields*/
};

#endif /* SRC_OSS_AST_ASTPLACEMENT_HPP_ */
