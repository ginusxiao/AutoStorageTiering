#include "protocol/cxx/object.pb.h"
#include "protocol/cxx/basic.pb.h"
#include "OSSCommon/OpCtx.hpp"
#include "ObjectManagement/ChunkStorageStruct.hpp"
#include "FsCommon/YfsTime.h"
#include "AST/MessageImpl.hpp"
#include "AST/ASTMigration.hpp"
#include "AST/ASTCommDispatcher.hpp"
#include "AST/ASTPlacement.hpp"
#include "AST/AST.hpp"
#include "AST/ASTHUB.hpp"
#include "AST/ASTStoragePolicy.hpp"
#include "AST/ASTSwapOut.hpp"

HUB::HUB(ControlCenter* ctrlcenter) : mSelfStatusLock("HUBStatusLock"),
        mASTStatusLock("ASTStatusLock"), mHUBStatus(HS_stopped),
        mASTStatus(AS_disabled), mEnableInBatch(true),
        mDisableInBatch(true), mEnableOrDisableASTSync(ywb_false),
        mEnableOrDisableASTResult(YWB_SUCCESS), mCollectConfigSeq(0),
        mCollectIOSeq(0), mToBeCollectedOBJsNum(0),
        mAlreadyCollectedOBJsNum(0), mSrcUnsettledLock("SrcUnsettledLock"),
        mTgtUnsettledLock("TgtUnsettledLock"), mASTCommDispatcher(NULL),
        mControlCenter(ctrlcenter)
{
    ywb_uint32_t index = 0;

    mSelfServiceId = mControlCenter->GetSelfServiceId();
    mEventContainer.clear();
    mPendingEventContainer.clear();
    mMigratingOBJ2SrcDisk.clear();

    mEnableASTMsg = new Message("ast", Message::OP_enable_ast);
    YWB_ASSERT(mEnableASTMsg != NULL);
    mDisableASTMsg = new Message("ast", Message::OP_disable_ast);
    YWB_ASSERT(mDisableASTMsg != NULL);
    mCollectConfigResp = new CollectLogicalConfigResponse();
    YWB_ASSERT(mCollectConfigResp != NULL);
    mCollectOBJResp = new CollectOBJResponse();
    YWB_ASSERT(mCollectOBJResp != NULL);
    mCollectOBJResp2 = new CollectOBJResponse();
    YWB_ASSERT(mCollectOBJResp2 != NULL);
    mCollectOBJRespFree = mCollectOBJResp;
    for(index = 0; index < Constant::OBJ_IO_RESPONSE_BATCH_NUM; index++)
    {
        mCollectOBJRespFragmentBatch[index] = new CollectOBJResponseFragment();
        YWB_ASSERT(mCollectOBJRespFragmentBatch[index] != NULL);
        mCollectOBJRespFragmentBatch2[index] = new CollectOBJResponseFragment();
        YWB_ASSERT(mCollectOBJRespFragmentBatch2[index] != NULL);
    }

    mCollectOBJRespFragmentBatchFree = mCollectOBJRespFragmentBatch;
    mRelocationTypeNotification = new NotifyRelocationType();
    YWB_ASSERT(mRelocationTypeNotification != NULL);
    mSwapRequirementNotification = new NotifySwapRequirement();
    YWB_ASSERT(mSwapRequirementNotification != NULL);
    mSwapCancellationNotification = new NotifySwapCancellation();
    YWB_ASSERT(mSwapCancellationNotification != NULL);

    mConfigView = new ConfigView(this);
    YWB_ASSERT(mConfigView != NULL);
    mPolicyMgr = new PolicyManager(this);
    YWB_ASSERT(mPolicyMgr != NULL);
    mAST = GetSingleASTInstance();
    YWB_ASSERT(mAST != NULL);
    mMsgMgr = new MessageManager(mAST, mControlCenter);
    YWB_ASSERT(mMsgMgr != NULL);
}

HUB::~HUB()
{
    ywb_uint32_t index = 0;

    YWB_ASSERT(!(HS_starting == mHUBStatus ||
            HS_running == mHUBStatus ||
            HS_serving_for_ast == mHUBStatus ||
            HS_collecting_IO == mHUBStatus ||
            HS_stopping == mHUBStatus));

    mEventContainer.clear();
    mPendingEventContainer.clear();
    mMigratingOBJ2SrcDisk.clear();
    mSrcUnsettledOBJs.clear();
    mTgtUnsettledOBJs.clear();

    if(mEnableASTMsg)
    {
        delete mEnableASTMsg;
        mEnableASTMsg = NULL;
    }

    if(mDisableASTMsg)
    {
        delete mDisableASTMsg;
        mDisableASTMsg = NULL;
    }

    if(mCollectConfigResp)
    {
        delete mCollectConfigResp;
        mCollectConfigResp = NULL;
    }

    if(mCollectOBJResp)
    {
        delete mCollectOBJResp;
        mCollectOBJResp = NULL;
    }

    if(mCollectOBJResp2)
    {
        delete mCollectOBJResp2;
        mCollectOBJResp2 = NULL;
    }

    mCollectOBJRespFree = NULL;
    for(index = 0; index < Constant::OBJ_IO_RESPONSE_BATCH_NUM; index++)
    {
        if(mCollectOBJRespFragmentBatch[index] != NULL)
        {
            delete mCollectOBJRespFragmentBatch[index];
            mCollectOBJRespFragmentBatch[index] = NULL;
        }

        if(mCollectOBJRespFragmentBatch2[index] != NULL)
        {
            delete mCollectOBJRespFragmentBatch2[index];
            mCollectOBJRespFragmentBatch2[index] = NULL;
        }
    }

    mCollectOBJRespFragmentBatchFree = NULL;

    if(mRelocationTypeNotification)
    {
        delete mRelocationTypeNotification;
        mRelocationTypeNotification = NULL;
    }

    if(mSwapRequirementNotification)
    {
        delete mSwapRequirementNotification;
        mSwapRequirementNotification = NULL;
    }

    if(mSwapCancellationNotification)
    {
        delete mSwapCancellationNotification;
        mSwapCancellationNotification = NULL;
    }

    if(mConfigView)
    {
        delete mConfigView;
        mConfigView = NULL;
    }

    if(mPolicyMgr)
    {
        delete mPolicyMgr;
        mPolicyMgr = NULL;
    }

    if(mASTCommDispatcher)
    {
        DestroyASTCommDispatcher();
    }

    if(mAST)
    {
//        delete mAST;
//        mAST = NULL;
        mAST->DecRef();
    }

    if(mMsgMgr)
    {
        delete mMsgMgr;
        mMsgMgr = NULL;
    }

    mControlCenter = NULL;
}

void HUB::Handle()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t status = 0;
    SubPoolKey subpoolkey;
    Event* event = NULL;

    for(event = mEventContainer.Dequeue();
            event != NULL;
            event = mEventContainer.Dequeue())
    {
        ast_log_debug("handle event@" << event
                << " against HUB, type: " << event->GetType());

        if(ShouldEventPend(event))
        {
            ast_log_debug("Current event@" << event << " should pend");
            EnqueuePending(event);
        }
        else
        {
            switch(event->GetType())
            {
            case Event::ET_hub_enable_ast:
            {
                HUBEventSet::EnableASTEvent* enableevent = NULL;

                enableevent = (HUBEventSet::EnableASTEvent*)event;
                if(false == enableevent->InBatchMode())
                {
                    /*EnableAST on individual SubPool*/
                    enableevent->GetSubPoolKey(&subpoolkey);
                    status = GetASTStatus();
                    if(status == HUB::AS_disabled)
                    {
                        /*
                         * start the AST Module, and mark the SubPool as enabling
                         * instead of enabled here(mark it as enabled at the point
                         * of receiving signal of AST Module successfully enabled)
                         * */
                        SetEnableInBatch(false);
                        SetSubPoolASTStatus(subpoolkey, HUB::AS_enabling);
                        EnableAST();
                    }
                    else if(status == HUB::AS_enabled)
                    {
                        /*
                         * mark SubPool as enabled directly
                         * */
                        SetSubPoolASTStatus(subpoolkey, HUB::AS_enabled);
                    }
                    else if(status == HUB::AS_enabling)
                    {
                        SetSubPoolASTStatus(subpoolkey, HUB::AS_enabling);
                    }
                    else if(status == HUB::AS_disabling)
                    {
                        /*
                         * HUB is sure that all SubPools' ASTStatus are set to disabled
                         * or disabling before calling DisableAST, so here set ASTStatus
                         * to enabling will be a signal that AST Module should be enabled
                         * again after disabled for at least one pool calls for EnableAST
                         * */
                        SetSubPoolASTStatus(subpoolkey, HUB::AS_enabling);
                    }
                }
                else
                {
                    /*EnableAST on all SubPools in batch*/
                    status = GetASTStatus();
                    if(status == HUB::AS_disabled)
                    {
                        SetEnableInBatch(true);
                        EnableAST();
                    }
                    else if(status == HUB::AS_disabling)
                    {
                        /*Re-launch EnableAST event internally*/
                        LaunchEnableASTEvent();
                    }
                    else if(status == HUB::AS_enabling)
                    {
                        ast_log_debug("AST is enabling");
                    }
                    else if(status == HUB::AS_enabled)
                    {
                        ast_log_debug("AST already been enabled");
                    }
                }

                break;
            }
            case Event::ET_hub_disable_ast:
            {
                HUBEventSet::DisableASTEvent* disableevent = NULL;

                disableevent = (HUBEventSet::DisableASTEvent*)event;
                if(false == disableevent->InBatchMode())
                {
                    /*DisableAST on individual SubPool*/
                    disableevent->GetSubPoolKey(&subpoolkey);
                    GetSubPoolASTStatus(subpoolkey, status);
                    if(status == HUB::AS_enabled)
                    {
                        /*
                         * Disable AST if this is the last one with enabled ASTStatus
                         * */
                        if(ShouldDisableAST(subpoolkey))
                        {
                            SetDisableInBatch(false);
                            SetSubPoolASTStatus(subpoolkey, HUB::AS_disabling);
                            ret = DisableAST();
                            /*
                             * async enable/disable will be handled with special event,
                             * HUB in stopping state, so HUB thread should exit here
                             * */
                            if((YWB_SUCCESS == ret) &&
                                    (ywb_true == mEnableOrDisableASTSync) &&
                                    (HUB::HS_stopping == mHUBStatus))
                            {
                                event->DecRef();
                                return;
                            }
                        }
                        else
                        {
                            if(HUB::HS_stopping == mHUBStatus)
                            {
                                SetSubPoolASTStatus(subpoolkey, HUB::AS_disabling);
                            }
                            else
                            {
                                SetSubPoolASTStatus(subpoolkey, HUB::AS_disabled);
                            }
                        }
                    }
                    else if(status == HUB::AS_enabling)
                    {
                        /*
                         * now that AST Module is not already enabled, feel free
                         * to set SubPool's ASTStatus to disabled
                         * */
                        SetSubPoolASTStatus(subpoolkey, HUB::AS_disabled);
                    }
                    else if(status == HUB::AS_disabling)
                    {
                        /*
                         * HUB is sure that all SubPools's ASTStatus is disabled
                         * or disabling before calling DisableAST
                         * */
                        /*SetSubPoolASTStatus(subpoolkey, HUB::AS_disabling);*/
                    }
                    else if(status == HUB::AS_disabled)
                    {
                        /*nothing to do*/
                        ;
                    }
                }
                else
                {
                    /*DisableAST on all SubPools*/
                    status = GetASTStatus();
                    if(status == HUB::AS_enabled)
                    {
                        SetDisableInBatch(true);
                        ret = DisableAST();
                        /*
                         * async enable/disable will be handled with special event,
                         * HUB in stopping state, so HUB thread should exit here
                         * */
                        if((YWB_SUCCESS == ret) &&
                                (ywb_true == mEnableOrDisableASTSync) &&
                                (HUB::HS_stopping == mHUBStatus))
                        {
                            event->DecRef();
                            return;
                        }
                    }
                    else if(status == HUB::AS_enabling)
                    {
                        /*Re-launch DisableAST event internally*/
                        LaunchDisableASTEvent(true);
                    }
                    else if(status == HUB::AS_disabling)
                    {
                        ast_log_debug("AST is disabling");
                    }
                    else if(status == HUB::AS_disabled)
                    {
                        ast_log_debug("AST already been disabled");
                    }
                }

                break;
            }
            case Event::ET_hub_enable_ast_echo:
            {
                HUBEventSet::EnableASTEchoEvent* enableastecho = NULL;

                enableastecho = (HUBEventSet::EnableASTEchoEvent*)event;
                HandleEnableASTEcho(enableastecho->GetStatus());

                break;
            }
            case Event::ET_hub_disable_ast_echo:
            {
                HUBEventSet::DisableASTEchoEvent* disableastecho = NULL;

                disableastecho = (HUBEventSet::DisableASTEchoEvent*)event;
                HandleDisableASTEcho(disableastecho->GetStatus());
                if((YWB_SUCCESS == disableastecho->GetStatus()) &&
                        (HUB::HS_stopping == mHUBStatus))
                {
                    event->DecRef();
                    return;
                }

                break;
            }
            case Event::ET_hub_add_disk:
            {
                HUBEventSet::AddDiskEvent* diskevent = NULL;
                DiskKey* diskkey = NULL;
                DiskBaseProp* diskprop = NULL;

                diskevent = (HUBEventSet::AddDiskEvent*)event;
                diskevent->GetDiskKey(&diskkey);
                diskevent->GetDiskProp(&diskprop);
                /*add disk into ConfigView regardless of AST Module's ASTStatus*/
                AddDisk(*diskkey, *diskprop);
                break;
            }
            case Event::ET_hub_remove_disk:
            {
                HUBEventSet::RemoveDiskEvent* diskevent = NULL;
                DiskKey* diskkey = NULL;

                status = GetASTStatus();
                diskevent = (HUBEventSet::RemoveDiskEvent*)event;
                diskevent->GetDiskKey(&diskkey);
                RemoveDisk(*diskkey);

                break;
            }
            case Event::ET_hub_add_obj:
            {
                ywb_status_t ret = YWB_SUCCESS;
                HUBEventSet::AddOBJEvent* objevent = NULL;
                OBJKey* objkey = NULL;
                OBJView* objview = NULL;

                objevent = (HUBEventSet::AddOBJEvent*)event;
                objevent->GetOBJKey(&objkey);
                ret = GetOBJView(*objkey, &objview);
                if(YWB_SUCCESS != ret)
                {
                    /*add OBJ into ConfigView regardless of AST Module's ASTStatus*/
                    AddOBJ(*objkey);
                }

                break;
            }
            case Event::ET_hub_delete_obj:
            {
                HUBEventSet::DeleteOBJEvent* objevent = NULL;
                OBJKey* objkey = NULL;

                status = GetASTStatus();
                objevent = (HUBEventSet::DeleteOBJEvent*)event;
                objevent->GetOBJKey(&objkey);
                DeleteOBJ(*objkey);

                break;
            }
            case Event::ET_hub_access_obj:
            {
                HUBEventSet::AccessOBJEvent* objevent = NULL;
                OBJKey* objkey = NULL;

                objevent = (HUBEventSet::AccessOBJEvent*)event;
                objevent->GetOBJKey(&objkey);
                AccessOBJ(*objkey, objevent->GetOffset(),
                        objevent->GetLen(), objevent->GetIOs(),
                        objevent->GetBW(), objevent->GetRT(),
                        objevent->GetReadOrWrite());

                break;
            }
            case Event::ET_hub_collect_config_view:
            {
                HUBEventSet::CollectConfigViewEvent* configviewevent = NULL;

                status = GetASTStatus();
                configviewevent = (HUBEventSet::CollectConfigViewEvent*)event;
                mCollectConfigSeq = configviewevent->GetSeqNo();
                CollectConfigView();

                break;
            }
            case Event::ET_hub_collect_io:
            {
                HUBEventSet::CollectIOEvent* ioevent = NULL;

                status = GetASTStatus();
                ioevent = (HUBEventSet::CollectIOEvent*)event;
                mCollectIOSeq = ioevent->GetSeqNo();
                if(HUBEventSet::CollectIOEvent::F_prep == ioevent->GetFlag())
                {
                    if((HUB::AS_disabling != status) &&
                            (HUB::AS_disabled != status))
                    {
                        ast_log_debug("Collect OBJ IO preparation");
                        /*indicates it is in process of collecting IO*/
                        mHUBStatus = HUB::HS_collecting_IO;
                        mConfigLock.rdlock();
                        mToBeCollectedOBJsNum = GetOBJsCnt();
                        mAlreadyCollectedOBJsNum = 0;
                    }
                    else
                    {
                        ast_log_debug("Collece OBJ IO preparation comes, but AST is "
                                "in disabling/disabled state");
                    }
                }
                else if(HUBEventSet::CollectIOEvent::F_normal == ioevent->GetFlag())
                {
                    if(0 == mAlreadyCollectedOBJsNum)
                    {
                        /*
                         * CollectIO itself will correctly handle it if AST is in
                         * disabling/disabled state
                         * */
                        CollectIO(ywb_true);
                    }
                    else
                    {
                        CollectIO(ywb_false);
                    }
                }
                else if(HUBEventSet::CollectIOEvent::F_post == ioevent->GetFlag())
                {
                    ast_log_debug("Collect OBJ IO post work");
                    mConfigLock.unlock();
                    /*indicate completion of collecting IO*/
                    mHUBStatus = HUB::HS_serving_for_ast;
#ifdef WITH_INITIAL_PLACEMENT
                    BuildRelocationTypeNotification();
                    SendRelocationTypeNotification();
                    ClearSubPoolRelocationType();
                    /*
                     * ET_hub_require_swap_disk and ET_hub_cancel_swap_disk
                     * event will not be pended
                     *
                     * Tell AST about swap requirement and cancellation, all disks
                     * in @mSwapRequiredDisks/@mSwapCancelledDisks SHOULD be sent
                     * out even though one entry for the same disk exist in
                     * @mSwapCancelledDisks/@mSwapRequiredDisks
                     * */
                    BuildSwapRequiredDisksNotification();
                    SendSwapRequiredDisks();
                    /*REMEMBER to clear @mSwapRequiredDisks*/
                    ClearSwapRequiredDisks();

                    BuildSwapCancelledDisksNotification();
                    SendSwapCancelledDisks();
                    /*REMEMBER to clear @mSwapCancelledDisks*/
                    ClearSwapCancelledDisks();
#endif
                    HandlePendingEvent();
                }
                else
                {
                    YWB_ASSERT(0);
                }

                break;
            }
            case Event::ET_hub_migrate:
            {
                ywb_status_t ret = YWB_SUCCESS;
                HUBEventSet::MigrateEvent* migrateevent = NULL;
                OBJKey* objkey = NULL;
                OBJView* objview = NULL;
                DiskKey srcdisk;
                DiskKey* tgtdisk = NULL;
                DiskView* srcdiskview = NULL;
                DiskView* tgtdiskview = NULL;
                SubPoolKey subpoolkey;
                Migration* migrationmgr = NULL;
                Migrator* migrator = NULL;

                status = GetASTStatus();
                migrateevent = (HUBEventSet::MigrateEvent*)event;
                migrateevent->GetOBJKey(&objkey);
                migrateevent->GetTargetDisk(&tgtdisk);
                srcdisk.SetDiskId(objkey->GetStorageId());
                srcdisk.SetSubPoolId(tgtdisk->GetSubPoolId());

                /*
                 * TODO: how about the OSS is in following statuses:
                 * malfunctioned,
                 * rebalancing,
                 * raid rebuilding,
                 * ...
                 * */

                if(((HUB::AS_disabling != status) &&
                        (HUB::AS_disabled != status)))
                {
                    if(YWB_SUCCESS != mConfigView->GetSubPoolKeyFromDiskID(
                            objkey->GetStorageId(), subpoolkey))
                    {
                        ast_log_info("Discard migration request regarding OBJ ["
                                << objkey->GetStorageId()
                                << ", " << objkey->GetInodeId()
                                << ", " << objkey->GetChunkId()
                                << ", " << objkey->GetChunkIndex()
                                << ", " << objkey->GetChunkVersion()
                                << "] for corresponding SubPool doesn't exist");
                        ret = YFS_ENOTDONE;
                    }
                    else if(!ShouldSubPoolBeInvolved(subpoolkey))
                    {
                        ast_log_info("Discard migration request regarding OBJ ["
                                << objkey->GetStorageId()
                                << ", " << objkey->GetInodeId()
                                << ", " << objkey->GetChunkId()
                                << ", " << objkey->GetChunkIndex()
                                << ", " << objkey->GetChunkVersion()
                                << "] for corresponding SubPool should not be involved now");
                        ret = YFS_ENOTDONE;
                    }
                    else if(YWB_SUCCESS != mConfigView->GetDiskView(srcdisk, &srcdiskview))
                    {
                        ast_log_info("Discard migration request regarding OBJ ["
                                << objkey->GetStorageId()
                                << ", " << objkey->GetInodeId()
                                << ", " << objkey->GetChunkId()
                                << ", " << objkey->GetChunkIndex()
                                << ", " << objkey->GetChunkVersion()
                                << "] for source disk [" << srcdisk.GetSubPoolId()
                                << ", " << srcdisk.GetDiskId() << "] doesn't exist");
                        ret = YFS_ENOTDONE;
                    }
                    else if(YWB_SUCCESS != mConfigView->GetDiskView(*tgtdisk, &tgtdiskview))
                    {
                        ast_log_info("Discard migration request regarding OBJ ["
                                << objkey->GetStorageId()
                                << ", " << objkey->GetInodeId()
                                << ", " << objkey->GetChunkId()
                                << ", " << objkey->GetChunkIndex()
                                << ", " << objkey->GetChunkVersion()
                                << "] for target disk [" << tgtdisk->GetSubPoolId()
                                << ", " << tgtdisk->GetDiskId() << "] doesn't exist");
                        ret = YFS_ENOTDONE;
                    }
                    else if(YWB_SUCCESS != mConfigView->GetOBJView(*objkey, &objview))
                    {
                        ast_log_info("Discard migration request regarding OBJ ["
                                << objkey->GetStorageId()
                                << ", " << objkey->GetInodeId()
                                << ", " << objkey->GetChunkId()
                                << ", " << objkey->GetChunkIndex()
                                << ", " << objkey->GetChunkVersion()
                                << "] for OBJ doesn't exist");
                        ret = YFS_ENOTDONE;
                    }
                    else
                    {
                        srcdiskview->IncPinnedNum(1);
                        tgtdiskview->IncPinnedNum(1);
                        Migrate(*objkey, *tgtdisk);
                        ret = YWB_SUCCESS;
                    }
                }
                else
                {
                    ast_log_info("Discard migration request regarding OBJ ["
                            << objkey->GetStorageId()
                            << ", " << objkey->GetInodeId()
                            << ", " << objkey->GetChunkId()
                            << ", " << objkey->GetChunkIndex()
                            << ", " << objkey->GetChunkVersion()
                            << "] for AST is in " << status << " status");
                    ret = YFS_ENOTDONE;
                }

                if(YWB_SUCCESS != ret)
                {
                    /*
                     * use target disk id instead source disk id to keep it
                     * consistent with CompleteMigration
                     * */
                    objkey->SetStorageId(tgtdisk->GetDiskId());
                    mAST->GetMigration(&migrationmgr);
                    migrationmgr->GetCurrentMigrator(&migrator);
                    if(migrator != NULL)
                    {
                        /*
                         * tell AST that migration is discarded
                         * FIXME: what is the better errno?
                         * */
                        migrator->LaunchCompleteMigrationEvent(*objkey, ret);
                    }
                }

                break;
            }
            case Event::ET_hub_migration_completed:
            {
                /*
                 * FIXME: should move migration completion notification into
                 * ASTCommDispatcher in the long run???
                 **/
                HUBEventSet::MigrationCompletedEvent* completionevent = NULL;
                OBJKey* objkey = NULL;

                status = GetASTStatus();
                completionevent = (HUBEventSet::MigrationCompletedEvent*)event;
                completionevent->GetOBJKey(&objkey);
                CompleteMigrate(*objkey, completionevent->GetMigrationStatus());

                break;
            }
#ifdef WITH_INITIAL_PLACEMENT
            case Event::ET_hub_update_relocation_type:
            {
                HUBEventSet::UpdateSubPoolRelocationTypeEvent* relocationtypeevent = NULL;
                SubPoolKey* subpoolkey = NULL;

                relocationtypeevent = (HUBEventSet::UpdateSubPoolRelocationTypeEvent*)event;
                relocationtypeevent->GetSubPoolKey(&subpoolkey);
                UpdateSubPoolRelocationType(*subpoolkey, relocationtypeevent->GetRelocationType());

                break;
            }
            case Event::ET_hub_require_swap_disk:
            {
                HUBEventSet::RequireSwapDiskEvent* requireswapevent = NULL;
                DiskKey* diskkey = NULL;

                requireswapevent = (HUBEventSet::RequireSwapDiskEvent*)event;
                requireswapevent->GetDiskKey(&diskkey);
                RequireSwap(*diskkey, requireswapevent->GetSwapLevel(),
                        requireswapevent->GetSwapDirection());

                break;
            }
            case Event::ET_hub_cancel_swap_disk:
            {
                HUBEventSet::CancelSwapDiskEvent* cancelswapevent = NULL;
                DiskKey* diskkey = NULL;

                cancelswapevent = (HUBEventSet::CancelSwapDiskEvent*)event;
                cancelswapevent->GetDiskKey(&diskkey);
                CancelSwap(*diskkey);

                break;
            }
#endif
            case Event::ET_hub_stop:
            {
                map<SubPoolKey, ywb_uint32_t, SubPoolKeyCmp>::iterator iter;
                SubPoolKey subpoolkey;

                status = GetASTStatus();
                if(HUB::AS_enabled == status ||
                        HUB::AS_enabling == status)
                {
                    /*
                     * finalization work against HUB should be executed
                     * before notifying caller thread, if any
                     **/
                    iter = mSubPoolASTStatus.begin();
                    while(mSubPoolASTStatus.end() != iter)
                    {
                        if(HUB::AS_enabled == iter->second ||
                                HUB::AS_enabling == iter->second)
                        {
                            subpoolkey = iter->first;
                            LaunchDisableASTEvent(ywb_true, subpoolkey);
                        }

                        iter++;
                    }
                }
                else
                {
                    mSelfStatusLock.Lock();
                    mSelfStatusCond.Signal();
                    mSelfStatusLock.Unlock();
                }

                break;
            }
            default:
                YWB_ASSERT(0);
                break;
            }
        }

        event->DecRef();
    }
}

void HUB::HandlePendingEvent()
{
    vector<Event*> eventvec;
    vector<Event*> eventvec2;
    vector<Event*>::iterator iter;

    /*
     * for simplicity, put all events in pending queue into
     * non-pending event queue
     **/
    if(!mPendingEventContainer.empty())
    {
        ast_log_debug("Handle pending event in pending queue");
        mPendingEventContainer.DequeueNoWait(eventvec);
        eventvec2.assign(eventvec.rbegin(), eventvec.rend());
        /*enqueue into front of non-pending queue*/
        for(iter = eventvec2.begin(); iter != eventvec2.end(); iter++)
        {
            ast_log_debug("EnqueueFront event@" << *iter << " into HUB's"
                    << " non-pending queue, type:" << (*iter)->GetType());
            mEventContainer.Enqueue_front(*iter);
        }

        eventvec.clear();
        eventvec2.clear();
    }
}

void* HUB::entry()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t exit = false;

    mSelfStatusLock.Lock();
    mHUBStatus = HS_starting;

    /*
     * any HUB specific initialization work, currently no
     * HUB specific initialization work!
     **/

    if(YWB_SUCCESS != ret)
    {
        mHUBStatus = HS_failed;
        exit = true;
    }
    else
    {
        mHUBStatus = HS_running;
    }

    mSelfStatusCond.Signal();
    ast_log_debug("Notify HUB start status");
    mSelfStatusLock.Unlock();

    if(false == exit)
    {
        Handle();
    }
    else
    {
        /*any cleanup work necessary before exiting*/
        /*pthread_exit(NULL);*/
    }

    return NULL;
}

ywb_status_t HUB::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    mSelfStatusLock.Lock();
    if (HS_stopped == mHUBStatus) {
        this->create();

        ast_log_debug("Waiting for HUB's start status");
        mSelfStatusCond.Wait(mSelfStatusLock);

        if(HS_failed == mHUBStatus)
        {
            mSelfStatusLock.Unlock();
            ast_log_debug("fail to start HUB");
            /*join();*/
            ret = YFS_EAGAIN;
        }
        else
        {
            YWB_ASSERT(HS_running == mHUBStatus);
            mSelfStatusLock.Unlock();
            ast_log_debug("HUB is successfully started");
            ret = YWB_SUCCESS;

            /*start ASTCommDispatcher*/
            CreateASTCommDispatcher();
            ret = StartMessageService();
        }

        if(YWB_SUCCESS != ret)
        {
            Stop(ywb_true);
        }
    }
    else
    {
        mSelfStatusLock.Unlock();
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t HUB::Stop(ywb_bool_t immediate)
{
    ywb_status_t ret = YWB_SUCCESS;

    mSelfStatusLock.Lock();
    if(HS_starting == mHUBStatus ||
            HS_running == mHUBStatus ||
            HS_serving_for_ast == mHUBStatus ||
            HS_collecting_IO == mHUBStatus)
    {
        if(HS_running == mHUBStatus ||
                HS_serving_for_ast == mHUBStatus ||
                HS_collecting_IO == mHUBStatus)
        {
            mHUBStatus = HS_stopping;
            LaunchStopHUBEvent(immediate);
            mSelfStatusCond.Wait(mSelfStatusLock);

            /*
             * Destroy ASTCommDispatcher after stop HUB
             * for sake of processing as much migration
             * event as possible
             * */
            DestroyASTCommDispatcher();
            StopMessageService();
        }

        join();
        mHUBStatus = HS_stopped;
    }
    else if(HS_stopped != mHUBStatus)
    {
        ret = YFS_EINVAL;
    }

    mSelfStatusLock.Unlock();

    return ret;
}

#ifdef WITH_AST_MON_INTEGRATED
void
HUB::HandleOSSASTSpecialConfig(FsCommand::AST::OSSASTSpecialConfig* config)
{
    ywb_status_t ret = YWB_SUCCESS;
    ::FsCommand::AST::TimeScheduleConfig schedulemsg;
    ::FsCommand::AST::PoolTimeScheduleConfig poolschedulemsg;
    FsMonitor::TimeScheduleConfig schedule;
    ywb_int32_t index = 0;
    ywb_int32_t index2 = 0;
    TimeRange timerange;
    vector<TimeRange> timeranges;
    SubPoolKey subpoolkey;
    ywb_uint32_t aststatus = S_max;

    YWB_ASSERT(config != NULL);

    if(config->has_cycle_period())
    {
        mASTOption.SetCyclePeriod(config->cycle_period());
    }

    if(config->has_short_term_window_cycles())
    {
        mASTOption.SetShortTermWindowCycles(config->short_term_window_cycles());
    }

    if(config->has_long_term_window_cycles())
    {
        mASTOption.SetLongTermWindowCycles(config->long_term_window_cycles());
    }

    if(config->has_disk_perf_profile())
    {
        mASTOption.SetDiskPerfProfile(config->disk_perf_profile());
    }

    /*deal with OSS wise global time schedule config*/
    if(config->has_global_time_config())
    {
        schedulemsg = config->global_time_config();
        schedule.SetWeekDays(schedulemsg.week_days());
        for(index2 = 0; index2 < schedulemsg.time_slices_size(); index2++)
        {
            timerange.SetStart(schedulemsg.time_slices(index2).start());
            timerange.SetEnd(schedulemsg.time_slices(index2).end());
            timeranges.push_back(timerange);
        }

        schedule.AddTimeRanges(timeranges);
        mASTOption.SetTimeScheduleConfig(schedule);
        timeranges.clear();
    }

    /*deal with individual pool's specific time schedule config*/
    if(config->time_config_size() > 0)
    {
        for(index = 0; index < config->time_config_size(); index++)
        {
            poolschedulemsg = config->time_config(index);
            subpoolkey.SetOss(mSelfServiceId);
            subpoolkey.SetPoolId(poolschedulemsg.pool_id());
            subpoolkey.SetSubPoolId(poolschedulemsg.pool_id());
            schedule.SetWeekDays(poolschedulemsg.config().week_days());
            for(index2 = 0; index2 < poolschedulemsg.config().time_slices_size(); index2++)
            {
                timerange.SetStart(poolschedulemsg.config().time_slices(index2).start());
                timerange.SetEnd(poolschedulemsg.config().time_slices(index2).end());
                timeranges.push_back(timerange);
            }

            schedule.AddTimeRanges(timeranges);
            SetSubPoolTimeScheduleConfig(subpoolkey, schedule);
            timeranges.clear();
        }
    }

    if(config->sync_ast_status_size() > 0)
    {
        for(index = 0; index < config->sync_ast_status_size(); index++)
        {
            subpoolkey.SetOss(mSelfServiceId);
            subpoolkey.SetPoolId(config->sync_ast_status(index).pool_id());
            subpoolkey.SetSubPoolId(config->sync_ast_status(index).pool_id());
            if(S_disabled == config->sync_ast_status(index).ast_status())
            {
                /*make sure it is really disabled*/
                ret = GetSubPoolASTStatus(subpoolkey, aststatus);
                if((YWB_SUCCESS == ret) && (S_disabled != aststatus))
                {
                    LaunchDisableASTEvent(subpoolkey);
                }
            }
            else if(S_enabling == config->sync_ast_status(index).ast_status())
            {
                LaunchEnableASTEvent(subpoolkey);
            }
            else if(S_enabled == config->sync_ast_status(index).ast_status())
            {
                /*make sure it is really enabled*/
                ret = GetSubPoolASTStatus(subpoolkey, aststatus);
                if((YWB_SUCCESS == ret) && (S_enabled != aststatus))
                {
                    LaunchEnableASTEvent(subpoolkey);
                }
            }
            else if(S_disabling == config->sync_ast_status(index).ast_status())
            {
                LaunchDisableASTEvent(subpoolkey);
            }
        }
    }
}

void
HUB::GenerateOSSASTSpecialConfigResponse(
        FsCommand::AST::OSSASTSpecialConfigResponse* response)
{
    map<SubPoolKey, ywb_uint32_t, SubPoolKeyCmp>::iterator iter;
    ::FsCommand::AST::PoolASTStatus* poolaststatus = NULL;

    YWB_ASSERT(response != NULL);

    response->mutable_status()->set_error_code(YWB_SUCCESS);
    response->mutable_status()->set_error_string("success");
    iter = mSubPoolASTStatus.begin();
    for(; iter != mSubPoolASTStatus.end(); iter++)
    {
        poolaststatus = response->add_real_ast_status();
        poolaststatus->set_pool_id(iter->first.GetPoolId());
        poolaststatus->set_ast_status(FsCommand::AST::ASTStatus(iter->second));
    }
}
#endif

#ifdef WITH_INITIAL_PLACEMENT
ywb_status_t HUB::UpdateSubPoolRelocationType(
        SubPoolKey& subpoolkey, ywb_int32_t val)
{
    map<SubPoolKey, ywb_int32_t, SubPoolKeyCmp>::iterator iter;
    iter = mSubPoolRelocationTypes.find(subpoolkey);
    if(mSubPoolRelocationTypes.end() != iter)
    {
        iter->second = val;
    }
    else
    {
        mSubPoolRelocationTypes.insert(std::make_pair(subpoolkey, val));
    }

    return YWB_SUCCESS;
}

void HUB::ClearSubPoolRelocationType()
{
    mSubPoolRelocationTypes.clear();
}

ywb_status_t HUB::RequireSwap(DiskKey& diskkey,
        ywb_int32_t level, ywb_int32_t direction)
{
    map<DiskKey, ywb_int32_t, DiskKeyCmp>::iterator iter;
    ywb_int32_t value = 0;

    value = SwapOutManager::GetCombinedLevelAndDirection(level, direction);
    iter = mSwapRequiredDisks.find(diskkey);
    if(mSwapRequiredDisks.end() != iter)
    {
        iter->second = value;
    }
    else
    {
        mSwapRequiredDisks.insert(std::make_pair(diskkey, value));
    }

    return YWB_SUCCESS;
}

void HUB::ClearSwapRequiredDisks()
{
    mSwapRequiredDisks.clear();
}

ywb_status_t HUB::CancelSwap(DiskKey& diskkey)
{
    list<DiskKey>::iterator iter;

    /*firstly check whether or not it is already existed*/
    iter = mSwapCancelledDisks.begin();
    while(mSwapCancelledDisks.end() != iter)
    {
        if(diskkey == *iter)
        {
            break;
        }

        iter++;
    }

    if(mSwapCancelledDisks.end() == iter)
    {
        mSwapCancelledDisks.push_back(diskkey);
    }

    return YWB_SUCCESS;
}

void HUB::ClearSwapCancelledDisks()
{
    mSwapCancelledDisks.clear();
}
#endif

void HUB::LaunchStopHUBEvent(ywb_bool_t immediate)
{
    HUBEventSet::StopHUBEvent* event = NULL;

    event = new HUBEventSet::StopHUBEvent();
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch stop event@" << event << " against HUB");
    if(immediate)
    {
        mEventContainer.Enqueue_front(event);
    }
    else
    {
        mEventContainer.Enqueue(event);
    }
}

void HUB::LaunchAddDiskEvent(DiskKey& diskkey, DiskBaseProp& diskprop)
{
    HUBEventSet::AddDiskEvent* event = NULL;

    event = new HUBEventSet::AddDiskEvent(diskkey, diskprop);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch add disk event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchRemoveDiskEvent(DiskKey& diskkey)
{
    HUBEventSet::RemoveDiskEvent* event = NULL;

    event = new HUBEventSet::RemoveDiskEvent(diskkey);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch remove disk event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchEnableASTEvent()
{
    HUBEventSet::EnableASTEvent* event = NULL;

    event = new HUBEventSet::EnableASTEvent();
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch enable AST event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchEnableASTEvent(SubPoolKey& subpoolkey)
{
    HUBEventSet::EnableASTEvent* event = NULL;

    event = new HUBEventSet::EnableASTEvent(subpoolkey);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch enable AST event@" << event << " against HUB"
            << " on SubPool: [oss: " << subpoolkey.GetOss()
            << ", pool: " << subpoolkey.GetPoolId()
            << ", subpool: " << subpoolkey.GetSubPoolId()
            << "]");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchDisableASTEvent(ywb_bool_t immediate)
{
    HUBEventSet::DisableASTEvent* event = NULL;

    event = new HUBEventSet::DisableASTEvent();
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch disable AST event@" << event << " against HUB");
    if(immediate)
    {
        mEventContainer.Enqueue_front(event);
    }
    else
    {
        mEventContainer.Enqueue(event);
    }
}

void HUB::LaunchDisableASTEvent(ywb_bool_t immediate, SubPoolKey& subpoolkey)
{
    HUBEventSet::DisableASTEvent* event = NULL;

    event = new HUBEventSet::DisableASTEvent(subpoolkey);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch disable AST event@" << event << " against HUB"
            << " on SubPool: [oss: " << subpoolkey.GetOss()
            << ", pool: " << subpoolkey.GetPoolId()
            << ", subpool: " << subpoolkey.GetSubPoolId()
            << "]");
    if(immediate)
    {
        mEventContainer.Enqueue_front(event);
    }
    else
    {
        mEventContainer.Enqueue(event);
    }
}

void HUB::LaunchEnableASTEchoEvent(ywb_status_t status)
{
    HUBEventSet::EnableASTEchoEvent* event = NULL;

    event = new HUBEventSet::EnableASTEchoEvent();
    YWB_ASSERT(event != NULL);
    event->SetStatus(status);
    event->IncRef();
    ast_log_debug("Launch EnableAST echo event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchDisableASTEchoEvent(ywb_status_t status)
{
    HUBEventSet::DisableASTEchoEvent* event = NULL;

    event = new HUBEventSet::DisableASTEchoEvent();
    YWB_ASSERT(event != NULL);
    event->SetStatus(status);
    event->IncRef();
    ast_log_debug("Launch DisableAST echo event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchAddOBJEvent(OBJKey& objkey)
{
    HUBEventSet::AddOBJEvent* event = NULL;

    event = new HUBEventSet::AddOBJEvent(objkey);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch add OBJ event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchDeleteOBJEvent(OBJKey& objkey)
{
    HUBEventSet::DeleteOBJEvent* event = NULL;

    event = new HUBEventSet::DeleteOBJEvent(objkey);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch delete OBJ event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchAccessOBJEvent(OBJKey& objkey, off_t offset,
        size_t len, ywb_uint32_t ios, ywb_uint32_t bw,
        ywb_uint32_t rt, ywb_bool_t read)
{
    HUBEventSet::AccessOBJEvent* event = NULL;

    event = new HUBEventSet::AccessOBJEvent(
            objkey, offset, len, ios, bw, rt, read);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch access OBJ event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchCollectConfigViewEvent(ywb_uint32_t seqno)
{
    HUBEventSet::CollectConfigViewEvent* event = NULL;

    event = new HUBEventSet::CollectConfigViewEvent(seqno);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch collect config view event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchCollectIOEvent(ywb_uint32_t seqno, ywb_uint32_t flag)
{
    HUBEventSet::CollectIOEvent* event = NULL;

    event = new HUBEventSet::CollectIOEvent(seqno, flag);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch collect IO event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchMigrateEvent(OBJKey& objkey, DiskKey& tgtdisk)
{
    HUBEventSet::MigrateEvent* event = NULL;

    event = new HUBEventSet::MigrateEvent(objkey, tgtdisk);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch migrate event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchMigrationCompletedEvent(
        OBJKey& objkey, ywb_status_t migrationstatus)
{
    HUBEventSet::MigrationCompletedEvent* event = NULL;

    event = new HUBEventSet::MigrationCompletedEvent(objkey, migrationstatus);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch migration completed event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

#ifdef WITH_INITIAL_PLACEMENT
void HUB::LaunchUpdateRelocationTypeEvent(
        SubPoolKey& subpoolkey, ywb_int32_t val)
{
    HUBEventSet::UpdateSubPoolRelocationTypeEvent* event = NULL;

    event = new HUBEventSet::UpdateSubPoolRelocationTypeEvent(subpoolkey, val);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch update subpool's relocation type event@"
            << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchSwapRequiredDiskEvent(DiskKey& diskkey,
        ywb_int32_t level, ywb_int32_t direction)
{
    HUBEventSet::RequireSwapDiskEvent* event = NULL;

    event = new HUBEventSet::RequireSwapDiskEvent(diskkey, level, direction);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch require swap disk event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}

void HUB::LaunchSwapCancelledDiskEvent(DiskKey& diskkey)
{
    HUBEventSet::CancelSwapDiskEvent* event = NULL;

    event = new HUBEventSet::CancelSwapDiskEvent(diskkey);
    YWB_ASSERT(event != NULL);
    event->IncRef();
    ast_log_debug("Launch cancel swap disk event@" << event << " against HUB");
    mEventContainer.Enqueue(event);
}
#endif

ywb_status_t HUB::StartMessageService()
{
    YWB_ASSERT(mMsgMgr != NULL);
    return mMsgMgr->StartMsgSvc();
}

void HUB::StopMessageService()
{
    if(mMsgMgr != NULL)
    {
        mMsgMgr->StopMsgSvc();
    }
}

void HUB::CreateASTCommDispatcher()
{
    ast_log_debug("create ASTCommDispatcher");
    YWB_ASSERT(mASTCommDispatcher == NULL);
    mASTCommDispatcher = new ASTCommDispatcher(
            mControlCenter->GetCommunicationLayout(), "ASTCommDispatcher");
    YWB_ASSERT(mASTCommDispatcher != NULL);
    mASTCommDispatcher->Start(CommDispatcher::NDT_ethernet);
}

void HUB::DestroyASTCommDispatcher()
{
    ast_log_debug("destroy ASTCommDispatcher");
    if (mASTCommDispatcher != NULL) {
        mASTCommDispatcher->Quit();
        delete mASTCommDispatcher;
        mASTCommDispatcher = NULL;
    }
}

void HUB::WaitUntilEnableASTEchoed()
{
    mASTStatusLock.Lock();
    ast_log_debug("HUB wait for AST enabled");
    mASTStatusCond.Wait(mASTStatusLock);
    mASTStatusLock.Unlock();
}

void HUB::WaitUntilDisableASTEchoed()
{
    mASTStatusLock.Lock();
    ast_log_debug("HUB wait for AST disabled");
    mASTStatusCond.Wait(mASTStatusLock);
    mASTStatusLock.Unlock();
}

void HUB::NotifyEnableASTResult(ywb_status_t ret)
{
    if(ywb_false == mEnableOrDisableASTSync)
    {
        LaunchEnableASTEchoEvent(ret);
    }
    else
    {
        mASTStatusLock.Lock();
        mEnableOrDisableASTResult = ret;
        ast_log_debug("Notify about EnablingAST result: " << ret);
        mASTStatusCond.Signal();
        mASTStatusLock.Unlock();
    }
}

void HUB::NotifyDisableASTResult(ywb_status_t ret)
{
    if(ywb_false == mEnableOrDisableASTSync)
    {
        LaunchDisableASTEchoEvent(ret);
    }
    else
    {
        mASTStatusLock.Lock();
        mEnableOrDisableASTResult = ret;
        ast_log_debug("Notify about DisablingAST result: " << ret);
        mASTStatusCond.Signal();
        mASTStatusLock.Unlock();
    }
}

void HUB::ReportChunk(const ChunkStorageStruct *const css)
{
    OBJKey objkey(css->mChunkId, css->mInodeId,
            css->mChunkIndex, css->mChunkVersion,
            css->mStorageId);

    LaunchAddOBJEvent(objkey);
}

ywb_bool_t
HUB::ShouldEventPend(Event* event)
{
    ywb_bool_t pend = ywb_false;
    ywb_uint32_t eventtype = Event::ET_end;

    if(HUB::HS_collecting_IO != mHUBStatus)
    {
        pend = ywb_false;
    }
    else
    {
        /*
         * the logical configuration should be frozen when collecting OBJ IO
         * (1) add/remove disk/OBJ will change the configuration;
         * (2) collect logical configuration will be delayed until completion
         * of current round of collecting OBJ IO, and new round of collecting
         * OBJ IO event will not be launched until completion of new round of
         * collecting logical configuration;
         * (3) migration completion will move the migrated OBJ from source
         * disk to target disk;
         * */
        eventtype = event->GetType();
        if(Event::ET_hub_add_disk == eventtype ||
                Event::ET_hub_remove_disk == eventtype ||
                Event::ET_hub_add_obj == eventtype ||
                Event::ET_hub_delete_obj == eventtype ||
                Event::ET_hub_collect_config_view == eventtype ||
                Event::ET_hub_migration_completed == eventtype)
        {
            pend = ywb_true;
        }
    }

    return pend;
}

ywb_bool_t
HUB::ShouldDisableAST(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t num = 0;
    ywb_uint32_t subpoolaststatus = 0;
    map<SubPoolKey, ywb_uint32_t, SubPoolKeyCmp>::iterator iter;

    /*get the total number of SubPools with enabling/enabled ASTStatus*/
    iter = mSubPoolASTStatus.begin();
    while(mSubPoolASTStatus.end() != iter)
    {
        if((HUB::AS_enabling == iter->second) ||
                (HUB::AS_enabled == iter->second))
        {
            num++;
        }

        iter++;
    }

    if(1 == num)
    {
        ret = GetSubPoolASTStatus(subpoolkey, subpoolaststatus);
        if(YWB_SUCCESS != ret)
        {
            return false;
        }
        else
        {
            return ((HUB::AS_enabling == subpoolaststatus) ||
                    (HUB::AS_enabled == subpoolaststatus));
        }
    }

    return false;
}

ywb_bool_t HUB::ShouldSubPoolBeInvolved(SubPoolKey& subpoolkey)
{
    ywb_bool_t inbatch = false;
    ywb_bool_t involved = false;
    ywb_uint32_t aststatus = HUB::AS_disabled;
    ywb_uint32_t subpoolaststatus = HUB::AS_disabled;
    SubPoolView* subpoolview = NULL;

    inbatch = GetEnableInBatch();
    aststatus = GetASTStatus();
    if(!inbatch)
    {
        /*
         * here will take both SubPool ASTStatus and SubPoolView into
         * consideration, for HUB may have zombie SubPool which is
         * removed from HUB's ConfigView forever!(Please Refer to
         * comment in HUB::RemoveDisk)
         * */
        involved = ((YWB_SUCCESS == GetSubPoolASTStatus(
                subpoolkey, subpoolaststatus)) &&
                (HUB::AS_enabled == subpoolaststatus) &&
                (YWB_SUCCESS == mConfigView->
                        GetSubPoolView(subpoolkey, &subpoolview)));
    }
    else if(HUB::AS_enabled == aststatus)
    {
        /*
         * here check existence of SubPoolView for SubPool's
         * ASTStatus may not be set if in batch mode
         * */
        involved = (YWB_SUCCESS == mConfigView->
                GetSubPoolView(subpoolkey, &subpoolview));
    }

    return (involved && CheckSubPoolIsScheduledNow(subpoolkey));
}

ywb_bool_t HUB::ShouldDiskBeInvolved(
        DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;

    ret = mConfigView->GetSubPoolKeyFromID(diskkey.GetSubPoolId(), &subpoolkey);
    return ((YWB_SUCCESS == ret) && (ShouldSubPoolBeInvolved(subpoolkey)));
}

ywb_bool_t HUB::ShouldRedirect(OBJKey& srcobj, OBJKey& tgtobj)
{
    DiskKey tgtdisk;

    if(YWB_SUCCESS != GetSrcUnsettledOBJ(srcobj, tgtdisk))
    {
        return ywb_false;
    }
    else
    {
        tgtobj.SetStorageId(tgtdisk.GetDiskId());
        tgtobj.SetInodeId(srcobj.GetInodeId());
        tgtobj.SetChunkId(srcobj.GetChunkId());
        tgtobj.SetChunkIndex(srcobj.GetChunkIndex());
        tgtobj.SetChunkVersion(srcobj.GetChunkVersion());

        return ywb_true;
    }
}

ywb_bool_t HUB::MarkOBJAsSettledIfNeccessary(OBJKey& tgtobj)
{
    DiskKey srcdisk;
    OBJKey srcobj;

    if(YWB_SUCCESS != GetTgtUnsettledOBJ(tgtobj, srcdisk))
    {
        return ywb_false;
    }
    else
    {
        srcobj.SetStorageId(srcdisk.GetDiskId());
        srcobj.SetInodeId(tgtobj.GetInodeId());
        srcobj.SetChunkId(tgtobj.GetChunkId());
        srcobj.SetChunkIndex(tgtobj.GetChunkIndex());
        srcobj.SetChunkVersion(tgtobj.GetChunkVersion());

        RemoveSrcUnsettleOBJ(srcobj);
        RemoveTgtUnsettleOBJ(tgtobj);

        return ywb_true;
    }
}

ywb_status_t
HUB::GetSubPoolASTStatus(SubPoolKey& subpoolkey,
        ywb_uint32_t& status)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t aststatus = HUB::AS_disabled;
    map<SubPoolKey, ywb_uint32_t, SubPoolKeyCmp>::iterator iter;

    /*not in batch mode, each SubPool will have its own ASTStatus*/
    if(false == GetEnableInBatch())
    {
        iter = mSubPoolASTStatus.find(subpoolkey);
        if(mSubPoolASTStatus.end() != iter)
        {
            status = iter->second;
        }
        else
        {
            ret = YFS_ENOENT;
        }
    }
    else
    {
        /*in batch mode, all SubPools will have the same ASTStatus as AST Module*/
        aststatus = GetASTStatus();
        status = aststatus;
    }

    return ret;
}

ywb_status_t
HUB::SetSubPoolASTStatus(SubPoolKey& subpoolkey,
        ywb_uint32_t status)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, ywb_uint32_t, SubPoolKeyCmp>::iterator iter;

    iter = mSubPoolASTStatus.find(subpoolkey);
    if(mSubPoolASTStatus.end() != iter)
    {
        iter->second = status;
    }
    else
    {
        mSubPoolASTStatus.insert(std::make_pair(subpoolkey, status));
    }

    return ret;
}

ywb_status_t
HUB::RemoveSubPoolASTStatus(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, ywb_uint32_t, SubPoolKeyCmp>::iterator iter;

    iter = mSubPoolASTStatus.find(subpoolkey);
    if(mSubPoolASTStatus.end() != iter)
    {
        mSubPoolASTStatus.erase(iter);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HUB::SetSubPoolTimeScheduleConfig(SubPoolKey& subpoolkey,
        TimeScheduleConfig& schedule)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, TimeScheduleConfig, SubPoolKeyCmp>::iterator iter;

    iter = mSubPoolTimeSchedules.find(subpoolkey);
    if(mSubPoolTimeSchedules.end() != iter)
    {
        iter->second = schedule;
    }
    else
    {
        mSubPoolTimeSchedules.insert(make_pair(subpoolkey, schedule));
    }

    return ret;
}

ywb_status_t
HUB::RemoveSubPoolTimeScheduleConfig(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, TimeScheduleConfig, SubPoolKeyCmp>::iterator iter;

    iter = mSubPoolTimeSchedules.find(subpoolkey);
    if(mSubPoolTimeSchedules.end() != iter)
    {
        mSubPoolTimeSchedules.erase(iter);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_bool_t
HUB::CheckSubPoolIsScheduledNow(SubPoolKey& subpoolkey)
{
//    ywb_status_t ret = YWB_SUCCESS;
//    ywb_uint32_t subpoolaststatus = 0;
    map<SubPoolKey, TimeScheduleConfig, SubPoolKeyCmp>::iterator iter;

    iter = mSubPoolTimeSchedules.find(subpoolkey);
    if(mSubPoolTimeSchedules.end() != iter)
    {
        /*given SubPool has its own TimeScheduleConfig*/
        return TimeScheduleConfig::TimeIsScheduled(&(iter->second));
    }
#ifdef WITH_AST_MON_INTEGRATED
    else
    {
        /*given SubPool will follow OSS wise TimeScheduleConfig*/
        ret = GetSubPoolASTStatus(subpoolkey, subpoolaststatus);
        if(YWB_SUCCESS == ret)
        {
            /*only if the SubPool is under AST's management*/
            return TimeScheduleConfig::TimeIsScheduled(
                    &(mASTOption.GetTimeScheduleConfig()))
        }
    }
#endif

    return ywb_false;
}

ywb_status_t HUB::GetSubPoolView(SubPoolKey& subpoolkey, SubPoolView** subpoolview)
{
    return mConfigView->GetSubPoolView(subpoolkey, subpoolview);
}

ywb_status_t HUB::GetSubPoolView(DiskKey& diskkey, SubPoolView** subpoolview)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;

    ret = mConfigView->GetSubPoolKeyFromID(diskkey.GetSubPoolId(), &subpoolkey);
    if(YWB_SUCCESS == ret)
    {
        ret = mConfigView->GetSubPoolView(subpoolkey, subpoolview);
    }

    return ret;
}

ywb_status_t HUB::GetDiskView(DiskKey& diskkey, DiskView** diskview)
{
    return mConfigView->GetDiskView(diskkey, diskview);
}

ywb_status_t HUB::GetDiskView(OBJKey& objkey, DiskView** diskview)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;

    ret = mConfigView->GetDiskKeyFromID(objkey.GetStorageId(), &diskkey);
    if(YWB_SUCCESS == ret)
    {
        ret = mConfigView->GetDiskView(diskkey, diskview);
    }

    return ret;
}

ywb_status_t HUB::GetOBJView(OBJKey& objkey, OBJView** objinfo)
{
    return mConfigView->GetOBJView(objkey, objinfo);
}

ywb_status_t HUB::GetFirstSubPoolView(
        SubPoolView** subpoolview, SubPoolKey* subpoolkey)
{
    return mConfigView->GetFirstSubPoolView(subpoolview, subpoolkey);
}

ywb_status_t HUB::GetNextSubPoolView(
        SubPoolView** subpoolview, SubPoolKey* subpoolkey)
{
    return mConfigView->GetNextSubPoolView(subpoolview, subpoolkey);
}

ywb_status_t HUB::GetFirstDiskView(
        DiskView** diskview, DiskKey* diskkey)
{
    return mConfigView->GetFirstDiskView(diskview, diskkey);
}

ywb_status_t HUB::GetNextDiskView(DiskView** diskview, DiskKey* diskkey)
{
    return mConfigView->GetNextDiskView(diskview, diskkey);
}

ywb_status_t HUB::GetFirstOBJView(OBJView** objview, OBJKey* objkey)
{
    return mConfigView->GetFirstOBJView(objview, objkey);
}

ywb_status_t HUB::GetNextOBJView(OBJView** objview, OBJKey* objkey)
{
    return mConfigView->GetNextOBJView(objview, objkey);
}

ywb_uint32_t HUB::GetOBJsCnt()
{
    return mConfigView->GetOBJCnt();
}

ywb_status_t HUB::GetTierWiseDisks(SubPoolKey& subpoolkey,
            ywb_int32_t tiertype, TierWiseDiskCandidates* candidates)
{
    return mConfigView->GetTierWiseDisks(subpoolkey, tiertype, candidates);
}

ywb_uint32_t HUB::GetTableVersion(int type)
{
    return mASTCommDispatcher->
            GetTableVersion(CommunicationLayout::AT_unit);
}

void HUB::GetOBJListByDiskId(ywb_uint64_t diskid, std::list<OBJKey> &list)
{
    mControlCenter->GetObjectLayout()->GetChunkInfoByStorageId(diskid, list);
}

void HUB::Enqueue(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("Enqueue event@" << event << " into HUB's"
            << " non-pending queue, type:" << event->GetType());
    event->IncRef();
    mEventContainer.Enqueue(event);
}

void HUB::Enqueue(vector<Event*> eventvec)
{
    vector<Event*>::iterator iter;

    iter = eventvec.begin();
    ast_log_debug("Enqueue event vector into HUB's non-pending queue, events:");
    for(; iter != eventvec.end(); iter++)
    {
        (*iter)->IncRef();
        ast_log_debug("event@" << *iter << ", type: " << (*iter)->GetType());
    }

    mEventContainer.Enqueue(eventvec);
}

void HUB::EnqueueFront(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("EnqueueFront event@" << event << " into HUB's"
            << " non-pending queue, type:" << event->GetType());
    event->IncRef();
    mEventContainer.Enqueue_front(event);
}

void HUB::EnqueueFront(vector<Event*> eventvec)
{
    vector<Event*>::iterator iter;

    iter = eventvec.begin();
    ast_log_debug("EnqueueFront event vector into HUB's non-pending queue, events:");
    for(; iter != eventvec.end(); iter++)
    {
        (*iter)->IncRef();
        ast_log_debug("event@" << *iter << ", type: " << (*iter)->GetType());
    }

    mEventContainer.Enqueue_front(eventvec);
}

void HUB::EnqueuePending(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("Enqueue event@" << event << " into HUB's"
            << " pending queue, type:" << event->GetType());
    event->IncRef();
    mPendingEventContainer.Enqueue(event);
}

void HUB::EnqueuePending(vector<Event*> eventvec)
{
    vector<Event*>::iterator iter;

    iter = eventvec.begin();
    ast_log_debug("Enqueue event vector into HUB's pending queue, events:");
    for(; iter != eventvec.end(); iter++)
    {
        (*iter)->IncRef();
        ast_log_debug("event@" << *iter << ", type: " << (*iter)->GetType());
    }

    mPendingEventContainer.Enqueue(eventvec);
}

void HUB::EnqueuePendingFront(Event* event)
{
    YWB_ASSERT(event);
    ast_log_debug("EnqueueFront event@" << event << " into HUB's"
            << " pending queue, type:" << event->GetType());
    event->IncRef();
    mPendingEventContainer.Enqueue_front(event);
}

void HUB::EnqueuePendingFront(vector<Event*> eventvec)
{
    vector<Event*>::iterator iter;

    iter = eventvec.begin();
    ast_log_debug("EnqueueFront event vector into HUB's pending queue, events:");
    for(; iter != eventvec.end(); iter++)
    {
        (*iter)->IncRef();
        ast_log_debug("event@" << *iter << ", type: " << (*iter)->GetType());
    }

    mPendingEventContainer.Enqueue_front(eventvec);
}

ywb_status_t HUB::EnableAST()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("EnableAST %s" << ((ywb_true == mEnableOrDisableASTSync) ?
            "Synchronously" : "Asynchronously"));
    YWB_ASSERT(mMsgMgr != NULL);
    /*REMEMBER to reset message manager*/
    mMsgMgr->Reset();
    mAST->SetMessageManager(mMsgMgr);
    YWB_ASSERT(mControlCenter != NULL);
//    mAST->SetHUB(this);
    mAST->SetControlCenter(mControlCenter);

    SetASTStatus(HUB::AS_enabling);
    mMsgMgr->Enqueue(mEnableASTMsg);
    if(ywb_true == mEnableOrDisableASTSync)
    {
        /*wait until the AST successfully enabled*/
        WaitUntilEnableASTEchoed();
        ret = mEnableOrDisableASTResult;
        HandleEnableASTEcho(mEnableOrDisableASTResult);
        mEnableOrDisableASTResult = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t HUB::DisableAST()
{
    ywb_status_t ret = YWB_SUCCESS;

    ast_log_debug("DisableAST %s" << ((ywb_true == mEnableOrDisableASTSync) ?
            "Synchronously" : "Asynchronously"));
    SetASTStatus(HUB::AS_disabling);
    mMsgMgr->Enqueue(mDisableASTMsg);
    if(ywb_true == mEnableOrDisableASTSync)
    {
        /*wait until the AST successfully disabled*/
        WaitUntilDisableASTEchoed();
        ret = mEnableOrDisableASTResult;
        HandleDisableASTEcho(mEnableOrDisableASTResult);
        mEnableOrDisableASTResult = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t HUB::HandleEnableASTEcho(ywb_status_t status)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, ywb_uint32_t, SubPoolKeyCmp>::iterator iter;
    SubPoolKey subpoolkey;

    if(YWB_SUCCESS == status)
    {
        ast_log_debug("EnableAST returns successfully");
        SetASTStatus(HUB::AS_enabled);
        mHUBStatus = HUB::HS_serving_for_ast;
        if(false == GetEnableInBatch())
        {
            /*
             * mark all SubPools with enabling ASTStaus as enabled
             * if in non-batch mode, otherwise all SubPools' ASTStatus
             * is dependent on AST Module's ASTStatus
             * */
            iter = mSubPoolASTStatus.begin();
            while(mSubPoolASTStatus.end() != iter)
            {
                if(HUB::AS_enabling == iter->second)
                {
                    subpoolkey = iter->first;
                    SetSubPoolASTStatus(subpoolkey, HUB::AS_enabled);
                }

                iter++;
            }
        }
    }
    else
    {
        ast_log_debug("EnableAST returns with err: " << status);
    }

    return ret;
}

ywb_status_t HUB::HandleDisableASTEcho(ywb_status_t status)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, ywb_uint32_t, SubPoolKeyCmp>::iterator iter;
    SubPoolKey subpoolkey;
    set<SubPoolKey, SubPoolKeyCmp> enablereqs;
    set<SubPoolKey, SubPoolKeyCmp>::iterator iter2;

    if(YWB_SUCCESS == status)
    {
        ast_log_debug("DisableAST returns successfully");
        SetASTStatus(HUB::AS_disabled);
        if(false == GetDisableInBatch())
        {
            /*
             * mark all SubPools with disabling ASTStaus as disabled
             * if in non-batch mode, otherwise all SubPools' ASTStatus
             * is dependent on AST Module's ASTStatus
             * */
            iter = mSubPoolASTStatus.begin();
            while(mSubPoolASTStatus.end() != iter)
            {
                if(HUB::AS_disabling == iter->second)
                {
                    subpoolkey = iter->first;
                    SetSubPoolASTStatus(subpoolkey, HUB::AS_disabled);
                }
                else if(HUB::AS_enabling == iter->second)
                {
                    /*
                     * means EnableAST message comes after DisableAST message
                     * */
                    enablereqs.insert(iter->first);
                }

                iter++;
            }

            SetEnableInBatch(false);
        }
        else
        {
            SetEnableInBatch(true);
        }

        /*execute those EnableAST request comes after DisableAST*/
        if(!(enablereqs.empty()))
        {
            iter2 = enablereqs.begin();
            while(enablereqs.end() != iter2)
            {
                subpoolkey = *iter2;
                LaunchEnableASTEvent(subpoolkey);
            }
        }

        if(HUB::HS_serving_for_ast == mHUBStatus ||
                HUB::HS_collecting_IO == mHUBStatus)
        {
            mHUBStatus = HUB::HS_running;
        }
        else if(HUB::HS_stopping == mHUBStatus)
        {
            mSelfStatusLock.Lock();
            mSelfStatusCond.Signal();
            mSelfStatusLock.Unlock();
        }
    }
    else
    {
        ast_log_debug("DisableAST returns with err: " << status);
    }

    return ret;
}

ywb_status_t HUB::AddDisk(DiskKey& diskkey, DiskBaseProp& diskprop)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;

    mConfigLock.wrlock();
    ret = mConfigView->AddDiskView(diskkey, diskprop);
    mConfigLock.unlock();

#ifdef WITH_INITIAL_PLACEMENT
    YWB_ASSERT(mPolicyMgr != NULL);
    ret = mConfigView->GetSubPoolKeyFromID(diskkey.GetSubPoolId(), &subpoolkey);
    if(YWB_SUCCESS == ret)
    {
        mPolicyMgr->Adjust(subpoolkey);
        mPolicyMgr->UpdateDiskSelector(subpoolkey, diskprop.DetermineDiskTier(),
                DiskSelector::DSE_add_disk);
    }
#endif

    return ret;
}

ywb_status_t HUB::RemoveDisk(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    ywb_uint32_t aststatus = HUB::AS_disabled;
    SubPoolView* subpoolview = NULL;
    ReportDeleteSubPoolMessage* subpooldelmsg = NULL;
    ReportDeleteDiskMessage* diskdelmsg = NULL;

    aststatus = GetASTStatus();
    mConfigLock.wrlock();
    ret = mConfigView->GetSubPoolKeyFromID(diskkey.GetSubPoolId(), &subpoolkey);
    YWB_ASSERT(YWB_SUCCESS == ret);
    /*
     * NOTE:
     * RemoveDiskView may already removed SubPool if @diskkey is the last disk
     * and meanwhile corresponding DiskView has not been pinned
     * */
    ret = mConfigView->RemoveDiskView(diskkey);
    if((YWB_SUCCESS == ret) && ((HUB::AS_disabling != aststatus) &&
            (HUB::AS_disabled != aststatus)))
    {
        ret = GetSubPoolView(subpoolkey, &subpoolview);
        if(YWB_SUCCESS != ret)
        {
            /*
             * now that RemoveDiskView returns successfully, we are sure the
             * SubPool do exist before calling RemoveDiskView, but now
             * GetSubPoolView fails, it must be removed for no more disk on it
             *
             * tell the AST about its deletion anyway!
             * */
            /*if(ShouldSubPoolBeInvolved(subpoolkey))*/
            {
                /*subpool deletion will lead to disk and OBJ deletion in AST side*/
                subpooldelmsg = new ReportDeleteSubPoolMessage(subpoolkey);
                YWB_ASSERT(subpooldelmsg != NULL);
                SendRemovedSubPool(subpooldelmsg);
            }

            /*
             * NOTE:
             * should not remove SubPool's ASTStatus and TimeScheduleConfig
             * from HUB, for:
             * (1) if the SubPool is "logically" removed for deletion of
             * all its disks, then it may comes back again when any new
             * disks addition, in this situation, it is HUB's duty to
             * maintain SubPool's ASTStatus and TimeScheduleConfig as
             * before the deletion.
             *
             * (2) if the SubPool is "physically" deleted, then it will
             * never comes back again, then retaining of infomation about
             * this SubPool will have no side effect except memory usage.
             * */
            /*RemoveSubPoolASTStatus(subpoolkey);*/
            /*RemoveSubPoolTimeScheduleConfig(subpoolkey);*/
        }
        else
        {
            if(subpoolview->GetDelayDeletedDisksNum() == subpoolview->GetDiskCnt())
            {
                /*subpool deletion will lead to disk and OBJ deletion in AST side*/
                subpooldelmsg = new ReportDeleteSubPoolMessage(subpoolkey);
                YWB_ASSERT(subpooldelmsg != NULL);
                SendRemovedSubPool(subpooldelmsg);
            }
            else/*if(ShouldDiskBeInvolved(diskkey))*/
            {
                /*
                 * this may cause a duplicated message if the DiskView is
                 * marked as delay-deleted and deleted by HUB::RemoveDisk,
                 * but currently we will delete the delay-deleted DiskView
                 * by ConfigView::RemoveDiskView which will avoid duplication
                 *
                 * tell the AST about its deletion anyway!
                 * */
                diskdelmsg = new ReportDeleteDiskMessage(diskkey);
                YWB_ASSERT(diskdelmsg != NULL);
                SendRemovedDisk(diskdelmsg);
            }
        }
    }
    else if(YFS_EBUSY == ret)
    {
        if((YWB_SUCCESS == GetSubPoolView(subpoolkey, &subpoolview)) &&
                (subpoolview->GetDelayDeletedDisksNum() ==
                        subpoolview->GetDiskCnt())/* &&
                (ShouldSubPoolBeInvolved(subpoolkey))*/)
        {
            /*subpool deletion will lead to disk and OBJ deletion in AST side*/
            subpooldelmsg = new ReportDeleteSubPoolMessage(subpoolkey);
            YWB_ASSERT(subpooldelmsg != NULL);
            SendRemovedSubPool(subpooldelmsg);
        }
        else/*if(ShouldDiskBeInvolved(diskkey))*/
        {
            diskdelmsg = new ReportDeleteDiskMessage(diskkey);
            YWB_ASSERT(diskdelmsg != NULL);
            SendRemovedDisk(diskdelmsg);
        }
    }

    mConfigLock.unlock();

#ifdef WITH_INITIAL_PLACEMENT
    /*notify the disk selector about its removing/removed state instantly*/
    YWB_ASSERT(mPolicyMgr != NULL);
    mPolicyMgr->Adjust(subpoolkey);
    mPolicyMgr->UpdateDiskSelector(subpoolkey, diskkey,
            DiskSelector::DSE_remove_disk);
    /*tell HUB about those potentially cancel swap disk */
    LaunchSwapCancelledDiskEvent(diskkey);
#endif

    return ret;
}

ywb_status_t HUB::AddOBJ(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    mConfigLock.wrlock();
    ret = mConfigView->AddOBJView(objkey);
    mConfigLock.unlock();

    return ret;
}

ywb_status_t HUB::DeleteOBJ(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolKey subpoolkey;
    ywb_uint32_t aststatus = HUB::AS_disabled;
    ReportDeleteOBJMessage* deletionmsg = NULL;
    ywb_bool_t diskfound = ywb_false;
    ywb_bool_t subpoolfound = ywb_false;

    aststatus = GetASTStatus();
    mConfigLock.wrlock();
    if(YWB_SUCCESS == mConfigView->GetDiskKeyFromID(
            objkey.GetStorageId(), &diskkey))
    {
        diskfound = ywb_true;
    }

    if(YWB_SUCCESS == mConfigView->GetSubPoolKeyFromDiskID(
            objkey.GetStorageId(), subpoolkey))
    {
        subpoolfound = ywb_true;
    }

    ret = mConfigView->DeleteOBJView(objkey);
    if((YWB_SUCCESS == ret) && ((HUB::AS_disabling != aststatus) &&
            (HUB::AS_disabled != aststatus)))
    {
        if(ywb_false == subpoolfound)
        {
            /*
             * send OBJ deletion message anyway if can not find
             * corresponding subpool
             * */
            deletionmsg = new ReportDeleteOBJMessage(objkey);
            YWB_ASSERT(deletionmsg != NULL);
            SendRemovedOBJ(deletionmsg);
        }
        else if(ShouldSubPoolBeInvolved(subpoolkey))
        {
            /*skip the subpool if can not find it*/
            deletionmsg = new ReportDeleteOBJMessage(objkey);
            YWB_ASSERT(deletionmsg != NULL);
            SendRemovedOBJ(deletionmsg);
        }
    }

    mConfigLock.unlock();

#ifdef WITH_INITIAL_PLACEMENT
    if((ywb_true == subpoolfound) && (ywb_true == diskfound))
    {
        /*notify the disk selector about OBJ removing instantly*/
        YWB_ASSERT(mPolicyMgr != NULL);
        mPolicyMgr->UpdateDiskSelector(subpoolkey, diskkey,
                DiskSelector::DSE_remove_obj);
    }
#endif

    return ret;
}

ywb_status_t HUB::AccessOBJ(OBJKey& objkey, off_t offset,
        size_t len, ywb_uint32_t ios, ywb_uint32_t bw,
        ywb_uint32_t rt, ywb_bool_t read)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskView* diskview = NULL;

    mConfigLock.rdlock();
    ret = GetDiskView(objkey, &diskview);
    if(YWB_SUCCESS == ret)
    {
        ret = diskview->UpdateOBJView(objkey, offset, len, ios, bw, rt, read);
    }

    mConfigLock.unlock();
    return ret;
}

void HUB::ResetOBJRespFree()
{
    mCollectOBJRespFree->Reset();
}

void HUB::SwitchOBJRespFree()
{
    if(mCollectOBJResp == mCollectOBJRespFree)
    {
        mCollectOBJRespFree = mCollectOBJResp2;
    }
    else
    {
        mCollectOBJRespFree = mCollectOBJResp;
    }
}

void HUB::ResetOBJRespFragmentBatchFree()
{
    ywb_uint32_t index = 0;

    YWB_ASSERT(mCollectOBJRespFragmentBatchFree != NULL);
    for(index = 0; index < Constant::OBJ_IO_RESPONSE_BATCH_NUM; index++)
    {
        mCollectOBJRespFragmentBatchFree[index]->Reset();
    }
}

void HUB::SwitchOBJRespFragmentBatchFree()
{
    if(mCollectOBJRespFragmentBatch == mCollectOBJRespFragmentBatchFree)
    {
        mCollectOBJRespFragmentBatchFree = mCollectOBJRespFragmentBatch2;
    }
    else
    {
        mCollectOBJRespFragmentBatchFree = mCollectOBJRespFragmentBatch;
    }
}

ywb_status_t HUB::CollectConfigView()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t aststatus = HUB::AS_disabled;
    ywb_uint32_t subpoolnum = 0;
    ywb_uint32_t disknum = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    DiskInfo diskinfo;
    SubPoolView* subpoolview = NULL;
    DiskView* diskview = NULL;
    DiskBaseProp* diskprop = NULL;

    mCollectConfigResp->Reset();
    mCollectConfigResp->SetSeqNo(mCollectConfigSeq);
    aststatus = GetASTStatus();
    if((HUB::AS_disabling == aststatus) ||
            (HUB::AS_disabled == aststatus))
    {
        ret = YFS_ENODATA;
    }
    else
    {
        ret = GetFirstSubPoolView(&subpoolview, &subpoolkey);
        while((YWB_SUCCESS == ret) && (subpoolview != NULL))
        {
            /*only subpools with enabled ASTStatus will be added*/
            if(ShouldSubPoolBeInvolved(subpoolkey))
            {
                mCollectConfigResp->AddSubPool(subpoolkey);
                subpoolnum++;
            }

            ret = GetNextSubPoolView(&subpoolview, &subpoolkey);
        }

        if(subpoolnum != 0)
        {
            ret = GetFirstDiskView(&diskview, &diskkey);
            while((YWB_SUCCESS == ret) && (diskview != NULL))
            {
                /*only disk's subpool is in enabled ASTStatus will be added*/
                if(ShouldDiskBeInvolved(diskkey))
                {
                    diskview->GetDiskBaseProp(&diskprop);
                    diskinfo.SetDiskKey(diskkey);
                    diskinfo.SetBaseProp(*diskprop);
                    mCollectConfigResp->AddDisk(diskinfo);
                    disknum++;
                }

                ret = GetNextDiskView(&diskview, &diskkey);
            }
        }

        if((subpoolnum == 0) || (disknum == 0))
        {
            ret = YFS_ENODATA;
        }
        else if((subpoolnum != 0) && (disknum != 0))
        {
            ret = YWB_SUCCESS;
        }
    }

    mCollectConfigResp->SetStatus(ret);
    SendConfig();

    return ret;
}

ywb_status_t HUB::CollectIO(ywb_status_t firstrequest)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t index = 0;
    ywb_uint32_t objnumperresponse = 0;
    ywb_uint32_t aststatus = HUB::AS_disabled;
    ywb_uint64_t objnum = 0;
    OBJKey objkey;
    OBJInfo objinfo;
    OBJView* objview = NULL;
    ChunkIOStat* iostat = NULL;
    CollectOBJResponseFragment* responsefrag = NULL;

    ResetOBJRespFree();
    mCollectOBJRespFree->SetSeqNo(mCollectIOSeq);
    ResetOBJRespFragmentBatchFree();
    objnumperresponse = (Constant::SINGLE_OBJ_IO_RESPONSE_SIZE) / (sizeof(objinfo));
    YWB_ASSERT(objnumperresponse > 0);
    aststatus = GetASTStatus();

    if((0 == mToBeCollectedOBJsNum) ||
            (mAlreadyCollectedOBJsNum ==
            mToBeCollectedOBJsNum) ||
            (HUB::AS_disabling == aststatus) ||
            (HUB::AS_disabled == aststatus))
    {
        ret = YFS_ENODATA;
        mCollectOBJRespFree->SetStatus(ret);
        mCollectOBJRespFree->SetFragmentNum(0);
    }
    else
    {
        responsefrag = mCollectOBJRespFragmentBatchFree[index++];
        YWB_ASSERT(responsefrag != NULL);
        if(firstrequest)
        {
            ret = GetFirstOBJView(&objview, &objkey);
        }
        else
        {
            ret = GetNextOBJView(&objview, &objkey);
        }

        while((YWB_SUCCESS == ret) && (objview != NULL))
        {
            objview->GetChunkIOStat(&iostat);
            objinfo.SetOBJKey(objkey);
            objinfo.SetChunkIOStat(*iostat);
            responsefrag->AddOBJ(objinfo);

            /*clear the stat info of @objview*/
            objview->Reset();
            mAlreadyCollectedOBJsNum++;
            objnum++;
            if(objnum < objnumperresponse)
            {
                ;
            }
            else if((index < Constant::OBJ_IO_RESPONSE_BATCH_NUM))
            {
                responsefrag->SetStatus(YWB_SUCCESS);
                responsefrag = mCollectOBJRespFragmentBatchFree[index++];
                YWB_ASSERT(responsefrag != NULL);
                objnum = 0;
            }
            else
            {
                responsefrag->SetStatus(YWB_SUCCESS);
                break;
            }

            ret = GetNextOBJView(&objview, &objkey);
        }

        if((0 != objnum))
        {
            ret = YWB_SUCCESS;
            responsefrag->SetStatus(ret);
        }

        if((YWB_SUCCESS == mCollectOBJRespFragmentBatchFree[0]->GetStatus()) &&
                (mAlreadyCollectedOBJsNum < mToBeCollectedOBJsNum))
        {
            mCollectOBJRespFree->SetFlag(CollectOBJResponse::F_next_batch);
            mCollectOBJRespFree->SetStatus(YWB_SUCCESS);
        }
        else
        {
            mCollectOBJRespFree->SetFlag(CollectOBJResponse::F_end);
            mCollectOBJRespFree->SetStatus(mCollectOBJRespFragmentBatchFree[0]->GetStatus());
        }

        if(YWB_SUCCESS == mCollectOBJRespFragmentBatchFree[index - 1]->GetStatus())
        {
            mCollectOBJRespFree->SetFragmentNum(index);
        }
        else
        {
            mCollectOBJRespFree->SetFragmentNum(index - 1);
        }
    }

    mCollectOBJRespFree->SetFragments(mCollectOBJRespFragmentBatchFree);
    SendIO();
    SwitchOBJRespFree();
    SwitchOBJRespFragmentBatchFree();

    return ret;
}

void HUB::SendRemovedSubPool(ReportDeleteSubPoolMessage* msg)
{
    ast_log_debug("send deleted subpool to AST");
    mMsgMgr->Enqueue(msg);
}

void HUB::SendRemovedDisk(ReportDeleteDiskMessage* msg)
{
    ast_log_debug("send deleted disk to AST");
    mMsgMgr->Enqueue(msg);
}

void HUB::SendRemovedOBJ(ReportDeleteOBJMessage* msg)
{
    ast_log_debug("send deleted OBJ to AST");
    mMsgMgr->Enqueue(msg);
}

void HUB::SendConfig()
{
    ast_log_debug("send collected configuration to AST");
    mMsgMgr->Enqueue(mCollectConfigResp);
}

void HUB::SendIO()
{
    ast_log_debug("send collected OBJ IO to AST");
    mMsgMgr->Enqueue(mCollectOBJRespFree);
}

#ifdef WITH_INITIAL_PLACEMENT
void HUB::SendRelocationTypeNotification()
{
    if(!(mRelocationTypeNotification->Empty()))
    {
        ast_log_debug("send relocation type notification to AST");
        mMsgMgr->Enqueue(mRelocationTypeNotification);
    }
}

void HUB::SendSwapRequiredDisks()
{
    if(!(mSwapRequirementNotification->Empty()))
    {
        ast_log_debug("send swap required disks to AST");
        mMsgMgr->Enqueue(mSwapRequirementNotification);
    }
}

void HUB::SendSwapCancelledDisks()
{
    if(!(mSwapCancellationNotification->Empty()))
    {
        ast_log_debug("send swap cancelled disks to AST");
        mMsgMgr->Enqueue(mSwapCancellationNotification);
    }
}
#endif

ywb_status_t HUB::Migrate(OBJKey& objkey, DiskKey& tgtdisk)
{
    ywb_status_t err = YWB_SUCCESS;
    DiskKey srcdisk;
    FS::Object::Migrate *repl = NULL;
    FS::Basic::Header *header = NULL;
    FS::Basic::Chunk *chunk = NULL;
    ReselectStorageParam *rsp = NULL;
    OpCtx *ctx = NULL;

    ast_log_debug("HUB is about to migrate OBJ ["
            << objkey.GetStorageId()
            << ", " << objkey.GetInodeId()
            << ", " << objkey.GetChunkId()
            << ", " << objkey.GetChunkIndex()
            << ", " << objkey.GetChunkVersion()
            << "], tgtdisk [" << tgtdisk.GetSubPoolId()
            << ", " << tgtdisk.GetDiskId() << "]");

    /*we are sure ASTCommDispatcher already setup*/
    ctx = mASTCommDispatcher->AllocContext();
    if(NULL == ctx)
    {
        ast_log_fatal("HUB migrate OBJ failed, out of memory");
        return YFS_EOUTOFMEMORY;
    }

    repl = new FS::Object::Migrate;
    if(NULL == repl)
    {
        ast_log_fatal("HUB migrate OBJ failed, out of memory");
        return YFS_EOUTOFMEMORY;
    }

    rsp = new ReselectStorageParam;
    if (rsp == NULL) {
        ast_log_fatal("HUB migrate OBJ failed, out of memory");
        return YFS_EOUTOFMEMORY;
    }

    header = repl->mutable_header();
    chunk = repl->mutable_chunk();
    header->set_seqno(0xabcd);
    header->set_topo_version(GetTableVersion(CommunicationLayout::AT_unit));
    header->set_message_version(FS_PROTOCOL_VERSION);

    chunk->set_inodeid(objkey.GetInodeId());
    chunk->set_index(objkey.GetChunkIndex());
    chunk->set_chunkid(objkey.GetChunkId());
    chunk->set_version(objkey.GetChunkVersion());
    chunk->set_snapid(0);
    repl->set_poolid(tgtdisk.GetSubPoolId());
    chunk->set_diskid(tgtdisk.GetDiskId());

    ctx->SetProtocolParam(repl);
    ctx->mSeqno = 0xabcd;
    ctx->mTableVersion = GetTableVersion(CommunicationLayout::AT_unit);
    ctx->SetOpCode(OpCode::EOC_reselect_path);
    ctx->SetCustomParam(OpCtx::CPT_reselect_storage_param, rsp);
    ctx->mInodeId = objkey.GetInodeId();
    ctx->mChunkIndex = objkey.GetChunkIndex();
    ctx->mChunkId = objkey.GetChunkId();
    ctx->mChunkVersion = objkey.GetChunkVersion();
    ctx->mDstSID = GetSelfServiceId();
#if WITH_POOL_SUPPORT
    ctx->mPoolId = tgtdisk.GetSubPoolId();
#endif
    ctx->mSlot = tgtdisk.GetDiskId();

#if !(WITH_POOL_SUPPORT)
    YWB_ASSERT(ctx->mPoolId == 0ULL);
#endif

    srcdisk.SetDiskId(objkey.GetStorageId());
    srcdisk.SetSubPoolId(tgtdisk.GetSubPoolId());
    objkey.SetStorageId(tgtdisk.GetDiskId());
    /*
     * ignore the return value, there do exist the situation that the to be
     * added entry already existed: the migration completion notification
     * may be lost, or another new migration request with the same OBJKey
     * comes before the migration completion notification reaches...
     * */
    AddIntoOBJ2SrcDiskTable(objkey, srcdisk);
    mControlCenter->ForwardToObjectLayout(ctx);
    ast_log_debug("HUB migrate with context: " << ctx->ToLogString());

    return err;
}

ywb_status_t HUB::CompleteMigrate(OBJKey& objkey, ywb_status_t status)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<OBJKey, DiskKey, OBJKeyCmp>::iterator iter;
    OBJKey srcobjkey;
    DiskKey srcdisk;
    DiskKey tgtdisk;
    DiskView* srcdiskview = NULL;
    DiskView* tgtdiskview = NULL;
    AST* ast = NULL;
    Migration* migrationmgr = NULL;
    Migrator* migrator = NULL;
    ywb_uint32_t aststatus = HUB::AS_disabled;

    ast_log_debug("HUB complete migrate OBJ ["
            << objkey.GetStorageId()
            << ", " << objkey.GetInodeId()
            << ", " << objkey.GetChunkId()
            << ", " << objkey.GetChunkIndex()
            << ", " << objkey.GetChunkVersion()
            << "], status: " << status);

    ast = mAST;
    ast->GetMigration(&migrationmgr);
    migrationmgr->GetCurrentMigrator(&migrator);
    /*Migrator may already been set to NULL if AST received DisableAST message*/
    /*YWB_ASSERT(migrator != NULL);*/

    /*corresponding entry should be deleted regardless of ASTStatus*/
    iter = mMigratingOBJ2SrcDisk.find(objkey);
    YWB_ASSERT(iter != mMigratingOBJ2SrcDisk.end());
    srcdisk = iter->second;
    mMigratingOBJ2SrcDisk.erase(iter);
    tgtdisk.SetDiskId(objkey.GetStorageId());
    tgtdisk.SetSubPoolId(srcdisk.GetSubPoolId());

    srcobjkey = objkey;
    srcobjkey.SetStorageId(srcdisk.GetDiskId());
    /*only move OBJ from source to target if successfully migrated*/
    if(YWB_SUCCESS == status)
    {
        ret = MoveOBJFromSrc2Tgt(srcdisk, srcobjkey, tgtdisk, objkey);
        YWB_ASSERT(YWB_SUCCESS == ret);
        /*mark this OBJ as unsettled*/
        MarkOBJAsSrcUnsettled(srcobjkey, tgtdisk);
        MarkOBJAsTgtUnsettled(objkey, srcdisk);
    }

    aststatus = GetASTStatus();
    if((HUB::AS_disabling != aststatus) &&
            (HUB::AS_disabled != aststatus) &&
            (migrator != NULL))
    {
        /*tell migrator that the migration completion*/
        migrator->LaunchCompleteMigrationEvent(objkey, status);
    }

    mConfigView->GetDiskView(srcdisk, &srcdiskview);
    YWB_ASSERT(srcdiskview != NULL);
    mConfigView->GetDiskView(tgtdisk, &tgtdiskview);
    YWB_ASSERT(tgtdiskview != NULL);
    srcdiskview->DecPinnedNum(1);
    if((srcdiskview->TestFlag(DiskView::DV_delay_deleted)) &&
            (0 == srcdiskview->GetPinnedNum()))
    {
        mConfigView->RemoveDiskView(srcdisk);
    }

    tgtdiskview->DecPinnedNum(1);
    if((tgtdiskview->TestFlag(DiskView::DV_delay_deleted)) &&
            (0 == tgtdiskview->GetPinnedNum()))
    {
        mConfigView->RemoveDiskView(tgtdisk);
    }

    return ret;
}

ywb_status_t HUB::BuildRelocationTypeNotification()
{
    mRelocationTypeNotification->Reset();
    return mRelocationTypeNotification->Assign(&mSubPoolRelocationTypes);
}

ywb_status_t HUB::BuildSwapRequiredDisksNotification()
{
    mSwapRequirementNotification->Reset();
    return mSwapRequirementNotification->Assign(&mSwapRequiredDisks);
}

ywb_status_t HUB::BuildSwapCancelledDisksNotification()
{
    mSwapCancellationNotification->Reset();
    return mSwapCancellationNotification->Assign(&mSwapCancelledDisks);
}

ywb_status_t HUB::AddIntoOBJ2SrcDiskTable(OBJKey& objkey, DiskKey& tgtdisk)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<OBJKey, DiskKey, OBJKeyCmp>::iterator, bool> pairret;

    pairret = mMigratingOBJ2SrcDisk.insert(::make_pair(objkey, tgtdisk));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t HUB::GetSrcDiskByOBJKey(OBJKey& objkey, DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<OBJKey, DiskKey, OBJKeyCmp>::iterator iter;

    iter = mMigratingOBJ2SrcDisk.find(objkey);
    if(iter != mMigratingOBJ2SrcDisk.end())
    {
        *diskkey = iter->second;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t HUB::RemoveFromOBJ2SrcDiskTable(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<OBJKey, DiskKey, OBJKeyCmp>::iterator iter;

    iter = mMigratingOBJ2SrcDisk.find(objkey);
    if(iter != mMigratingOBJ2SrcDisk.end())
    {
        mMigratingOBJ2SrcDisk.erase(iter);
    }

    return ret;
}

ywb_status_t HUB::MarkOBJAsSrcUnsettled(OBJKey& objkey, DiskKey& diskkey)
{
    std::pair<std::map<OBJKey, DiskKey, OBJKeyCmp>::iterator, bool> pairret;

    mSrcUnsettledLock.Lock();
    pairret = mSrcUnsettledOBJs.insert(std::make_pair(objkey, diskkey));
    mSrcUnsettledLock.Unlock();

    if(true == pairret.second)
    {
        return YWB_SUCCESS;
    }
    else
    {
        return YFS_EEXIST;
    }
}

ywb_status_t HUB::GetSrcUnsettledOBJ(OBJKey& objkey, DiskKey& diskkey)
{
    std::map<OBJKey, DiskKey, OBJKeyCmp>::iterator iter;

    mSrcUnsettledLock.Lock();
    iter = mSrcUnsettledOBJs.find(objkey);
    mSrcUnsettledLock.Unlock();

    if(mSrcUnsettledOBJs.end() != iter)
    {
        diskkey = iter->second;
        return YWB_SUCCESS;
    }
    else
    {
        return YFS_ENOENT;
    }
}

ywb_status_t HUB::RemoveSrcUnsettleOBJ(OBJKey& objkey)
{
    std::map<OBJKey, DiskKey, OBJKeyCmp>::iterator iter;

    mSrcUnsettledLock.Lock();
    iter = mSrcUnsettledOBJs.find(objkey);
    if(mSrcUnsettledOBJs.end() != iter)
    {
        mSrcUnsettledOBJs.erase(iter);
        mSrcUnsettledLock.Unlock();
        return YWB_SUCCESS;
    }
    else
    {
        mSrcUnsettledLock.Unlock();
        return YFS_ENOENT;
    }
}

ywb_status_t HUB::MarkOBJAsTgtUnsettled(OBJKey& objkey, DiskKey& diskkey)
{
    std::pair<std::map<OBJKey, DiskKey, OBJKeyCmp>::iterator, bool> pairret;

    mTgtUnsettledLock.Lock();
    pairret = mTgtUnsettledOBJs.insert(std::make_pair(objkey, diskkey));
    mTgtUnsettledLock.Unlock();

    if(true == pairret.second)
    {
        return YWB_SUCCESS;
    }
    else
    {
        return YFS_EEXIST;
    }
}

ywb_status_t HUB::GetTgtUnsettledOBJ(OBJKey& objkey, DiskKey& diskkey)
{
    std::map<OBJKey, DiskKey, OBJKeyCmp>::iterator iter;

    mTgtUnsettledLock.Lock();
    iter = mTgtUnsettledOBJs.find(objkey);
    mTgtUnsettledLock.Unlock();

    if(mTgtUnsettledOBJs.end() != iter)
    {
        diskkey = iter->second;
        return YWB_SUCCESS;
    }
    else
    {
        return YFS_ENOENT;
    }
}

ywb_status_t HUB::RemoveTgtUnsettleOBJ(OBJKey& objkey)
{
    std::map<OBJKey, DiskKey, OBJKeyCmp>::iterator iter;

    mTgtUnsettledLock.Lock();
    iter = mTgtUnsettledOBJs.find(objkey);

    if(mTgtUnsettledOBJs.end() != iter)
    {
        mTgtUnsettledOBJs.erase(iter);
        mTgtUnsettledLock.Unlock();
        return YWB_SUCCESS;
    }
    else
    {
        mTgtUnsettledLock.Unlock();
        return YFS_ENOENT;
    }
}

/*FIXME: a more sophisticated implementation*/
ywb_status_t HUB::MoveOBJFromSrc2Tgt(DiskKey& srcdisk,
        OBJKey& srcobj, DiskKey& tgtdisk, OBJKey& tgtobj)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskView* srcdiskview = NULL;
    DiskView* tgtdiskview = NULL;
    OBJView* srcobjview = NULL;
    OBJView* tgtobjview = NULL;
    ChunkIOStat* iostat = NULL;
    AccessTrack* track = NULL;

    mConfigLock.wrlock();
    ret = mConfigView->GetDiskView(srcdisk, &srcdiskview);
    YWB_ASSERT(srcdiskview != NULL);
    ret = mConfigView->GetDiskView(tgtdisk, &tgtdiskview);
    YWB_ASSERT(tgtdiskview != NULL);
    ret = srcdiskview->GetOBJView(srcobj, &srcobjview);
    YWB_ASSERT(srcobjview != NULL);
    srcobjview->GetChunkIOStat(&iostat);
    srcobjview->GetAccessTrack(&track);
    tgtobjview = new OBJView(srcobjview->GetRemainingRT(), *iostat, *track);
    YWB_ASSERT(tgtobjview != NULL);
    ret = tgtdiskview->AddOBJView(tgtobj, tgtobjview);
    YWB_ASSERT(YWB_SUCCESS == ret);
    srcdiskview->DeleteOBJView(srcobj);
    mConfigLock.unlock();

    return ret;
}

/*copy from AttrNode/Chunk/YfsChunksMgr.cpp*/
ywb_bool_t
TimeScheduleConfig::TimeIsScheduled(TimeScheduleConfig *schedule)
{
    ywb_time_exp_t exptime;
    ywb_status_t err = YWB_SUCCESS;
    ywb_uint32_t seconds = 0;
    /*TimeRange range;*/
    std::vector<TimeRange>::iterator iter;

    err = ywb_time_exp_lt(&exptime, ywb_time_now());
    if(YWB_SUCCESS != err) {
        return ywb_true;
    }

    if(!((0x80 >> exptime.tm_wday) & schedule->mWeekDays)) {
        return ywb_false;
    }

    seconds = (exptime.tm_hour * 3600) + (exptime.tm_min * 60) + exptime.tm_sec;
    /*range.SetStart(seconds);
    range.SetEnd(seconds);
    iter = std::lower_bound(schedule->mRangeList.begin(), schedule->mRangeList.end(), range);
    if(schedule->mRangeList.end() == iter) {
        return ywb_false;
    }

    if(iter->GetStart() > seconds || iter->GetEnd() < seconds) {
        return ywb_false;
    }*/

    iter = schedule->mRangeList.begin();
    while(schedule->mRangeList.end() != iter)
    {
        if((iter->GetStart() <= seconds) && (iter->GetEnd() > seconds))
        {
            return ywb_true;
        }

        iter++;
    }

    return ywb_false;
}

ywb_status_t
TimeScheduleConfig::AddTimeRange(TimeRange& newrange, ywb_bool_t newtimeschedule)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::vector<TimeRange>::iterator iter;
    TimeRange range;
    ywb_bool_t shouldadd = false;
    std::vector<TimeRange> tempvec;
    TimeRange prevrange;
    TimeRange nextrange;
    TimeRange temprange;

    if(mRangeList.empty())
    {
        mRangeList.push_back(newrange);
    }
    else
    {
        /*
         * if fulldayrange exist, it must be at index 0, and there is
         * exactly one item in the vector
         * */
        range = mRangeList.at(0);
        if((0 == range.GetStart()) && (86401 == range.GetEnd()))
        {
            YWB_ASSERT(1 == mRangeList.size());
            if(newtimeschedule)
            {
                /*erase fullday range*/
                mRangeList.pop_back();
                mRangeList.push_back(newrange);
            }
        }
        else
        {
            iter = mRangeList.begin();
            /*
             * Adjust:
             * firstly adjust every timerange in the vector
             * */
            while(mRangeList.end() != iter)
            {
                range = *iter;

                if((newrange.GetStart() == range.GetStart()) &&
                        (newrange.GetEnd() == range.GetEnd()))
                {
                    /*
                     * case 1:
                     *          ------              range
                     *          ------              new range
                     * */
                    break;
                }
                else if(newrange.GetEnd() < range.GetStart())
                {
                    /*
                     * case 2:
                     *          ------              range
                     *   ----                       new range
                     * */
                    /*DO NOT add it into @mRangeList here*/
                    shouldadd = true;
                }
                else if((newrange.GetStart() < range.GetStart()) &&
                        (range.GetStart() <= newrange.GetEnd()) &&
                        (newrange.GetEnd() < range.GetEnd()))
                {
                    /*
                     * case 3:
                     *          ------              range
                     *   --------                   new range
                     * */
                    iter->SetStart(newrange.GetStart());
                }
                else if((newrange.GetStart() <= range.GetStart()) &&
                        (newrange.GetEnd() >= range.GetEnd()))
                {
                    /*
                     * case 4:
                     *          ------              range
                     *   --------------             new range
                     * */
                    iter->SetStart(newrange.GetStart());
                    iter->SetEnd(newrange.GetEnd());
                }
                else if((newrange.GetStart() > range.GetStart()) &&
                        (newrange.GetEnd() < range.GetEnd()))
                {
                    /*
                     * case 5:
                     *          ------              range
                     *           ----               new range
                     * */
                    break;
                }
                else if((newrange.GetStart() > range.GetStart()) &&
                        (newrange.GetStart() <= range.GetEnd()) &&
                        (newrange.GetEnd() > range.GetEnd()))
                {
                    /*
                     * case 6:
                     *          ------              range
                     *           --------           new range
                     * */
                    iter->SetEnd(newrange.GetEnd());
                }
                else if(newrange.GetStart() > range.GetEnd())
                {
                    /*
                     * case 7:
                     *          ------              range
                     *                  --------    new range
                     * */
                    /*DO NOT add it into @mRangeList here*/
                    shouldadd = true;
                }
                else
                {
                    ast_log_debug("Unexpected case, time range [start: "
                            << range.GetStart() << ", end: " << range.GetEnd()
                            << "], and new time range [start: "
                            << range.GetStart() << ", end: " << range.GetEnd()
                            << "]");
                    ret = YFS_EINVAL;
                }

                iter++;
            }

            if(shouldadd)
            {
                mRangeList.push_back(newrange);
                sort(mRangeList.begin(), mRangeList.end(), TimeRangeCmp::TimeRangeLessThan);
            }

            /*
             * Merge:
             * merge all entries if possible
             * (1) we are sure there is at least one entry
             * (2) we are sure all entries are sorted in start-ascending order
             * */
            iter = mRangeList.begin();
            prevrange = *iter;
            iter++;
            while(mRangeList.end() != iter)
            {
                nextrange = *iter;
                if(prevrange.GetEnd() < nextrange.GetStart())
                {
                    /*@prevrange and @nextrange has no overlap*/
                    tempvec.push_back(prevrange);
                    prevrange = nextrange;
                }
                else if(prevrange.GetEnd() >= nextrange.GetStart())
                {
                    /*@prevrange and @nextrange has overlap*/
                    temprange.SetStart(prevrange.GetStart());
                    temprange.SetEnd((nextrange.GetEnd() > prevrange.GetEnd() ?
                            nextrange.GetEnd() : prevrange.GetEnd()));
                    prevrange = temprange;
                }

                iter++;
            }

            tempvec.push_back(prevrange);
            mRangeList.clear();
            mRangeList.assign(tempvec.begin(), tempvec.end());
            tempvec.clear();
            sort(mRangeList.begin(), mRangeList.end(), TimeRangeCmp::TimeRangeLessThan);
        }
    }

    return ret;
}

ywb_status_t
TimeScheduleConfig::AddTimeRanges(vector<TimeRange>& ranges)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t index = 0;

    if(!(ranges.empty()))
    {
        Reset();
        AddTimeRange(ranges.at(0), true);

        for(index = 1; index < ranges.size(); index++)
        {
            ret = AddTimeRange(ranges.at(index), false);
            if(YWB_SUCCESS != ret)
            {
                break;
            }
        }
    }
    else
    {
        ret = YFS_ENODATA;
    }

    return ret;
}

ywb_bool_t
TimeScheduleConfig::CheckTimeScheduleConfigEqual(
        vector<TimeRange>& timeranges, ywb_uint32_t weekdays)
{
    ywb_uint32_t index = 0;

    if(mWeekDays == weekdays)
    {
        if(mRangeList.size() == timeranges.size())
        {
            for(index = 0; index < timeranges.size(); index++)
            {
                if(timeranges.at(index) != mRangeList.at(index))
                {
                    return false;
                }
            }

            return true;
        }
    }

    return false;
}

void TimeScheduleConfig::Reset()
{
    mRangeList.clear();
    TimeRange fulldayrange(0, 86401);
    mRangeList.push_back(fulldayrange);
}

/*FIXME: A more sophisticated algorithm*/
ywb_uint32_t SpinningDiskAccessModel::DeterminAccessModel(
        AccessTrack* history, off_t curoffset,
        size_t curlen, ywb_bool_t read)
{
    ywb_uint32_t model = AccessModel::AM_none;
    off_t lastoffset = 0;
    size_t lastlen = 0;

    lastoffset = history->GetLastOffset();
    lastlen = history->GetLastLen();
    YWB_ASSERT(lastoffset >= 0);
    YWB_ASSERT(lastlen >= 0);

    if((curoffset > lastoffset) &&
            (lastlen == (size_t)(curoffset - lastoffset)))
    {
        model |= AccessModel::AM_seq;
    }
    else
    {
        model |= AccessModel::AM_rnd;
    }

    if(read)
    {
        model |= AccessModel::AM_read;
    }
    else
    {
        model |= AccessModel::AM_write;
    }

    return model;
}

/*FIXME: currently same algorithm as Spinning drives*/
ywb_uint32_t SSDAccessModel::DeterminAccessModel(
        AccessTrack* history, off_t curoffset,
        size_t curlen, ywb_bool_t read)
{
    ywb_uint32_t model = AccessModel::AM_none;
    off_t lastoffset = 0;
    size_t lastlen = 0;

    lastoffset = history->GetLastOffset();
    lastlen = history->GetLastLen();
    YWB_ASSERT(lastoffset >= 0);
    YWB_ASSERT(lastlen >= 0);

    if((curoffset > lastoffset) &&
            (lastlen == (size_t)(curoffset - lastoffset)))
    {
        model |= AccessModel::AM_seq;
    }
    else
    {
        model |= AccessModel::AM_rnd;
    }

    if(read)
    {
        model |= AccessModel::AM_read;
    }
    else
    {
        model |= AccessModel::AM_write;
    }

    return model;
}

void OBJView::Update(ywb_uint32_t model, off_t offset, size_t len,
        ywb_uint32_t ios, ywb_uint32_t bw, ywb_uint32_t rtusec)
{
    ywb_uint32_t rtmsec = 0;
    ywb_uint32_t totalusec = 0;

    YWB_ASSERT(((AccessModel::AM_rnd | AccessModel::AM_read) == model) ||
            ((AccessModel::AM_rnd | AccessModel::AM_write) == model) ||
            ((AccessModel::AM_seq | AccessModel::AM_read) == model) ||
            ((AccessModel::AM_seq | AccessModel::AM_write) == model));

    /*the passed @rt is in measure of usec, transfer into msec at first*/
    totalusec = mRemainingRT + rtusec;
    rtmsec = totalusec / (1000);
    mRemainingRT = totalusec % (1000);

    if((AccessModel::AM_rnd | AccessModel::AM_read) == model)
    {
        mStat.SetRndReadIOs(mStat.GetRndReadIOs() + ios);
        mStat.SetRndReadBW(mStat.GetRndReadBW() + bw);
        mStat.SetRndTotalRT(mStat.GetRndTotalRT() + rtmsec);
    }
    else if((AccessModel::AM_rnd | AccessModel::AM_write) == model)
    {
        mStat.SetRndWriteIOs(mStat.GetRndWriteIOs() + ios);
        mStat.SetRndWriteBW(mStat.GetRndWriteBW() + bw);
        mStat.SetRndTotalRT(mStat.GetRndTotalRT() + rtmsec);
    }
    else if((AccessModel::AM_seq | AccessModel::AM_read) == model)
    {
        mStat.SetSeqReadIOs(mStat.GetSeqReadIOs() + ios);
        mStat.SetSeqReadBW(mStat.GetSeqReadBW() + bw);
        mStat.SetSeqTotalRT(mStat.GetSeqTotalRT() + rtmsec);
    }
    else
    {
        mStat.SetSeqWriteIOs(mStat.GetSeqWriteIOs() + ios);
        mStat.SetSeqWriteBW(mStat.GetSeqWriteBW() + bw);
        mStat.SetSeqTotalRT(mStat.GetSeqTotalRT() + rtmsec);
    }

    mAccess.SetLastOffset(offset);
    mAccess.SetLastLen(len);
}

void OBJView::Reset()
{
    mRemainingRT = 0;
    mStat.SetRndReadIOs(0);
    mStat.SetRndWriteIOs(0);
    mStat.SetRndTotalRT(0);
    mStat.SetRndReadBW(0);
    mStat.SetRndWriteBW(0);
    mStat.SetSeqReadIOs(0);
    mStat.SetSeqWriteIOs(0);
    mStat.SetSeqTotalRT(0);
    mStat.SetSeqReadBW(0);
    mStat.SetSeqWriteBW(0);
    mAccess.Reset();
}

ywb_status_t DiskView::AddOBJView(OBJKey& objkey, OBJView* objinfo)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::pair<std::map<OBJKey, OBJView*, OBJKeyCmp>::iterator, bool> pairret;

    ast_log_debug("Add OBJView into DiskView, objkey["
            << objkey.GetStorageId()
            << ", " << objkey.GetInodeId()
            << ", " << objkey.GetChunkId()
            << ", " << objkey.GetChunkIndex()
            << ", " << objkey.GetChunkVersion()
            << "]");
    pairret = mOBJs.insert(std::make_pair(objkey, objinfo));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t DiskView::GetOBJView(OBJKey& objkey, OBJView** objinfo)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<OBJKey, OBJView*, OBJKeyCmp>::iterator iter;

    iter = mOBJs.find(objkey);
    if(mOBJs.end() != iter)
    {
        *objinfo = iter->second;
    }
    else
    {
        *objinfo = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t DiskView::DeleteOBJView(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<OBJKey, OBJView*, OBJKeyCmp>::iterator iter;
    OBJView* objview = NULL;

    ast_log_debug("Delete OBJView from DiskView, objkey["
            << objkey.GetStorageId()
            << ", " << objkey.GetInodeId()
            << ", " << objkey.GetChunkId()
            << ", " << objkey.GetChunkIndex()
            << ", " << objkey.GetChunkVersion()
            << "]");
    iter = mOBJs.find(objkey);
    if(mOBJs.end() != iter)
    {
        objview = iter->second;
        mOBJs.erase(iter);
        delete objview;
        objview = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t DiskView::UpdateOBJView(OBJKey& objkey,
        off_t offset, size_t len, ywb_uint32_t ios,
        ywb_uint32_t bw, ywb_uint32_t rt, ywb_bool_t read)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t model = AccessModel::AM_none;
    std::map<OBJKey, OBJView*, OBJKeyCmp>::iterator iter;
    OBJView* objview = NULL;
    AccessTrack* history = NULL;

    ast_log_debug("Update OBJView, objkey["
            << objkey.GetStorageId()
            << ", " << objkey.GetInodeId()
            << ", " << objkey.GetChunkId()
            << ", " << objkey.GetChunkIndex()
            << ", " << objkey.GetChunkVersion()
            << "]");
    iter = mOBJs.find(objkey);
    if(mOBJs.end() != iter)
    {
        objview = iter->second;
        objview->GetAccessTrack(&history);
        model = mModel->DeterminAccessModel(history, offset, len, read);
        objview->Update(model, offset, len, ios, bw, rt);
    }
    else
    {
        /*it is acceptable to keep calm even so instead of assertion*/
        /*YWB_ASSERT(0);*/
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t DiskView::GetFirstOBJView(OBJView** objview, OBJKey* objkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    mIt = mOBJs.begin();
    if(mIt != mOBJs.end())
    {
        *objkey = mIt->first;
        *objview = mIt->second;
        mIt++;
    }
    else
    {
        *objview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t DiskView::GetNextOBJView(OBJView** objview, OBJKey* objkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mOBJs.end() != mIt)
    {
        *objkey = mIt->first;
        *objview = mIt->second;
        mIt++;
    }
    else
    {
        *objview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_uint32_t DiskView::DetermineDiskTier()
{
    return mProp.DetermineDiskTier();
}

ywb_status_t TierView::AddDiskView(DiskKey& diskkey, DiskView* diskview)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::pair<std::map<DiskKey, DiskView*, DiskKeyCmp>::iterator, bool> pairret;

    pairret = mDisks.insert(std::make_pair(diskkey, diskview));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t TierView::GetDiskView(DiskKey& diskkey, DiskView** diskview)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<DiskKey, DiskView*, DiskKeyCmp>::iterator iter;

    iter = mDisks.find(diskkey);
    if(mDisks.end() != iter)
    {
        *diskview = iter->second;
    }
    else
    {
        *diskview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t TierView::RemoveDiskView(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<DiskKey, DiskView*, DiskKeyCmp>::iterator iter;
    /*DiskView* diskview = NULL;*/

    iter = mDisks.find(diskkey);
    if(mDisks.end() != iter)
    {
        /*diskview = iter->second;*/
        mDisks.erase(iter);
        /*it is SubPoolView's responsibility to de-construct it*/
        /*delete diskview;
        diskview = NULL;*/
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_uint32_t TierView::GetDiskCnt()
{
    return mDisks.size();
}

ywb_status_t TierView::GetTierWiseDiskCandidates(TierWiseDiskCandidates* candidates)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<DiskKey, DiskView*, DiskKeyCmp>::iterator iter;
    DiskKey diskkey;
    DiskView* diskview = NULL;
    DiskCandidate* diskcandidate = NULL;
    DiskBaseProp* diskprop = NULL;

    YWB_ASSERT(candidates != NULL);
    if(!(mDisks.empty()))
    {
        iter = mDisks.begin();
        while(mDisks.end() != iter)
        {
            diskkey = iter->first;
            diskview = iter->second;
            ret = candidates->GetDiskCandidate(diskkey, &diskcandidate);
            if(!((YWB_SUCCESS == ret) && (diskcandidate != NULL)))
            {
                candidates->AddDiskCandidate(diskkey);
                ret = candidates->GetDiskCandidate(diskkey, &diskcandidate);
                YWB_ASSERT(YWB_SUCCESS == ret);
            }
            else
            {
                diskcandidate->ClearFlag(DiskCandidate::DCF_suspecious);
            }

            diskview->GetDiskBaseProp(&diskprop);
            diskcandidate->SetTotalCap(diskprop->GetDiskCap());
            diskcandidate->SetAvailCap(diskprop->GetDiskCap() - diskview->GetOBJCnt());
            diskcandidate->SetRecentlyRemovedOBJNum(0);
            /*diskcandidate->SetAvailTickets(0);*/
            diskcandidate->SetIOs(0);
            diskcandidate->SetBWs(0);
            candidates->SetRecentlyRemovedOBJNum(0);

            iter++;
        }

        ret = YWB_SUCCESS;
    }
    else
    {
        ret = YFS_ENODATA;
    }

    return ret;
}

ywb_status_t SubPoolView::AddDiskView(DiskKey& diskkey, DiskView* diskview)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::pair<std::map<DiskKey, DiskView*, DiskKeyCmp>::iterator, bool> pairret;
    ywb_uint32_t tiertype = Tier::TIER_cnt;
    TierView* tierview = NULL;

    pairret = mDisks.insert(std::make_pair(diskkey, diskview));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }
    else
    {
        tiertype = diskview->DetermineDiskTier();
        ret = AddTierView(tiertype);
        if(YWB_SUCCESS == ret)
        {
            AdjustTierComb(tiertype, ywb_true);
            ret = GetTierView(tiertype, &tierview);
            YWB_ASSERT(YWB_SUCCESS == ret);
            tierview->AddDiskView(diskkey, diskview);
        }
    }

    return ret;
}

ywb_status_t SubPoolView::GetDiskView(DiskKey& diskkey, DiskView** diskview)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<DiskKey, DiskView*, DiskKeyCmp>::iterator iter;

    iter = mDisks.find(diskkey);
    if(mDisks.end() != iter)
    {
        *diskview = iter->second;
    }
    else
    {
        *diskview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t SubPoolView::RemoveDiskView(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<DiskKey, DiskView*, DiskKeyCmp>::iterator iter;
    TierView* tierview = NULL;
    DiskView* diskview = NULL;
    ywb_uint32_t tiertype = Tier::TIER_cnt;

    iter = mDisks.find(diskkey);
    if(mDisks.end() != iter)
    {
        diskview = iter->second;
        tiertype = diskview->GetTierType();
        ret = GetTierView(tiertype, &tierview);
        YWB_ASSERT(YWB_SUCCESS == ret);
        tierview->RemoveDiskView(diskkey);
        if(0 == tierview->GetDiskCnt())
        {
            ret = RemoveTierView(tiertype);
            if(YWB_SUCCESS == ret)
            {
                AdjustTierComb(tiertype, ywb_false);
            }
        }

        if(0 == diskview->GetPinnedNum())
        {
            if(diskview->TestFlag(DiskView::DV_delay_deleted))
            {
                mDelayDeletedDisksNum--;
            }

            mDisks.erase(iter);
            delete diskview;
            diskview = NULL;
        }
        else
        {
            diskview->SetFlagUniq(DiskView::DV_delay_deleted);
            mDelayDeletedDisksNum++;
            /*FIXME: what is a better errno?*/
            ret = YFS_EBUSY;
        }
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t SubPoolView::AddOBJView(
        DiskKey& diskkey, OBJKey& objkey, OBJView* objinfo)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskView* diskview = NULL;

    ret = GetDiskView(diskkey, &diskview);
    if(YWB_SUCCESS == ret)
    {
        ret = diskview->AddOBJView(objkey, objinfo);
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t SubPoolView::GetOBJView(
        DiskKey& diskkey, OBJKey& objkey, OBJView** objinfo)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskView* diskview = NULL;

    ret = GetDiskView(diskkey, &diskview);
    if(YWB_SUCCESS == ret)
    {
        ret = diskview->GetOBJView(objkey, objinfo);
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t SubPoolView::DeleteOBJView(
        DiskKey& diskkey, OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskView* diskview = NULL;

    ret = GetDiskView(diskkey, &diskview);
    if(YWB_SUCCESS == ret)
    {
        ret = diskview->DeleteOBJView(objkey);
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t SubPoolView::GetFirstDiskView(
        DiskView** diskview, DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    mIt = mDisks.begin();
    if(mIt != mDisks.end())
    {
        *diskkey = mIt->first;
        *diskview = mIt->second;
        mIt++;
    }
    else
    {
        *diskview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t SubPoolView::GetNextDiskView(
        DiskView** diskview, DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mDisks.end() != mIt)
    {
        *diskkey = mIt->first;
        *diskview = mIt->second;
        mIt++;
    }
    else
    {
        *diskview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t SubPoolView::GetFirstOBJView(OBJView** objview, OBJKey* objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskView* diskview = NULL;

    mIt = mDisks.begin();
    while(mIt != mDisks.end())
    {
        diskview = mIt->second;
        YWB_ASSERT(diskview != NULL);
        ret = diskview->GetFirstOBJView(objview, objkey);
        if(YWB_SUCCESS == ret)
        {
            break;
        }
        else
        {
            mIt++;
        }
    }

    if(mIt == mDisks.end())
    {
        *objview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t SubPoolView::GetNextOBJView(OBJView** objview, OBJKey* objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskView* diskview = NULL;

    if(mDisks.end() != mIt)
    {
        diskview = mIt->second;
        ret = diskview->GetNextOBJView(objview, objkey);
        /*reach the disk end*/
        if(YWB_SUCCESS != ret)
        {
            mIt++;
            while(mIt != mDisks.end())
            {
                diskview = mIt->second;
                YWB_ASSERT(diskview != NULL);
                ret = diskview->GetFirstOBJView(objview, objkey);
                if(YWB_SUCCESS == ret)
                {
                    break;
                }
                else
                {
                    mIt++;
                }
            }

            if(mDisks.end() == mIt)
            {
                *objview = NULL;
                ret = YFS_ENOENT;
            }
        }
    }
    else
    {
        *objview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t SubPoolView::AddTierView(ywb_uint32_t tiertype)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::list<TierView*>::iterator iter;
    TierView* tierview = NULL;

    YWB_ASSERT((Tier::TIER_0 < tiertype) && (tiertype < Tier::TIER_cnt));
    if(SubPool::TC_ssd_ent_sata == mCurTierComb)
    {
        ret = YFS_EEXIST;
        return ret;
    }

    iter = mTierViews.begin();
    while(mTierViews.end() != iter)
    {
        if(tiertype == (*iter)->GetTierType())
        {
            ret = YFS_EEXIST;
            break;
        }

        iter++;
    }

    if(mTierViews.end() == iter)
    {
        tierview = new TierView(tiertype);
        YWB_ASSERT(tierview != NULL);
        mTierViews.push_back(tierview);
    }

    return ret;
}

ywb_status_t SubPoolView::GetTierView(ywb_uint32_t tiertype, TierView** tierview)
{
    ywb_status_t ret = YFS_ENOENT;
    std::list<TierView*>::iterator iter;

    YWB_ASSERT((Tier::TIER_0 < tiertype) && (tiertype < Tier::TIER_cnt));
    *tierview = NULL;
    iter = mTierViews.begin();
    while(mTierViews.end() != iter)
    {
        if(tiertype == (*iter)->GetTierType())
        {
            ret = YWB_SUCCESS;
            *tierview = *iter;
            break;
        }

        iter++;
    }

    return ret;
}

ywb_status_t SubPoolView::RemoveTierView(ywb_uint32_t tiertype)
{
    ywb_status_t ret = YFS_ENOENT;
    std::list<TierView*>::iterator iter;
    TierView* tierview = NULL;

    YWB_ASSERT((Tier::TIER_0 < tiertype) && (tiertype < Tier::TIER_cnt));
    iter = mTierViews.begin();
    while(mTierViews.end() != iter)
    {
        if(tiertype == (*iter)->GetTierType())
        {
            /*double check*/
            if(0 == (*iter)->GetDiskCnt())
            {
                ret = YWB_SUCCESS;
                tierview = *iter;
                delete tierview;
                tierview = NULL;
            }
            else
            {
                ret = YFS_EBUSY;
            }

            break;
        }

        iter++;
    }

    return ret;
}

ywb_uint32_t SubPoolView::GetCurTierComb()
{
    return mCurTierComb;
}

ywb_uint32_t SubPoolView::AdjustTierComb(ywb_uint32_t tiertype, ywb_bool_t add)
{
    ywb_status_t ret = YWB_SUCCESS;

    YWB_ASSERT((Tier::TIER_0 < tiertype) && (tiertype < Tier::TIER_cnt));
    /*add tier into tier combination*/
    if(add)
    {
        if(mCurTierComb == SubPool::TC_cnt)
        {
            mCurTierComb = (1 << tiertype);
        }
        else
        {
            mCurTierComb |= (1 << tiertype);
        }
    }
    else
    {
        if(mCurTierComb != SubPool::TC_cnt)
        {
            mCurTierComb &= (~(1 << tiertype));
        }
    }

    return ret;
}

ywb_uint32_t SubPoolView::GetOBJCnt()
{
    ywb_uint32_t count = 0;
    std::map<DiskKey, DiskView*, DiskKeyCmp>::iterator iter;

    iter = mDisks.begin();
    while(mDisks.end() != iter)
    {
        count += iter->second->GetOBJCnt();
        iter++;
    }

    return count;
}

ywb_status_t ConfigView::GetDiskKeyFromID(
        ywb_uint64_t diskid, DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<ywb_uint64_t, DiskKey>::iterator iter;

    mLock.rdlock();
    iter = mDiskID2Key.find(diskid);
    if(mDiskID2Key.end() != iter)
    {
        *diskkey = iter->second;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    mLock.unlock();
    return ret;
}

ywb_status_t ConfigView::GetSubPoolKeyFromID(
        ywb_uint32_t subpoolid, SubPoolKey* subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<ywb_uint32_t, SubPoolKey>::iterator iter;

    mLock.rdlock();
    iter = mSubPoolID2Key.find(subpoolid);
    if(mSubPoolID2Key.end() != iter)
    {
        *subpoolkey = iter->second;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    mLock.unlock();
    return ret;
}

ywb_status_t
ConfigView::GetSubPoolKeyFromDiskID(ywb_uint64_t diskid,
        SubPoolKey& subpoolkey)
{
    std::map<ywb_uint64_t, DiskKey>::iterator iter;
    std::map<ywb_uint32_t, SubPoolKey>::iterator iter2;

    mLock.rdlock();
    iter = mDiskID2Key.find(diskid);
    if(mDiskID2Key.end() != iter)
    {
        iter2 = mSubPoolID2Key.find((iter->second).GetSubPoolId());
        if(mSubPoolID2Key.end() != iter2)
        {
            subpoolkey = iter2->second;
            mLock.unlock();
            return YWB_SUCCESS;
        }
    }

    mLock.unlock();
    return YFS_ENOENT;
}

ywb_status_t ConfigView::AddSubPoolView(
        SubPoolKey& subpoolkey, SubPoolView* subpoolview)
{
//    ywb_status_t ret = YWB_SUCCESS;
//    std::pair<std::map<ywb_uint32_t, SubPoolKey>::iterator, bool> pairret;
//    std::pair<std::map<SubPoolKey, SubPoolView*,
//            SubPoolKeyCmp>::iterator, bool> pairret2;
//
//    pairret = mSubPoolId2Key.insert(
//            std::make_pair(subpoolkey.GetSubPoolId(), subpoolkey));
//    if(pairret.second)
//    {
//        YWB_ASSERT(YFS_ENOENT == GetSubPoolView(subpoolkey, &subpoolview));
//        YWB_ASSERT(subpoolview != NULL);
//        pairret2 = mSubPools.insert(std::make_pair(subpoolkey, subpoolview));
//        YWB_ASSERT(true == pairret2.second);
//    }
//    else
//    {
//        ret = GetSubPoolView(subpoolkey, &subpoolview);
//        YWB_ASSERT(YWB_SUCCESS == ret);
//        YWB_ASSERT(NULL != subpoolview);
//        ret = YFS_EEXIST;
//    }
//
//    return ret;
    ywb_status_t ret = YWB_SUCCESS;
    std::pair<std::map<SubPoolKey, SubPoolView*,
            SubPoolKeyCmp>::iterator, bool> pairret;

    pairret = mSubPools.insert(std::make_pair(subpoolkey, subpoolview));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t ConfigView::GetSubPoolView(
        SubPoolKey& subpoolkey, SubPoolView** subpoolview)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<SubPoolKey, SubPoolView*, SubPoolKeyCmp>::iterator iter;

    mLock.rdlock();
    ret = GetSubPoolViewLocked(subpoolkey, subpoolview);
    mLock.unlock();

    return ret;
}

ywb_status_t ConfigView::GetSubPoolViewLocked(
        SubPoolKey& subpoolkey, SubPoolView** subpoolview)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::map<SubPoolKey, SubPoolView*, SubPoolKeyCmp>::iterator iter;

    iter = mSubPools.find(subpoolkey);
    if(mSubPools.end() != iter)
    {
        *subpoolview = iter->second;
    }
    else
    {
        *subpoolview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ConfigView::RemoveSubPoolView(SubPoolKey& subpoolkey)
{
//    ywb_status_t ret = YWB_SUCCESS;
//    std::map<ywb_uint32_t, SubPoolKey>::iterator iter;
//    std::map<SubPoolKey, SubPoolView*, SubPoolKeyCmp>::iterator iter2;
//    SubPoolView* subpoolview = NULL;
//
//    iter = mSubPoolId2Key.find(subpoolkey.GetSubPoolId());
//    if(mSubPoolId2Key.end() != iter)
//    {
//        mSubPoolId2Key.erase(iter);
//        iter2 = mSubPools.find(subpoolkey);
//        YWB_ASSERT(mSubPools.end() != iter2);
//        subpoolview = iter2->second;
//        delete subpoolview;
//        subpoolview = NULL;
//    }
//    else
//    {
//        YWB_ASSERT(YFS_ENOENT == GetSubPoolView(subpoolkey, &subpoolview));
//        ret = YFS_ENOENT;
//    }
//
//    return ret;
    ywb_status_t ret = YWB_SUCCESS;
    std::map<SubPoolKey, SubPoolView*, SubPoolKeyCmp>::iterator iter;
    SubPoolView* subpoolview = NULL;

    iter = mSubPools.find(subpoolkey);
    if(mSubPools.end() != iter)
    {
        subpoolview = iter->second;
        mSubPools.erase(iter);
        delete subpoolview;
        subpoolview = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ConfigView::AddDiskView(DiskKey& diskkey, DiskBaseProp& diskprop)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    pair<std::map<ywb_uint64_t, DiskKey>::iterator, bool> pairret;
    pair<std::map<ywb_uint32_t, SubPoolKey>::iterator, bool> pairret2;
    SubPoolView* subpoolview = NULL;
    DiskView* diskview = NULL;

    mLock.wrlock();
    pairret = mDiskID2Key.insert(
            std::make_pair(diskkey.GetDiskId(), diskkey));
    if(pairret.second)
    {
        subpoolkey.SetOss(mHUB->GetSelfServiceId());
        /*FIXME: what shall be the pool id*/
        subpoolkey.SetPoolId(diskkey.GetSubPoolId());
        subpoolkey.SetSubPoolId(diskkey.GetSubPoolId());
        pairret2 = mSubPoolID2Key.insert(
                std::make_pair(diskkey.GetSubPoolId(), subpoolkey));

        if(pairret2.second)
        {
            YWB_ASSERT(YFS_ENOENT == GetSubPoolViewLocked(subpoolkey, &subpoolview));
            subpoolview = new SubPoolView();
            YWB_ASSERT(subpoolview != NULL);
            AddSubPoolView(subpoolkey, subpoolview);
        }
        else
        {
            ret = GetSubPoolViewLocked(subpoolkey, &subpoolview);
            YWB_ASSERT(YWB_SUCCESS == ret);
            YWB_ASSERT(NULL != subpoolview);
        }

        diskview = new DiskView(diskprop);
        YWB_ASSERT(NULL != diskview);
        ret = subpoolview->AddDiskView(diskkey, diskview);
        YWB_ASSERT(YWB_SUCCESS == ret);
    }
    else
    {
        YWB_ASSERT(YWB_SUCCESS == GetDiskViewLocked(diskkey, &diskview));
        ret = YFS_EEXIST;
    }

    mLock.unlock();
    return ret;
}

ywb_status_t ConfigView::GetDiskView(DiskKey& diskkey, DiskView** diskview)
{
    ywb_status_t ret = YWB_SUCCESS;

    mLock.rdlock();
    ret = GetDiskViewLocked(diskkey, diskview);
    mLock.unlock();

    return ret;
}

ywb_status_t ConfigView::GetDiskViewLocked(DiskKey& diskkey, DiskView** diskview)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    std::map<ywb_uint64_t, DiskKey>::iterator iter;
    std::map<ywb_uint32_t, SubPoolKey>::iterator iter2;
    SubPoolView* subpoolview = NULL;

    iter = mDiskID2Key.find(diskkey.GetDiskId());
    if(mDiskID2Key.end() != iter)
    {
        iter2 = mSubPoolID2Key.find(diskkey.GetSubPoolId());
        YWB_ASSERT(mSubPoolID2Key.end() != iter2);
        subpoolkey = iter2->second;
        ret = GetSubPoolViewLocked(subpoolkey, &subpoolview);
        YWB_ASSERT(YWB_SUCCESS == ret);
        YWB_ASSERT(subpoolview != NULL);
        ret = subpoolview->GetDiskView(diskkey, diskview);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ConfigView::RemoveDiskView(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    std::map<ywb_uint64_t, DiskKey>::iterator iter;
    std::map<ywb_uint32_t, SubPoolKey>::iterator iter2;
    SubPoolView* subpoolview = NULL;

    mLock.wrlock();
    iter = mDiskID2Key.find(diskkey.GetDiskId());
    if(mDiskID2Key.end() != iter)
    {
        iter2 = mSubPoolID2Key.find(diskkey.GetSubPoolId());
        if(mSubPoolID2Key.end() != iter2)
        {
            subpoolkey = iter2->second;
            ret = GetSubPoolViewLocked(subpoolkey, &subpoolview);
            if((YWB_SUCCESS == ret) && (subpoolview != NULL))
            {
                ret = subpoolview->RemoveDiskView(diskkey);
            }

            if((YWB_SUCCESS == ret) && (0 == subpoolview->GetDiskCnt()))
            {
                mSubPoolID2Key.erase(iter2);
                RemoveSubPoolView(subpoolkey);
            }

            if(YWB_SUCCESS == ret || YFS_ENOENT == ret)
            {
                mDiskID2Key.erase(iter);
            }
        }
    }
    else
    {
        ret = YFS_ENOENT;
    }

    mLock.unlock();
    return ret;
}

ywb_status_t ConfigView::AddOBJView(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolKey subpoolkey;
    std::map<ywb_uint64_t, DiskKey>::iterator iter;
    std::map<ywb_uint32_t, SubPoolKey>::iterator iter2;
    SubPoolView* subpoolview = NULL;
    OBJView* objinfo = NULL;

    /*
     * NOTE:
     * lock is not necessary for mDiskID2Key/mSubPoolID2Key/mSubPools
     * will only be updated by HUB thread itself(but not here), and
     * this routine will only be called by HUB thread itself too
     *
     * if above mentioned comments are not met any more, then attention
     * MUST be paid here
     * */
    iter = mDiskID2Key.find(objkey.GetStorageId());
    if(mDiskID2Key.end() != iter)
    {
        diskkey = iter->second;
        iter2 = mSubPoolID2Key.find(diskkey.GetSubPoolId());
        YWB_ASSERT(mSubPoolID2Key.end() != iter2);
        subpoolkey = iter2->second;
        ret = GetSubPoolViewLocked(subpoolkey, &subpoolview);
        YWB_ASSERT(YWB_SUCCESS == ret);
        YWB_ASSERT(subpoolview != NULL);
        objinfo = new OBJView();
        YWB_ASSERT(objinfo != NULL);
        ret = subpoolview->AddOBJView(diskkey, objkey, objinfo);
        if(YWB_SUCCESS != ret)
        {
            delete objinfo;
            objinfo = NULL;
//            ret = YWB_SUCCESS;
        }
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t ConfigView::GetOBJView(OBJKey& objkey, OBJView** objinfo)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolKey subpoolkey;
    std::map<ywb_uint64_t, DiskKey>::iterator iter;
    std::map<ywb_uint32_t, SubPoolKey>::iterator iter2;
    SubPoolView* subpoolview = NULL;

    /*lock is not necessary(pls refer to comment in ConfigView::AddOBJView)*/
    iter = mDiskID2Key.find(objkey.GetStorageId());
    if(mDiskID2Key.end() != iter)
    {
        diskkey = iter->second;
        iter2 = mSubPoolID2Key.find(diskkey.GetSubPoolId());
        YWB_ASSERT(mSubPoolID2Key.end() != iter2);
        subpoolkey = iter2->second;
        ret = GetSubPoolViewLocked(subpoolkey, &subpoolview);
        YWB_ASSERT(YWB_SUCCESS == ret);
        YWB_ASSERT(subpoolview != NULL);
        ret = subpoolview->GetOBJView(diskkey, objkey, objinfo);
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t ConfigView::DeleteOBJView(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    SubPoolKey subpoolkey;
    std::map<ywb_uint64_t, DiskKey>::iterator iter;
    std::map<ywb_uint32_t, SubPoolKey>::iterator iter2;
    SubPoolView* subpoolview = NULL;

    /*lock is not necessary(pls refer to comment in ConfigView::AddOBJView)*/
    iter = mDiskID2Key.find(objkey.GetStorageId());
    if(mDiskID2Key.end() != iter)
    {
        diskkey = iter->second;
        iter2 = mSubPoolID2Key.find(diskkey.GetSubPoolId());
        if((mSubPoolID2Key.end() != iter2))
        {
            subpoolkey = iter2->second;
            ret = GetSubPoolViewLocked(subpoolkey, &subpoolview);
            if((YWB_SUCCESS == ret) && (subpoolview != NULL))
            {
                ret = subpoolview->DeleteOBJView(diskkey, objkey);
            }
        }
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t ConfigView::GetFirstSubPoolView(
        SubPoolView** subpoolview, SubPoolKey* subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    /*lock is not necessary(pls refer to comment in ConfigView::AddOBJView)*/
    mIt = mSubPools.begin();
    if(mIt != mSubPools.end())
    {
        *subpoolkey = mIt->first;
        *subpoolview = mIt->second;
        mIt++;
    }
    else
    {
        *subpoolview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ConfigView::GetNextSubPoolView(
        SubPoolView** subpoolview, SubPoolKey* subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    /*lock is not necessary(pls refer to comment in ConfigView::AddOBJView)*/
    if(mSubPools.end() != mIt)
    {
        *subpoolkey = mIt->first;
        *subpoolview = mIt->second;
        mIt++;
    }
    else
    {
        *subpoolview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ConfigView::GetFirstDiskView(
        DiskView** diskview, DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolView* subpoolview = NULL;

    /*lock is not necessary(pls refer to comment in ConfigView::AddOBJView)*/
    mIt = mSubPools.begin();
    while(mIt != mSubPools.end())
    {
        subpoolview = mIt->second;
        YWB_ASSERT(subpoolview != NULL);
        ret = subpoolview->GetFirstDiskView(diskview, diskkey);
        if(YWB_SUCCESS == ret)
        {
            break;
        }
        else
        {
            mIt++;
        }
    }

    if(mIt == mSubPools.end())
    {
        *diskview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ConfigView::GetNextDiskView(
        DiskView** diskview, DiskKey* diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolView* subpoolview = NULL;

    /*lock is not necessary(pls refer to comment in ConfigView::AddOBJView)*/
    if(mSubPools.end() != mIt)
    {
        subpoolview = mIt->second;
        ret = subpoolview->GetNextDiskView(diskview, diskkey);
        if(YWB_SUCCESS != ret)
        {
            mIt++;
            while(mIt != mSubPools.end())
            {
                subpoolview = mIt->second;
                YWB_ASSERT(subpoolview != NULL);
                ret = subpoolview->GetFirstDiskView(diskview, diskkey);
                if(YWB_SUCCESS == ret)
                {
                    break;
                }
                else
                {
                    mIt++;
                }
            }

            if(mSubPools.end() == mIt)
            {
                *diskview = NULL;
                ret = YFS_ENOENT;
            }
        }
    }
    else
    {
        *diskview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ConfigView::GetFirstOBJView(OBJView** objview, OBJKey* objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    SubPoolView* subpoolview = NULL;
    ConfigView* configview = NULL;

    /*lock is not necessary(pls refer to comment in ConfigView::AddOBJView)*/
    mHUB->GetConfigView(&configview);
    YWB_ASSERT(this == configview);
    mIt = mSubPools.begin();
    while(mIt != mSubPools.end())
    {
        subpoolkey = mIt->first;
        if(mHUB->ShouldSubPoolBeInvolved(subpoolkey))
        {
            subpoolview = mIt->second;
            YWB_ASSERT(subpoolview != NULL);
            ret = subpoolview->GetFirstOBJView(objview, objkey);
            if(YWB_SUCCESS == ret)
            {
                break;
            }
            else
            {
                mIt++;
            }
        }
        else
        {
            mIt++;
        }
    }

    if(mIt == mSubPools.end())
    {
        *objview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ConfigView::GetNextOBJView(OBJView** objview, OBJKey* objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    SubPoolView* subpoolview = NULL;
    ConfigView* configview = NULL;

    /*lock is not necessary(pls refer to comment in ConfigView::AddOBJView)*/
    mHUB->GetConfigView(&configview);
    YWB_ASSERT(this == configview);
    if(mSubPools.end() != mIt)
    {
        subpoolview = mIt->second;
        ret = subpoolview->GetNextOBJView(objview, objkey);
        if(YWB_SUCCESS != ret)
        {
            mIt++;
            while(mIt != mSubPools.end())
            {
                subpoolkey = mIt->first;
                if(mHUB->ShouldSubPoolBeInvolved(subpoolkey))
                {
                    subpoolview = mIt->second;
                    YWB_ASSERT(subpoolview != NULL);
                    ret = subpoolview->GetFirstOBJView(objview, objkey);
                    if(YWB_SUCCESS == ret)
                    {
                        break;
                    }
                    else
                    {
                        mIt++;
                    }
                }
                else
                {
                    mIt++;
                }
            }

            if(mSubPools.end() == mIt)
            {
                *objview = NULL;
                ret = YFS_ENOENT;
            }
        }
    }
    else
    {
        *objview = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t ConfigView::GetTierWiseDisks(SubPoolKey& subpoolkey,
        ywb_int32_t tiertype, TierWiseDiskCandidates* candidates)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolView* subpoolview = NULL;
    TierView* tierview = NULL;

    YWB_ASSERT(candidates != NULL);
    mLock.rdlock();
    ret = GetSubPoolViewLocked(subpoolkey, &subpoolview);
    if(YWB_SUCCESS == ret)
    {
        ret = subpoolview->GetTierView(tiertype, &tierview);
        if(YWB_SUCCESS == ret)
        {
            ret = tierview->GetTierWiseDiskCandidates(candidates);
        }
    }

    mLock.unlock();
    return ret;
}

ywb_uint32_t ConfigView::GetOBJCnt()
{
    ywb_uint32_t count = 0;
    std::map<SubPoolKey, SubPoolView*, SubPoolKeyCmp>::iterator iter;

    iter = mSubPools.begin();
    while(mSubPools.end() != iter)
    {
        count += iter->second->GetOBJCnt();
        iter++;
    }

    return count;
}

void ConfigView::Reset()
{
    SubPoolView* subpoolview = NULL;

    mIt = mSubPools.begin();
    for(; mIt != mSubPools.end();)
    {
        subpoolview = mIt->second;
        mIt++;
        delete subpoolview;
        subpoolview = NULL;
    }

    mSubPools.clear();
    mDiskID2Key.clear();
    mSubPoolID2Key.clear();
}

/* vim:set ts=4 sw=4 expandtab */
