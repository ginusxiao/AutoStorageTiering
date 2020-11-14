#include "AST/AST.hpp"
#include "AST/MessageImpl.hpp"
#include "AST/MessageExpress.hpp"
#include "AST/ASTTime.hpp"
#include "AST/ASTIO.hpp"

void ChunkIOStat::Reset()
{
    mRndReadIOs = 0;
    mRndWriteIOs = 0;
    mRndTotalRt = 0;
    mRndReadBw = 0;
    mRndWriteBw = 0;
    mSeqReadIOs = 0;
    mSeqWriteIOs = 0;
    mSeqTotalRt = 0;
    mSeqReadBw = 0;
    mSeqWriteBw = 0;
}

void ChunkIOStat::Dump(ostringstream* ostr)
{
    *ostr << "[mRndReadIOs: " << mRndReadIOs
            << ", mRndWriteIOs: " << mRndWriteIOs
            << ", mRndTotalRt: " << mRndTotalRt
            << ", mRndReadBw: " << mRndReadBw
            << ", mRndWriteBw: " << mRndWriteBw
            << ", mSeqReadIOs: " << mSeqReadIOs
            << ", mSeqWriteIOs: " << mSeqWriteIOs
            << ", mSeqTotalRt: " << mSeqTotalRt
            << ", mSeqReadBw: " << mSeqReadBw
            << ", mSeqWriteBw: " << mSeqWriteBw
            << "]\n";
}

ywb_uint32_t OBJIO::UpdateStatCycles()
{
    mStatCycles++;
    return mStatCycles;
}

void OBJIO::ResetStatCycles()
{
    mStatCycles = 0;
}

/*
 * FIXME: here use global GetAST() of singleton version
 **/
ywb_bool_t OBJIO::IsMonitoredLongEnough()
{
    AST* ast = NULL;
    TimeManager* timemgr = NULL;
    DecisionWindowManager* shortwindow = NULL;
    ywb_uint32_t shorttermcycles = 0;
    ywb_uint32_t cycles = 0;
    ywb_uint32_t thresh = 0;

    ast = GetSingleASTInstance();
    ast->GetTimeManager(&timemgr);
    timemgr->GetDecisionWindow(
            DecisionWindowManager::DW_short_term, &shortwindow);
    shorttermcycles = shortwindow->GetCycles();
    cycles = GetStatCycles();
    thresh = Constant::EFFECTIVE_MONITOR_CYCLE_PERCENT;

    if((100 * cycles) < (thresh * shorttermcycles))
    {
        return false;
    }
    else
    {
        return true;
    }
}

void OBJIO::CalculateEMAIO(ywb_uint32_t type, ywb_uint32_t multiplier1,
        ywb_uint32_t multiplier2, ywb_uint32_t divisor)
{
    ChunkIOStat* rawstat = NULL;
    ChunkIOStat* emastat = NULL;
    ywb_uint32_t newema = 0;

    YWB_ASSERT(IOStat::IOST_short_term_ema == type ||
            IOStat::IOST_long_term_ema == type);
    /*get current raw IO*/
    GetIOStat(IOStat::IOST_raw, &rawstat);
    /*get previous EMA*/
    GetIOStat(type, &emastat);

    /*calculate current EMA of random read IOs*/
    EMA::Calculate(emastat->GetRndReadIOs(), rawstat->GetRndReadIOs(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetRndReadIOs(newema);

    /*calculate current EMA of random write IOs*/
    EMA::Calculate(emastat->GetRndWriteIOs(), rawstat->GetRndWriteIOs(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetRndWriteIOs(newema);

    /*calculate current EMA of random total response time*/
    EMA::Calculate(emastat->GetRndTotalRT(), rawstat->GetRndTotalRT(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetRndTotalRT(newema);

    /*calculate current EMA of random read BW*/
    EMA::Calculate(emastat->GetRndReadBW(), rawstat->GetRndReadBW(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetRndReadBW(newema);

    /*calculate current EMA of random write BW*/
    EMA::Calculate(emastat->GetRndWriteBW(), rawstat->GetRndWriteBW(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetRndWriteBW(newema);

    /*calculate current EMA of sequential read IOs*/
    EMA::Calculate(emastat->GetSeqReadIOs(), rawstat->GetSeqReadIOs(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetSeqReadIOs(newema);

    /*calculate current EMA of sequential write IOs*/
    EMA::Calculate(emastat->GetSeqWriteIOs(), rawstat->GetSeqWriteIOs(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetSeqWriteIOs(newema);

    /*calculate current EMA of sequential total response time*/
    EMA::Calculate(emastat->GetSeqTotalRT(), rawstat->GetSeqTotalRT(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetSeqTotalRT(newema);

    /*calculate current EMA of sequential read BW*/
    EMA::Calculate(emastat->GetSeqReadBW(), rawstat->GetSeqReadBW(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetSeqReadBW(newema);

    /*calculate current EMA of sequential write BW*/
    EMA::Calculate(emastat->GetSeqWriteBW(), rawstat->GetSeqWriteBW(),
            multiplier1, multiplier2, divisor, &newema);
    emastat->SetSeqWriteBW(newema);
}

void
OBJIO::ResetIO(ywb_uint32_t type)
{
    YWB_ASSERT(0 <= type && type < Constant::IO_STAT_TYPE_CNT);
    mIOStat[type].Reset();
}

void
OBJIO::Dump(ostringstream* ostr)
{
    *ostr << "OBJIO:\n";
    *ostr << "IOStat type: IOST_raw:";
    mIOStat[IOStat::IOST_raw].Dump(ostr);
    *ostr << "IOStat type: IOST_short_term_ema:";
    mIOStat[IOStat::IOST_short_term_ema].Dump(ostr);
    *ostr << "IOStat type: IOST_long_term_ema:";
    mIOStat[IOStat::IOST_long_term_ema].Dump(ostr);
}

void
DiskIOStat::Reset()
{
    mRndReadStat = 0;
    mSeqReadStat = 0;
    mRndWriteStat = 0;
    mSeqWriteStat = 0;
    mReadStat = 0;
    mWriteStat = 0;
    mRndStat = 0;
    mSeqStat = 0;
}

void
DiskIOStatRange::Reset()
{
    mRndReadMin = YWB_UINT32_MAX;
    mRndReadMax = 0;
    mSeqReadMin = YWB_UINT32_MAX;
    mSeqReadMax = 0;
    mRndWriteMin = YWB_UINT32_MAX;
    mRndWriteMax = 0;
    mSeqWriteMin = YWB_UINT32_MAX;
    mSeqWriteMax = 0;
    mRndMin = YWB_UINT32_MAX;
    mRndMax = 0;
    mSeqMin = YWB_UINT32_MAX;
    mSeqMax = 0;
    mRndSeqMin = YWB_UINT32_MAX;
    mRndSeqMax = 0;
}

void
DiskIO::UpdateStatIOs(ywb_uint32_t type, OBJIO* obj, bool plus)
{
    ChunkIOStat *stat = NULL;
    ywb_uint32_t rndreadstat = 0;
    ywb_uint32_t seqreadstat = 0;
    ywb_uint32_t rndwritestat = 0;
    ywb_uint32_t seqwritestat = 0;
    ywb_uint32_t readstat = 0;
    ywb_uint32_t writestat = 0;
    ywb_uint32_t rndstat = 0;
    ywb_uint32_t seqstat = 0;

    if(IOStat::IOST_raw == type ||
            IOStat::IOST_short_term_ema == type ||
            IOStat::IOST_long_term_ema == type)
    {
        obj->GetIOStat(type, &stat);
        rndreadstat = mStatIOs[type].GetRndReadStat();
        seqreadstat = mStatIOs[type].GetSeqReadStat();
        rndwritestat = mStatIOs[type].GetRndWriteStat();
        seqwritestat = mStatIOs[type].GetSeqWriteStat();
        readstat = mStatIOs[type].GetReadStat();
        writestat = mStatIOs[type].GetWriteStat();
        rndstat = mStatIOs[type].GetRndStat();
        seqstat = mStatIOs[type].GetSeqStat();

        if(plus)
        {
            rndreadstat += stat->GetRndReadIOs();
            seqreadstat += stat->GetSeqReadIOs();
            rndwritestat += stat->GetRndWriteIOs();
            seqwritestat += stat->GetSeqWriteIOs();

            readstat += stat->GetRndReadIOs();
            readstat += stat->GetSeqReadIOs();

            writestat += stat->GetRndWriteIOs();
            writestat += stat->GetSeqWriteIOs();

            rndstat += stat->GetRndReadIOs();
            rndstat += stat->GetRndWriteIOs();

            seqstat += stat->GetSeqReadIOs();
            seqstat += stat->GetSeqWriteIOs();
        }
        else
        {
            if(rndreadstat > stat->GetRndReadIOs())
            {
                rndreadstat -= stat->GetRndReadIOs();
            }
            else
            {
                rndreadstat = 0;
            }

            if(seqreadstat > stat->GetSeqReadIOs())
            {
                seqreadstat -= stat->GetSeqReadIOs();
            }
            else
            {
                seqreadstat = 0;
            }

            if(rndwritestat > stat->GetRndWriteIOs())
            {
                rndwritestat -= stat->GetRndWriteIOs();
            }
            else
            {
                rndwritestat = 0;
            }

            if(seqwritestat > stat->GetSeqWriteIOs())
            {
                seqwritestat -= stat->GetSeqWriteIOs();
            }
            else
            {
                seqwritestat = 0;
            }

            if(readstat > (stat->GetRndReadIOs() + stat->GetSeqReadIOs()))
            {
                readstat -= (stat->GetRndReadIOs() + stat->GetSeqReadIOs());
            }
            else
            {
                readstat = 0;
            }

            if(writestat > (stat->GetRndWriteIOs() + stat->GetSeqWriteIOs()))
            {
                writestat -= (stat->GetRndWriteIOs() + stat->GetSeqWriteIOs());
            }
            else
            {
                writestat = 0;
            }

            if(rndstat > (stat->GetRndReadIOs() + stat->GetRndWriteIOs()))
            {
                rndstat -= (stat->GetRndReadIOs() + stat->GetRndWriteIOs());
            }
            else
            {
                rndstat = 0;
            }

            if(seqstat > (stat->GetSeqReadIOs() + stat->GetSeqWriteIOs()))
            {
                seqstat -= (stat->GetSeqReadIOs() + stat->GetSeqWriteIOs());
            }
            else
            {
                seqstat = 0;
            }
        }

        mStatIOs[type].SetRndReadStat(rndreadstat);
        mStatIOs[type].SetSeqReadStat(seqreadstat);
        mStatIOs[type].SetRndWriteStat(rndwritestat);
        mStatIOs[type].SetSeqWriteStat(seqwritestat);

        mStatIOs[type].SetReadStat(readstat);
        mStatIOs[type].SetWriteStat(writestat);

        mStatIOs[type].SetRndStat(rndstat);
        mStatIOs[type].SetSeqStat(seqstat);
    }
}

void
DiskIO::UpdateStatBW(ywb_uint32_t type, OBJIO* obj, bool plus)
{
    ChunkIOStat *stat = NULL;
    ywb_uint32_t rndreadstat = 0;
    ywb_uint32_t seqreadstat = 0;
    ywb_uint32_t rndwritestat = 0;
    ywb_uint32_t seqwritestat = 0;
    ywb_uint32_t readstat = 0;
    ywb_uint32_t writestat = 0;
    ywb_uint32_t rndstat = 0;
    ywb_uint32_t seqstat = 0;

    if(IOStat::IOST_raw == type ||
            IOStat::IOST_short_term_ema == type ||
            IOStat::IOST_long_term_ema == type)
    {
        obj->GetIOStat(type, &stat);
        rndreadstat = mStatBW[type].GetRndReadStat();
        seqreadstat = mStatBW[type].GetSeqReadStat();
        rndwritestat = mStatBW[type].GetRndWriteStat();
        seqwritestat = mStatBW[type].GetSeqWriteStat();
        readstat = mStatBW[type].GetReadStat();
        writestat = mStatBW[type].GetWriteStat();
        rndstat = mStatBW[type].GetRndStat();
        seqstat = mStatBW[type].GetSeqStat();

        if(plus)
        {
            rndreadstat += stat->GetRndReadBW();
            seqreadstat += stat->GetSeqReadBW();
            rndwritestat += stat->GetRndWriteBW();
            seqwritestat += stat->GetSeqWriteBW();

            readstat += stat->GetRndReadBW();
            readstat += stat->GetSeqReadBW();

            writestat += stat->GetRndWriteBW();
            writestat += stat->GetSeqWriteBW();

            rndstat += stat->GetRndReadBW();
            rndstat += stat->GetRndWriteBW();

            seqstat += stat->GetSeqReadBW();
            seqstat += stat->GetSeqWriteBW();
        }
        else
        {
            if(rndreadstat > stat->GetRndReadBW())
            {
                rndreadstat -= stat->GetRndReadBW();
            }
            else
            {
                rndreadstat = 0;
            }

            if(seqreadstat > stat->GetSeqReadBW())
            {
                seqreadstat -= stat->GetSeqReadBW();
            }
            else
            {
                seqreadstat = 0;
            }

            if(rndwritestat > stat->GetRndWriteBW())
            {
                rndwritestat -= stat->GetRndWriteBW();
            }
            else
            {
                rndwritestat = 0;
            }

            if(seqwritestat > stat->GetSeqWriteBW())
            {
                seqwritestat -= stat->GetSeqWriteBW();
            }
            else
            {
                seqwritestat = 0;
            }

            if(readstat > (stat->GetRndReadBW() + stat->GetSeqReadBW()))
            {
                readstat -= (stat->GetRndReadBW() + stat->GetSeqReadBW());
            }
            else
            {
                readstat = 0;
            }

            if(writestat > (stat->GetRndWriteBW() + stat->GetSeqWriteBW()))
            {
                writestat -= (stat->GetRndWriteBW() + stat->GetSeqWriteBW());
            }
            else
            {
                writestat = 0;
            }

            if(rndstat > (stat->GetRndReadBW() + stat->GetRndWriteBW()))
            {
                rndstat -= (stat->GetRndReadBW() + stat->GetRndWriteBW());
            }
            else
            {
                rndstat = 0;
            }

            if(seqstat > (stat->GetSeqReadBW() + stat->GetSeqWriteBW()))
            {
                seqstat -= (stat->GetSeqReadBW() + stat->GetSeqWriteBW());
            }
            else
            {
                seqstat = 0;
            }
        }

        mStatBW[type].SetRndReadStat(rndreadstat);
        mStatBW[type].SetSeqReadStat(seqreadstat);
        mStatBW[type].SetRndWriteStat(rndwritestat);
        mStatBW[type].SetSeqWriteStat(seqwritestat);

        mStatBW[type].SetReadStat(readstat);
        mStatBW[type].SetWriteStat(writestat);

        mStatBW[type].SetRndStat(rndstat);
        mStatBW[type].SetSeqStat(seqstat);
    }
}

void
DiskIO::UpdateStatRT(ywb_uint32_t type, OBJIO* obj, bool plus)
{
    ChunkIOStat *stat = NULL;
    ywb_uint32_t rndstat = 0;
    ywb_uint32_t seqstat = 0;

    if(IOStat::IOST_raw == type ||
            IOStat::IOST_short_term_ema == type ||
            IOStat::IOST_long_term_ema == type)
    {
        obj->GetIOStat(type, &stat);
        rndstat = mStatRT[type].GetRndStat();
        seqstat = mStatRT[type].GetSeqStat();

        if(plus)
        {
            rndstat += stat->GetRndTotalRT();
            seqstat += stat->GetSeqTotalRT();
        }
        else
        {
            if(rndstat > stat->GetRndTotalRT())
            {
                rndstat -= stat->GetRndTotalRT();
            }
            else
            {
                rndstat = 0;
            }

            if(seqstat > stat->GetSeqTotalRT())
            {
                seqstat -= stat->GetRndTotalRT();
            }
            else
            {
                seqstat = 0;
            }
        }

        mStatRT[type].SetRndStat(rndstat);
        mStatRT[type].SetSeqStat(seqstat);
    }
}

void
DiskIO::UpdateIOsRange(ywb_uint32_t type, OBJIO* obj)
{
    ChunkIOStat *stat = NULL;
    ywb_uint32_t rndreadios = 0;
    ywb_uint32_t seqreadios = 0;
    ywb_uint32_t rndwriteios = 0;
    ywb_uint32_t seqwriteios = 0;
    ywb_uint32_t rndios = 0;
    ywb_uint32_t seqios = 0;
    ywb_uint32_t rndseqios = 0;

    YWB_ASSERT(type < Constant::IO_STAT_TYPE_CNT);
    obj->GetIOStat(type, &stat);

    rndreadios = stat->GetRndReadIOs();
    seqreadios = stat->GetSeqReadIOs();
    rndwriteios = stat->GetRndWriteIOs();
    seqwriteios = stat->GetSeqWriteIOs();
    rndios = rndreadios + rndwriteios;
    seqios = seqreadios + seqwriteios;
    rndseqios = rndios + seqios;

    if(rndreadios < mIOsRange[type].GetRndReadMin())
    {
        mIOsRange[type].SetRndReadMin(rndreadios);
    }

    if(rndreadios > mIOsRange[type].GetRndReadMax())
    {
        mIOsRange[type].SetRndReadMax(rndreadios);
    }

    if(seqreadios < mIOsRange[type].GetSeqReadMin())
    {
        mIOsRange[type].SetSeqReadMin(seqreadios);
    }

    if(seqreadios > mIOsRange[type].GetSeqReadMax())
    {
        mIOsRange[type].SetSeqReadMax(seqreadios);
    }

    if(rndwriteios < mIOsRange[type].GetRndWriteMin())
    {
        mIOsRange[type].SetRndWriteMin(rndwriteios);
    }

    if(rndwriteios > mIOsRange[type].GetRndWriteMax())
    {
        mIOsRange[type].SetRndWriteMax(rndwriteios);
    }

    if(seqwriteios < mIOsRange[type].GetSeqWriteMin())
    {
        mIOsRange[type].SetSeqWriteMin(seqwriteios);
    }

    if(seqwriteios > mIOsRange[type].GetSeqWriteMax())
    {
        mIOsRange[type].SetSeqWriteMax(seqwriteios);
    }

    if(rndios < mIOsRange[type].GetRndMin())
    {
        mIOsRange[type].SetRndMin(rndios);
    }

    if(rndios > mIOsRange[type].GetRndMax())
    {
        mIOsRange[type].SetRndMax(rndios);
    }

    if(seqios < mIOsRange[type].GetSeqMin())
    {
        mIOsRange[type].SetSeqMin(seqios);
    }

    if(seqios > mIOsRange[type].GetSeqMax())
    {
        mIOsRange[type].SetSeqMax(seqios);
    }

    if(rndseqios < mIOsRange[type].GetRndSeqMin())
    {
        mIOsRange[type].SetRndSeqMin(rndseqios);
    }

    if(rndseqios > mIOsRange[type].GetRndSeqMax())
    {
        mIOsRange[type].SetRndSeqMax(rndseqios);
    }
}

void
DiskIO::UpdateBWRange(ywb_uint32_t type, OBJIO* obj)
{
    ChunkIOStat *stat = NULL;
//    ywb_uint32_t rndreadbw = 0;
//    ywb_uint32_t seqreadbw = 0;
//    ywb_uint32_t rndwritebw = 0;
//    ywb_uint32_t seqwritebw = 0;
    ywb_uint32_t rndbw = 0;
    ywb_uint32_t seqbw = 0;
    ywb_uint32_t rndseqbw = 0;

    YWB_ASSERT(type < Constant::IO_STAT_TYPE_CNT);
    obj->GetIOStat(type, &stat);

    rndbw = stat->GetRndReadBW() + stat->GetRndWriteBW();
    seqbw = stat->GetSeqReadBW() + stat->GetSeqWriteBW();
    rndseqbw = rndbw + seqbw;

//    if(rndreadbw < mBWRange[type].GetRndReadMin())
//    {
//        mBWRange[type].SetRndReadMin(rndreadbw);
//    }
//
//    if(rndreadbw > mBWRange[type].GetRndReadMax())
//    {
//        mBWRange[type].SetRndReadMax(rndreadbw);
//    }
//
//    if(seqreadbw < mBWRange[type].GetSeqReadMin())
//    {
//        mBWRange[type].SetSeqReadMin(seqreadbw);
//    }
//
//    if(seqreadbw > mBWRange[type].GetSeqReadMax())
//    {
//        mBWRange[type].SetSeqReadMax(seqreadbw);
//    }
//
//    if(rndwritebw < mBWRange[type].GetRndWriteMin())
//    {
//        mBWRange[type].SetRndWriteMin(rndreadbw);
//    }
//
//    if(rndwritebw > mBWRange[type].GetRndWriteMax())
//    {
//        mBWRange[type].SetRndWriteMax(rndreadbw);
//    }
//
//    if(seqwritebw < mBWRange[type].GetSeqWriteMin())
//    {
//        mBWRange[type].SetSeqWriteMin(seqwritebw);
//    }
//
//    if(seqwritebw > mBWRange[type].GetSeqWriteMax())
//    {
//        mBWRange[type].SetSeqWriteMax(seqwritebw);
//    }

    if(rndbw < mBWRange[type].GetRndMin())
    {
        mBWRange[type].SetRndMin(rndbw);
    }

    if(rndbw > mBWRange[type].GetRndMax())
    {
        mBWRange[type].SetRndMax(rndbw);
    }

    if(seqbw < mBWRange[type].GetSeqMin())
    {
        mBWRange[type].SetSeqMin(seqbw);
    }

    if(seqbw > mBWRange[type].GetSeqMax())
    {
        mBWRange[type].SetSeqMax(seqbw);
    }

    if(rndseqbw < mBWRange[type].GetRndSeqMin())
    {
        mBWRange[type].SetRndSeqMin(rndseqbw);
    }

    if(rndseqbw > mBWRange[type].GetRndSeqMax())
    {
        mBWRange[type].SetRndSeqMax(rndseqbw);
    }
}

/*
 * minimum random response time
     *      maximum random response time
     *      minimum sequential response time
     *      maximum sequential response time*/
void
DiskIO::UpdateRTRange(ywb_uint32_t type, OBJIO* obj)
{
    ChunkIOStat *stat = NULL;
    ywb_uint32_t rndresp = 0;
    ywb_uint32_t seqresp = 0;
    ywb_uint32_t rndseqresp = 0;

    YWB_ASSERT(type < Constant::IO_STAT_TYPE_CNT);
    obj->GetIOStat(type, &stat);

    rndresp = stat->GetRndTotalRT();
    seqresp = stat->GetSeqTotalRT();
    rndseqresp = rndresp + seqresp;

    if(rndresp < mRTRange[type].GetRndMin())
    {
        mRTRange[type].SetRndMin(rndresp);
    }

    if(rndresp > mRTRange[type].GetRndMax())
    {
        mRTRange[type].SetRndMax(rndresp);
    }

    if(seqresp < mRTRange[type].GetSeqMin())
    {
        mRTRange[type].SetSeqMin(seqresp);
    }

    if(seqresp > mRTRange[type].GetSeqMax())
    {
        mRTRange[type].SetSeqMax(seqresp);
    }

    if(rndseqresp < mRTRange[type].GetRndSeqMin())
    {
        mRTRange[type].SetRndSeqMin(rndseqresp);
    }

    if(rndseqresp > mRTRange[type].GetRndSeqMax())
    {
        mRTRange[type].SetRndSeqMax(rndseqresp);
    }
}

void
DiskIO::Reset()
{
    ywb_uint32_t iostattype = IOStat::IOST_cnt;

    for(iostattype = IOStat::IOST_raw;
            iostattype < IOStat::IOST_cnt;
            iostattype++)
    {
        mStatIOs[iostattype].Reset();
        mStatBW[iostattype].Reset();
        mStatRT[iostattype].Reset();
        mIOsRange[iostattype].Reset();
        mBWRange[iostattype].Reset();
        mRTRange[iostattype].Reset();
    }
}

IOManager::IOManager(AST* ast)
{
    YWB_ASSERT(ast != NULL);

    mCollectOBJPrepReq = new CollectOBJRequest(Constant::OSS_ID);
    YWB_ASSERT(mCollectOBJPrepReq != NULL);
    mCollectOBJPrepReq->SetFlag(CollectOBJRequest::F_prepare);
    mCollectOBJReq = new CollectOBJRequest(Constant::OSS_ID);
    YWB_ASSERT(mCollectOBJReq != NULL);
    mCollectOBJReq->SetFlag(CollectOBJRequest::F_normal);
    mCollectOBJPostReq = new CollectOBJRequest(Constant::OSS_ID);
    YWB_ASSERT(mCollectOBJPostReq != NULL);
    mCollectOBJPostReq->SetFlag(CollectOBJRequest::F_post);

    mRef = 1;
    mAst = ast;
}

IOManager::~IOManager()
{
    if(mCollectOBJPrepReq)
    {
        mCollectOBJPrepReq->DecRef();
    }

    if(mCollectOBJReq)
    {
        mCollectOBJReq->DecRef();
    }

    if(mCollectOBJPostReq)
    {
        mCollectOBJPostReq->DecRef();
    }

    mAst = NULL;
}

ywb_status_t
IOManager::CollectOBJIOPrep(ywb_uint32_t ossid)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Generator* gen = NULL;

    GetAST(&ast);
    ast->GetGenerator(&gen);
    gen->Send(mCollectOBJPrepReq);
    return ret;
}

/*
 * currently @ossid is ignored
 **/
ywb_status_t
IOManager::CollectOBJIO(ywb_uint32_t ossid)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Generator* gen = NULL;

    GetAST(&ast);
    ast->GetGenerator(&gen);
//    gen->SendSync(mCollectOBJReq);
    gen->Send(mCollectOBJReq);
    return ret;
}

ywb_status_t
IOManager::CollectOBJIOPost(ywb_uint32_t ossid)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Generator* gen = NULL;

    GetAST(&ast);
    ast->GetGenerator(&gen);
    gen->Send(mCollectOBJPostReq);
    return ret;
}

ywb_status_t
IOManager::ResolveOBJIO(CollectOBJResponse* collectobjresp)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t index = 0;
    ywb_uint32_t shorttermmultiplier1 = 0;
    ywb_uint32_t shorttermmultiplier2 = 0;
    ywb_uint32_t shorttermdevisor = 0;
    ywb_uint32_t longtermmultiplier1 = 0;
    ywb_uint32_t longtermmultiplier2 = 0;
    ywb_uint32_t longtermdevisor = 0;
    ywb_bool_t objresolutionsuccess = ywb_false;
    AST* ast = NULL;
    TimeManager* timemgr = NULL;
    DecisionWindowManager* shorttermwindow = NULL;
    DecisionWindowManager* longtermwindow = NULL;
    LogicalConfig* config = NULL;
    CollectOBJResponseFragment** collectobjrespfrags = NULL;
    CollectOBJResponseFragment* collectobjrespfrag = NULL;
    OBJInfo objinfo;
    OBJKey* objkey = NULL;
    ChunkIOStat* objstat = NULL;
    OBJIO* objio = NULL;
    OBJ* obj = NULL;

    GetAST(&ast);
    ast->GetTimeManager(&timemgr);
    timemgr->GetDecisionWindow(
            DecisionWindowManager::DW_short_term, &shorttermwindow);
    timemgr->GetDecisionWindow(
            DecisionWindowManager::DW_long_term, &longtermwindow);
    ast->GetLogicalConfig(&config);

    shorttermmultiplier1 = shorttermwindow->GetMultiplierOne();
    shorttermmultiplier2 = shorttermwindow->GetMultiplierTwo();
    shorttermdevisor = shorttermwindow->GetCyclesPlusOneSquare();
    longtermmultiplier1 = longtermwindow->GetMultiplierOne();
    longtermmultiplier2 = longtermwindow->GetMultiplierTwo();
    longtermdevisor = longtermwindow->GetCyclesPlusOneSquare();

    do {
        YWB_ASSERT(collectobjresp != NULL);
        if(YWB_SUCCESS != collectobjresp->GetStatus())
        {
            ast_log_debug("Resolve OBJ IO failure for failing to collect OBJ IO");
            ret = collectobjresp->GetStatus();
            break;
        }

        ast_log_debug("Resolve OBJ IO start, fragments num "
                << collectobjresp->GetFragmentNum());
        collectobjrespfrags = collectobjresp->GetFragments();
        YWB_ASSERT(collectobjrespfrags != NULL);
        YWB_ASSERT(collectobjresp->GetFragmentNum() <= Constant::OBJ_IO_RESPONSE_BATCH_NUM);
        for(index = 0; (index < collectobjresp->GetFragmentNum()) &&
                (YWB_SUCCESS == ret); index++)
        {
            collectobjrespfrag = collectobjrespfrags[index];
            YWB_ASSERT(collectobjrespfrag != NULL);
            YWB_ASSERT(YWB_SUCCESS == collectobjrespfrag->GetStatus());
            objresolutionsuccess = ywb_false;

            ast_log_debug("Resolve fragment " << index << " with "
                    << collectobjrespfrag->GetOBJCnt() << " OBJs");
            ret = collectobjrespfrag->GetFirstOBJ(&objinfo);
            while((YWB_SUCCESS == ret))
            {
                objresolutionsuccess = ywb_true;
                objinfo.GetOBJKey(&objkey);
                objinfo.GetChunkIOStat(&objstat);

                /*firstly find in current configuration*/
                ret = GetOBJIO(*objkey, &objio);
                if((YWB_SUCCESS == ret) && (objio != NULL))
                {
                    objio->GetOBJ(&obj);
                    obj->ClearFlag(OBJ::OF_suspicious);

                    /*just modify the already existed OBJ IO*/
                    objio->SetRawIO(*objstat);
                    /*FIXME: shall reset the short term EMA or not???*/
                    if(shorttermwindow->IsNewWindowStarted())
                    {
                        objio->ResetStatCycles();
        //                objio->ResetIO(IOStat::IOST_short_term_ema);
                    }

                    objio->CalculateEMAIO(IOStat::IOST_short_term_ema,
                            shorttermmultiplier1, shorttermmultiplier2,
                            shorttermdevisor);
                    objio->UpdateStatCycles();
                    /*FIXME: shall reset the long term EMA or not???*/
                    if(longtermwindow->IsNewWindowStarted())
                    {
        //                objio->ResetIO(IOStat::IOST_long_term_ema);
                    }

                    objio->CalculateEMAIO(IOStat::IOST_long_term_ema,
                            longtermmultiplier1, longtermmultiplier2,
                            longtermdevisor);
                }
                else if((YWB_SUCCESS == ret) && (objio == NULL))
                {
                    /*hava not set objio*/
                    ret = config->GetOBJ(*objkey, &obj);
                    YWB_ASSERT((YWB_SUCCESS == ret) && (obj != NULL));
                    obj->ClearFlag(OBJ::OF_suspicious);

                    objio = new OBJIO();
                    if(objio != NULL)
                    {
                        objio->SetRawIO(*objstat);
                        objio->CalculateEMAIO(IOStat::IOST_short_term_ema,
                                shorttermmultiplier1, shorttermmultiplier2,
                                shorttermdevisor);
                        objio->CalculateEMAIO(IOStat::IOST_long_term_ema,
                                longtermmultiplier1, longtermmultiplier2,
                                longtermdevisor);
                        objio->UpdateStatCycles();
                        objio->SetOBJ(obj);
                        obj->SetOBJIO(objio);
                        config->PutOBJ(obj);
                    }
                    else
                    {
                        config->PutOBJ(obj);
                        ret = YFS_EOUTOFMEMORY;
                        break;
                    }
                }
                else
                {
                    objio = new OBJIO();
                    if(objio != NULL)
                    {
                        obj = new OBJ(*objkey);
                        if(obj != NULL)
                        {
                            obj->ClearFlag(OBJ::OF_suspicious);
                            objio->SetRawIO(*objstat);
                            objio->CalculateEMAIO(IOStat::IOST_short_term_ema,
                                    shorttermmultiplier1, shorttermmultiplier2,
                                    shorttermdevisor);
                            objio->CalculateEMAIO(IOStat::IOST_long_term_ema,
                                    longtermmultiplier1, longtermmultiplier2,
                                    longtermdevisor);
                            objio->UpdateStatCycles();
                            objio->SetOBJ(obj);
        //                    obj->SetOBJKey(*objkey);
                            obj->SetOBJIO(objio);
                            ret = config->AddOBJ(*objkey, obj);
                            if(ret != YWB_SUCCESS)
                            {
                                delete obj;
                                obj = NULL;
                            }
                        }
                        else
                        {
                            delete objio;
                            objio = NULL;
                            ret = YFS_EOUTOFMEMORY;
                            break;
                        }
                    }
                    else
                    {
                        ret = YFS_EOUTOFMEMORY;
                        break;
                    }
                }

                ret = collectobjrespfrag->GetNextOBJ(&objinfo);
            }

            if((YFS_EOUTOFMEMORY != ret) && (ywb_true == objresolutionsuccess))
            {
                ret = YWB_SUCCESS;
            }
        }

        ast_log_debug("Resolve OBJ IO done, ret " << ret);
    } while(0);

    collectobjresp->DecRef();

    return ret;
}

ywb_status_t
IOManager::SummarizeDiskIO()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t errout = false;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    SubPoolKey subpoolkey;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);

    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        ret = subpool->SummarizeDiskIO();
        config->PutSubPool(subpool);
        if(ret != YWB_SUCCESS)
        {
            errout = true;
            break;
        }

        ret = config->GetNextSubPool(&subpool, &subpoolkey);
    }

    if(false == errout)
    {
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t
IOManager::AddOBJIO(OBJKey& key, OBJIO*& objio)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig *config = NULL;
    OBJVal* obj = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);

    ret = config->GetOBJ(key, &obj);
    if(YWB_SUCCESS == ret)
    {
        obj->SetOBJIO(objio);
        objio->SetOBJ(obj);
        config->PutOBJ(obj);
    }

    return ret;
}

ywb_status_t
IOManager::AddDiskIO(DiskKey& key, DiskIO*& diskio)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig *config = NULL;
    Disk* disk = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);

    ret = config->GetDisk(key, &disk);
    if(YWB_SUCCESS == ret)
    {
        disk->SetDiskIO(diskio);
        config->PutDisk(disk);
    }

    return ret;
}

ywb_status_t
IOManager::GetOBJIO(OBJKey& key, OBJIO** objio)
{
    ywb_status_t ret = YFS_ENOENT;
    AST* ast = NULL;
    LogicalConfig *config = NULL;
    OBJVal* obj = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);

    *objio = NULL;
    ret = config->GetOBJ(key, &obj);
    if(YWB_SUCCESS == ret)
    {
        obj->GetOBJIO(objio);
        config->PutOBJ(obj);
    }

    return ret;
}

ywb_status_t
IOManager::GetDiskIO(DiskKey& key, DiskIO** diskio)
{
    ywb_status_t ret = YFS_ENOENT;
    AST* ast = NULL;
    LogicalConfig *config = NULL;
    Disk* disk = NULL;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);

    *diskio = NULL;
    ret = config->GetDisk(key, &disk);
    if(YWB_SUCCESS == ret)
    {
        disk->GetDiskIO(diskio);
        config->PutDisk(disk);
    }

    return ret;
}

ywb_status_t
IOManager::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("IOManager reset");
    mCollectOBJPrepReq->SetStatus(YWB_SUCCESS);
    mCollectOBJReq->SetStatus(YWB_SUCCESS);
    mCollectOBJPostReq->SetStatus(YWB_SUCCESS);
    mRef = 1;

    return ret;
}

ywb_status_t
IOManager::Consolidate()
{
    ywb_status_t ret = YWB_SUCCESS;

    /*summarize disk IO before generating advise*/
    ret = SummarizeDiskIO();

    return ret;
}


/* vim:set ts=4 sw=4 expandtab */
