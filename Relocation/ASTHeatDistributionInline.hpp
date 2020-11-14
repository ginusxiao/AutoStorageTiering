#ifndef __AST_HEAT_DISTRIBUTION_INLINE_HPP
#define __AST_HEAT_DISTRIBUTION_INLINE_HPP

#include "AST/ASTHeatDistribution.hpp"

ywb_uint32_t
HeatRange::GetMin()
{
    return mMin;
}

ywb_uint32_t
HeatRange::GetMax()
{
    return mMax;
}

ywb_uint32_t
HeatRange::GetBucketCap()
{
    return mBucketCap;
}

ywb_uint32_t
HeatRange::GetMinPortionBoundary()
{
    return mMinPortionBoundary;
}

ywb_uint32_t
HeatRange::GetMaxPortionBoundary()
{
    return mMaxPortionBoundary;
}

ywb_uint32_t
HeatRange::GetAvg()
{
    return mAvg;
}

ywb_uint64_t
HeatRange::GetSum()
{
    return mSum;
}

void
HeatRange::SetMin(ywb_uint32_t val)
{
    mMin = val;
}

void
HeatRange::SetMax(ywb_uint32_t val)
{
    mMax = val;
}

void
HeatRange::SetBucketCap(ywb_uint32_t val)
{
    mBucketCap = val;
}

void
HeatRange::SetMinPortionBoundary(ywb_uint32_t val)
{
    mMinPortionBoundary = val;
}

void
HeatRange::SetMaxPortionBoundary(ywb_uint32_t val)
{
    mMaxPortionBoundary = val;
}

void
HeatRange::SetAvg(ywb_uint32_t val)
{
    mAvg = val;
}

void
HeatRange::SetSum(ywb_uint64_t val)
{
    mSum = val;
}

void
HeatRange::UpdateSum(ywb_uint64_t val, ywb_bool_t add)
{
    if(ywb_true == add)
    {
        mSum += val;
    }
    else
    {
        mSum = (mSum > val ? (mSum - val) : 0);
    }
}

void
HeatRangeSummary::GetRndReadCnt(HeatRange** range)
{
    *range = &mRndReadCnt;
}

void
HeatRangeSummary::GetRndWriteCnt(HeatRange** range)
{
    *range = &mRndWriteCnt;
}

void HeatRangeSummary::GetRndAccumIOs(HeatRange** range)
{
    *range = &mRndAccumIOs;
}

void
HeatRangeSummary::GetRndAccumRT(HeatRange** range)
{
    *range = &mRndAccumRT;
}

void
HeatRangeSummary::GetRndAccumBW(HeatRange** range)
{
    *range = &mRndAccumBW;
}

void
HeatRangeSummary::GetSeqReadCnt(HeatRange** range)
{
    *range = &mSeqReadCnt;
}

void
HeatRangeSummary::GetSeqWriteCnt(HeatRange** range)
{
    *range = &mSeqWriteCnt;
}

void
HeatRangeSummary::GetSeqAccumIOs(HeatRange** range)
{
    *range = &mSeqAccumIOs;
}

void
HeatRangeSummary::GetSeqAccumRT(HeatRange** range)
{
    *range = &mSeqAccumRT;
}

void
HeatRangeSummary::GetSeqAccumBW(HeatRange** range)
{
    *range = &mSeqAccumBW;
}

void
HeatRangeSummary::GetRndSeqAccumIOs(HeatRange** range)
{
    *range = &mRndSeqAccumIOs;
}

void
HeatRangeSummary::GetRndSeqAccumBW(HeatRange** range)
{
    *range = &mRndSeqAccumBW;
}

void
HeatRangeSummary::GetRndSeqAccumRT(HeatRange** range)
{
    *range = &mRndSeqAccumRT;
}

void
HeatRangeSummary::GetHeatRangeType(IOStat::Type* type)
{
    *type = mHeatRangeType;
}

void
HeatRangeSummary::SetRndReadCnt(HeatRange& range)
{
    mRndReadCnt = range;
}

void
HeatRangeSummary::SetRndWriteCnt(HeatRange& range)
{
    mRndWriteCnt = range;
}

void
HeatRangeSummary::SetRndAccumIOs(HeatRange& range)
{
    mRndAccumIOs = range;
}

void
HeatRangeSummary::SetRndAccumRT(HeatRange& range)
{
    mRndAccumRT = range;
}

void
HeatRangeSummary::SetRndAccumBW(HeatRange& range)
{
    mRndAccumBW = range;
}

void
HeatRangeSummary::SetSeqReadCnt(HeatRange& range)
{
    mSeqReadCnt = range;
}

void
HeatRangeSummary::SetSeqWriteCnt(HeatRange& range)
{
    mSeqWriteCnt = range;
}

void
HeatRangeSummary::SetSeqAccumIOs(HeatRange& range)
{
    mSeqAccumIOs = range;
}

void
HeatRangeSummary::SetSeqAccumRT(HeatRange& range)
{
    mSeqAccumRT = range;
}

void
HeatRangeSummary::SetSeqAccumBW(HeatRange& range)
{
    mSeqAccumBW = range;
}

void
HeatRangeSummary::SetRndSeqAccumIOs(HeatRange& range)
{
    mRndSeqAccumIOs = range;
}

void
HeatRangeSummary::SetRndSeqAccumBW(HeatRange& range)
{
    mRndSeqAccumBW = range;
}

void
HeatRangeSummary::SetRndSeqAccumRT(HeatRange& range)
{
    mRndSeqAccumRT = range;
}

void
HeatRangeSummary::SetHeatRangeType(IOStat::Type type)
{
    mHeatRangeType = type;
}

ywb_uint32_t
HeatBucket::GetPartitionBase(){
    return mPartitionBase;
}

ywb_uint32_t
HeatBucket::GetSortBase(){
    return mSortBase;
}

ywb_uint32_t
HeatBucket::GetSortedAboveBoundary(){
    return mSortedAboveBoundary;
}

ywb_uint32_t
HeatBucket::GetSortedBelowBoundary(){
    return mSortedBelowBoundary;
}

ywb_uint32_t
HeatBucket::GetBucketBoundary(){
    return mBucketBoundary;
}

ywb_uint32_t
HeatBucket::GetOBJsAboveBoundary()
{
    return mOBJsAboveBoundary;
}

ywb_uint32_t
HeatBucket::GetOBJsBelowBoundary()
{
    return mOBJsBelowBoundary;
}

ywb_uint32_t
HeatBucket::GetLastSortedBucketAbove()
{
    return mLastSortedBucketAbove;
}

ywb_uint32_t
HeatBucket::GetLastSortedBucketBelow()
{
    return mLastSortedBucketBelow;
}

ywb_uint32_t
HeatBucket::GetCurHotBucket()
{
    return mCurHotBucket;
}

ywb_uint32_t
HeatBucket::GetCurColdBucket()
{
    return mCurColdBucket;
}

void
HeatBucket::GetHeatDistribution(HeatDistribution** heatdistribution)
{
    *heatdistribution = mHeatDistribution;
}

void
HeatBucket::SetPartitionBase(ywb_uint32_t partitionBase) {
    mPartitionBase = partitionBase;
}

void
HeatBucket::SetSortBase(ywb_uint32_t sortBase) {
    mSortBase = sortBase;
}

void
HeatBucket::SetSortedAboveBoundary(ywb_uint32_t sortedAboveBoundary) {
    ast_log_debug("Set sorted OBJs above boundary to: "
            << sortedAboveBoundary);
    mSortedAboveBoundary = sortedAboveBoundary;
}

void
HeatBucket::SetSortedBelowBoundary(ywb_uint32_t sortedBelowBoundary) {
    ast_log_debug("Set sorted OBJs below boundary to: "
            << sortedBelowBoundary);
    mSortedBelowBoundary = sortedBelowBoundary;
}

void
HeatBucket::SetBucketBoundary(ywb_uint32_t bucketBoundary) {
    mBucketBoundary = bucketBoundary;
}

void
HeatBucket::SetOBJsAboveBoundary(ywb_uint32_t aboveboundary)
{
    ast_log_debug("Set OBJs above boundary to: " << aboveboundary);
    mOBJsAboveBoundary = aboveboundary;
}

void
HeatBucket::SetOBJsBelowBoundary(ywb_uint32_t belowboundary)
{
    ast_log_debug("Set OBJs below boundary to: " << belowboundary);
    mOBJsBelowBoundary = belowboundary;
}

void
HeatBucket::SetLastSortedBucketAbove(ywb_uint32_t lastabove)
{
    ast_log_debug("Set last sorted bucket above boundary to: " << lastabove);
    mLastSortedBucketAbove = lastabove;
}

void
HeatBucket::SetLastSortedBucketBelow(ywb_uint32_t lastbelow)
{
    ast_log_debug("Set last sorted bucket below boundary to: " << lastbelow);
    mLastSortedBucketBelow = lastbelow;
}

void
HeatBucket::SetCurHotBucket(ywb_uint32_t hotbucket)
{
    mCurHotBucket = hotbucket;
}

void
HeatBucket::SetCurColdBucket(ywb_uint32_t coldbucket)
{
    mCurColdBucket = coldbucket;
}

void
DiskHeatDistribution::GetDisk(Disk** disk)
{
    *disk = mDisk;
}

void TierWiseHeatDistribution::GetTier(Tier** tier)
{
    *tier = mTier;
}

void
SubPoolHeatDistribution::GetSubPool(SubPool** subpool)
{
    *subpool = mSubPool;
}

void
SubPoolHeatDistribution::GetHeatDistributuinManager(
        HeatDistributionManager** heatmanager)
{
    *heatmanager = mHeatManager;
}

void HeatDistributionManager::GetAST(AST** ast)
{
    *ast = mAst;
}

//void HeatDistributionManager::GetLogicalConfig(LogicalConfig** config)
//{
//    *config = mConfig;
//}
//
//void HeatDistributionManager::SetLogicalConfig(LogicalConfig* config)
//{
//    mConfig = config;
//}

#endif

/* vim:set ts=4 sw=4 expandtab */
