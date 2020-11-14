#include <map>
#include "FsCommon/YfsTime.h"
#include "AST/AST.hpp"
#include "AST/ASTEvent.hpp"
#include "AST/ASTHUB.hpp"
#include "AST/ASTMigration.hpp"

ywb_status_t
MigrationDisk::AddArbitratee(Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;

    mArbitratees.push_back(arbitratee);

    return ret;
}

ywb_status_t
MigrationDisk::GetFirstArbitratee(Arbitratee** arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mArbitratees.empty())
    {
        mIt = mArbitratees.begin();
        *arbitratee = *mIt;
        mIterInitialized = 1;
    }
    else
    {
        *arbitratee = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
MigrationDisk::GetNextArbitratee(Arbitratee** arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIterInitialized != 0)
    {
        mIt++;
        if(mIt != mArbitratees.end())
        {
            *arbitratee = *mIt;
        }
        else
        {
            /*back one step*/
            mIt--;
            *arbitratee = NULL;
            ret = YFS_ENOENT;
        }
    }
    else
    {
        ret = GetFirstArbitratee(arbitratee);
    }

    return ret;
}

ywb_status_t
MigrationDisk::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
//    Arbitratee* arbitratee = NULL;

    /*Arbitratees are allocated and destroyed by ARB*/
//    mIt = mArbitratees.begin();
//    for(; mIt != mArbitratees.end(); )
//    {
//        arbitratee = *mIt;
//        delete arbitratee;
//        arbitratee = NULL;
//    }
    mArbitratees.clear();

    return ret;
}

ywb_status_t
MigrationDisk::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;
//    Arbitratee* arbitratee = NULL;

    mIterInitialized = 0;
    /*FIXME: retain them or not?*/
    mCurReadExpected = 0;
    mCurReadObserved = 0;
    mCurWriteExpected = 0;
    mCurWriteObserved = 0;
    mAccumReadExpected = 0;
    mAccumReadObserved = 0;
    mAccumWriteExpected = 0;
    mAccumWriteObserved = 0;
    mFlag.SetFlag(MDF_free);

    /*Arbitratees are allocated and destroyed by ARB*/
//    mIt = mArbitratees.begin();
//    for(; mIt != mArbitratees.end(); )
//    {
//        arbitratee = *mIt;
//        delete arbitratee;
//        arbitratee = NULL;
//    }
    mArbitratees.clear();

    return ret;
}

ywb_status_t
Migrator::Start()
{
    return YWB_SUCCESS;
}

ywb_status_t
Migrator::Stop(ywb_bool_t immediate)
{
    return YWB_SUCCESS;
}

void
Migrator::Submit(BaseArbitratee* item)
{

}

ywb_status_t
Migrator::PrepareMigration(Arbitratee* item)
{
    return YWB_SUCCESS;
}

ywb_status_t
Migrator::DoMigration(Arbitratee* item)
{
    return YWB_SUCCESS;
}

ywb_status_t
Migrator::CompleteMigration(Arbitratee* item, ywb_status_t migstatus)
{
    return YWB_SUCCESS;
}

ywb_status_t
Migrator::StatPerformance(ywb_uint32_t readPerf,
        ywb_uint32_t writePerf)
{
    return YWB_SUCCESS;
}

DefaultMigrator::DefaultMigrator(Migration* migration) :
    Migrator(migration), mStatus(DefaultMigrator::S_stopped),
    mMutex("DefaultMigratorLock")
{
    AST* ast = NULL;

    mFakeArbitrateeForStopMigrator = new FakeArbitrateeForStop();
    YWB_ASSERT(mFakeArbitrateeForStopMigrator != NULL);

    migration->GetAST(&ast);
    ast->GetControlCenter(&mControlCenter);
    YWB_ASSERT(mControlCenter != NULL);
}

DefaultMigrator::~DefaultMigrator()
{
    BaseArbitratee* arbitratee = NULL;
    while ((arbitratee = mContainer.DequeueNoWait()) != NULL) {
        delete arbitratee;
        arbitratee = NULL;
    }

    if(mFakeArbitrateeForStopMigrator != NULL)
    {
        delete mFakeArbitrateeForStopMigrator;
        mFakeArbitrateeForStopMigrator = NULL;
    }

    mControlCenter = NULL;
}

void
DefaultMigrator::Handle()
{
    BaseArbitratee* basearbitratee = NULL;

    for(basearbitratee = mContainer.Dequeue();
            basearbitratee != NULL;
            basearbitratee = mContainer.Dequeue())
    {
        if(basearbitratee->TestFlag(BaseArbitratee::BAF_fake_complete_mig))
        {
            FakeArbitrateeForCompleteMigration* fakearbitrateeforcompletemig = NULL;
            OBJKey* objkey = NULL;
            Arbitratee* arbitratee = NULL;
            ArbitrateeKey arbitrateekey;
            map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter;

            fakearbitrateeforcompletemig =
                    (FakeArbitrateeForCompleteMigration*)basearbitratee;
            ast_log_debug("DefaultMigrator handle BAF_fake_complete_mig arbitratee@"
                    << fakearbitrateeforcompletemig << ", migration status: "
                    << fakearbitrateeforcompletemig->GetMigrationStatus());
            fakearbitrateeforcompletemig->GetOBJKey(&objkey);
            fakearbitrateeforcompletemig->
                    OBJKey2ArbitrateeKey(objkey, &arbitrateekey);

            iter = mInflight.find(arbitrateekey);
            /*
             * arbitratee may have been expired before receiving corresponding
             * migration completion event, under which circumstance, arbitratee
             * already been removed from @mInflight
             * */
            if(mInflight.end() != iter)
            {
                arbitratee = iter->second;
                mInflight.erase(iter);
                CompleteMigration(arbitratee,
                        fakearbitrateeforcompletemig->GetMigrationStatus());
            }

            delete fakearbitrateeforcompletemig;
            fakearbitrateeforcompletemig = NULL;
        }
        else if(basearbitratee->TestFlag(BaseArbitratee::BAF_fake_check_expired))
        {
            FakeArbitrateeForCheckExpired* fakearbitrateeforcheckexpired = NULL;

            fakearbitrateeforcheckexpired =
                    (FakeArbitrateeForCheckExpired*)basearbitratee;
            ast_log_debug("DefaultMigrator handle BAF_fake_check_expired arbitratee"
                    << fakearbitrateeforcheckexpired);

            CheckExpired();
            delete fakearbitrateeforcheckexpired;
            fakearbitrateeforcheckexpired = NULL;
        }
        else if(basearbitratee->TestFlag(BaseArbitratee::BAF_fake_stop))
        {
            FakeArbitrateeForStop* fakearbitrateeforstop = NULL;

            fakearbitrateeforstop = (FakeArbitrateeForStop*)basearbitratee;
            ast_log_debug("DefaultMigrator handle BAF_fake_stop arbitratee"
                    << fakearbitrateeforstop);
            YWB_ASSERT(fakearbitrateeforstop == mFakeArbitrateeForStopMigrator);
            break;
        }
        else
        {
            Arbitratee* arbitratee = NULL;

            arbitratee = (Arbitratee*)basearbitratee;
            ast_log_debug("DefaultMigrator handle arbitratee" << arbitratee);
            PrepareMigration(arbitratee);
            DoMigration(arbitratee);
        }
    }

    mMutex.Lock();
    YWB_ASSERT(DefaultMigrator::S_stopping == mStatus);
    ast_log_debug("Notifying DefaultMigrator stopped");
    mCond.Signal();
    mMutex.Unlock();
}

void*
DefaultMigrator::entry()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t exit = ywb_false;

    mMutex.Lock();
    mStatus = DefaultMigrator::S_starting;

    /*
     * any DefaultMigrator specific initialization work should
     * be executed here, and the parent thread will be notified
     * about execution result.
     *
     * currently no DefaultMigrator specific initialization work!
     **/

    /*notify the parent thread about the initialization result*/
    if(YWB_SUCCESS != ret)
    {
        mStatus = DefaultMigrator::S_failed;
        exit = ywb_true;
    }
    else
    {
        mStatus = DefaultMigrator::S_running;
    }

    /*notify the AST about executor thread start status*/
    ast_log_debug("Notify DefaultMigrator's start status");
    mCond.Signal();
    mMutex.Unlock();

    if(ywb_false == exit)
    {
        Handle();
    }

    return NULL;
}

ywb_status_t
DefaultMigrator::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    mMutex.Lock();
    if(DefaultMigrator::S_stopped == mStatus)
    {
        create();

        ast_log_debug("Waiting for DefaultMigrator's start status");
        mCond.Wait(mMutex);

        if(DefaultMigrator::S_failed == mStatus)
        {
            ast_log_debug("DefaultMigrator fail to start");
            mMutex.Unlock();
            /*join();*/
            ret = YWB_EAGAIN;
        }
        else
        {
            ast_log_debug("DefaultMigrator successfully start");
            YWB_ASSERT(DefaultMigrator::S_running == mStatus);
            mMutex.Unlock();
        }
    }
    else
    {
        mMutex.Unlock();
        ast_log_debug("Start DefaultMigrator in invalid state");
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
DefaultMigrator::Stop(ywb_bool_t immediate)
{
    ywb_status_t ret = YWB_SUCCESS;

    mMutex.Lock();
    if(DefaultMigrator::S_starting == mStatus ||
            DefaultMigrator::S_running == mStatus ||
            DefaultMigrator::S_failed == mStatus)
    {
        if(DefaultMigrator::S_running == mStatus)
        {
            mStatus = DefaultMigrator::S_stopping;

            if(true == immediate)
            {
                SubmitFront(mFakeArbitrateeForStopMigrator);
            }
            else
            {
                Submit(mFakeArbitrateeForStopMigrator);
            }

            /*
             * wait for DefaultMigrator stop(stop plan/arb/mig
             * while the thread is still alive)
             **/
            ast_log_debug("Wait for DefaultMigrator stopping");
            mCond.Wait(mMutex);
        }

        join();
        mStatus = DefaultMigrator::S_stopped;
    }
    else if(DefaultMigrator::S_stopped != mStatus)
    {
        ast_log_debug("Stop DefaultMigrator in invalid state: " << mStatus);
        ret = YFS_EINVAL;
    }

    mMutex.Unlock();

    return ret;
}

ywb_uint32_t
DefaultMigrator::GetStatus()
{
    return mStatus;
}

void
DefaultMigrator::Submit(BaseArbitratee* item)
{
    if(item != NULL)
    {
        mContainer.Enqueue(item);
    }
}

void
DefaultMigrator::SubmitFront(BaseArbitratee* item)
{
    if(item != NULL)
    {
        mContainer.Enqueue_front(item);
    }
}

ywb_status_t
DefaultMigrator::PrepareMigration(Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_status_t
DefaultMigrator::DoMigration(Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint64_t tgtid = 0;
    HUB* hub = NULL;
    OBJKey* objkey = NULL;
    SubPoolKey* subpoolkey = NULL;
    DiskKey tgtdisk;
    ArbitrateeKey arbitrateekey;

    YWB_ASSERT(arbitratee != NULL);
    hub = mControlCenter->GetHUB();
    YWB_ASSERT(hub != NULL);
    tgtid = arbitratee->GetTargetID();
    arbitratee->GetOBJKey(&objkey);
    arbitratee->GetSubPoolKey(&subpoolkey);
    tgtdisk.SetDiskId(tgtid);
    tgtdisk.SetSubPoolId(subpoolkey->GetSubPoolId());
    arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);
    ast_log_debug("DefaultMigrator do migration, arbitrateekey["
            << arbitrateekey.GetStorageId()
            << ", " << arbitrateekey.GetInodeId()
            << ", " << arbitrateekey.GetChunkId()
            << ", " << arbitrateekey.GetChunkIndex()
            << ", " << arbitrateekey.GetChunkVersion()
            << "], arbitratee@" << arbitratee);

    /*
     * FIXME:
     * mInflightArbitratees is used for managing the mapping
     * between the to be migrated arbitratee's key and arbitratee
     * itself, when complete migration, the mapping table is
     * looked up by arbitrateee's key, see call stack as below:
     * ASTCommDispatcher::ASTMigrateProcedure::Process --->
     * ASTCommDispatcher::LaunchMigrationCompletedEvent --->
     * HUB::LaunchMigrationCompletedEvent --->
     * HUB::Handle --->
     * HUB::CompleteMigrate --->
     * DefaultMigrator::LaunchCompleteMigrationEvent --->
     * DefaultMigrator::Handle --->
     * iter = mInflight.find(arbitrateekey)
     *
     * unfortunately, in ASTCommDispatcher's context, we have
     * OpCtx only which should be translated into Arbitrateekey
     * like:
     *      objkey.SetInodeId(opctx->mInodeId);
     *      objkey.SetChunkId(opctx->mChunkId);
     *      objkey.SetChunkIndex(opctx->mChunkIndex);
     *      objkey.SetChunkVersion(opctx->mChunkVersion);
     *      objkey.SetStorageId(opctx->mSlot);
     * here opctx->mSlot is the target disk id set in HUB::Migrate,
     * not the source disk id! To solve this problem, here we must
     * set ArbitrateeKey::mStorageId to target disk id instead!
     * */
    arbitrateekey.SetStorageId(tgtdisk.GetDiskId());
    mInflight.insert(std::make_pair(arbitrateekey, arbitratee));
    /*
     * FIXME:
     * Shall use message manager as the communication channel???
     * */
    hub->LaunchMigrateEvent(*objkey, tgtdisk);

    return ret;
}

ywb_status_t
DefaultMigrator::CompleteMigration(Arbitratee* arbitratee,
        ywb_status_t migstatus)
{
    ywb_status_t ret = YWB_SUCCESS;
    Migration* migration = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPoolKey *subpoolkey = NULL;
    OBJKey* objkey = NULL;
    Event* event = NULL;
    Executor* exe = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;
    ArbitrateeKey arbitrateekey;

    YWB_ASSERT(arbitratee != NULL);
    GetMigration(&migration);
    migration->GetAST(&ast);
    ast->GetLogicalConfig(&config);
    arbitratee->GetOBJKey(&objkey);
    arbitratee->GetSubPoolKey(&subpoolkey);
    sourcekey.SetDiskId(objkey->GetStorageId());
    sourcekey.SetSubPoolId(subpoolkey->GetSubPoolId());
    targetkey.SetDiskId(arbitratee->GetTargetID());
    targetkey.SetSubPoolId(subpoolkey->GetSubPoolId());
    arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);

    /*
     * if the subpool still exist, then notify the plan/arb/mig about
     * migration completion event, which will further drive another
     * plan against this subpool, otherwise tell the subpool to not
     * generate any more plans against it
     **/
    if(!(config->CheckSubPoolExistence(*subpoolkey)))
    {
        /*
         * FIXME:
         * a better errno indicating that corresponding subpool not exist,
         * and should not generate any new plan against this subpool, use
         * YFS_ENOSPACE temporarily
         * */
        migstatus = YFS_ENOSPACE;
    }

    event = new ExecutorEventSet::MigrationCompletedEvent(
            *subpoolkey, sourcekey, targetkey, arbitrateekey,
            arbitratee->GetEpoch(), migstatus);
    if(event == NULL)
    {
        ast_log_debug("Out of memory!");
        return YFS_EOUTOFMEMORY;
    }
    else
    {
        ast->GetExecutor(&exe);
        exe->LaunchMigrationCompletedEvent(event);
    }

    return ret;
}

void
DefaultMigrator::LaunchCheckExpiredEvent()
{
    BaseArbitratee* fakeevent = NULL;

    fakeevent = new FakeArbitrateeForCheckExpired();
    YWB_ASSERT(fakeevent != NULL);
    mContainer.Enqueue(fakeevent);
    ast_log_debug("launch check expired event@"
            << fakeevent << " against default migrator");
}

void
DefaultMigrator::LaunchCompleteMigrationEvent(
        OBJKey objkey, ywb_status_t status)
{
    BaseArbitratee* fakeevent = NULL;

    fakeevent = new FakeArbitrateeForCompleteMigration(objkey, status);
    YWB_ASSERT(fakeevent != NULL);
    mContainer.Enqueue(fakeevent);
    ast_log_debug("launch complete migration event@"
            << fakeevent << " against default migrator");
}

ywb_status_t
DefaultMigrator::StatPerformance(ywb_uint32_t readPerf, ywb_uint32_t writePerf)
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

/*
 * for simplicity, just check those arbitratees in @mInflight,
 * @mContainer may be concurrently accessed by other thread
 * */
ywb_status_t
DefaultMigrator::CheckExpired()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_time_t timediff;
    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter;
    Arbitratee* arbitratee = NULL;

    ast_log_debug("DefaultMigrator check expired arbitratees");
    if(!(mInflight.empty()))
    {
        iter = mInflight.begin();
        for(; iter != mInflight.end(); )
        {
            arbitratee = iter->second;

            timediff = ywb_time_now() - arbitratee->GetSubmittedTime();
            if(timediff >= arbitratee->GetExpireTime())
            {
                HandleExpired(arbitratee);
                mInflight.erase(iter++);
            }
            else
            {
                iter++;
            }
        }
    }

    return ret;
}

ywb_status_t
DefaultMigrator::HandleExpired(Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    Migration* migration = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPoolKey *subpoolkey = NULL;
    OBJKey* objkey = NULL;
    SubPool* subpool = NULL;
    Event* event = NULL;
    Executor* exe = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;
    ArbitrateeKey arbitrateekey;

    ast_log_debug("DefaultMigrator handle expired arbitratee@" << arbitratee);
    YWB_ASSERT(arbitratee != NULL);
    GetMigration(&migration);
    migration->GetAST(&ast);
    ast->GetLogicalConfig(&config);
    arbitratee->GetOBJKey(&objkey);
    arbitratee->GetSubPoolKey(&subpoolkey);
    sourcekey.SetDiskId(objkey->GetStorageId());
    sourcekey.SetSubPoolId(subpoolkey->GetSubPoolId());
    targetkey.SetDiskId(arbitratee->GetTargetID());
    targetkey.SetSubPoolId(subpoolkey->GetSubPoolId());

    arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);
    ret = config->GetSubPool(*subpoolkey, &subpool);
    /*
     * if the subpool still exist, then notify the plan/arb/mig about
     * migration expiration event
     **/
    if((YWB_SUCCESS == ret))
    {
        event = new ExecutorEventSet::MigrationExpiredEvent(*subpoolkey,
                sourcekey, targetkey, arbitrateekey, arbitratee->GetEpoch());
        if(event == NULL)
        {
            ast_log_debug("Out of memory!");
            ret = YFS_EOUTOFMEMORY;
        }
        else
        {
            ast->GetExecutor(&exe);
            exe->LaunchMigrationExpiredEvent(event);
        }

        config->PutSubPool(subpool);
    }

    return ret;
}

ywb_uint32_t
DefaultMigrator::GetInflightNum()
{
    if(!mInflight.empty())
    {
        return mInflight.size();
    }
    else
    {
        return 0;
    }
}

ywb_status_t
Migration::Initialize()
{
    ywb_status_t ret = YWB_SUCCESS;
    DefaultMigrator* defaultmigrator = NULL;

    defaultmigrator = new DefaultMigrator(this);
    if(NULL == defaultmigrator)
    {
        ret = YFS_EOUTOFMEMORY;
        return ret;
    }

    RegisterMigrator("default", defaultmigrator);
    mDefaultMigrator = defaultmigrator;

    return ret;
}

ywb_status_t
Migration::Finalize()
{
    ywb_status_t ret = YWB_SUCCESS;
    Migrator* defaultmigrator = NULL;

    GetMigrator("default", &defaultmigrator);
    DeRegisterMigrator("default");
    mDefaultMigrator = NULL;

    return ret;
}

ywb_status_t
Migration::StartMigrator(string migratorname)
{
    ywb_status_t ret = YWB_SUCCESS;
    Migrator* migrator = NULL;

    /*migrator has not been started*/
    if(NULL == mCurMigrator)
    {
        ret = GetMigrator(migratorname, &migrator);
        if((YWB_SUCCESS == ret) && (migrator != NULL))
        {
            mCurMigrator = migrator;
        }
        else
        {
            mCurMigrator = mDefaultMigrator;
        }

        ret = mCurMigrator->Start();
    }
    else
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t
Migration::StopMigrator(ywb_bool_t immediate)
{
    ywb_status_t ret = YWB_SUCCESS;

    /*migrator has been started*/
    if(mCurMigrator != NULL)
    {
        mCurMigrator->Stop(immediate);
        mCurMigrator = NULL;
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
Migration::ReceiveArbitratee(Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    OBJKey* objkey = NULL;
    SubPoolKey* subpoolkey = NULL;
    DiskKey sourcekey;
    DiskKey targetkey;
    MigrationDisk* disk = NULL;
    Arbitratee* arbitratee2 = NULL;

    YWB_ASSERT(arbitratee != NULL);
    GetAST(&ast);
    ast->GetLogicalConfig(&config);
    arbitratee->GetOBJKey(&objkey);
    arbitratee->GetSubPoolKey(&subpoolkey);
    sourcekey.SetDiskId(objkey->GetStorageId());
    sourcekey.SetSubPoolId(subpoolkey->GetSubPoolId());
    targetkey.SetDiskId(arbitratee->GetTargetID());
    targetkey.SetSubPoolId(subpoolkey->GetSubPoolId());

    ast_log_debug("Migration receive arbitratee@" << arbitratee);
    /*
     * check whether the source/target disk still exists
     *
     * Note: will not check the OBJ's existence here, offload the
     * check to migrator instead
     **/
    if((!config->CheckDiskExistence(targetkey)) ||
            (!config->CheckDiskExistence(sourcekey))/* ||
            (!config->CheckOBJExistence(*objkey))*/)
    {
        ast_log_debug("Source/Target migration disk does not exist");
        return YFS_EINVAL;
    }

    ret = GetMigrationDisk(targetkey, &disk);
    if(!((YWB_SUCCESS == ret) && (disk != NULL)))
    {
        disk = new MigrationDisk();
        if(NULL == disk)
        {
            ret = YFS_EOUTOFMEMORY;
            return ret;
        }
        else
        {
            AddMigrationDisk(targetkey, disk);
        }
    }

    disk->AddArbitratee(arbitratee);
    arbitratee->SetSubmittedTime(ywb_time_now());
    /*disk is available, launch next migration*/
    if((disk->TestFlag(MigrationDisk::MDF_free)))
    {
        ret = disk->GetNextArbitratee(&arbitratee2);
        YWB_ASSERT((YWB_SUCCESS == ret) && (arbitratee2 != NULL));
        disk->ClearFlag(MigrationDisk::MDF_free);
        ret = LaunchMigration(arbitratee2);
        /*FIXME: any necessary work if fail to launch migration*/
    }
    else
    {
        ret = YWB_SUCCESS;
    }

    return ret;
}

/*
 * NOTE:
 * will leave the migration completed arbitratee there without
 * removing it from associated MigrationDisk
 * */
ywb_status_t
Migration::Complete(DiskKey& diskkey, ArbitrateeKey& arbitrateekey,
        ywb_status_t migstatus, ywb_bool_t triggermore)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t readperf = 0;
    ywb_uint32_t writeperf = 0;
    ywb_uint32_t accumreadperf = 0;
    ywb_uint32_t accumwriteperf = 0;
    MigrationDisk* disk = NULL;
    Arbitratee* arbitratee = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;

    RemoveInflight(arbitrateekey);
    /*
     * it is possible that one migration completion event comes
     * after corresponding migration disk deleted
     * */
    ret = GetMigrationDisk(diskkey, &disk);
    if((YWB_SUCCESS == ret) && (disk != NULL))
    {
        if(YWB_SUCCESS == migstatus)
        {
            /*get performance statistics*/
            StatPerformance(diskkey, &readperf, &writeperf);
            disk->SetCurReadObserved(readperf);
            disk->SetCurWriteObserved(writeperf);
            accumreadperf = disk->GetAccumReadObserved();
            accumwriteperf = disk->GetAccumWriteObserved();
            disk->SetAccumReadObserved(accumreadperf + readperf);
            disk->SetAccumWriteObserved(accumwriteperf + writeperf);
            disk->SetFlagUniq(MigrationDisk::MDF_free);
        }

        if(!triggermore)
        {
            return ret;
        }

        /*FIXME: how to deal with failure case*/

        /*launch next migration if at least one waiting for migration*/
        ret = disk->GetNextArbitratee(&arbitratee);
        if((YWB_SUCCESS == ret) && (arbitratee != NULL))
        {
            GetAST(&ast);
            ast->GetLogicalConfig(&config);
            /*disk still exist*/
            if(config->CheckDiskExistence(diskkey))
            {
                disk->ClearFlag(MigrationDisk::MDF_free);
                ret = LaunchMigration(arbitratee);
            }
            else
            {
                /*delete the migration disk*/
                RemoveMigrationDisk(diskkey);
            }
        }
        else
        {
            ret = YFS_ENODATA;
        }
    }

    return ret;
}

/*
 * NOTE:
 * will leave the expired arbitratee there without
 * removing it from associated MigrationDisk
 * */
ywb_status_t
Migration::Expire(DiskKey& diskkey,
        ArbitrateeKey& arbitrateekey,
        ywb_bool_t triggermore)
{
    ywb_status_t ret = YWB_SUCCESS;
    MigrationDisk* disk = NULL;
    Arbitratee* arbitratee = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;

    RemoveInflight(arbitrateekey);
    ret = GetMigrationDisk(diskkey, &disk);
    if((YWB_SUCCESS == ret) && (disk != NULL))
    {
        disk->SetFlagUniq(MigrationDisk::MDF_free);
        if(!triggermore)
        {
            return ret;
        }

        /*launch next migration if at least one waiting for migration*/
        ret = disk->GetNextArbitratee(&arbitratee);
        if((YWB_SUCCESS == ret) && (arbitratee != NULL))
        {
            GetAST(&ast);
            ast->GetLogicalConfig(&config);
            /*disk still exist*/
            if(config->CheckDiskExistence(diskkey))
            {
                disk->ClearFlag(MigrationDisk::MDF_free);
                ret = LaunchMigration(arbitratee);
            }
            else
            {
                /*delete the migration disk*/
                RemoveMigrationDisk(diskkey);
            }
        }
        else
        {
            ret = YFS_ENODATA;
        }
    }

    return ret;
}

/*FIXME:*/
ywb_status_t
Migration::StatPerformance(DiskKey& diskkey,
        ywb_uint32_t* readPerf, ywb_uint32_t* writePerf)
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_status_t
Migration::LaunchMigration(Arbitratee* arbitratee)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey* objkey = NULL;
    ArbitrateeKey arbitrateekey;

    ast_log_debug("Migration launch migration of arbitratee@" << arbitratee);
    arbitratee->GetOBJKey(&objkey);
    YWB_ASSERT(objkey != NULL);
    arbitratee->OBJKey2ArbitrateeKey(objkey, &arbitrateekey);
    mInflight.insert(make_pair(arbitrateekey, arbitratee));
    arbitratee->SetFlagUniq(BaseArbitratee::BAF_migrating);

    YWB_ASSERT(mCurMigrator != NULL);
    mCurMigrator->Submit(arbitratee);

    return ret;
}

ywb_status_t
Migration::AddMigrationDisk(DiskKey& diskkey, MigrationDisk* disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<DiskKey, MigrationDisk*, DiskKeyCmp>::iterator, bool> pairret;

    pairret = mMigrationDisks.insert(make_pair(diskkey, disk));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t
Migration::RemoveMigrationDisk(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskKey, MigrationDisk*, DiskKeyCmp>::iterator iter;
    MigrationDisk* disk = NULL;

    iter = mMigrationDisks.find(diskkey);
    if(iter != mMigrationDisks.end())
    {
        disk = iter->second;
        mMigrationDisks.erase(iter);
        delete disk;
        disk = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Migration::GetMigrationDisk(DiskKey& diskkey, MigrationDisk** disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskKey, MigrationDisk*, DiskKeyCmp>::iterator iter;

    iter = mMigrationDisks.find(diskkey);
    if(iter != mMigrationDisks.end())
    {
        *disk = iter->second;
    }
    else
    {
        *disk = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Migration::RegisterMigrator(const string& name, Migrator* mig)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<string, Migrator*>::iterator, bool> pairret;

    pairret = mMigrators.insert(make_pair(name, mig));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t
Migration::DeRegisterMigrator(const string& name)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<string, Migrator*>::iterator iter;
    Migrator* mig = NULL;

    iter = mMigrators.find(name);
    if(iter != mMigrators.end())
    {
        mig = iter->second;
        mMigrators.erase(iter);
        delete mig;
        mig = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Migration::GetMigrator(const string& name, Migrator** mig)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<string, Migrator*>::iterator iter;

    iter = mMigrators.find(name);
    if(iter != mMigrators.end())
    {
        *mig = iter->second;
    }
    else
    {
        *mig = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Migration::RemoveInflight(ArbitrateeKey& arbitrateekey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter;

    iter = mInflight.find(arbitrateekey);
    if(iter != mInflight.end())
    {
        mInflight.erase(iter);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

void Migration::DrainAllInflight()
{
    /*DO NOT de-construct in-flight arbitratees here*/
    mInflight.clear();
}

ywb_uint32_t
Migration::GetInflightEmpty()
{
    return mInflight.empty();
}

//ywb_status_t
//Migration::RemoveInflightOfPreviousWindow(ArbitrateeKey& arbitrateekey)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter;
//
//    iter = mInflightOfPreviousWindow.find(arbitrateekey);
//    if(iter != mInflightOfPreviousWindow.end())
//    {
//        mInflightOfPreviousWindow.erase(iter);
//    }
//    else
//    {
//        ret = YFS_ENOENT;
//    }
//
//    return ret;
//}

ywb_status_t
Migration::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = Reset(ywb_true);
    mAst = NULL;

    return ret;
}

ywb_status_t
Migration::Reset(ywb_bool_t all)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskKey, MigrationDisk*, DiskKeyCmp>::iterator iter;
    MigrationDisk* disk = NULL;
    map<string, Migrator*>::iterator iter2;
    Migrator* migrator = NULL;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    DiskKey diskkey;

    ast_log_debug("Migration reset");
    if(ywb_false == all)
    {
        GetAST(&ast);
        YWB_ASSERT(ast != NULL);
        ast->GetLogicalConfig(&config);
        YWB_ASSERT(config != NULL);

        iter = mMigrationDisks.begin();
        for(; iter != mMigrationDisks.end(); )
        {
            diskkey = iter->first;
            disk = iter->second;
            /*
             * to be deleted migration disk must satisfy:
             * (1) does not exist
             * (2) is not in migrating state
             **/
            if((disk->TestFlag(MigrationDisk::MDF_free)) &&
                    (!(config->CheckDiskExistence(diskkey))))
            {
                mMigrationDisks.erase(iter++);
                delete disk;
                disk = NULL;
            }
            else
            {
                /*otherwise, clear all arbitratees in MigrationDisk*/
                disk->Reset();
                iter++;
            }
        }
    }
    else
    {
        iter = mMigrationDisks.begin();
        for(; iter != mMigrationDisks.end(); )
        {
            disk = iter->second;
            mMigrationDisks.erase(iter++);
            delete disk;
            disk = NULL;
        }

        if(!GetInflightEmpty())
        {
            DrainAllInflight();
        }

        YWB_ASSERT(NULL == mCurMigrator);
        iter2 = mMigrators.begin();
        for(; iter2 != mMigrators.end(); )
        {
            migrator = iter2->second;
            mMigrators.erase(iter2++);
            delete migrator;
            migrator = NULL;
        }

        mCurMigrator = NULL;
        mDefaultMigrator = NULL;
    }

    return ret;
}

ywb_status_t
Migration::Init()
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp>::iterator iter;
    SubPoolKey* subpoolkey = NULL;
    DiskKey targetkey;
    MigrationDisk* disk = NULL;
    Arbitratee* arbitratee = NULL;

    /*
     * currently will not add all those arbitratees in @mInflight
     * back into corresponding MigrationDisk though MigrationDisk
     * was reset in Reset(), instead will only care if there are
     * in-flight arbitratees and clear MDF_free flag if so
     *
     * NOTE:
     * if add in-flight arbitratees back to MigrationDisk, then
     * must pay attention to GetNextArbitratee which must skip
     * those arbitratees already in-flight!!!
     * */
    if(!mInflight.empty())
    {
        iter = mInflight.begin();
        for(; iter != mInflight.end(); iter++)
        {
            arbitratee = iter->second;
            arbitratee->GetSubPoolKey(&subpoolkey);
            targetkey.SetDiskId(arbitratee->GetTargetID());
            targetkey.SetSubPoolId(subpoolkey->GetSubPoolId());
            ret = GetMigrationDisk(targetkey, &disk);
            if((YWB_SUCCESS == ret) && (disk != NULL))
            {
                /*disk->AddArbitratee(arbitratee);*/
                disk->ClearFlag(MigrationDisk::MDF_free);
            }
        }
    }

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
