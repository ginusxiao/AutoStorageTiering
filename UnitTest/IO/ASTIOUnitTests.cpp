#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/IO/ASTIOUnitTests.hpp"

void ASTOBJIOTest::SetUp()
{
    ChunkIOStat rawstat;
    OBJIO* objio = NULL;

    objio = mObjIO;
    rawstat.SetRndReadIOs(10);
    rawstat.SetRndReadBW(10 * (Constant::DEFAULT_IOSIZE));
    rawstat.SetRndWriteIOs(10);
    rawstat.SetRndWriteBW(10 * (Constant::DEFAULT_IOSIZE));
    rawstat.SetRndTotalRT((10 * (Constant::META_RT_READ)) +
            (10 * (Constant::META_RT_WRITE)));
    rawstat.SetSeqReadIOs(10);
    rawstat.SetSeqReadBW(10 * (Constant::DEFAULT_IOSIZE));
    rawstat.SetSeqWriteIOs(10);
    rawstat.SetSeqWriteBW(10 * (Constant::DEFAULT_IOSIZE));
    rawstat.SetSeqTotalRT((10 * (Constant::META_RT_READ)) +
            (10 * (Constant::META_RT_WRITE)));
    objio->SetRawIO(rawstat);
}

void ASTOBJIOTest::TearDown()
{

}

OBJIO* ASTOBJIOTest::GetOBJIO()
{
    return mObjIO;
}

void ASTDiskIOTest::SetUp()
{
    ywb_uint32_t loop = 0;
    ChunkIOStat rawstat;

    for(loop = 0; loop < 6; loop++)
    {
        mObjIO[loop] = new OBJIO();
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

        mObjIO[loop]->SetRawIO(rawstat);
    }
}

void ASTDiskIOTest::TearDown()
{

}

DiskIO* ASTDiskIOTest::GetDiskIO()
{
    return mDiskIO;
}

OBJIO* ASTDiskIOTest::GetOBJIO(ywb_uint32_t objindex)
{
    return mObjIO[objindex];
}

void ASTIOManagerTest::SetUp()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
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
    OBJIO* objio = NULL;
    ChunkIOStat rawstat;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};

    config = GetLogicalConfig();
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        subpool = new SubPool(subpoolkey);
        YWB_ASSERT(subpool != NULL);
        config->AddSubPool(subpoolkey, subpool);

        diskprop.SetDiskRPM(10000);
        diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
        diskprop.SetRaidWidth(1);
        diskprop.SetDiskCap(1280000);
        diskkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            diskprop.SetDiskClass(diskclass[diskloop % ((sizeof(diskclass) / sizeof(ywb_uint32_t)))]);
            diskkey.SetDiskId(diskid);
            disk = new Disk(diskprop);
            YWB_ASSERT(disk != NULL);
            config->AddDisk(diskkey, disk);

            objkey.SetStorageId(diskid);
            for(objloop = 0; objloop < 6; objloop++, inodeid++)
            {
                rawstat.SetRndReadIOs(objloop);
                rawstat.SetRndReadBW(objloop * (Constant::DEFAULT_IOSIZE));
                rawstat.SetRndWriteIOs(objloop);
                rawstat.SetRndWriteBW(objloop * (Constant::DEFAULT_IOSIZE));
                rawstat.SetRndTotalRT((objloop * (Constant::META_RT_READ)) +
                        (objloop * (Constant::META_RT_WRITE)));
                rawstat.SetSeqReadIOs(objloop);
                rawstat.SetSeqReadBW(objloop * (Constant::DEFAULT_IOSIZE));
                rawstat.SetSeqWriteIOs(objloop);
                rawstat.SetSeqWriteBW(objloop * (Constant::DEFAULT_IOSIZE));
                rawstat.SetSeqTotalRT((objloop * (Constant::META_RT_READ)) +
                        (objloop * (Constant::META_RT_WRITE)));

                objkey.SetInodeId(inodeid);
                obj = new OBJ(objkey);
                YWB_ASSERT(obj != NULL);
                objio = new OBJIO();
                YWB_ASSERT(objio != NULL);
                objio->SetRawIO(rawstat);
                objio->SetOBJ(obj);
                obj->SetOBJIO(objio);
                config->AddOBJ(objkey, obj);
            }
        }
    }
}

void ASTIOManagerTest::TearDown()
{

}

LogicalConfig* ASTIOManagerTest::GetLogicalConfig()
{
    LogicalConfig* config = NULL;

    mAst->GetLogicalConfig(&config);
    return config;
}

IOManager* ASTIOManagerTest::GetIOManager()
{
    IOManager* iomgr = NULL;

    mAst->GetIOManager(&iomgr);
    return iomgr;
}

DecisionWindowManager*
ASTIOManagerTest::GetDecisionWindowManager(ywb_uint32_t windowindex)
{
    TimeManager* timemgr = NULL;
    DecisionWindowManager* windowmgr = NULL;

    mAst->GetTimeManager(&timemgr);
    timemgr->GetDecisionWindow(windowindex, &windowmgr);
    return windowmgr;
}

ywb_status_t
ASTIOManagerTest::GetStat(DiskIO* diskio, IOStat::Type stattype,
        Workload::Characteristics workload,
        ywb_uint32_t* rndread, ywb_uint32_t* seqread,
        ywb_uint32_t* rndwrite, ywb_uint32_t* seqwrite,
        ywb_uint32_t* read, ywb_uint32_t* write,
        ywb_uint32_t* rnd, ywb_uint32_t* seq)
{
    DiskIOStat* stat = NULL;

    YWB_ASSERT(diskio != NULL);
    if((stattype < IOStat::IOST_raw) || (stattype > IOStat::IOST_long_term_ema))
    {
        return YWB_EINVAL;
    }

    if((workload < Workload::WC_io) || (workload > Workload::WC_rt))
    {
        return YWB_EINVAL;
    }

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
ASTIOManagerTest::StatMatch(DiskIO* diskio,
       IOStat::Type stattype, Workload::Characteristics workload)
{
    ywb_uint32_t rndread = 0;
    ywb_uint32_t seqread = 0;
    ywb_uint32_t rndwrite = 0;
    ywb_uint32_t seqwrite = 0;
    ywb_uint32_t read = 0;
    ywb_uint32_t write = 0;
    ywb_uint32_t rnd = 0;
    ywb_uint32_t seq = 0;

    YWB_ASSERT(diskio != NULL);
    YWB_ASSERT(stattype < IOStat::IOST_cnt);
    YWB_ASSERT(workload < Workload::WC_cnt);

    GetStat(diskio, stattype, workload, &rndread, &seqread,
            &rndwrite, &seqwrite, &read, &write, &rnd, &seq);

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

ywb_status_t ASTIOManagerTest::AddOBJ(
        ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    LogicalConfig* config = NULL;
    OBJ* obj = NULL;

    config = GetLogicalConfig();
    objkey.SetStorageId(diskid);
    objkey.SetInodeId(inodeid);
    obj = new OBJ(objkey);

    ret = config->AddOBJ(objkey, obj);
    if(YWB_SUCCESS != ret)
    {
        delete obj;
        obj = NULL;
    }

    return ret;
}

ywb_status_t ASTIOManagerTest::AddDisk(ywb_uint32_t diskclass,
        ywb_uint64_t diskid, ywb_uint32_t subpoolid)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    LogicalConfig* config = NULL;
    Disk* disk = NULL;

    config = GetLogicalConfig();
    diskprop.SetDiskClass(diskclass);
    diskprop.SetDiskRPM(10000);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);

    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(diskid);
    disk = new Disk(diskprop);
    YWB_ASSERT(disk != NULL);
    ret = config->AddDisk(diskkey, disk);
    if(YWB_SUCCESS != ret)
    {
        delete disk;
        disk = NULL;
    }

    return ret;
}

ywb_status_t ASTIOManagerTest::AddOBJIO(
        ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    IOManager* iomgr = NULL;
    OBJIO* objio = NULL;

    iomgr = GetIOManager();
    objkey.SetStorageId(diskid);
    objkey.SetInodeId(inodeid);
    objio = new OBJIO();
    YWB_ASSERT(objio != NULL);
    ret = iomgr->AddOBJIO(objkey, objio);
    if(YWB_SUCCESS != ret)
    {
        delete objio;
        objio = NULL;
    }

    return ret;
}

ywb_status_t ASTIOManagerTest::AddDiskIO(
        ywb_uint64_t diskid, ywb_uint32_t subpoolid)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    IOManager* iomgr = NULL;
    DiskIO* diskio = NULL;

    iomgr = GetIOManager();
    diskkey.SetSubPoolId(subpoolid);
    diskkey.SetDiskId(diskid);
    diskio = new DiskIO();
    YWB_ASSERT(diskio != NULL);
    ret = iomgr->AddDiskIO(diskkey, diskio);
    if(YWB_SUCCESS != ret)
    {
        delete diskio;
        diskio = NULL;
    }

    return ret;
}

void ASTIOManagerTest::PrepareResolveOBJIOEnv()
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
    DecisionWindowManager* shortwindowmgr = NULL;
    DecisionWindowManager* longwindowmgr = NULL;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ChunkIOStat rawstat;
    ywb_uint32_t diskclass[3] = {DiskBaseProp::DCT_ssd,
            DiskBaseProp::DCT_ent, DiskBaseProp::DCT_sata};
    ywb_uint32_t cycleshares[Constant::CYCLE_TYPE_CNT] = {0, 0};

    shortwindowmgr = GetDecisionWindowManager(DecisionWindowManager::DW_short_term);
    longwindowmgr = GetDecisionWindowManager(DecisionWindowManager::DW_long_term);
    config = GetLogicalConfig();

    cycleshares[0] = 11;
    cycleshares[1] = 1;
    shortwindowmgr->SetCycles(12);
    shortwindowmgr->SetCycleShare(cycleshares, Constant::CYCLE_TYPE_CNT);
    cycleshares[0] = 25;
    cycleshares[1] = 1;
    longwindowmgr->SetCycles(26);
    longwindowmgr->SetCycleShare(cycleshares, Constant::CYCLE_TYPE_CNT);

    subpoolid = Constant::SUBPOOL_ID + 100;
    diskid = Constant::DISK_ID + 1000;
    inodeid = Constant::DEFAULT_INODE + 10000;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
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

            /*Set OBJIO for all OBJs belongs to this disk*/
            for(objloop = 0; objloop < 2; objloop++, inodeid++)
            {
                if(0 == diskloop)
                {
                    /*Set OBJIO for all OBJs belongs to this disk*/
                    rawstat.SetRndReadIOs(objloop);
                    rawstat.SetRndReadBW(objloop * (Constant::DEFAULT_IOSIZE));
                    rawstat.SetRndWriteIOs(objloop);
                    rawstat.SetRndWriteBW(objloop * (Constant::DEFAULT_IOSIZE));
                    rawstat.SetRndTotalRT((objloop * (Constant::META_RT_READ)) +
                            (objloop * (Constant::META_RT_WRITE)));
                    rawstat.SetSeqReadIOs(objloop);
                    rawstat.SetSeqReadBW(objloop * (Constant::DEFAULT_IOSIZE));
                    rawstat.SetSeqWriteIOs(objloop);
                    rawstat.SetSeqWriteBW(objloop * (Constant::DEFAULT_IOSIZE));
                    rawstat.SetSeqTotalRT((objloop * (Constant::META_RT_READ)) +
                            (objloop * (Constant::META_RT_WRITE)));

                    objkey.SetInodeId(inodeid);
                    obj = new OBJ(objkey);
                    YWB_ASSERT(obj != NULL);
                    objio = new OBJIO();
                    YWB_ASSERT(objio != NULL);
                    objio->SetRawIO(rawstat);
                    objio->SetOBJ(obj);
                    obj->SetOBJIO(objio);
                    obj->SetFlagComb(OBJ::OF_suspicious);
                    config->AddOBJ(objkey, obj);
                }
                else if(1 == diskloop)
                {
                    /*Don't set OBJIO for any OBJs belongs to this disk*/
                    objkey.SetInodeId(inodeid);
                    obj = new OBJ(objkey);
                    YWB_ASSERT(obj != NULL);
                    obj->SetFlagComb(OBJ::OF_suspicious);
                    config->AddOBJ(objkey, obj);
                }
                else
                {
                    /*No OBJs on this disk*/
                }
            }
        }
    }
}

void ASTIOManagerTest::BuildCollectOBJResponse()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;
    OBJInfo objinfo;
    ChunkIOStat rawstat;
    CollectOBJResponseFragment* ioresponsefrag = NULL;

    /*keep this consistent with PrepareResolveOBJIOEnv*/
    diskid = Constant::DISK_ID + 1000;
    inodeid = Constant::DEFAULT_INODE + 10000;
    mCollectOBJRespFrag[0]->Reset();
    mCollectOBJRespFrag[1]->Reset();
    mCollectOBJResp->Reset();
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++)
    {
        if(subpoolloop < 2)
        {
            ioresponsefrag = mCollectOBJRespFrag[0];
        }
        else
        {
            ioresponsefrag = mCollectOBJRespFrag[1];
        }

        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            objkey.SetStorageId(diskid);
            objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
            objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
            objkey.SetChunkVersion(1);

            for(objloop = 0; objloop < 2; objloop++, inodeid++)
            {
                if(0 == objloop)
                {
                    rawstat.SetRndReadIOs(2 * objloop);
                    rawstat.SetRndReadBW(2 * objloop * (Constant::DEFAULT_IOSIZE));
                    rawstat.SetRndWriteIOs(2 * objloop);
                    rawstat.SetRndWriteBW(2 * objloop * (Constant::DEFAULT_IOSIZE));
                    rawstat.SetRndTotalRT((2 * objloop * (Constant::META_RT_READ)) +
                            (2 * objloop * (Constant::META_RT_WRITE)));
                    rawstat.SetSeqReadIOs(2 * objloop);
                    rawstat.SetSeqReadBW(2 * objloop * (Constant::DEFAULT_IOSIZE));
                    rawstat.SetSeqWriteIOs(2 * objloop);
                    rawstat.SetSeqWriteBW(2 * objloop * (Constant::DEFAULT_IOSIZE));
                    rawstat.SetSeqTotalRT((2 * objloop * (Constant::META_RT_READ)) +
                            (2 * objloop * (Constant::META_RT_WRITE)));

                    objkey.SetInodeId(inodeid);
                    objinfo.SetOBJKey(objkey);
                    objinfo.SetChunkIOStat(rawstat);
                    ioresponsefrag->AddOBJ(objinfo);
                }
            }
        }

        mCollectOBJRespFrag[0]->SetStatus(YWB_SUCCESS);
        mCollectOBJRespFrag[1]->SetStatus(YWB_SUCCESS);
        mCollectOBJResp->SetFragments(mCollectOBJRespFrag);
        mCollectOBJResp->SetFragmentNum(2);
        mCollectOBJResp->SetStatus(YWB_SUCCESS);
    }
}

void ASTIOManagerTest::BuildCollectOBJResponse2()
{
    mCollectOBJResp->SetStatus(YFS_ENODATA);
}

ywb_status_t ASTIOManagerTest::ResolveOBJIO()
{
    IOManager* iomgr = NULL;

    iomgr = GetIOManager();
    return iomgr->ResolveOBJIO(mCollectOBJResp);
}

ywb_status_t ASTIOManagerTest::CheckResolveOBJIOResult()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;
    LogicalConfig* config = NULL;
    IOManager* iomgr = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;

    config = GetLogicalConfig();
    iomgr = GetIOManager();
    /*keep this consistent with PrepareResolveOBJIOEnv*/
    diskid = Constant::DISK_ID + 1000;
    inodeid = Constant::DEFAULT_INODE + 10000;
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++)
    {
        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            objkey.SetStorageId(diskid);
            objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
            objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
            objkey.SetChunkVersion(1);

            for(objloop = 0; objloop < 2; objloop++, inodeid++)
            {
                if(0 == objloop)
                {
                    objkey.SetInodeId(inodeid);
                    ret = config->GetOBJ(objkey, &obj);
                    YWB_ASSERT(ret == YWB_SUCCESS);
                    ret = iomgr->GetOBJIO(objkey, &objio);
                    YWB_ASSERT(ret == YWB_SUCCESS);
                    YWB_ASSERT(objio != NULL);

                    if((0 == diskloop) || (1 == diskloop))
                    {
                        YWB_ASSERT(false == obj->TestFlag(OBJ::OF_suspicious));
                    }
                }
                else
                {
                    if(0 == diskloop)
                    {
                        objkey.SetInodeId(inodeid);
                        ret = config->GetOBJ(objkey, &obj);
                        YWB_ASSERT(ret == YWB_SUCCESS);
                        ret = iomgr->GetOBJIO(objkey, &objio);
                        YWB_ASSERT(ret == YWB_SUCCESS);
                        YWB_ASSERT(objio != NULL);
                        YWB_ASSERT(true == obj->TestFlag(OBJ::OF_suspicious));
                    }
                    else if(1 == diskloop)
                    {
                        objkey.SetInodeId(inodeid);
                        ret = config->GetOBJ(objkey, &obj);
                        YWB_ASSERT(ret == YWB_SUCCESS);
                        ret = iomgr->GetOBJIO(objkey, &objio);
                        YWB_ASSERT(ret == YWB_SUCCESS);
                        YWB_ASSERT(objio == NULL);
                        YWB_ASSERT(true == obj->TestFlag(OBJ::OF_suspicious));
                    }
                }
            }
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

    testing::AddGlobalTestEnvironment(new ASTIOTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
