#include <unistd.h>
#include "UnitTest/AST/Migration/ASTMigrationUnitTests.hpp"

TEST_F(MigrationDiskTest, GetFirstNextArbitratee_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateenum = 0;
    MigrationDisk* migrationdisk = NULL;
    Arbitratee* arbitratee = NULL;

    migrationdisk = GetMigrationDisk();
    ret = migrationdisk->GetFirstArbitratee(&arbitratee);
    while(YWB_SUCCESS == ret)
    {
        arbitrateenum++;
        ret = migrationdisk->GetNextArbitratee(&arbitratee);
    }

    ASSERT_EQ(arbitrateenum, 6);
}

TEST_F(MigrationDiskTest, GetFirstNextArbitratee_NoElement_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    MigrationDisk* migrationdisk = NULL;
    Arbitratee* arbitratee = NULL;

    migrationdisk = GetMigrationDisk2();
    ret = migrationdisk->GetFirstArbitratee(&arbitratee);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationDiskTest, GetFirstNextArbitratee_Reentry_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateenum = 0;
    MigrationDisk* migrationdisk = NULL;
    Arbitratee* arbitratee = NULL;

    migrationdisk = GetMigrationDisk();
    ret = migrationdisk->GetFirstArbitratee(&arbitratee);
    while((YWB_SUCCESS == ret) && (arbitrateenum < 3))
    {
        arbitrateenum++;
        ret = migrationdisk->GetNextArbitratee(&arbitratee);
    }
    ASSERT_EQ(arbitrateenum, 3);

    arbitrateenum = 0;
    ret = migrationdisk->GetFirstArbitratee(&arbitratee);
    while(YWB_SUCCESS == ret)
    {
        arbitrateenum++;
        ret = migrationdisk->GetNextArbitratee(&arbitratee);
    }
    ASSERT_EQ(arbitrateenum, 6);
}

TEST_F(DefaultMigratorTest, Start_SUCCESS)
{
    ASSERT_EQ(GetStatus(), DefaultMigratorMock::S_running);
    CleanUp(false);
}

TEST_F(DefaultMigratorTest, Start_StopImmediately_SUCCESS)
{
    ASSERT_EQ(GetStatus(), DefaultMigratorMock::S_running);
    CleanUp(true);
}

TEST_F(DefaultMigratorTest, Submit_SUCCESS)
{
    ywb_uint32_t arbitrateenum = 0;
    ywb_uint64_t tgtid = 0;
    OBJKey objkey;
    SubPoolKey subpoolkey;
    ChunkIOStat stat;
    Arbitratee* arbitratee = NULL;

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    subpoolkey.SetOss(0);
    subpoolkey.SetPoolId(0);
    subpoolkey.SetSubPoolId(0);
    while(arbitrateenum < 6)
    {
        objkey.SetInodeId(Constant::DEFAULT_INODE + arbitrateenum);
        objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + arbitrateenum);
        arbitrateenum++;
        arbitratee = new Arbitratee(objkey, tgtid,
                subpoolkey, stat, 0, 0, 0, 0, 0);
        YWB_ASSERT(arbitratee != NULL);
        Submit(arbitratee);
    }

    sleep(1);
    CleanUp(false);

#if 0
    /*wait until migrator thread complete handling*/
    while(DefaultMigrator::S_stopped !=
            GetDefaultMigratorMock()->GetStatus())
    {
        sleep(1);
    }
#endif

    /*
     * these assertion must be after CleanUp(), otherwise the
     * DefaultMigrator's thread may have not handled them
     *
     * CleanUp() will submit a fake arbitratee which acts as
     * stop signal, but this arbitratee will not experience
     * Prepare-Do-Complete phases.
     **/
    ASSERT_EQ(GetDefaultMigratorMock()->GetStatus(), DefaultMigrator::S_stopped);
    ASSERT_EQ(GetSubmittedNum(), 7);
    ASSERT_EQ(GetPreparedNum(), 6);
    ASSERT_EQ(GetDoneNum(), 6);
    ASSERT_EQ(GetCompletedNum(), 6);
}

TEST_F(DefaultMigratorTest, Submit_StopImmediately_SUCCESS)
{
    ywb_uint32_t arbitrateenum = 0;
    Arbitratee* arbitratee = NULL;

    while(arbitrateenum < 6)
    {
        arbitrateenum++;
        arbitratee = new Arbitratee();
        YWB_ASSERT(arbitratee != NULL);
        Submit(arbitratee);
    }

    CleanUp(true);
    /*
     * these assertion must be after CleanUp(), otherwise the
     * DefaultMigrator's thread may have not handled them
     *
     * CleanUp() will submit a fake arbitratee which acts as
     * stop signal, but this arbitratee will not experience
     * Prepare-Do-Complete phases.
     **/
    ASSERT_EQ(GetSubmittedNum(), 6);
    ASSERT_EQ(GetPreparedNum(), 0);
    ASSERT_EQ(GetDoneNum(), 0);
    ASSERT_EQ(GetCompletedNum(), 0);
}

TEST_F(DefaultMigratorTest, Stop_WithOutSubmitAnyArbitratee_SUCCESS)
{
    CleanUp(false);
    ASSERT_EQ(GetStatus(), DefaultMigratorMock::S_stopped);
}

TEST_F(DefaultMigratorTest, StopImmediately_WithOutSubmitAnyArbitratee_SUCCESS)
{
    CleanUp(true);
    ASSERT_EQ(GetStatus(), DefaultMigratorMock::S_stopped);
}

TEST_F(DefaultMigratorTest, Stop_SUCCESS)
{
    ywb_uint32_t arbitrateenum = 0;
    Arbitratee* arbitratee = NULL;

    while(arbitrateenum < 6)
    {
        arbitrateenum++;
        arbitratee = new Arbitratee();
        YWB_ASSERT(arbitratee != NULL);
        Submit(arbitratee);
    }

    CleanUp(false);
    ASSERT_EQ(GetStatus(), DefaultMigratorMock::S_stopped);
}

TEST_F(DefaultMigratorTest, StopImmediately_SUCCESS)
{
    ywb_uint32_t arbitrateenum = 0;
    Arbitratee* arbitratee = NULL;

    while(arbitrateenum < 6)
    {
        arbitrateenum++;
        arbitratee = new Arbitratee();
        YWB_ASSERT(arbitratee != NULL);
        Submit(arbitratee);
    }

    CleanUp(true);
    ASSERT_EQ(GetStatus(), DefaultMigratorMock::S_stopped);
}

TEST_F(DefaultMigratorTest, DoMigration_SUCCESS)
{
    ywb_uint32_t arbitrateenum = 0;
    ywb_uint64_t diskid =  Constant::DISK_ID;
    OBJKey objkey;
    SubPoolKey subpoolkey;
    ChunkIOStat rawstat;
    Arbitratee* arbitratee = NULL;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    SetOBJKey(objkey, Constant::DISK_ID,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);
    SetHUBSimulateExpiration(true);
    while(arbitrateenum < 6)
    {
        objkey.SetInodeId(Constant::DEFAULT_INODE + arbitrateenum);
        objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + arbitrateenum);
        arbitratee = new Arbitratee(objkey, diskid,
                subpoolkey, rawstat, IOStat::IOST_raw,
                Plan::PD_ctr_demote, Constant::DEFAULT_OBJ_CAP, 100, 101);
        YWB_ASSERT(arbitratee != NULL);
        Submit(arbitratee);
        arbitrateenum++;
    }

    /*this will make sure all arbitratees are submitted to HUB*/
    CleanUp(false);
    ASSERT_EQ(GetInflightNum(), 6);
}

TEST_F(DefaultMigratorTest, CheckExpired_SUCCESS)
{
    ywb_uint32_t arbitrateenum = 0;
    ywb_uint64_t diskid =  Constant::DISK_ID;
    OBJKey objkey;
    SubPoolKey subpoolkey;
    ChunkIOStat rawstat;
    Arbitratee* arbitratee = NULL;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    SetOBJKey(objkey, Constant::DISK_ID,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);
    SetHUBSimulateExpiration(true);
    while(arbitrateenum < 6)
    {
        objkey.SetInodeId(Constant::DEFAULT_INODE + arbitrateenum);
        objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + arbitrateenum);
        arbitratee = new Arbitratee(objkey, diskid,
                subpoolkey, rawstat, IOStat::IOST_raw,
                Plan::PD_ctr_demote, Constant::DEFAULT_OBJ_CAP, 100, 101);
        YWB_ASSERT(arbitratee != NULL);
        arbitratee->SetExpireTime(10);
        Submit(arbitratee);
        arbitrateenum++;
    }

    /*this will make sure all arbitratees are submitted to HUB*/
    CleanUp(false);
    CheckExpired();
    ASSERT_EQ(GetInflightNum(), 0);
}

TEST_F(DefaultMigratorTest, ExpiredAndThenCompleted_SUCCESS)
{
    ywb_uint32_t arbitrateenum = 0;
    ywb_uint64_t diskid =  Constant::DISK_ID;
    OBJKey objkey;
    SubPoolKey subpoolkey;
    ChunkIOStat rawstat;
    Arbitratee* arbitratee = NULL;

    SetSubPoolKey(subpoolkey, Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    SetOBJKey(objkey, Constant::DISK_ID,
            Constant::DEFAULT_INODE, Constant::DEFAULT_CHUNK_ID);
    SetHUBSimulateExpiration(true);
    while(arbitrateenum < 6)
    {
        objkey.SetInodeId(Constant::DEFAULT_INODE + arbitrateenum);
        objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + arbitrateenum);
        arbitratee = new Arbitratee(objkey, diskid,
                subpoolkey, rawstat, IOStat::IOST_raw,
                Plan::PD_ctr_demote, Constant::DEFAULT_OBJ_CAP, 100, 101);
        YWB_ASSERT(arbitratee != NULL);
        Submit(arbitratee);
        /*set arbitratee expiration time to be 10us*/
        arbitratee->SetExpireTime(10);
        arbitrateenum++;
    }

    /*make sure all arbitratees expired*/
    usleep(100);
    LaunchCheckExpired();
    objkey.SetInodeId(Constant::DEFAULT_INODE + 0);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + 0);
    /*simulate first arbitratee completed after expired*/
    LaunchMigrationCompleted(objkey);
    CleanUp(false);
}

TEST_F(MigrationTest, AddMigrationDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    migration = GetMigration();
    diskkey.SetSubPoolId(subpoolid);

    while(diskloop++ < 3)
    {
        diskkey.SetDiskId(diskid++);
        ret = migration->GetMigrationDisk(diskkey, &migrationdisk);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }
}

TEST_F(MigrationTest, GetMigrationDisk_NoSuchMigrationDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);
    ret = migration->GetMigrationDisk(diskkey, &migrationdisk);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationTest, RemoveMigrationDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;
    Migration* migration = NULL;

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    migration = GetMigration();
    diskkey.SetSubPoolId(subpoolid);

    while(diskloop++ < 3)
    {
        diskkey.SetDiskId(diskid++);
        ret = migration->RemoveMigrationDisk(diskkey);
        ASSERT_EQ(ret, YWB_SUCCESS);
    }
}

TEST_F(MigrationTest, RemoveMigrationDisk_NoSuchMigrationDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Migration* migration = NULL;

    migration = GetMigration();
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(Constant::DISK_ID + 1000);
    ret = migration->RemoveMigrationDisk(diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationTest, RegisterMigrator_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t migratornum = 0;
    Migration* migration = NULL;
    Migrator* migrator = NULL;
    char migratorname[16] = {0};

    migration = GetMigration();
    while(migratornum < 3)
    {
        memset(migratorname, 0, 16);
        sprintf(migratorname, "%s-%d", "Migrator", migratornum);
        ret = migration->GetMigrator(migratorname, &migrator);
        ASSERT_EQ(ret, YWB_SUCCESS);
        migratornum++;
    }
}

TEST_F(MigrationTest, GetMigrator_NoSuchMigrator_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    Migration* migration = NULL;
    Migrator* migrator = NULL;

    migration = GetMigration();
    ret = migration->GetMigrator("Migrator1000", &migrator);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationTest, DeRegisterMigrator_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    Migration* migration = NULL;
    Migrator* fakemigrator = NULL;

    migration = GetMigration();
    fakemigrator = new FakeMigrator(migration);
    YWB_ASSERT(fakemigrator != NULL);
    migration->RegisterMigrator("FakeMigrator", fakemigrator);
    ret = migration->DeRegisterMigrator("FakeMigrator");
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(MigrationTest, DeRegisterMigrator_NoSuchMigrator_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    Migration* migration = NULL;

    migration = GetMigration();
    ret = migration->DeRegisterMigrator("Migrator" + 1000);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationTest, Initialize_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    Migration* migration = NULL;
    Migrator* migrator = NULL;

    migration = GetMigration();
    ret = migration->GetMigrator("default", &migrator);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(MigrationTest, ReceiveArbitratee_SourceDiskNotExist_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey srcdiskkey;
    LogicalConfig* config = NULL;

    config = GetConfig();
    srcdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    srcdiskkey.SetDiskId(Constant::DISK_ID);
    config->RemoveDisk(srcdiskkey);

    ret = ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, Constant::DISK_ID + 1,
            Constant::DEFAULT_INODE, Constant::DEFAULT_OBJ_CAP,
            Plan::PD_ctr_promote, 100, 101);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(MigrationTest, ReceiveArbitratee_TargetMigrationDiskNotSetup_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, Constant::DISK_ID + 4,
            Constant::DEFAULT_INODE, Constant::DEFAULT_OBJ_CAP,
            Plan::PD_ctr_promote, 100, 101);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(MigrationTest, ReceiveArbitratee_TargetMigrationDiskFree_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    Migration* migration = NULL;
    Arbitratee* arbitratee = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    migrationdisk->SetFlagUniq(MigrationDisk::MDF_free);
    ret = ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(migrationdisk->TestFlag(MigrationDisk::MDF_free), false);
    ret = migrationdisk->GetNextArbitratee(&arbitratee);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationTest, ReceiveArbitratee_TargetMigrationDiskBusy_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    Migration* migration = NULL;
    Arbitratee* arbitratee = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    migrationdisk->ClearFlag(MigrationDisk::MDF_free);
    ret = ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = migrationdisk->GetNextArbitratee(&arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(MigrationTest, Complete_MigrationDiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    migration->RemoveMigrationDisk(tgtdiskkey);
    ret = Complete(Constant::DEFAULT_INODE, YWB_SUCCESS, true);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationTest, Complete_NotTriggerAnyMore_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ret = Complete(Constant::DEFAULT_INODE, YWB_SUCCESS, false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(migrationdisk->TestFlag(MigrationDisk::MDF_free), true);
}

TEST_F(MigrationTest, Complete_TriggerMoreButNoMoreArbitratee_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    arbitratee->GetOBJKey(&objkey);
    ret = Complete(Constant::DEFAULT_INODE, YWB_SUCCESS, true);
    ASSERT_EQ(ret, YFS_ENODATA);
    ASSERT_EQ(migrationdisk->TestFlag(MigrationDisk::MDF_free), true);
}

TEST_F(MigrationTest, Complete_TriggerMoreAndHaveMoreArbitratee_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE + 1,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ret = Complete(Constant::DEFAULT_INODE, YWB_SUCCESS, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(migrationdisk->TestFlag(MigrationDisk::MDF_free), false);
}

TEST_F(MigrationTest, Complete_TriggerMoreButTargetDiskNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    LogicalConfig* config = NULL;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    config = GetConfig();
    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE + 1,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    config->RemoveDisk(tgtdiskkey);
    ret = Complete(Constant::DEFAULT_INODE, YWB_SUCCESS, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationTest, Expire_MigrationDiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    migration->RemoveMigrationDisk(tgtdiskkey);
    ret = Expire(Constant::DEFAULT_INODE, true);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationTest, Expire_NotTriggerAnyMore_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ret = Expire(Constant::DEFAULT_INODE, false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(migrationdisk->TestFlag(MigrationDisk::MDF_free), true);
}

TEST_F(MigrationTest, Expire_TriggerMoreButNoMoreArbitratee_YFS_ENODATA)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    arbitratee->GetOBJKey(&objkey);
    ret = Expire(Constant::DEFAULT_INODE, true);
    ASSERT_EQ(ret, YFS_ENODATA);
    ASSERT_EQ(migrationdisk->TestFlag(MigrationDisk::MDF_free), true);
}

TEST_F(MigrationTest, Expire_TriggerMoreAndHaveMoreArbitratee_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE + 1,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ret = Expire(Constant::DEFAULT_INODE, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(migrationdisk->TestFlag(MigrationDisk::MDF_free), false);
}

TEST_F(MigrationTest, Expire_TriggerMoreButTargetDiskNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    LogicalConfig* config = NULL;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    config = GetConfig();
    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE + 1,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    config->RemoveDisk(tgtdiskkey);
    ret = Expire(Constant::DEFAULT_INODE, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(MigrationTest, Expire_NotTriggerAnyMoreAndThenAnotherOneCompleted_SUCCESS)
{
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE + 1,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    Expire(Constant::DEFAULT_INODE, false);
    Complete(Constant::DEFAULT_INODE + 1, YWB_SUCCESS, true);
    /*no assertion anymore*/
}

TEST_F(MigrationTest, Expire_TriggerMoreButNoMoreArbitrateeAndThenAnotherOneCompleted_SUCCESS)
{
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE + 1,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    Expire(Constant::DEFAULT_INODE, false);
    Complete(Constant::DEFAULT_INODE + 1, YWB_SUCCESS, true);
    /*no assertion anymore*/
}

TEST_F(MigrationTest, Expire_TriggerMoreButTargetDiskNotExistAndThenAnotherOneCompleted_SUCCESS)
{
    ywb_uint64_t tgtdiskid = 0;
    DiskKey tgtdiskkey;
    ArbitrateeKey arbitrateekey;
    LogicalConfig* config = NULL;
    Migration* migration = NULL;
    MigrationDisk* migrationdisk = NULL;

    config = GetConfig();
    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;
    tgtdiskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    tgtdiskkey.SetDiskId(tgtdiskid);

    migration->GetMigrationDisk(tgtdiskkey, &migrationdisk);
    YWB_ASSERT(migrationdisk != NULL);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    ReceiveArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE + 1,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    config->RemoveDisk(tgtdiskkey);
    Expire(Constant::DEFAULT_INODE, true);
    Complete(Constant::DEFAULT_INODE + 1, YWB_SUCCESS, true);
    /*no assertion anymore*/
}

TEST_F(MigrationTest, Expire_AndThenSameArbitrateeCompleted_SUCCESS)
{
    /*
     * currently, it is impossible that migration completion event
     * against one expired arbitratee is received by Migration
     * module for DefaultMigrator already filtered these arbitratees
     * (DefaultMigrator will identify those expired arbitratees and
     * do not pass the notification down to Migration/ARB/Plan module)
     * */
}

TEST_F(MigrationTest, LaunchMigration_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtdiskid = 0;
    Migration* migration = NULL;
    Arbitratee* arbitratee = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;

    arbitratee = GenerateNewArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    YWB_ASSERT(arbitratee != NULL);
    ASSERT_EQ(migration->GetInflightEmpty(), true);
    ret = migration->LaunchMigration(arbitratee);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(migration->GetInflightEmpty(), false);
}

TEST_F(MigrationTest, RemoveInflight_SUCCESS)
{
    ywb_uint64_t tgtdiskid = 0;
    ArbitrateeKey arbitrateekey;
    Migration* migration = NULL;
    Arbitratee* arbitratee = NULL;
    OBJKey* objkey = NULL;

    migration = GetMigration();
    tgtdiskid = Constant::DISK_ID + 1;

    arbitratee = GenerateNewArbitratee(Constant::SUBPOOL_ID,
            Constant::DISK_ID, tgtdiskid, Constant::DEFAULT_INODE,
            Constant::DEFAULT_OBJ_CAP, Plan::PD_ctr_promote, 100, 101);
    YWB_ASSERT(arbitratee != NULL);
    migration->LaunchMigration(arbitratee);
    arbitratee->GetOBJKey(&objkey);
    arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);
    migration->RemoveInflight(arbitrateekey);
    ASSERT_EQ(migration->GetInflightEmpty(), true);
}

/* vim:set ts=4 sw=4 expandtab */
