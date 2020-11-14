#include "YfsDirectorySetting.hpp"
#include "OSSCommon/Logger.hpp"
#include "AST/ASTHUB.hpp"
#include "UnitTest/AST/Message/ASTMessageUnitTests.hpp"

ASTMock* gAst = NULL;
OSSMock* gOss = NULL;

void ASTMessageTestEnvironment::SetUp()
{
    HUB* hub = NULL;
    gAst = new ASTMock();
    YWB_ASSERT(gAst != NULL);
    gOss = new OSSMock();
    YWB_ASSERT(gOss != NULL);

    hub = gOss->GetHUB();
    /*to match with Stop() in ControlCenter::Finalize()*/
    hub->Start();
}

void ASTMessageTestEnvironment::TearDown()
{
    if(gAst)
    {
        delete gAst;
        gAst = NULL;
    }

    if(gOss)
    {
        delete gOss;
        gOss = NULL;
    }
}

ASTMock::ASTMock()
{

}

ASTMock::~ASTMock()
{

}

StorageServiceMonClientMock::StorageServiceMonClientMock(
        OSSMock* oss) : StorageServiceMonClient(oss)
{
    mOss = oss;
}

StorageServiceMonClientMock::~StorageServiceMonClientMock()
{
    mOss = NULL;
}

ywb_status_t
StorageServiceMonClientMock::Init()
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_status_t
StorageServiceMonClientMock::initialize()
{
    return YWB_SUCCESS;
}

OSSMock::OSSMock()
{
    mClient = new StorageServiceMonClientMock(this);
    YWB_ASSERT(mClient != NULL);
    SetMonClient(mClient);
    mClient->Create();
}

OSSMock::~OSSMock()
{
    /*remember not to delete @mMonClntMock here*/
    /*if(mClient)
    {
        delete mClient;
        mClient = NULL;
    }*/
}

ASTMessageManagerTest::ASTMessageManagerTest()
{
//    mAstProxy = new ASTProxy(gAst);
//    YWB_ASSERT(mAstProxy != NULL);
//    mOssProxy = new OSSProxy(gOss);
//    YWB_ASSERT(mOssProxy != NULL);
    mAstMsgMgr = new MessageManager(gAst, gOss);
    YWB_ASSERT(mAstMsgMgr != NULL);
}

ASTMessageManagerTest::~ASTMessageManagerTest()
{
//    if(mAstProxy)
//    {
//        delete mAstProxy;
//        mAstProxy = NULL;
//    }
//
//    if(mOssProxy)
//    {
//        delete mOssProxy;
//        mOssProxy = NULL;
//    }

    if(mAstMsgMgr)
    {
        delete mAstMsgMgr;
        mAstMsgMgr = NULL;
    }
}

void ASTMessageManagerTest::SetUp()
{
//    mAstMsgMgr->Start();
//
//    mAstMsgMgr->RegisterTargetProxy("oss", mOssProxy);
//    mAstMsgMgr->RegisterTargetProxy("ast", mAstProxy);
//
//    mAstMsgMgr->StartTargetProxy("oss");
//    mAstMsgMgr->StartTargetProxy("ast");
    mAstMsgMgr->StartMsgSvc();
}

void ASTMessageManagerTest::TearDown()
{
//    mAstMsgMgr->DeRegisterTargetProxy("oss");
//    mAstMsgMgr->DeRegisterTargetProxy("ast");
//
//    mAstMsgMgr->StopTargetProxy("oss");
//    mAstMsgMgr->StopTargetProxy("ast");
//
//    mAstMsgMgr->Stop();
    mAstMsgMgr->StopMsgSvc();
}

MessageManager*
ASTMessageManagerTest::GetMessageManager()
{
    return mAstMsgMgr;
}

void CollectLogicalConfigResponseTest::SetUp()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    DiskBaseProp diskprop;
    DiskInfo diskinfo;

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);

    for(subpoolloop = 0;
            subpoolloop < 5;
            subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        mConfigResponse->AddSubPool(subpoolkey);

        diskkey.SetSubPoolId(subpoolid);
        for(diskloop = 0;
                diskloop < 3;
                diskloop++, diskid++)
        {
            if(0 == diskloop)
            {
                diskprop.SetDiskClass(DiskBaseProp::DCT_ssd);
                diskprop.SetDiskRPM(15000);
            }
            else if(1 == diskloop)
            {
                diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
                diskprop.SetDiskRPM(10000);
            }
            else if(2 == diskloop)
            {
                diskprop.SetDiskClass(DiskBaseProp::DCT_sata);
                diskprop.SetDiskRPM(7500);
            }

            diskkey.SetDiskId(diskid);
            diskinfo.SetBaseProp(diskprop);
            diskinfo.SetDiskKey(diskkey);
            mConfigResponse->AddDisk(diskinfo);
        }
    }
}

void CollectLogicalConfigResponseTest::TearDown()
{
    mConfigResponse->Reset();
}

CollectLogicalConfigResponse*
CollectLogicalConfigResponseTest::GetConfigResponse()
{
    return mConfigResponse;
}

CollectLogicalConfigResponse*
CollectLogicalConfigResponseTest::GetConfigResponse2()
{
    return mConfigResponse2;
}

void CollectOBJResponseFragmentTest::SetUp()
{
    ywb_uint32_t objloop = 0;
    ywb_uint64_t inodeid = 0;
    ywb_uint32_t factors = 0;
    OBJKey objkey;
    ChunkIOStat rawstat;
    OBJInfo objinfo;

    inodeid = Constant::DEFAULT_INODE;
    objkey.SetStorageId(Constant::DISK_ID);
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

    for(objloop = 0; objloop < 6; objloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        objinfo.SetOBJKey(objkey);
        objinfo.SetChunkIOStat(rawstat);
        mOBJResponseFrag->AddOBJ(objinfo);
    }
}

void CollectOBJResponseFragmentTest::TearDown()
{
    mOBJResponseFrag->Reset();
}

CollectOBJResponseFragment*
CollectOBJResponseFragmentTest::GetOBJResponseFrag()
{
    return mOBJResponseFrag;
}

CollectOBJResponseFragment*
CollectOBJResponseFragmentTest::GetOBJResponseFrag2()
{
    return mOBJResponseFrag2;
}

void ReportDeleteOBJMessageTest::SetUp()
{
    ywb_uint32_t objloop = 0;
    ywb_uint64_t inodeid = 0;
    OBJKey objkey;

    inodeid = Constant::DEFAULT_INODE;
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID);
    objkey.SetChunkIndex(Constant::DEFAULT_CHUNK_INDEX);
    objkey.SetChunkVersion(1);

    for(objloop = 0; objloop < 6; objloop++, inodeid++)
    {
        objkey.SetInodeId(inodeid);
        mOBJDeletionMsg->AddOBJ(objkey);
    }
}

void ReportDeleteOBJMessageTest::TearDown()
{
    mOBJDeletionMsg->Reset();
}

ReportDeleteOBJsMessage*
ReportDeleteOBJMessageTest::GetOBJDeletionMsg()
{
    return mOBJDeletionMsg;
}

ReportDeleteOBJsMessage*
ReportDeleteOBJMessageTest::GetOBJDeletionMsg2()
{
    return mOBJDeletionMsg2;
}

void ReportDeleteDiskMessageTest::SetUp()
{
    ywb_uint32_t diskloop = 0;
    ywb_uint64_t diskid = 0;
    DiskKey diskkey;

    diskid = Constant::DISK_ID;
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    for(diskloop = 0;
            diskloop < 3;
            diskloop++, diskid++)
    {
        diskkey.SetDiskId(diskid);
        mDiskDeletionMsg->AddDisk(diskkey);
    }
}

void ReportDeleteDiskMessageTest::TearDown()
{
    mDiskDeletionMsg->Reset();
}

ReportDeleteDisksMessage*
ReportDeleteDiskMessageTest::GetDiskDeletionMsg()
{
    return mDiskDeletionMsg;
}

ReportDeleteDisksMessage*
ReportDeleteDiskMessageTest::GetDiskDeletionMsg2()
{
    return mDiskDeletionMsg2;
}

void ReportDeleteSubPoolMessageTest::SetUp()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t subpoolid = 0;
    SubPoolKey subpoolkey;

    subpoolid = Constant::SUBPOOL_ID;
    subpoolkey.SetOss(Constant::OSS_ID);
    subpoolkey.SetPoolId(Constant::POOL_ID);

    for(subpoolloop = 0;
            subpoolloop < 5;
            subpoolloop++, subpoolid++)
    {
        subpoolkey.SetSubPoolId(subpoolid);
        mSubPoolDeletionMsg->AddSubPool(subpoolkey);
    }
}

void ReportDeleteSubPoolMessageTest::TearDown()
{
    mSubPoolDeletionMsg->Reset();
}

ReportDeleteSubPoolsMessage*
ReportDeleteSubPoolMessageTest::GetSubPoolDeletionMsg()
{
    return mSubPoolDeletionMsg;
}

ReportDeleteSubPoolsMessage*
ReportDeleteSubPoolMessageTest::GetSubPoolDeletionMsg2()
{
    return mSubPoolDeletionMsg2;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    /*ControlCenter will creater log for us*/
#if 0
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

    create_logger(logfile, "trace");
#endif

    testing::AddGlobalTestEnvironment(new ASTMessageTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
