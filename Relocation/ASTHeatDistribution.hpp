#ifndef __AST_HEAT_DISTRIBUTION_HPP__
#define __AST_HEAT_DISTRIBUTION_HPP__

#include <vector>
#include <map>
#include <set>
#include <string>
#include <sstream>
#include "common/FsInts.h"
#include "common/FsStatus.h"
#include "AST/ASTConstant.hpp"
#include "OSSCommon/Logger.hpp"
#include "AST/ASTIO.hpp"
#include "AST/ASTAdviceRule.hpp"

using namespace std;

class HeatCmp
{
public:
    /*
     * check if @obj1 is greater than @obj2
     *
     * WARN: std::sort may coredump if comparison of two equal value
     * returns true, so Greater/Less can only returns true if one
     * operand do greater/less than another operand, otherwise false
     * should be returned. This means Greater not equals to !Less,
     * and Less not equals to !Greater.
     **/
    typedef bool (*Greater)(const OBJVal* obj1, const OBJVal* obj2);
    /*check if @obj1 is less than @obj2*/
    typedef bool (*Less)(const OBJVal* obj1, const OBJVal* obj2);

    /*helper rotuines for Greater/Less comparison, random IO/BW/RT*/
    static ywb_int32_t LongTermEMARndIOsCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t LongTermEMARndBWCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t LongTermEMARndRTCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t ShortTermEMARndIOsCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t ShortTermEMARndBWCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t ShortTermEMARndRTCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t RawRndIOsCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t RawRndBWCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t RawRndRTCmp(const OBJVal* obj1, const OBJVal* obj2);

    /*helper rotuines for Greater/Less comparison, sequential IO/BW/RT*/
    static ywb_int32_t LongTermEMASeqIOsCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t LongTermEMASeqBWCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t LongTermEMASeqRTCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t ShortTermEMASeqIOsCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t ShortTermEMASeqBWCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t ShortTermEMASeqRTCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t RawSeqIOsCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t RawSeqBWCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t RawSeqRTCmp(const OBJVal* obj1, const OBJVal* obj2);

    /*helper rotuines for Greater/Less comparison, random+sequential IO/BW/RT*/
    static ywb_int32_t LongTermEMAAccumIOsCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t LongTermEMAAccumBWCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t LongTermEMAAccumRTCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t ShortTermEMAAccumIOsCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t ShortTermEMAAccumBWCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t ShortTermEMAAccumRTCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t RawAccumIOsCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t RawAccumBWCmp(const OBJVal* obj1, const OBJVal* obj2);
    static ywb_int32_t RawAccumRTCmp(const OBJVal* obj1, const OBJVal* obj2);

    /*random IO/BW/RT*/
    static bool GreaterByLongTermEMARndIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByLongTermEMARndBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByLongTermEMARndRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByShortTermEMARndIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByShortTermEMARndBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByShortTermEMARndRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByRawRndIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByRawRndBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByRawRndRT(const OBJVal* obj1, const OBJVal* obj2);

    /*sequential IO/BW/RT*/
    static bool GreaterByLongTermEMASeqIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByLongTermEMASeqBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByLongTermEMASeqRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByShortTermEMASeqIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByShortTermEMASeqBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByShortTermEMASeqRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByRawSeqIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByRawSeqBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByRawSeqRT(const OBJVal* obj1, const OBJVal* obj2);

    /*random+sequential IO/BW/RT*/
    static bool GreaterByLongTermEMAAccumIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByLongTermEMAAccumBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByLongTermEMAAccumRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByShortTermEMAAccumIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByShortTermEMAAccumBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByShortTermEMAAccumRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByRawAccumIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByRawAccumBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool GreaterByRawAccumRT(const OBJVal* obj1, const OBJVal* obj2);

    /*random IO/BW/RT*/
    static bool LessByLongTermEMARndIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByLongTermEMARndBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByLongTermEMARndRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByShortTermEMARndIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByShortTermEMARndBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByShortTermEMARndRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByRawRndIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByRawRndBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByRawRndRT(const OBJVal* obj1, const OBJVal* obj2);

    /*sequential IO/BW/RT*/
    static bool LessByLongTermEMASeqIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByLongTermEMASeqBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByLongTermEMASeqRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByShortTermEMASeqIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByShortTermEMASeqBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByShortTermEMASeqRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByRawSeqIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByRawSeqBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByRawSeqRT(const OBJVal* obj1, const OBJVal* obj2);

    /*random+sequential IO/BW/RT*/
    static bool LessByLongTermEMAAccumIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByLongTermEMAAccumBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByLongTermEMAAccumRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByShortTermEMAAccumIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByShortTermEMAAccumBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByShortTermEMAAccumRT(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByRawAccumIOs(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByRawAccumBW(const OBJVal* obj1, const OBJVal* obj2);
    static bool LessByRawAccumRT(const OBJVal* obj1, const OBJVal* obj2);
};

/*heat range*/
class HeatRange {
public:
//    enum Type{
//        HRT_raw = 0,
//        HRT_short_term_ema,
//        HRT_long_term_ema,
//        HRT_cnt,
//    };
//    typedef IOStat::Type Type;

    HeatRange() : mMin(YWB_UINT32_MAX), mMax(0), mBucketCap(YWB_UINT32_MAX),
    mMinPortionBoundary(mMin), mMaxPortionBoundary(mMax), mAvg(0), mSum(0) {}

    HeatRange(ywb_uint32_t min, ywb_uint32_t max, ywb_uint32_t minboundary,
            ywb_uint32_t maxboundary, ywb_uint32_t avg, ywb_uint64_t sum) :
                mMin(min), mMax(max), mMinPortionBoundary(minboundary),
                mMaxPortionBoundary(maxboundary), mAvg(avg), mSum(sum)
    {
        /*
         * FIXME: much attention should be paid for the situation that
         * @max and @min are equal to or approximately equal to each
         * other!
         * */
        if(max > min)
        {
            /*FIXME: shall we roundup?*/
            mBucketCap = (max - min) / (Constant::BUCKET_NUM);
        }
        else
        {
            mBucketCap = YWB_UINT32_MAX;
        }
    }

    HeatRange(const HeatRange& range)
    {
        mMin = range.mMin;
        mMax = range.mMax;
        mBucketCap = range.mBucketCap;
        mMinPortionBoundary = range.mMinPortionBoundary;
        mMaxPortionBoundary = range.mMaxPortionBoundary;
        mAvg = range.mAvg;
        mSum = range.mSum;
    }

    HeatRange& operator= (const HeatRange& range)
    {
        if(this == &range)
            return *this;

        mMin = range.mMin;
        mMax = range.mMax;
        mBucketCap = range.mBucketCap;
        mMinPortionBoundary = range.mMinPortionBoundary;
        mMaxPortionBoundary = range.mMaxPortionBoundary;
        mAvg = range.mAvg;
        mSum = range.mSum;

        return *this;
    }

    inline ywb_uint32_t GetMin();
    inline ywb_uint32_t GetMax();
    inline ywb_uint32_t GetBucketCap();
    inline ywb_uint32_t GetMinPortionBoundary();
    inline ywb_uint32_t GetMaxPortionBoundary();
    inline ywb_uint32_t GetAvg();
    inline ywb_uint64_t GetSum();

    inline void SetMin(ywb_uint32_t val);
    inline void SetMax(ywb_uint32_t val);
    inline void SetBucketCap(ywb_uint32_t val);
    inline void SetMinPortionBoundary(ywb_uint32_t val);
    inline void SetMaxPortionBoundary(ywb_uint32_t val);
    inline void SetAvg(ywb_uint32_t val);
    inline void SetSum(ywb_uint64_t val);
    inline void UpdateSum(ywb_uint64_t val, ywb_bool_t add);

    void Calculate(ywb_uint32_t objnum);
    void Dump(ostringstream* ostr);

private:
    ywb_uint32_t mMin;
    ywb_uint32_t mMax;
    /*generally equals to (mMax - mMin) / BUCKET_NUM*/
    ywb_uint32_t mBucketCap;
    /*
     * following four fields will be calculated whatever, but
     * used only when @mMin and @mMax are approximately equal.
     * */
    ywb_uint32_t mMinPortionBoundary;
    ywb_uint32_t mMaxPortionBoundary;
    /*
     * average IO activity, it is very likely to be inaccurate,
     * but use it in case of all OBJs have approximately same
     * IO activity.
     * */
    ywb_uint32_t mAvg;
    /*total IO activity*/
    ywb_uint64_t mSum;
};

/*
 * heat range on different dimension(random/sequential read/write,
 * response time, bandwidth)
 **/
class HeatRangeSummary {
public:
    HeatRangeSummary() : mRndReadCnt(), mRndWriteCnt(),
    mRndAccumIOs(), mRndAccumRT(),
    mRndAccumBW(), mSeqReadCnt(),
    mSeqWriteCnt(), mSeqAccumIOs(),
    mSeqAccumRT(), mSeqAccumBW(),
    mRndSeqAccumIOs(), mRndSeqAccumBW(),
    mRndSeqAccumRT(), mHeatRangeType(IOStat::IOST_cnt){}

    inline void GetRndReadCnt(HeatRange** range);
    inline void GetRndWriteCnt(HeatRange** range);
    inline void GetRndAccumIOs(HeatRange** range);
    inline void GetRndAccumRT(HeatRange** range);
    inline void GetRndAccumBW(HeatRange** range);
    inline void GetSeqReadCnt(HeatRange** range);
    inline void GetSeqWriteCnt(HeatRange** range);
    inline void GetSeqAccumIOs(HeatRange** range);
    inline void GetSeqAccumRT(HeatRange** range);
    inline void GetSeqAccumBW(HeatRange** range);
    inline void GetRndSeqAccumIOs(HeatRange** range);
    inline void GetRndSeqAccumBW(HeatRange** range);
    inline void GetRndSeqAccumRT(HeatRange** range);
    inline void GetHeatRangeType(IOStat::Type* type);

    inline void SetRndReadCnt(HeatRange& range);
    inline void SetRndWriteCnt(HeatRange& range);
    inline void SetRndAccumIOs(HeatRange& range);
    inline void SetRndAccumRT(HeatRange& range);
    inline void SetRndAccumBW(HeatRange& range);
    inline void SetSeqReadCnt(HeatRange& range);
    inline void SetSeqWriteCnt(HeatRange& range);
    inline void SetSeqAccumIOs(HeatRange& range);
    inline void SetSeqAccumRT(HeatRange& range);
    inline void SetSeqAccumBW(HeatRange& range);
    inline void SetRndSeqAccumIOs(HeatRange& range);
    inline void SetRndSeqAccumBW(HeatRange& range);
    inline void SetRndSeqAccumRT(HeatRange& range);
    inline void SetHeatRangeType(IOStat::Type type);

    void Dump(ostringstream* ostr);

private:
    HeatRange mRndReadCnt;
    HeatRange mRndWriteCnt;
    HeatRange mRndAccumIOs;
    HeatRange mRndAccumRT;
    HeatRange mRndAccumBW;

    HeatRange mSeqReadCnt;
    HeatRange mSeqWriteCnt;
    HeatRange mSeqAccumIOs;
    HeatRange mSeqAccumRT;
    HeatRange mSeqAccumBW;

    HeatRange mRndSeqAccumIOs;
    HeatRange mRndSeqAccumBW;
    HeatRange mRndSeqAccumRT;

    /*one heat range for one IOStat::Type*/
    IOStat::Type mHeatRangeType;
};

class HeatDistribution;
class SubPoolHeatDistribution;
class HeatDistributionManager;
/*heat distribution on bucket*/
class HeatBucket {
public:
    HeatBucket(HeatDistribution* heatdistribution);
    HeatBucket(ywb_uint32_t partitionbase, ywb_uint32_t sortbase,
            HeatDistribution* heatdistribution);
    ~HeatBucket();

    inline ywb_uint32_t GetPartitionBase();
    inline ywb_uint32_t GetSortBase();
    inline ywb_uint32_t GetSortedAboveBoundary();
    inline ywb_uint32_t GetSortedBelowBoundary();
    inline ywb_uint32_t GetBucketBoundary();
    inline ywb_uint32_t GetOBJsAboveBoundary();
    inline ywb_uint32_t GetOBJsBelowBoundary();
    inline ywb_uint32_t GetLastSortedBucketAbove();
    inline ywb_uint32_t GetLastSortedBucketBelow();
    inline ywb_uint32_t GetCurHotBucket();
    inline ywb_uint32_t GetCurColdBucket();
    inline void GetHeatDistribution(HeatDistribution** heatdistribution);

    inline void SetPartitionBase(ywb_uint32_t partitionBase);
    inline void SetSortBase(ywb_uint32_t sortBase);
    inline void SetSortedAboveBoundary(ywb_uint32_t sortedAboveBoundary);
    inline void SetSortedBelowBoundary(ywb_uint32_t sortedBelowBoundary);
    inline void SetBucketBoundary(ywb_uint32_t bucketBoundary);
    inline void SetOBJsAboveBoundary(ywb_uint32_t aboveboundary);
    inline void SetOBJsBelowBoundary(ywb_uint32_t belowboundary);
    inline void SetLastSortedBucketAbove(ywb_uint32_t lastabove);
    inline void SetLastSortedBucketBelow(ywb_uint32_t lastbelow);
    inline void SetCurHotBucket(ywb_uint32_t hotbucket);
    inline void SetCurColdBucket(ywb_uint32_t coldbucket);

//    /*register compare functions*/
//    ywb_status_t RegisterGreater(ywb_uint32_t partitionbase, HeatCmp::Greater greater);
//    ywb_status_t RegisterLess(ywb_uint32_t partitionbase, HeatCmp::Less less);
//    /*get compare functions*/
//    ywb_status_t GetGreater(ywb_uint32_t partitionbase, HeatCmp::Greater* greater);
//    ywb_status_t GetLess(ywb_uint32_t partitionbase, HeatCmp::Less* less);
//
//    /*do initialization*/
//    ywb_status_t Initialize();

    /*add OBJ into bucket*/
    ywb_status_t AddOBJ(ywb_uint32_t bucketindex, OBJVal*& obj);

    /*dynamically determine the bucket boundary*/
    void DeterminBucketBoundary();
    /*get total number of OBJs above the boundary*/
    ywb_uint32_t CalculateOBJsAboveBoundary();
    /*get total number of OBJs below the boundary*/
    ywb_uint32_t CalculateOBJsBelowBoundary();

    ywb_status_t GetGreater(ywb_uint32_t sortbase,
            HeatCmp::Greater* greater);
    ywb_status_t GetLess(ywb_uint32_t sortbase,
            HeatCmp::Less* less);

    /*get @maxN heat in descending order*/
    ywb_status_t SortAbove(ywb_uint32_t maxN);
    /*get @minN heat in ascending order*/
    ywb_status_t SortBelow(ywb_uint32_t minN);

    /*
     * require more sorted OBJs above boundary,
     * should be compatible with SortAbove
     **/
    ywb_status_t ExtendSortAboveBoundary(ywb_uint32_t required);
    /*
     * require more sorted OBJs below boundary,
     * should be compatible with SortBelow
     **/
    ywb_status_t ExtendSortBelowBoundary(ywb_uint32_t required);

    /*get first hot OBJ from given bucket index on*/
    ywb_status_t GetFirstHotOBJFromBucketIndexOn(
            ywb_uint32_t bucketindex, OBJVal** obj);
    /*get first hottest OBJ, should only be called after sorting*/
    ywb_status_t GetFirstHotOBJ(OBJVal **obj);
    /*get next hottest OBJ, should only be called after sorting*/
    ywb_status_t GetNextHotOBJ(OBJVal **obj);

    /*get first cold OBJ from given bucket index on*/
    ywb_status_t GetFirstColdOBJFromBucketIndexOn(
            ywb_uint32_t bucketindex, OBJVal** obj);
    /*get first coldest OBJ, should only be called after sorting*/
    ywb_status_t GetFirstColdOBJ(OBJVal **obj);
    /*get next coldest OBJ, should only be called after sorting*/
    ywb_status_t GetNextColdOBJ(OBJVal **obj);

    /*
     * dump all OBJs satisfying given rule into @OBJSet,
     * should only be called after sorting
     **/
    ywb_status_t GetAllSatisfyingOBJs(AdviceRule& rule, set<OBJVal*>* OBJSet);

//    /*destroy bucket indexed by @bucketIndex*/
//    ywb_status_t DestroyBucket(ywb_uint32_t bucketindex);
    /*destroy all the buckets*/
    ywb_status_t Destroy();

private:
    /*a bucket contain lots of OBJs*/
    vector<OBJVal*> mBuckets[Constant::BUCKET_NUM];
//    /*iterator used for traversing*/
//    vector<OBJVal*>::iterator mIt[Constant::BUCKET_NUM];
    /*current iterator for hot OBJ*/
    vector<OBJVal*>::iterator mCurHotIt;
    /*current iterator for cold OBJ*/
    vector<OBJVal*>::iterator mCurColdIt;
//    /*compare functions*/
//    map<ywb_uint32_t, HeatCmp::Greater> mGreaters;
//    /*compare functions*/
//    map<ywb_uint32_t, HeatCmp::Less> mLesses;
    /*partition base*/
    ywb_uint32_t mPartitionBase;
    /*sorting base*/
    ywb_uint32_t mSortBase;
    /*how many elements above the boundary are participated in sorting*/
    ywb_uint32_t mSortedAboveBoundary;
    /*how many elements below the boundary are participated in sorting*/
    ywb_uint32_t mSortedBelowBoundary;
    /*boundary of hot cold bucket, this boundary can be dynamical?*/
    ywb_uint32_t mBucketBoundary;
    /*number of OBJs above boundary*/
    ywb_uint32_t mOBJsAboveBoundary;
    /*number of OBJs below boundary*/
    ywb_uint32_t mOBJsBelowBoundary;
    /*last sorted bucket above boundary*/
    ywb_uint32_t mLastSortedBucketAbove;
    /*last sorted bucket below boundary*/
    ywb_uint32_t mLastSortedBucketBelow;
    /*current hot bucket index*/
    ywb_uint32_t mCurHotBucket;
    /*current cold bucket index*/
    ywb_uint32_t mCurColdBucket;

    /*reference to heat distribution*/
    HeatDistribution* mHeatDistribution;
    /*back reference to heat distribution manager*/
    HeatDistributionManager* mHeatManager;
};

/*base class for both DiskHeatDistribution and TierWiseHeatDistribution*/
class HeatDistribution {
public:
    HeatDistribution(SubPoolHeatDistribution* subpoolheat) :
        mSubPoolHeat(subpoolheat) {}

    virtual ~HeatDistribution()
    {
        Destroy();
    }

    inline void GetSubPoolHeat(SubPoolHeatDistribution** subpoolheat)
    {
        *subpoolheat = mSubPoolHeat;
    }

    /*get first disk of this kind of heat distribution*/
    virtual ywb_status_t GetFirstDisk(Disk **disk);
    /*get next disk of this kind of heat distribution*/
    virtual ywb_status_t GetNextDisk(Disk **disk);
    /*get first obj of this kind of heat distribution*/
    virtual ywb_status_t GetFirstOBJ(OBJVal **obj);
    /*get next obj of this kind of heat distribution*/
    virtual ywb_status_t GetNextOBJ(OBJVal **obj);

    /*check whether to proceed at the very begining of Setup*/
    virtual ywb_status_t PreSetupCheck();
    virtual void PutDisk(Disk* disk);
    virtual void PutOBJ(OBJVal* obj);

    /*calculate disk heat summary*/
    ywb_status_t SetupHeatRangeSummary(IOStat::Type type);
    /*get given OBJ's bucket index*/
    ywb_uint32_t GetOBJBucketIndex(ywb_uint32_t partitionbase,
            HeatRangeSummary*& summary, OBJVal*& obj);
    /*
     * this method will deal with the situation that all OBJs
     * have the approximately same IO activity specially
     * */
    ywb_uint32_t GetOBJBucketIndex(ywb_uint32_t advicetype,
            ywb_uint32_t partitionbase,
            HeatRangeSummary*& summary, OBJVal*& obj);

    /*add heat bucket*/
    ywb_status_t AddHeatBucket(HeatBucket* bucket);
    /*partition*/
    ywb_status_t Partition(AdviceRule& rule,
            HeatRangeSummary*& summary, HeatBucket*& bucket);
    /*sorting utilities*/
    ywb_status_t Sort(AdviceRule& rule, HeatBucket*& bucket,
            ywb_uint32_t sortnumaboveboundary,
            ywb_uint32_t sortnumbelowboundary);
    /*setup heat distribution according to given rule*/
    ywb_status_t Setup(AdviceRule& rule,
            ywb_uint32_t sortnumaboveboundary,
            ywb_uint32_t sortnumbelowboundary);

    ywb_status_t GetHeatRangeTypeFromAdviceRule(
            AdviceRule& rule, IOStat::Type* type);
    ywb_status_t GetHeatRangeSummary(IOStat::Type type,
            HeatRangeSummary** summary);
    ywb_status_t GetHeatBucket(AdviceRule& rule,
            HeatBucket** heatdistribution);

    /*get first hottest OBJ of specified rule*/
    ywb_status_t GetFirstHotOBJ(AdviceRule& rule, OBJVal** obj);
    /*get next hottest OBJ of specified rule*/
    ywb_status_t GetNextHotOBJ(AdviceRule& rule, OBJVal** obj);
    /*get first coldest OBJ of specified rule*/
    ywb_status_t GetFirstColdOBJ(AdviceRule& rule, OBJVal** obj);
    /*get next coldest OBJ of specified rule*/
    ywb_status_t GetNextColdOBJ(AdviceRule& rule, OBJVal** obj);

    /*get all OBJs satisfying the given rule*/
    ywb_status_t GetAllSatisfyingOBJs(AdviceRule& rule, set<OBJVal*>* OBJSet);

    /*destroy tier-wise bucket regarding heat base @base*/
    ywb_status_t Destroy(AdviceRule& rule);
    /*destroy tier-wise bucket*/
    ywb_status_t Destroy();

private:
    /*all kinds of heat summary of this disk, used for bucket devision*/
    list<HeatRangeSummary*> mHeatSummary;
    /*disk-wise heat distribution on different base*/
    list<HeatBucket*> mHeatBucket;
    /*back reference to subpool heat distribution*/
    SubPoolHeatDistribution* mSubPoolHeat;
};

/*disk-wise heat distribution*/
class DiskHeatDistribution : public HeatDistribution{
public:
    DiskHeatDistribution(Disk*& disk,
            SubPoolHeatDistribution* subpoolheat) :
                HeatDistribution(subpoolheat), mDisk(disk)
    {
        YWB_ASSERT(mDisk != NULL);
        mDisk->IncRef();
    }

    ~DiskHeatDistribution()
    {
        if(mDisk)
        {
            mDisk->DecRef();
            mDisk = NULL;
        }
    }

    inline void GetDisk(Disk** disk);

    ywb_status_t GetFirstDisk(Disk **disk);
    ywb_status_t GetNextDisk(Disk **disk);
    ywb_status_t GetFirstOBJ(OBJVal **obj);
    ywb_status_t GetNextOBJ(OBJVal **obj);
    ywb_status_t PreSetupCheck();
    void PutDisk(Disk* disk);
    void PutOBJ(OBJVal* obj);

private:
    /*back reference to disk*/
    Disk* mDisk;
};

/*intra-tier disk heat distribution for Warm Demote and ITR*/
class TierInternalDiskWiseHeatDistribution {
public:
    TierInternalDiskWiseHeatDistribution(Tier* tier,
            SubPoolHeatDistribution* subpoolheat) :
                mTier(tier), mSubPoolHeat(subpoolheat)
    {
        YWB_ASSERT(mTier != NULL);
        mTier->IncRef();
    }

    ~TierInternalDiskWiseHeatDistribution()
    {
        Destroy();
    }

    /*setup heat distribution of specified disk*/
    ywb_status_t SetupDiskHeatDistribution(DiskKey& key,
            AdviceRule& rule, ywb_uint32_t sortnumaboveboundary,
            ywb_uint32_t sortnumbelowboundary);
    /*setup heat distribution of current tier*/
    ywb_status_t SetupDiskHeatDistribution(AdviceRule& rule,
            ywb_uint32_t sortnumaboveboundary,
            ywb_uint32_t sortnumbelowboundary);
    /*get disk wise heat distribution of specified disk*/
    ywb_status_t GetDiskHeatDistribution(DiskKey& key,
            DiskHeatDistribution** heatdistribution);

    /*
     * calculate disk performance skew and find out those most
     * utilized disks and least utilized disks, among which
     * migration should be triggered
     **/
    ywb_status_t CalculatePerformanceSkew();

    /*
     * get tier internal disk wise first/next hot/cold OBJ, disk is
     * chosen at first and then OBJ
     **/
    ywb_status_t GetFirstHotOBJ(AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetNextHotOBJ(AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetFirstColdOBJ(AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetNextColdOBJ(AdviceRule& rule, OBJVal** obj);

    /*get tier internal disk wise first/next hot/cold OBJ in given disk*/
    ywb_status_t GetFirstHotOBJ(DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetNextHotOBJ(DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetFirstColdOBJ(DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetNextColdOBJ(DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);

    /*destroy heat distribution of specified disk*/
    ywb_status_t Destroy(DiskKey& diskkey, AdviceRule& rule);
    /*destroy heat distribution of specified disk*/
    ywb_status_t Destroy(DiskKey& diskkey);
    /*destroy heat distribution of specified advice rule*/
    ywb_status_t Destroy(AdviceRule& rule);
    /*destroy tier wise disks's heat distribution*/
    ywb_status_t Destroy();

private:
    /*map of disks' heat distribution in this tier*/
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp> mDiskHeat;
//    /*most utilized disks*/
//    list<DiskKey> mMostUtilizedDisks;
//    /*least utilized disks*/
//    list<DiskKey> mLeastUtilizedDisks;
    map<AdviceRule, list<DiskKey>*, AdviceRuleCmp> mMostUtilizedDisks;
    map<AdviceRule, list<DiskKey>*, AdviceRuleCmp> mLeastUtilizedDisks;

    /*back reference to tier*/
    Tier* mTier;
    /*back reference to subpool heat distribution*/
    SubPoolHeatDistribution* mSubPoolHeat;
};

/*tier-wise heat distribution*/
class TierWiseHeatDistribution : public HeatDistribution{
public:
    TierWiseHeatDistribution(Tier*& tier,
            SubPoolHeatDistribution* subpoolheat) :
                HeatDistribution(subpoolheat), mTier(tier)
    {
        YWB_ASSERT(mTier != NULL);
        mTier->IncRef();
    }

    ~TierWiseHeatDistribution()
    {
        if(mTier)
        {
            mTier->DecRef();
            mTier = NULL;
        }
    }

    inline void GetTier(Tier** tier);

    ywb_status_t GetFirstDisk(Disk **disk);
    ywb_status_t GetNextDisk(Disk **disk);
    ywb_status_t GetFirstOBJ(OBJVal **obj);
    ywb_status_t GetNextOBJ(OBJVal **obj);
    ywb_status_t PreSetupCheck();
    void PutDisk(Disk* disk);
    void PutOBJ(OBJVal* obj);

private:
    /*back reference to tier*/
    Tier* mTier;
};

class SubPoolHeatDistribution {
public:
    SubPoolHeatDistribution(SubPool* subpool,
            HeatDistributionManager* heatmanager) :
                mSubPool(subpool), mHeatManager(heatmanager)
    {
        ywb_uint32_t tier = 0;

        for(; tier < Constant::TIER_NUM; tier++)
        {
            mTierDiskHeat[tier] = NULL;
            mTierHeat[tier] = NULL;
        }

        YWB_ASSERT(subpool != NULL);
        mSubPool->IncRef();
    }

    ~SubPoolHeatDistribution()
    {
        Destroy();
    }

    inline void GetSubPool(SubPool** subpool);
    inline void GetHeatDistributuinManager(
            HeatDistributionManager** heatmanager);

    /*
     * setup pool tier level heat distribution, all the OBJs will participate
     * in bucket partition, but not necessary in sorting, for limitation
     * of resource, not all OBJs will be migrated.
     * @tier: the specified tier of sub-pool
     * @sortNumAboveBoundary: at most how many OBJs above the boundary will
     *                        participate in the sorting
     * @sortNumBelowBoundary: at most how many OBJs above the boundary will
     *                        participate in the sorting
     **/
    ywb_status_t SetupTierWiseHeatDistribution(ywb_uint32_t tier,
            AdviceRule& rule, ywb_uint32_t sortnumaboveboundary,
            ywb_uint32_t sortnumbelowboundary);
    ywb_status_t SetupTierInternalDiskWiseHeatDistribution(
            ywb_uint32_t tier, AdviceRule& rule,
            ywb_uint32_t sortnumaboveboundary,
            ywb_uint32_t sortnumbelowboundary);
    ywb_status_t SetupTierInternalDiskWiseHeatDistribution(
            ywb_uint32_t tier, DiskKey& diskkey, AdviceRule& rule,
            ywb_uint32_t sortnumaboveboundary, ywb_uint32_t sortnumbelowboundary);
    ywb_status_t SetupDiskWiseHeatDistribution(DiskKey& diskkey, AdviceRule& rule,
            ywb_uint32_t sortnumaboveboundary, ywb_uint32_t sortnumbelowboundary);

    /*get tier internal disk wise heat distribution*/
    ywb_status_t GetTierInternalDiskWiseHeatDistribution(ywb_uint32_t tier,
            TierInternalDiskWiseHeatDistribution** heatdistribution);
    /*get tier wise heat distribution*/
    ywb_status_t GetTierWiseHeatDistribution(ywb_uint32_t tier,
            TierWiseHeatDistribution** heatdistribution);
    ywb_status_t GetDiskWiseHeatDistribution(DiskKey& diskkey,
            DiskHeatDistribution** diskheatdistribution);

    /*get tier wise first/next hot/cod OBJ*/
    ywb_status_t GetTierWiseFirstHotOBJ(
            ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierWiseNextHotOBJ(
            ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierWiseFirstColdOBJ(
            ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierWiseNextColdOBJ(
            ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj);

    /*get tier internal disk wise first/next hot/cod OBJ*/
    ywb_status_t GetTierInternalDiskWiseFirstHotOBJ(
            ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierInternalDiskWiseNextHotOBJ(
            ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierInternalDiskWiseFirstColdOBJ(
            ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierInternalDiskWiseNextColdOBJ(
            ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj);

    /*get tier internal disk wise first/next hot/cod OBJ*/
    ywb_status_t GetTierInternalDiskWiseFirstHotOBJ(ywb_uint32_t tier,
            DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierInternalDiskWiseNextHotOBJ(ywb_uint32_t tier,
            DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierInternalDiskWiseFirstColdOBJ(ywb_uint32_t tier,
            DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierInternalDiskWiseNextColdOBJ(ywb_uint32_t tier,
            DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);

    /*get disk wise first/next hot/cod OBJ*/
    ywb_status_t GetTierInternalDiskWiseFirstHotOBJ(
            DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierInternalDiskWiseNextHotOBJ(
            DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierInternalDiskWiseFirstColdOBJ(
            DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);
    ywb_status_t GetTierInternalDiskWiseNextColdOBJ(
            DiskKey& diskkey, AdviceRule& rule, OBJVal** obj);

    ywb_status_t DestroyTierWiseHeatDistribution(
            ywb_uint32_t tier, AdviceRule& rule);
    ywb_status_t DestroyTierWiseHeatDistribution(ywb_uint32_t tier);

    ywb_status_t DestroyTierInternalDiskWiseHeatDistribution(
            ywb_uint32_t tier, AdviceRule& rule);
    ywb_status_t DestroyTierInternalDiskWiseHeatDistribution(ywb_uint32_t tier);

    ywb_status_t DestroyTierInternalDiskWiseHeatDistribution(
            ywb_uint32_t tier, DiskKey& diskkey, AdviceRule& rule);
    ywb_status_t DestroyTierInternalDiskWiseHeatDistribution(
            ywb_uint32_t tier, DiskKey& diskkey);

    ywb_status_t DestroyDiskWiseHeatDistribution(
            DiskKey& diskkey, AdviceRule& rule);
    ywb_status_t DestroyDiskWiseHeatDistribution(DiskKey& diskkey);

    ywb_status_t Destroy();

private:
    /*sub-pool level intra-tier disk heat distribution*/
    TierInternalDiskWiseHeatDistribution* mTierDiskHeat[Constant::TIER_NUM];
    /*sub-pool level tier heat distribution*/
    TierWiseHeatDistribution* mTierHeat[Constant::TIER_NUM];

    /*back reference to subpool*/
    SubPool* mSubPool;
    /*back reference to heat distribution manager*/
    HeatDistributionManager* mHeatManager;
};

/*heat distribution is driven by advice generating process, it will
 *not partition and sorting by itself
 **/
class HeatDistributionManager {
public:
    HeatDistributionManager(AST* ast)
    {
//        mConfig = NULL;
        mAst = ast;
        Initialize();
    }

    ~HeatDistributionManager()
    {
        Destroy();
    }

    inline void GetAST(AST** ast);
//    inline void GetLogicalConfig(LogicalConfig** config);
//    inline void SetLogicalConfig(LogicalConfig* config);

    /*register compare functions*/
    ywb_status_t RegisterGreater(
            ywb_uint32_t sortbase, HeatCmp::Greater greater);
    ywb_status_t RegisterLess(
            ywb_uint32_t sortbase, HeatCmp::Less less);
    /*get compare functions*/
    ywb_status_t GetGreater(
            ywb_uint32_t sortbase, HeatCmp::Greater* greater);
    ywb_status_t GetLess(ywb_uint32_t sortbase, HeatCmp::Less* less);
    /*do initialization*/
    ywb_status_t Initialize();

    /*
     * setup pool tier level heat distribution, all the OBJs will participate
     * in bucket partition, but not necessary for sorting, because limitation
     * of resource, not all OBJs will be migrated.
     * @key: identifier of sub-pool
     * @tier: the specified tier of sub-pool
     * @sortNumAboveBoundary: at most how many OBJs above the boundary will
     *                        participate in the sorting
     * @sortNumBelowBoundary: at most how many OBJs below the boundary will
     *                        participate in the sorting
     **/
    ywb_status_t SetupSubPoolHeatDistribution(SubPoolKey& subpoolkey,
            ywb_uint32_t tier, AdviceRule& rule,
            ywb_uint32_t sortnumaboveboundary,
            ywb_uint32_t sortnumbelowboundary);
    ywb_status_t SetupSubPoolHeatDistribution(SubPoolKey& subpoolkey,
            ywb_uint32_t tier, DiskKey& diskkey, AdviceRule& rule,
            ywb_uint32_t sortnumaboveboundary, ywb_uint32_t sortnumbelowboundary);
    ywb_status_t SetupSubPoolHeatDistribution(SubPoolKey& subpoolkey,
            DiskKey& diskkey, AdviceRule& rule,
            ywb_uint32_t sortnumaboveboundary,
            ywb_uint32_t sortnumbelowboundary);

    ywb_status_t GetSubPoolHeatDistribution(SubPoolKey& key,
            SubPoolHeatDistribution** subpoolheat);

    /*destroy specified subpool's subpool wise heat distribution*/
    ywb_status_t DestroySubPoolHeatDistribution(SubPoolKey& key);
    /*destroy heat distribution manager*/
    ywb_status_t Destroy();

    ywb_status_t Reset();
    ywb_status_t Init();

private:
    /*compare functions*/
    map<ywb_uint32_t, HeatCmp::Greater> mGreaters;
    /*compare functions*/
    map<ywb_uint32_t, HeatCmp::Less> mLesses;

    /*OSS-wise sub-pool heat distribution*/
    map<SubPoolKey, SubPoolHeatDistribution*, SubPoolKeyCmp> mSubPools;
//    /*YfsAST::LogicalConfig is "global" one, and here is just a "local" copy*/
//    LogicalConfig* mConfig;
    /*back reference to AST*/
    AST* mAst;
};

#include "AST/ASTHeatDistributionInline.hpp"

#endif

/* vim:set ts=4 sw=4 expandtab */
