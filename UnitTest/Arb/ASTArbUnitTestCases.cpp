#include "UnitTest/AST/Arb/ASTArbUnitTests.hpp"

TEST_F(SubPoolArbitrateeTest, AddArbitratee_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateeloop = 0;
    ywb_uint64_t inodeid = 0;
    ArbitrateeKey arbitrateekey;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    subpoolarbitratees = GetSubPoolArbitratee();
    inodeid = Constant::DEFAULT_INODE;
    SetArbitrateeKey(arbitrateekey, Constant::DISK_ID,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);
    for(arbitrateeloop = 0; arbitrateeloop < 6; arbitrateeloop++, inodeid++)
    {
        arbitrateekey.SetInodeId(inodeid);
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }
}

TEST_F(SubPoolArbitrateeTest, AddArbitratee_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    ArbitrateeKey arbitrateekey;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    subpoolarbitratees = GetSubPoolArbitratee();
    SetArbitrateeKey(arbitrateekey, Constant::DISK_ID,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);
    ret = subpoolarbitratees->AddArbitratee(arbitrateekey, arbitratee);
    ASSERT_EQ(ret, YFS_EEXIST);
}

TEST_F(SubPoolArbitrateeTest, RemoveArbitratee_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ArbitrateeKey arbitrateekey;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    subpoolarbitratees = GetSubPoolArbitratee();
    SetArbitrateeKey(arbitrateekey, Constant::DISK_ID,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);
    ret = subpoolarbitratees->RemoveArbitratee(arbitrateekey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolArbitrateeTest, RemoveArbitratee_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ArbitrateeKey arbitrateekey;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    subpoolarbitratees = GetSubPoolArbitratee();
    SetArbitrateeKey(arbitrateekey, Constant::DISK_ID + 1000,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);
    ret = subpoolarbitratees->RemoveArbitratee(arbitrateekey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolArbitrateeTest, GetFirstNextArbitratee_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateenum = 0;
    ywb_uint64_t inodeid = 0;
    ArbitrateeKey arbitrateekey;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;

    subpoolarbitratees = GetSubPoolArbitratee();
    inodeid = Constant::DEFAULT_INODE;
    ret = subpoolarbitratees->GetFirstArbitratee(&arbitratee);
    while((YWB_SUCCESS == ret))
    {
        arbitrateenum++;
        arbitratee->GetOBJKey(&objkey);
        arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);
        ASSERT_EQ(arbitrateekey.GetInodeId(), inodeid++);
        ret = subpoolarbitratees->GetNextArbitratee(&arbitratee);
    }
    ASSERT_EQ(arbitrateenum, 6);
}

TEST_F(SubPoolArbitrateeTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateeloop = 0;
    ywb_uint64_t inodeid = 0;
    ArbitrateeKey arbitrateekey;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    subpoolarbitratees = GetSubPoolArbitratee();
    inodeid = Constant::DEFAULT_INODE;
    SetArbitrateeKey(arbitrateekey, Constant::DISK_ID,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);
    subpoolarbitratees->Destroy();
    for(arbitrateeloop = 0; arbitrateeloop < 6; arbitrateeloop++, inodeid++)
    {
        arbitrateekey.SetInodeId(inodeid);
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(ArbitratorTest, GetSubPoolArbitrateeFromIn_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolnum = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    arb = GetArbitrator();
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolnum++;
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }
    ASSERT_EQ(subpoolnum, 3);
}

TEST_F(ArbitratorTest, GetSubPoolArbitrateeFromIn_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 100);
    arb = GetArbitrator();
    ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ArbitratorTest, GetSubPoolArbitrateeFromOut_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 2);
    arb = GetArbitrator();
    ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, AddArbitrateeIn_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateeloop = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 8;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1);
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 2);
    SetArbitrateeKey(arbitrateekey, diskid,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);
    arb = GetArbitrator();
    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    while(arbitrateeloop < 6)
    {
        arbitrateekey.SetInodeId(inodeid);
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        ASSERT_EQ(ret, YWB_SUCCESS);
        arbitrateeloop++;
        inodeid++;
    }
}

TEST_F(ArbitratorTest, RemoveArbitrateeIn_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateeloop = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 8;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1);
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 2);
    SetArbitrateeKey(arbitrateekey, diskid,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    for(arbitrateeloop = 0; arbitrateeloop < 6; arbitrateeloop++, inodeid++)
    {
        arbitrateekey.SetInodeId(inodeid);
        ret = arb->RemoveArbitrateeIn(subpoolkey, arbitrateekey);
        ASSERT_EQ(ret, YWB_SUCCESS);
        arbitrateeloop++;
        inodeid++;
    }

    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1);
    for(arbitrateeloop = 0; arbitrateeloop < 6; arbitrateeloop++, inodeid++)
    {
        arbitrateekey.SetInodeId(inodeid);
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        ASSERT_EQ(ret, YFS_ENOENT);
        arbitrateeloop++;
        inodeid++;
    }
}

TEST_F(ArbitratorTest, RemoveSubPoolArbitrateeIn_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    arb = GetArbitrator();
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->RemoveSubPoolArbitrateeIn(subpoolkey);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }

    subpoolid = Constant::SUBPOOL_ID;
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(ArbitratorTest, RemoveSubPoolArbitrateeIn_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 100);
    arb = GetArbitrator();
    ret = arb->RemoveSubPoolArbitrateeIn(subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ArbitratorTest, AddArbitrateeOut_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateeloop = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 6;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1);
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 2);
    SetArbitrateeKey(arbitrateekey, diskid,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
    while(arbitrateeloop < 6)
    {
        arbitrateekey.SetInodeId(inodeid);
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        ASSERT_EQ(ret, YWB_SUCCESS);
        arbitrateeloop++;
        inodeid++;
    }
}

TEST_F(ArbitratorTest, RemoveArbitrateeOut_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateeloop = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 6;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1);
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 2);
    SetArbitrateeKey(arbitrateekey, diskid,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
    for(arbitrateeloop = 0; arbitrateeloop < 6; arbitrateeloop++, inodeid++)
    {
        arbitrateekey.SetInodeId(inodeid);
        ret = arb->RemoveArbitrateeOut(subpoolkey, arbitrateekey);
        ASSERT_EQ(ret, YWB_SUCCESS);
        arbitrateeloop++;
        inodeid++;
    }

    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1);
    for(arbitrateeloop = 0; arbitrateeloop < 6; arbitrateeloop++, inodeid++)
    {
        arbitrateekey.SetInodeId(inodeid);
        ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
        ASSERT_EQ(ret, YFS_ENOENT);
        arbitrateeloop++;
        inodeid++;
    }
}

TEST_F(ArbitratorTest, RemoveSubPoolArbitrateeOut_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    arb = GetArbitrator();
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->RemoveSubPoolArbitrateeOut(subpoolkey);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }

    subpoolid = Constant::SUBPOOL_ID;
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(ArbitratorTest, RemoveSubPoolArbitrateeOut_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 100);
    arb = GetArbitrator();
    ret = arb->RemoveSubPoolArbitrateeOut(subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ArbitratorTest, CheckConfigChange_SourceDiskNotExist_TRUE)
{
    ywb_bool_t change = false;
    SubPoolKey subpoolkey;
    DiskKey sourcedisk;
    Arbitrator* arb = NULL;
    LogicalConfig* config = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;

    arb = GetArbitrator();
    config = GetConfig();
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 2);

    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    subpoolarbitratees->GetFirstArbitratee(&arbitratee);
    arbitratee->GetOBJKey(&objkey);
    sourcedisk.SetSubPoolId(subpoolkey.GetSubPoolId());
    sourcedisk.SetDiskId(objkey->GetStorageId());
    config->RemoveDisk(sourcedisk);
    change = arb->CheckConfigChange(subpoolkey, arbitratee);
    ASSERT_EQ(change, true);
}

TEST_F(ArbitratorTest, CheckConfigChange_TargetDiskNotExist_TRUE)
{
    ywb_bool_t change = false;
    SubPoolKey subpoolkey;
    DiskKey targetdisk;
    Arbitrator* arb = NULL;
    LogicalConfig* config = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    arb = GetArbitrator();
    config = GetConfig();
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 2);

    arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
    subpoolarbitratees->GetFirstArbitratee(&arbitratee);
    targetdisk.SetDiskId(arbitratee->GetTargetID());
    targetdisk.SetSubPoolId(subpoolkey.GetSubPoolId());
    config->RemoveDisk(targetdisk);
    change = arb->CheckConfigChange(subpoolkey, arbitratee);
    ASSERT_EQ(change, true);
}

TEST_F(ArbitratorTest, CheckConfigChange_SubPoolNotExist_TRUE)
{
    ywb_bool_t change = false;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    LogicalConfig* config = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    arb = GetArbitrator();
    config = GetConfig();
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 2);

    config->RemoveSubPool(subpoolkey);
    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    subpoolarbitratees->GetFirstArbitratee(&arbitratee);
    change = arb->CheckConfigChange(subpoolkey, arbitratee);
    ASSERT_EQ(change, true);
}

TEST_F(ArbitratorTest, CheckConflict_TRUE)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t conflict = false;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;
    Arbitratee* arbitratee2 = NULL;
    ChunkIOStat* rawstat = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    YWB_ASSERT(Plan::PD_ctr_demote == arbitratee->GetDirection());
    arbitratee->GetIOStat(&rawstat);
    arbitratee2 = new Arbitratee(arbitrateekey, diskid + 1,
            subpoolkey, *rawstat, IOStat::IOST_raw,
            Plan::PD_ctr_promote, Constant::DEFAULT_OBJ_CAP, 102, 101);
    YWB_ASSERT(arbitratee2 != NULL);
    conflict = arb->CheckConflict(subpoolkey, arbitrateekey, arbitratee2);
    ASSERT_EQ(conflict, true);
    ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(arbitratee->GetDirection(), Plan::PD_ctr_promote);
    ASSERT_EQ(arbitratee->GetCycle(), 102);
    delete arbitratee2;
    arbitratee2 = NULL;
}

TEST_F(ArbitratorTest, CheckConflict_FALSE)
{
    ywb_bool_t conflict = false;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 10000;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    arbitratee = new Arbitratee();
    YWB_ASSERT(arbitratee != NULL);
    conflict = arb->CheckConflict(subpoolkey, arbitrateekey, arbitratee);
    ASSERT_EQ(conflict, false);
    delete arbitratee;
    arbitratee = NULL;
}

TEST_F(ArbitratorTest, CheckFrequentUpDown_ArbitrateeOutExpires_FALSE)
{
    ywb_bool_t freqeuent = false;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    CycleManager* cyclemanager = NULL;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 4;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    cyclemanager = GetCycleManager();
    arb = GetArbitrator();
//    arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
//    subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    GetArbitrateeFromOut(subpoolkey, arbitrateekey, &arbitratee);
    cyclemanager->SetCycleRelative(arbitratee->GetCycle() + 1000);
    freqeuent = arb->CheckFrequentUpDown(subpoolkey, arbitrateekey);
    ASSERT_EQ(freqeuent, false);
}

TEST_F(ArbitratorTest, CheckFrequentUpDown_ArbitrateeOutNotExpires_TRUE)
{
    ywb_bool_t freqeuent = false;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    CycleManager* cyclemanager = NULL;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 4;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    cyclemanager = GetCycleManager();
    arb = GetArbitrator();
//    arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
//    subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    GetArbitrateeFromOut(subpoolkey, arbitrateekey, &arbitratee);
    cyclemanager->SetCycleRelative(arbitratee->GetCycle() + 1);
    freqeuent = arb->CheckFrequentUpDown(subpoolkey, arbitrateekey);
    ASSERT_EQ(freqeuent, true);
}

TEST_F(ArbitratorTest, CheckBenefitOverCost_TRUE)
{
    ywb_bool_t benefit = false;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    benefit = arb->CheckBenefitOverCost(subpoolkey, arbitrateekey);
    ASSERT_EQ(benefit, true);
}

TEST_F(ArbitratorTest, Reserve_ByArbitratee_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
//    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
//    subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    GetArbitrateeFromIn(subpoolkey, arbitrateekey, &arbitratee);
    ret = arb->Reserve(subpoolkey, arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Reserve_ByArbitrateeKey_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    ret = arb->Reserve(subpoolkey, arbitrateekey);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Reserve_ByArbitrateeButNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 4;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    ret = arb->Reserve(subpoolkey, arbitrateekey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ArbitratorTest, UnReserve_ByArbitratee_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
//    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
//    subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    GetArbitrateeFromIn(subpoolkey, arbitrateekey, &arbitratee);
    arb->Reserve(subpoolkey, arbitratee);
    ret = arb->UnReserve(subpoolkey, arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, UnReserve_ByArbitrateeKey_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    arb->Reserve(subpoolkey, arbitrateekey);
    ret = arb->UnReserve(subpoolkey, arbitrateekey);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Consume_ByArbitratee_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
//    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
//    subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    GetArbitrateeFromIn(subpoolkey, arbitrateekey, &arbitratee);
    arb->Reserve(subpoolkey, arbitratee);
    ret = arb->Consume(subpoolkey, arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Consume_ByArbitrateeKey_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;

    diskid = Constant::DISK_ID + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 0);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    arb->Reserve(subpoolkey, arbitrateekey);
    ret = arb->Consume(subpoolkey, arbitrateekey);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, ReceivePlan_SourceDiskNotExist_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t srcdiskid = 0;
    ywb_uint64_t tgtdiskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    DiskKey srcdiskkey;
    LogicalConfig* config = NULL;
    Arbitrator* arb = NULL;
    Plan* plan = NULL;

    config = GetConfig();
    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID + 2;
    srcdiskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    tgtdiskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (srcdiskid - 1) + 10000;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    srcdiskkey.SetSubPoolId(subpoolid);
    srcdiskkey.SetDiskId(srcdiskid);
    plan = GenerateNewPlan(subpoolid, srcdiskid, tgtdiskid,
            inodeid, 100, Plan::PD_ctr_promote, 100);
    YWB_ASSERT(plan != NULL);
    config->RemoveDisk(srcdiskkey);
    ret = arb->ReceivePlan(subpoolkey, plan);
    DestroyPlan(plan);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(ArbitratorTest, ReceivePlan_SubPoolNotExist_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t srcdiskid = 0;
    ywb_uint64_t tgtdiskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Plan* plan = NULL;

    config = GetConfig();
    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID + 2;
    srcdiskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    tgtdiskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (srcdiskid - 1) + 10000;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    plan = GenerateNewPlan(subpoolid, srcdiskid, tgtdiskid,
            inodeid, 100, Plan::PD_ctr_promote, 100);
    YWB_ASSERT(plan != NULL);
    config->RemoveSubPool(subpoolkey);
    ret = arb->ReceivePlan(subpoolkey, plan);
    DestroyPlan(plan);
    ASSERT_EQ(ret, YFS_EINVAL);
    ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ArbitratorTest, ReceivePlan_FailToReserveForCapExceeds_YFS_ENOSPACE)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t srcdiskid = 0;
    ywb_uint64_t tgtdiskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    Plan* plan = NULL;

    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID + 2;
    srcdiskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    tgtdiskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (srcdiskid - 1) + 10000;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    plan = GenerateNewPlan(subpoolid, srcdiskid, tgtdiskid,
            inodeid, 512000000, Plan::PD_ctr_promote, 100);
    YWB_ASSERT(plan != NULL);
    ret = arb->ReceivePlan(subpoolkey, plan);
    DestroyPlan(plan);
    ASSERT_EQ(ret, YFS_ENOSPACE);
}

TEST_F(ArbitratorTest, ReceivePlan_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t srcdiskid = 0;
    ywb_uint64_t tgtdiskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    Plan* plan = NULL;

    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID + 2;
    srcdiskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    tgtdiskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 1;
    inodeid = Constant::DEFAULT_INODE + 6 * (srcdiskid - 1) + 10000;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    plan = GenerateNewPlan(subpoolid, srcdiskid, tgtdiskid,
            inodeid, 100, Plan::PD_ctr_promote, 100);
    YWB_ASSERT(plan != NULL);
    ret = arb->ReceivePlan(subpoolkey, plan);
    DestroyPlan(plan);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Complete_CurWindow_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    DiskKey tgtdiskkey;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;

    subpoolid = Constant::SUBPOOL_ID + 2;
    diskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    /*to simulate current window already reserve for it*/
    arb->Reserve(subpoolkey, arbitrateekey);
    ret = arb->Complete(subpoolkey, arbitrateekey, true, YWB_SUCCESS);
    ASSERT_EQ(ret, YWB_SUCCESS);
//    arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
//    ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    ret = GetArbitrateeFromOut(subpoolkey, arbitrateekey, &arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = CheckDiskPerf(subpoolkey, arbitratee->GetTargetID(),
            IOStat::IOST_raw, 0, 0, 0, 2480, 9920, 7);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Complete_PrevWindow_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;

    subpoolid = Constant::SUBPOOL_ID + 2;
    diskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    /*simulate previous window by not reserving here*/
    ret = arb->Complete(subpoolkey, arbitrateekey, false, YWB_SUCCESS);
    ASSERT_EQ(ret, YWB_SUCCESS);
//    arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
//    ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    ret = GetArbitrateeFromOut(subpoolkey, arbitrateekey, &arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = CheckDiskPerf(subpoolkey, arbitratee->GetTargetID(),
            IOStat::IOST_raw, 0, 0, 0, 2480, 9920, 7);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Complete_PrevWindow_ReserveFail_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;
    ChunkIOStat* iostat = NULL;

    subpoolid = Constant::SUBPOOL_ID + 2;
    diskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();

    GetArbitrateeFromIn(subpoolkey, arbitrateekey, &arbitratee);
    arbitratee->GetIOStat(&iostat);
    /*
     * make iops requirement huge enough and fails reservation,
     * should not set it to YWB_UINT32_MAX, otherwise overflow
     * will occur
     * */
    iostat->SetRndReadIOs((YWB_UINT32_MAX)/4);
    ret = arb->Complete(subpoolkey, arbitrateekey, false, YWB_SUCCESS);
    ASSERT_EQ(ret, YWB_SUCCESS);
//    arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
//    ret = subpoolarbitratees->GetArbitratee(arbitrateekey, &arbitratee);
    ret = GetArbitrateeFromOut(subpoolkey, arbitrateekey, &arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = CheckDiskPerf(subpoolkey, arbitratee->GetTargetID(),
            IOStat::IOST_raw, 0, 0, 0, 1800, 7200, 6);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Expire_CurWindow_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    DiskKey tgtdiskkey;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;

    subpoolid = Constant::SUBPOOL_ID + 2;
    diskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    /*to simulate current window already reserved for it*/
    arb->Reserve(subpoolkey, arbitrateekey);
    ret = arb->Expire(subpoolkey, arbitrateekey, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetArbitrateeFromIn(subpoolkey, arbitrateekey, &arbitratee);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = GetArbitrateeFromOut(subpoolkey, arbitrateekey, &arbitratee);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ArbitratorTest, Expire_PrevWindow_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    Arbitrator* arb = NULL;
    Arbitratee* arbitratee = NULL;

    subpoolid = Constant::SUBPOOL_ID + 2;
    diskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    /*simulate previous window by not reserving here*/
    ret = arb->Expire(subpoolkey, arbitrateekey, false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = GetArbitrateeFromIn(subpoolkey, arbitrateekey, &arbitratee);
    ASSERT_EQ(ret, YFS_ENOENT);
    ret = GetArbitrateeFromOut(subpoolkey, arbitrateekey, &arbitratee);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(ArbitratorTest, Expire_CurWindowAndThenAnotherOneCompleted_SUCCESS)
{
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    ArbitrateeKey arbitrateekey2;
    DiskKey tgtdiskkey;
    Arbitrator* arb = NULL;

    subpoolid = Constant::SUBPOOL_ID + 2;
    diskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);
    SetArbitrateeKey(arbitrateekey2, diskid,
            inodeid + 1, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    /*to simulate current window already reserved for it*/
    arb->Reserve(subpoolkey, arbitrateekey);
    arb->Reserve(subpoolkey, arbitrateekey2);
    arb->Expire(subpoolkey, arbitrateekey, true);
    arb->Complete(subpoolkey, arbitrateekey2, true, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Expire_PrevWindowAndThenAnotherOneCompleted_SUCCESS)
{
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    SubPoolKey subpoolkey;
    ArbitrateeKey arbitrateekey;
    ArbitrateeKey arbitrateekey2;
    Arbitrator* arb = NULL;

    subpoolid = Constant::SUBPOOL_ID + 2;
    diskid = Constant::DISK_ID + 3 * (subpoolid - 1) + 2;
    inodeid = Constant::DEFAULT_INODE + 6 * (diskid - 1) + 1;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, subpoolid);
    SetArbitrateeKey(arbitrateekey, diskid,
            inodeid, Constant::DEFAULT_CHUNK_ID);
    SetArbitrateeKey(arbitrateekey2, diskid,
            inodeid + 1, Constant::DEFAULT_CHUNK_ID);

    arb = GetArbitrator();
    /*simulate previous window by not reserving here*/
    arb->Expire(subpoolkey, arbitrateekey, false);
    arb->Complete(subpoolkey, arbitrateekey2, false, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Expire_AndThenSameArbitrateeCompleted_SUCCESS)
{
    /*
     * currently, it is impossible that migration completion event
     * against one expired arbitratee is received by ARB module
     * for DefaultMigrator already filtered these arbitratees
     * (DefaultMigrator will identify those expired arbitratees and
     * do not pass the notification down to Migration/ARB/Plan module)
     * */
}

TEST_F(ArbitratorTest, DrainAllArbitrateesIn_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    arb->DrainAllArbitrateesIn();
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(ArbitratorTest, DrainAllArbitrateesOut_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    arb->DrainAllArbitrateesOut();
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

//TEST_F(ArbitratorTest, Reset_INHaveMigratingArbitratees_SUCCESS)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    ywb_uint32_t subpoolid = 0;
//    SubPoolKey subpoolkey;
//    Arbitrator* arb = NULL;
//    SubPoolArbitratee* subpoolarbitratees = NULL;
//    Arbitratee* arbitratee = NULL;
//    Arbitratee* arbitratee2 = NULL;
//
//    arb = GetArbitrator();
//    subpoolid = Constant::SUBPOOL_ID;
//    subpoolkey.SetOss(Constant::OSS_ID);
//    subpoolkey.SetPoolId(Constant::POOL_ID);
//    subpoolkey.SetSubPoolId(subpoolid);
//
//    arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
//    subpoolarbitratees->GetFirstArbitratee(&arbitratee);
//    arbitratee->SetFlagUniq(Arbitratee::BAF_migrating);
//    arb->Reset();
//    ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
//    ASSERT_EQ(ret, YWB_SUCCESS);
//    subpoolarbitratees->GetFirstArbitratee(&arbitratee2);
//    ASSERT_EQ(arbitratee, arbitratee2);
//    ret = subpoolarbitratees->GetNextArbitratee(&arbitratee2);
//    ASSERT_EQ(ret, YFS_ENOENT);
//}

TEST_F(ArbitratorTest, Reset_INSubPoolArbitrateesEmptyButSubPoolExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ret = subpoolarbitratees->Destroy();
        ASSERT_EQ(ret, YWB_SUCCESS);
    }

    arb->Reset(ywb_false);
    subpoolid = Constant::SUBPOOL_ID;

    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ret = subpoolarbitratees->GetFirstArbitratee(&arbitratee);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(ArbitratorTest, Reset_INSubPoolArbitrateesEmptyAndSubPoolNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    config = GetConfig();
    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    config->RemoveSubPool(subpoolkey);
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ret = subpoolarbitratees->Destroy();
        ASSERT_EQ(ret, YWB_SUCCESS);
    }

    arb->Reset(ywb_false);
    subpoolid = Constant::SUBPOOL_ID;

    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        if(0 == subpoolloop)
        {
            ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
            ASSERT_EQ(ret, YFS_ENOENT);
        }
        else
        {
            ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
            ASSERT_EQ(ret, YWB_SUCCESS);
            ret = subpoolarbitratees->GetFirstArbitratee(&arbitratee);
            ASSERT_EQ(ret, YFS_ENOENT);
        }
    }
}

TEST_F(ArbitratorTest, Reset_OUTHaveArbitrateesNotExpire_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    CycleManager* cyclemgr = NULL;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    cyclemgr = GetCycleManager();
    arb = GetArbitrator();
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    cyclemgr->SetCycleRelative(101);
    arb->Reset(ywb_false);
    ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpoolarbitratees->GetFirstArbitratee(&arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(ArbitratorTest, Reset_OUTSubPoolArbitrateesEmptyButSubPoolExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    CycleManager* cyclemgr = NULL;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    cyclemgr = GetCycleManager();
    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    cyclemgr->SetCycleRelative(200);
    arb->Reset(ywb_false);
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ret = subpoolarbitratees->GetFirstArbitratee(&arbitratee);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

TEST_F(ArbitratorTest, Reset_OUTSubPoolArbitrateesEmptyAndSubPoolNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    LogicalConfig* config = NULL;
    CycleManager* cyclemgr = NULL;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;
    Arbitratee* arbitratee = NULL;

    config = GetConfig();
    cyclemgr = GetCycleManager();
    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    config->RemoveSubPool(subpoolkey);
    cyclemgr->SetCycleRelative(200);
    arb->Reset(ywb_false);
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        if(0 == subpoolloop)
        {
            ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
            ASSERT_EQ(ret, YFS_ENOENT);
        }
        else
        {
            ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
            ASSERT_EQ(ret, YWB_SUCCESS);
            ret = subpoolarbitratees->GetFirstArbitratee(&arbitratee);
            ASSERT_EQ(ret, YFS_ENOENT);
        }
    }
}

TEST_F(ArbitratorTest, Destroy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;
    Arbitrator* arb = NULL;
    SubPoolArbitratee* subpoolarbitratees = NULL;

    arb = GetArbitrator();
    subpoolid = Constant::SUBPOOL_ID;
    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    arb->Destroy();
    for(subpoolloop = 0; subpoolloop < 3; subpoolloop++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        ret = arb->GetSubPoolArbitrateeFromIn(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YFS_ENOENT);
        ret = arb->GetSubPoolArbitrateeFromOut(subpoolkey, &subpoolarbitratees);
        ASSERT_EQ(ret, YFS_ENOENT);
    }
}

/* vim:set ts=4 sw=4 expandtab */
