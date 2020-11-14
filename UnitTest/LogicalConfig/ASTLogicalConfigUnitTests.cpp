#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/LogicalConfig/ASTLogicalConfigUnitTests.hpp"

void DiskTest::SetUp()
{
    ywb_uint32_t loop = 0;
    OBJKey objkey;
    ChunkIOStat rawstat;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;

    for(loop = 0; loop < 6; loop++)
    {
        objkey.SetStorageId(Constant::DISK_ID);
        objkey.SetInodeId(Constant::DEFAULT_INODE + loop);
//        objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
//        objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
//        objkey.SetChunkVersion(Constant::DEFAULT_VERSION);

        obj = new OBJ(objkey);
        objio = new OBJIO();

        rawstat.SetRndReadIOs(loop);
        rawstat.SetRndReadBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndWriteIOs(loop);
        rawstat.SetRndWriteBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndTotalRT((loop * (Constant::META_RT_READ)) +
                (loop * (Constant::META_RT_WRITE)));
        rawstat.SetSeqReadIOs(loop);
        rawstat.SetSeqReadBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqWriteIOs(loop);
        rawstat.SetSeqWriteBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqTotalRT((loop * (Constant::META_RT_READ)) +
                (loop * (Constant::META_RT_WRITE)));

        objio->SetRawIO(rawstat);
        objio->SetOBJ(obj);
        obj->SetOBJIO(objio);

        mDisk->AddOBJ(objkey, obj);
    }
}

void DiskTest::TearDown()
{

}

Disk* DiskTest::GetDisk()
{
    return mDisk;
}

ywb_status_t
DiskTest::GetStat(IOStat::Type stattype,
        Workload::Characteristics workload,
        ywb_uint32_t* rndread, ywb_uint32_t* seqread,
        ywb_uint32_t* rndwrite, ywb_uint32_t* seqwrite,
        ywb_uint32_t* read, ywb_uint32_t* write,
        ywb_uint32_t* rnd, ywb_uint32_t* seq)
{
    Disk* disk = NULL;
    DiskIO* diskio = NULL;
    DiskIOStat* stat = NULL;

    if((stattype < IOStat::IOST_raw) || (stattype > IOStat::IOST_long_term_ema))
    {
        return YWB_EINVAL;
    }

    if((workload < Workload::WC_io) || (workload > Workload::WC_rt))
    {
        return YWB_EINVAL;
    }

    disk = GetDisk();
    disk->GetDiskIO(&diskio);
    YWB_ASSERT(diskio != NULL);

    if(workload == Workload::WC_bw)
    {
        diskio->GetStatBW(stattype, &stat);
    }
    else if(workload == Workload::WC_io)
    {
        diskio->GetStatIOs(stattype, &stat);
    }
    else if(workload == Workload::WC_rt)
    {
        diskio->GetStatRT(stattype, &stat);
    }

    *rndread = stat->GetRndReadStat();
    *seqread = stat->GetSeqReadStat();
    *rndwrite = stat->GetRndWriteStat();
    *seqwrite = stat->GetSeqWriteStat();
    *read = stat->GetReadStat();
    *write = stat->GetWriteStat();
    *rnd = stat->GetRndStat();
    *seq = stat->GetSeqStat();

    return YWB_SUCCESS;
}

ywb_bool_t
DiskTest::StatMatch(IOStat::Type stattype,
        Workload::Characteristics workload)
{
    ywb_uint32_t rndread = 0;
    ywb_uint32_t seqread = 0;
    ywb_uint32_t rndwrite = 0;
    ywb_uint32_t seqwrite = 0;
    ywb_uint32_t read = 0;
    ywb_uint32_t write = 0;
    ywb_uint32_t rnd = 0;
    ywb_uint32_t seq = 0;

    YWB_ASSERT(stattype < IOStat::IOST_cnt);
    YWB_ASSERT(workload < Workload::WC_cnt);

    GetStat(stattype, workload, &rndread, &seqread, &rndwrite,
            &seqwrite, &read, &write, &rnd, &seq);

    if(IOStat::IOST_raw == stattype)
    {
        if(Workload::WC_io == workload)
        {
            YWB_ASSERT(rndread == 15);
            YWB_ASSERT(seqread == 15);
            YWB_ASSERT(rndwrite == 15);
            YWB_ASSERT(seqwrite == 15);
            YWB_ASSERT(read == 30);
            YWB_ASSERT(write == 30);
            YWB_ASSERT(rnd == 30);
            YWB_ASSERT(seq == 30);
        }
        else if(Workload::WC_bw == workload)
        {
            YWB_ASSERT(rndread == 60);
            YWB_ASSERT(seqread == 60);
            YWB_ASSERT(rndwrite == 60);
            YWB_ASSERT(seqwrite == 60);
            YWB_ASSERT(read == 120);
            YWB_ASSERT(write == 120);
            YWB_ASSERT(rnd == 120);
            YWB_ASSERT(seq = 120);
        }
        else if(Workload::WC_rt == workload)
        {
            YWB_ASSERT(rndread == 0);
            YWB_ASSERT(seqread == 0);
            YWB_ASSERT(rndwrite == 0);
            YWB_ASSERT(seqwrite == 0);
            YWB_ASSERT(read == 0);
            YWB_ASSERT(write == 0);
            YWB_ASSERT(rnd == 75);
            YWB_ASSERT(seq == 75);
        }
    }
    else if(IOStat::IOST_short_term_ema == stattype)
    {

    }
    else if(IOStat::IOST_long_term_ema == stattype)
    {

    }

    return true;
}

ywb_status_t
DiskTest::GetRange(IOStat::Type stattype,
        Workload::Characteristics workload,
        ywb_uint32_t* rndmin, ywb_uint32_t* rndmax,
        ywb_uint32_t* rndreadmin, ywb_uint32_t* rndreadmax,
        ywb_uint32_t* rndwritemin, ywb_uint32_t* rndwritemax,
        ywb_uint32_t* seqmin, ywb_uint32_t* seqmax,
        ywb_uint32_t* seqreadmin, ywb_uint32_t* seqreadmax,
        ywb_uint32_t* seqwritemin, ywb_uint32_t* seqwritemax,
        ywb_uint32_t* rndseqmin, ywb_uint32_t* rndseqmax)
{
    Disk* disk = NULL;
    DiskIO* diskio = NULL;
    DiskIOStatRange* range = NULL;

    if((stattype < IOStat::IOST_raw) || (stattype > IOStat::IOST_long_term_ema))
    {
        return YWB_EINVAL;
    }

    if((workload < Workload::WC_io) || (workload > Workload::WC_rt))
    {
        return YWB_EINVAL;
    }

    disk = GetDisk();
    disk->GetDiskIO(&diskio);
    YWB_ASSERT(diskio != NULL);

    if(workload == Workload::WC_io)
    {
        diskio->GetIOsRange(stattype, &range);
    }
    else if(workload == Workload::WC_bw)
    {
        diskio->GetBWRange(stattype, &range);
    }
    else if(workload == Workload::WC_rt)
    {
        diskio->GetRTRange(stattype, &range);
    }

    *rndmax = range->GetRndMax();
    *rndmin = range->GetRndMin();
    *rndreadmax = range->GetRndReadMax();
    *rndreadmin = range->GetRndReadMin();
    *rndwritemax = range->GetRndWriteMax();
    *rndwritemin = range->GetRndWriteMin();
    *seqmax = range->GetSeqMax();
    *seqmin = range->GetSeqMin();
    *seqreadmax = range->GetSeqReadMax();
    *seqreadmin = range->GetSeqReadMin();
    *seqwritemax = range->GetSeqWriteMax();
    *seqwritemin = range->GetSeqWriteMin();
    *rndseqmax = range->GetRndSeqMax();
    *rndseqmin = range->GetRndSeqMin();

    return YWB_SUCCESS;
}

ywb_bool_t
DiskTest::RangeMatch(IOStat::Type stattype,
        Workload::Characteristics workload)
{
    ywb_uint32_t rndmin = 0;
    ywb_uint32_t rndmax = 0;
    ywb_uint32_t rndreadmin = 0;
    ywb_uint32_t rndreadmax = 0;
    ywb_uint32_t rndwritemin = 0;
    ywb_uint32_t rndwritemax = 0;
    ywb_uint32_t seqmin = 0;
    ywb_uint32_t seqmax = 0;
    ywb_uint32_t seqreadmin = 0;
    ywb_uint32_t seqreadmax = 0;
    ywb_uint32_t seqwritemin = 0;
    ywb_uint32_t seqwritemax = 0;
    ywb_uint32_t rndseqmin = 0;
    ywb_uint32_t rndseqmax = 0;

    YWB_ASSERT(stattype < IOStat::IOST_cnt);
    YWB_ASSERT(workload < Workload::WC_cnt);

    GetRange(stattype, workload, &rndmin, &rndmax,
            &rndreadmin, &rndreadmax, &rndwritemin, &rndwritemax,
            &seqmin, &seqmax, &seqreadmin, &seqreadmax,
            &seqwritemin, &seqwritemax, &rndseqmin, &rndseqmax);

    if(IOStat::IOST_raw == stattype)
    {
        if(Workload::WC_io == workload)
        {
            YWB_ASSERT(rndmin == 0);
            YWB_ASSERT(rndmax == 10);
            YWB_ASSERT(rndreadmin == 0);
            YWB_ASSERT(rndreadmax == 5);
            YWB_ASSERT(rndwritemin == 0);
            YWB_ASSERT(rndwritemax == 5);
            YWB_ASSERT(seqmin == 0);
            YWB_ASSERT(seqmax == 10);
            YWB_ASSERT(seqreadmin == 0);
            YWB_ASSERT(seqreadmax == 5);
            YWB_ASSERT(seqwritemin == 0);
            YWB_ASSERT(seqwritemax == 5);
            YWB_ASSERT(rndseqmin == 0);
            YWB_ASSERT(rndseqmax == 20);
        }
        else if(Workload::WC_bw == workload)
        {
            YWB_ASSERT(rndmin == 0);
            YWB_ASSERT(rndmax == 40);
            YWB_ASSERT(rndreadmin == 0);
            YWB_ASSERT(rndreadmax == 0);
            YWB_ASSERT(rndwritemin == 0);
            YWB_ASSERT(rndwritemax == 0);
            YWB_ASSERT(seqmin == 0);
            YWB_ASSERT(seqmax == 40);
            YWB_ASSERT(seqreadmin == 0);
            YWB_ASSERT(seqreadmax == 0);
            YWB_ASSERT(seqwritemin == 0);
            YWB_ASSERT(seqwritemax == 0);
            YWB_ASSERT(rndseqmin == 0);
            YWB_ASSERT(rndseqmax == 80);
        }
        else if(Workload::WC_rt == workload)
        {
            YWB_ASSERT(rndmin == 0);
            YWB_ASSERT(rndmax == 25);
            YWB_ASSERT(rndreadmin == 0);
            YWB_ASSERT(rndreadmax == 0);
            YWB_ASSERT(rndwritemin == 0);
            YWB_ASSERT(rndwritemax == 0);
            YWB_ASSERT(seqmin == 0);
            YWB_ASSERT(seqmax == 25);
            YWB_ASSERT(seqreadmin == 0);
            YWB_ASSERT(seqreadmax == 0);
            YWB_ASSERT(seqwritemin == 0);
            YWB_ASSERT(seqwritemax == 0);
            YWB_ASSERT(rndseqmin == 0);
            YWB_ASSERT(rndseqmax == 50);
        }
    }
    else if(IOStat::IOST_short_term_ema == stattype)
    {

    }
    else if(IOStat::IOST_long_term_ema == stattype)
    {

    }

    return true;
}

void TierTest::SetUp()
{

}

void TierTest::TearDown()
{

}

Tier* TierTest::GetTier()
{
    return mTier;
}

void TierTest::SetUpByOurselves()
{
    ywb_uint32_t diskindex = 0;
    ywb_uint32_t objindex = 0;
    DiskKey diskkey;
    OBJKey objkey;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    for(diskindex = 0; diskindex < Constant::DEFAULT_DISK_NUM_PER_TIER; diskindex++)
    {
        disk = new Disk();
        diskkey.SetDiskId(Constant::DISK_ID + diskindex);
        diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

        for(objindex = 0; objindex < 6; objindex++)
        {
            objkey.SetStorageId(diskkey.GetDiskId());
            objkey.SetInodeId(Constant::DEFAULT_INODE + objindex);
            objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX + objindex);
            objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
            objkey.SetChunkVersion(Constant::DEFAULT_VERSION);
            obj = new OBJ(objkey);
            disk->AddOBJ(objkey, obj);
        }

        mTier->AddDisk(diskkey, disk);
    }
}

ywb_uint32_t
TierTest::AddDisks(ywb_uint32_t subpoolid,
        ywb_uint64_t firstdiskid, ywb_uint32_t disknum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t loop = 0;
    ywb_uint32_t addeddisks = 0;
    DiskKey diskkey;
    Tier* tier = NULL;
    Disk* disk = NULL;

    tier = mTier;
    for(loop = 0; loop < disknum; loop++)
    {
        diskkey.SetDiskId(firstdiskid + loop);
        diskkey.SetSubPoolId(subpoolid);
        disk = new Disk();
        ret = tier->AddDisk(diskkey, disk);
        if(YWB_SUCCESS == ret)
        {
            addeddisks++;
        }
    }

    return addeddisks;
}

ywb_uint32_t
TierTest::AddOBJs(ywb_uint32_t subpoolid, ywb_uint64_t diskid,
        ywb_uint64_t firstinodeid, ywb_uint32_t objnum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t loop = 0;
    ywb_uint32_t addedobjs = 0;
    ChunkIOStat rawstat;
    DiskKey diskkey;
    OBJKey objkey;
    Tier* tier = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;

    tier = mTier;
    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);
    ret = tier->GetDisk(diskkey, &disk);
    YWB_ASSERT(YWB_SUCCESS == ret);
    YWB_ASSERT(disk != NULL);

    for(loop = 0; loop < objnum; loop++)
    {
        objkey.SetStorageId(diskid);
        objkey.SetInodeId(firstinodeid + loop);

        obj = new OBJ(objkey);
        objio = new OBJIO();
        if(!obj || !objio)
        {
            break;
        }

        rawstat.SetRndReadIOs(loop);
        rawstat.SetRndReadBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndWriteIOs(loop);
        rawstat.SetRndWriteBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndTotalRT((loop * (Constant::META_RT_READ)) +
                (loop * (Constant::META_RT_WRITE)));
        rawstat.SetSeqReadIOs(loop);
        rawstat.SetSeqReadBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqWriteIOs(loop);
        rawstat.SetSeqWriteBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqTotalRT((loop * (Constant::META_RT_READ)) +
                (loop * (Constant::META_RT_WRITE)));

        objio->SetRawIO(rawstat);
        objio->SetOBJ(obj);
        obj->SetOBJIO(objio);

        ret = disk->AddOBJ(objkey, obj);
        if(YWB_SUCCESS == ret)
        {
            addedobjs++;
        }
    }

    return addedobjs;
}

void SubPoolTest::SetUp()
{

}

void SubPoolTest::TearDown()
{

}

SubPool* SubPoolTest::GetSubPool()
{
    return mSubPool;
}

ywb_status_t
SubPoolTest::AddDisk(ywb_uint32_t diskclass,
        ywb_uint64_t diskid, ywb_uint32_t subpoolid)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;

    subpool = mSubPool;
    diskprop.SetDiskClass(diskclass);
    diskprop.SetDiskRPM(10000);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);

    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);

    disk = new Disk(diskprop);
    ret = subpool->AddDisk(diskkey, disk);
    YWB_ASSERT(YWB_SUCCESS == ret);

    return ret;
}

ywb_uint32_t
SubPoolTest::AddOBJs(ywb_uint64_t diskid, ywb_uint32_t subpoolid,
        ywb_uint64_t firstinodeid, ywb_uint32_t objnum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t loop = 0;
    ywb_uint32_t addedobjs = 0;
    ChunkIOStat rawstat;
    DiskKey diskkey;
    OBJKey objkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;

    subpool = mSubPool;
    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);
    ret = subpool->GetDisk(diskkey, &disk);
    YWB_ASSERT(YWB_SUCCESS == ret);
    YWB_ASSERT(disk != NULL);

    for(loop = 0; loop < objnum; loop++)
    {
        objkey.SetStorageId(diskid);
        objkey.SetInodeId(firstinodeid + loop);

        obj = new OBJ(objkey);
        objio = new OBJIO();
        if(!obj || !objio)
        {
            break;
        }

        rawstat.SetRndReadIOs(loop);
        rawstat.SetRndReadBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndWriteIOs(loop);
        rawstat.SetRndWriteBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndTotalRT((loop * (Constant::META_RT_READ)) +
                (loop * (Constant::META_RT_WRITE)));
        rawstat.SetSeqReadIOs(loop);
        rawstat.SetSeqReadBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqWriteIOs(loop);
        rawstat.SetSeqWriteBW(loop * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqTotalRT((loop * (Constant::META_RT_READ)) +
                (loop * (Constant::META_RT_WRITE)));

        objio->SetRawIO(rawstat);
        objio->SetOBJ(obj);
        obj->SetOBJIO(objio);

        ret = subpool->AddOBJ(objkey, obj);
        if(YWB_SUCCESS == ret)
        {
            addedobjs++;
        }
    }

    return addedobjs;
}

ywb_status_t
SubPoolTest::GetStat(ywb_uint64_t diskid,
        ywb_uint32_t subpoolid, IOStat::Type stattype,
        Workload::Characteristics workload,
        ywb_uint32_t* rndread, ywb_uint32_t* seqread,
        ywb_uint32_t* rndwrite, ywb_uint32_t* seqwrite,
        ywb_uint32_t* read, ywb_uint32_t* write,
        ywb_uint32_t* rnd, ywb_uint32_t* seq)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    DiskIO* diskio = NULL;
    DiskIOStat* stat = NULL;

    if((stattype < IOStat::IOST_raw) || (stattype > IOStat::IOST_long_term_ema))
    {
        return YWB_EINVAL;
    }

    if((workload < Workload::WC_io) || (workload > Workload::WC_rt))
    {
        return YWB_EINVAL;
    }

    subpool = mSubPool;
    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);
    ret = subpool->GetDisk(diskkey, &disk);
    YWB_ASSERT(YWB_SUCCESS == ret);
    YWB_ASSERT(disk != NULL);
    disk->GetDiskIO(&diskio);
    YWB_ASSERT(diskio != NULL);

    if(workload == Workload::WC_bw)
    {
        diskio->GetStatBW(stattype, &stat);
    }
    else if(workload == Workload::WC_io)
    {
        diskio->GetStatIOs(stattype, &stat);
    }
    else if(workload == Workload::WC_rt)
    {
        diskio->GetStatRT(stattype, &stat);
    }

    *rndread = stat->GetRndReadStat();
    *seqread = stat->GetSeqReadStat();
    *rndwrite = stat->GetRndWriteStat();
    *seqwrite = stat->GetSeqWriteStat();
    *read = stat->GetReadStat();
    *write = stat->GetWriteStat();
    *rnd = stat->GetRndStat();
    *seq = stat->GetSeqStat();

    return YWB_SUCCESS;
}

ywb_bool_t
SubPoolTest::StatMatch(ywb_uint64_t diskid,
        ywb_uint32_t subpoolid, IOStat::Type stattype,
        Workload::Characteristics workload)
{
    ywb_uint32_t rndread = 0;
    ywb_uint32_t seqread = 0;
    ywb_uint32_t rndwrite = 0;
    ywb_uint32_t seqwrite = 0;
    ywb_uint32_t read = 0;
    ywb_uint32_t write = 0;
    ywb_uint32_t rnd = 0;
    ywb_uint32_t seq = 0;

    YWB_ASSERT(stattype < IOStat::IOST_cnt);
    YWB_ASSERT(workload < Workload::WC_cnt);

    GetStat(diskid, subpoolid, stattype, workload,
            &rndread, &seqread, &rndwrite,
            &seqwrite, &read, &write, &rnd, &seq);

    if(IOStat::IOST_raw == stattype)
    {
        if(Workload::WC_io == workload)
        {
            YWB_ASSERT(rndread == 15);
            YWB_ASSERT(seqread == 15);
            YWB_ASSERT(rndwrite == 15);
            YWB_ASSERT(seqwrite == 15);
            YWB_ASSERT(read == 30);
            YWB_ASSERT(write == 30);
            YWB_ASSERT(rnd == 30);
            YWB_ASSERT(seq == 30);
        }
        else if(Workload::WC_bw == workload)
        {
            YWB_ASSERT(rndread == 60);
            YWB_ASSERT(seqread == 60);
            YWB_ASSERT(rndwrite == 60);
            YWB_ASSERT(seqwrite == 60);
            YWB_ASSERT(read == 120);
            YWB_ASSERT(write == 120);
            YWB_ASSERT(rnd == 120);
            YWB_ASSERT(seq == 120);
        }
        else if(Workload::WC_rt == workload)
        {
            YWB_ASSERT(rndread == 0);
            YWB_ASSERT(seqread == 0);
            YWB_ASSERT(rndwrite == 0);
            YWB_ASSERT(seqwrite == 0);
            YWB_ASSERT(read == 0);
            YWB_ASSERT(write == 0);
            YWB_ASSERT(rnd == 75);
            YWB_ASSERT(seq == 75);
        }
    }
    else if(IOStat::IOST_short_term_ema == stattype)
    {

    }
    else if(IOStat::IOST_long_term_ema == stattype)
    {

    }

    return true;
}

LogicalConfig* LogicalConfigTest::GetLogicalConfig()
{
    return mLogicalConfig;
}

ywb_status_t LogicalConfigTest::AddSubPool(ywb_uint32_t ossid,
        ywb_uint32_t poolid, ywb_uint32_t subpoolid)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;

    config = GetLogicalConfig();
    subpoolkey.SetOss(ossid);
    subpoolkey.SetPoolId(poolid);
    subpoolkey.SetSubPoolId(subpoolid);

    subpool = new SubPool(subpoolkey);
    ret = config->AddSubPool(subpoolkey, subpool);
    if(YWB_SUCCESS != ret)
    {
        delete subpool;
        subpool = NULL;
    }

    return ret;
}

ywb_uint32_t
LogicalConfigTest::AddSubPools(ywb_uint32_t ossid, ywb_uint32_t poolid,
        ywb_uint32_t firstsubpoolid, ywb_uint32_t subpoolnum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t loop = 0;
    ywb_uint32_t addedsubpools = 0;

    for(loop = 0; loop < subpoolnum; loop++)
    {
        ret = AddSubPool(ossid, poolid, firstsubpoolid + loop);
        if(YWB_SUCCESS == ret)
        {
            addedsubpools++;
        }
    }

    return addedsubpools;
}

ywb_status_t LogicalConfigTest::AddDisk(ywb_uint32_t diskclass,
        ywb_uint64_t diskid, ywb_uint32_t subpoolid)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    LogicalConfig* config = NULL;
    Disk* disk = NULL;

    config = GetLogicalConfig();
    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);
    diskprop.SetDiskClass(diskclass);
    diskprop.SetDiskRPM(10000);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);

    disk = new Disk(diskprop);
    ret = config->AddDisk(diskkey, disk);
    if(YWB_SUCCESS != ret)
    {
        delete disk;
        disk = NULL;
    }

    return ret;
}

ywb_uint32_t
LogicalConfigTest::AddDisks(ywb_uint32_t diskclass, ywb_uint64_t firstdiskid,
        ywb_uint32_t subpoolid, ywb_uint32_t disknum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t loop = 0;
    ywb_uint32_t addeddisks = 0;

    for(loop = 0; loop < disknum; loop++)
    {
        ret = AddDisk(diskclass, firstdiskid + loop, subpoolid);
        if(YWB_SUCCESS == ret)
        {
            addeddisks++;
        }
    }

    return addeddisks;
}

/*return number of successfully added OBJs*/
ywb_uint32_t LogicalConfigTest::AddOBJs(ywb_uint64_t diskid,
        ywb_uint64_t firstinodeid, ywb_uint32_t objnum)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t objindex = 0;
    ywb_uint32_t objssuccess = 0;
    ChunkIOStat rawstat;
    OBJKey objkey;
    LogicalConfig* config = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;

    config = GetLogicalConfig();
    for(objindex = 0; objindex < objnum; objindex++)
    {
        objkey.SetStorageId(diskid);
        objkey.SetInodeId(firstinodeid + objindex);

        obj = new OBJ(objkey);
        objio = new OBJIO();
        if(!obj || !objio)
        {
            break;
        }

        rawstat.SetRndReadIOs(objindex);
        rawstat.SetRndReadBW(objindex * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndWriteIOs(objindex);
        rawstat.SetRndWriteBW(objindex * (Constant::DEFAULT_IOSIZE));
        rawstat.SetRndTotalRT((objindex * (Constant::META_RT_READ)) +
                (objindex * (Constant::META_RT_WRITE)));
        rawstat.SetSeqReadIOs(objindex);
        rawstat.SetSeqReadBW(objindex * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqWriteIOs(objindex);
        rawstat.SetSeqWriteBW(objindex * (Constant::DEFAULT_IOSIZE));
        rawstat.SetSeqTotalRT((objindex * (Constant::META_RT_READ)) +
                (objindex * (Constant::META_RT_WRITE)));

        objio->SetRawIO(rawstat);
        objio->SetOBJ(obj);
        obj->SetOBJIO(objio);

        ret = config->AddOBJ(objkey, obj);
        if(YWB_SUCCESS == ret)
        {
            objssuccess++;
        }
        else
        {
            if(obj != NULL)
            {
                delete obj;
                obj = NULL;
            }

            /*@objio will be destroyed as @obj*/
        }
    }

    return objssuccess;
}

ywb_bool_t
LogicalConfigTest::AddOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    ywb_uint32_t addednum = 0;

    addednum = AddOBJs(diskid, inodeid, 1);
    if(1 == addednum)
    {
        return true;
    }
    else
    {
        return false;
    }
}

SubPool* LogicalConfigTest::GetSubPool(ywb_uint32_t ossid,
        ywb_uint32_t poolid, ywb_uint32_t subpoolid)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;

    config = GetLogicalConfig();
    subpoolkey.SetOss(ossid);
    subpoolkey.SetPoolId(poolid);
    subpoolkey.SetSubPoolId(subpoolid);

    ret = config->GetSubPool(subpoolkey, &subpool);
    if(YWB_SUCCESS == ret)
    {
        return subpool;
    }
    else
    {
        return NULL;
    }
}

Disk* LogicalConfigTest::GetDisk(
        ywb_uint64_t diskid, ywb_uint32_t subpoolid)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    LogicalConfig* config = NULL;
    Disk* disk = NULL;

    config = GetLogicalConfig();
    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);

    ret = config->GetDisk(diskkey, &disk);
    if(YWB_SUCCESS == ret)
    {
        return disk;
    }
    else
    {
        return NULL;
    }
}

OBJ* LogicalConfigTest::GetOBJ(
        ywb_uint64_t diskid,ywb_uint64_t inodeid)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    LogicalConfig* config = NULL;
    OBJ* obj = NULL;

    config = GetLogicalConfig();
    objkey.SetStorageId(diskid);
    objkey.SetInodeId(inodeid);

    ret = config->GetOBJ(objkey, &obj);
    if(YWB_SUCCESS == ret)
    {
        return obj;
    }
    else
    {
        return NULL;
    }
}

ywb_status_t LogicalConfigTest::RemoveSubPool(ywb_uint32_t ossid,
        ywb_uint32_t poolid, ywb_uint32_t subpoolid)
{
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    subpoolkey.SetOss(ossid);
    subpoolkey.SetPoolId(poolid);
    subpoolkey.SetSubPoolId(subpoolid);

    return config->RemoveSubPool(subpoolkey);
}

ywb_status_t
LogicalConfigTest::RemoveDisk(ywb_uint64_t diskid, ywb_uint32_t subpoolid)
{
    DiskKey diskkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);

    return config->RemoveDisk(diskkey);
}

ywb_status_t
LogicalConfigTest::RemoveOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    OBJKey objkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    objkey.SetStorageId(diskid);
    objkey.SetInodeId(inodeid);

    return config->RemoveOBJ(objkey);
}


ywb_status_t
LogicalConfigTest::AddDeletedSubPool(ywb_uint32_t ossid,
        ywb_uint32_t poolid, ywb_uint32_t subpoolid)
{
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    subpoolkey.SetOss(ossid);
    subpoolkey.SetPoolId(poolid);
    subpoolkey.SetSubPoolId(subpoolid);

    return config->AddDeletedSubPool(subpoolkey);
}

ywb_status_t
LogicalConfigTest::AddDeletedDisk(ywb_uint64_t diskid, ywb_uint32_t subpoolid)
{
    DiskKey diskkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);

    return config->AddDeletedDisk(diskkey);
}

ywb_status_t LogicalConfigTest::AddDeletedOBJ(
        ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    OBJKey objkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    objkey.SetStorageId(diskid);
    objkey.SetInodeId(inodeid);

    return config->AddDeletedOBJ(objkey);
}

ywb_bool_t LogicalConfigTest::CheckSubPoolDeleted(ywb_uint32_t ossid,
        ywb_uint32_t poolid, ywb_uint32_t subpoolid)
{
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    subpoolkey.SetOss(ossid);
    subpoolkey.SetPoolId(poolid);
    subpoolkey.SetSubPoolId(subpoolid);

    return config->CheckSubPoolDeleted(subpoolkey);
}

ywb_bool_t LogicalConfigTest::CheckDiskDeleted(
        ywb_uint64_t diskid, ywb_uint32_t subpoolid)
{
    DiskKey diskkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);

    return config->CheckDiskDeleted(diskkey);
}

ywb_bool_t LogicalConfigTest::CheckOBJDeleted(
        ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    OBJKey objkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    objkey.SetStorageId(diskid);
    objkey.SetInodeId(inodeid);

    return config->CheckOBJDeleted(objkey);
}

ywb_bool_t LogicalConfigTest::CheckSubPoolExistence(ywb_uint32_t ossid,
        ywb_uint32_t poolid, ywb_uint32_t subpoolid)
{
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    subpoolkey.SetOss(ossid);
    subpoolkey.SetPoolId(poolid);
    subpoolkey.SetSubPoolId(subpoolid);

    return config->CheckSubPoolExistence(subpoolkey);
}

ywb_bool_t LogicalConfigTest::CheckDiskExistence(
        ywb_uint64_t diskid, ywb_uint32_t subpoolid)
{
    DiskKey diskkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    diskkey.SetDiskId(diskid);
    diskkey.SetSubPoolId(subpoolid);

    return config->CheckDiskExistence(diskkey);
}

ywb_bool_t LogicalConfigTest::CheckOBJExistence(
        ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    OBJKey objkey;
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    objkey.SetStorageId(diskid);
    objkey.SetInodeId(inodeid);

    return config->CheckOBJExistence(objkey);
}

void LogicalConfigTest::PrepareResolveLogicalConfigEnv()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};

    config = GetLogicalConfig();
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        if(2 != subpoolloop)
        {
            subpoolkey.SetSubPoolId(subpoolid);
            subpool = new SubPool(subpoolkey);
            YWB_ASSERT(subpool != NULL);
            subpool->SetFlagComb(SubPool::SPF_suspicious);
            config->AddSubPool(subpoolkey, subpool);

            diskprop.SetDiskRPM(10000);
            diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
            diskprop.SetRaidWidth(1);
            diskprop.SetDiskCap(1280000);
            diskkey.SetSubPoolId(subpoolid);
            for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
            {
                if(2 != diskloop)
                {
                    diskprop.SetDiskClass(diskclass[diskloop % ((sizeof(diskclass) / sizeof(ywb_uint32_t)))]);
                    diskkey.SetDiskId(diskid);
                    disk = new Disk(diskprop);
                    YWB_ASSERT(disk != NULL);
                    disk->SetFlagComb(Disk::DF_suspicious);
                    config->AddDisk(diskkey, disk);
                }
            }
        }
    }
}

void LogicalConfigTest::BuildCollectLogicalConfigResponse()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    DiskInfo diskinfo;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        if(1 != subpoolloop)
        {
            subpoolkey.SetSubPoolId(subpoolid);
            mCollectResp->AddSubPool(subpoolkey);

            diskprop.SetDiskRPM(10000);
            diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
            diskprop.SetRaidWidth(1);
            diskprop.SetDiskCap(1280000);
            diskkey.SetSubPoolId(subpoolid);
            for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
            {
                if(1 != diskloop)
                {
                    diskprop.SetDiskClass(diskclass[diskloop % ((sizeof(diskclass) / sizeof(ywb_uint32_t)))]);
                    diskkey.SetDiskId(diskid);
                    diskinfo.SetDiskKey(diskkey);
                    diskinfo.SetBaseProp(diskprop);
                    mCollectResp->AddDisk(diskinfo);
                }
            }

            /*the corresponding subpool not exist*/
            diskkey.SetSubPoolId(subpoolid + 100);
            diskkey.SetDiskId(diskid + 1000);
            diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
            diskinfo.SetDiskKey(diskkey);
            diskinfo.SetBaseProp(diskprop);
            mCollectResp->AddDisk(diskinfo);
        }
        else
        {
            /*report disks but do not report their corresponding subpool*/
            diskprop.SetDiskRPM(10000);
            diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
            diskprop.SetRaidWidth(1);
            diskprop.SetDiskCap(1280000);
            diskkey.SetSubPoolId(subpoolid);
            for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
            {
                if(1 != diskloop)
                {
                    diskprop.SetDiskClass(diskclass[diskloop % ((sizeof(diskclass) / sizeof(ywb_uint32_t)))]);
                    diskkey.SetDiskId(diskid);
                    diskinfo.SetDiskKey(diskkey);
                    diskinfo.SetBaseProp(diskprop);
                    mCollectResp->AddDisk(diskinfo);
                }
            }
        }
    }
}

ywb_status_t
LogicalConfigTest::ResolveLogicalConfig()
{
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    return config->ResolveLogicalConfig(mCollectResp);
}

ywb_status_t
LogicalConfigTest::CheckResolveLogicalConfigResult()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    DiskInfo diskinfo;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;

    config = GetLogicalConfig();
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = config->GetSubPool(subpoolkey, &subpool);
        YWB_ASSERT((YWB_SUCCESS == ret) && (subpool != NULL));

        if(0 == subpoolloop)
        {
            YWB_ASSERT(false == subpool->TestFlag(SubPool::SPF_suspicious));
            diskkey.SetSubPoolId(subpoolid);
            for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
            {
                diskkey.SetDiskId(diskid);
                ret = config->GetDisk(diskkey, &disk);
                YWB_ASSERT((YWB_SUCCESS == ret) && (disk != NULL));

                if(0 == diskloop)
                {
                    YWB_ASSERT(false == disk->TestFlag(Disk::DF_suspicious));
                }
                else if(1 == diskloop)
                {
                    YWB_ASSERT(true == disk->TestFlag(Disk::DF_suspicious));
                }
            }

            diskkey.SetSubPoolId(subpoolid + 100);
            diskkey.SetDiskId(diskid + 1000);
            ret = config->GetDisk(diskkey, &disk);
            YWB_ASSERT(YWB_SUCCESS != ret);
        }
        else if(1 == subpoolloop)
        {
            YWB_ASSERT(true == subpool->TestFlag(SubPool::SPF_suspicious));
            diskkey.SetSubPoolId(subpoolid);
            for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
            {
                if((0 == diskloop) || (1 == diskloop))
                {
                    diskkey.SetDiskId(diskid);
                    ret = config->GetDisk(diskkey, &disk);
                    YWB_ASSERT((YWB_SUCCESS == ret) && (disk != NULL));
                    YWB_ASSERT(true == disk->TestFlag(Disk::DF_suspicious));
                }
                else if(2 == diskloop)
                {
                    diskkey.SetDiskId(diskid);
                    ret = config->GetDisk(diskkey, &disk);
                    YWB_ASSERT((YWB_SUCCESS != ret));
                }
            }
        }
        else if(2 == subpoolloop)
        {
            diskkey.SetSubPoolId(subpoolid);
            for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
            {
                if((0 == diskloop) || (2 == diskloop))
                {
                    diskkey.SetDiskId(diskid);
                    ret = config->GetDisk(diskkey, &disk);
                    YWB_ASSERT((YWB_SUCCESS == ret) && (disk != NULL));
                }
            }

            diskkey.SetSubPoolId(subpoolid + 100);
            diskkey.SetDiskId(diskid + 1000);
            ret = config->GetDisk(diskkey, &disk);
            YWB_ASSERT(YWB_SUCCESS != ret);
        }
    }

    return YWB_SUCCESS;
}

void LogicalConfigTest::PrepareLogicalConfigForDeleteMessage()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};

    config = GetLogicalConfig();
    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        subpool = new SubPool(subpoolkey);
        YWB_ASSERT(subpool != NULL);
        config->AddSubPool(subpoolkey, subpool);

        diskprop.SetDiskRPM(10000);
        diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
        diskprop.SetRaidWidth(1);
        diskprop.SetDiskCap(1280000);
        diskkey.SetSubPoolId(subpoolid);
        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            diskprop.SetDiskClass(diskclass[diskloop % ((sizeof(diskclass) / sizeof(ywb_uint32_t)))]);
            diskkey.SetDiskId(diskid);
            disk = new Disk(diskprop);
            YWB_ASSERT(disk != NULL);
            config->AddDisk(diskkey, disk);

            objkey.SetStorageId(diskid);
            objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
            objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
            objkey.SetChunkVersion(1);
            for(objloop = 0; objloop < 6; objloop++, inodeid++)
            {
                objkey.SetInodeId(inodeid);
                obj = new OBJ(objkey);
                YWB_ASSERT(obj != NULL);
                config->AddOBJ(objkey, obj);
            }
        }
    }
}

void LogicalConfigTest::BuildReportDeleteOBJMessage()
{
    ywb_uint32_t objloop = 0;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;

    inodeid = Constant::DEFAULT_INODE;
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);
    for(objloop = 0; objloop < 6; objloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        mDeletedOBJMsg->AddOBJ(objkey);
    }
}

ywb_status_t
LogicalConfigTest::ResolveDeletedOBJs()
{
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    return config->ResolveDeletedOBJs(mDeletedOBJMsg);
}

ywb_status_t
LogicalConfigTest::CheckResolveDeletedOBJsResult()
{
    ywb_bool_t deleted = false;
    ywb_uint32_t objloop = 0;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;

    inodeid = Constant::DEFAULT_INODE;
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);
    for(objloop = 0; objloop < 6; objloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        deleted = mLogicalConfig->CheckOBJDeleted(objkey);
        YWB_ASSERT(true == deleted);
    }

    return YWB_SUCCESS;
}

void LogicalConfigTest::BuildReportDeleteDiskMessage()
{
    ywb_uint32_t diskloop = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;

    diskid = Constant::DISK_ID;
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
    {
        diskkey.SetDiskId(diskid);
        mDeletedDiskMsg->AddDisk(diskkey);
    }
}

ywb_status_t
LogicalConfigTest::ResolveDeletedDisks()
{
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    return config->ResolveDeletedDisks(mDeletedDiskMsg);
}

ywb_status_t
LogicalConfigTest::CheckResolveDeletedDisksResult()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t deletedobjs = 0;
    ywb_bool_t deleted = false;
    ywb_uint32_t diskloop = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;
    OBJKey objkey;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    diskid = Constant::DISK_ID;
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
    {
        diskkey.SetDiskId(diskid);
        ret = mLogicalConfig->GetDisk(diskkey, &disk);
        YWB_ASSERT((YWB_SUCCESS == ret) && (disk != NULL));
        deleted = mLogicalConfig->CheckDiskDeleted(diskkey);
        YWB_ASSERT(true == deleted);

        deletedobjs = 0;
        ret = disk->GetFirstOBJ(&obj, &objkey);
        while((YWB_SUCCESS == ret) && (obj != NULL))
        {
            deleted = mLogicalConfig->CheckOBJDeleted(objkey);
            YWB_ASSERT(true == deleted);
            deletedobjs++;
            ret = disk->GetNextOBJ(&obj, &objkey);
        }

        YWB_ASSERT(6 == deletedobjs);
    }

    return YWB_SUCCESS;
}

void LogicalConfigTest::BuildReportDeleteSubPoolMessage()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;

    subpoolid = Constant::SUBPOOL_ID;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(subpoolid + subpoolloop);
        mDeletedSubPoolMsg->AddSubPool(subpoolkey);
    }
}

ywb_status_t
LogicalConfigTest::ResolveDeletedSubPools()
{
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    return config->ResolveDeletedSubPools(mDeletedSubPoolMsg);
}

ywb_status_t
LogicalConfigTest::CheckResolveDeletedSubPoolsResult()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t deleted = false;
    ywb_uint32_t deletedobjs = 0;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    OBJKey objkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = mLogicalConfig->GetSubPool(subpoolkey, &subpool);
        YWB_ASSERT((YWB_SUCCESS == ret) && (subpool != NULL));
        deleted = mLogicalConfig->CheckSubPoolDeleted(subpoolkey);
        YWB_ASSERT(true == deleted);

        diskkey.SetSubPoolId(subpoolid);
        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            diskkey.SetDiskId(diskid);
            ret = mLogicalConfig->GetDisk(diskkey, &disk);
            YWB_ASSERT((YWB_SUCCESS == ret) && (disk != NULL));
            deleted = mLogicalConfig->CheckDiskDeleted(diskkey);
            YWB_ASSERT(true == deleted);

            deletedobjs = 0;
            ret = disk->GetFirstOBJ(&obj, &objkey);
            while((YWB_SUCCESS == ret) && (obj != NULL))
            {
                deleted = mLogicalConfig->CheckOBJDeleted(objkey);
                YWB_ASSERT(true == deleted);
                deletedobjs++;
                ret = disk->GetNextOBJ(&obj, &objkey);
            }

            YWB_ASSERT(6 == deletedobjs);
        }
    }

    return YWB_SUCCESS;
}

int main(int argc, char *argv[])
{
    int ret = 0;
    std::string logfile =
            YfsDirectorySetting::GetLogDirectory() + Constant::LOG_PATH;

    char ch;
    bool daemon = false;
    bool ignore = false;
    opterr = 0;

    while ((ch = getopt(argc, argv, "gd")) != -1) {
        if (ch == '?') {
            ignore = true;
            continue;
        }
        daemon = true;
    }

    if (daemon && !ignore) {
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }

    create_logger(logfile, "trace");

    testing::AddGlobalTestEnvironment(new ASTLogicalConfigTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
