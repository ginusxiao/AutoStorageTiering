#ifndef __AST_PERFORMANCE_INLINE_HPP
#define __AST_PERFORMANCE_INLINE_HPP

#include "AST/ASTPerformance.hpp"

ywb_uint32_t DiskPerf::GetCyclePerid()
{
    return mCyclePeriod;
}

ywb_uint32_t DiskPerf::GetBWThreshold(){
    return mBWThreshold;
}

ywb_uint32_t DiskPerf::GetBWConsumed(){
    return mBWConsumed;
}

ywb_uint32_t DiskPerf::GetIOThreshold(){
    return mIOThreshold;
}

ywb_uint32_t DiskPerf::GetIOConsumed(){
    return mIOConsumed;
}

ywb_int32_t DiskPerf::GetCapTotal(){
    return mCapTotal;
}

ywb_int32_t DiskPerf::GetCapConsumed(){
    return mCapConsumed;
}

void DiskPerf::GetDiskProfile(DiskPerfProfile** profile)
{
    *profile =  mDiskPerfProfile;
}

void DiskPerf::GetDiskIO(DiskIO** diskio)
{
    *diskio = mDiskIO;
}

void DiskPerf::GetDisk(Disk** disk)
{
    *disk = mDisk;
}

void DiskPerf::SetCyclePeriod(ywb_uint32_t val)
{
    mCyclePeriod = val;
}

void DiskPerf::SetIOThreshold(ywb_uint32_t val) {
    mIOThreshold = val;
}

void DiskPerf::SetIOConsumed(ywb_uint32_t val) {
    mIOConsumed = val;
}

void DiskPerf::SetBWThreshold(ywb_uint32_t val) {
    mBWThreshold = val;
}

void DiskPerf::SetBWConsumed(ywb_uint32_t val) {
    mBWConsumed = val;
}

void DiskPerf::SetCapTotal(ywb_int32_t val) {
    mCapTotal = val;
}

void DiskPerf::SetCapConsumed(ywb_int32_t val) {
    mCapConsumed = val;
}

void
DiskAvailPerfRankingUnit::GetDiskKey(DiskKey** key)
{
    *key = &mKey;
}

ywb_uint32_t
DiskAvailPerfRankingUnit::GetIOAvail()
{
    return mIOAvail;
}

ywb_uint32_t
DiskAvailPerfRankingUnit::GetIOReserved()
{
    return mIOReserved;
}

ywb_uint32_t
DiskAvailPerfRankingUnit::GetBWAvail()
{
    return mBWAvail;
}

ywb_uint32_t
DiskAvailPerfRankingUnit::GetBWReserved()
{
    return mBWReserved;
}

ywb_int32_t
DiskAvailPerfRankingUnit::GetCapAvail()
{
    return mCapAvail;
}

ywb_int32_t
DiskAvailPerfRankingUnit::GetCapReserved()
{
    return mCapReserved;
}

void
DiskAvailPerfRankingUnit::GetDiskPerf(DiskPerf** diskperf)
{
    *diskperf = mDiskPerf;
}

void
DiskAvailPerfRankingUnit::GetTierPerf(TierDiskAvailPerfRanking** tierperf)
{
    *tierperf = mTierPerf;
}

void
TierDiskAvailPerfRanking::GetAST(AST** ast)
{
    PerfManager* perfmgr = NULL;

    mSubPoolPerf->GetPerfManager(&perfmgr);
    perfmgr->GetAST(ast);
}

void
TierDiskAvailPerfRanking::GetSubPoolDiskAvailPerfRanking(
            SubPoolDiskAvailPerfRanking** subpoolperf)
{
    *subpoolperf = mSubPoolPerf;
}

void
SubPoolDiskAvailPerfRanking::GetPerfManager(PerfManager** mgr)
{
    *mgr = mPerfMgr;
}

void PerfManager::GetAST(AST** ast)
{
    *ast = mAst;
}

#endif

/* vim:set ts=4 sw=4 expandtab */
