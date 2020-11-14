#ifndef __AST_HUB_HPP__
#define __AST_HUB_HPP__

#include <string>
#include "common/FsInts.h"
#include "common/FsCommon.h"
#include "FsCommon/YfsMutex.hpp"
#include "FsCommon/YfsCond.h"
#include "common/FsStatus.h"
#include "FsCommon/YfsThread.h"
#include "common/FsAssert.h"
#include "Template/Queue.hpp"
#include "Communication/MessageDefine.h"
#include "ObjectManagement/ChunkStorageStruct.hpp"
#include "AST/ASTUtil.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/AST.hpp"
#include "AST/MessageExpress.hpp"
#include "AST/ASTEvent.hpp"

class TimeScheduleConfig;
class ConfigView;
class TierView;
class SubPoolView;
class DiskView;
class OBJView;
class ControlCenter;
class ASTCommDispatcher;
class CollectLogicalConfigResponse;
class CollectOBJResponseFragment;
class CollectOBJResponse;
class ReportDeleteOBJsMessage;
class ReportDeleteDisksMessage;
class ReportDeleteSubPoolsMessage;
class ReportDeleteOBJMessage;
class ReportDeleteDiskMessage;
class ReportDeleteSubPoolMessage;

#ifdef WITH_INITIAL_PLACEMENT
class PolicyManager;
class TierWiseDiskCandidates;
class NotifyRelocationType;
#endif

/*Refer to class StorageScanThread*/
class HUB : public Thread
{
public:
    enum HUBStatus {
        HS_starting = 0,
        HS_failed,
        /*HUB is running but not serve for AST*/
        HS_running,
        /*HUB is serving for AST*/
        HS_serving_for_ast,
        /*
         * HUB is collecting OBJ IO(of course it is serving for AST now),
         * specially designed for split transmission of collected IO
         * */
        HS_collecting_IO,
        HS_stopping,
        HS_stopped,
    };

    /*will be used for both AST Module's ASTStatus and SubPools' ASTStaus*/
    enum ASTStatus {
        AS_disabled = 0,
        AS_enabling,
        AS_enabled,
        AS_disabling,
    };

    HUB(ControlCenter*);
    virtual ~HUB();

    inline ywb_uint32_t GetHUBStatus()
    {
        ywb_uint32_t status = 0;

        mSelfStatusLock.Lock();
        status = mHUBStatus;
        mSelfStatusLock.Unlock();

        return status;
    }

    inline ywb_uint32_t GetHUBStatusLocked()
    {
        return mHUBStatus;
    }

    inline ywb_uint32_t GetASTStatus()
    {
        ywb_uint32_t status = 0;

        mASTStatusLock.Lock();
        status = mASTStatus;
        mASTStatusLock.Unlock();

        return status;
    }

    inline ywb_uint32_t GetASTStatusLocked()
    {
        return mASTStatus;
    }

    inline ywb_bool_t GetEnableInBatch()
    {
        return mEnableInBatch;
    }

    inline ywb_bool_t GetDisableInBatch()
    {
        return mDisableInBatch;
    }

    inline ywb_bool_t GetEnableOrDisableASTSync()
    {
        return mEnableOrDisableASTSync;
    }

    inline ywb_status_t GetEnableOrDisableASTResult()
    {
        return mEnableOrDisableASTResult;
    }

    inline yfs_sid_t GetSelfServiceId()
    {
        return mSelfServiceId;
    }

    inline void SetSelfServiceId(yfs_sid_t sid)
    {
        mSelfServiceId = sid;
    }

    inline void GetConfigView(ConfigView** configview)
    {
        *configview = mConfigView;
    }

    inline void GetPolicyManager(PolicyManager** policymgr)
    {
        *policymgr = mPolicyMgr;
    }

    inline void GetASTCommDispatcher(ASTCommDispatcher** dispatcher)
    {
        *dispatcher = mASTCommDispatcher;
    }

    inline void GetAST(AST** ast)
    {
        *ast = mAST;
    }

    inline void GetMessageManager(MessageManager** msgmgr)
    {
        *msgmgr = mMsgMgr;
    }

    inline void GetControlCenter(ControlCenter** center)
    {
        *center = mControlCenter;
    }

    inline void SetHUBStatus(ywb_uint32_t val)
    {
        mSelfStatusLock.Lock();
        mHUBStatus = val;
        mSelfStatusLock.Unlock();
    }

    inline void SetHUBStatusLocked(ywb_uint32_t val)
    {
        mHUBStatus = val;
    }

    inline void SetASTStatus(ywb_uint32_t val)
    {
        mASTStatusLock.Lock();
        mASTStatus = val;
        mASTStatusLock.Unlock();
    };

    inline void SetASTSTatusLocked(ywb_uint32_t val)
    {
        mASTStatus = val;
    }

    inline void SetEnableInBatch(ywb_bool_t val)
    {
        mEnableInBatch = val;
    }

    inline void SetDisableInBatch(ywb_bool_t val)
    {
        mDisableInBatch = val;
    }

    inline void SetEnableOrDisableASTSync(ywb_bool_t val)
    {
        mEnableOrDisableASTSync = val;
    }

    inline void SetEnableOrDisableASTResult(ywb_status_t val)
    {
        mEnableOrDisableASTResult = val;
    }

    void Handle();
    void HandlePendingEvent();
    virtual void* entry();
    ywb_status_t Start();
    /*
     * notify the HUB to stop
     * @immediate: whether to stop HUB immediately or not
     * */
    ywb_status_t Stop(ywb_bool_t immediate);

    /*switch it on when ASTMon has been integrated and merged with AST*/
#ifdef WITH_AST_MON_INTEGRATED
    /*REMEMBER to include /protocol/cxx/command_ast.pb.h*/
    /*called by Monitor-OSS AST special config service*/
    void HandleOSSASTSpecialConfig(FsCommand::AST::OSSASTSpecialConfig* config);
    /*called before launching ping request against Monitor*/
    void GenerateOSSASTSpecialConfigResponse(
            FsCommand::AST::OSSASTSpecialConfigResponse* response);
#endif

#ifdef WITH_INITIAL_PLACEMENT
    ywb_status_t UpdateSubPoolRelocationType(SubPoolKey& subpoolkey, ywb_int32_t val);
    void ClearSubPoolRelocationType();
    ywb_status_t RequireSwap(DiskKey& diskkey, ywb_int32_t level, ywb_int32_t direction);
    void ClearSwapRequiredDisks();
    ywb_status_t CancelSwap(DiskKey& diskkey);
    void ClearSwapCancelledDisks();
#endif

    /*event to HUB*/
    void LaunchStopHUBEvent(ywb_bool_t immediate);
    void LaunchAddDiskEvent(DiskKey& diskkey, DiskBaseProp& diskprop);
    void LaunchRemoveDiskEvent(DiskKey& diskkey);
    /*mainly for test*/
    void LaunchEnableASTEvent();
    void LaunchEnableASTEvent(SubPoolKey& subpoolkey);
    /*mainly for test*/
    void LaunchDisableASTEvent(ywb_bool_t immediate);
    void LaunchDisableASTEvent(ywb_bool_t immediate, SubPoolKey& subpoolkey);
    void LaunchEnableASTEchoEvent(ywb_status_t status);
    void LaunchDisableASTEchoEvent(ywb_status_t status);
    void LaunchAddOBJEvent(OBJKey& objkey);
    void LaunchDeleteOBJEvent(OBJKey& objkey);
    void LaunchAccessOBJEvent(OBJKey& objkey, off_t offset,
            size_t len, ywb_uint32_t ios, ywb_uint32_t bw,
            ywb_uint32_t rt, ywb_bool_t read);
    void LaunchCollectConfigViewEvent(ywb_uint32_t seqno);
    void LaunchCollectIOEvent(ywb_uint32_t seqno, ywb_uint32_t flag);
    void LaunchMigrateEvent(OBJKey& objkey, DiskKey& tgtdisk);
    void LaunchMigrationCompletedEvent(OBJKey& objkey,
            ywb_status_t migrationstatus);
#ifdef WITH_INITIAL_PLACEMENT
    void LaunchUpdateRelocationTypeEvent(
            SubPoolKey& subpoolkey, ywb_int32_t val);
    void LaunchSwapRequiredDiskEvent(DiskKey& diskkey,
            ywb_int32_t level, ywb_int32_t direction);
    void LaunchSwapCancelledDiskEvent(DiskKey& diskkey);
#endif

//    /*event from HUB*/
//    void LaunchOBJDeletedEvent();
//    void LaunchDiskRemovedEvent();
//    void LaunchSubPoolRemovedEvent();
//    void LaunchMigrationCompletedEvent();

    ywb_status_t StartMessageService();
    void StopMessageService();
    void CreateASTCommDispatcher();
    void DestroyASTCommDispatcher();

    void WaitUntilEnableASTEchoed();
    void WaitUntilDisableASTEchoed();
    void NotifyEnableASTResult(ywb_status_t ret);
    void NotifyDisableASTResult(ywb_status_t ret);

    void ReportChunk(const ChunkStorageStruct *const css);

    /*whether or not the given event should be pended and processed later*/
    ywb_bool_t ShouldEventPend(Event* event);
    /*Disable AST only if i am the last one with enabled/enabling status*/
    ywb_bool_t ShouldDisableAST(SubPoolKey& subpoolkey);
    /*whether given subpool should be added into collect config view response*/
    ywb_bool_t ShouldSubPoolBeInvolved(SubPoolKey& subpoolkey);
    /*whether given disk should be added into collect config view response*/
    ywb_bool_t ShouldDiskBeInvolved(DiskKey& diskkey);
    /*whether redirection is required*/
    ywb_bool_t ShouldRedirect(OBJKey& srcobj, OBJKey& tgtobj);
    /*mark the OBJ as settled*/
    ywb_bool_t MarkOBJAsSettledIfNeccessary(OBJKey& tgtobj);

    ywb_status_t GetSubPoolASTStatus(SubPoolKey& subpoolkey,
            ywb_uint32_t& status);
    /*will add a new one if no such entry*/
    ywb_status_t SetSubPoolASTStatus(SubPoolKey& subpoolkey,
            ywb_uint32_t status);
    ywb_status_t RemoveSubPoolASTStatus(SubPoolKey& subpoolkey);

    ywb_status_t SetSubPoolTimeScheduleConfig(SubPoolKey& subpoolkey,
            TimeScheduleConfig& schedule);
    ywb_status_t RemoveSubPoolTimeScheduleConfig(SubPoolKey& subpoolkey);
    ywb_bool_t CheckSubPoolIsScheduledNow(SubPoolKey& subpoolkey);

    ywb_status_t GetSubPoolView(SubPoolKey& subpoolkey,
            SubPoolView** subpoolview);
    ywb_status_t GetSubPoolView(DiskKey& diskkey, SubPoolView** subpoolview);

    ywb_status_t GetDiskView(DiskKey& diskkey, DiskView** diskview);
    ywb_status_t GetDiskView(OBJKey& objkey, DiskView** diskview);

    ywb_status_t GetOBJView(OBJKey& objkey, OBJView** objinfo);

    ywb_status_t GetFirstSubPoolView(
            SubPoolView** subpoolview, SubPoolKey* subpoolkey);
    ywb_status_t GetNextSubPoolView(
            SubPoolView** subpoolview, SubPoolKey* subpoolkey);
    ywb_status_t GetFirstDiskView(DiskView** diskview, DiskKey* diskkey);
    ywb_status_t GetNextDiskView(DiskView** diskview, DiskKey* diskkey);
    ywb_status_t GetFirstOBJView(OBJView** objview, OBJKey* objkey);
    ywb_status_t GetNextOBJView(OBJView** objview, OBJKey* objkey);

    ywb_uint32_t GetOBJsCnt();
    ywb_status_t GetTierWiseDisks(SubPoolKey& subpoolkey,
            ywb_int32_t tiertype, TierWiseDiskCandidates* candidates);

protected:
    ywb_uint32_t GetTableVersion(int type);
    void GetOBJListByDiskId(ywb_uint64_t diskid, std::list<OBJKey> &list);

    void Enqueue(Event*);
    void Enqueue(vector<Event*>);
    void EnqueueFront(Event*);
    void EnqueueFront(vector<Event*> eventvec);
    void EnqueuePending(Event*);
    void EnqueuePending(vector<Event*>);
    void EnqueuePendingFront(Event*);
    void EnqueuePendingFront(vector<Event*> eventvec);

    virtual ywb_status_t EnableAST();
    virtual ywb_status_t DisableAST();
    virtual ywb_status_t HandleEnableASTEcho(ywb_status_t ret);
    virtual ywb_status_t HandleDisableASTEcho(ywb_status_t ret);
    virtual ywb_status_t AddDisk(DiskKey& diskkey, DiskBaseProp& diskprop);
    virtual ywb_status_t RemoveDisk(DiskKey& diskkey);
    virtual ywb_status_t AddOBJ(OBJKey& objkey);
    virtual ywb_status_t DeleteOBJ(OBJKey& objkey);
    virtual ywb_status_t AccessOBJ(OBJKey& objkey, off_t offset,
            size_t len, ywb_uint32_t ios, ywb_uint32_t bw,
            ywb_uint32_t rt, ywb_bool_t read);

    void ResetOBJRespFree();
    void SwitchOBJRespFree();
    void ResetOBJRespFragmentBatchFree();
    void SwitchOBJRespFragmentBatchFree();
    /*
     * ONLY SubPools with enabled ASTStatus will be returned,
     * SubPools filter will be enforced locally
     * */
    virtual ywb_status_t CollectConfigView();
    /*
     * ONLY OBJs belongs to SubPools with enabled ASTStatus will be
     * returned, SubPools filter will be offloaded to GetFirstOBJView
     * and GetNextOBJView.
     *
     * @firstrequest: whether this is the very first collect io request
     *                or subsequent collect io request, used for collect
     *                io response fragmentation
     * */
    virtual ywb_status_t CollectIO(ywb_status_t firstrequest);
    virtual ywb_status_t Migrate(OBJKey& objkey, DiskKey& tgtdisk);
    virtual ywb_status_t CompleteMigrate(OBJKey& objkey, ywb_status_t status);
#ifdef WITH_INITIAL_PLACEMENT
    virtual ywb_status_t BuildRelocationTypeNotification();
    virtual ywb_status_t BuildSwapRequiredDisksNotification();
    virtual ywb_status_t BuildSwapCancelledDisksNotification();
#endif

    /*message related*/
    virtual void SendRemovedSubPool(ReportDeleteSubPoolMessage* msg);
    virtual void SendRemovedDisk(ReportDeleteDiskMessage* msg);
    virtual void SendRemovedOBJ(ReportDeleteOBJMessage* msg);
    virtual void SendConfig();
    virtual void SendIO();
#ifdef WITH_INITIAL_PLACEMENT
    virtual void SendRelocationTypeNotification();
    virtual void SendSwapRequiredDisks();
    virtual void SendSwapCancelledDisks();
#endif

    ywb_status_t AddIntoOBJ2SrcDiskTable(OBJKey& objkey, DiskKey& tgtdisk);
    ywb_status_t GetSrcDiskByOBJKey(OBJKey& objkey, DiskKey* diskkey);
    ywb_status_t RemoveFromOBJ2SrcDiskTable(OBJKey& objkey);

    ywb_status_t MarkOBJAsSrcUnsettled(OBJKey& objkey, DiskKey& diskkey);
    ywb_status_t GetSrcUnsettledOBJ(OBJKey& objkey, DiskKey& diskkey);
    ywb_status_t RemoveSrcUnsettleOBJ(OBJKey& objkey);

    ywb_status_t MarkOBJAsTgtUnsettled(OBJKey& objkey, DiskKey& diskkey);
    ywb_status_t GetTgtUnsettledOBJ(OBJKey& objkey, DiskKey& diskkey);
    ywb_status_t RemoveTgtUnsettleOBJ(OBJKey& objkey);

    ywb_status_t MoveOBJFromSrc2Tgt(DiskKey& srcdisk,
            OBJKey& srcobj, DiskKey& tgtdisk, OBJKey& tgtobj);

private:
    Mutex mSelfStatusLock;
    Cond mSelfStatusCond;
    Mutex mASTStatusLock;
    Cond mASTStatusCond;
    /*protect the config view from change when collecting OBJ IO*/
    RWLock mConfigLock;
    /*HUB itself's status*/
    ywb_uint32_t mHUBStatus;
    /*
     * AST Module's status(Not Pool's AST Status)
     *
     * All Pools will share the same AST Module, it means that
     * when EnableAST on one pool for the very first time, AST
     * Module must be started, and all successive EnableAST on
     * any pool will not start AST Module any more except the
     * AST Module already been teared down.
     * */
    ywb_uint32_t mASTStatus;
    /*
     * whether to EnableAST on all SubPools in batch,
     * no lock protection for no sharing between threads
     * */
    ywb_bool_t mEnableInBatch;
    /*
     * whether to DisableAST on all SubPools in batch,
     * no lock protection for no sharing between threads
     * */
    ywb_bool_t mDisableInBatch;
    /*
     * wait synchronously until EnableAST/DisableAST responded before
     * handling next event.
     * */
    ywb_bool_t mEnableOrDisableASTSync;
    /*
     * result of EnableAST/DisableAST, only used when @mEnableOrDisableASTSync
     * is set to ywb_true
     * */
    ywb_status_t mEnableOrDisableASTResult;
    /*
     * All SubPools' ASTStatus are maintained here
     *
     * Currently this field will not be manipulated by other threads
     * except HUB itself.
     * */
    map<SubPoolKey, ywb_uint32_t, SubPoolKeyCmp> mSubPoolASTStatus;
    /*
     * SubPools and their TimeScheduleConfig
     *
     * Contains ONLY those SubPools with their own TimeScheduleConfig
     * (for those SubPools without their own TimeScheduleConfig will
     * use OSS wise TimeScheduleConfig which is stored in @mASTOption)
     *
     * Currently this field will be manipulated by HUB thread itself,
     * so requires no lock
     * */
    map<SubPoolKey, TimeScheduleConfig, SubPoolKeyCmp> mSubPoolTimeSchedules;
#ifdef WITH_AST_MON_INTEGRATED
    /*global ASTOption*/
    /*REMEMBER to include Monitor/agents/ASTAgent.hpp*/
    ASTOption mASTOption;
#endif

#ifdef WITH_INITIAL_PLACEMENT
    /*
     * manage all subpools with relocation type changed in current cycle,
     * set whenever one subpool changes its initial placement policy
     * */
    map<SubPoolKey, ywb_int32_t, SubPoolKeyCmp> mSubPoolRelocationTypes;
    /*
     * NOTE: though NotifySwapRequirement and NotifySwapCancellation has same
     * fields as here, they can not be combined into one!!!
     * */
    /*
     * manage all disks requiring for swap in current cycle, the value is a
     * combination of swap level and swap direction(level << 16 + direction)
     *
     * set whenever one disk requires for swap or disk's swap level changes,
     * cleared whenever all its entries has been retrieved
     * */
    map<DiskKey, ywb_int32_t, DiskKeyCmp> mSwapRequiredDisks;
    /*
     * manage all disks canceling swap in current cycle
     *
     * set whenever one disk no longer requires for swap or disk is in removing/
     * removed state, cleared whenever all its entries has been retrieved
     * */
    list<DiskKey> mSwapCancelledDisks;
#endif
    ywb_uint32_t mCollectConfigSeq;
    ywb_uint32_t mCollectIOSeq;
    /*total number of OBJs to be collected by CollectIO*/
    ywb_uint32_t mToBeCollectedOBJsNum;
    /*number of OBJs already been collected*/
    ywb_uint32_t mAlreadyCollectedOBJsNum;
    yfs_sid_t mSelfServiceId;
    /*migrating OBJ to source disk mapping table*/
    map<OBJKey, DiskKey, OBJKeyCmp> mMigratingOBJ2SrcDisk;

    /*
     * Unsettled OBJs are those OBJs which are migrated from source disk
     * to target disk, but the MDS may have no awareness of the migration
     * and new IO against OSS may have stale metadata which may leads to
     * failure of access. So for IO against those migrated OBJs, redirection
     * is required if stale metadata is used.
     *
     * @mSrcUnsettledOBJs: all unsettled OBJs of which the source disk is
     * @mTgtUnsettledOBJs: all unsettled OBJs of which the target disk is
     *
     * If one OBJ is migrated, it MUST be added into both @mSrcUnsettledOBJs
     * and @mTgtUnsettledOBJs.
     *
     * If IO against unsettled OBJ is coming with stale metadata, then search
     * from @mSrcUnsettledOBJs for target disk and redirect the IO to target
     * disk.
     *
     * If IO against unsettled OBJ is comming with latest metadata, then
     * search from @mTgtUnsettledOBJs for source disk and delete it from both
     * @mSrcUnsettledOBJs and @mTgtUnsettledOBJs.
     * */
    /*source OBJKey and target disk map*/
    std::map<OBJKey, DiskKey, OBJKeyCmp> mSrcUnsettledOBJs;
    /*target OBJKey and source disk map*/
    std::map<OBJKey, DiskKey, OBJKeyCmp> mTgtUnsettledOBJs;
    /*lock for protection of @mSrcUnsettledOBJs*/
    Mutex mSrcUnsettledLock;
    /*lock for protection of @mTgtUnsettledLock*/
    Mutex mTgtUnsettledLock;

    Message* mEnableASTMsg;
    Message* mDisableASTMsg;
    CollectLogicalConfigResponse* mCollectConfigResp;
    /*
     * Collect OBJ IOs response will consume large amount of memory if all
     * in one response, so fragmentation is required, multiple responses
     * will be returned.
     *
     * But each Collect OBJ IOs response will follow this path: HUB ->
     * Message Manager -> AST, which involve several messages and thread
     * switching, if we combine several response together and operate in
     * batch, then these batched response will only involve same numbers
     * of messages and thread switching as one response.
     *
     * So following model(time sequence) will be adopted:
     *          AST                         Message Manager                   HUB
     *
     * START
     * Collect OBJ IOs request(1st) ——————> Message transfer ——————>
     *                                                       <—————— send batched response(1st)
     * Part 1 of batched response   <—————— Message transfer
     * Part 1 of batched response   <—————— Message transfer
     *          ......              <—————— Message transfer
     * Part m of batched response   <—————— Message transfer
     * Collect OBJ IOs request(2nd) ——————> Message transfer ——————>
     *              |
     *              V
     * Resolve batched responses(1st)
     *                                                       <—————— send batched response(2nd)
     * Part 1 of batched response   <—————— Message transfer
     * Part 1 of batched response   <—————— Message transfer
     *          ......              <—————— Message transfer
     * Part m of batched response   <—————— Message transfer
     * Collect OBJ IOs request(3rd) ——————> Message transfer ——————>
     *              |
     *              V
     * Resolve batched responses(2nd)
     *                                                       <—————— send batched response(3rd)
     * Part 1 of batched response   <—————— Message transfer
     * Part 1 of batched response   <—————— Message transfer
     *          ......              <—————— Message transfer
     * Part m of batched response   <—————— Message transfer
     *
     *           ......                     ......                          ......
     *
     * Collect OBJ IOs request(last)——————> Message transfer ——————>
     *              |
     *              V
     * Resolve batched responses(last - 1)
     *                                                       <—————— send batched response(last)
     * Part 1 of batched response   <—————— Message transfer
     * Part 1 of batched response   <—————— Message transfer
     *          ......              <—————— Message transfer
     * Part m of batched response   <—————— Message transfer
     *              |
     *              V
     * Resolve batched response(last)
     * END
     *
     * Here, two CollectOBJResponse arrays will be used, one is for HUB collecting
     * OBJ IOs response, and the other one is for Message Manager(or AST) to
     * resolve from, so those two arrays will work in ping-pong mode.
     * */
    CollectOBJResponse* mCollectOBJResp;
    CollectOBJResponse* mCollectOBJResp2;
    /*
     * currently available Collect OBJ IOs response, either
     * mCollectOBJResp or mCollectOBJResp2, will be set to
     * mCollectOBJResp at initial state
     * */
    CollectOBJResponse* mCollectOBJRespFree;
    CollectOBJResponseFragment* mCollectOBJRespFragmentBatch[Constant::OBJ_IO_RESPONSE_BATCH_NUM];
    CollectOBJResponseFragment* mCollectOBJRespFragmentBatch2[Constant::OBJ_IO_RESPONSE_BATCH_NUM];
    /*
     * currently available Collect OBJ IOs response batch,
     * either &mCollectOBJRespsBatch or &mCollectOBJRespsBatch2,
     * will be set to &mCollectOBJRespsBatch at initial state
     * */
    CollectOBJResponseFragment** mCollectOBJRespFragmentBatchFree;
#ifdef WITH_INITIAL_PLACEMENT
    /*Notify AST about each pool's relocation type*/
    NotifyRelocationType* mRelocationTypeNotification;
    /*Notify AST about swap-required disks*/
    NotifySwapRequirement* mSwapRequirementNotification;
    /*Notify AST about swap-cancelled disks*/
    NotifySwapCancellation* mSwapCancellationNotification;
#endif
    Queue<Event> mEventContainer;
    /*pending events are accommodated here*/
    Queue<Event> mPendingEventContainer;
    ConfigView* mConfigView;
    PolicyManager* mPolicyMgr;
    ASTCommDispatcher* mASTCommDispatcher;
    AST* mAST;
    MessageManager* mMsgMgr;
    ControlCenter* mControlCenter;
};

class TimeRange
{
public:
    TimeRange() : mStart(0), mEnd(86401)
    {

    }

    TimeRange(ywb_uint32_t start, ywb_uint32_t end)
    {
        YWB_ASSERT(start >= 0);
        YWB_ASSERT(end <= 86401);
        YWB_ASSERT(start < end);
        mStart = start;
        mEnd = end;
    }

    TimeRange(const TimeRange& range)
    {
        mStart = range.mStart;
        mEnd = range.mEnd;
    }

    TimeRange& operator= (const TimeRange& range)
    {
        if(this == &range)
            return *this;

        mStart = range.mStart;
        mEnd = range.mEnd;

        return *this;
    }

    /*sort on basis of @mStart*/
    bool operator<(const TimeRange& range)
    {
        if(mStart == range.mStart)
        {
            return mEnd <= range.mEnd;
        }
        else
        {
            return mStart < range.mStart;
        }
    }

    bool operator==(const TimeRange& range)
    {
        return ((mStart == range.mStart) && (mEnd == range.mEnd));
    }

    bool operator!=(const TimeRange& range)
    {
        return ((mStart != range.mStart) || (mEnd != range.mEnd));
    }

    inline ywb_uint32_t GetStart()
    {
        return mStart;
    }

    inline ywb_uint32_t GetEnd()
    {
        return mEnd;
    }

    inline void SetStart(ywb_uint32_t start)
    {
        mStart = start;
    }

    inline void SetEnd(ywb_uint32_t end)
    {
        mEnd = end;
    }

private:
    /*in measure of seconds*/
    ywb_uint32_t mStart;
    /*in measure of seconds, excluding*/
    ywb_uint32_t mEnd;
};

class TimeRangeCmp
{
public:
    static bool TimeRangeLessThan(
            TimeRange range1, TimeRange range2)
    {
        if(range1.GetStart() == range2.GetStart())
        {
            return range1.GetEnd() <= range2.GetEnd();
        }
        else
        {
            return range1.GetStart() < range2.GetStart();
        }
    }
};

class TimeScheduleConfig
{
public:
    TimeScheduleConfig() : mWeekDays(255)
    {
        TimeRange fulldayrange(0, 86401);
        mRangeList.push_back(fulldayrange);
    }

    TimeScheduleConfig(ywb_uint32_t weekdays) : mWeekDays(weekdays)
    {
        TimeRange fulldayrange(0, 86401);
        mRangeList.push_back(fulldayrange);
    }

    TimeScheduleConfig(const TimeScheduleConfig& config)
    {
        mWeekDays = config.mWeekDays;
        mRangeList.assign(config.mRangeList.begin(), config.mRangeList.end());
    }

    TimeScheduleConfig& operator= (const TimeScheduleConfig& config)
    {
        if(this == &config)
            return *this;

        mWeekDays = config.mWeekDays;
        mRangeList.assign(config.mRangeList.begin(), config.mRangeList.end());

        return *this;
    }

    virtual ~TimeScheduleConfig()
    {
        mRangeList.clear();
    }

    inline ywb_uint32_t GetWeekDays()
    {
        return mWeekDays;
    }

    inline void SetWeekDays(ywb_uint32_t weekdays)
    {
        YWB_ASSERT(weekdays <= 255);
        mWeekDays = weekdays;
    }

    static ywb_bool_t TimeIsScheduled(TimeScheduleConfig *schedule);

    /*@newtimeschedule: indicate this is a new time schedule if set*/
    ywb_status_t AddTimeRange(TimeRange& newrange, ywb_bool_t newtimeschedule);
    ywb_status_t AddTimeRanges(vector<TimeRange>& ranges);
    ywb_bool_t CheckTimeScheduleConfigEqual(vector<TimeRange>& timeranges,
            ywb_uint32_t weekdays);
    void Reset();

private:
    /*all time ranges have no overlap*/
    std::vector<TimeRange> mRangeList;
    /*in bitmap format, start from Sunday and the last bit will be ignored*/
    ywb_uint8_t mWeekDays;
};


class AccessTrack
{
public:
    AccessTrack() : mLastOffset(0), mLastLen(0)
    {

    }

    AccessTrack(const AccessTrack& track)
    {
        this->mLastOffset = track.mLastOffset;
        this->mLastLen = track.mLastLen;
    }

    AccessTrack& operator= (const AccessTrack& track)
    {
        if(this == &track)
            return *this;

        this->mLastOffset = track.mLastOffset;
        this->mLastLen = track.mLastLen;

        return *this;
    }

    virtual ~AccessTrack() {}

    inline off_t GetLastOffset()
    {
        return mLastOffset;
    }

    inline size_t GetLastLen()
    {
        return mLastLen;
    }

    inline void SetLastOffset(off_t offset)
    {
        mLastOffset = offset;
    }

    inline void SetLastLen(size_t len)
    {
        mLastLen = len;
    }

    inline void Reset()
    {
        mLastOffset = 0;
        mLastLen = 0;
    }

private:
    off_t mLastOffset;
    size_t mLastLen;
};

class AccessModel
{
public:
    enum Model{
        AM_none = 0,
        AM_rnd = 0x0001,
        AM_seq = 0x0002,
        AM_read = 0x0004,
        AM_write = 0x0008,
    };

    AccessModel(string name) : mName(name)  {}
    virtual ~AccessModel() {}

    virtual ywb_uint32_t DeterminAccessModel(AccessTrack* history,
            off_t curoffset, size_t curlen, ywb_bool_t read) = 0;

private:
    string mName;
};

class SpinningDiskAccessModel : public AccessModel
{
public:
    SpinningDiskAccessModel() : AccessModel("SpinningDisk") {}
    virtual ~SpinningDiskAccessModel() {}

    ywb_uint32_t DeterminAccessModel(AccessTrack* history,
                off_t curoffset, size_t curlen, ywb_bool_t read);

private:

};

class SSDAccessModel : public AccessModel
{
public:
    SSDAccessModel() : AccessModel("SSD") {}
    virtual ~SSDAccessModel() {}

    ywb_uint32_t DeterminAccessModel(AccessTrack* history,
                off_t curoffset, size_t curlen, ywb_bool_t read);

private:

};

class OBJView
{
public:
    OBJView() : mRemainingRT(0), mStat(), mAccess()
    {

    }

    OBJView(ywb_uint32_t remainingrt, ChunkIOStat& stat, AccessTrack& track) :
        mRemainingRT(remainingrt), mStat(stat), mAccess(track)
    {

    }

    virtual ~OBJView() {}

    inline ywb_uint32_t GetRemainingRT()
    {
        return mRemainingRT;
    }

    inline void GetChunkIOStat(ChunkIOStat** stat)
    {
        *stat = &mStat;
    }

    inline void GetAccessTrack(AccessTrack** history )
    {
        *history = &mAccess;
    }

    /*@model: comes from AccessModel::Model*/
    void Update(ywb_uint32_t model, off_t offset, size_t len,
            ywb_uint32_t ios, ywb_uint32_t bw, ywb_uint32_t rt);
    void Reset();

private:
//    OBJKey mKey;
    /*
     * the passed in @rt in Update() method is in measure of usec,
     * but response time in ChunkIOStat is in measure of msec,
     * so must transfer @rt into msec at first, but the remaining
     * less than 1 msec must be remembered, in measure of usec
     **/
    ywb_uint32_t mRemainingRT;
    ChunkIOStat mStat;
    AccessTrack mAccess;
};

class DiskView
{
public:
    enum
    {
        DV_delay_deleted = 0,
    };

    DiskView(DiskBaseProp diskprop) : mProp(diskprop),
            mTierType(Tier::TIER_cnt), mPinnedNum(0)
    {
        mOBJs.clear();

        if((DiskBaseProp::DCT_ent == diskprop.GetDiskClass()) ||
                (DiskBaseProp::DCT_sata == diskprop.GetDiskClass()))
        {
            mModel = new SpinningDiskAccessModel();
            YWB_ASSERT(mModel != NULL);
        }
        else
        {
            mModel = new SSDAccessModel();
            YWB_ASSERT(mModel != NULL);
        }

        mFlag.Reset();
    }

    virtual ~DiskView()
    {
        OBJView* obj = NULL;

        mIt = mOBJs.begin();
        for(; mIt != mOBJs.end();)
        {
            obj = mIt->second;
            mIt++;
            delete obj;
            obj = NULL;
        }

        mOBJs.clear();
        if(mModel)
        {
            delete mModel;
            mModel = NULL;
        }
    }

    inline void GetDiskBaseProp(DiskBaseProp** diskprop)
    {
        *diskprop = &mProp;
    }

    inline ywb_uint32_t GetTierType()
    {
        return mTierType;
    }

    inline ywb_uint32_t GetOBJCnt()
    {
        return mOBJs.size();
    }

    inline ywb_uint32_t GetPinnedNum()
    {
        return mPinnedNum;
    }

    inline void SetPinnedNum(ywb_uint32_t val)
    {
        mPinnedNum = val;
    }

    inline void IncPinnedNum(ywb_uint32_t val)
    {
        mPinnedNum += val;
    }

    inline void DecPinnedNum(ywb_uint32_t val)
    {
        mPinnedNum = (mPinnedNum > val ? (mPinnedNum - val) : 0);
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

    ywb_status_t AddOBJView(OBJKey& objkey, OBJView* objinfo);
    ywb_status_t GetOBJView(OBJKey& objkey, OBJView** objinfo);
    ywb_status_t DeleteOBJView(OBJKey& objkey);

    ywb_status_t UpdateOBJView(OBJKey& objkey,
            off_t offset, size_t len, ywb_uint32_t ios,
            ywb_uint32_t bw, ywb_uint32_t rt, ywb_bool_t read);

    ywb_status_t GetFirstOBJView(OBJView** objview, OBJKey* objkey);
    ywb_status_t GetNextOBJView(OBJView** objview, OBJKey* objkey);

    ywb_uint32_t DetermineDiskTier();

private:
//    DiskKey mkey;
    DiskBaseProp mProp;
    ywb_uint32_t mTierType;
    std::map<OBJKey, OBJView*, OBJKeyCmp> mOBJs;
    std::map<OBJKey, OBJView*, OBJKeyCmp>::iterator mIt;
    /*I do think AccessModel should be disk specific*/
    AccessModel* mModel;
    /*if @mPinnedNum not equals to 0, then the DiskView could not be deleted*/
    ywb_uint32_t mPinnedNum;
    Flag mFlag;
};

/*
 * introduce TierView to support initial placement policy, it has
 * a thin logic, the life cycle of all DiskViews managed here
 * are dependent on SubPoolView, so REMEMBER to do not de-construct
 * them here
 * */
class TierView
{
public:
    TierView() : mTierType(Tier::TIER_cnt) {}
    TierView(ywb_uint32_t tiertype) : mTierType(tiertype) {}

    virtual ~TierView()
    {
        /*it is the SubPoolView's duty to de-construct DiskViews here*/
        mDisks.clear();
    }

    inline ywb_uint32_t GetTierType()
    {
        return mTierType;
    }

    ywb_status_t AddDiskView(DiskKey& diskkey, DiskView* diskview);
    ywb_status_t GetDiskView(DiskKey& diskkey, DiskView** diskview);
    ywb_status_t RemoveDiskView(DiskKey& diskkey);

    ywb_uint32_t GetDiskCnt();
    ywb_status_t GetTierWiseDiskCandidates(TierWiseDiskCandidates* cands);

private:
    /*all DiskViews of the same tier*/
    std::map<DiskKey, DiskView*, DiskKeyCmp> mDisks;
    ywb_uint32_t mTierType;
};

class SubPoolView
{
public:
    SubPoolView() : mDelayDeletedDisksNum(0), mCurTierComb(SubPool::TC_cnt)
    {
        mDisks.clear();
    }

    virtual ~SubPoolView()
    {
        std::list<TierView*>::iterator iter;
        TierView* tierview = NULL;
        DiskView* diskview = NULL;

        iter = mTierViews.begin();
        while(mTierViews.end() != iter)
        {
            tierview = *iter;
            iter++;
            delete tierview;
            tierview = NULL;
        }

        mTierViews.clear();
        mIt = mDisks.begin();
        for(; mIt != mDisks.end();)
        {
            diskview = mIt->second;
            mIt++;
            delete diskview;
            diskview = NULL;
        }

        mDelayDeletedDisksNum = 0;
        mDisks.clear();
        mCurTierComb = SubPool::TC_cnt;
    }

    inline ywb_uint32_t GetDelayDeletedDisksNum()
    {
        return mDelayDeletedDisksNum;
    }

    inline ywb_uint32_t GetDiskCnt()
    {
        return mDisks.size();
    }

    inline void IncDelayDeletedDisksNum(ywb_uint32_t val)
    {
        mDelayDeletedDisksNum += val;
    }

    inline void DecDelayDeletedDisksNum(ywb_uint32_t val)
    {
        mDelayDeletedDisksNum = (mDelayDeletedDisksNum > val) ?
                (mDelayDeletedDisksNum - val) : 0;
    }

    ywb_status_t AddDiskView(DiskKey& diskkey, DiskView* diskview);
    ywb_status_t GetDiskView(DiskKey& diskkey, DiskView** diskview);
    ywb_status_t RemoveDiskView(DiskKey& diskkey);

    ywb_status_t AddOBJView(DiskKey& diskkey,
            OBJKey& objkey, OBJView* objinfo);
    ywb_status_t GetOBJView(DiskKey& diskkey,
            OBJKey& objkey, OBJView** objinfo);
    ywb_status_t DeleteOBJView(DiskKey& diskkey, OBJKey& objkey);

    ywb_status_t GetFirstDiskView(DiskView** diskview, DiskKey* diskkey);
    ywb_status_t GetNextDiskView(DiskView** diskview, DiskKey* diskkey);
    ywb_status_t GetFirstOBJView(OBJView** objview, OBJKey* objkey);
    ywb_status_t GetNextOBJView(OBJView** objview, OBJKey* objkey);

    ywb_status_t AddTierView(ywb_uint32_t tiertype);
    ywb_status_t GetTierView(ywb_uint32_t tiertype, TierView** tierview);
    ywb_status_t RemoveTierView(ywb_uint32_t tiertype);

    ywb_uint32_t GetCurTierComb();
    ywb_uint32_t AdjustTierComb(ywb_uint32_t tiertype, ywb_bool_t add);

    ywb_uint32_t GetOBJCnt();

private:
//    SubPoolKey mKey;
    /*number of disks marked as delay-deleted*/
    ywb_uint32_t mDelayDeletedDisksNum;
    /*all DiskViews are managed in flat mode without tier classification*/
    std::map<DiskKey, DiskView*, DiskKeyCmp> mDisks;
    std::map<DiskKey, DiskView*, DiskKeyCmp>::iterator mIt;
    /*introduce TierView for initial placement policy management*/
    std::list<TierView*> mTierViews;
    /*current tier combination*/
    ywb_uint32_t mCurTierComb;
};

class ConfigView
{
public:
    ConfigView(HUB* hub) : mHUB(hub)
    {
        mSubPools.clear();
    }

    virtual ~ConfigView()
    {
        Reset();
        mHUB = NULL;
    }

    inline ywb_uint32_t GetSubPoolCnt()
    {
        return mSubPools.size();
    }

    ywb_status_t GetDiskKeyFromID(ywb_uint64_t diskid, DiskKey* diskkey);
    ywb_status_t GetSubPoolKeyFromID(
            ywb_uint32_t subpoolid, SubPoolKey* subpoolkey);
    ywb_status_t GetSubPoolKeyFromDiskID(ywb_uint64_t diskid,
            SubPoolKey& subpoolkey);

    ywb_status_t GetSubPoolView(SubPoolKey& subpoolkey,
            SubPoolView** subpoolview);
    ywb_status_t GetSubPoolViewLocked(SubPoolKey& subpoolkey,
            SubPoolView** subpoolview);

    ywb_status_t AddDiskView(DiskKey& diskkey, DiskBaseProp& diskprop);
    ywb_status_t GetDiskView(DiskKey& diskkey, DiskView** diskview);
    ywb_status_t GetDiskViewLocked(DiskKey& diskkey, DiskView** diskview);
    ywb_status_t RemoveDiskView(DiskKey& diskkey);

    ywb_status_t AddOBJView(OBJKey& objkey);
    ywb_status_t GetOBJView(OBJKey& objkey, OBJView** objinfo);
    ywb_status_t DeleteOBJView(OBJKey& objkey);

    ywb_status_t GetFirstSubPoolView(
            SubPoolView** subpoolview, SubPoolKey* subpoolkey);
    ywb_status_t GetNextSubPoolView(
            SubPoolView** subpoolview, SubPoolKey* subpoolkey);
    ywb_status_t GetFirstDiskView(DiskView** diskview, DiskKey* diskkey);
    ywb_status_t GetNextDiskView(DiskView** diskview, DiskKey* diskkey);
    ywb_status_t GetFirstOBJView(OBJView** objview, OBJKey* objkey);
    ywb_status_t GetNextOBJView(OBJView** objview, OBJKey* objkey);

    /*
     * get tier wise disk candidates for initial placement, lock is necessary
     * @candidates: IN and OUT
     * */
    ywb_status_t GetTierWiseDisks(SubPoolKey& subpoolkey, ywb_int32_t tiertype,
            TierWiseDiskCandidates* candidates);

    ywb_uint32_t GetOBJCnt();
    void Reset();

private:
    /*
     * NOTE:
     * currently AddSubPoolView/RemoveSubPoolView are derived from
     * AddDiskView/RemoveDiskView, they will leave maintenance of
     * @mSubPoolId2Key to AddDiskView/RemoveDiskView instead of
     * themselves, it means that the external should not call both
     * AddSubPoolView/RemoveSubPoolView directly.
     *
     * lock is not necessary for these two routines for they are
     * passively called only by AddDiskView/RemoveDiskView
     * */
    ywb_status_t AddSubPoolView(SubPoolKey& subpoolkey,
            SubPoolView* subpoolview);
    ywb_status_t RemoveSubPoolView(SubPoolKey& subpoolkey);

private:
    std::map<ywb_uint64_t, DiskKey> mDiskID2Key;
    std::map<ywb_uint32_t, SubPoolKey> mSubPoolID2Key;
    std::map<SubPoolKey, SubPoolView*, SubPoolKeyCmp> mSubPools;
    std::map<SubPoolKey, SubPoolView*, SubPoolKeyCmp>::iterator mIt;
    /*
     * protecting for concurrent access of ConfigView.
     *
     * Originally all these fields will only be accessed by HUB thread
     * itself, but to support initial placement policy, other threads
     * in OSS may concurrently access these fields, but currently these
     * threads will read-only access to SubPoolView/TierView/DiskView
     * (no access to OBJView), so lock is enforced only when concurrent
     * access to SubPoolView/TierView/DiskView is possible
     * */
    RWLock mLock;
    HUB* mHUB;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
