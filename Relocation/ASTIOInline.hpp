#ifndef __AST_IO_INLINE_HPP__
#define __AST_IO_INLINE_HPP__

#include "AST/ASTIO.hpp"

ywb_uint32_t
ChunkIOStat::GetRndReadIOs()
{
    return mRndReadIOs;
}

ywb_uint32_t
ChunkIOStat::GetRndWriteIOs()
{
    return mRndWriteIOs;
}

ywb_uint32_t
ChunkIOStat::GetRndTotalRT()
{
    return mRndTotalRt;
}

ywb_uint32_t
ChunkIOStat::GetRndReadBW()
{
    return mRndReadBw;
}

ywb_uint32_t
ChunkIOStat::GetRndWriteBW()
{
    return mRndWriteBw;
}

ywb_uint32_t
ChunkIOStat::GetSeqReadIOs()
{
    return mSeqReadIOs;
}

ywb_uint32_t
ChunkIOStat::GetSeqWriteIOs()
{
    return mSeqWriteIOs;
}

ywb_uint32_t
ChunkIOStat::GetSeqTotalRT()
{
    return mSeqTotalRt;
}

ywb_uint32_t
ChunkIOStat::GetSeqReadBW()
{
    return mSeqReadBw;
}

ywb_uint32_t
ChunkIOStat::GetSeqWriteBW()
{
    return mSeqWriteBw;
}

void
ChunkIOStat::SetRndReadIOs(ywb_uint32_t val)
{
    mRndReadIOs = val;
}

void
ChunkIOStat::SetRndWriteIOs(ywb_uint32_t val)
{
    mRndWriteIOs = val;
}

void
ChunkIOStat::SetRndTotalRT(ywb_uint32_t val)
{
    mRndTotalRt = val;
}

void
ChunkIOStat::SetRndReadBW(ywb_uint32_t val)
{
    mRndReadBw = val;
}

void
ChunkIOStat::SetRndWriteBW(ywb_uint32_t val)
{
    mRndWriteBw = val;
}

void
ChunkIOStat::SetSeqReadIOs(ywb_uint32_t val)
{
    mSeqReadIOs = val;
}

void
ChunkIOStat::SetSeqWriteIOs(ywb_uint32_t val)
{
    mSeqWriteIOs = val;
}

void
ChunkIOStat::SetSeqTotalRT(ywb_uint32_t val)
{
    mSeqTotalRt = val;
}

void
ChunkIOStat::SetSeqReadBW(ywb_uint32_t val)
{
    mSeqReadBw = val;
}

void
ChunkIOStat::SetSeqWriteBW(ywb_uint32_t val)
{
    mSeqWriteBw = val;
}

void OBJIO::GetIOStat(ywb_uint32_t type, ChunkIOStat** val)
{
    if(IOStat::IOST_raw == type ||
            IOStat::IOST_short_term_ema == type ||
            IOStat::IOST_long_term_ema == type)
    {
        *val = &mIOStat[type];
    }
    else
    {
        *val = NULL;
    }
}

ywb_uint32_t
OBJIO::GetStatCycles()
{
    return mStatCycles;
}

ywb_uint32_t
OBJIO::GetPeakBwLastWin()
{
    return mPeakBwLastWin;
}

ywb_uint32_t
OBJIO::GetPeakBwCurWin()
{
    return mPeakBwCurWin;
}

void
OBJIO::GetOBJ(OBJ** val)
{
    *val = mOBJ;
}

void
OBJIO::SetRawIO(ChunkIOStat& val)
{
    mIOStat[IOStat::IOST_raw].SetRndReadIOs(val.GetRndReadIOs());
    mIOStat[IOStat::IOST_raw].SetRndReadIOs(val.GetRndReadIOs());
    mIOStat[IOStat::IOST_raw].SetRndWriteIOs(val.GetRndWriteIOs());
    mIOStat[IOStat::IOST_raw].SetRndTotalRT(val.GetRndTotalRT());
    mIOStat[IOStat::IOST_raw].SetRndReadBW(val.GetRndReadBW());
    mIOStat[IOStat::IOST_raw].SetRndWriteBW(val.GetRndWriteBW());
    mIOStat[IOStat::IOST_raw].SetSeqReadIOs(val.GetSeqReadIOs());
    mIOStat[IOStat::IOST_raw].SetSeqWriteIOs(val.GetSeqWriteIOs());
    mIOStat[IOStat::IOST_raw].SetSeqTotalRT(val.GetSeqTotalRT());
    mIOStat[IOStat::IOST_raw].SetSeqReadBW(val.GetSeqReadBW());
    mIOStat[IOStat::IOST_raw].SetSeqWriteBW(val.GetSeqWriteBW());
}

void
OBJIO::SetStatCycles(ywb_uint32_t val)
{
    mStatCycles = val;
}

void
OBJIO::SetPeakBwLastWin(ywb_uint32_t val)
{
    mPeakBwLastWin = val;
}

void
OBJIO::SetPeakBwCurWin(ywb_uint32_t val)
{
    mPeakBwCurWin = val;
}

void
OBJIO::SetOBJ(OBJ*& val)
{
    mOBJ = val;
}

ywb_uint32_t
DiskIOStat::GetRndReadStat()
{
    return mRndReadStat;
}

ywb_uint32_t
DiskIOStat::GetSeqReadStat()
{
    return mSeqReadStat;
}

ywb_uint32_t
DiskIOStat::GetRndWriteStat()
{
    return mRndWriteStat;
}

ywb_uint32_t
DiskIOStat::GetSeqWriteStat()
{
    return mSeqWriteStat;
}

ywb_uint32_t
DiskIOStat::GetReadStat()
{
    return mReadStat;
}

ywb_uint32_t
DiskIOStat::GetWriteStat()
{
    return mWriteStat;
}

ywb_uint32_t
DiskIOStat::GetRndStat()
{
    return mRndStat;
}

ywb_uint32_t
DiskIOStat::GetSeqStat()
{
    return mSeqStat;
}

void
DiskIOStat::SetRndReadStat(ywb_uint32_t val)
{
    mRndReadStat = val;
}

void
DiskIOStat::SetSeqReadStat(ywb_uint32_t val)
{
    mSeqReadStat = val;
}

void
DiskIOStat::SetRndWriteStat(ywb_uint32_t val)
{
    mRndWriteStat = val;
}

void
DiskIOStat::SetSeqWriteStat(ywb_uint32_t val)
{
    mSeqWriteStat = val;
}

void DiskIOStat::SetReadStat(ywb_uint32_t val)
{
    mReadStat = val;
}

void
DiskIOStat::SetWriteStat(ywb_uint32_t val)
{
    mWriteStat = val;
}

void
DiskIOStat::SetRndStat(ywb_uint32_t val)
{
    mRndStat = val;
}

void
DiskIOStat::SetSeqStat(ywb_uint32_t val)
{
    mSeqStat = val;
}

ywb_uint32_t DiskIOStatRange::GetRndMax() {
    return mRndMax;
}

ywb_uint32_t DiskIOStatRange::GetRndMin() {
    return mRndMin;
}

ywb_uint32_t DiskIOStatRange::GetRndReadMax() {
    return mRndReadMax;
}

ywb_uint32_t DiskIOStatRange::GetRndReadMin() {
    return mRndReadMin;
}

ywb_uint32_t DiskIOStatRange::GetRndWriteMax() {
    return mRndWriteMax;
}

ywb_uint32_t DiskIOStatRange::GetRndWriteMin() {
    return mRndWriteMin;
}

ywb_uint32_t DiskIOStatRange::GetSeqMax() {
    return mSeqMax;
}

ywb_uint32_t DiskIOStatRange::GetSeqMin() {
    return mSeqMin;
}

ywb_uint32_t DiskIOStatRange::GetSeqReadMax() {
    return mSeqReadMax;
}

ywb_uint32_t DiskIOStatRange::GetSeqReadMin() {
    return mSeqReadMin;
}

ywb_uint32_t DiskIOStatRange::GetSeqWriteMax() {
    return mSeqWriteMax;
}

ywb_uint32_t DiskIOStatRange::GetSeqWriteMin() {
    return mSeqWriteMin;
}

ywb_uint32_t DiskIOStatRange::GetRndSeqMax()
{
    return mRndSeqMax;
}

ywb_uint32_t DiskIOStatRange::GetRndSeqMin()
{
    return mRndSeqMin;
}

void DiskIOStatRange::SetRndMax(ywb_uint32_t val) {
    mRndMax = val;
}

void DiskIOStatRange::SetRndMin(ywb_uint32_t val) {
    mRndMin = val;
}

void DiskIOStatRange::SetRndReadMax(ywb_uint32_t val) {
    mRndReadMax = val;
}

void DiskIOStatRange::SetRndReadMin(ywb_uint32_t val) {
    mRndReadMin = val;
}

void DiskIOStatRange::SetRndWriteMax(ywb_uint32_t val) {
    mRndWriteMax = val;
}

void DiskIOStatRange::SetRndWriteMin(ywb_uint32_t val) {
    mRndWriteMin = val;
}

void DiskIOStatRange::SetSeqMax(ywb_uint32_t val) {
    mSeqMax = val;
}

void DiskIOStatRange::SetSeqMin(ywb_uint32_t val) {
    mSeqMin = val;
}

void DiskIOStatRange::SetSeqReadMax(ywb_uint32_t val) {
    mSeqReadMax = val;
}

void DiskIOStatRange::SetSeqReadMin(ywb_uint32_t val) {
    mSeqReadMin = val;
}

void DiskIOStatRange::SetSeqWriteMax(ywb_uint32_t val) {
    mSeqWriteMax = val;
}

void DiskIOStatRange::SetSeqWriteMin(ywb_uint32_t val) {
    mSeqWriteMin = val;
}

void DiskIOStatRange::SetRndSeqMax(ywb_uint32_t val)
{
    mRndSeqMax = val;
}

void DiskIOStatRange::SetRndSeqMin(ywb_uint32_t val)
{
    mRndSeqMin = val;
}

void DiskIO::GetStatIOs(IOStat::Type type, DiskIOStat** stat)
{
    YWB_ASSERT(type < Constant::IO_STAT_TYPE_CNT);
    *stat = &mStatIOs[type];
}

void DiskIO::GetStatBW(IOStat::Type type, DiskIOStat** stat)
{
    YWB_ASSERT(type < Constant::IO_STAT_TYPE_CNT);
    *stat = &mStatBW[type];
}

void DiskIO::GetStatRT(IOStat::Type type, DiskIOStat** stat)
{
    YWB_ASSERT(type < Constant::IO_STAT_TYPE_CNT);
    *stat = &mStatRT[type];
}

void DiskIO::GetIOsRange(IOStat::Type type, DiskIOStatRange** range)
{
    YWB_ASSERT(type < Constant::IO_STAT_TYPE_CNT);
    *range = &mIOsRange[type];
}

void DiskIO::GetBWRange(IOStat::Type type, DiskIOStatRange** range)
{
    YWB_ASSERT(type < Constant::IO_STAT_TYPE_CNT);
    *range = &mBWRange[type];
}

void DiskIO::GetRTRange(IOStat::Type type, DiskIOStatRange** range)
{
    YWB_ASSERT(type < Constant::IO_STAT_TYPE_CNT);
    *range = &mRTRange[type];
}

void DiskIO::SetDisk(Disk* disk)
{
    mDisk = disk;
}

void IOManager::GetAST(AST** ast)
{
    *ast = mAst;
}

void IOManager::IncRef()
{
    fs_atomic_inc(&mRef);
}

void IOManager::DecRef()
{
    if(fs_atomic_dec_and_test(&mRef))
    {
        delete this;
    }
}

//void IOManager::SetCollectOBJResponse(CollectOBJResponse* response)
//{
//    mCollectOBJResp = response;
//}

#endif

/* vim:set ts=4 sw=4 expandtab */
