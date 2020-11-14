#ifndef __AST_IO_HPP__
#define __AST_IO_HPP__

#include <string>
#include <sstream>
#include "FsInts.h"
#include "FsStatus.h"
#include "FsAtomic.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/Singleton.hpp"

using namespace unise;

/*forward reference*/
//class OBJ;
//class Disk;
//class LogicalConfig;
class EMA
{
public:
    static void Calculate(ywb_uint32_t prevema, ywb_uint32_t curval,
            ywb_uint32_t multiplier1, ywb_uint32_t multiplier2,
            ywb_uint32_t divisor, ywb_uint32_t *curema)
    {
        YWB_ASSERT(divisor != 0);
        if(prevema != 0)
        {
            *curema = ((prevema * multiplier1) +
                    (curval * multiplier2)) / (divisor);
        }
        else
        {
            *curema = curval;
        }
    }
};

class IOStat
{
public:
    enum Type{
        IOST_raw = 0,
        IOST_short_term_ema,
        IOST_long_term_ema,
        IOST_cnt,
    };

    static Type Uint2Type(ywb_uint32_t val)
    {
        if(val == 0)
        {
            return IOST_raw;
        }
        else if(val == 1)
        {
            return IOST_short_term_ema;
        }
        else if(val == 2)
        {
            return IOST_long_term_ema;
        }
        else
        {
            return IOST_cnt;
        }
    }
};

class ChunkIOStat {
public:
    ChunkIOStat() : mRndReadIOs(0), mRndWriteIOs(0),
            mRndTotalRt(0), mRndReadBw(0),
            mRndWriteBw(0), mSeqReadIOs(0),
            mSeqWriteIOs(0), mSeqTotalRt(0),
            mSeqReadBw(0), mSeqWriteBw(0){}

    ChunkIOStat(const ChunkIOStat& stat)
    {
        this->mRndReadIOs = stat.mRndReadIOs;
        this->mRndWriteIOs = stat.mRndWriteIOs;
        this->mRndTotalRt =  stat.mRndTotalRt;
        this->mRndReadBw = stat.mRndReadBw;
        this->mRndWriteBw = stat.mRndWriteBw;

        this->mSeqReadIOs = stat.mSeqReadIOs;
        this->mSeqWriteIOs = stat.mSeqWriteIOs;
        this->mSeqTotalRt =  stat.mSeqTotalRt;
        this->mSeqReadBw = stat.mSeqReadBw;
        this->mSeqWriteBw = stat.mSeqWriteBw;
    }

    ChunkIOStat& operator= (const ChunkIOStat& stat)
    {
        if(this == &stat)
            return *this;

        this->mRndReadIOs = stat.mRndReadIOs;
        this->mRndWriteIOs = stat.mRndWriteIOs;
        this->mRndTotalRt =  stat.mRndTotalRt;
        this->mRndReadBw = stat.mRndReadBw;
        this->mRndWriteBw = stat.mRndWriteBw;

        this->mSeqReadIOs = stat.mSeqReadIOs;
        this->mSeqWriteIOs = stat.mSeqWriteIOs;
        this->mSeqTotalRt =  stat.mSeqTotalRt;
        this->mSeqReadBw = stat.mSeqReadBw;
        this->mSeqWriteBw = stat.mSeqWriteBw;

        return *this;
    }

    inline ywb_uint32_t GetRndReadIOs();
    inline ywb_uint32_t GetRndWriteIOs();
    inline ywb_uint32_t GetRndTotalRT();
    inline ywb_uint32_t GetRndReadBW();
    inline ywb_uint32_t GetRndWriteBW();
    inline ywb_uint32_t GetSeqReadIOs();
    inline ywb_uint32_t GetSeqWriteIOs();
    inline ywb_uint32_t GetSeqTotalRT();
    inline ywb_uint32_t GetSeqReadBW();
    inline ywb_uint32_t GetSeqWriteBW();

    inline void SetRndReadIOs(ywb_uint32_t val);
    inline void SetRndWriteIOs(ywb_uint32_t val);
    inline void SetRndTotalRT(ywb_uint32_t val);
    inline void SetRndReadBW(ywb_uint32_t val);
    inline void SetRndWriteBW(ywb_uint32_t val);
    inline void SetSeqReadIOs(ywb_uint32_t val);
    inline void SetSeqWriteIOs(ywb_uint32_t val);
    inline void SetSeqTotalRT(ywb_uint32_t val);
    inline void SetSeqReadBW(ywb_uint32_t val);
    inline void SetSeqWriteBW(ywb_uint32_t val);

    void Reset();
    void Dump(ostringstream* ostr);

private:
    /*random read IO count*/
    ywb_uint32_t mRndReadIOs;
    /*random write IO count*/
    ywb_uint32_t mRndWriteIOs;
    /*random IO accumulative response time, in measure of msec*/
    ywb_uint32_t mRndTotalRt;
    /*random read IO size*/
    ywb_uint32_t mRndReadBw;
    /*random write IO size*/
    ywb_uint32_t mRndWriteBw;

    /*sequential read IO count*/
    ywb_uint32_t mSeqReadIOs;
    /*sequential write IO count*/
    ywb_uint32_t mSeqWriteIOs;
    /*sequential IO accumulative response time, in measure of msec*/
    ywb_uint32_t mSeqTotalRt;
    /*sequential read IO size*/
    ywb_uint32_t mSeqReadBw;
    /*sequential write IO size*/
    ywb_uint32_t mSeqWriteBw;
};

class OBJIO {
public:
    OBJIO() : mStatCycles(0), mPeakBwLastWin(0),
            mPeakBwCurWin(0), mOBJ(NULL){}

    ~OBJIO()
    {
        mOBJ = NULL;
    }

    /*unify GetRawIO and GetEMAIO, mainly for external use*/
    inline void GetIOStat(ywb_uint32_t type, ChunkIOStat** val);
    inline ywb_uint32_t GetStatCycles();
    inline ywb_uint32_t GetPeakBwLastWin();
    inline ywb_uint32_t GetPeakBwCurWin();
    inline void GetOBJ(OBJ** val);

    inline void SetRawIO(ChunkIOStat& val);
    inline void SetStatCycles(ywb_uint32_t val);
    inline void SetPeakBwLastWin(ywb_uint32_t val);
    inline void SetPeakBwCurWin(ywb_uint32_t val);
    inline void SetOBJ(OBJ*& val);

    ywb_uint32_t UpdateStatCycles();
    void ResetStatCycles();
    ywb_bool_t IsMonitoredLongEnough();
    /*calculate EMA IO stat for specified ema index(decision window)*/
    void CalculateEMAIO(ywb_uint32_t type, ywb_uint32_t multiplier1,
            ywb_uint32_t multiplier2, ywb_uint32_t divisor);
    /*reset OBJIO of certain io stat type*/
    void ResetIO(ywb_uint32_t type);
    void Dump(ostringstream* ostr);

private:
    /*RAW IO, short-term EMA IO and long-term EMA IO*/
    ChunkIOStat mIOStat[Constant::IO_STAT_TYPE_CNT];
    /*how long this OBJ IO is monitored*/
    ywb_uint32_t mStatCycles;
    /*max raw BW in last window*/
    ywb_uint32_t mPeakBwLastWin;
    /*max raw BW in current window*/
    ywb_uint32_t mPeakBwCurWin;
    /*back reference to OBJ*/
    OBJ* mOBJ;
};

class DiskIOStat
{
public:
    DiskIOStat() : mRndReadStat(0), mSeqReadStat(0),
            mRndWriteStat(0),mSeqWriteStat(0), mReadStat(0),
            mWriteStat(0), mRndStat(0), mSeqStat(0){}

    inline ywb_uint32_t GetRndReadStat();
    inline ywb_uint32_t GetSeqReadStat();
    inline ywb_uint32_t GetRndWriteStat();
    inline ywb_uint32_t GetSeqWriteStat();
    inline ywb_uint32_t GetReadStat();
    inline ywb_uint32_t GetWriteStat();
    inline ywb_uint32_t GetRndStat();
    inline ywb_uint32_t GetSeqStat();

    inline void SetRndReadStat(ywb_uint32_t val);
    inline void SetSeqReadStat(ywb_uint32_t val);
    inline void SetRndWriteStat(ywb_uint32_t val);
    inline void SetSeqWriteStat(ywb_uint32_t val);
    inline void SetReadStat(ywb_uint32_t val);
    inline void SetWriteStat(ywb_uint32_t val);
    inline void SetRndStat(ywb_uint32_t val);
    inline void SetSeqStat(ywb_uint32_t val);

    void Reset();

private:
    /*random read IO/BW/RESPTIME*/
    ywb_uint32_t mRndReadStat;
    /*sequential read IO/BW/RESPTIME*/
    ywb_uint32_t mSeqReadStat;
    /*random write IO/BW/RESPTIME*/
    ywb_uint32_t mRndWriteStat;
    /*sequential write IO/BW/RESPTIME*/
    ywb_uint32_t mSeqWriteStat;
    /*read IO/BW/RESPTIME*/
    ywb_uint32_t mReadStat;
    /*write IO/BW/RESPTIME*/
    ywb_uint32_t mWriteStat;
    /*random IO/BW/RESPTIME*/
    ywb_uint32_t mRndStat;
    /*sequential IO/BW/RESPTIME*/
    ywb_uint32_t mSeqStat;
};

class DiskIOStatRange
{
public:
    DiskIOStatRange() : mRndReadMin(YWB_UINT32_MAX), mRndReadMax(0),
            mSeqReadMin(YWB_UINT32_MAX), mSeqReadMax(0),
            mRndWriteMin(YWB_UINT32_MAX), mRndWriteMax(0),
            mSeqWriteMin(YWB_UINT32_MAX), mSeqWriteMax(0),
            mRndMin(YWB_UINT32_MAX), mRndMax(0),
            mSeqMin(YWB_UINT32_MAX), mSeqMax(0),
            mRndSeqMin(YWB_UINT32_MAX), mRndSeqMax(0) {}

    inline ywb_uint32_t GetRndMax();
    inline ywb_uint32_t GetRndMin();
    inline ywb_uint32_t GetRndReadMax();
    inline ywb_uint32_t GetRndReadMin();
    inline ywb_uint32_t GetRndWriteMax();
    inline ywb_uint32_t GetRndWriteMin();
    inline ywb_uint32_t GetSeqMax();
    inline ywb_uint32_t GetSeqMin();
    inline ywb_uint32_t GetSeqReadMax();
    inline ywb_uint32_t GetSeqReadMin();
    inline ywb_uint32_t GetSeqWriteMax();
    inline ywb_uint32_t GetSeqWriteMin();
    inline ywb_uint32_t GetRndSeqMax();
    inline ywb_uint32_t GetRndSeqMin();

    inline void SetRndMax(ywb_uint32_t val);
    inline void SetRndMin(ywb_uint32_t val);
    inline void SetRndReadMax(ywb_uint32_t val);
    inline void SetRndReadMin(ywb_uint32_t val);
    inline void SetRndWriteMax(ywb_uint32_t val);
    inline void SetRndWriteMin(ywb_uint32_t val);
    inline void SetSeqMax(ywb_uint32_t val);
    inline void SetSeqMin(ywb_uint32_t val);
    inline void SetSeqReadMax(ywb_uint32_t val);
    inline void SetSeqReadMin(ywb_uint32_t val);
    inline void SetSeqWriteMax(ywb_uint32_t val);
    inline void SetSeqWriteMin(ywb_uint32_t val);
    inline void SetRndSeqMax(ywb_uint32_t val);
    inline void SetRndSeqMin(ywb_uint32_t val);

    void Reset();

private:
    /*minimum random read IO/BW/RESPTIME*/
    ywb_uint32_t mRndReadMin;
    /*maximum random read IO/BW/RESPTIME*/
    ywb_uint32_t mRndReadMax;
    /*minimum sequential read IO/BW/RESPTIME*/
    ywb_uint32_t mSeqReadMin;
    /*maximum sequential read IO/BW/RESPTIME*/
    ywb_uint32_t mSeqReadMax;
    /*minimum random write IO/BW/RESPTIME*/
    ywb_uint32_t mRndWriteMin;
    /*maximum random write IO/BW/RESPTIME*/
    ywb_uint32_t mRndWriteMax;
    /*minimum sequential write IO/BW/RESPTIME*/
    ywb_uint32_t mSeqWriteMin;
    /*maximum sequential write IO/BW/RESPTIME*/
    ywb_uint32_t mSeqWriteMax;
    /*minimum random IO/BW/RESPTIME*/
    ywb_uint32_t mRndMin;
    /*maximum random IO/BW/RESPTIME*/
    ywb_uint32_t mRndMax;
    /*minimum sequential IO/BW/RESPTIME*/
    ywb_uint32_t mSeqMin;
    /*maximum sequential IO/BW/RESPTIME*/
    ywb_uint32_t mSeqMax;
    /*minimum random+sequential IO/BW/RESPTIME*/
    ywb_uint32_t mRndSeqMin;
    /*maximum random+sequential IO/BW/RESPTIME*/
    ywb_uint32_t mRndSeqMax;
};

class DiskIO {
public:
    DiskIO()
    {
        mDisk = NULL;
    }

    ~DiskIO()
    {
        mDisk = NULL;
    }

    inline void GetStatIOs(IOStat::Type type, DiskIOStat** stat);
    inline void GetStatBW(IOStat::Type type, DiskIOStat** stat);
    inline void GetStatRT(IOStat::Type type, DiskIOStat** stat);

    inline void GetIOsRange(IOStat::Type type, DiskIOStatRange** range);
    inline void GetBWRange(IOStat::Type type, DiskIOStatRange** range);
    inline void GetRTRange(IOStat::Type type, DiskIOStatRange** range);

    inline void SetDisk(Disk* disk);

    /*update disk's RAW/EMA IO by plus/minus OBJ's RAW/EMA IO*/
    void UpdateStatIOs(ywb_uint32_t type, OBJIO* obj, bool plus);
    /*update disk's RAW/EMA BW by plus/minus OBJ's RAW/EMA BW*/
    void UpdateStatBW(ywb_uint32_t type, OBJIO* obj, bool plus);
    /*update disk's RAW/EMA RT by plus/minus OBJ's RAW/EMA RT*/
    void UpdateStatRT(ywb_uint32_t type, OBJIO* obj, bool plus);
    /*update disk's RAW/EMA Min/Max IOs, will not consider OBJ deletion*/
    void UpdateIOsRange(ywb_uint32_t type, OBJIO* obj);
    /*update disk's RAW/EMA Min/Max BW, will not consider OBJ deletion*/
    void UpdateBWRange(ywb_uint32_t type, OBJIO* obj);
    /*update disk's RAW/EMA Min/Max BW, will not consider OBJ deletion*/
    void UpdateRTRange(ywb_uint32_t type, OBJIO* obj);

    void Reset();

private:
    /*RAW/EMA random/sequential read/write IO*/
    DiskIOStat mStatIOs[Constant::IO_STAT_TYPE_CNT];

    /*RAW/EMA random/sequential read/write BW*/
    DiskIOStat mStatBW[Constant::IO_STAT_TYPE_CNT];

    /*
     * RAW/EMA random/sequential RESP Time
     * currently only take following into consideration:
     *      total random response time
     *      total sequential response time
     **/
    DiskIOStat mStatRT[Constant::IO_STAT_TYPE_CNT];

    /*
     * random/sequential IO range
     * array size is Constant::IO_STAT_TYPE_CNT, one for RAW
     * IO and the others for EMA IO
     * currently only take the following into consideration:
     *      minimum random read IOs
     *      maximum random read IOs
     *      minimum sequential read IOs
     *      maximum sequential read IOs
     *      minimum random write IOs
     *      maximum random write IOs
     *      minimum sequential write IOs
     *      maximum sequential write IOs
     *      minimum random IOs
     *      maximum random IOs
     *      minimum sequential IOs
     *      maximum sequential IOs
     *      minimum random+sequential IOS
     *      maximum random+sequential IOS
     **/
    DiskIOStatRange mIOsRange[Constant::IO_STAT_TYPE_CNT];

    /*
     * random/sequential BW range
     * array size is Constant::IO_STAT_TYPE_CNT, one for RAW
     * IO and the others for EMA IO
     * currently only take the following into consideration:
     *      minimum random BW
     *      maximum random BW
     *      minimum sequential BW
     *      maximum sequential BW
     *      minimum random+sequential BW
     *      maximum random+sequential BW
     **/
    DiskIOStatRange mBWRange[Constant::IO_STAT_TYPE_CNT];

    /*
     * random/sequential IO range
     * array size is Constant::EMA_CNT + 1, one for RAW IO,
     * and the others for EMA IO
     * currently only take the following into consideration:
     *      minimum random response time
     *      maximum random response time
     *      minimum sequential response time
     *      maximum sequential response time
     *      minimum random+sequential response time
     *      maximum random+sequential response time
     **/
    DiskIOStatRange mRTRange[Constant::IO_STAT_TYPE_CNT];

    /*back reference to disk*/
    Disk* mDisk;
};

class CollectOBJRequest;
class CollectOBJResponse;
class IOManager{
public:
    /*
     * put definition of constructor and de-constructor in .cpp
     * to avoid cross reference issue
     **/
    IOManager(AST* ast);
    virtual ~IOManager();

    inline void GetAST(AST** ast);
    inline void IncRef();
    inline void DecRef();
//    inline void SetCollectOBJResponse(CollectOBJResponse* response);

    /*prepare for collecting OBJ IO*/
    virtual ywb_status_t CollectOBJIOPrep(ywb_uint32_t ossid);
    /*collect OBJ RAW IO in last cycle from OSS*/
    virtual ywb_status_t CollectOBJIO(ywb_uint32_t ossid);
    /*do any work necessary post collect OBJ IO*/
    virtual ywb_status_t CollectOBJIOPost(ywb_uint32_t ossid);
    /*
     * resolve the collected OBJ RAW IO, calculate OBJ EMA IO,
     * disk RAW IO and disk EMA IO, add OBJIO and DiskIO into
     * system IO management
     **/
    virtual ywb_status_t ResolveOBJIO(CollectOBJResponse* resp);
    /*summarize disk IO from all its belongings*/
    ywb_status_t SummarizeDiskIO();

    ywb_status_t AddOBJIO(OBJKey&, OBJIO*& objio);
    ywb_status_t AddDiskIO(DiskKey&, DiskIO*& diskio);

    /*get OBJ IO*/
    ywb_status_t GetOBJIO(OBJKey& key, OBJIO** objio);
    /*get Disk IO*/
    ywb_status_t GetDiskIO(DiskKey& key, DiskIO** diskio);

    /*reset OBJ IO management to newly constructed state*/
    ywb_status_t Reset();
    /*
     *
     * (1) delete OBJIO/DiskIO corresponding to those OBJs/Disks
     * in deleted set, but currently OBJIO and DiskIO will be
     * deleted as the deletion of OBJ and Disk in Consolidate()
     * of logical configuration.
     *
     * (2) Summarize all disks' IO.
     **/
    ywb_status_t Consolidate();

private:
    /*
     * collect OBJ IO preparation request, constructed internally,
     * will not be responded explicitly
     * */
    CollectOBJRequest* mCollectOBJPrepReq;
    /*collect OBJ IO request, constructed internally*/
    CollectOBJRequest* mCollectOBJReq;
    /*
     * collect OBJ IO post-work request, constructed internally,
     * will not be responded explicitly
     * */
    CollectOBJRequest* mCollectOBJPostReq;
//    /*collect OBJ IO response, set externally*/
//    CollectOBJResponse* mCollectOBJResp;

    /*reference count*/
    fs_atomic_t mRef;
    /*back reference to AST*/
    AST* mAst;
};

#include "AST/ASTIOInline.hpp"

#endif

/* vim:set ts=4 sw=4 expandtab */
