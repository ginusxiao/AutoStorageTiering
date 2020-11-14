/*
 * ASTSwapOut.hpp
 *
 *  Created on: 2016年6月6日
 *      Author: Administrator
 */

#ifndef SRC_OSS_AST_ASTSWAPOUT_HPP_
#define SRC_OSS_AST_ASTSWAPOUT_HPP_

#include <map>
#include "FsCommon/YfsMutex.hpp"
#include "AST/MessageImpl.hpp"

using namespace std;

enum SwapLevel{
    SL_low = 0,
    SL_medium,
    SL_high,
    SL_max,
};

enum SwapDirection{
    SD_up = 0,
    SD_down,
    SD_max,
};

class SwapOutManager;
class SubPoolSwapOut
{
public:
    /*
     * move definition of constructor and de-constructor to cpp file
     * for avoiding compilation error(incomplete type ‘SwapOutManager’
     * used)
     * */
    SubPoolSwapOut();
    virtual ~SubPoolSwapOut();

    /*@val: combination of swap level and swap direction*/
    ywb_status_t AddSwapDisk(DiskKey& diskkey, ywb_int32_t val);
    ywb_status_t RemoveSwapDisk(DiskKey& diskkey);
    /*get the number of disks require swap*/
    ywb_uint32_t GetSwapRequiredDiskNum(ywb_int32_t swapdirection);
    /*get the number of disks require swap of given condition*/
    ywb_uint32_t GetSwapRequiredDiskNum(ywb_int32_t swapdirection, ywb_int32_t swaplevel);

    /*auxilary routine for ScheduleNextSwapDisk*/
    ywb_status_t ScheduleNextRound(ywb_int32_t swapdirection, DiskKey& diskkey);
    /*
     * schedule the next to be swapped out disk of given swap direction.
     * lock is required externally
     * */
    ywb_status_t ScheduleNextSwapDisk(ywb_int32_t swapdirection, DiskKey& diskkey);

    ywb_bool_t Empty();

private:
    set<DiskKey, DiskKeyCmp> mHighSwapDisksUp;
    set<DiskKey, DiskKeyCmp> mMediumSwapDisksUp;
    set<DiskKey, DiskKeyCmp> mLowSwapDisksUp;

    set<DiskKey, DiskKeyCmp> mHighSwapDisksDown;
    set<DiskKey, DiskKeyCmp> mMediumSwapDisksDown;
    set<DiskKey, DiskKeyCmp> mLowSwapDisksDown;

    /*iterator used for scheduling swap disk*/
    set<DiskKey, DiskKeyCmp>::iterator mHighUpIt;
    set<DiskKey, DiskKeyCmp>::iterator mMediumUpIt;
    set<DiskKey, DiskKeyCmp>::iterator mLowUpIt;
    set<DiskKey, DiskKeyCmp>::iterator mHighDownIt;
    set<DiskKey, DiskKeyCmp>::iterator mMediumDownIt;
    set<DiskKey, DiskKeyCmp>::iterator mLowDownIt;

    /*schedule ratio of high/medium/low swap*/
    ywb_int32_t mHighRatio;
    ywb_int32_t mMediumRatio;
    ywb_int32_t mLowRatio;
    /*
     * schedule round, assume @mHighRatio, @mMediumRatio and @mLowRatio
     * equals to 6, 3 and 2, then it works like below:
     * round        scheduled level       comment
     * round 1:     high            <--- full trip start
     * round 2:     medium
     * round 3:     low
     * round 4:     high
     * round 5:     medium
     * round 6:     low
     * round 7:     high
     * round 8:     medium
     * round 9:     high
     * round 10:    high
     * round 11:    high            <--- full trip end
     *
     * round 12: high               <--- new full trip start, same as round 1
     * ......
     *
     * round 23: high               <--- new full trip start, same as round 1
     *
     * in each round, only one kind of swap(high, medium or low) will be
     * scheduled, and all disks of this kind will be scheduled
     *
     * a new added swap disk may not be scheduled until next new full trip
     * start(for example, current round is round 7 and the newly added swap
     * disk is of low swap requirement)
     *
     * CAUTION should be paid for @m***SwapDisks*** may change during scheduling:
     * (1) add one entry may cause removing of another entry having the same
     *     DiskKey but different swap level
     * (2) iterator should be updated when removing one entry and the removed
     *     entry is the one current iterator pointed to
     * */
    ywb_int32_t mScheduleRoundUp;
    ywb_int32_t mScheduleRoundDown;
    /*comes from SwapOutManager::SwapLevel*/
    ywb_int32_t mScheduledSwapLevelUp;
    ywb_int32_t mScheduledSwapLevelDown;
};

class SwapOutManager
{
public:
    enum SwapLevel{
        SL_low = 0,
        SL_medium,
        SL_high,
        SL_max,
    };

    enum SwapDirection{
        SD_up = 0,
        SD_down,
        SD_max,
    };

    SwapOutManager(AST* ast) : mAST(ast)
    {
        YWB_ASSERT(mAST != NULL);
    }

    virtual ~SwapOutManager()
    {
        Reset();
        mAST = NULL;
    }

    /*get the value combined of swap level and swap direction*/
    static ywb_int32_t GetCombinedLevelAndDirection(
            ywb_int32_t level, ywb_int32_t direction);
    /*get swap level from combined value*/
    static ywb_int32_t GetSwapLevel(ywb_int32_t val);
    /*get swap direction from combined value*/
    static ywb_int32_t GetSwapDirection(ywb_int32_t val);

    ywb_status_t AddSubPoolSwapOut(SubPoolKey& subpoolkey);
    ywb_status_t RemoveSubPoolSwapOut(SubPoolKey& subpoolkey);
    ywb_status_t AddSwapDisk(DiskKey& diskkey, ywb_int32_t val);
    ywb_status_t RemoveSwapDisk(DiskKey& diskkey);
    /*return true if any disks of specified subpool requires swap*/
    ywb_bool_t GetSwapRequired(SubPoolKey& subpoolkey);
    /*get the number of disks require swap*/
    ywb_status_t GetSwapRequiredDiskNum(SubPoolKey& subpoolkey,
            ywb_int32_t swapdirection, ywb_uint32_t& disknum);

    /*
     * return true if given subpool is in @mSwapSubPools regardless
     * of whether it is empty
     * */
    ywb_bool_t GetSwapRelocationDue(SubPoolKey& subpoolkey);
    /*return true if specified disk requires swap*/
//    ywb_bool_t GetSwapRequired(SubPoolKey& subpoolkey,
//            DiskKey& diskkey, ywb_int32_t& val);
//    /*return true if specified disk requires swap*/
//    ywb_bool_t GetSwapRequired(DiskKey& diskkey, ywb_int32_t& val);

    /*
     * choose appropriate disk as target of swap out
     * @subpool: from which the target disk is chosen
     * @from: the chosen appropriate target disk for swapping out
     * @to: the chosen appropriate target disk for swapping in
     * */
//    ywb_status_t ChooseSwapTargetDiskPair(SubPoolKey& subpool,
//            DiskKey& from, DiskKey& to);
    /*schedule next to be swapped disk*/
    ywb_status_t ScheduleNextSwapDisk(SubPoolKey& subpoolkey,
            ywb_int32_t swapdirection, DiskKey& diskkey);

    ywb_status_t ResolveRelocationType(NotifyRelocationType* relocationtype);
    ywb_status_t ResolveSwapRequirement(NotifySwapRequirement* requirement);
    ywb_status_t ResolveSwapCancellation(NotifySwapCancellation* cancellation);
    ywb_status_t Consolidate();
    ywb_status_t Reset();

private:
    /*
     * SHOULD be retained even when cross the decision window boundary,
     *
     * one subpool will be added into @mSwapSubPools if its relocation
     * type have swap flag set, even though not any of its belonging
     * disks requires swap currently
     *
     * one subpool will be removed from @mSwapSubPools if its relocation
     * type have not set swap out flag
     * */
    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp> mSwapSubPools;
    /*for protecting concurrent access*/
    RWLock mRWLock;
    /*back reference to AST*/
    AST* mAST;
};

#endif /* SRC_OSS_AST_ASTSWAPOUT_HPP_ */
