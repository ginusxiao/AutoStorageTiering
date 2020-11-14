#include "stdlib.h"
#include "time.h"
#include "AST/ASTConstant.hpp"
#include "OSSCommon/Logger.hpp"
#include "AST/MessageExpress.hpp"
#include "AST/Deprecated/OSSSimulator.hpp"

void
OSSCoordinator::AddOneSubPool(SubPoolKey** subpool)
{
    mNextPoolId = mNextPoolId + 1;
    mNextSubPoolId = mNextSubPoolId + 1;

    mNextSubPoolKey.SetOss(mOssId);
    mNextSubPoolKey.SetPoolId(mNextPoolId);
    mNextSubPoolKey.SetSubPoolId(mNextSubPoolId);

    *subpool = &mNextSubPoolKey;
    mSubPools.push_back(mNextSubPoolKey);
    ast_log_trace("Add one subpool: [" << mOssId
            << ", " << mNextPoolId
            << ", " << mNextSubPoolId << "]");
}

void
OSSCoordinator::AddOneDisk(SubPoolKey& subpoolkey,
        ywb_uint32_t diskclass, DiskInfo** disk)
{
    mNextDiskId = mNextDiskId + 1;
    mNextDiskKey.SetDiskId(mNextDiskId);
    mNextDiskKey.SetSubPoolId(subpoolkey.GetSubPoolId());

    mNextDiskProp.SetDiskClass(diskclass);
    mNextDiskProp.SetDiskRPM(Constant::DEFAULT_RPM);
    mNextDiskProp.SetRaidType(DiskBaseProp::DRT_raid0);
    mNextDiskProp.SetRaidWidth(Constant::DEFAULT_RAID_WIDTH);
    mNextDiskProp.SetDiskCap(Constant::DEFAULT_DISK_CAP);

    mDiskInfo.SetDiskKey(mNextDiskKey);
    mDiskInfo.SetBaseProp(mNextDiskProp);

    *disk = &mDiskInfo;
    mDisks.push_back(mDiskInfo);
    ast_log_trace("Add one disk: [" << subpoolkey.GetSubPoolId()
            << ", " << mNextDiskId << "]");
}

void
OSSCoordinator::AddOneOBJ(DiskKey& diskkey, OBJInfo** obj, ywb_uint32_t seed)
{
    ywb_uint32_t rndnumread = 0;
    ywb_uint32_t rndnumwrite = 0;

    ywb_uint32_t seqnumread = 0;
    ywb_uint32_t seqnumwrite = 0;

    mNextChunkIndex = mNextChunkIndex + 1;
    mNextInodeId = mNextInodeId + 1;
    mNextOBJKey.SetChunkIndex(mNextChunkIndex);
    mNextOBJKey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    mNextOBJKey.SetChunkVersion(Constant::DEFAULT_VERSION);
    mNextOBJKey.SetInodeId(mNextInodeId);
    mNextOBJKey.SetStorageId(diskkey.GetDiskId());

    srand(seed);
    rndnumread = (rand() % Constant::RAND_NUM_MOD);
    mNextChunkIOStat.SetRndReadIOs(rndnumread);
    mNextChunkIOStat.SetRndReadBW(rndnumread * (Constant::DEFAULT_IOSIZE));

    rndnumwrite = (rand() % Constant::RAND_NUM_MOD);
    mNextChunkIOStat.SetRndWriteIOs(rndnumwrite);
    mNextChunkIOStat.SetRndWriteBW(rndnumwrite * (Constant::DEFAULT_IOSIZE));

    mNextChunkIOStat.SetRndTotalRT((rndnumread * (Constant::META_RT_READ)) +
            (rndnumwrite * (Constant::META_RT_WRITE)));

    seqnumread = (rand() % Constant::RAND_NUM_MOD);
    mNextChunkIOStat.SetSeqReadIOs(seqnumread);
    mNextChunkIOStat.SetSeqReadBW(seqnumread * (Constant::DEFAULT_IOSIZE));

    seqnumwrite = (rand() % Constant::RAND_NUM_MOD);
    mNextChunkIOStat.SetSeqWriteIOs(seqnumwrite);
    mNextChunkIOStat.SetSeqWriteBW(seqnumwrite * (Constant::DEFAULT_IOSIZE));

    mNextChunkIOStat.SetSeqTotalRT((seqnumread * (Constant::META_RT_READ)) +
            (seqnumwrite * (Constant::META_RT_WRITE)));

    mNextOBJInfo.SetOBJKey(mNextOBJKey);
    mNextOBJInfo.SetChunkIOStat(mNextChunkIOStat);

    *obj = &mNextOBJInfo;
    mOBJs.push_back(mNextOBJInfo);
    ast_log_trace("Add one OBJ: [" << diskkey.GetDiskId()
            << ", " << mNextInodeId
            << ", " << Constant::DEFAULT_CHUNK_ID
            << ", " << mNextChunkIndex
            << ", " << Constant::DEFAULT_VERSION
            << "]");
}

void
OSSCoordinator::AddBatchOfOBJ(DiskKey& diskkey, ywb_uint32_t batchnum)
{
    ywb_uint32_t index = 0;
    ywb_uint32_t seed = 0;
    OBJInfo* objinfo = NULL;

    seed = ((unsigned)time(NULL)) % Constant::RAND_SEED_MOD;
    for(; index < batchnum; index++)
    {
        AddOneOBJ(diskkey, &objinfo,
                seed + (Constant::RAND_SEED_INCREMENTAL) * index);
    }
}

void
OSSCoordinator::AddOneThreeTierSubPoolWithDiskAndOBJ(SubPoolKey** subpool)
{
    SubPoolKey* subpoolkey = NULL;
    DiskInfo* diskinfo = NULL;
    DiskKey* diskkey = NULL;

    AddOneSubPool(&subpoolkey);

    AddOneDisk(*subpoolkey, DiskBaseProp::DCT_ssd, &diskinfo);
    diskinfo->GetDiskKey(&diskkey);
    AddBatchOfOBJ(*diskkey, Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR);

    AddOneDisk(*subpoolkey, DiskBaseProp::DCT_ent, &diskinfo);
    diskinfo->GetDiskKey(&diskkey);
    AddBatchOfOBJ(*diskkey, Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR);

    AddOneDisk(*subpoolkey, DiskBaseProp::DCT_sata, &diskinfo);
    diskinfo->GetDiskKey(&diskkey);
    AddBatchOfOBJ(*diskkey, Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR);

    *subpool = subpoolkey;
}

/*delete the last subpool*/
ywb_status_t
OSSCoordinator::DeleteOneSubPool(SubPoolKey* subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskInfo disk;
    DiskKey* diskkey = NULL;
    OBJInfo obj;
    OBJKey* objkey = NULL;
    ywb_uint32_t deleteddisknum = 0;
    ywb_uint32_t deletedobjnum = 0;
    ostringstream ostr;

    if(!mSubPools.empty())
    {
        *subpoolkey = mSubPools.front();
        mSubPools.pop_front();
        ostr << "Delete one subpool: [" << subpoolkey->GetOss()
                << ", " << subpoolkey->GetPoolId()
                << ", " << subpoolkey->GetSubPoolId()
                << "], and its corresponding disks and OBJs\n";

        /*
         * In current implementation, one disk will be deleted before
         * deleting one subpool, and the deleted subpool owns the disk
         * previously deleted
         **/
        deleteddisknum = 1;
        while(!mDisks.empty() &&
                deleteddisknum < Constant::DEFAULT_DISK_NUM_PER_SUBPOOL)
        {
            disk = mDisks.front();
            disk.GetDiskKey(&diskkey);
            YWB_ASSERT(diskkey->GetSubPoolId() == subpoolkey->GetSubPoolId());
            mDisks.pop_front();
            deleteddisknum++;

            ostr << "Delete one disk: [" << diskkey->GetSubPoolId()
                    << ", " << diskkey->GetDiskId()
                    << "], and its corresponding OBJs\n";

            /*
             * Also delete OBJs corresponding to disk
             *
             * We are sure there are Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR
             * remaining OBJs corresponding to this disk to be deleted
             **/
            deletedobjnum = 0;
            while(!mOBJs.empty() &&
                    deletedobjnum < Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR)
            {
                obj = mOBJs.front();
                obj.GetOBJKey(&objkey);
                YWB_ASSERT(objkey->GetStorageId() == diskkey->GetDiskId());
                mOBJs.pop_front();
                deletedobjnum++;

                ostr << "Delete one OBJ: [" << objkey->GetStorageId()
                        << ", " << objkey->GetInodeId()
                        << ", " << objkey->GetChunkId()
                        << ", " << objkey->GetChunkIndex()
                        << ", " << objkey->GetChunkVersion()
                        << "]\n";
            }
        }

        ast_log_debug(ostr.str());
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

/*delete the last disk*/
ywb_status_t
OSSCoordinator::DeleteOneDisk(DiskInfo* disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey* diskkey = NULL;
    OBJInfo obj;
    OBJKey* objkey = NULL;
    ywb_uint32_t deletedobjnum = 0;
    ostringstream ostr;

    if(!mDisks.empty())
    {
        *disk = mDisks.front();
        mDisks.pop_front();

        disk->GetDiskKey(&diskkey);
        ostr << "Delete one disk: [" << diskkey->GetSubPoolId()
                << ", " << diskkey->GetDiskId()
                << "], and its corresponding OBJs\n";

        /*
         * Also delete OBJs corresponding to disk
         *
         * In current implementation, one OBJ will be deleted before
         * deleting one disk, and the deleted disk owns the previously
         * deleted OBJ
         **/
        deletedobjnum = 1;
        while(!mOBJs.empty() &&
                deletedobjnum < Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR)
        {
            obj = mOBJs.front();
            obj.GetOBJKey(&objkey);
            YWB_ASSERT(objkey->GetStorageId() == diskkey->GetDiskId());
            mOBJs.pop_front();
            deletedobjnum++;

            ostr << "Delete one OBJ: [" << objkey->GetStorageId()
                    << ", " << objkey->GetInodeId()
                    << ", " << objkey->GetChunkId()
                    << ", " << objkey->GetChunkIndex()
                    << ", " << objkey->GetChunkVersion()
                    << "]\n";
        }
        ast_log_debug(ostr.str());
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

/*delete the last OBJ*/
ywb_status_t
OSSCoordinator::DeleteOneOBJ(OBJInfo* obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey* objkey = NULL;

    if(!mOBJs.empty())
    {
        *obj = mOBJs.front();
        mOBJs.pop_front();

        obj->GetOBJKey(&objkey);
        ast_log_trace("Delete one OBJ: [" << objkey->GetStorageId()
                << ", " << objkey->GetInodeId()
                << ", " << objkey->GetChunkId()
                << ", " << objkey->GetChunkIndex()
                << ", " << objkey->GetChunkVersion()
                << "]");
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
OSSCoordinator::GetLastSubPool(SubPoolKey* subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mSubPools.empty())
    {
        *subpoolkey = mSubPools.back();
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
OSSCoordinator::GetLastDisk(DiskInfo* diskinfo)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(!mDisks.empty())
    {
        *diskinfo = mDisks.back();
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
OSSCoordinator::GetFirstDiskFromBack(DiskInfo* diskinfo)
{
    ywb_status_t ret = YFS_ENOENT;

    mDiskRIter = mDisks.rbegin();
    if(mDiskRIter != mDisks.rend())
    {
        ret = YWB_SUCCESS;
        *diskinfo = *mDiskRIter;
        mDiskRIter++;
    }

    return ret;
}

ywb_status_t
OSSCoordinator::GetNextDiskFromBack(DiskInfo* diskinfo)
{
    ywb_status_t ret = YFS_ENOENT;

    if(mDiskRIter != mDisks.rend())
    {
        ret = YWB_SUCCESS;
        *diskinfo = *mDiskRIter;
        mDiskRIter++;
    }

    return ret;
}

bool OSSCoordinator::IsSubPoolEmpty()
{
    return mSubPools.empty();
}

ywb_status_t
OSSCoordinator::FillCollectConfigResp(CollectLogicalConfigResponse* resp)
{
    ywb_status_t ret = YWB_SUCCESS;
    list<SubPoolKey>::iterator subpooliter;
    list<DiskInfo>::iterator diskiter;

    subpooliter = mSubPools.begin();
    for(; subpooliter != mSubPools.end(); subpooliter++)
    {
        resp->AddSubPool(*subpooliter);
    }

    diskiter = mDisks.begin();
    for(; diskiter != mDisks.end(); diskiter++)
    {
        resp->AddDisk(*diskiter);
    }

    return ret;
}

ywb_status_t
OSSCoordinator::FillCollectOBJResp(CollectOBJResponse* resp)
{
    ywb_status_t ret = YWB_SUCCESS;
    list<OBJInfo>::iterator objiter;

    objiter = mOBJs.begin();
    for(; objiter != mOBJs.end(); objiter++)
    {
        resp->AddOBJ(*objiter);
    }

    return ret;
}

ywb_status_t
OSSCoordinator::Initialize()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolnum = 0;
    SubPoolKey* subpoolkey = NULL;

    ast_log_trace("Create " << Constant::DEFAULT_SUBPOOL_NUM
            << " subpools with disk and OBJs");
    while(subpoolnum < Constant::DEFAULT_SUBPOOL_NUM)
    {
        AddOneThreeTierSubPoolWithDiskAndOBJ(&subpoolkey);
        subpoolnum++;
    }

    return ret;
}

OSSMonClientMock::OSSMonClientMock(OSSSimulator* osssim,
        OSSControlCenterMock* osscontrolcenter) :
        StorageServiceMonClient(osscontrolcenter),
        mOSSSim(osssim), mSeqno(0)
{
    mOSSCenter = osscontrolcenter;
    mCollectConfigResp = new CollectLogicalConfigResponse();
    YWB_ASSERT(mCollectConfigResp != NULL);
}

OSSMonClientMock::~OSSMonClientMock()
{
    if(mCollectConfigResp)
    {
        mCollectConfigResp->DecRef();
    }

    mOSSSim = NULL;
    mOSSCenter = NULL;
}

//ywb_status_t
//OSSMonClientMock::HandleMonitorMessage(yfs_sid_t sid,
//        ywb_uint64_t connid, NMsgblock* msg)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//
//    return ret;
//}
//
//void
//OSSMonClientMock::HandleDropped(int type,
//        ywb_uint32_t sid, ywb_uint64_t connid)
//{
//    return;
//}

ywb_status_t
OSSMonClientMock::Init()
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

//ywb_uint64_t
//OSSMonClientMock::AllocConnId()
//{
//    return 0;
//}

ywb_status_t
OSSMonClientMock::initialize()
{
    return YWB_SUCCESS;
}

ywb_status_t
OSSMonClientMock::RetrieveLogicalConfig(
        ywb_uint32_t parentmsgno, ywb_uint32_t ossid)
{
    ywb_status_t ret = YWB_SUCCESS;

    mSeqno = parentmsgno;
    mOSSSim->DynamicalConfig();
    ReceiveLogicalConfig(NULL);

    return ret;
}

ywb_status_t
OSSMonClientMock::ReceiveLogicalConfig(NMsgblock* message)
{
    ywb_status_t ret = YWB_SUCCESS;
    OSSCoordinator* osscoordinator = NULL;

    mOSSSim->GetOSSCoordinator(&osscoordinator);
    /*firstly clear the original configuration*/
    mCollectConfigResp->Reset();
    /*fill current configuration into @mCollectConfigResp*/
    osscoordinator->FillCollectConfigResp(mCollectConfigResp);
    mCollectConfigResp->SetSeqNo(mSeqno);
    mOSSSim->Send(mCollectConfigResp);

    return ret;
}

OSSControlCenterMock::OSSControlCenterMock(OSSSimulator* osssim) :
        mOSSSim(osssim), mCollectConfigSeqno(0), mCollectIOSeqno(0)
{
    YWB_ASSERT(osssim != NULL);
    mMonClntMock = new OSSMonClientMock(osssim, this);
    YWB_ASSERT(mMonClntMock != NULL);
    SetMonClient(mMonClntMock);
    mMonClntMock->Create();
    mCollectConfigResp = new CollectLogicalConfigResponse();
    YWB_ASSERT(mCollectConfigResp != NULL);
    mCollectOBJResp = new CollectOBJResponse();
    YWB_ASSERT(mCollectOBJResp != NULL);
}

OSSControlCenterMock::~OSSControlCenterMock()
{
    /*remember not to delete @mMonClntMock here*/
//    if(mMonClntMock != NULL)
//    {
//        delete mMonClntMock;
//        mMonClntMock = NULL;
//    }
    if(mCollectConfigResp)
    {
        ast_log_debug("mCollectConfigResp decrease reference");
        mCollectConfigResp->DecRef();
    }

    if(mCollectOBJResp)
    {
        ast_log_debug("mCollectOBJResp decrease reference");
        mCollectOBJResp->DecRef();
    }

    mOSSSim = NULL;
}

ywb_status_t
OSSControlCenterMock::RetrieveLogicalConfig(
        ywb_uint32_t parentmsgno, ywb_uint32_t ossid)
{
    ywb_status_t ret = YWB_SUCCESS;
    OSSCoordinator* osscoordinator = NULL;

    mCollectConfigSeqno = parentmsgno;
    mOSSSim->DynamicalConfig();
    mOSSSim->GetOSSCoordinator(&osscoordinator);
    /*firstly clear the original configuration*/
    mCollectConfigResp->Reset();
    /*fill current configuration into @mCollectConfigResp*/
    osscoordinator->FillCollectConfigResp(mCollectConfigResp);
    mCollectConfigResp->SetSeqNo(mCollectConfigSeqno);
    mOSSSim->Send(mCollectConfigResp);

    return ret;
}

ywb_status_t
OSSControlCenterMock::RetrieveOBJ(
        ywb_uint32_t parentmsgno, ywb_uint32_t ossid)
{
    ywb_status_t ret = YWB_SUCCESS;
    OSSCoordinator* osscoordinator = NULL;

    mCollectIOSeqno = parentmsgno;
    mOSSSim->GetOSSCoordinator(&osscoordinator);
    /*firstly clear the original configuration*/
    mCollectOBJResp->Reset();
    /*fill current configuration into @mCollectConfigResp*/
    osscoordinator->FillCollectOBJResp(mCollectOBJResp);
    mCollectOBJResp->SetSeqNo(mCollectIOSeqno);
    ast_log_debug("mCollectOBJResp increase reference");
    mOSSSim->Send(mCollectOBJResp);

    return ret;
}

OSSSimulator::OSSSimulator() : mCurCycle(0)
{
    mOSSCoordinator = new OSSCoordinator();
    YWB_ASSERT(mOSSCoordinator != NULL);
    mOSSCenter = new OSSControlCenterMock(this);
    YWB_ASSERT(mOSSCenter != NULL);

    mAst = GetSingleASTInstance();
    YWB_ASSERT(mAst != NULL);
    mAst->GetMessageManager(&mMsgMgr);
//    mMsgMgr = new MessageManager(mAst, mOSSCenter);
    YWB_ASSERT(mMsgMgr != NULL);
//    mAst->SetMessageManager(mMsgMgr);
    mEnableASTMsg = new Message("ast", Message::OP_enable_ast);
    YWB_ASSERT(mEnableASTMsg != NULL);
    mDisableASTMsg = new Message("ast", Message::OP_disable_ast);
    YWB_ASSERT(mDisableASTMsg != NULL);
}

OSSSimulator::~OSSSimulator()
{
    if(mAst != NULL)
    {
//        delete mAst;
//        mAst = NULL;
        mAst->DecRef();
    }

    if(mEnableASTMsg != NULL)
    {
        delete mEnableASTMsg;
        mEnableASTMsg = NULL;
    }

    if(mDisableASTMsg != NULL)
    {
        delete mDisableASTMsg;
        mDisableASTMsg = NULL;
    }

//    if(mMsgMgr)
//    {
//        delete mMsgMgr;
//        mMsgMgr = NULL;
//    }

    if(mOSSCoordinator != NULL)
    {
        delete mOSSCoordinator;
        mOSSCoordinator = NULL;
    }

    if(mOSSCenter != NULL)
    {
        delete mOSSCenter;
        mOSSCenter = NULL;
    }

    mMsgMgr = NULL;
}

ywb_status_t
OSSSimulator::Initialize()
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = mOSSCoordinator->Initialize();
    return ret;
}

ywb_status_t
OSSSimulator::Start()
{
    ywb_status_t ret = YWB_SUCCESS;

    mAst->SetControlCenter(mOSSCenter);
    /*start message service*/
//    mAst->StartMsgSvc();
    StartMessageService();
    /*enable AST via message service*/
    mMsgMgr->Enqueue(mEnableASTMsg);

    return ret;
}

ywb_status_t
OSSSimulator::Stop()
{
    ywb_status_t ret = YWB_SUCCESS;

    mAst->GetMessageManager(&mMsgMgr);
    /*disable AST via message service*/
    mMsgMgr->Enqueue(mDisableASTMsg);
    /*stop message service*/
    StopMessageService();

    return ret;
}

void
OSSSimulator::StartMessageService()
{
    YWB_ASSERT(mMsgMgr != NULL);
    mMsgMgr->StartMsgSvc();
}

void
OSSSimulator::StopMessageService()
{
    if(mMsgMgr != NULL)
    {
        mMsgMgr->StopMsgSvc();
    }
}

ywb_status_t
OSSSimulator::DynamicalConfig()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t subpoolnum = 0;
    OSSCoordinator* coordinator = NULL;
    SubPoolKey* subpoolkeyp = NULL;
    DiskInfo* diskinfop = NULL;
    DiskKey* diskkeyp = NULL;
    SubPoolKey subpoolkey;
    DiskInfo diskinfo;
    DiskKey diskkey;
    OBJInfo objinfo;
    DiskInfo diskinfos[Constant::DEFAULT_DISK_NUM_PER_SUBPOOL];
    ywb_uint32_t phase = 0;
    ywb_uint32_t disknum = 0;

    mCurCycle++;
    ast_log_trace("Current cycle: " << mCurCycle);
    GetOSSCoordinator(&coordinator);
    if(coordinator->IsSubPoolEmpty())
    {
        ast_log_trace("Create " << Constant::DEFAULT_SUBPOOL_NUM
                << " subpools with disk and OBJs");
        while(subpoolnum < Constant::DEFAULT_SUBPOOL_NUM)
        {
            coordinator->AddOneThreeTierSubPoolWithDiskAndOBJ(&subpoolkeyp);
            subpoolnum++;
        }
    }
    else
    {
//        phase = (srand((unsigned)time(NULL))) % OSSCoordinator::OP_cnt;
        phase = mCurCycle % OSSCoordinator::OP_cnt;
        switch(phase)
        {
        case OSSCoordinator::OP_none:
            /*
             * Nothing to do at current, but it is necessary for
             * cycle count starts from 1
             **/
            break;
        case OSSCoordinator::OP_delete_obj:
            coordinator->DeleteOneOBJ(&objinfo);
            break;
        case OSSCoordinator::OP_delete_disk:
            coordinator->DeleteOneDisk(&diskinfo);
            break;
        case OSSCoordinator::OP_delete_subpool:
            coordinator->DeleteOneSubPool(&subpoolkey);
            break;
        case OSSCoordinator::OP_add_subpool:
//            /*add subpool with disk and OBJs*/
//            coordinator->AddOneThreeTierSubPoolWithDiskAndOBJ(&subpoolkey);
            coordinator->AddOneSubPool(&subpoolkeyp);
            break;
        case OSSCoordinator::OP_add_disk:
//            /*add disk(with OBJs) only to the last subpool*/
            coordinator->GetLastSubPool(&subpoolkey);

            coordinator->AddOneDisk(subpoolkey,
                    DiskBaseProp::DCT_ssd, &diskinfop);
//            diskinfo->GetDiskKey(&diskkey);
//            coordinator->AddBatchOfOBJ(*diskkey,
//                    Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR);

            coordinator->AddOneDisk(subpoolkey,
                    DiskBaseProp::DCT_ent, &diskinfop);
//            diskinfo->GetDiskKey(&diskkey);
//            coordinator->AddBatchOfOBJ(*diskkey,
//                    Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR);

            coordinator->AddOneDisk(subpoolkey,
                    DiskBaseProp::DCT_sata, &diskinfop);
//            diskinfo->GetDiskKey(&diskkey);
//            coordinator->AddBatchOfOBJ(*diskkey,
//                    Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR);

            break;
        case OSSCoordinator::OP_add_obj:
//            /*add OBJs only to the last disk*/
//            coordinator->GetLastDisk(&diskinfo2);
//            diskinfo2.GetDiskKey(&diskkey);
//            coordinator->AddBatchOfOBJ(*diskkey,
//                    Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR);
            /*
             * Add OBJs into the last Constant::DEFAULT_DISK_NUM_PER_SUBPOOL
             * disks
             **/
            ret = coordinator->GetFirstDiskFromBack(&(diskinfos[disknum++]));
            while((YWB_SUCCESS == ret) &&
                    (disknum < Constant::DEFAULT_DISK_NUM_PER_SUBPOOL))
            {
                ret = coordinator->GetNextDiskFromBack(&(diskinfos[disknum++]));
            }

            for(disknum = Constant::DEFAULT_DISK_NUM_PER_SUBPOOL;
                    disknum > 0; disknum--)
            {
                diskinfos[disknum - 1].GetDiskKey(&diskkeyp);
                coordinator->AddBatchOfOBJ(*diskkeyp,
                        Constant::DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR);
            }

            break;
        }
    }

    return ret;
}

void OSSSimulator::Send(Message* msg)
{
    YWB_ASSERT(msg != NULL);
    mMsgMgr->Enqueue(msg);
}

/* vim:set ts=4 sw=4 expandtab */
