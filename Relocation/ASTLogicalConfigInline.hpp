#ifndef __AST_LOGICAL_CONFIG_INLINE_HPP__
#define __AST_LOGICAL_CONFIG_INLINE_HPP__

#include "AST/ASTLogicalConfig.hpp"

ywb_uint64_t OBJKey::GetChunkId()
{
    return mChunkId;
}

ywb_uint64_t OBJKey::GetInodeId()
{
    return mInodeId;
}

ywb_uint32_t OBJKey::GetChunkIndex()
{
    return mChunkIndex;
}

ywb_uint32_t OBJKey::GetChunkVersion()
{
    return mChunkVersion;
}

ywb_uint64_t OBJKey::GetStorageId()
{
    return mStorageId;
}

void OBJKey::SetChunkId(ywb_uint64_t id)
{
    mChunkId = id;
}

void OBJKey::SetInodeId(ywb_uint64_t id)
{
    mInodeId = id;
}

void OBJKey::SetChunkIndex(ywb_uint32_t index)
{
    mChunkIndex = index;
}

void OBJKey::SetChunkVersion(ywb_uint32_t ver)
{
    mChunkVersion = ver;
}

void OBJKey::SetStorageId(ywb_uint64_t id)
{
    mStorageId = id;
}

void OBJ::GetOBJKey(OBJKey** key)
{
    *key = &mKey;
}

void OBJ::SetOBJKey(OBJKey& key)
{
    mKey.SetChunkId(key.GetChunkId());
    mKey.SetInodeId(key.GetInodeId());
    mKey.SetChunkIndex(key.GetChunkIndex());
    mKey.SetChunkVersion(key.GetChunkVersion());
    mKey.SetStorageId(key.GetStorageId());
}

//void
//OBJ::GetOBJIOConst(const OBJIO** io)
//{
//    *io = mObjIO;
//}

void OBJ::GetOBJIO(OBJIO** io)
{
    *io = mObjIO;
}

void OBJ::SetOBJIO(OBJIO*& io)
{
    YWB_ASSERT(mObjIO == NULL);
    mObjIO = io;
}

void OBJ::SetFlagComb(ywb_uint32_t flag)
{
    mFlag.SetFlagComb(flag);
}

void OBJ::SetFlagUniq(ywb_uint32_t flag)
{
    mFlag.SetFlagUniq(flag);
}

void OBJ::ClearFlag(ywb_uint32_t flag)
{
    mFlag.ClearFlag(flag);
}

bool OBJ::TestFlag(ywb_uint32_t flag)
{
    return mFlag.TestFlag(flag);
}

void OBJ::IncRef()
{
    fs_atomic_inc(&mRef);
}

void OBJ::DecRef()
{
    if(fs_atomic_dec_and_test(&mRef))
    {
        delete this;
    }
}

ywb_uint64_t DiskKey::GetDiskId()
{
    return mDiskId;
}

ywb_uint32_t DiskKey::GetSubPoolId()
{
    return mSubPoolId;
}

void DiskKey::SetDiskId(ywb_uint64_t id)
{
    mDiskId = id;
}

void DiskKey::SetSubPoolId(ywb_uint32_t id)
{
    mSubPoolId = id;
}

ywb_uint32_t DiskBaseProp::GetRaidType()
{
    return mRaid;
}

ywb_uint32_t DiskBaseProp::GetRaidWidth()
{
    return mWidth;
}

ywb_uint32_t DiskBaseProp::GetDiskClass()
{
    return mDiskClass;
}

ywb_uint32_t DiskBaseProp::GetDiskRPM()
{
    return mDiskRpm;
}

ywb_uint32_t DiskBaseProp::GetDiskCap()
{
    return mDiskCap;
}

void DiskBaseProp::SetRaidType(ywb_uint32_t raid)
{
    mRaid = raid;
}

void DiskBaseProp::SetRaidWidth(ywb_uint32_t width)
{
    mWidth = width;
}

void DiskBaseProp::SetDiskClass(ywb_uint32_t type)
{
    mDiskClass = type;
}

void DiskBaseProp::SetDiskRPM(ywb_uint32_t rpm)
{
    mDiskRpm = rpm;
}

void DiskBaseProp::SetDiskCap(ywb_uint32_t cap)
{
    mDiskCap = cap;
}

ywb_uint32_t Disk::GetTier()
{
    return mTier;
}

void Disk::GetDiskIO(DiskIO** io)
{
    *io = mDiskIO;
}

void Disk::SetBaseProp(DiskBaseProp& prop)
{
    mProp = prop;
}

void Disk::SetTier(ywb_uint32_t tier)
{
    mTier = tier;
}

void Disk::SetDiskIO(DiskIO*& io)
{
    YWB_ASSERT(mDiskIO == NULL);
    mDiskIO = io;
}

void Disk::SetFlagComb(ywb_uint32_t flag)
{
    mFlag.SetFlagComb(flag);
}

void Disk::SetFlagUniq(ywb_uint32_t flag)
{
    mFlag.SetFlagUniq(flag);
}

void Disk::ClearFlag(ywb_uint32_t flag)
{
    mFlag.ClearFlag(flag);
}

bool Disk::TestFlag(ywb_uint32_t flag)
{
    return mFlag.TestFlag(flag);
}

//void Disk::IncRef()
//{
//    fs_atomic_inc(&mRef);
//}
//
//void Disk::DecRef()
//{
//    if(fs_atomic_dec_and_test(&mRef))
//    {
//        delete this;
//    }
//}

void Disk::PutOBJ(OBJVal* obj)
{
    obj->DecRef();
}

ywb_uint32_t Disk::GetOBJCnt()
{
    return mObjsMap.size();
}

ywb_uint32_t Tier::GetType()
{
    return mType;
}

void
Tier::SetType(ywb_uint32_t val)
{
    mType = val;
}

void Tier::SetFlagComb(ywb_uint32_t flag)
{
    mFlag.SetFlagComb(flag);
}

void Tier::SetFlagUniq(ywb_uint32_t flag)
{
    mFlag.SetFlagUniq(flag);
}

void Tier::ClearFlag(ywb_uint32_t flag)
{
    mFlag.ClearFlag(flag);
}

bool Tier::TestFlag(ywb_uint32_t flag)
{
    return mFlag.TestFlag(flag);
}

void Tier::IncRef()
{
    fs_atomic_inc(&mRef);
}

void Tier::DecRef()
{
    if(fs_atomic_dec_and_test(&mRef))
    {
        delete this;
    }
}

void Tier::PutOBJ(OBJVal* obj)
{
    obj->DecRef();
}

void Tier::PutDisk(Disk* disk)
{
    disk->DecRef();
}

ywb_uint32_t SubPoolKey::GetOss()
{
    return mOss;
}

ywb_uint32_t SubPoolKey::GetPoolId()
{
    return mPoolId;
}

ywb_uint32_t SubPoolKey::GetSubPoolId()
{
    return mSubPoolId;
}

void SubPoolKey::SetOss(ywb_uint32_t id)
{
    mOss = id;
}

void SubPoolKey::SetPoolId(ywb_uint32_t id)
{
    mPoolId = id;
}

void SubPoolKey::SetSubPoolId(ywb_uint32_t id)
{
    mSubPoolId = id;
}

void SubPool::GetSubPoolKey(SubPoolKey** key)
{
    *key = &mKey;
}

//ywb_uint32_t
//SubPool::GetLastTierComb()
//{
//    return mLastTierComb;
//}

ywb_uint32_t SubPool::GetCurTierComb()
{
    ywb_uint32_t ret = 0;

    mTiersLock.Lock();
    ret = mCurTierComb;
    mTiersLock.Unlock();

    return ret;
}

void SubPool::SetSubPoolKey(SubPoolKey& key)
{
    mKey = key;
}

//void
//SubPool::SetLastTierComb(ywb_uint32_t val)
//{
//    mLastTierComb = val;
//}

void SubPool::SetCurTierComb(ywb_uint32_t val)
{
    mTiersLock.Lock();
    mCurTierComb = val;
    mTiersLock.Unlock();
}

void SubPool::SetFlagComb(ywb_uint32_t flag)
{
    mFlag.SetFlagComb(flag);
}

void SubPool::SetFlagUniq(ywb_uint32_t flag)
{
    mFlag.SetFlagUniq(flag);
}

void SubPool::ClearFlag(ywb_uint32_t flag)
{
    mFlag.ClearFlag(flag);
}

bool SubPool::TestFlag(ywb_uint32_t flag)
{
    return mFlag.TestFlag(flag);
}

//void SubPool::IncRef()
//{
//    fs_atomic_inc(&mRef);
//}
//
//void SubPool::DecRef()
//{
//    if(fs_atomic_dec_and_test(&mRef))
//    {
//        delete this;
//    }
//}

void SubPool::PutOBJ(OBJVal* obj)
{
    obj->DecRef();
}

void SubPool::PutDisk(Disk* disk)
{
    disk->DecRef();
}

void SubPool::PutTier(Tier* tier)
{
    tier->DecRef();
}

void LogicalConfig::GetAST(AST** ast)
{
    *ast = mAst;
}

void LogicalConfig::PutOBJ(OBJVal* obj)
{
    obj->DecRef();
}

void LogicalConfig::PutDisk(Disk* disk)
{
    disk->DecRef();
}

void LogicalConfig::PutTier(Tier* tier)
{
    tier->DecRef();
}

void LogicalConfig::PutSubPool(SubPool* subpool)
{
    subpool->DecRef();
}

ywb_bool_t LogicalConfig::DiskId2KeyEmpty()
{
    return mDiskId2Key.empty();
}

ywb_bool_t LogicalConfig::SubPoolId2KeyEmpty()
{
    return mSubPoolId2Key.empty();
}

ywb_bool_t LogicalConfig::SubPoolsEmpty()
{
    return mSubPools.empty();
}

ywb_bool_t LogicalConfig::DeletedSubPoolsEmpty()
{
    return mDeletedSubPools.empty();
}

ywb_bool_t LogicalConfig::DeletedDisksEmpty()
{
    return mDeletedDisks.empty();
}

ywb_bool_t LogicalConfig::DeletedOBJsEmpty()
{
    return mDeletedOBJs.empty();
}

//void
//LogicalConfig::SetCollectLogicalConfigResponse(
//        CollectLogicalConfigResponse*& response)
//{
//    mCollectConfigResp = response;
//}
//
//void
//LogicalConfig::SetDeleteOBJMessage(ReportDeleteOBJMessage*& msg)
//{
//    mDeleteOBJMsg = msg;
//}
//
//void
//LogicalConfig::SetDeleteDiskMessage(ReportDeleteDiskMessage*& msg)
//{
//    mDeleteDiskMsg = msg;
//}
//
//void
//LogicalConfig::SetDeleteSubPoolMessage(ReportDeleteSubPoolMessage*& msg)
//{
//    mDeleteSubPoolMsg = msg;
//}

#endif

/* vim:set ts=4 sw=4 expandtab */
