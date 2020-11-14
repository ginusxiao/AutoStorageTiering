#include "YfsDirectorySetting.hpp"
#include "CommonInitialize.h"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/Migration/ASTMigrationUnitTests.hpp"

void MigrationDiskTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t arbitrateeloop = 0;
    Arbitratee* arbitratee = NULL;

    while(arbitrateeloop < 6)
    {
        arbitratee = new Arbitratee();
        YWB_ASSERT(arbitratee != NULL);
        ret = mMigrationDisk->AddArbitratee(arbitratee);
        YWB_ASSERT(YWB_SUCCESS == ret);
        arbitrateeloop++;
    }
}

void MigrationDiskTest::TearDown()
{
    ywb_status_t ret = YWB_SUCCESS;
    Arbitratee* arbitratee = NULL;
    vector<Arbitratee*> arbitrateevec;
    vector<Arbitratee*>::iterator arbitrateeiter;

    ret = mMigrationDisk->GetFirstArbitratee(&arbitratee);
    while(YWB_SUCCESS == ret)
    {
        arbitrateevec.push_back(arbitratee);
        ret = mMigrationDisk->GetNextArbitratee(&arbitratee);
    }

    mMigrationDisk->Reset();
    arbitrateeiter = arbitrateevec.begin();
    while(arbitrateeiter != arbitrateevec.end())
    {
        arbitratee = *arbitrateeiter;
        arbitrateeiter = arbitrateevec.erase(arbitrateeiter);
        delete arbitratee;
        arbitratee = NULL;
    }
}

MigrationDisk*
MigrationDiskTest::GetMigrationDisk()
{
    return mMigrationDisk;
}

MigrationDisk*
MigrationDiskTest::GetMigrationDisk2()
{
    return mMigrationDisk2;
}

void* DefaultMigratorMock::entry()
{
    return DefaultMigrator::entry();
}

ywb_status_t DefaultMigratorMock::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = DefaultMigrator::Start();

    return ret;
}

ywb_status_t DefaultMigratorMock::Stop()
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = DefaultMigrator::Stop(mStopImmediate);

    return ret;
}

void DefaultMigratorMock::Submit(BaseArbitratee* item)
{
    DefaultMigrator::Submit(item);
    if(item != NULL)
    {
        mSubmittedNum++;
    }
}

void DefaultMigratorMock::SubmitFront(BaseArbitratee* item)
{
    DefaultMigrator::SubmitFront(item);
    if(item != NULL)
    {
        mSubmittedFrontNum++;
    }
}

ywb_status_t
DefaultMigratorMock::PrepareMigration(Arbitratee* item)
{
    ywb_status_t ret = YWB_SUCCESS;

    DefaultMigrator::PrepareMigration(item);
    if(item != NULL)
    {
        mPreparedNum++;
    }

    return ret;
}

ywb_status_t
DefaultMigratorMock::DoMigration(Arbitratee* item)
{
    ywb_status_t ret = YWB_SUCCESS;

    DefaultMigrator::DoMigration(item);
    if(item != NULL)
    {
        mDoneNum++;
    }

    return ret;
}

ywb_status_t
DefaultMigratorMock::CompleteMigration(Arbitratee* item, ywb_status_t migstatus)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(item != NULL)
    {
        mCompletedNum++;
    }

    DefaultMigrator::CompleteMigration(item, migstatus);

    return ret;
}

void DefaultMigratorMock::LaunchCompleteMigrationEvent(
        OBJKey objkey, ywb_status_t err)
{
    DefaultMigrator::LaunchCompleteMigrationEvent(objkey, err);
}

ywb_uint32_t DefaultMigratorMock::GetSubmittedNum()
{
    return mSubmittedNum;
}

ywb_uint32_t DefaultMigratorMock::GetSubmittedFrontNum()
{
    return mSubmittedFrontNum;
}

ywb_uint32_t DefaultMigratorMock::GetPreparedNum()
{
    return mPreparedNum;
}

ywb_uint32_t DefaultMigratorMock::GetDoneNum()
{
    return mDoneNum;
}

ywb_uint32_t DefaultMigratorMock::GetCompletedNum()
{
    return mCompletedNum;
}

ywb_bool_t DefaultMigratorMock::GetStopImmediate()
{
    return mStopImmediate;
}

void DefaultMigratorMock::SetStopImmediate(ywb_bool_t stopimmediate)
{
    mStopImmediate = stopimmediate;
}

void DefaultMigratorMock::CleanUp()
{
    vector<BaseArbitratee*>::iterator veciter;
    BaseArbitratee* arbitratee = NULL;

    veciter = mArbitrateeVec.begin();
    while(veciter != mArbitrateeVec.end())
    {
        arbitratee = *veciter;
        veciter = mArbitrateeVec.erase(veciter);
        delete arbitratee;
        arbitratee = NULL;
    }
}

void DefaultMigratorTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = mControlCenterMock->Start();
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = mDefaultMigratorMock->Start();
    YWB_ASSERT(YWB_SUCCESS == ret);
}

void DefaultMigratorTest::TearDown()
{
    mDefaultMigratorMock->Stop();
    /*
     * DO NOT stop ControlCenterMock here for de-construction
     * of ControlCenterMock will do it for us
     **/
//    mControlCenterMock->Stop();
}

void DefaultMigratorTest::Submit(Arbitratee* item)
{
    return mDefaultMigratorMock->Submit(item);
}

void DefaultMigratorTest::SubmitFront(Arbitratee* item)
{
    return mDefaultMigratorMock->SubmitFront(item);
}

DefaultMigratorMock*
DefaultMigratorTest::GetDefaultMigratorMock()
{
    return mDefaultMigratorMock;
}

ywb_uint32_t DefaultMigratorTest::GetStatus()
{
    return mDefaultMigratorMock->GetStatus();
}

ywb_uint32_t DefaultMigratorTest::GetSubmittedNum()
{
    return mDefaultMigratorMock->GetSubmittedNum();
}

ywb_uint32_t DefaultMigratorTest::GetSubmittedFrontNum()
{
    return mDefaultMigratorMock->GetSubmittedFrontNum();
}

ywb_uint32_t DefaultMigratorTest::GetPreparedNum()
{
    return mDefaultMigratorMock->GetPreparedNum();
}

ywb_uint32_t DefaultMigratorTest::GetDoneNum()
{
    return mDefaultMigratorMock->GetDoneNum();
}

ywb_uint32_t DefaultMigratorTest::GetCompletedNum()
{
    return mDefaultMigratorMock->GetCompletedNum();
}

void DefaultMigratorTest::CleanUp(ywb_bool_t stopimmediate)
{
    ywb_status_t ret = YWB_SUCCESS;

    mDefaultMigratorMock->CleanUp();
    mDefaultMigratorMock->SetStopImmediate(stopimmediate);
    ret = mDefaultMigratorMock->Stop();
    YWB_ASSERT(YWB_SUCCESS == ret);
    ret = mControlCenterMock->Stop();
    YWB_ASSERT(YWB_SUCCESS == ret);
}

void MigrationTest::SetUp()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskloop = 0;
    ywb_uint64_t diskid = 0;
    DiskBaseProp diskprop;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    Migration* migration = NULL;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    MigrationDisk* migrationdisk = NULL;

    ast = mAst;
    ast->GetLogicalConfig(&config);
    ast->GetMigration(&migration);
    diskid = Constant::DISK_ID;
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskprop.SetDiskRPM(10000);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    /*logical configuration related*/
    subpool = new SubPool(subpoolkey);
    YWB_ASSERT(subpool != NULL);
    ret = config->AddSubPool(subpoolkey, subpool);
    YWB_ASSERT(YWB_SUCCESS == ret);

    while(diskloop++ < 6)
    {
        diskkey.SetDiskId(diskid++);
        disk = new Disk(diskprop);
        YWB_ASSERT(disk != NULL);
        ret = config->AddDisk(diskkey, disk);
        YWB_ASSERT(YWB_SUCCESS == ret);
    }

    /*migration disk related*/
    diskloop = 0;
    diskid = Constant::DISK_ID;
    while(diskloop++ < 3)
    {
        diskkey.SetDiskId(diskid++);
        migrationdisk = new MigrationDisk();
        YWB_ASSERT(migrationdisk != NULL);
        ret = migration->AddMigrationDisk(diskkey, migrationdisk);
        YWB_ASSERT(YWB_SUCCESS == ret);
    }

    migration->Initialize();
    RegisterMigrators();
    ret = migration->StartMigrator("NoExistentMigrator");
    YWB_ASSERT(YWB_SUCCESS == ret);
}

void MigrationTest::TearDown()
{
    ywb_uint32_t diskloop = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;
    AST* ast = NULL;
    Migration* migration = NULL;

    ast = mAst;
    ast->GetMigration(&migration);
    diskid = Constant::DISK_ID;
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    while(diskloop++ < 3)
    {
        diskkey.SetDiskId(diskid++);
        migration->RemoveMigrationDisk(diskkey);
    }

    DeRegisterMigrators();
    migration->StopMigrator(true);
    migration->Finalize();
    migration->DrainAllInflight();
//    RemoveArbitratees();
}

LogicalConfig* MigrationTest::GetConfig()
{
    LogicalConfig* config = NULL;

    mAst->GetLogicalConfig(&config);

    return config;
}

Migration* MigrationTest::GetMigration()
{
    Migration* migration = NULL;

    mAst->GetMigration(&migration);

    return migration;
}

void MigrationTest::RegisterMigrators()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t migratornum = 0;
    Migration* migration = NULL;
    Migrator* migrator = NULL;

    migration = GetMigration();
    while(migratornum++ < 3)
    {
        migrator = new FakeMigrator(migration);
        YWB_ASSERT(migrator != NULL);
        ret = migration->RegisterMigrator("Migrator" + migratornum, migrator);
        YWB_ASSERT(YWB_SUCCESS == ret);
    }
}

void MigrationTest::DeRegisterMigrators()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t migratornum = 0;
    Migration* migration = NULL;

    migration = GetMigration();
    while(migratornum++ < 3)
    {
        ret = migration->DeRegisterMigrator("Migrator" + migratornum);
        YWB_ASSERT(YWB_SUCCESS == ret);
    }
}

ywb_status_t
MigrationTest::AddArbitratee(ywb_uint64_t inodeid, Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<ywb_uint64_t, Arbitratee*>::iterator, bool> pairret;

    pairret = mArbitratees.insert(make_pair(inodeid, arbitratee));
    if(pairret.second)
    {
        ret = YWB_SUCCESS;
    }
    else
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t
MigrationTest::GetArbitratee(
        ywb_uint64_t inodeid, Arbitratee** arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ywb_uint64_t, Arbitratee*>::iterator iter;

    iter = mArbitratees.find(inodeid);
    if(mArbitratees.end() != iter)
    {
        *arbitratee = iter->second;
        ret = YWB_SUCCESS;
    }
    else
    {
        ret = YFS_ENOENT;
        *arbitratee = NULL;
    }

    return ret;
}

void MigrationTest::RemoveArbitratees()
{
    map<ywb_uint64_t, Arbitratee*>::iterator arbitrateeiter;
    Arbitratee* arbitratee = NULL;

    arbitrateeiter = mArbitratees.begin();
    while(arbitrateeiter != mArbitratees.end())
    {
        arbitratee = arbitrateeiter->second;
        mArbitratees.erase(arbitrateeiter++);
        delete arbitratee;
        arbitratee = NULL;
    }
}

Arbitratee* MigrationTest::GenerateNewArbitratee(
        ywb_uint32_t subpoolid, ywb_uint64_t srcdiskid,
        ywb_uint64_t tgtdiskid, ywb_uint64_t inodeid,
        ywb_uint32_t cap, ywb_uint32_t direction,
        ywb_uint64_t cycle, ywb_uint64_t epoch)
{
    ywb_uint32_t factors = 0;
    SubPoolKey subpoolkey;
    OBJKey objkey;
    ChunkIOStat rawstat;
    Arbitratee* arbitratee = NULL;

    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    subpoolkey.SetSubPoolId(subpoolid);

    objkey.SetStorageId(srcdiskid);
    objkey.SetInodeId(inodeid);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    factors = 10;
    rawstat.SetRndReadIOs(factors);
    rawstat.SetRndReadBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetRndWriteIOs(factors);
    rawstat.SetRndWriteBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetRndTotalRT((factors * (Constant::META_RT_READ)) +
            (factors * (Constant::META_RT_WRITE)));
    rawstat.SetSeqReadIOs(factors);
    rawstat.SetSeqReadBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetSeqWriteIOs(factors);
    rawstat.SetSeqWriteBW(factors * (Constant::DEFAULT_IOSIZE));
    rawstat.SetSeqTotalRT((factors * (Constant::META_RT_READ)) +
            (factors * (Constant::META_RT_WRITE)));

    arbitratee = new Arbitratee(objkey, tgtdiskid, subpoolkey,
            rawstat, IOStat::IOST_raw, direction, cap, cycle, epoch);
    YWB_ASSERT(arbitratee != NULL);

    return arbitratee;
}

ywb_status_t
MigrationTest::ReceiveArbitratee(
        ywb_uint32_t subpoolid, ywb_uint64_t srcdiskid,
        ywb_uint64_t tgtdiskid, ywb_uint64_t inodeid,
        ywb_uint32_t cap, ywb_uint32_t direction,
        ywb_uint64_t cycle, ywb_uint64_t epoch)
{
    ywb_status_t ret = YWB_SUCCESS;
    Migration* migration = NULL;
    Arbitratee* arbitratee = NULL;

    migration = GetMigration();
    arbitratee = GenerateNewArbitratee(subpoolid, srcdiskid,
            tgtdiskid, inodeid, cap, direction, cycle, epoch);
    YWB_ASSERT(arbitratee != NULL);
    ret = migration->ReceiveArbitratee(arbitratee);
    AddArbitratee(inodeid, arbitratee);

    return ret;
}

ywb_status_t
MigrationTest::Complete(ywb_uint64_t inodeid, ywb_bool_t more)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    ArbitrateeKey arbitrateekey;
    SubPoolKey* subpoolkey = NULL;
    OBJKey *objkey = NULL;
    Migration* migration = NULL;
    Arbitratee* arbitratee = NULL;

    migration = GetMigration();
    ret = GetArbitratee(inodeid, &arbitratee);
    if(YWB_SUCCESS == ret)
    {
        arbitratee->GetSubPoolKey(&subpoolkey);
        diskkey.SetSubPoolId(subpoolkey->GetSubPoolId());
        diskkey.SetDiskId(arbitratee->GetTargetID());
        arbitratee->GetOBJKey(&objkey);
        arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);
        ret = migration->Complete(diskkey, arbitrateekey, more);
    }

    return ret;
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

    ::ywb_common_initialize();
    create_logger(logfile, "trace");

    testing::AddGlobalTestEnvironment(new ASTMigrationTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
