#include "YfsDirectorySetting.hpp"
#include "AST/ASTLogger.hpp"
#include "UnitTest/AST/HUB/ASTHUBUnitTests.hpp"

ControlCenter* gCenter = NULL;

ywb_status_t
TimeScheduleConfigTest::AddTimeRanges(vector<TimeRange> ranges)
{
    return mConfig.AddTimeRanges(ranges);
}

void
TimeScheduleConfigTest::SetTimeScheduleConfig(TimeScheduleConfig& config)
{
    mConfig = config;
}

ywb_bool_t
TimeScheduleConfigTest::CheckTimeScheduleConfig(
        vector<TimeRange>& timeranges, ywb_uint32_t weekdays)
{
    return mConfig.CheckTimeScheduleConfigEqual(timeranges, weekdays);
}

void DiskViewTest::SetUp()
{
    ywb_uint32_t objloop = 0;
    OBJKey objkey;
    OBJView* objview = NULL;

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
    for(objloop = 0; objloop < 6; objloop++)
    {
        objkey.SetInodeId(Constant::DEFAULT_INODE + objloop);
        objkey.SetChunkId(Constant::DEFAULT_CHUNK_ID + objloop);
        objview = new OBJView();
        YWB_ASSERT(objview != NULL);
        mDiskView->AddOBJView(objkey, objview);
    }
}

void DiskViewTest::TearDown()
{

}

ywb_status_t DiskViewTest::AddOBJView(OBJKey& objkey, OBJView** objview)
{
    ywb_status_t ret = YWB_SUCCESS;

    *objview = new OBJView();
    YWB_ASSERT(objview != NULL);
    ret = mDiskView->AddOBJView(objkey, *objview);

    return ret;
}

ywb_status_t DiskViewTest::GetOBJView(OBJKey& objkey, OBJView** objview)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = mDiskView->GetOBJView(objkey, objview);

    return ret;
}

ywb_status_t DiskViewTest::DeleteOBJView(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = mDiskView->DeleteOBJView(objkey);

    return ret;
}

DiskView* DiskViewTest::GetDiskView()
{
    return mDiskView;
}

void SubPoolViewTest::SetUp()
{
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint64_t inodeid = 0;
    DiskKey diskkey;
    DiskBaseProp diskprop;
    OBJKey objkey;
    DiskView* diskview = NULL;
    OBJView* objview = NULL;

    inodeid = Constant::DEFAULT_INODE;
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    SetDefaultDiskBaseProp(diskprop);
    for(diskloop = 0; diskloop < 6; diskloop++)
    {
        diskkey.SetDiskId(Constant::DISK_ID + diskloop);
        diskview = new DiskView(diskprop);
        YWB_ASSERT(diskview != NULL);
        mSubPoolView->AddDiskView(diskkey, diskview);

        if((diskloop % 2) == 1)
        {
            mNonEmptyDiskCnt++;
            objkey.SetStorageId(Constant::DISK_ID + diskloop);
            objkey.SetChunkIndex(1);
            objkey.SetChunkVersion(1);
            for(objloop = 0; objloop < 6; objloop++, inodeid++)
            {
                mOBJCnt++;
                objkey.SetInodeId(inodeid);
                objkey.SetChunkId(inodeid);
                objview = new OBJView();
                YWB_ASSERT(objview != NULL);
                diskview->AddOBJView(objkey, objview);
            }
        }
    }
}

void SubPoolViewTest::TearDown()
{

}

ywb_status_t SubPoolViewTest::AddDiskView(DiskKey& diskkey)
{
    DiskBaseProp diskprop;
    DiskView* diskview = NULL;

    SetDefaultDiskBaseProp(diskprop);
    diskview = new DiskView(diskprop);
    YWB_ASSERT(diskview != NULL);
    return mSubPoolView->AddDiskView(diskkey, diskview);
}

ywb_status_t SubPoolViewTest::GetDiskView(
        DiskKey& diskkey, DiskView** diskview)
{
    return mSubPoolView->GetDiskView(diskkey, diskview);
}

ywb_status_t SubPoolViewTest::RemoveDiskView(DiskKey& diskkey)
{
    return mSubPoolView->RemoveDiskView(diskkey);
}

ywb_status_t SubPoolViewTest::AddOBJView(
        DiskKey& diskkey, OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJView* objview = NULL;

    objview = new OBJView();
    YWB_ASSERT(objview != NULL);

    ret = mSubPoolView->AddOBJView(diskkey, objkey, objview);
    if(YWB_SUCCESS != ret)
    {
        delete objview;
        objview = NULL;
    }

    return ret;
}

ywb_status_t SubPoolViewTest::GetOBJView(DiskKey& diskkey,
        OBJKey& objkey, OBJView** objview)
{
    return mSubPoolView->GetOBJView(diskkey, objkey, objview);
}

ywb_status_t SubPoolViewTest::DeleteOBJView(
        DiskKey& diskkey, OBJKey& objkey)
{
    return mSubPoolView->DeleteOBJView(diskkey, objkey);
}

SubPoolView* SubPoolViewTest::GetSubPoolView()
{
    return mSubPoolView;
}

SubPoolView* SubPoolViewTest::GetSubPoolView2()
{
    return mSubPoolView2;
}

void ConfigViewTest::SetUp()
{
    ywb_uint32_t subpoolloop = 0;
    ywb_uint32_t diskloop = 0;
    ywb_uint32_t objloop = 0;
    ywb_uint32_t subpoolid = 0;
    ywb_uint64_t diskid = 0;
    ywb_uint64_t inodeid = 0;
    DiskKey diskkey;
    DiskBaseProp diskprop;
    OBJKey objkey;
    TimeScheduleConfig schedule;

    subpoolid = Constant::SUBPOOL_ID;
    diskid = Constant::DISK_ID;
    inodeid = Constant::DEFAULT_INODE;
    SetDefaultDiskBaseProp(diskprop);
    mHUB->SetEnableInBatch(false);
    mHUB->SetASTStatus(HUB::AS_enabled);
    for(subpoolloop = 0; subpoolloop < 6; subpoolloop++, subpoolid++)
    {
        mNonEmptySubPoolCnt++;
        diskkey.SetSubPoolId(subpoolid);
        for(diskloop = 0; diskloop < 6; diskloop++, diskid++)
        {
            mDiskCnt++;
            diskkey.SetDiskId(diskid);
            AddDiskView(diskkey, diskprop);
            if((diskloop % 2) == 1)
            {
                mNonEmptyDiskCnt++;
                objkey.SetStorageId(diskid);
                objkey.SetChunkIndex(1);
                objkey.SetChunkVersion(1);
                for(objloop = 0; objloop < 6; objloop++, inodeid++)
                {
                    mTotalOBJCnt++;
                    if(0 == (subpoolloop % 2))
                    {
                        mTotalOBJCntOnSubPoolWithASTEnabled++;
                    }

                    objkey.SetInodeId(inodeid);
                    objkey.SetChunkId(inodeid);
                    AddOBJView(objkey);
                }
            }
        }

        /*
         * to simulate that some subpool is AST-enabled, and others not
         * mainly for test of GetFirstOBJView/GetNextOBJView
         * */
        if(0 == (subpoolloop % 2))
        {
            SetSubPoolASTStatus(subpoolid, HUB::AS_enabled);
            SetSubPoolTimeScheduleConfig(subpoolid, schedule);
        }
    }
}

void ConfigViewTest::TearDown()
{

}

ywb_status_t
ConfigViewTest::SetSubPoolASTStatus(ywb_uint32_t subpoolid, ywb_uint32_t aststatus)
{
    SubPoolKey subpoolkey(mHUB->GetSelfServiceId(), subpoolid, subpoolid);
    return mHUB->SetSubPoolASTStatus(subpoolkey, aststatus);
}

ywb_status_t
ConfigViewTest::SetSubPoolTimeScheduleConfig(
        ywb_uint32_t subpoolid, TimeScheduleConfig& schedule)
{
    SubPoolKey subpoolkey(mHUB->GetSelfServiceId(), subpoolid, subpoolid);
    return mHUB->SetSubPoolTimeScheduleConfig(subpoolkey, schedule);
}

ywb_status_t ConfigViewTest::GetSubPoolView(SubPoolKey& subpoolkey,
        SubPoolView** subpoolview)
{
    return mConfigView->GetSubPoolView(subpoolkey, subpoolview);
}

ywb_status_t ConfigViewTest::AddDiskView(
        DiskKey& diskkey, DiskBaseProp& diskprop)
{
    return mConfigView->AddDiskView(diskkey, diskprop);
}

ywb_status_t ConfigViewTest::GetDiskView(
        DiskKey& diskkey, DiskView** diskview)
{
    return mConfigView->GetDiskView(diskkey, diskview);
}

ywb_status_t ConfigViewTest::RemoveDiskView(DiskKey& diskkey)
{
    return mConfigView->RemoveDiskView(diskkey);
}

ywb_status_t ConfigViewTest::AddOBJView(OBJKey& objkey)
{
    return mConfigView->AddOBJView(objkey);
}

ywb_status_t ConfigViewTest::GetOBJView(OBJKey& objkey, OBJView** objinfo)
{
    return mConfigView->GetOBJView(objkey, objinfo);
}

ywb_status_t ConfigViewTest::DeleteOBJView(OBJKey& objkey)
{
    return mConfigView->DeleteOBJView(objkey);
}

ywb_status_t HUBMock2::AddDisk(DiskKey& diskkey,
        DiskBaseProp& diskprop)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = HUB::AddDisk(diskkey, diskprop);
    /*tell the external that AddDiskEvent has been handled*/
    Signal();

    return ret;
}

ywb_status_t HUBMock2::RemoveDisk(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = HUB::RemoveDisk(diskkey);
    Signal();

    return ret;
}

ywb_status_t HUBMock2::AddOBJ(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = HUB::AddOBJ(objkey);
    Signal();

    return ret;
}

ywb_status_t HUBMock2::DeleteOBJ(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = HUB::DeleteOBJ(objkey);
    Signal();

    return ret;
}

ywb_status_t HUBMock2::AccessOBJ(OBJKey& objkey, off_t offset,
        size_t len, ywb_uint32_t ios, ywb_uint32_t bw,
        ywb_uint32_t rt, ywb_bool_t read)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = HUB::AccessOBJ(objkey, offset, len, ios, bw, rt, read);
    Signal();

    return ret;
}

ywb_status_t HUBMock2::Migrate(OBJKey& objkey, DiskKey& tgtdisk)
{
    ywb_status_t ret = YWB_SUCCESS;

    mMigratingOBJ = objkey;
    Signal();

    return ret;
}

ywb_status_t HUBMock2::CompleteMigrate(OBJKey& objkey, ywb_status_t status)
{
    ywb_status_t ret = YWB_SUCCESS;

    mLastMigratedOBJ = objkey;
    Signal();

    return ret;
}

void HUBMock2::SendRemovedSubPool()
{
    mRemovedSubPoolCnt++;
    Signal();
}

void HUBMock2::SendRemovedDisk()
{
    mRemovedDiskCnt++;
    Signal();
}

void HUBMock2::SendRemovedOBJ()
{
    mDeletedOBJCnt++;
    Signal();
}

void HUBMock2::SendConfig()
{
    mConfigViewCollectedTimes++;
    Signal();
}

void HUBMock2::SendIO()
{
    mIOCollectedTimes++;
    Signal();
}

void HUBMock2::ResetAST()
{
    AST* ast = NULL;

    GetAST(&ast);
    YWB_ASSERT(ast != NULL);

    ast->Finalize();
    ast->Initialize();
}

void HUBTest::SetUp()
{
    StartHUB();
    /*StartAST();*/
}

void HUBTest::TearDown()
{
    /*StopAST(true);*/
    /*clear the state of AST*/
    /*ResetAST();*/
    StopHUB(false);
}

ywb_uint32_t HUBTest::GetASTStatus()
{
    return mHUB->GetASTStatus();
}

ywb_status_t HUBTest::StartHUB()
{
    ywb_status_t ret = YWB_SUCCESS;

    /*
     * DO　NOT de-construct @mRawHUB for it share the same AST
     * pointer with @mHUB(AST is got by GetSingleASTInstance())
     * */
    mRawHUB = gCenter->GetHUB();
    gCenter->SetHUB(mHUB);
    /*to match with Stop() in ControlCenter::Finalize()*/
    mRawHUB->Start();
    mHUB->Start();

    return ret;
}

ywb_status_t HUBTest::StopHUB(ywb_bool_t immediate)
{
    ywb_status_t ret = YWB_SUCCESS;

    mHUB->Stop(immediate);
    gCenter->SetHUB(mRawHUB);

    return ret;
}

ywb_status_t HUBTest::StartAST()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t startwaittimes = 0;

    mHUB->LaunchEnableASTEvent();
    /*wait until the AST is really enabled*/
    while((startwaittimes < 50) && (GetASTStatus() != HUB::AS_enabled))
    {
        startwaittimes++;
        usleep(1000);
    }

    if(startwaittimes >= 50)
    {
        ast_log_fatal("fail to wait until AST started");
    }
    else
    {
        ast_log_debug("wait until AST started for " << startwaittimes << " times");
    }

    return ret;
}

ywb_status_t HUBTest::StopAST(ywb_bool_t immediate)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t stopwaittimes = 0;

    mHUB->LaunchDisableASTEvent(immediate);
    /*wait until the AST is really disabled*/
    while((stopwaittimes < 50) && (GetASTStatus() != HUB::AS_disabled))
    {
        stopwaittimes++;
        usleep(1000);
    }

    if(stopwaittimes >= 50)
    {
        ast_log_fatal("fail to wait until AST stopped");
    }
    else
    {
        ast_log_debug("wait until AST stopped for " << stopwaittimes << " times");
    }

    return ret;
}

ywb_status_t HUBTest::ResetAST()
{
    ywb_status_t ret = YWB_SUCCESS;

    mHUB->ResetAST();

    return ret;
}

void HUBTest::WaitForHUBSignal()
{
    mHUB->Wait();
}

void HUBTest::Lock()
{
    mHUB->GetLock()->Lock();
}

void HUBTest::Wait()
{
    mHUB->GetCond()->Wait(*(mHUB->GetLock()));
}

void HUBTest::Unlock()
{
    mHUB->GetLock()->Unlock();
}

void HUBTest::SetDiskKey(DiskKey& diskkey, ywb_uint64_t diskid)
{
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskkey.SetDiskId(diskid);
}

void HUBTest::SetDefaultDiskBaseProp(DiskBaseProp& diskprop)
{
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskprop.SetDiskRPM(10000);
    diskprop.SetRaidType(DiskBaseProp::DRT_raid0);
    diskprop.SetRaidWidth(1);
    diskprop.SetDiskCap(128000);
}

void HUBTest::SetOBJKey(OBJKey& objkey,
        ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    objkey.SetStorageId(diskid);
    objkey.SetInodeId(inodeid);
    objkey.SetChunkId(inodeid);
    objkey.SetChunkIndex(1);
    objkey.SetChunkVersion(1);
}

void HUBTest::AddOneDisk(ywb_uint64_t diskid, ywb_bool_t directly)
{
    DiskKey diskkey;
    DiskBaseProp diskprop;

    SetDiskKey(diskkey, diskid);
    SetDefaultDiskBaseProp(diskprop);
    if(directly)
    {
        mHUB->AddDisk(diskkey, diskprop);
    }
    else
    {
        mHUB->LaunchAddDiskEvent(diskkey, diskprop);
    }
}

void HUBTest::RemoveOneDisk(ywb_uint64_t diskid, ywb_bool_t directly)
{
    DiskKey diskkey;

    mRemovedDiskCnt++;
    SetDiskKey(diskkey, diskid);
    if(directly)
    {
        mHUB->RemoveDisk(diskkey);
    }
    else
    {
        mHUB->LaunchRemoveDiskEvent(diskkey);
    }
}

void HUBTest::AddOneOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid, ywb_bool_t directly)
{
    OBJKey objkey;

    SetOBJKey(objkey, diskid, inodeid);
    if(directly)
    {
        mHUB->AddOBJ(objkey);
    }
    else
    {
        mHUB->LaunchAddOBJEvent(objkey);
    }
}

void HUBTest::DeleteOneOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid, ywb_bool_t directly)
{
    OBJKey objkey;

    mDeletedOBJCnt++;
    SetOBJKey(objkey, diskid, inodeid);
    if(directly)
    {
        mHUB->DeleteOBJ(objkey);
    }
    else
    {
        mHUB->LaunchDeleteOBJEvent(objkey);
    }
}

void HUBTest::AccessOneOBJ(ywb_uint64_t diskid, ywb_uint64_t inodeid,
        off_t offset, size_t len, ywb_uint32_t ios, ywb_uint32_t bw,
        ywb_uint32_t rt, ywb_bool_t read, ywb_bool_t directly)
{
    OBJKey objkey;

    SetOBJKey(objkey, diskid, inodeid);
    if(directly)
    {
        mHUB->AccessOBJ(objkey, offset, len, ios, bw, rt, read);
    }
    else
    {
        mHUB->LaunchAccessOBJEvent(objkey, offset, len, ios, bw, rt, read);
    }
}

void HUBTest::AddDisksDirectly(ywb_uint64_t firstdiskid,ywb_uint32_t disknum)
{
    ywb_uint32_t diskloop = 0;

    for(diskloop = 0; diskloop < disknum; diskloop++)
    {
        AddOneDisk(firstdiskid + diskloop, true);
    }
}

void HUBTest::AddOBJsDirectly(ywb_uint64_t diskid,
        ywb_uint64_t firstinodeid, ywb_uint32_t objnum)
{
    ywb_uint32_t objloop = 0;

    for(objloop = 0; objloop < objnum; objloop++)
    {
        AddOneOBJ(diskid, firstinodeid + objloop, true);
    }
}

ywb_status_t HUBTest::GetSubPoolKeyFromDiskID(
        ywb_uint64_t diskid, SubPoolKey& subpoolkey)
{
    ConfigView* configview = NULL;

    mHUB->GetConfigView(&configview);
    YWB_ASSERT(configview != NULL);
    return configview->GetSubPoolKeyFromDiskID(diskid, subpoolkey);
}

ywb_status_t HUBTest::GetDiskView(
        ywb_uint64_t diskid, DiskView** diskview)
{
    DiskKey diskkey;

    SetDiskKey(diskkey, diskid);
    return mHUB->GetDiskView(diskkey, diskview);
}

ywb_status_t HUBTest::GetOBJView(ywb_uint64_t diskid,
        ywb_uint64_t inodeid, OBJView** objview)
{
    OBJKey objkey;

    SetOBJKey(objkey, diskid, inodeid);
    return mHUB->GetOBJView(objkey, objview);
}

void HUBTest::PrepareConfigView()
{
    AddDisksDirectly(Constant::DISK_ID, 3);
    AddOBJsDirectly(Constant::DISK_ID, Constant::DEFAULT_INODE, 6);
    AddOBJsDirectly(Constant::DISK_ID + 1, Constant::DEFAULT_INODE, 6);
    AddOBJsDirectly(Constant::DISK_ID + 1, Constant::DEFAULT_INODE, 6);
}

void HUBTest::CollectConfigView()
{
    mConfigViewCollectedTimes++;
    mHUB->LaunchCollectConfigViewEvent(100);
}

ywb_bool_t HUBTest::CheckConfigView()
{
    /*WaitForHUBSignal();*/
    return (mHUB->GetConfigViewCollectedTimes() == mConfigViewCollectedTimes);
}

void HUBTest::PrepareIO()
{
    ywb_uint32_t firstinode = Constant::DEFAULT_INODE;

    AddDisksDirectly(Constant::DISK_ID, 3);
    AddOBJsDirectly(Constant::DISK_ID, firstinode, 6);
    firstinode += 6;
    AddOBJsDirectly(Constant::DISK_ID + 1, firstinode, 6);
    firstinode += 6;
    AddOBJsDirectly(Constant::DISK_ID + 2, firstinode, 6);
    AccessOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE,
            2, 4, 1, 4, 305, true, true);
    AccessOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE,
            6, 4, 1, 4, 315, true, true);
    AccessOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE + 1,
            2, 4, 1, 4, 325, true, true);
    AccessOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE + 2,
            6, 4, 1, 4, 315, true, true);
    AccessOneOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE + 3,
            2, 4, 1, 4, 325, true, true);
}

void HUBTest::CollectIO()
{
    mIOCollectedTimes++;
    mHUB->LaunchCollectIOEvent(199, HUBEventSet::CollectIOEvent::F_prep);
    mHUB->LaunchCollectIOEvent(200, HUBEventSet::CollectIOEvent::F_normal);
}

ywb_bool_t HUBTest::CheckIO()
{
    /*WaitForHUBSignal();*/
    mHUB->LaunchCollectIOEvent(201, HUBEventSet::CollectIOEvent::F_post);
    return (mHUB->GetIOCollectedTimes() == mIOCollectedTimes);
}

void HUBTest::PrepareMigrate()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t firstinode = Constant::DEFAULT_INODE;
    SubPoolKey subpoolkey;
    TimeScheduleConfig schedule;

    AddDisksDirectly(Constant::DISK_ID, 3);
    AddOBJsDirectly(Constant::DISK_ID, firstinode, 6);
    firstinode += 6;
    AddOBJsDirectly(Constant::DISK_ID + 1, firstinode, 6);
    firstinode += 6;
    AddOBJsDirectly(Constant::DISK_ID + 2, firstinode, 6);

    mHUB->SetASTStatus(HUB::AS_enabled);
    ret = GetSubPoolKeyFromDiskID(Constant::DISK_ID, subpoolkey);
    YWB_ASSERT(YWB_SUCCESS == ret);
    mHUB->SetSubPoolASTStatus(subpoolkey, HUB::AS_enabled);
    mHUB->SetSubPoolTimeScheduleConfig(subpoolkey, schedule);
}

void HUBTest::Migrate(ywb_uint64_t srcdiskid, ywb_uint64_t inodeid, ywb_uint64_t tgtdiskid)
{
    OBJKey objkey;
    DiskKey tgtdisk;

    SetOBJKey(objkey, srcdiskid, inodeid);
    SetDiskKey(tgtdisk, tgtdiskid);
    mMigratingOBJ = objkey;
    mHUB->LaunchMigrateEvent(objkey, tgtdisk);
}

ywb_bool_t HUBTest::CheckMigrate()
{
    OBJKey objkey;

    /*WaitForHUBSignal();*/
    objkey = mHUB->GetMigratingOBJ();
    return ((objkey.GetStorageId() == mMigratingOBJ.GetStorageId()) &&
            (objkey.GetInodeId() == mMigratingOBJ.GetInodeId()));
}


void HUBTest::CompleteMigrate(ywb_uint64_t diskid, ywb_uint64_t inodeid)
{
    OBJKey objkey;

    SetOBJKey(objkey, diskid, inodeid);
    mLastMigratedOBJ = objkey;
    mHUB->LaunchMigrationCompletedEvent(objkey, YWB_SUCCESS);
}

ywb_bool_t HUBTest::CheckCompleteMigrate()
{
    OBJKey objkey;

    WaitForHUBSignal();
    objkey = mHUB->GetLastMigratedOBJ();
    return ((objkey.GetStorageId() == mLastMigratedOBJ.GetStorageId()) &&
            (objkey.GetInodeId() == mLastMigratedOBJ.GetInodeId()));
}

HUBMock2* HUBTest::GetHUBMock2()
{
    return mHUB;
}

int main(int argc, char *argv[])
{
    int ret = 0;

    testing::AddGlobalTestEnvironment(new ASTHUBTestEnvironment);
    testing::InitGoogleTest(&argc, argv);
    ret = RUN_ALL_TESTS();

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
