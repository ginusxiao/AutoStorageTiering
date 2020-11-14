#ifndef __AST_ARB_HPP__
#define __AST_ARB_HPP__

#include <list>
#include <map>
#include "common/FsInts.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTUtil.hpp"
#include "AST/ASTError.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "AST/ASTPlan.hpp"

typedef OBJKey ArbitrateeKey;
typedef OBJKeyCmp ArbitrateeKeyCmp;

class BaseArbitratee {
public:
    enum {
        /*for normal arbitratee, already submitted to migrator*/
        BAF_submitted = 0,
        /*for normal arbitratee, migration already launched*/
        BAF_migrating,
        /*for normal arbitratee, migration completed*/
        BAF_migrated,
        /*for normal arbitratee, migration expired*/
        BAF_expired,
        /*for arbitratee which fails to be migrated*/
        BAF_migration_failed,
        /*fake arbitratee, used as signal to stop DefaultMigrator*/
        BAF_fake_stop,
        /*fake arbitratee, used as identification of migration completion event*/
        BAF_fake_complete_mig,
        /*fake arbitratee, used as signal to check expired arbitratees*/
        BAF_fake_check_expired,
    };

    BaseArbitratee() : mCycle(0), mEpoch(0)
    {
        mFlag.Reset();
    }

    BaseArbitratee(OBJKey objkey, ywb_uint32_t cycle, ywb_uint64_t epoch) :
        mKey(objkey), mCycle(cycle), mEpoch(epoch)
    {
        mFlag.Reset();
    }

    virtual ~BaseArbitratee() {}

    inline void GetOBJKey(OBJKey** objkey)
    {
        *objkey = &mKey;
    }

    inline ywb_uint64_t GetCycle()
    {
        return mCycle;
    }

    inline ywb_uint64_t GetEpoch()
    {
        return mEpoch;
    }

    inline void SetCycle(ywb_uint64_t cycle)
    {
        mCycle = cycle;
    }

    inline void SetEpoch(ywb_uint64_t epoch)
    {
        mEpoch = epoch;
    }

    inline void SetFlagComb(ywb_uint32_t flag)
    {
        mFlag.SetFlag(flag);
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

    /*translation between OBJKey and ArbitrateeKey*/
    void OBJKey2ArbitrateeKey(OBJKey* objkey,
            ArbitrateeKey* arbitrateekey);
    /*translation between ArbitrateeKey and OBJKey*/
    void ArbitrateeKey2OBJKey(
            ArbitrateeKey* arbitrateekey, OBJKey* objkey);

private:
    /*flags*/
    Flag mFlag;
    /*OBJ key associated with this arbitratee*/
    OBJKey mKey;
    /*
     * relative cycle to the AST enabled
     * record the generation cycle of this item,
     * record the cycle in which this item is migrated successfully
     **/
    ywb_uint64_t mCycle;
    /*to which epoch this arbitratee belongs*/
    ywb_uint64_t mEpoch;
};

class Arbitratee : public BaseArbitratee {
public:
    Arbitratee() : mTargetId(YWB_UINT64_MAX), mSubmittedTime(0),
            mExpireTime(Constant::ARBITRATEE_EXPIRE_TIME),
            mStatType(IOStat::IOST_cnt), mDir(Plan::PD_cnt), mCap(0)
    {

    }

    Arbitratee(OBJKey& objkey, ywb_uint64_t tgtid,
            SubPoolKey& subpoolkey, ChunkIOStat& stat,
            ywb_uint32_t stattype, ywb_uint32_t direction,
            ywb_uint32_t cap, ywb_uint32_t cycle, ywb_uint64_t epoch) :
                BaseArbitratee(objkey, cycle, epoch),
                mTargetId(tgtid), mSubmittedTime(0),
                mExpireTime(Constant::ARBITRATEE_EXPIRE_TIME),
                mSubPoolKey(subpoolkey), mStat(stat),
                mStatType(stattype), mDir(direction), mCap(cap) {}

    inline ywb_uint64_t GetTargetID()
    {
        return mTargetId;
    }

    inline ywb_time_t GetSubmittedTime()
    {
        return mSubmittedTime;
    }

    inline ywb_time_t GetExpireTime()
    {
        return mExpireTime;
    }

    inline void GetSubPoolKey(SubPoolKey** subpoolkey)
    {
        *subpoolkey = &mSubPoolKey;
    }

    inline void GetIOStat(ChunkIOStat** stat)
    {
        *stat = &mStat;
    }

    inline ywb_uint32_t GetStatType()
    {
        return mStatType;
    }

    inline ywb_uint32_t GetDirection()
    {
        return mDir;
    }

    inline ywb_uint32_t GetCap()
    {
        return mCap;
    }

    inline void SetTargetId(ywb_uint64_t tgtid)
    {
        mTargetId = tgtid;
    }

    inline void SetSubmittedTime(ywb_time_t now)
    {
        mSubmittedTime = now;
    }

    inline void SetExpireTime(ywb_time_t expire)
    {
        mExpireTime = expire;
    }

    inline void SetIOStat(ChunkIOStat& stat)
    {
        mStat = stat;
    }

    inline void SetStatType(ywb_uint32_t type)
    {
        mStatType = type;
    }

    inline void SetDirection(ywb_uint32_t direction)
    {
        mDir = direction;
    }

    inline void SetCap(ywb_uint32_t cap)
    {
        mCap = cap;
    }

private:
    /*
     * target disk id for migration
     * can get target disk key via subpool key and target disk id
     * can get source disk key via subpool key and OBJ key
     **/
    ywb_uint64_t mTargetId;
    /*when arbitratee is submitted to migration*/
    ywb_time_t mSubmittedTime;
    /*expiration time interval*/
    ywb_time_t mExpireTime;
    /*subpool key*/
    SubPoolKey mSubPoolKey;
    /*IO statistics used for reservation*/
    ChunkIOStat mStat;
    /*of which IOStat::Type the @mStat is, derived from Plan*/
    ywb_uint32_t mStatType;
    /*plan direction, derived from Plan*/
    ywb_uint32_t mDir;
    /*capacity requirement*/
    ywb_uint32_t mCap;
};

class FakeArbitrateeForStop : public BaseArbitratee
{
public:
    FakeArbitrateeForStop()
    {
        SetFlagUniq(BaseArbitratee::BAF_fake_stop);
    }
};

class FakeArbitrateeForCompleteMigration : public BaseArbitratee
{
public:
    FakeArbitrateeForCompleteMigration(OBJKey objkey, ywb_status_t status) :
        BaseArbitratee(objkey, 0, 0), mMigrationStatus(status)
    {
        SetFlagUniq(BaseArbitratee::BAF_fake_complete_mig);
    }

    inline ywb_status_t GetMigrationStatus()
    {
        return mMigrationStatus;
    }

private:
    ywb_status_t mMigrationStatus;
};

class FakeArbitrateeForCheckExpired : public BaseArbitratee
{
public:
    FakeArbitrateeForCheckExpired()
    {
        SetFlagUniq(BaseArbitratee::BAF_fake_check_expired);
    }
};

class SubPoolArbitratee {
public:
    ~SubPoolArbitratee()
    {
        Destroy();
    }

    ywb_status_t GetArbitratee(ArbitrateeKey& key, Arbitratee** val);
    ywb_status_t AddArbitratee(ArbitrateeKey& key, Arbitratee* val);
    /*
     * NOTE:
     * this function will not delete corresponding arbitratee.
     * more attention should be paid when using  RemoveArbitratee
     * and GetFirstArbitratee/GetNextArbitratee alternate.
     **/
    ywb_status_t RemoveArbitratee(ArbitrateeKey& key);

    /*
     * NOTE: GetFirstArbitratee and GetNextArbitratee both are not
     * thread-safe.
     *
     * Even in single thread environment, must pay attention to
     * following two points:
     * (1) Add a new arbitratee after GetNextArbitratee reaches the
     *     end, now that the iterator reaches the end, newly added
     *     arbitratees can not be accessed by GetNextArbitratee.
     *
     * (2) Delete an arbitratee to which current iterator points,
     *     this will leads to unexpected result.
     *
     * Following are based on single thread environment.
     * For (1), can be solved(but point (2) still exists) by not
     * change the iterator when call GetFirstArbitratee and
     * GetNextArbitratee, but instead change it at next invocation,
     * as below:
     * ywb_status_t GetFirstArbitratee(Arbitratee** val)
     *   {
     *       ywb_status_t ret = YWB_SUCCESS;
     *
     *       if(!mItems2.empty())
     *       {
     *           mIt2 = mItems2.begin();
     *           *val = *mIt2;
     *       }
     *       else
     *       {
     *           *val = NULL;
     *           ret = YFS_ENOENT;
     *       }
     *
     *       return ret;
     *   }
     *
     *   ywb_status_t GetNextArbitratee(Arbitratee** val)
     *   {
     *       ywb_status_t ret = YWB_SUCCESS;
     *
     *       mIt2++;
     *       if(mIt2 != mItems2.end())
     *       {
     *           *val = *mIt2;
     *       }
     *       else
     *       {
     *           //backward one step
     *           mIt2--;
     *           *val = NULL;
     *           ret = YFS_ENOENT;
     *       }
     *
     *       return ret;
     *   }
     *
     *   For (2), can not be solved for deletion operation may lies
     *   between calling of GetFirstArbitratee and GetNextArbitratee,
     *   which may leads the iterator to be invalid, for example:
     *   ret = GetFirstArbitratee(&arbitratee);
     *   while((YWB_SUCCESS == ret) && (arbitratee != NULL))
     *   {
     *      do something against arbitratee;
     *      delete arbitratee;
     *      ret = GetNextArbitratee(&arbitratee);
     *   }
     *
     *   Considering current usage of GetFirstArbitratee and
     *   GetNextArbitratee will not encounter point(1) problem
     *   (for these two are called only in Arbitrator::Reset
     *   currently), we re-implement these two functions by
     *   ignoring point (1) problem.
     **/
    ywb_status_t GetFirstArbitratee(Arbitratee** val);
    ywb_status_t GetNextArbitratee(Arbitratee** val);

    ywb_bool_t Empty();
    ywb_status_t Destroy();

private:
    /*for searching*/
    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp> mItems;
    /*for traversing*/
    list<Arbitratee*> mItems2;
    list<Arbitratee*>::iterator mIt2;
};

class Arbitrator {
public:
    Arbitrator(AST* ast) : mCTRLeaseTime(Constant::DEFAULT_CTR_LEASE_TIME),
            mITRLeaseTime(Constant::DEFAULT_ITR_LEASE_TIME),
            mWDLeaseTime(Constant::DEFAULT_WD_LEASE_TIME),
            mAst(ast) {}

    ~Arbitrator()
    {
        Destroy();
    }

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    inline ywb_uint32_t GetLeaseTime(ywb_uint32_t direction)
    {
        ywb_uint32_t leasetime = 0;

        YWB_ASSERT((0 <= direction) && (direction < Plan::PD_cnt));
        if(Plan::PD_ctr_promote == direction ||
                Plan::PD_ctr_demote == direction)
        {
            leasetime = mCTRLeaseTime;
        }

        return leasetime;
    }

    ywb_status_t GetSubPoolArbitrateeFromIn(SubPoolKey& subpoolkey,
            SubPoolArbitratee** subpoolarbitratees);
    ywb_status_t GetSubPoolArbitrateeFromOut(SubPoolKey& subpoolkey,
            SubPoolArbitratee** subpoolarbitratees);

    ywb_status_t AddArbitrateeIn(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, Arbitratee* val);
    /*NOTE: this function will not delete corresponding arbitratee*/
    ywb_status_t RemoveArbitrateeIn(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey);
    ywb_status_t RemoveSubPoolArbitrateeIn(SubPoolKey& subpoolkey);

    ywb_status_t AddArbitrateeOut(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, Arbitratee* val);
    /*NOTE: this function will not delete corresponding arbitratee*/
    ywb_status_t RemoveArbitrateeOut(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey);
    ywb_status_t RemoveSubPoolArbitrateeOut(SubPoolKey& subpoolkey);

    ywb_status_t AddNotCompletedMigratingItem(ArbitrateeKey&, Arbitratee*);
    ywb_status_t GetNotCompletedMigratingItem(ArbitrateeKey&, Arbitratee**);
    /*NOTE: this function will delete corresponding arbitratee*/
    ywb_status_t RemoveNotCompletedMigratingItem(ArbitrateeKey&);

    /*
     * check whether or not the configuration change:
     * 1. whether the subpool does not exist
     * 2. whether the source/target disk does not exist
     * 3. whether the associated OBJ does not exist
     **/
    ywb_bool_t CheckConfigChange(SubPoolKey& subpoolkey,
            Arbitratee* arbitratee);

    /*
     * figure out whether this item is conflicted with other arbitratee,
     * if two arbitratees come from the same OBJ but has different
     * plan direction, then the one has higher static priority will be
     * kept, the other one will be dropped
     *
     * called before one arbitratee is added into SubPoolArbitratee
     *
     * IN:
     *      subpoolkey - to which the arbitratee belongs
     *      arbitrateekey - to be arbitrated arbitratee
     *      arbitratee - arbitratee corresponding to @arbitrateekey,
     *                   used for setting the substituted arbitratee
     *                   if this abitratee has higher priority
     * OUT: whether or not conflict exist
     **/
    ywb_bool_t CheckConflict(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, Arbitratee* arbitratee);

    /*
     * figure out whether this item is frequently migrated up and down
     * in lease time
     **/
    ywb_bool_t CheckFrequentUpDown(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey);

    /*check whether or not the benefit is greater than cost*/
    ywb_bool_t CheckBenefitOverCost(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey);

    /*
     * get new target for plan(arbitratee) generated in previous cycle
     *
     * re-choose target disk is based on below considerations:
     * (1) for plan generated in previous cycle, target disk is chosen
     *     according to previous cycle's circumstance, which may change
     *     in current cycle
     * (2) if directly drop plan generated in previous cycle, then plan
     *     with high priority may be dropped which cause the high priority
     *     plan not be migrated
     *
     * BUT currently, this functionality is not supported, for:
     * (1) re-choose procedure is time-consumed
     * (2) the disk at which previous cycle's plan targeted will not be
     *     chosen by another plan in current design, so the disk is still
     *     available
     *
     * Current implementation:
     * To avoid one disk is owned by one plan for a long time(such as 3
     * cycles, defined by Constant::PLAN_BIND_DISK_TIME_LIMIT) without
     * migration, the plan will be dropped unconditionally so that the
     * targeting disk can be used by other plans
     **/
    ywb_status_t ReChooseMigrationTarget(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, ywb_uint32_t stattype);

    /*reserve for one arbitratee on both source disk and  target disk*/
    ywb_status_t Reserve(SubPoolKey& subpoolkey, Arbitratee* arbitratee);
    /*reserve for arbitratee of given key*/
    ywb_status_t Reserve(SubPoolKey& subpoolkey, ArbitrateeKey& arbitrateekey);
    /*unreserve for one arbitratee on both source disk and  target disk*/
    ywb_status_t UnReserve(SubPoolKey& subpoolkey, Arbitratee* arbitratee);
    /*unreserve for arbitratee of given key*/
    ywb_status_t UnReserve(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey);
    /*
     * consume the reserved performance for given arbitratee
     * MUST be called before arbitratee is removed from IN @mItemsIn
     * */
    ywb_status_t Consume(SubPoolKey& subpoolkey, Arbitratee* arbitratee);
    /*
     * consume the reserved performance for given arbitratee
     * MUST be called before arbitratee is removed from IN @mItemsIn
     * */
    ywb_status_t Consume(SubPoolKey& subpoolkey, ArbitrateeKey& arbitrateekey);

    /*receive plan from plan module*/
    ywb_status_t ReceivePlan(SubPoolKey& subpoolkey, Plan* plan);

    /*submit arbitratee to migration module*/
    ywb_status_t Submit(SubPoolKey& subpoolkey, Arbitratee* item);

    /*complete the migration plan*/
    ywb_status_t Complete(SubPoolKey& subpoolkey, ArbitrateeKey& arbitrateekey,
            ywb_bool_t curwindow, ywb_status_t migstatus);
    /*one migration plan expired*/
    ywb_status_t Expire(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, ywb_bool_t curwindow);

    /*destroy all arbitratees IN*/
    void DrainAllArbitrateesIn();
    /*destroy all arbitratees OUT*/
    void DrainAllArbitrateesOut();
    /*destroy all arbitratees IN and OUT*/
    void DrainAllArbitratees();

//    /*handle those reserved but not submitted to migrate arbitratees*/
//    ywb_status_t HandleReservedButNotMigrated();
    /*
     * when new decision cycle comes, new advice/plan/arbitratee(for current
     * decision window) will be generated, but some arbitratees are still
     * in-flight which must be retained until echoed.
     *
     * BUT to avoid one OBJ from being migrated up and down frequently, those
     * already migrated arbitratees in @mItemsOut will be kept until they are
     * expired. For those expired arbitratees, they will be deleted now.
     *
     * @all:
     *      reset all fields if set to ywb_true;
     *      retain some info for future use otherwise;
     **/
    ywb_status_t Reset(ywb_bool_t all);

    ywb_status_t Destroy();

private:
    /*
     * manage all those to be migrated arbitratees, arbitratees are generated
     * from submitted plans, and then ARB do some check before migration
     **/
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp> mItemsIn;
//    /*
//     * manage all arbitratees of previously decision window
//     * in BAF_migrating state
//     * */
//    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>
//            mNotCompletedMigratingItems;
    /*
     * manage all those already migrated arbitratees, arbitratees will not be
     * deleted until the lease time expires
     *
     * arbitratee will be deleted at following two points:
     * (1) frequent up and down checking, if one checked arbitratee is found
     *     already expired, then it will be deleted
     * (2) new decision cycle comes, traverse all items in @mItemsOut, find
     *     those expired ones, and then delete them
     **/
    map<SubPoolKey, SubPoolArbitratee*, SubPoolKeyCmp> mItemsOut;

    /*CTR lease time*/
    ywb_uint32_t mCTRLeaseTime;
    /*ITR lease time*/
    ywb_uint32_t mITRLeaseTime;
    /*WD lease time*/
    ywb_uint32_t mWDLeaseTime;

    AST* mAst;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
