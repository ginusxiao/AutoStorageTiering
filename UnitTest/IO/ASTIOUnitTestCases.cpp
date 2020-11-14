#include <stdio.h>
#include "FsInts.h"
#include "OSSCommon/Logger.hpp"
#include "UnitTest/AST/IO/ASTIOUnitTests.hpp"

TEST_F(ASTOBJIOTest, CalculateEMAIO_FirstEMA_SUCCESS)
{
    ywb_uint32_t shorttermcycles = 12;
    ywb_uint32_t multiplier1 = 0;
    ywb_uint32_t multiplier2 = 0;
    ywb_uint32_t divisor = 0;
    OBJIO* objio = NULL;
    ChunkIOStat* rawstat = NULL;
    ChunkIOStat* emastat = NULL;

    objio = GetOBJIO();
    divisor = ((shorttermcycles + 1) * (shorttermcycles + 1));
    multiplier1 = (divisor - 2 * (shorttermcycles + 1));
    multiplier2 = 2 * (shorttermcycles + 1);

    objio->GetIOStat(IOStat::IOST_raw, &rawstat);
    objio->CalculateEMAIO(IOStat::IOST_short_term_ema,
            multiplier1, multiplier2, divisor);
    objio->GetIOStat(IOStat::IOST_short_term_ema, &emastat);

    ASSERT_EQ(emastat->GetRndReadIOs(), 10);
    ASSERT_EQ(emastat->GetRndWriteIOs(), 10);
    ASSERT_EQ(emastat->GetRndTotalRT(), 50);
    ASSERT_EQ(emastat->GetRndReadBW(), 40);
    ASSERT_EQ(emastat->GetRndWriteBW(), 40);
    ASSERT_EQ(emastat->GetSeqReadIOs(), 10);
    ASSERT_EQ(emastat->GetSeqWriteIOs(), 10);
    ASSERT_EQ(emastat->GetSeqTotalRT(), 50);
    ASSERT_EQ(emastat->GetSeqReadBW(), 40);
    ASSERT_EQ(emastat->GetSeqWriteBW(), 40);

    ASSERT_EQ(rawstat->GetRndReadIOs(), emastat->GetRndReadIOs());
    ASSERT_EQ(rawstat->GetRndTotalRT(), emastat->GetRndTotalRT());
    ASSERT_EQ(rawstat->GetSeqWriteIOs(), emastat->GetSeqWriteIOs());
    ASSERT_EQ(rawstat->GetSeqWriteBW(), emastat->GetSeqWriteBW());
}

TEST_F(ASTOBJIOTest, CalculateEMAIO_NonFirstEMA_SUCCESS)
{
    ywb_uint32_t shorttermcycles = 12;
    ywb_uint32_t multiplier1 = 0;
    ywb_uint32_t multiplier2 = 0;
    ywb_uint32_t divisor = 0;
    OBJIO* objio = NULL;
    ChunkIOStat* rawstat = NULL;
    ChunkIOStat* emastat = NULL;

    objio = GetOBJIO();
    divisor = ((shorttermcycles + 1) * (shorttermcycles + 1));
    multiplier1 = (divisor - 2 * (shorttermcycles + 1));
    multiplier2 = 2 * (shorttermcycles + 1);

    objio->GetIOStat(IOStat::IOST_raw, &rawstat);
    objio->CalculateEMAIO(IOStat::IOST_short_term_ema,
            multiplier1, multiplier2, divisor);
    rawstat->SetRndReadIOs(20);
    rawstat->SetRndReadBW(20 * (Constant::DEFAULT_IOSIZE));
    rawstat->SetRndWriteIOs(20);
    rawstat->SetRndWriteBW(20 * (Constant::DEFAULT_IOSIZE));
    rawstat->SetRndTotalRT((20 * (Constant::META_RT_READ)) +
            (20 * (Constant::META_RT_WRITE)));
    rawstat->SetSeqReadIOs(20);
    rawstat->SetSeqReadBW(20 * (Constant::DEFAULT_IOSIZE));
    rawstat->SetSeqWriteIOs(20);
    rawstat->SetSeqWriteBW(20 * (Constant::DEFAULT_IOSIZE));
    rawstat->SetSeqTotalRT((20 * (Constant::META_RT_READ)) +
            (20 * (Constant::META_RT_WRITE)));

    objio->CalculateEMAIO(IOStat::IOST_short_term_ema,
            multiplier1, multiplier2, divisor);
    objio->GetIOStat(IOStat::IOST_short_term_ema, &emastat);

    ASSERT_EQ(emastat->GetRndReadIOs(), 11);
    ASSERT_EQ(emastat->GetRndWriteIOs(), 11);
    ASSERT_EQ(emastat->GetRndTotalRT(), 57);
    ASSERT_EQ(emastat->GetRndReadBW(), 46);
    ASSERT_EQ(emastat->GetRndWriteBW(), 46);
    ASSERT_EQ(emastat->GetSeqReadIOs(), 11);
    ASSERT_EQ(emastat->GetSeqWriteIOs(), 11);
    ASSERT_EQ(emastat->GetSeqTotalRT(), 57);
    ASSERT_EQ(emastat->GetSeqReadBW(), 46);
    ASSERT_EQ(emastat->GetSeqWriteBW(), 46);
}

TEST_F(ASTDiskIOTest, UpdateStatIOs_Plus_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStat* diskstat = NULL;
    ChunkIOStat* objstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(1);
    diskio->UpdateStatIOs(IOStat::IOST_raw, objio, true);
    diskio->GetStatIOs(IOStat::IOST_raw, &diskstat);
    objio->GetIOStat(IOStat::IOST_raw, &objstat);

    ASSERT_EQ(diskstat->GetRndReadStat(), objstat->GetRndReadIOs());
    ASSERT_EQ(diskstat->GetRndWriteStat(), objstat->GetRndWriteIOs());
    ASSERT_EQ(diskstat->GetSeqReadStat(), objstat->GetSeqReadIOs());
    ASSERT_EQ(diskstat->GetSeqWriteStat(), objstat->GetSeqWriteIOs());
    ASSERT_EQ(diskstat->GetReadStat(), objstat->GetRndReadIOs() + objstat->GetSeqReadIOs());
    ASSERT_EQ(diskstat->GetWriteStat(), objstat->GetRndWriteIOs() + objstat->GetSeqWriteIOs());
    ASSERT_EQ(diskstat->GetRndStat(), objstat->GetRndReadIOs() + objstat->GetRndWriteIOs());
    ASSERT_EQ(diskstat->GetSeqStat(), objstat->GetSeqReadIOs() + objstat->GetSeqWriteIOs());
}

TEST_F(ASTDiskIOTest, UpdateStatIOs_Minus_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStat* diskstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(2);
    diskio->UpdateStatIOs(IOStat::IOST_raw, objio, false);
    diskio->GetStatIOs(IOStat::IOST_raw, &diskstat);

    ASSERT_EQ(diskstat->GetRndReadStat(), 0);
    ASSERT_EQ(diskstat->GetRndWriteStat(), 0);
    ASSERT_EQ(diskstat->GetSeqReadStat(), 0);
    ASSERT_EQ(diskstat->GetSeqWriteStat(), 0);
    ASSERT_EQ(diskstat->GetReadStat(), 0);
    ASSERT_EQ(diskstat->GetWriteStat(), 0);
    ASSERT_EQ(diskstat->GetRndStat(), 0);
    ASSERT_EQ(diskstat->GetSeqStat(), 0);
}

TEST_F(ASTDiskIOTest, UpdateStatBW_Plus_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStat* diskstat = NULL;
    ChunkIOStat* objstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(1);
    diskio->UpdateStatBW(IOStat::IOST_raw, objio, true);
    diskio->GetStatBW(IOStat::IOST_raw, &diskstat);
    objio->GetIOStat(IOStat::IOST_raw, &objstat);

    ASSERT_EQ(diskstat->GetRndReadStat(), objstat->GetRndReadBW());
    ASSERT_EQ(diskstat->GetRndWriteStat(), objstat->GetRndWriteBW());
    ASSERT_EQ(diskstat->GetSeqReadStat(), objstat->GetSeqReadBW());
    ASSERT_EQ(diskstat->GetSeqWriteStat(), objstat->GetSeqWriteBW());
    ASSERT_EQ(diskstat->GetReadStat(), objstat->GetRndReadBW() + objstat->GetSeqReadBW());
    ASSERT_EQ(diskstat->GetWriteStat(), objstat->GetRndWriteBW() + objstat->GetSeqWriteBW());
    ASSERT_EQ(diskstat->GetRndStat(), objstat->GetRndReadBW() + objstat->GetRndWriteBW());
    ASSERT_EQ(diskstat->GetSeqStat(), objstat->GetSeqReadBW() + objstat->GetSeqWriteBW());
}

TEST_F(ASTDiskIOTest, UpdateStatBW_Minus_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStat* diskstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(2);
    diskio->UpdateStatBW(IOStat::IOST_raw, objio, false);
    diskio->GetStatBW(IOStat::IOST_raw, &diskstat);

    ASSERT_EQ(diskstat->GetRndReadStat(), 0);
    ASSERT_EQ(diskstat->GetRndWriteStat(), 0);
    ASSERT_EQ(diskstat->GetSeqReadStat(), 0);
    ASSERT_EQ(diskstat->GetSeqWriteStat(), 0);
    ASSERT_EQ(diskstat->GetReadStat(), 0);
    ASSERT_EQ(diskstat->GetWriteStat(), 0);
    ASSERT_EQ(diskstat->GetRndStat(), 0);
    ASSERT_EQ(diskstat->GetSeqStat(), 0);
}

TEST_F(ASTDiskIOTest, UpdateStatRT_Plus_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStat* diskstat = NULL;
    ChunkIOStat* objstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(1);
    diskio->UpdateStatRT(IOStat::IOST_raw, objio, true);
    diskio->GetStatRT(IOStat::IOST_raw, &diskstat);
    objio->GetIOStat(IOStat::IOST_raw, &objstat);

    ASSERT_EQ(diskstat->GetRndReadStat(), 0);
    ASSERT_EQ(diskstat->GetRndWriteStat(), 0);
    ASSERT_EQ(diskstat->GetSeqReadStat(), 0);
    ASSERT_EQ(diskstat->GetSeqWriteStat(), 0);
    ASSERT_EQ(diskstat->GetReadStat(), 0);
    ASSERT_EQ(diskstat->GetWriteStat(), 0);
    ASSERT_EQ(diskstat->GetRndStat(), objstat->GetRndTotalRT());
    ASSERT_EQ(diskstat->GetSeqStat(), objstat->GetSeqTotalRT());
}

TEST_F(ASTDiskIOTest, UpdateStatRT_Minus_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStat* diskstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(2);
    diskio->UpdateStatRT(IOStat::IOST_raw, objio, false);
    diskio->GetStatRT(IOStat::IOST_raw, &diskstat);

    ASSERT_EQ(diskstat->GetRndReadStat(), 0);
    ASSERT_EQ(diskstat->GetRndWriteStat(), 0);
    ASSERT_EQ(diskstat->GetSeqReadStat(), 0);
    ASSERT_EQ(diskstat->GetSeqWriteStat(), 0);
    ASSERT_EQ(diskstat->GetReadStat(), 0);
    ASSERT_EQ(diskstat->GetWriteStat(), 0);
    ASSERT_EQ(diskstat->GetRndStat(), 0);
    ASSERT_EQ(diskstat->GetSeqStat(), 0);
}

TEST_F(ASTDiskIOTest, UpdateIOsRange_NewValueOutOfRange_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStatRange* diskrange = NULL;
    ChunkIOStat* objstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(1);
    diskio->UpdateIOsRange(IOStat::IOST_raw, objio);
    diskio->GetIOsRange(IOStat::IOST_raw, &diskrange);
    objio->GetIOStat(IOStat::IOST_raw, &objstat);

    ASSERT_EQ(diskrange->GetRndReadMax(), objstat->GetRndReadIOs());
    ASSERT_EQ(diskrange->GetRndReadMin(), objstat->GetRndReadIOs());
    ASSERT_EQ(diskrange->GetRndWriteMax(), objstat->GetRndWriteIOs());
    ASSERT_EQ(diskrange->GetRndWriteMin(), objstat->GetRndWriteIOs());
    ASSERT_EQ(diskrange->GetSeqReadMax(), objstat->GetSeqReadIOs());
    ASSERT_EQ(diskrange->GetSeqReadMin(), objstat->GetSeqReadIOs());
    ASSERT_EQ(diskrange->GetSeqWriteMax(), objstat->GetSeqWriteIOs());
    ASSERT_EQ(diskrange->GetSeqWriteMin(), objstat->GetSeqWriteIOs());
    ASSERT_EQ(diskrange->GetRndMax(), objstat->GetRndReadIOs() + objstat->GetRndWriteIOs());
    ASSERT_EQ(diskrange->GetRndMin(), objstat->GetRndReadIOs() + objstat->GetRndWriteIOs());
    ASSERT_EQ(diskrange->GetSeqMax(), objstat->GetSeqReadIOs() + objstat->GetSeqWriteIOs());
    ASSERT_EQ(diskrange->GetSeqMin(), objstat->GetSeqReadIOs() + objstat->GetSeqWriteIOs());
    ASSERT_EQ(diskrange->GetRndSeqMax(), (objstat->GetRndReadIOs() +
            objstat->GetRndWriteIOs() + objstat->GetSeqReadIOs() + objstat->GetSeqWriteIOs()));
    ASSERT_EQ(diskrange->GetRndSeqMin(), (objstat->GetRndReadIOs() +
            objstat->GetRndWriteIOs() + objstat->GetSeqReadIOs() + objstat->GetSeqWriteIOs()));
}

TEST_F(ASTDiskIOTest, UpdateIOsRange_NewValueLessThanMin_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStatRange* prevdiskrange = NULL;
    DiskIOStatRange* curdiskrange = NULL;
    ChunkIOStat* objstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(4);
    diskio->UpdateIOsRange(IOStat::IOST_raw, objio);
    diskio->GetIOsRange(IOStat::IOST_raw, &prevdiskrange);
    objio = GetOBJIO(2);
    diskio->UpdateIOsRange(IOStat::IOST_raw, objio);
    diskio->GetIOsRange(IOStat::IOST_raw, &curdiskrange);
    objio->GetIOStat(IOStat::IOST_raw, &objstat);

    ASSERT_EQ(prevdiskrange->GetRndReadMax(), curdiskrange->GetRndReadMax());
    ASSERT_EQ(curdiskrange->GetRndReadMin(), objstat->GetRndReadIOs());
    ASSERT_EQ(prevdiskrange->GetRndWriteMax(), curdiskrange->GetRndWriteMax());
    ASSERT_EQ(curdiskrange->GetRndWriteMin(), objstat->GetRndWriteIOs());
    ASSERT_EQ(prevdiskrange->GetSeqReadMax(), curdiskrange->GetSeqReadMax());
    ASSERT_EQ(curdiskrange->GetSeqReadMin(), objstat->GetSeqReadIOs());
    ASSERT_EQ(prevdiskrange->GetSeqWriteMax(), curdiskrange->GetSeqWriteMax());
    ASSERT_EQ(curdiskrange->GetSeqWriteMin(), objstat->GetSeqWriteIOs());
    ASSERT_EQ(prevdiskrange->GetRndMax(), curdiskrange->GetRndMax());
    ASSERT_EQ(curdiskrange->GetRndMin(), objstat->GetRndReadIOs() + objstat->GetRndWriteIOs());
    ASSERT_EQ(prevdiskrange->GetSeqMax(), curdiskrange->GetSeqMax());
    ASSERT_EQ(curdiskrange->GetSeqMin(), objstat->GetSeqReadIOs() + objstat->GetSeqWriteIOs());
    ASSERT_EQ(prevdiskrange->GetRndSeqMax(), curdiskrange->GetRndSeqMax());
    ASSERT_EQ(curdiskrange->GetRndSeqMin(), (objstat->GetRndReadIOs() +
            objstat->GetRndWriteIOs() + objstat->GetSeqReadIOs() + objstat->GetSeqWriteIOs()));
}

TEST_F(ASTDiskIOTest, UpdateBWRange_NewValueOutOfRange_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStatRange* diskrange = NULL;
    ChunkIOStat* objstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(1);
    diskio->UpdateBWRange(IOStat::IOST_raw, objio);
    diskio->GetBWRange(IOStat::IOST_raw, &diskrange);
    objio->GetIOStat(IOStat::IOST_raw, &objstat);

    ASSERT_EQ(diskrange->GetRndMax(), objstat->GetRndReadBW() + objstat->GetRndWriteBW());
    ASSERT_EQ(diskrange->GetRndMin(), objstat->GetRndReadBW() + objstat->GetRndWriteBW());
    ASSERT_EQ(diskrange->GetSeqMax(), objstat->GetSeqReadBW() + objstat->GetSeqWriteBW());
    ASSERT_EQ(diskrange->GetSeqMin(), objstat->GetSeqReadBW() + objstat->GetSeqWriteBW());
    ASSERT_EQ(diskrange->GetRndSeqMax(), (objstat->GetRndReadBW() +
            objstat->GetRndWriteBW() + objstat->GetSeqReadBW() + objstat->GetSeqWriteBW()));
    ASSERT_EQ(diskrange->GetRndSeqMin(), (objstat->GetRndReadBW() +
            objstat->GetRndWriteBW() + objstat->GetSeqReadBW() + objstat->GetSeqWriteBW()));
}

TEST_F(ASTDiskIOTest, UpdateBWRange_NewValueLessThanMin_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStatRange* prevdiskrange = NULL;
    DiskIOStatRange* curdiskrange = NULL;
    ChunkIOStat* objstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(4);
    diskio->UpdateBWRange(IOStat::IOST_raw, objio);
    diskio->GetBWRange(IOStat::IOST_raw, &prevdiskrange);
    objio = GetOBJIO(2);
    diskio->UpdateBWRange(IOStat::IOST_raw, objio);
    diskio->GetBWRange(IOStat::IOST_raw, &curdiskrange);
    objio->GetIOStat(IOStat::IOST_raw, &objstat);

    ASSERT_EQ(prevdiskrange->GetRndMax(), curdiskrange->GetRndMax());
    ASSERT_EQ(curdiskrange->GetRndMin(), objstat->GetRndReadBW() + objstat->GetRndWriteBW());
    ASSERT_EQ(prevdiskrange->GetSeqMax(), curdiskrange->GetSeqMax());
    ASSERT_EQ(curdiskrange->GetSeqMin(), objstat->GetSeqReadBW() + objstat->GetSeqWriteBW());
    ASSERT_EQ(prevdiskrange->GetRndSeqMax(), curdiskrange->GetRndSeqMax());
    ASSERT_EQ(curdiskrange->GetRndSeqMin(), (objstat->GetRndReadBW() +
            objstat->GetRndWriteBW() + objstat->GetSeqReadBW() + objstat->GetSeqWriteBW()));
}

TEST_F(ASTDiskIOTest, UpdateRTRange_NewValueOutOfRange_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStatRange* diskrange = NULL;
    ChunkIOStat* objstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(1);
    diskio->UpdateRTRange(IOStat::IOST_raw, objio);
    diskio->GetRTRange(IOStat::IOST_raw, &diskrange);
    objio->GetIOStat(IOStat::IOST_raw, &objstat);

    ASSERT_EQ(diskrange->GetRndMax(), objstat->GetRndTotalRT());
    ASSERT_EQ(diskrange->GetRndMin(), objstat->GetRndTotalRT());
    ASSERT_EQ(diskrange->GetSeqMax(), objstat->GetSeqTotalRT());
    ASSERT_EQ(diskrange->GetSeqMin(), objstat->GetSeqTotalRT());
    ASSERT_EQ(diskrange->GetRndSeqMax(), objstat->GetRndTotalRT() + objstat->GetSeqTotalRT());
    ASSERT_EQ(diskrange->GetRndSeqMin(), objstat->GetRndTotalRT() + objstat->GetSeqTotalRT());
}

TEST_F(ASTDiskIOTest, UpdateRTRange_NewValueLessThanMin_SUCCESS)
{
    DiskIO* diskio = NULL;
    OBJIO* objio = NULL;
    DiskIOStatRange* prevdiskrange = NULL;
    DiskIOStatRange* curdiskrange = NULL;
    ChunkIOStat* objstat = NULL;

    diskio = GetDiskIO();
    objio = GetOBJIO(4);
    diskio->UpdateRTRange(IOStat::IOST_raw, objio);
    diskio->GetRTRange(IOStat::IOST_raw, &prevdiskrange);
    objio = GetOBJIO(2);
    diskio->UpdateRTRange(IOStat::IOST_raw, objio);
    diskio->GetRTRange(IOStat::IOST_raw, &curdiskrange);
    objio->GetIOStat(IOStat::IOST_raw, &objstat);

    ASSERT_EQ(prevdiskrange->GetRndMax(), curdiskrange->GetRndMax());
    ASSERT_EQ(curdiskrange->GetRndMin(), objstat->GetRndTotalRT());
    ASSERT_EQ(prevdiskrange->GetSeqMax(), curdiskrange->GetSeqMax());
    ASSERT_EQ(curdiskrange->GetSeqMin(), objstat->GetSeqTotalRT());
    ASSERT_EQ(prevdiskrange->GetRndSeqMax(), curdiskrange->GetRndSeqMax());
    ASSERT_EQ(curdiskrange->GetRndSeqMin(), objstat->GetRndTotalRT() + objstat->GetSeqTotalRT());
}

TEST_F(ASTIOManagerTest, SummarizeDiskIO_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    IOStat::Type stattype;
    Workload::Characteristics workload;
    ywb_uint64_t diskid = 0;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    DiskKey diskkey;
    IOManager* iomgr = NULL;
    DiskIO* diskio = NULL;

    iomgr = GetIOManager();
    diskid = Constant::DISK_ID;

    ret = iomgr->SummarizeDiskIO();
    ASSERT_EQ(ret, YWB_SUCCESS);

    for(subpoolloop = 0; subpoolloop < 5; subpoolloop++)
    {
        diskkey.SetSubPoolId(Constant::SUBPOOL_ID + subpoolloop);
        for(diskloop = 0; diskloop < 3; diskloop++, diskid++)
        {
            diskkey.SetDiskId(diskid);
            iomgr->GetDiskIO(diskkey, &diskio);

            stattype = IOStat::IOST_raw;
            workload = Workload::WC_io;
            match = StatMatch(diskio, stattype, workload);
            ASSERT_EQ(match, true);

            workload = Workload::WC_bw;
            match = StatMatch(diskio, stattype, workload);
            ASSERT_EQ(match, true);

            workload = Workload::WC_rt;
            match = StatMatch(diskio, stattype, workload);
            ASSERT_EQ(match, true);
        }
    }
}

TEST_F(ASTIOManagerTest, AddOBJIO_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    DiskKey diskkey;

    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE + 10000;
    ret = AddOBJ(diskid, inodeid);
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = AddOBJIO(diskid, inodeid);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ASTIOManagerTest, AddOBJIO_OBJNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;

    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE + 10000;
    ret = AddOBJIO(diskid, inodeid);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTIOManagerTest, AddDiskIO_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint32_t subpoolid = 0;

    diskid = Constant::DISK_ID + 1000;
    subpoolid = Constant::SUBPOOL_ID;
    ret = AddDisk(DiskBaseProp::DCT_ent, diskid, subpoolid);
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = AddDiskIO(diskid, subpoolid);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ASTIOManagerTest, AddDiskIO_DiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint32_t subpoolid = 0;

    diskid = Constant::DISK_ID + 1000;
    subpoolid = Constant::SUBPOOL_ID;
    ret = AddDiskIO(diskid, subpoolid);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTIOManagerTest, GetOBJIO_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    IOManager* iomgr = NULL;
    OBJIO* objio = NULL;

    iomgr = GetIOManager();
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    ret = iomgr->GetOBJIO(objkey, &objio);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ASTIOManagerTest, GetOBJIO_OBJNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    IOManager* iomgr = NULL;
    OBJIO* objio = NULL;

    iomgr = GetIOManager();
    objkey.SetStorageId(Constant::DISK_ID + 1000);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 10000);
    ret = iomgr->GetOBJIO(objkey, &objio);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTIOManagerTest, GetDiskIO_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    IOManager* iomgr = NULL;
    DiskIO* diskio = NULL;

    iomgr = GetIOManager();
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID);
    ret = iomgr->GetDiskIO(diskkey, &diskio);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ASTIOManagerTest, GetDiskIO_DiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    IOManager* iomgr = NULL;
    DiskIO* diskio = NULL;

    iomgr = GetIOManager();
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 100);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);
    ret = iomgr->GetDiskIO(diskkey, &diskio);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ASTIOManagerTest, ResolveOBJIO_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    PrepareResolveOBJIOEnv();
    BuildCollectOBJResponse();
    ret = ResolveOBJIO();
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = CheckResolveOBJIOResult();
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ASTIOManagerTest, ResolveOBJIO2_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    BuildCollectOBJResponse2();
    ret = ResolveOBJIO();
    ASSERT_EQ(ret, YFS_ENODATA);
}

/* vim:set ts=4 sw=4 expandtab */
