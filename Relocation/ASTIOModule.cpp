#include "FsCommon/YfsTime.h"
#include "OSSCommon/Logger.hpp"
#include "Storage/ChunkFileHandler.hpp"
#include "Storage/IOParam.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIOModule.hpp"

ywb_status_t ASTIOModule::Initialize(StorageManager *sm,
        IOModule *iom, int workercnt)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    DiskBaseProp diskprop;
    IOModule *lastiom = NULL;

    io_log_debug("Initialize ASTIOModule " << mName);
    if (iom != NULL) {
        lastiom = iom->GetLastIOModule();
    }

    mStorageManager = sm;
    mPrevIOModule = lastiom;

    if (lastiom != NULL) {
        YWB_ASSERT(lastiom->GetNextIOModule() == NULL);
        lastiom->SetNextIOModule(this);
    }

    diskkey.SetSubPoolId(mStorageManager->GetPoolId());
    diskkey.SetDiskId(mStorageManager->ID());
    /*
     * FIXME:
     * how to get disk base prop???
     * */

    /*following is just a temporary solution*/
    if("/mnt/data1/" == sm->GetPath())
    {
        diskprop.SetDiskClass(DiskBaseProp::DCT_ssd);
        diskprop.SetDiskRPM(15000);
        diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
        diskprop.SetRaidWidth(1);
        /*
         * set usable space to 200G, but in AST integration test, each OBJ
         * only occupy 1MB, so can accomodate 204800 OBJs
         * */
        diskprop.SetDiskCap(204800);
    }
    else if("/mnt/data2/" == sm->GetPath())
    {
        diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
        diskprop.SetDiskRPM(10000);
        diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
        diskprop.SetRaidWidth(1);
        /*
         * set usable space to 200G, but in AST integration test, each OBJ
         * only occupy 1MB, so can accomodate 204800 OBJs
         * */
        diskprop.SetDiskCap(204800);
    }

    GetHUB()->LaunchAddDiskEvent(diskkey, diskprop);

    return ret;
}

ywb_status_t ASTIOModule::Finalize()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;

    io_log_debug("Finalize ASTIOModule " << mName);
    if (mNextIOModule != NULL) {
        delete mNextIOModule;
        mNextIOModule = NULL;
    }

    if (mPrevIOModule != NULL) {
        mPrevIOModule->SetNextIOModule(NULL);
        mPrevIOModule = NULL;
    }

    diskkey.SetSubPoolId(mStorageManager->GetPoolId());
    diskkey.SetDiskId(mStorageManager->ID());
    /*
     * if ControlCenter is set to Stopping, then do not
     * launch remove disk event any more
     **/
    if(GetControlCenter()->IsRunning())
    {
        GetHUB()->LaunchRemoveDiskEvent(diskkey);
    }

    return ret;
}

ywb_status_t
ASTIOModule::Create(ChunkFileHandler *cfh, IOParam *param)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey(cfh->mChunk->GetChunkId(),
            cfh->mChunk->GetInodeId(),
            cfh->mChunk->GetChunkIndex(),
            cfh->mChunk->GetChunkVersion(),
            mStorageManager->ID());

    ret = GetNextIOModule()->Create(cfh, param);
    GetHUB()->LaunchAddOBJEvent(objkey);

    return ret;
}

ywb_status_t
ASTIOModule::Write(ChunkFileHandler *cfh, IOParam *param)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t elapsed = 0;
    ywb_time_t begintime;
    ywb_time_t endtime;
    OBJKey objkey(cfh->mChunk->GetChunkId(),
            cfh->mChunk->GetInodeId(),
            cfh->mChunk->GetChunkIndex(),
            cfh->mChunk->GetChunkVersion(),
            mStorageManager->ID());
    OBJKey tgtobj;

    if(!(GetHUB()->ShouldRedirect(objkey, tgtobj)))
    {
        GetHUB()->MarkOBJAsSettledIfNeccessary(objkey);
        begintime = ywb_time_now();
        ret = GetNextIOModule()->Write(cfh, param);
        endtime = ywb_time_now();
        /*in measure of usec*/
        elapsed = endtime - begintime;
        GetHUB()->LaunchAccessOBJEvent(objkey, param->mOffset, param->mUnion.mSize,
                1, RoundUp2KB(param->mUnion.mSize), elapsed, false);
    }
    else
    {
        /*
         * FIXME: how to ???
         * update @cfh at first, especially ChunkFileHandler::mChunk
         * */
        begintime = ywb_time_now();
        ret = GetNextIOModule()->Write(cfh, param);
        endtime = ywb_time_now();
        /*in measure of usec*/
        elapsed = endtime - begintime;
        GetHUB()->LaunchAccessOBJEvent(tgtobj, param->mOffset, param->mUnion.mSize,
                1, RoundUp2KB(param->mUnion.mSize), elapsed, false);
    }

    return ret;
}

ywb_status_t
ASTIOModule::Read(ChunkFileHandler *cfh, IOParam *param)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t elapsed = 0;
    ywb_time_t begintime;
    ywb_time_t endtime;
    OBJKey objkey(cfh->mChunk->GetChunkId(),
            cfh->mChunk->GetInodeId(),
            cfh->mChunk->GetChunkIndex(),
            cfh->mChunk->GetChunkVersion(),
            mStorageManager->ID());
    OBJKey tgtobj;

    if(!(GetHUB()->ShouldRedirect(objkey, tgtobj)))
    {
        GetHUB()->MarkOBJAsSettledIfNeccessary(objkey);
        begintime = ywb_time_now();
        ret = GetNextIOModule()->Read(cfh, param);
        endtime = ywb_time_now();
        /*in measure of usec*/
        elapsed = endtime - begintime;
        GetHUB()->LaunchAccessOBJEvent(objkey, param->mOffset, param->mUnion.mSize,
                1, RoundUp2KB(param->mUnion.mSize), elapsed, true);
    }
    else
    {
        /*
         * FIXME: how to ???
         * update @cfh at first, especially ChunkFileHandler::mChunk
         * */
        begintime = ywb_time_now();
        ret = GetNextIOModule()->Read(cfh, param);
        endtime = ywb_time_now();
        /*in measure of usec*/
        elapsed = endtime - begintime;
        GetHUB()->LaunchAccessOBJEvent(tgtobj, param->mOffset, param->mUnion.mSize,
                1, RoundUp2KB(param->mUnion.mSize), elapsed, true);
    }

    return ret;
}

ywb_status_t
ASTIOModule::Truncate(ChunkFileHandler *cfh, IOParam *param)
{
    return GetNextIOModule()->Truncate(cfh, param);
}

ywb_status_t
ASTIOModule::Flush(ChunkFileHandler *cfh, IOParam *param)
{
    return GetNextIOModule()->Flush(cfh, param);
}

ywb_status_t
ASTIOModule::Delete(ChunkFileHandler *cfh, IOParam *param)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey(cfh->mChunk->GetChunkId(),
            cfh->mChunk->GetInodeId(),
            cfh->mChunk->GetChunkIndex(),
            cfh->mChunk->GetChunkVersion(),
            mStorageManager->ID());
    OBJKey tgtobj;

    if(!(GetHUB()->ShouldRedirect(objkey, tgtobj)))
    {
        GetHUB()->MarkOBJAsSettledIfNeccessary(objkey);
        ret = GetNextIOModule()->Delete(cfh, param);
        GetHUB()->LaunchDeleteOBJEvent(objkey);
    }
    else
    {
        /*
         * FIXME: how to ???
         * update @cfh at first, especially ChunkFileHandler::mChunk
         * */
        ret = GetNextIOModule()->Delete(cfh, param);
        GetHUB()->LaunchDeleteOBJEvent(tgtobj);
    }

    return ret;
}

ywb_status_t
ASTIOModule::Rename(ChunkFileHandler *cfh, IOParam *param)
{
    return GetNextIOModule()->Rename(cfh, param);
}

ywb_status_t
ASTIOModule::SetDirty(ChunkFileHandler *cfh, IOParam *param)
{
    return GetNextIOModule()->SetDirty(cfh, param);
}

ywb_status_t
ASTIOModule::Replicate(ChunkFileHandler *cfh, IOParam *param)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey(cfh->mChunk->GetChunkId(),
            cfh->mChunk->GetInodeId(),
            cfh->mChunk->GetChunkIndex(),
            cfh->mChunk->GetChunkVersion(),
            mStorageManager->ID());

    ret = GetNextIOModule()->Replicate(cfh, param);
    /*replication caused by snapshot should not be accounted*/
    if((param != NULL) && (0 == param->mSnapshotId))
    {
        GetHUB()->LaunchAddOBJEvent(objkey);
    }

    return ret;
}

ywb_status_t
ASTIOModule::Snapshot(ChunkFileHandler *cfh, IOParam *param)
{
    return GetNextIOModule()->Snapshot(cfh, param);
}

/* vim:set ts=4 sw=4 expandtab */
