#ifndef __AST_MIGRATION_HPP__
#define __AST_MIGRATION_HPP__

#include <pthread.h>
#include <list>
#include <map>
#include "common/FsInts.h"
#include "common/FsStatus.h"
#include "FsCommon/YfsThread.h"
#include "FsCommon/YfsMutex.hpp"
#include "FsCommon/YfsCond.h"
#include "Template/Queue.hpp"
#include "OSSCommon/ControlCenter.hpp"
#include "AST/ASTUtil.hpp"
#include "AST/ASTArb.hpp"

using namespace std;

class Migration;
/*disk migration related performance evaluation*/
class MigrationDisk{
public:
    enum {
        /*disk not participate into migration*/
        MDF_free = 0,
    };

    MigrationDisk() : mCurReadExpected(0), mCurReadObserved(0),
            mCurWriteExpected(0), mCurWriteObserved(0),
            mAccumReadExpected(0), mAccumReadObserved(0),
            mAccumWriteExpected(0), mAccumWriteObserved(0),
            mIterInitialized(0){
        mFlag.SetFlag(MDF_free);
    }

    ~MigrationDisk()
    {
        Destroy();
    }

    inline ywb_uint32_t GetCurReadExpected()
    {
        return mCurReadExpected;
    }

    inline ywb_uint32_t GetCurReadObserved()
    {
        return mCurReadObserved;
    }

    inline ywb_uint32_t GetCurWriteExpected()
    {
        return mCurWriteExpected;
    }

    inline ywb_uint32_t GetCurWriteObserved()
    {
        return mCurWriteObserved;
    }

    inline ywb_uint32_t GetAccumReadExpected()
    {
        return mAccumReadExpected;
    }

    inline ywb_uint32_t GetAccumReadObserved()
    {
        return mAccumReadObserved;
    }

    inline ywb_uint32_t GetAccumWriteExpected()
    {
        return mAccumWriteExpected;
    }

    inline ywb_uint32_t GetAccumWriteObserved()
    {
        return mAccumWriteObserved;
    }

    inline void SetCurReadExpected(ywb_uint32_t val)
    {
        mCurReadExpected = val;
    }

    inline void SetCurReadObserved(ywb_uint32_t val)
    {
        mCurReadObserved = val;
    }

    inline void SetCurWriteExpected(ywb_uint32_t val)
    {
        mCurWriteExpected = val;
    }

    inline void SetCurWriteObserved(ywb_uint32_t val)
    {
        mCurWriteObserved = val;
    }

    inline void SetAccumReadExpected(ywb_uint32_t val)
    {
        mAccumReadExpected = val;
    }

    inline void SetAccumReadObserved(ywb_uint32_t val)
    {
        mAccumReadObserved = val;
    }

    inline void SetAccumWriteExpected(ywb_uint32_t val)
    {
        mAccumWriteExpected = val;
    }

    inline void SetAccumWriteObserved(ywb_uint32_t val)
    {
        mAccumWriteObserved = val;
    }

    inline void SetFlagComb(ywb_uint32_t flag)
    {
        mFlag.SetFlagComb(flag);
    }

    inline void SetFlagUniq(ywb_uint32_t flag)
    {
        mFlag.SetFlagUniq(flag);
    }

    inline void ClearFlag(ywb_uint32_t flag)
    {
        mFlag.ClearFlag(flag);
    }

    inline bool TestFlag(ywb_uint32_t flag)
    {
        return mFlag.TestFlag(flag);
    }

    ywb_status_t AddArbitratee(Arbitratee*);

    ywb_status_t GetFirstArbitratee(Arbitratee**);
    ywb_status_t GetNextArbitratee(Arbitratee**);

    ywb_status_t Destroy();

    /*
     * when migration for new decision window comes, all the currently
     * managed arbitratees can be deleted
     **/
    ywb_status_t Reset();

private:
    /*expected migration performance(read) evaluation*/
    ywb_uint32_t mCurReadExpected;
    /*real migration performance(read) evaluation*/
    ywb_uint32_t mCurReadObserved;
    /*expected migration performance(write) evaluation*/
    ywb_uint32_t mCurWriteExpected;
    /*real migration performance(write) evaluation*/
    ywb_uint32_t mCurWriteObserved;
    /*expected accumulative migration performance(read) evaluation*/
    ywb_uint32_t mAccumReadExpected;
    /*real accumulative migration performance(read) evaluation*/
    ywb_uint32_t mAccumReadObserved;
    /*expected accumulative migration performance(write) evaluation*/
    ywb_uint32_t mAccumWriteExpected;
    /*real accumulative migration performance(write)evaluation*/
    ywb_uint32_t mAccumWriteObserved;

    /*indicates whether the iterator initialized*/
    ywb_uint32_t mIterInitialized;
    /*flags*/
    Flag mFlag;
    /*list of to be migrated OBJs*/
    list<Arbitratee*> mArbitratees;
    list<Arbitratee*>::iterator mIt;
};

class Migration;
/*base class for executor of migration*/
class Migrator{
public:
    Migrator(Migration* migration)
    {
        YWB_ASSERT(migration != NULL);
        mMigration = migration;
    }

    virtual ~Migrator()
    {
        mMigration = NULL;
    }

    inline void GetMigration(Migration** migration)
    {
        *migration = mMigration;
    }

    virtual ywb_status_t Start();
    /*@immediate: whether to stop migrator immediately or not*/
    virtual ywb_status_t Stop(ywb_bool_t immediate);

    /*submit migration task*/
    virtual void Submit(BaseArbitratee* item) = 0;
    virtual void SubmitFront(BaseArbitratee* item) = 0;
    /*any required work before migration*/
    virtual ywb_status_t PrepareMigration(Arbitratee* item) = 0;
    /*do the migration work*/
    virtual ywb_status_t DoMigration(Arbitratee* item) = 0;
    /*any necessary work after migration*/
    virtual ywb_status_t CompleteMigration(Arbitratee* item,
            ywb_status_t migstatus) = 0;
    /*notify the migrator to check any expired arbitratee*/
    virtual void LaunchCheckExpiredEvent() = 0;
    /*notify about migration completion*/
    virtual void LaunchCompleteMigrationEvent(OBJKey objkey, ywb_status_t err) = 0;
    /*get the read/write performance corresponding to source/target disk*/
    virtual ywb_status_t StatPerformance(ywb_uint32_t readPerf,
            ywb_uint32_t writePerf) = 0;

    /*back reference to migration control structure*/
    Migration* mMigration;
};

class DefaultMigrator : public Thread, public Migrator{
public:
    enum Status
    {
        S_stopped = 0,
        S_starting,
        S_running,
        S_stopping,
        S_failed,
    };

    DefaultMigrator(Migration* migration);
    ~DefaultMigrator();

    void Handle();
    void* entry();
    ywb_status_t Start();
    /*
     * DefaultMigrator thread will not stop until complete handling
     * of all its received arbitratees
     *
     * @immediate: whether to stop migrator immediately or not
     **/
    ywb_status_t Stop(ywb_bool_t immediate);

    ywb_uint32_t GetStatus();
    void Submit(BaseArbitratee* item);
    /*call this if the to be submitted item has high priority*/
    void SubmitFront(BaseArbitratee* item);
    ywb_status_t PrepareMigration(Arbitratee* item);
    /*do the migration work*/
    ywb_status_t DoMigration(Arbitratee* item);
    /*any necessary work after migration*/
    ywb_status_t CompleteMigration(Arbitratee* item, ywb_status_t migstatus);
    /*notify the migrator to check any expired arbitratee*/
    void LaunchCheckExpiredEvent();
    /*notify about migration completion*/
    void LaunchCompleteMigrationEvent(OBJKey objkey, ywb_status_t status);
    /*get the read/write performance corresponding to source/target disk*/
    ywb_status_t StatPerformance(ywb_uint32_t readPerf,
            ywb_uint32_t writePerf);

    /*
     * check any expired arbitratee, mainly for avoiding successive
     * arbitratees from being blocked for waiting in-flight
     * arbitratee's migration completion event for a long time
     * */
    ywb_status_t CheckExpired();
    /*any necessary work when one arbitratee expires*/
    ywb_status_t HandleExpired(Arbitratee* item);

    ywb_uint32_t GetInflightNum();

private:
//    /*
//     * list of arbitratees which fail to complete the migration done
//     * notification
//     **/
//    list<Arbitratee*> mBlockers;
    ywb_uint32_t mStatus;
    Queue<BaseArbitratee> mContainer;
    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp> mInflight;
    BaseArbitratee* mFakeArbitrateeForStopMigrator;
    ControlCenter* mControlCenter;
    /*used for status synchronization*/
    Mutex mMutex;
    Cond mCond;
};

/*
 * migration manager
 *
 * #MIGRATOR#
 * any time only one type of migrator can be active, that is mCurMigrator
 * which is set to mDefaultMigrator by default if no other migrator is
 * specified.
 *
 * must call Initialize before hand and the default migrator will be used.
 * one must call RegisterMigrator and SetCurrentMigrator before using a
 * currently nonexistent migrator.
 *
 * #SCHEDULER#
 * current scheduler is self-governing based on migration disk instead
 * of centralized control, it works as below:
 * (1) when receiving one arbitratee(arbitratee is in ARB's scope, but
 *     here we follow that), migration module will dispatch it to target
 *     disk, which is so called migration disk. If the disk is free, an
 *     arbitratee will be migrated immediately, otherwise wait for the
 *     disk becomes free again.
 * (2) when one migration disk reports completion of one migration, next
 *     arbitratee will be migrated if there do exist such arbitratee(s),
 *     otherwise this migration disk will stall and wait for new coming
 *     arbitratee.
 *
 * shortness of this scheduler is that it does not take expected/observed
 * performance into consideration, this will be improvement in future, if
 * the performance not meet expectation, migration disk will be scheduled
 * at a later time(this requires support of time module)
 *
 **/
class Migration{
public:
    Migration(AST* ast)
    {
        mAst = ast;
        mCurMigrator = NULL;
        mDefaultMigrator = NULL;
    }

    virtual ~Migration()
    {
        Destroy();
    }

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    inline void GetCurrentMigrator(Migrator** migrator)
    {
        *migrator = mCurMigrator;
    }

    inline void SetAST(AST* ast)
    {
        YWB_ASSERT(NULL == mAst);
        mAst = ast;
    }

    /*currently used only for test*/
    inline void SetCurrentMigrator(Migrator* migrator)
    {
        mCurMigrator = migrator;
    }

    /*initialization*/
    ywb_status_t Initialize();
    /*finalization*/
    ywb_status_t Finalize();

    /*start a migrator specified by name*/
    ywb_status_t StartMigrator(string name);
    /*
     * stop current migrator
     * @immediate: whether to stop migrator immediately or not
     **/
    ywb_status_t StopMigrator(ywb_bool_t immediate);

    /*receive migration plan*/
    ywb_status_t ReceiveArbitratee(Arbitratee* item);
//    /*schedule next migration, return next migration item*/
//    ywb_status_t ScheduleNextMigration(Arbitratee** item);

//    /*any required work before migration*/
//    ywb_status_t PrepareMigration(Arbitratee* item);
//    /*do the migration work*/
//    ywb_status_t DoMigration(Arbitratee* item);

    /*any necessary work after migration*/
    ywb_status_t Complete(DiskKey& diskkey, ArbitrateeKey& arbitrateekey,
            ywb_status_t migstatus, ywb_bool_t triggermore);
    /*any necessary work when one migration expired*/
    ywb_status_t Expire(DiskKey& diskkey,
            ArbitrateeKey& arbitrateekey,
            ywb_bool_t triggermore);
    /*get the read/write performance corresponding to source/target disk*/
    ywb_status_t StatPerformance(DiskKey& diskkey,
            ywb_uint32_t* readPerf, ywb_uint32_t* writePerf);
    /*launch a new migration*/
    ywb_status_t LaunchMigration(Arbitratee* item);

    /*add migration disk*/
    ywb_status_t AddMigrationDisk(DiskKey& diskkey, MigrationDisk* disk);
    /*remove migration disk*/
    ywb_status_t RemoveMigrationDisk(DiskKey& diskkey);
    /*get migration disk*/
    ywb_status_t GetMigrationDisk(DiskKey& diskkey, MigrationDisk** disk);

    /*register a migrator*/
    ywb_status_t RegisterMigrator(const string& name, Migrator* mig);
    /*unregister a migrator*/
    ywb_status_t DeRegisterMigrator(const string& name);
    /*get the migrator with given name*/
    ywb_status_t GetMigrator(const string& name, Migrator** mig);

    /*remove one item from in flight*/
    ywb_status_t RemoveInflight(ArbitrateeKey&);
    /*remove all inflight items, leave the de-construction work to ARB*/
    void DrainAllInflight();
    /*if the in-flight is empty*/
    ywb_uint32_t GetInflightEmpty();

//    ywb_status_t RemoveInflightOfPreviousWindow(ArbitrateeKey&);

    ywb_status_t Destroy();
    /*
     * @all:
     *      reset all its fields if set to ywb_true;
     *      retain some info for future use otherwise;
     * */
    ywb_status_t Reset(ywb_bool_t all);
    ywb_status_t Init();

private:
    /*info about all disks participated in migration*/
    map<DiskKey, MigrationDisk*, DiskKeyCmp> mMigrationDisks;
    /*all in flight items of current decision window*/
    map<ArbitrateeKey, Arbitratee*, ArbitrateeKeyCmp> mInflight;
    /*all registered migrators*/
    map<string, Migrator*> mMigrators;
    /*current migrator being used*/
    Migrator* mCurMigrator;
    /*default migrator being used if user not specify itself*/
    Migrator* mDefaultMigrator;

    /*reference to AST*/
    AST* mAst;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
