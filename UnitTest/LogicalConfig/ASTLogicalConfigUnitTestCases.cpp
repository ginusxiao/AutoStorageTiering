#include "UnitTest/AST/LogicalConfig/ASTLogicalConfigUnitTests.hpp"

TEST_F(DiskTest, AddOBJ_SUCCESS)
{
    Disk* disk = NULL;

    disk = GetDisk();
    ASSERT_EQ(disk->GetOBJCnt(), (ywb_uint32_t)6);
}

TEST_F(DiskTest, AddOBJ_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    disk = GetDisk();

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 2);
    obj = new OBJ(objkey);

    ret = disk->AddOBJ(objkey, obj);
    ASSERT_EQ(ret, YFS_EEXIST);
    delete obj;
    obj = NULL;
}

TEST_F(DiskTest, GetOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    disk = GetDisk();
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 2);

    ret = disk->GetOBJ(objkey, &obj);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(obj != NULL, true);
}

TEST_F(DiskTest, GetOBJ_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    disk = GetDisk();
    objkey.SetStorageId(Constant::DISK_ID - 1);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 2);

    ret = disk->GetOBJ(objkey, &obj);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(obj == NULL, true);
}

TEST_F(DiskTest, RemoveOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    disk = GetDisk();
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 2);

    ret = disk->RemoveOBJ(objkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = disk->GetOBJ(objkey, &obj);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(obj == NULL, true);
}

TEST_F(DiskTest, RemoveOBJ_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    Disk* disk = NULL;

    disk = GetDisk();
    objkey.SetStorageId(Constant::DISK_ID - 1);
    objkey.SetInodeId(Constant::DEFAULT_INODE + 2);

    ret = disk->RemoveOBJ(objkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(DiskTest, GetFirstOBJ_DiskHasNoOBJ_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    disk = new Disk();
    ret = disk->GetFirstOBJ(&obj, &objkey);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(obj == NULL, true);
}

TEST_F(DiskTest, GetFirstAndNextOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t loop = 0;
    ywb_uint32_t objnum = 0;
    OBJKey objkey;
    Disk* disk = NULL;
    OBJ* obj = NULL;

    disk = GetDisk();
    objnum = disk->GetOBJCnt();
    ret = disk->GetFirstOBJ(&obj, &objkey);
    while((YWB_SUCCESS == ret) && (obj != NULL))
    {
        ASSERT_EQ(objkey.GetStorageId(), (ywb_uint64_t)Constant::DISK_ID);
        ASSERT_EQ(objkey.GetInodeId(), Constant::DEFAULT_INODE + loop);
        loop++;
        ret = disk->GetNextOBJ(&obj, &objkey);
    }

    ASSERT_EQ(loop, objnum);
}

TEST_F(DiskTest, DeterminDiskTier_SUCCESS)
{
    Disk* disk = NULL;
    DiskBaseProp diskprop;
    ywb_uint32_t tier;

    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    disk = GetDisk();
    disk->SetBaseProp(diskprop);
    tier = disk->DetermineDiskTier();
    ASSERT_EQ(tier, Tier::TIER_1);

    diskprop.SetDiskClass(100);
    disk->SetBaseProp(diskprop);
    tier = disk->DetermineDiskTier();
    ASSERT_EQ(tier, Tier::TIER_cnt);
}

TEST_F(DiskTest, SummarizeDiskIO_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    IOStat::Type stattype;
    Workload::Characteristics workload;
    Disk* disk = NULL;

    disk = GetDisk();
    ret = disk->SummarizeDiskIO();
    ASSERT_EQ(ret, YWB_SUCCESS);

    stattype = IOStat::IOST_raw;
    workload = Workload::WC_io;
    match = StatMatch(stattype, workload);
    ASSERT_EQ(match, true);

    workload = Workload::WC_bw;
    match = StatMatch(stattype, workload);
    ASSERT_EQ(match, true);

    workload = Workload::WC_rt;
    match = StatMatch(stattype, workload);
    ASSERT_EQ(match, true);
}

TEST_F(TierTest, GetDiskCnt_SUCCESS)
{
    ywb_uint32_t diskcnt = 0;
    Tier* tier = NULL;

    SetUpByOurselves();
    tier = GetTier();
    diskcnt = tier->GetDiskCnt();
    ASSERT_EQ(diskcnt, (ywb_uint32_t)Constant::DEFAULT_DISK_NUM_PER_TIER);
}

TEST_F(TierTest, GetOBJCnt_SUCCESS)
{
    ywb_uint32_t objcnt = 0;
    Tier* tier = NULL;

    SetUpByOurselves();
    tier = GetTier();
    objcnt = tier->GetOBJCnt();
    ASSERT_EQ(objcnt, (ywb_uint32_t)(Constant::DEFAULT_DISK_NUM_PER_TIER *
            6));
}

TEST_F(TierTest, AddDisk_SUCCESS)
{
    ywb_uint32_t diskcnt = 0;
    Tier* tier = NULL;

    SetUpByOurselves();
    tier = GetTier();
    diskcnt = tier->GetDiskCnt();
    ASSERT_EQ(diskcnt, (ywb_uint32_t)Constant::DEFAULT_DISK_NUM_PER_TIER);
}

TEST_F(TierTest, AddDisk_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Tier* tier = NULL;
    Disk* disk = NULL;

    SetUpByOurselves();
    tier = GetTier();
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    disk = new Disk();
    ret = tier->AddDisk(diskkey, disk);
    ASSERT_EQ(ret, YFS_EEXIST);
    delete disk;
    disk = NULL;
}

TEST_F(TierTest, GetDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Tier* tier = NULL;
    Disk* disk = NULL;

    SetUpByOurselves();
    tier = GetTier();
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    ret = tier->GetDisk(diskkey, &disk);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(disk != NULL, true);
}

TEST_F(TierTest, GetDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Tier* tier = NULL;
    Disk* disk = NULL;

    SetUpByOurselves();
    tier = GetTier();
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1000);

    ret = tier->GetDisk(diskkey, &disk);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(disk == NULL, true);
}

TEST_F(TierTest, RemoveDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskcnt1 = 0;
    ywb_uint32_t diskcnt2 = 0;
    DiskKey diskkey;
    Tier* tier = NULL;

    SetUpByOurselves();
    tier = GetTier();
    diskcnt1 = tier->GetDiskCnt();
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    ret = tier->RemoveDisk(diskkey);
    diskcnt2 = tier->GetDiskCnt();
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskcnt2, diskcnt1 - 1);
}

TEST_F(TierTest, RemoveDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t diskcnt1 = 0;
    ywb_uint32_t diskcnt2 = 0;
    DiskKey diskkey;
    Tier* tier = NULL;

    SetUpByOurselves();
    tier = GetTier();
    diskcnt1 = tier->GetDiskCnt();
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID + 1000);

    ret = tier->RemoveDisk(diskkey);
    diskcnt2 = tier->GetDiskCnt();
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(diskcnt2, diskcnt1);
}

TEST_F(TierTest, GetFirstOBJ_TierHasNoDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    Tier tier;
    OBJ* obj = NULL;

    ret = tier.GetFirstOBJ(&obj);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(obj == NULL, true);
}

TEST_F(TierTest, GetFirstOBJ_AllDisksEmpty_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t addeddisks = 0;
    Tier* tier = NULL;
    OBJ* obj = NULL;

    tier = GetTier();
    addeddisks = AddDisks(Constant::SUBPOOL_ID, Constant::DISK_ID, 2);
    YWB_ASSERT(addeddisks == 2);

    ret = tier->GetFirstOBJ(&obj);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(obj == NULL, true);
}

TEST_F(TierTest, GetFirstOBJ_FirstDiskHasNoOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t addeddisks = 0;
    ywb_uint32_t addedobjs = 0;
    Tier* tier = NULL;
    OBJKey *objkey = NULL;
    OBJ* objgot = NULL;

    tier = GetTier();
    addeddisks = AddDisks(Constant::SUBPOOL_ID, Constant::DISK_ID, 2);
    YWB_ASSERT(addeddisks == 2);
    addedobjs = AddOBJs(Constant::SUBPOOL_ID,
            Constant::DISK_ID + 1, Constant::DEFAULT_INODE, 2);
    YWB_ASSERT(addedobjs == 2);

    ret = tier->GetFirstOBJ(&objgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    objgot->GetOBJKey(&objkey);
    ASSERT_EQ(objkey->GetInodeId(), (ywb_uint64_t)Constant::DEFAULT_INODE);
}

TEST_F(TierTest, GetFirstAndNextOBJ_IntermediateDiskHasNoOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t addeddisks = 0;
    ywb_uint32_t addedobjs = 0;
    Tier* tier = NULL;
    OBJ* objgot = NULL;
    OBJKey* objkey = NULL;

    tier = GetTier();
    addeddisks = AddDisks(Constant::SUBPOOL_ID, Constant::DISK_ID, 3);
    YWB_ASSERT(addeddisks == 3);
    addedobjs = AddOBJs(Constant::SUBPOOL_ID,
            Constant::DISK_ID, Constant::DEFAULT_INODE, 2);
    YWB_ASSERT(addedobjs == 2);

    addedobjs = AddOBJs(Constant::SUBPOOL_ID,
            Constant::DISK_ID, Constant::DEFAULT_INODE + 20, 2);
    YWB_ASSERT(addedobjs == 2);

//    objkey[0].SetStorageId(diskkey[0].GetDiskId());
//    objkey[0].SetInodeId(0);
//    objkey[1].SetStorageId(diskkey[0].GetDiskId());
//    objkey[1].SetInodeId(1);
//    obj[0] = new OBJ(objkey[0]);
//    disk[0]->AddOBJ(objkey[0], obj[0]);
//    obj[0] = new OBJ(objkey[1]);
//    disk[0]->AddOBJ(objkey[1], obj[1]);
//
//    objkey[2].SetStorageId(diskkey[2].GetDiskId());
//    objkey[2].SetInodeId(20);
//    objkey[3].SetStorageId(diskkey[2].GetDiskId());
//    objkey[3].SetInodeId(21);
//    disk[2]->AddOBJ(objkey[2], obj[2]);
//    disk[2]->AddOBJ(objkey[3], obj[3]);
//    tier->AddDisk(diskkey[2], disk[2]);

    ret = tier->GetFirstOBJ(&objgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    objgot->GetOBJKey(&objkey);
    ASSERT_EQ(objkey->GetInodeId(), (ywb_uint64_t)Constant::DEFAULT_INODE);

    ret = tier->GetNextOBJ(&objgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    objgot->GetOBJKey(&objkey);
    ASSERT_EQ(objkey->GetInodeId(), (ywb_uint64_t)(Constant::DEFAULT_INODE + 1));

    ret = tier->GetNextOBJ(&objgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    objgot->GetOBJKey(&objkey);
    ASSERT_EQ(objkey->GetInodeId(), (ywb_uint64_t)(Constant::DEFAULT_INODE + 20));

    ret = tier->GetNextOBJ(&objgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    objgot->GetOBJKey(&objkey);
    ASSERT_EQ(objkey->GetInodeId(), (ywb_uint64_t)(Constant::DEFAULT_INODE + 21));

    ret = tier->GetNextOBJ(&objgot);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierTest, GetFirstDisk_TierHasNoDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    Tier tier;
    Disk* disk = NULL;

    ret = tier.GetFirstDisk(&disk);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(disk == NULL, true);
}

TEST_F(TierTest, GetFirstAndNextDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t loop = 0;
    Tier* tier = NULL;
    Disk* disk = NULL;

    SetUpByOurselves();
    tier = GetTier();
    ret = tier->GetFirstDisk(&disk);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(disk != NULL, true);

    loop = 1;
    while(loop++ < Constant::DEFAULT_DISK_NUM_PER_TIER)
    {
        ret = tier->GetNextDisk(&disk);
        ASSERT_EQ(ret, YWB_SUCCESS);
        ASSERT_EQ(disk != NULL, true);
    }

    ret = tier->GetNextDisk(&disk);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(TierTest, GetKeysAndDisks_TierHasNoDisk_SUCCESS)
{
    ywb_uint32_t ret = 0;
    Tier tier;
    list<DiskKey> diskkeys;
    list<Disk*> disks;

    ret = tier.GetKeysAndDisks(diskkeys, disks);
    ASSERT_EQ(ret, 0);
    ASSERT_EQ(diskkeys.empty(), true);
    ASSERT_EQ(disks.empty(), true);
}

TEST_F(TierTest, GetKeysAndDisks_SUCCESS)
{
    ywb_uint32_t ret = 0;
    Tier* tier = NULL;
    list<DiskKey> diskkeys;
    list<Disk*> disks;

    SetUpByOurselves();
    tier = GetTier();
    ret = tier->GetKeysAndDisks(diskkeys, disks);
    ASSERT_EQ(ret, (ywb_uint32_t)Constant::DEFAULT_DISK_NUM_PER_TIER);
    ASSERT_EQ(diskkeys.size(), ret);
    ASSERT_EQ(disks.size(), ret);
}

TEST_F(SubPoolTest, AddTier_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;
    Tier* tier = NULL;

    subpool = GetSubPool();
    tier = new Tier(Tier::TIER_0);
    ret = subpool->AddTier(tier);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(SubPoolTest, AddTier_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;
    Tier* tier[2] = {NULL, NULL};

    subpool = GetSubPool();
    tier[0] = new Tier(Tier::TIER_0);
    tier[1] = new Tier(Tier::TIER_0);
    ret = subpool->AddTier(tier[0]);
    ret = subpool->AddTier(tier[1]);
    ASSERT_EQ(ret, YFS_EEXIST);
}

TEST_F(SubPoolTest, GetTier_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Tier* tiergot = NULL;

    subpool = GetSubPool();
    tier = new Tier(Tier::TIER_0);
    subpool->AddTier(tier);
    ret = subpool->GetTier(Tier::TIER_1, &tiergot);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolTest, GetTier_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Tier* tiergot = NULL;

    subpool = GetSubPool();
    tier = new Tier(Tier::TIER_0);
    subpool->AddTier(tier);
    ret = subpool->GetTier(Tier::TIER_0, &tiergot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(tiergot, tier);
}

TEST_F(SubPoolTest, AdjustTierComb_InvalidTierType_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;

    subpool = GetSubPool();
    ret = subpool->AdjustTierComb(Tier::TIER_cnt, true);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(SubPoolTest, AdjustTierComb_AddToInitialValue_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t curtiercomb = 0;
    SubPool* subpool = NULL;

    subpool = GetSubPool();
    ret = subpool->AdjustTierComb(Tier::TIER_0, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    curtiercomb = subpool->GetCurTierComb();
    ASSERT_EQ(curtiercomb, 1);
}

TEST_F(SubPoolTest, AdjustTierComb_Add_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t curtiercomb = 0;
    SubPool* subpool = NULL;

    subpool = GetSubPool();
    ret = subpool->AdjustTierComb(Tier::TIER_0, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    curtiercomb = subpool->GetCurTierComb();
    ASSERT_EQ(curtiercomb, 1);

    ret = subpool->AdjustTierComb(Tier::TIER_1, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    curtiercomb = subpool->GetCurTierComb();
    ASSERT_EQ(curtiercomb, 3);

    ret = subpool->AdjustTierComb(Tier::TIER_2, true);
    ASSERT_EQ(ret, YWB_SUCCESS);
    curtiercomb = subpool->GetCurTierComb();
    ASSERT_EQ(curtiercomb, 7);
}

TEST_F(SubPoolTest, AdjustTierComb_DeleteToInitialValue_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t prevtiercomb = 0;
    ywb_uint32_t curtiercomb = 0;
    SubPool* subpool = NULL;

    subpool = GetSubPool();
    prevtiercomb = subpool->GetCurTierComb();
    ret = subpool->AdjustTierComb(Tier::TIER_0, false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    curtiercomb = subpool->GetCurTierComb();
    ASSERT_EQ(curtiercomb, prevtiercomb);
}

TEST_F(SubPoolTest, AdjustTierComb_Delete_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t curtiercomb = 0;
    SubPool* subpool = NULL;

    subpool = GetSubPool();
    subpool->AdjustTierComb(Tier::TIER_0, true);
    subpool->AdjustTierComb(Tier::TIER_1, true);
    subpool->AdjustTierComb(Tier::TIER_2, true);
    ret = subpool->AdjustTierComb(Tier::TIER_2, false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    curtiercomb = subpool->GetCurTierComb();
    ASSERT_EQ(curtiercomb, 3);
    ret = subpool->AdjustTierComb(Tier::TIER_0, false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    curtiercomb = subpool->GetCurTierComb();
    ASSERT_EQ(curtiercomb, 2);
    ret = subpool->AdjustTierComb(Tier::TIER_1, false);
    ASSERT_EQ(ret, YWB_SUCCESS);
    curtiercomb = subpool->GetCurTierComb();
    ASSERT_EQ(curtiercomb, 0);
}

TEST_F(SubPoolTest, AddDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;
    Disk* diskgot = NULL;

    subpool = GetSubPool();
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    disk = new Disk(diskprop);
    ret = subpool->AddDisk(diskkey, disk);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpool->GetTier(Tier::TIER_1, &tier);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(tier != NULL, true);
    ret = tier->GetDisk(diskkey, &diskgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskgot, disk);
}

TEST_F(SubPoolTest, GetDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    Disk* diskgot = NULL;

    subpool = GetSubPool();
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    disk = new Disk(diskprop);
    subpool->AddDisk(diskkey, disk);
    ret = subpool->GetDisk(diskkey, &diskgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(diskgot, disk);
}

TEST_F(SubPoolTest, GetDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    Disk* diskgot = NULL;

    subpool = GetSubPool();
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    disk = new Disk(diskprop);
    subpool->AddDisk(diskkey, disk);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    ret = subpool->GetDisk(diskkey, &diskgot);
    ASSERT_EQ(ret, YFS_ENOENT);
    ASSERT_EQ(diskgot == NULL, true);
}

TEST_F(SubPoolTest, RemoveDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    Disk* diskgot = NULL;

    subpool = GetSubPool();
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    disk = new Disk(diskprop);
    subpool->AddDisk(diskkey, disk);
    ret = subpool->RemoveDisk(diskkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpool->GetDisk(diskkey, &diskgot);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolTest, RemoveDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    Disk* diskgot = NULL;

    subpool = GetSubPool();
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);

    disk = new Disk(diskprop);
    subpool->AddDisk(diskkey, disk);
    diskkey.SetDiskId(Constant::DISK_ID + 1);
    ret = subpool->RemoveDisk(diskkey);
    ASSERT_EQ(ret, YFS_ENOENT);
    diskkey.SetDiskId(Constant::DISK_ID);
    ret = subpool->GetDisk(diskkey, &diskgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(SubPoolTest, AddOBJ_NoCorrespondingDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    SubPool* subpool = NULL;
    OBJ* obj = NULL;

    subpool = GetSubPool();
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    obj = new OBJ(objkey);
    ret = subpool->AddOBJ(objkey, obj);
    ASSERT_EQ(ret, YFS_ENOENT);

    delete obj;
    obj = NULL;
}

TEST_F(SubPoolTest, AddOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJ* objgot = NULL;

    subpool = GetSubPool();
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    disk = new Disk(diskprop);
    subpool->AddDisk(diskkey, disk);

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    obj = new OBJ(objkey);
    ret = subpool->AddOBJ(objkey, obj);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = disk->GetOBJ(objkey, &objgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(objgot, obj);
}

TEST_F(SubPoolTest, GetOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJ* objgot = NULL;

    subpool = GetSubPool();
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    disk = new Disk(diskprop);
    subpool->AddDisk(diskkey, disk);

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    obj = new OBJ(objkey);
    subpool->AddOBJ(objkey, obj);
    ret = subpool->GetOBJ(objkey, &objgot);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(objgot, obj);
}

TEST_F(SubPoolTest, GetOBJ_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    SubPool* subpool = NULL;
    OBJ* objgot = NULL;

    subpool = GetSubPool();
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    ret = subpool->GetOBJ(objkey, &objgot);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolTest, RemoveOBJ_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    SubPool* subpool = NULL;

    subpool = GetSubPool();
    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    ret = subpool->RemoveOBJ(objkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolTest, RemoveOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskBaseProp diskprop;
    DiskKey diskkey;
    OBJKey objkey;
    SubPool* subpool = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJ* objgot = NULL;

    subpool = GetSubPool();
    diskkey.SetDiskId(Constant::DISK_ID);
    diskkey.SetSubPoolId(Constant::SUBPOOL_ID);
    diskprop.SetDiskClass(DiskBaseProp::DCT_ent);
    disk = new Disk(diskprop);
    subpool->AddDisk(diskkey, disk);

    objkey.SetStorageId(Constant::DISK_ID);
    objkey.SetInodeId(Constant::DEFAULT_INODE);
    obj = new OBJ(objkey);
    subpool->AddOBJ(objkey, obj);
    ret = subpool->RemoveOBJ(objkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = subpool->GetOBJ(objkey, &objgot);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(SubPoolTest, SummarizeDiskIO_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t match = false;
    ywb_uint32_t newobjs1 = 0;
    ywb_uint32_t newobjs2 = 0;
    ywb_uint32_t newobjs3 = 0;
    IOStat::Type stattype;
    Workload::Characteristics workload;
    SubPool* subpool = NULL;

    subpool = GetSubPool();
    AddDisk(DiskBaseProp::DCT_ssd, Constant::DISK_ID, Constant::SUBPOOL_ID);
    newobjs1 = AddOBJs(Constant::DISK_ID, Constant::SUBPOOL_ID,
            Constant::DEFAULT_INODE, 6);
    AddDisk(DiskBaseProp::DCT_ent, Constant::DISK_ID + 1, Constant::SUBPOOL_ID);
    newobjs2 = AddOBJs(Constant::DISK_ID + 1, Constant::SUBPOOL_ID,
                Constant::DEFAULT_INODE, 6);
    AddDisk(DiskBaseProp::DCT_sata, Constant::DISK_ID + 2, Constant::SUBPOOL_ID);
    newobjs3 = AddOBJs(Constant::DISK_ID + 2, Constant::SUBPOOL_ID,
                Constant::DEFAULT_INODE, 6);

    YWB_ASSERT(newobjs1 == 6);
    YWB_ASSERT(newobjs2 == 6);
    YWB_ASSERT(newobjs3 == 6);

    ret = subpool->SummarizeDiskIO();
    ASSERT_EQ(ret, YWB_SUCCESS);

    stattype = IOStat::IOST_raw;
    workload = Workload::WC_io;
    match = StatMatch(Constant::DISK_ID, Constant::SUBPOOL_ID, stattype, workload);
    ASSERT_EQ(match, true);
    match = StatMatch(Constant::DISK_ID + 1, Constant::SUBPOOL_ID, stattype, workload);
    ASSERT_EQ(match, true);
    match = StatMatch(Constant::DISK_ID + 2, Constant::SUBPOOL_ID, stattype, workload);
    ASSERT_EQ(match, true);
}

TEST_F(LogicalConfigTest, AddSubPool_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;

    ret = AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YWB_SUCCESS);
    subpool = GetSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(subpool != NULL, true);
}

TEST_F(LogicalConfigTest, AddSubPool_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ret = AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YFS_EEXIST);
}

TEST_F(LogicalConfigTest, AddDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    Disk* disk = NULL;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ret = AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YWB_SUCCESS);
    disk = GetDisk(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(disk != NULL, true);
}

TEST_F(LogicalConfigTest, AddDisk_YFS_EINVAL)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YFS_EINVAL);
}

TEST_F(LogicalConfigTest, AddDisk_YFS_EEXIST)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    ret = AddDisk(DiskBaseProp::DCT_sata,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YFS_EEXIST);
}

TEST_F(LogicalConfigTest, AddOBJ_SUCCESS)
{
    ywb_bool_t success = false;
    OBJ* obj = NULL;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    success = AddOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(success, true);
    obj = GetOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(obj != NULL, true);
}

TEST_F(LogicalConfigTest, AddOBJ_SubPoolNotExist_FAIL)
{
    ywb_bool_t success = false;

    success = AddOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(success, false);
}

TEST_F(LogicalConfigTest, AddOBJ_DiskNotExist_FAIL)
{
    ywb_bool_t success = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    success = AddOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(success, false);
}

TEST_F(LogicalConfigTest, GetSubPool_SUCCESS)
{
    SubPool* subpool = NULL;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    subpool = GetSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(subpool != NULL, true);
}

TEST_F(LogicalConfigTest, GetSubPool_YFS_ENOENT)
{
    SubPool* subpool = NULL;

    subpool = GetSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(subpool == NULL, true);
}

TEST_F(LogicalConfigTest, GetDisk_SUCCESS)
{
    Disk* disk = NULL;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    disk = GetDisk(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(disk != NULL, true);
}

TEST_F(LogicalConfigTest, GetDisk_YFS_ENOENT)
{
    Disk* disk = NULL;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    disk = GetDisk(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(disk == NULL, true);
}

TEST_F(LogicalConfigTest, GetOBJ_SUCCESS)
{
    OBJ* obj = NULL;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);
    obj = GetOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(obj != NULL, true);
}

TEST_F(LogicalConfigTest, GetOBJ_YFS_ENOENT)
{
    OBJ* obj = NULL;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);
    obj = GetOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE + 1);
    ASSERT_EQ(obj == NULL, true);
}

TEST_F(LogicalConfigTest, RemoveSubPool_SubPoolDoExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ret = RemoveSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, RemoveSubPool_SubPoolNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = RemoveSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, RemoveDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    ret = RemoveDisk(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, RemoveDisk_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    ret = RemoveDisk(Constant::DISK_ID + 1, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(LogicalConfigTest, RemoveOBJ_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);
    ret = RemoveOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, RemoveOBJ_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);
    ret = RemoveOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE + 1);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(LogicalConfigTest, AddDeletedSubPool_SubPoolNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = AddDeletedSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(LogicalConfigTest, AddDeletedSubPool_SubPoolEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ret = AddDeletedSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, AddDeletedSubPool_SubPoolDiskEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    ret = AddDeletedSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, AddDeletedSubPool_SUCCESS)
{
    ywb_status_t status = YWB_SUCCESS;
    ywb_bool_t deleted = false;
    ywb_uint32_t loop = 0;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE,
            6);
    AddDisk(DiskBaseProp::DCT_sata,
            Constant::DISK_ID + 1, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID + 1, Constant::DEFAULT_INODE,
            6);

    status = AddDeletedSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(status, YWB_SUCCESS);
    deleted = CheckSubPoolDeleted(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(deleted, true);
    deleted = CheckDiskDeleted(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(deleted, true);
    deleted = CheckDiskDeleted(Constant::DISK_ID + 1, Constant::SUBPOOL_ID);
    ASSERT_EQ(deleted, true);
    for(loop = 0; loop < 6; loop++)
    {
        deleted = CheckOBJDeleted(Constant::DISK_ID, Constant::DEFAULT_INODE + loop);
        ASSERT_EQ(deleted, true);
        deleted = CheckOBJDeleted(Constant::DISK_ID + 1, Constant::DEFAULT_INODE + loop);
        ASSERT_EQ(deleted, true);
    }
}

TEST_F(LogicalConfigTest, AddDeletedDisk_DiskNotExist_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;

    ret = AddDeletedDisk(Constant::DISK_ID ,Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(LogicalConfigTest, AddDeletedDisk_DiskEmpty_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    ret = AddDeletedDisk(Constant::DISK_ID ,Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, AddDeletedDisk_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t loop = 0;
    ywb_bool_t deleted = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE,
            6);
    ret = AddDeletedDisk(Constant::DISK_ID ,Constant::SUBPOOL_ID);
    ASSERT_EQ(ret, YWB_SUCCESS);

    for(loop = 0; loop < 6; loop++)
    {
        deleted = CheckOBJDeleted(Constant::DISK_ID, Constant::DEFAULT_INODE + loop);
        ASSERT_EQ(deleted, true);
    }
}

TEST_F(LogicalConfigTest, AddDeletedOB_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t deleted = false;
    ywb_uint32_t loop = 0;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE,
            6);

    for(loop = 0; loop < 6; loop++)
    {
        ret = AddDeletedOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE + loop);
        ASSERT_EQ(ret, YWB_SUCCESS);
        deleted = CheckOBJDeleted(Constant::DISK_ID, Constant::DEFAULT_INODE + loop);
        ASSERT_EQ(deleted, true);
    }
}

TEST_F(LogicalConfigTest, AddDeletedOBJ_OBJNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t deleted = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    ret = AddDeletedOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(ret, YWB_SUCCESS);
    deleted = CheckOBJDeleted(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(deleted, true);
}

TEST_F(LogicalConfigTest, AddDeletedOBJ_DiskNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t deleted = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ret = AddDeletedOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(ret, YWB_SUCCESS);
    deleted = CheckOBJDeleted(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(deleted, true);
}

TEST_F(LogicalConfigTest, AddDeletedOBJ_SubPoolNotExist_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t deleted = false;

    ret = AddDeletedOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(ret, YWB_SUCCESS);
    deleted = CheckOBJDeleted(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(deleted, true);
}

TEST_F(LogicalConfigTest, CheckSubPoolExistence_SubPoolDeleted_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE,
            6);

    AddDeletedSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    exist = CheckSubPoolExistence(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckSubPoolExistence_SubPoolNotInConfig_FALSE)
{
    ywb_bool_t exist = false;

    exist = CheckSubPoolExistence(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckSubPoolExistence_SubPoolInConfig_TRUE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    exist = CheckSubPoolExistence(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(exist, true);
}

TEST_F(LogicalConfigTest, CheckDiskExistence_SubPoolDeleted_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);

    AddDeletedSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    exist = CheckDiskExistence(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckDiskExistence_DiskDeleted_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);

    AddDeletedDisk(Constant::DISK_ID, Constant::SUBPOOL_ID);
    exist = CheckDiskExistence(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckDiskExistence_SubPoolNotInConfig_FALSE)
{
    ywb_bool_t exist = false;

    exist = CheckDiskExistence(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckDiskExistence_DiskNotInConfig_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    exist = CheckDiskExistence(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckDiskExistence_DiskInConfig_TRUE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);

    exist = CheckDiskExistence(Constant::DISK_ID, Constant::SUBPOOL_ID);
    ASSERT_EQ(exist, true);
}

TEST_F(LogicalConfigTest, CheckOBJExistence_SubPoolDeleted_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);

    AddDeletedSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    exist = CheckOBJExistence(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckOBJExistence_DiskDeleted_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);

    AddDeletedDisk(Constant::DISK_ID, Constant::SUBPOOL_ID);
    exist = CheckOBJExistence(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckOBJExistence_OBJDeleted_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);

    AddDeletedOBJ(Constant::DISK_ID, Constant::DEFAULT_INODE);
    exist = CheckOBJExistence(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckOBJExistence_SubPoolNotInConfig_FALSE)
{
    ywb_bool_t exist = false;

    exist = CheckOBJExistence(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckOBJExistence_DiskNotInConfig_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);

    exist = CheckOBJExistence(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckOBJExistence_OBJNotInConfig_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);

    exist = CheckOBJExistence(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(exist, false);
}

TEST_F(LogicalConfigTest, CheckOBJExistence_OBJInConfig_FALSE)
{
    ywb_bool_t exist = false;

    AddSubPool(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDisk(DiskBaseProp::DCT_ent,
            Constant::DISK_ID, Constant::SUBPOOL_ID);
    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE, 1);

    exist = CheckOBJExistence(Constant::DISK_ID, Constant::DEFAULT_INODE);
    ASSERT_EQ(exist, true);
}

TEST_F(LogicalConfigTest, GetFirstNextSubPool_LogicalConfigEmpty_YFS_ENOENT)
{
    ywb_status_t ret = YWB_SUCCESS;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    SubPoolKey subpoolkey;

    config = GetLogicalConfig();
    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    ASSERT_EQ(ret, YFS_ENOENT);
}

TEST_F(LogicalConfigTest, GetFirstNextSubPool_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;
    LogicalConfig* config = NULL;
    SubPool* subpool = NULL;
    SubPoolKey subpoolkey;

    config = GetLogicalConfig();
    AddSubPools(Constant::OSS_ID, Constant::POOL_ID,
            Constant::SUBPOOL_ID, 2);
    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(subpoolkey.GetSubPoolId(), (ywb_uint32_t)Constant::SUBPOOL_ID);
    ret = config->GetNextSubPool(&subpool, &subpoolkey);
    ASSERT_EQ(ret, YWB_SUCCESS);
    ASSERT_EQ(subpoolkey.GetSubPoolId(), (ywb_uint32_t)(Constant::SUBPOOL_ID + 1));
}

TEST_F(LogicalConfigTest, GetKeysAndSubPools_LogicalConfigEmpty_YFS_ENOENT)
{
    ywb_uint32_t keysnum = 0;
    LogicalConfig* config = NULL;
    list<SubPoolKey> subpoolkeys;
    list<SubPool*> subpools;

    config = GetLogicalConfig();
    keysnum = config->GetKeysAndSubPools(subpoolkeys, subpools);
    ASSERT_EQ(keysnum, 0);
}

TEST_F(LogicalConfigTest, GetKeysAndSubPools_SUCCESS)
{
    ywb_uint32_t keysnum = 0;
    ywb_uint32_t loop = 0;
    LogicalConfig* config = NULL;
    list<SubPoolKey> subpoolkeys;
    list<SubPool*> subpools;
    list<SubPoolKey>::iterator keyiter;
    list<SubPool*>::iterator subpooliter;

    config = GetLogicalConfig();
    AddSubPools(Constant::OSS_ID, Constant::POOL_ID,
            Constant::SUBPOOL_ID, 2);
    keysnum = config->GetKeysAndSubPools(subpoolkeys, subpools);
    ASSERT_EQ(keysnum, 2);

    keyiter = subpoolkeys.begin();
    subpooliter = subpools.begin();
    while(keyiter != subpoolkeys.end())
    {
        ASSERT_EQ(keyiter->GetSubPoolId(), Constant::SUBPOOL_ID + loop);
        ASSERT_EQ(*subpooliter != NULL, true);
        loop++;
        keyiter++;
        subpooliter++;
    }
}

TEST_F(LogicalConfigTest, Reset_SUCCESS)
{
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    AddSubPools(Constant::OSS_ID, Constant::POOL_ID,
            Constant::SUBPOOL_ID, 2);
    AddDisks(DiskBaseProp::DCT_ssd, Constant::DISK_ID,
            Constant::SUBPOOL_ID, 2);
    AddDisks(DiskBaseProp::DCT_ent, Constant::DISK_ID + 2,
            Constant::SUBPOOL_ID + 1, 2);

    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE,
            6);
    AddOBJs(Constant::DISK_ID + 2, Constant::DEFAULT_INODE,
            6);

    AddDeletedSubPool(Constant::OSS_ID, Constant::POOL_ID, Constant::SUBPOOL_ID);
    AddDeletedDisk(Constant::DISK_ID + 1, Constant::SUBPOOL_ID + 1);
    config->Reset();

    ASSERT_EQ(config->DiskId2KeyEmpty(), true);
    ASSERT_EQ(config->SubPoolId2KeyEmpty(), true);
    ASSERT_EQ(config->SubPoolsEmpty(), true);
    ASSERT_EQ(config->DeletedSubPoolsEmpty(), true);
    ASSERT_EQ(config->DeletedDisksEmpty(), true);
    ASSERT_EQ(config->DeletedOBJsEmpty(), true);
}

TEST_F(LogicalConfigTest, Consolidate_SUCCESS)
{
    LogicalConfig* config = NULL;

    config = GetLogicalConfig();
    AddSubPools(Constant::OSS_ID, Constant::POOL_ID,
            Constant::SUBPOOL_ID, 2);
    AddDisks(DiskBaseProp::DCT_ssd, Constant::DISK_ID,
            Constant::SUBPOOL_ID, 2);
    AddDisks(DiskBaseProp::DCT_ent, Constant::DISK_ID + 2,
            Constant::SUBPOOL_ID + 1, 2);

    AddOBJs(Constant::DISK_ID, Constant::DEFAULT_INODE,
            6);
    AddOBJs(Constant::DISK_ID + 2, Constant::DEFAULT_INODE,
            6);

    AddDeletedSubPool(Constant::OSS_ID, Constant::POOL_ID, Constant::SUBPOOL_ID + 1);
    config->Consolidate();

    ASSERT_EQ(config->DeletedSubPoolsEmpty(), true);
    ASSERT_EQ(config->DeletedDisksEmpty(), true);
    ASSERT_EQ(config->DeletedOBJsEmpty(), true);
    ASSERT_EQ(CheckSubPoolExistence(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID), true);
    ASSERT_EQ(CheckSubPoolExistence(Constant::OSS_ID,
            Constant::POOL_ID, Constant::SUBPOOL_ID + 1), false);
    ASSERT_EQ(CheckDiskExistence(Constant::DISK_ID, Constant::SUBPOOL_ID), true);
    ASSERT_EQ(CheckDiskExistence(Constant::DISK_ID + 1, Constant::SUBPOOL_ID), true);
    ASSERT_EQ(CheckDiskExistence(Constant::DISK_ID + 2, Constant::SUBPOOL_ID + 1), false);
    ASSERT_EQ(CheckDiskExistence(Constant::DISK_ID + 3, Constant::SUBPOOL_ID + 1), false);
    ASSERT_EQ(CheckOBJExistence(Constant::DISK_ID, Constant::DEFAULT_INODE), true);
    ASSERT_EQ(CheckOBJExistence(Constant::DISK_ID,
            Constant::DEFAULT_INODE + 6 - 1), true);
    ASSERT_EQ(CheckOBJExistence(Constant::DISK_ID + 2, Constant::DEFAULT_INODE), false);
    ASSERT_EQ(CheckOBJExistence(Constant::DISK_ID + 2,
            Constant::DEFAULT_INODE + 6 - 1), false);
}

TEST_F(LogicalConfigTest, ResolveLogicalConfig_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    PrepareResolveLogicalConfigEnv();
    BuildCollectLogicalConfigResponse();
    ret = ResolveLogicalConfig();
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = CheckResolveLogicalConfigResult();
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, ResolveDeletedOBJs_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    PrepareLogicalConfigForDeleteMessage();
    BuildReportDeleteOBJMessage();
    ret = ResolveDeletedOBJs();
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = CheckResolveDeletedOBJsResult();
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, ResolveDeletedDisks_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    PrepareLogicalConfigForDeleteMessage();
    BuildReportDeleteDiskMessage();
    ret = ResolveDeletedDisks();
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = CheckResolveDeletedDisksResult();
    ASSERT_EQ(ret, YWB_SUCCESS);
}

TEST_F(LogicalConfigTest, ResolveDeletedSubPools_SUCCESS)
{
    ywb_status_t ret = YWB_SUCCESS;

    PrepareLogicalConfigForDeleteMessage();
    BuildReportDeleteSubPoolMessage();
    ret = ResolveDeletedSubPools();
    ASSERT_EQ(ret, YWB_SUCCESS);
    ret = CheckResolveDeletedSubPoolsResult();
    ASSERT_EQ(ret, YWB_SUCCESS);
}

/* vim:set ts=4 sw=4 expandtab */
