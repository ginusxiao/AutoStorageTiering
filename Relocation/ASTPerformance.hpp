#ifndef __AST_PERFORMANCE_HPP__
#define __AST_PERFORMANCE_HPP__

#include <vector>
#include <map>
#include <set>
#include <list>
#include "common/FsInts.h"
#include "common/FsStatus.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTDiskProfile.hpp"
#include "AST/ASTIO.hpp"
#include "AST/AST.hpp"

using namespace std;

/*forward reference*/
class TierDiskAvailPerfRanking;
class SubPoolDiskAvailPerfRanking;
class PerfManager;

class BlackList
{
public:
    ~BlackList()
    {
        if(!mDisks.empty())
        {
            mDisks.clear();
        }
    }

    bool Empty();

    ywb_status_t AddDisk(DiskKey& diskkey);
    ywb_status_t RemoveDisk(DiskKey& diskkey);

    ywb_status_t GetFirstDisk(DiskKey* diskkey);
    ywb_status_t GetNextDisk(DiskKey* diskkey);
    ywb_status_t Find(DiskKey&);

    /*
     * get difference set of BlackList A and B, leave those
     * disks in A but not in B
     **/
    ywb_status_t GetDifferenceSet(BlackList* listb);

    ywb_status_t Reset();
    ywb_status_t Destroy();

private:
    set<DiskKey, DiskKeyCmp> mDisks;
    set<DiskKey, DiskKeyCmp>::iterator mIt;
};

typedef BlackList WhiteList;

/*disk performance stats*/
class DiskPerf {
public:
    DiskPerf(ywb_uint32_t cycleperiod, DiskPerfProfile* profile,
            DiskIO* diskio, Disk* disk) : mCyclePeriod(cycleperiod),
            mIOThreshold(0), mIOConsumed(0),
            mBWThreshold(0), mBWConsumed(0),
            mCapTotal(0), mCapConsumed(0),
            mDiskPerfProfile(profile),
            mDiskIO(diskio), mDisk(disk)
    {
        /*
         * @mDiskPerfProfile maybe NULL if disks' performance
         * profile has no entry corresponding
         **/
        YWB_ASSERT(mDiskIO != NULL);
        YWB_ASSERT(mDisk != NULL);
        mDisk->IncRef();
    }

    ~DiskPerf()
    {
        if(mDisk)
        {
            mDisk->DecRef();
            mDisk = NULL;
        }

        mDiskPerfProfile = NULL;
        mDiskIO = NULL;
    }

    /*calculate disk performance according to IO statistics*/
    ywb_status_t CalculateDiskPerf(ywb_uint32_t perftype);

    inline ywb_uint32_t GetCyclePerid();
    inline ywb_uint32_t GetIOThreshold();
    inline ywb_uint32_t GetIOConsumed();
    inline ywb_uint32_t GetBWThreshold();
    inline ywb_uint32_t GetBWConsumed();
    inline ywb_int32_t GetCapTotal();
    inline ywb_int32_t GetCapConsumed();
    inline void GetDiskProfile(DiskPerfProfile** profile);
    inline void GetDiskIO(DiskIO** diskio);
    inline void GetDisk(Disk** disk);

    inline void SetCyclePeriod(ywb_uint32_t val);
    inline void SetIOThreshold(ywb_uint32_t val);
    inline void SetIOConsumed(ywb_uint32_t val);
    inline void SetBWThreshold(ywb_uint32_t val);
    inline void SetBWConsumed(ywb_uint32_t val);
    inline void SetCapTotal(ywb_int32_t val);
    inline void SetCapConsumed(ywb_int32_t val);

private:
//    /*disk key*/
//    DiskKey mkey;
    /*
     * how long one cycle lasts for, used for calculating IO/BW
     * threshold of one cycle
     * */
    ywb_uint32_t mCyclePeriod;
    /*IOPS threshold*/
    ywb_uint32_t mIOThreshold;
    /*IOPS consumed*/
    ywb_uint32_t mIOConsumed;

    /*BW threshold*/
    ywb_uint32_t mBWThreshold;
    /*BW consumed*/
    ywb_uint32_t mBWConsumed;

    /*
     * total capacity, use ywb_int32_t instead of ywb_uint32_t
     * for capacity the user reserve/unreserve/consume can be
     * negative numbers
     **/
    ywb_int32_t mCapTotal;
    /*capacity consumed*/
    ywb_int32_t mCapConsumed;

    /*pointer to disk performance profile*/
    DiskPerfProfile* mDiskPerfProfile;
    /*reference to disk IO statistics*/
    DiskIO* mDiskIO;
    /*reference to disk*/
    Disk* mDisk;
};

/*ranking unit of disk available performance*/
class DiskAvailPerfRankingUnit {
public:
    friend class DiskAvailPerfRankingCmp;
    DiskAvailPerfRankingUnit(DiskKey& key, DiskPerf*& perf,
            TierDiskAvailPerfRanking* tierperf) :
        mKey(key), mIOAvail(0), mIOReserved(0), mBWAvail(0),
        mBWReserved(0), mCapAvail(0), mCapReserved(0),
        mDiskPerf(perf), mTierPerf(tierperf)
    {
        YWB_ASSERT(mDiskPerf != NULL);
    }

    ~DiskAvailPerfRankingUnit()
    {
        if(mDiskPerf)
        {
            delete mDiskPerf;
            mDiskPerf = NULL;
        }

        mTierPerf = NULL;
    }

    inline void GetDiskKey(DiskKey** key);
    inline ywb_uint32_t GetIOAvail();
    inline ywb_uint32_t GetIOReserved();
    inline ywb_uint32_t GetBWAvail();
    inline ywb_uint32_t GetBWReserved();
    inline ywb_int32_t GetCapAvail();
    inline ywb_int32_t GetCapReserved();
    inline void GetDiskPerf(DiskPerf** diskperf);
    inline void GetTierPerf(TierDiskAvailPerfRanking** tierperf);

    /*calculate disk available performance*/
    void CalculateDiskAvailPerf();
    /*reserve io/bw/cap*/
    ywb_status_t Reserve(ywb_int32_t iosrequired,
            ywb_int32_t bwrequired, ywb_int32_t caprequired);
    /*unreserve io/bw/cap*/
    ywb_status_t UnReserve(ywb_int32_t iosrequired,
            ywb_int32_t bwrequired, ywb_int32_t caprequired);
    ywb_status_t Consume(ywb_int32_t iosrequired,
            ywb_int32_t bwrequired, ywb_int32_t caprequired);

private:
    /*disk key*/
    DiskKey mKey;
    /*available iops of disk*/
    ywb_uint32_t mIOAvail;
    /*IOPS reserved*/
    ywb_uint32_t mIOReserved;
    /*available bandwidth of disk*/
    ywb_uint32_t mBWAvail;
    /*BW reserved*/
    ywb_uint32_t mBWReserved;
    /*
     * available capacity, use ywb_int32_t instead of ywb_uint32_t
     * for capacity the user reserve/unreserve/consume can be
     * negative numbers
     **/
    ywb_int32_t mCapAvail;
    /*capacity reserved*/
    ywb_int32_t mCapReserved;

    /*reference to disk performance*/
    DiskPerf* mDiskPerf;
    /*back reference to tier available performance ranking*/
    TierDiskAvailPerfRanking* mTierPerf;
};

class DiskAvailPerfRankingCmp
{
public:
    typedef bool (*Greater)(DiskAvailPerfRankingUnit*, DiskAvailPerfRankingUnit*);

    static bool DiskAvailPerfRankingGreater(
            DiskAvailPerfRankingUnit* diskunit1,
            DiskAvailPerfRankingUnit* diskunit2)
    {
        if(diskunit1->mIOAvail != diskunit2->mIOAvail)
        {
            return diskunit1->mIOAvail > diskunit2->mIOAvail;
        }
        else if(diskunit1->mBWAvail != diskunit2->mBWAvail)
        {
            return diskunit1->mBWAvail > diskunit2->mBWAvail;
        }
        else if(diskunit1->mCapAvail != diskunit2->mCapAvail)
        {
            return diskunit1->mCapAvail > diskunit2->mCapAvail;
        }
        else
        {
            return false;
        }
    }
};

/*tier-wise disk ranking*/
class TierDiskAvailPerfRanking {
public:
    TierDiskAvailPerfRanking(SubPoolDiskAvailPerfRanking* perfranking,
            DiskAvailPerfRankingCmp::Greater greaterthan) :
                mGreater(greaterthan), mSubPoolPerf(perfranking)
    {
        YWB_ASSERT(perfranking != NULL);
    }

    ~TierDiskAvailPerfRanking()
    {
        Destroy();
    }

    inline void GetAST(AST** ast);
    inline void GetSubPoolDiskAvailPerfRanking(
            SubPoolDiskAvailPerfRanking** subpoolperf);
    /*
     * setup disk available performance ranking unit of certain
     * performance type
     **/
    ywb_status_t SetupDiskAvailPerfRankingUnit(DiskKey& key,
            ywb_uint32_t perftype);
    /*setup disk available performance ranking unit*/
    ywb_status_t SetupDiskAvailPerfRankingUnit(DiskKey& key);
    /*get disk's available performance ranking unit of certain type*/
    ywb_status_t GetDiskAvailPerfRankingUnit(DiskKey& key,
            ywb_uint32_t perftype, DiskAvailPerfRankingUnit** diskunit);
    /*remove disk's available performance ranking unit*/
    ywb_status_t RemoveDiskAvailPerfRankingUnit(DiskKey& key);

    /*sort tier-wise disks (only) at the very beginning of each cycle*/
    void SortDisks(ywb_uint32_t perftype);
    void SortDisks();
    /*for test only*/
    void GetSortedDisks(ywb_uint32_t perftype, vector<DiskKey>* diskvec);

    /*reserve io/bw/cap of specifed performance type on specified disk*/
    ywb_status_t Reserve(DiskKey& key, ywb_uint32_t perftype,
            ywb_int32_t iosrequired, ywb_int32_t bwrequired,
            ywb_int32_t caprequired);
    /*unreserve io/bw/cap of specifed performance type on specified disk*/
    ywb_status_t UnReserve(DiskKey& key, ywb_uint32_t perftype,
            ywb_int32_t iosrequired, ywb_int32_t bwrequired,
            ywb_int32_t caprequired);
    /*consume io/bw/cap of specifed performance type on specified disk*/
    ywb_status_t Consume(DiskKey& key, ywb_uint32_t perftype,
            ywb_int32_t iosrequired, ywb_int32_t bwrequired,
            ywb_int32_t caprequired);

    /*
     * get the most favorite disk, currently take the following into
     * consideration:
     * 1. disk's performance
     * 2. disk's schedule status(reflected by blacklist and whitelist)
     *
     * blacklist - list of disks which will not be chosen
     * whitelist - list of disks from which the favorite disk comes
     *
     * if whitelist is not assigned, then build it internally,
     * if blacklist is assigned, then omit those intersection items.
     *
     * for each favorite disk, DO NOT modify fields in DiskPerf and
     * DiskAvailRankingUnit here, leave that to Reserve().
     *
     * we claim that "sort disks' performance on demand" for we sort
     * here, which is the last chance to sort disks(it is ok to sort
     * at anywhere else, but we are not sure whether or not there will
     * be a successive GetFavoriteDisk invocation under which case
     * sorting will be useless work)
     *
     * NOTE: DO NOT MODIFY @blacklist and @whitelist internally
     **/
    ywb_status_t GetFavoriteDisk(ywb_uint32_t perftype,
            ywb_uint32_t iopsrequired, ywb_uint32_t bwrequired,
            ywb_int32_t caprequired, BlackList* blacklist,
            WhiteList* whitelist, DiskKey* diskkey);

    ywb_status_t Destroy();

private:
    /*used for indexing*/
    map<DiskKey, DiskAvailPerfRankingUnit*, DiskKeyCmp>
            mIndexing[Constant::DISK_PERF_TYPE_CNT];
    /*accommodate all disks' available performance*/
    vector<DiskAvailPerfRankingUnit*>
            mDiskRanking[Constant::DISK_PERF_TYPE_CNT];
    /*remember current favorite position*/
    ywb_uint32_t mCurFavoritePos[Constant::DISK_PERF_TYPE_CNT];
    /*comparison function*/
    DiskAvailPerfRankingCmp::Greater mGreater;
    /*back reference to subpool perf ranking*/
    SubPoolDiskAvailPerfRanking* mSubPoolPerf;
};

/*subpool-wise disk ranking organized by tier*/
class SubPoolDiskAvailPerfRanking {
public:
    SubPoolDiskAvailPerfRanking(SubPool* subpool, PerfManager* mgr) :
        mSubPool(subpool), mPerfMgr(mgr)
    {
        ywb_uint32_t tier = 0;

        for(; tier < Constant::TIER_NUM; tier++)
        {
            mTierDiskRanking[tier] = NULL;
        }

        YWB_ASSERT(mSubPool != NULL);
        mSubPool->IncRef();
    }

    ~SubPoolDiskAvailPerfRanking()
    {
        Destroy();
    }

    inline void GetPerfManager(PerfManager** mgr);

    /*
     * setup tier-wise disk ranking
     * @tierIndex: tier index
     * @less: compare function
     **/
    ywb_status_t SetupTierDiskAvailPerfRanking(ywb_uint32_t tier,
            DiskAvailPerfRankingCmp::Greater greaterthan);
    ywb_status_t GetTierDiskAvailPerfRanking(ywb_uint32_t tier,
            TierDiskAvailPerfRanking** tieravail);

    /*reserve io/bw/cap of specifed performance type on specified disk*/
    ywb_status_t Reserve(DiskKey& key, ywb_uint32_t perftype,
            ywb_int32_t iosrequired, ywb_int32_t bwrequired,
            ywb_int32_t caprequired);
    /*unreserve io/bw/cap of specifed performance type on specified disk*/
    ywb_status_t UnReserve(DiskKey& key, ywb_uint32_t perftype,
            ywb_int32_t iosrequired, ywb_int32_t bwrequired,
            ywb_int32_t caprequired);
    /*consume io/bw/cap of specified performance type on given disk*/
    ywb_status_t Consume(DiskKey& key, ywb_uint32_t perftype,
            ywb_int32_t iosrequired, ywb_int32_t bwrequired,
            ywb_int32_t caprequired);

    /*
     * get the most favorite disk in given tier
     * NOTE: DO NOT MODIFY @blacklist and @whitelist internally
     **/
    ywb_status_t GetFavoriteDisk(ywb_uint32_t tier, ywb_uint32_t perftype,
            ywb_uint32_t iopsrequired, ywb_uint32_t bwrequired,
            ywb_uint32_t caprequired, BlackList* blacklist,
            WhiteList* whitelist, DiskKey* diskkey);

    /*destroy tier-wise disk ranking*/
    ywb_status_t Destroy(ywb_uint32_t tier);

    /*destroy subpool-wise disk performance ranking*/
    ywb_status_t Destroy();

private:
    /*sub-pool level disk available performance ranking*/
    TierDiskAvailPerfRanking* mTierDiskRanking[Constant::TIER_NUM];
    /*reference to subpool*/
    SubPool* mSubPool;
    /*back reference to performance manager*/
    PerfManager* mPerfMgr;
};

class PerfManager{
public:
    PerfManager(AST* ast)
    {
       mAst = ast;
    }

    ~PerfManager()
    {
        Destroy();
    }

    inline void GetAST(AST** ast);

    /*
     * setup sub-pool's all tiers's disk ranking
     * @key: sub-pool key
     * @Greater: compare function
     **/
    ywb_status_t SetupSubPoolDiskAvailPerfRanking(SubPoolKey& key,
            DiskAvailPerfRankingCmp::Greater greaterthan);

    /*get specified subpool's disk available performance ranking*/
    ywb_status_t GetSubPoolDiskAvailPerfRanking(SubPoolKey& key,
            SubPoolDiskAvailPerfRanking** subpoolavail);

    /*
     * get the most favorite disk in given sub-pool and tier
     *
     * blacklist - list of disks which will not be chosen
     * whitelist - list of disks from which the favorite disk comes
     *
     * whitelist is prior to blacklist, that is if both of them
     * are assigned, then only whitelist is considered
     *
     * if both of blacklist and whitelist are not assigned, then
     * all disks will be taken into consideration
     *
     * NOTE: DO NOT MODIFY @blacklist and @whitelist internally
     **/
    ywb_status_t GetFavoriteDisk(SubPoolKey& key, ywb_uint32_t tier,
            ywb_uint32_t perftype, ywb_uint32_t iopsrequired,
            ywb_uint32_t bwrequired, ywb_uint32_t caprequired,
            BlackList* blacklist, WhiteList* whitelist, DiskKey* diskkey);

    /*destroy certain subpool's disk performance ranking*/
    ywb_status_t Destroy(SubPoolKey& key);
    /*destroy overall*/
    ywb_status_t Destroy();

    /*reset disk performance of last decision window*/
    ywb_status_t Reset();
    /*init disk performance for new decision window*/
    ywb_status_t Init();

private:
    /*node-wise disk ranking organized by subpool*/
    map<SubPoolKey, SubPoolDiskAvailPerfRanking*, SubPoolKeyCmp>
            mSubPoolAvailPerfRanking;
    /*reference to AST*/
    AST* mAst;
};

#include "AST/ASTPerformanceInline.hpp"

#endif

/* vim:set ts=4 sw=4 expandtab */
