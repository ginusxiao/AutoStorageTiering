#include "OSSNetwork/MetaCommDispatcher.hpp"
#include "OSS/AST/ASTCommDispatcher.hpp"
#include "OSSCommon/OpCtx.hpp"
#include "AST/ASTHUB.hpp"

ASTCommDispatcher::ASTCommDispatcher(
        CommunicationLayout* cl, const char *name) :
        CommDispatcher(cl, name)
{
    GetProcFactory()->UpdateProceduer(OpCode::EOC_reselect_path,
                                      new ASTMigrateProcedure());
}

ASTCommDispatcher::~ASTCommDispatcher()
{

}

void ASTCommDispatcher::LaunchMigrationCompletedEvent(
        OBJKey objkey, ywb_status_t migrationstatus)
{
    ControlCenter* ctrlcenter = NULL;
    HUB* hub = NULL;

    ctrlcenter = GetLayout()->GetControlCenter();
    hub = ctrlcenter->GetHUB();
    hub->LaunchMigrationCompletedEvent(objkey, migrationstatus);
}

ywb_status_t ASTCommDispatcher::ASTMigrateProcedure::Process(
        void *dispatcher, void *ctx)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    OpCtx* opctx = NULL;
    ASTCommDispatcher* astdispatcher = NULL;

    astdispatcher = (ASTCommDispatcher*)dispatcher;
    /*
     * 1. send migration completion event to HUB
     * */
    opctx = (OpCtx*)ctx;
//    OpCtx::IncRef(opctx);
    objkey.SetInodeId(opctx->mInodeId);
    objkey.SetChunkId(opctx->mChunkId);
    objkey.SetChunkIndex(opctx->mChunkIndex);
    objkey.SetChunkVersion(opctx->mChunkVersion);
    objkey.SetStorageId(opctx->mSlot);
    /*tell HUB the migration result anyway regardless success or not*/
    astdispatcher->LaunchMigrationCompletedEvent(objkey, opctx->mError);
//    OpCtx::DecRef(opctx);

    /*
     * 2. forward it to CommunicationLayout::mMetaDispatcher only
     *    if successfully migrate the OBJ
     * */
    opctx->mDstSID = astdispatcher->GetLayout()->
            GetSidByInodeid(opctx->mInodeId);
    if((INVALID_SERVERID != opctx->mDstSID) &&
            (YWB_SUCCESS == opctx->mError))
    {
        opctx->mDispatcher = astdispatcher->GetLayout()->mMetaDispatcher;
        astdispatcher->GetLayout()->Attatch(opctx);
    }
    else
    {
        ast_log_fatal("Invalid server id");
    }

    return ret;
}


/* vim:set ts=4 sw=4 expandtab */
