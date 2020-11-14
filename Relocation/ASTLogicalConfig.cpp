#include "AST/AST.hpp"
#include "OSSCommon/Logger.hpp"
#include "AST/ASTIO.hpp"
#include "AST/MessageImpl.hpp"
#include "AST/MessageExpress.hpp"
#include "AST/ASTRelocation.hpp"
#include "AST/ASTLogicalConfig.hpp"

bool
OBJKeyCmp::operator()(const OBJKey& key1, const OBJKey& key2) const
{
    if(key1.mStorageId != key2.mStorageId)
        return key1.mStorageId < key2.mStorageId;
    else if(key1.mInodeId != key2.mInodeId)
        return key1.mInodeId < key2.mInodeId;
    else if(key1.mChunkId != key2.mChunkId)
        return key1.mChunkId < key2.mChunkId;
    else if(key1.mChunkIndex != key2.mChunkIndex)
        return key1.mChunkIndex < key2.mChunkIndex;
    else if(key1.mChunkVersion != key2.mChunkVersion)
        return key1.mChunkVersion < key2.mChunkVersion;
    else
        return false;
}

bool
DiskKeyCmp::operator()(const DiskKey& key1, const DiskKey& key2) const
{
    if(key1.mSubPoolId != key2.mSubPoolId)
        return key1.mSubPoolId < key2.mSubPoolId;
    else if(key1.mDiskId != key2.mDiskId)
        return key1.mDiskId < key2.mDiskId;
    else
        return false;
}

//OBJ::OBJ() : mObjIO(NULL), mRef(0) {}
OBJ::OBJ(OBJKey& key) : mKey(key), mObjIO(NULL), mRef(0)
{
    mFlag.Reset();
}

OBJ::~OBJ()
{
    if(mObjIO)
    {
        delete mObjIO;
        mObjIO = NULL;
    }
}

void OBJ::Dump(ostringstream* ostr)
{
    *ostr << "OBJKey: [" << mKey.GetStorageId()
            << ", " << mKey.GetInodeId()
            << ", " << mKey.GetChunkId()
            << ", " << mKey.GetChunkIndex()
            << ", " << mKey.GetChunkVersion()
            << "]\n";
    mObjIO->Dump(ostr);
}

bool
DiskBaseProp::MatchExactly(const DiskBaseProp& prop)
{
    return (mRaid == prop.mRaid) && (mWidth == prop.mWidth) &&
            (mDiskClass == prop.mDiskClass) && (mDiskRpm == prop.mDiskRpm)
            && (mDiskCap == prop.mDiskCap);
}

bool
DiskBaseProp::MatchFuzzy(const DiskBaseProp& prop)
{
    return (mDiskClass == prop.mDiskClass) &&
            (mDiskRpm == prop.mDiskRpm) &&
            (mRaid == prop.mRaid) &&
            (mWidth == prop.mWidth) &&
            (mDiskCap != prop.mDiskCap);
}

ywb_uint32_t
DiskBaseProp::DetermineDiskTier()
{
    ywb_uint32_t tiertype = 0;
    switch(mDiskClass)
    {
    case DiskBaseProp::DCT_ssd:
        tiertype = Tier::TIER_0;
        break;
    case DiskBaseProp::DCT_ent:
        tiertype = Tier::TIER_1;
        break;
    case DiskBaseProp::DCT_sata:
        tiertype = Tier::TIER_2;
        break;
    default:
        tiertype = Tier::TIER_cnt;
        break;
    }

    return tiertype;
}

bool
DiskBasePropCmp::operator()(const DiskBaseProp& prop1,
        const DiskBaseProp& prop2) const
{
    if(prop1.mDiskClass != prop2.mDiskClass)
    {
        return prop1.mDiskClass < prop2.mDiskClass;
    }
    else if(prop1.mDiskRpm != prop2.mDiskRpm)
    {
        return prop1.mDiskRpm < prop2.mDiskRpm;
    }
    else if(prop1.mRaid != prop2.mRaid)
    {
        return prop1.mRaid < prop2.mRaid;
    }
    else if(prop1.mWidth != prop2.mWidth)
    {
        return prop1.mWidth < prop2.mWidth;
    }
    else if(prop1.mDiskCap != prop2.mDiskCap)
    {
        return prop1.mDiskCap < prop2.mDiskCap;
    }
    else
    {
        return false;
    }
}

Disk::Disk() : mTier(Tier::TIER_cnt), mRef(0), mDiskIO(NULL)
{
    mFlag.SetFlag(Disk::DF_healthy);
}

Disk::Disk(DiskBaseProp& prop) : mProp(prop), mTier(Tier::TIER_cnt),
        mRef(0), mDiskIO(NULL)
{
    mFlag.SetFlag(Disk::DF_healthy);
}

Disk::~Disk()
{
    map<OBJKey, OBJVal*, OBJKeyCmp>::iterator iter;
    OBJVal* obj = NULL;

    iter = mObjsMap.begin();
    for(; iter != mObjsMap.end();)
    {
        obj = iter->second;
        mObjsMap.erase(iter++);
        obj->DecRef();
    }
    mObjsMap.clear();

    if(mDiskIO)
    {
        delete mDiskIO;
        mDiskIO = NULL;
    }
}

void Disk::IncRef()
{
    ast_log_trace("INCREF disk@" << this);
    fs_atomic_inc(&mRef);
}

void Disk::DecRef()
{
    ast_log_trace("DECREF disk@" << this);
    if(fs_atomic_dec_and_test(&mRef))
    {
        delete this;
    }
}

ywb_status_t
Disk::GetBaseProp(DiskBaseProp** prop)
{
    ywb_status_t ret = YWB_SUCCESS;

    *prop = &mProp;
    return ret;
}

ywb_status_t
Disk::AddOBJ(OBJKey& key, OBJVal* obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<OBJKey, OBJVal*, OBJKeyCmp>::iterator, bool> ret2;

    ret2 = mObjsMap.insert(std::make_pair(key, obj));
    if(ret2.second)
    {
        obj->IncRef();
        ast_log_trace("Add OBJ [" << key.GetStorageId()
                << ", " << key.GetInodeId()
                << ", " << key.GetChunkId()
                << ", " << key.GetChunkIndex()
                << ", " << key.GetChunkVersion()
                << "] into disk");
    }
    else
    {
        ast_log_trace("OBJ [" << key.GetStorageId()
                << ", " << key.GetInodeId()
                << ", " << key.GetChunkId()
                << ", " << key.GetChunkIndex()
                << ", " << key.GetChunkVersion()
                << "] already exist!");

        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t
Disk::GetOBJ(OBJKey& key, OBJVal** obj)
{
    map<OBJKey, OBJVal*, OBJKeyCmp>::iterator iter;

    iter = mObjsMap.find(key);
    if(mObjsMap.end() == iter)
    {
        *obj = NULL;
        return YFS_ENOENT;
    }
    else
    {
        *obj = iter->second;
        (*obj)->IncRef();
        return YWB_SUCCESS;
    }
}

//ywb_status_t
//Disk::RemoveOBJ(OBJKey& key)
//{
//    ywb_uint32_t ret = 0;
//
//    ret = mObjsMap.erase(key);
//    if(1 == ret)
//    {
//        return YWB_SUCCESS;
//    }
//    else
//    {
//        return YFS_ENOENT;
//    }
//}

ywb_status_t
Disk::RemoveOBJ(OBJKey& key)
{
    map<OBJKey, OBJVal*, OBJKeyCmp>::iterator iter;
    OBJVal* obj = NULL;

    ast_log_debug("Remove OBJ: [" << key.GetStorageId()
            << ", " << key.GetInodeId()
            << ", " << key.GetChunkId()
            << ", " << key.GetChunkIndex()
            << ", " << key.GetChunkVersion()
            << "] from disk");

    iter = mObjsMap.find(key);
    if(mObjsMap.end() == iter)
    {
        return YFS_ENOENT;
    }
    else
    {
        obj = iter->second;
        mObjsMap.erase(iter);
        PutOBJ(obj);
        return YWB_SUCCESS;
    }
}

ywb_status_t
Disk::GetFirstOBJ(OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    mIt = mObjsMap.begin();
    if(mIt != mObjsMap.end())
    {
        *obj = mIt->second;
        mIt++;
        (*obj)->IncRef();
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Disk::GetNextOBJ(OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mObjsMap.end() != mIt)
    {
        *obj = mIt->second;
        mIt++;
        (*obj)->IncRef();
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Disk::GetFirstOBJ(OBJVal** obj, OBJKey* objkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    mIt = mObjsMap.begin();
    if(mIt != mObjsMap.end())
    {
        *objkey = mIt->first;
        *obj = mIt->second;
        mIt++;
        (*obj)->IncRef();
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
Disk::GetNextOBJ(OBJVal** obj, OBJKey* objkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mObjsMap.end() != mIt)
    {
        *objkey = mIt->first;
        *obj = mIt->second;
        mIt++;
        (*obj)->IncRef();
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

/*
 * Currently, disk's tier is determined by disk class, but
 * it is possible to have a more reasonable determination.
 *
 * Currently, only take SSD/ENT/SATA into consideration
 **/
ywb_uint32_t
Disk::DetermineDiskTier()
{
    mTier = mProp.DetermineDiskTier();
    ast_log_trace("Disk tier decision: " << mTier);

    return mTier;
}

ywb_status_t Disk::SummarizeDiskIO()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskIO* diskio = NULL;
    OBJ* obj = NULL;
    OBJIO* objio = NULL;
    ywb_uint32_t stattype = IOStat::IOST_cnt;

    ast_log_debug("Summarize disk's IO");
    diskio = mDiskIO;
    /*disk IO not setup*/
    if(NULL == diskio)
    {
        diskio = new DiskIO();
        if(diskio != NULL)
        {
            diskio->SetDisk(this);
            mDiskIO = diskio;
        }
        else
        {
            ast_log_debug("Out of memory!");
            return YFS_EOUTOFMEMORY;
        }
    }

    /*FIXME: reset IO or not?*/
    mDiskIO->Reset();
    ret = GetFirstOBJ(&obj);
    while((YWB_SUCCESS == ret) && (obj != NULL))
    {
        obj->GetOBJIO(&objio);
        if(objio != NULL)
        {
            stattype = IOStat::IOST_raw;
            for(; stattype < IOStat::IOST_cnt; stattype++)
            {
                diskio->UpdateStatIOs(stattype, objio, true);
                diskio->UpdateStatBW(stattype, objio, true);
                diskio->UpdateStatRT(stattype, objio, true);
                diskio->UpdateIOsRange(stattype, objio);
                diskio->UpdateBWRange(stattype, objio);
                diskio->UpdateRTRange(stattype, objio);
            }
        }

        PutOBJ(obj);
        ret = GetNextOBJ(&obj);
    }

    /*Take it as success when comes here*/
    return YWB_SUCCESS;
}

ywb_uint32_t
Tier::GetDiskCnt()
{
    mDisksLock.Lock();
    if(mDisks.empty())
    {
        mDisksLock.Unlock();
        return 0;
    }
    else
    {
        mDisksLock.Unlock();
        return mDisks.size();
    }
}

ywb_uint32_t
Tier::GetOBJCnt()
{
    map<DiskKey, Disk*, DiskKeyCmp>::iterator iter;
    Disk* disk = NULL;
    ywb_uint32_t count = 0;

    mDisksLock.Lock();
    iter = mDisks.begin();
    for(; iter != mDisks.end(); iter++)
    {
        disk = iter->second;
        count += disk->GetOBJCnt();
    }
    mDisksLock.Unlock();

    return count;
}

ywb_status_t
Tier::AddDisk(DiskKey& key, Disk* disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<DiskKey, Disk*, DiskKeyCmp>::iterator, bool> ret2;

    mDisksLock.Lock();
    ret2 = mDisks.insert(std::make_pair(key, disk));
    ast_log_debug("Add disk [" << key.GetSubPoolId()
            << ", " << key.GetDiskId()
            << "] into tier " << GetType());
    if(ret2.second)
    {
        disk->IncRef();
    }
    else
    {
        ret = YFS_EEXIST;
    }
    mDisksLock.Unlock();

    return ret;
}

ywb_status_t
Tier::GetDisk(DiskKey& key, Disk** disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskKey, Disk*, DiskKeyCmp>::iterator iter;

    mDisksLock.Lock();
    iter = mDisks.find(key);
    if(mDisks.end() == iter)
    {
        *disk = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        *disk = iter->second;
        (*disk)->IncRef();
    }
    mDisksLock.Unlock();

    return ret;
}

ywb_status_t
Tier::RemoveDisk(DiskKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskKey, Disk*, DiskKeyCmp>::iterator iter;
    Disk* disk = NULL;

    ast_log_debug("Remove disk [" << key.GetSubPoolId()
            << ", " << key.GetDiskId()
            << "] from tier " << GetType());

    mDisksLock.Lock();
    iter = mDisks.find(key);
    if(iter != mDisks.end())
    {
        disk = iter->second;
        mDisks.erase(iter);
        PutDisk(disk);
    }
    else
    {
        ret = YFS_ENOENT;
    }
    mDisksLock.Unlock();

    return ret;
}

ywb_status_t
Tier::GetFirstOBJ(OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    Disk* disk = NULL;

    mDisksLock.Lock();
    mIt = mDisks.begin();
    while(mIt != mDisks.end())
    {
        disk = mIt->second;
        /*find the first disk having OBJs*/
        if(disk->GetOBJCnt() != 0)
        {
            break;
        }

        mIt++;
    }

    if(mIt != mDisks.end())
    {
        disk = mIt->second;
        ret = disk->GetFirstOBJ(obj);
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    mDisksLock.Unlock();

    return ret;
}

ywb_status_t
Tier::GetNextOBJ(OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    Disk* disk = NULL;

    mDisksLock.Lock();
    /*not reach the tier end*/
    if(mDisks.end() != mIt)
    {
        disk = mIt->second;
        ret = disk->GetNextOBJ(obj);
        /*reach the disk end*/
        if(!((YWB_SUCCESS == ret) && (*obj != NULL)))
        {
            mIt++;
            while(mIt != mDisks.end())
            {
                disk = mIt->second;
                /*find the first disk having OBJs*/
                if(disk->GetOBJCnt() != 0)
                {
                    break;
                }

                mIt++;
            }

            /*reach the tier end*/
            if(mDisks.end() == mIt)
            {
                *obj = NULL;
                ret = YFS_ENOENT;
            }
            else
            {
                disk = mIt->second;
                ret = disk->GetFirstOBJ(obj);
            }
        }
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    mDisksLock.Unlock();

    return ret;
}

ywb_status_t
Tier::GetFirstDisk(Disk** disk)
{
    ywb_status_t ret = YWB_SUCCESS;

    mDisksLock.Lock();
    mIt = mDisks.begin();
    if(mIt != mDisks.end())
    {
        *disk = mIt->second;
        (*disk)->IncRef();
        mIt++;
    }
    else
    {
        *disk = NULL;
        ret = YFS_ENOENT;
    }
    mDisksLock.Unlock();

    return ret;
}

ywb_status_t
Tier::GetNextDisk(Disk** disk)
{
    ywb_status_t ret = YWB_SUCCESS;

    mDisksLock.Lock();
    if(mIt != mDisks.end())
    {
        *disk = mIt->second;
        (*disk)->IncRef();
        mIt++;
    }
    else
    {
        *disk = NULL;
        ret = YFS_ENOENT;
    }
    mDisksLock.Unlock();

    return ret;
}

ywb_status_t
Tier::GetFirstDisk(Disk** disk, DiskKey* key)
{
    ywb_status_t ret = YWB_SUCCESS;

    mDisksLock.Lock();
    mIt = mDisks.begin();
    if(mIt != mDisks.end())
    {
        *key = mIt->first;
        *disk = mIt->second;
        (*disk)->IncRef();
        mIt++;
    }
    else
    {
        *disk = NULL;
        ret = YFS_ENOENT;
    }
    mDisksLock.Unlock();

    return ret;
}

ywb_status_t
Tier::GetNextDisk(Disk** disk, DiskKey* key)
{
    ywb_status_t ret = YWB_SUCCESS;

    mDisksLock.Lock();
    if(mIt != mDisks.end())
    {
        *key = mIt->first;
        *disk = mIt->second;
        (*disk)->IncRef();
        mIt++;
    }
    else
    {
        *disk = NULL;
        ret = YFS_ENOENT;
    }
    mDisksLock.Unlock();

    return ret;
}

ywb_uint32_t
Tier::GetKeysAndDisks(list<DiskKey>& diskkeys, list<Disk*>& disks)
{
    map<DiskKey, Disk*, DiskKeyCmp>::iterator iter;
    Disk* disk = NULL;

    mDisksLock.Lock();
    iter = mDisks.begin();
    for(; iter != mDisks.end(); iter++)
    {
        disk = iter->second;
        disk->IncRef();
        diskkeys.push_back(iter->first);
        disks.push_back(disk);
    }
    mDisksLock.Unlock();

    return diskkeys.size();
}

bool
SubPoolKeyCmp::operator()(const SubPoolKey& key1, const SubPoolKey& key2) const
{
    if(key1.mPoolId != key2.mPoolId)
        return key1.mPoolId < key2.mPoolId;
    else if(key1.mSubPoolId != key2.mSubPoolId)
        return key1.mSubPoolId < key2.mSubPoolId;
    else if(key1.mOss != key2.mOss)
        return key1.mOss < key2.mOss;
    else
        return false;
}

void SubPool::IncRef()
{
    ast_log_trace("INCREF subpool@" << this);
    fs_atomic_inc(&mRef);
}

void SubPool::DecRef()
{
    ast_log_trace("DECREF subpool@" << this);
    if(fs_atomic_dec_and_test(&mRef))
    {
        delete this;
    }
}

ywb_status_t
SubPool::AddTier(Tier* newtier)
{
    ywb_status_t ret = YWB_SUCCESS;

    mTiersLock.Lock();
    if(!mTiers.empty())
    {
        list<Tier*>::iterator iter;
        Tier* tier = NULL;

        iter = mTiers.begin();
        for(; iter != mTiers.end(); iter++)
        {
            tier = *iter;
            if(tier->GetType() == newtier->GetType())
            {
                ast_log_debug("Add tier " << newtier->GetType()
                        << " into subpool [" << mKey.GetOss()
                        << ", "  << mKey.GetPoolId()
                        << ", "  << mKey.GetSubPoolId()
                        << "] failed, tier already exist!");

                ret = YFS_EEXIST;
            }
        }
    }

    if(YWB_SUCCESS == ret)
    {
        newtier->IncRef();
        mTiers.push_back(newtier);
        AdjustTierComb(newtier->GetType(), true);
        ast_log_debug("Add tier " << newtier->GetType()
                << " into subpool [" << mKey.GetOss()
                << ", "  << mKey.GetPoolId()
                << ", "  << mKey.GetSubPoolId()
                << "] successfully");
    }
    mTiersLock.Unlock();

    return ret;
}

ywb_status_t
SubPool::GetTier(ywb_uint32_t type, Tier** tier)
{
    ywb_status_t ret = YFS_ENOENT;

    *tier = NULL;
    mTiersLock.Lock();
    if(!mTiers.empty())
    {
        list<Tier*>::iterator iter;
        Tier* cur = NULL;

        iter = mTiers.begin();
        for(; iter != mTiers.end(); iter++)
        {
            cur = *iter;
            if(cur->GetType() == type)
            {
                *tier = cur;
                cur->IncRef();
                ret = YWB_SUCCESS;
                break;
            }
        }
    }
    else
    {
        ret = YFS_ENOENT;
    }
    mTiersLock.Unlock();

    return ret;
}

//ywb_status_t
//SubPool::RemoveTier(ywb_uint32_t type)
//{
//    ywb_status_t ret = YFS_ENOENT;
//
//    ast_log_debug("Remove tier " << type
//            << " from subpool [" << mKey.GetOss()
//            << ", "  << mKey.GetPoolId()
//            << ", "  << mKey.GetSubPoolId() << "]");
//
//    if(!mTiers.empty())
//    {
//        list<Tier*>::iterator iter;
//        Tier* cur = NULL;
//
//        iter = mTiers.begin();
//        for(; iter != mTiers.end(); iter++)
//        {
//            cur = *iter;
//            if(cur->GetType() == type)
//            {
//                mTiers.erase(iter);
//                AdjustTierComb(type, false);
//                PutTier(cur);
//                ret = YWB_SUCCESS;
//                break;
//            }
//        }
//    }
//    else
//    {
//        ret = YFS_ENOENT;
//    }
//
//    return ret;
//}

ywb_status_t
SubPool::AdjustTierComb(ywb_uint32_t tiertype, bool add)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(tiertype >= Tier::TIER_cnt)
    {
        return YFS_EINVAL;
    }

    /*add tier into tier combination*/
    if(add)
    {
        ast_log_debug("Add tier " << tiertype
                << " into tier combination, current tier combination "
                << mCurTierComb);
        if(mCurTierComb == SubPool::TC_cnt)
        {
            mCurTierComb = (1 << tiertype);
        }
        else
        {
            /*@tiertype should not be contained in @mCurTierComb*/
            YWB_ASSERT(0 == (mCurTierComb & (1 << tiertype)));
            mCurTierComb = mCurTierComb + (1 << tiertype);
        }

        ast_log_debug("Tier combination after add tier " << tiertype
                << " is: " << mCurTierComb);
    }
    else
    {
        ast_log_debug("Remove tier " << tiertype
                << " from tier combination, current tier combination "
                << mCurTierComb);
        if(mCurTierComb != SubPool::TC_cnt)
        {
            /*@tiertype should be contained in @mCurTierComb*/
            YWB_ASSERT((mCurTierComb & (1 << tiertype)));
            mCurTierComb = mCurTierComb - (1 << tiertype);
        }

        ast_log_debug("Tier combination after remove tier " << tiertype
                << " is: " << mCurTierComb);
    }

    return ret;
}

ywb_status_t
SubPool::AddDisk(DiskKey& key, Disk* disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tiertype = 0;
    Tier* tier = NULL;

    ast_log_debug("Add disk [" << key.GetSubPoolId()
            << ", " << key.GetDiskId()
            << "] into subpool [" << mKey.GetOss()
            << ", " << mKey.GetPoolId()
            << ", " << mKey.GetSubPoolId() << "]");
    YWB_ASSERT(disk != NULL);
    YWB_ASSERT(key.GetSubPoolId() == mKey.GetSubPoolId());
    tiertype = disk->DetermineDiskTier();
    if(!((tiertype >= Tier::TIER_0) && (tiertype < Tier::TIER_cnt)))
    {
        ast_log_debug("Could not determine disk's tier");
        return YFS_EINVAL;
    }

    ret = GetTier(tiertype, &tier);
    /*find the tier*/
    if(ret != YWB_SUCCESS)
    {
        tier = new Tier(tiertype);
        if(NULL == tier)
        {
            ast_log_debug("Out of memory!");
            return YFS_EOUTOFMEMORY;
        }

        ret = AddTier(tier);
        YWB_ASSERT(YWB_SUCCESS == ret);
        ret = tier->AddDisk(key, disk);
    }
    else
    {
        ret = tier->AddDisk(key, disk);
        PutTier(tier);
    }

    return ret;
}

ywb_status_t
SubPool::GetDisk(DiskKey& key, Disk** disk)
{
    ywb_status_t ret = YFS_ENOENT;
    list<Tier*>::iterator iter;
    Tier* tier = NULL;

    *disk = NULL;
    if(key.GetSubPoolId() == mKey.GetSubPoolId())
    {
        mTiersLock.Lock();
        iter = mTiers.begin();
        for(; iter != mTiers.end(); iter++)
        {
            tier = *iter;
            ret = tier->GetDisk(key, disk);
            if((YWB_SUCCESS == ret) && (disk != NULL))
            {
                break;
            }
        }
        mTiersLock.Unlock();
    }

    return ret;
}

ywb_status_t
SubPool::RemoveDisk(DiskKey& key)
{
    ywb_status_t ret = YFS_ENOENT;
    list<Tier*>::iterator iter;
    Tier* tier = NULL;

    YWB_ASSERT(key.GetSubPoolId() == mKey.GetSubPoolId());
    ast_log_debug("Remove disk [" << key.GetSubPoolId()
            << ", " << key.GetDiskId()
            << "] from subpool [" << mKey.GetOss()
            << ", " << mKey.GetPoolId()
            << ", " << mKey.GetSubPoolId() << "]");

    mTiersLock.Lock();
    iter = mTiers.begin();
    for(; iter != mTiers.end(); iter++)
    {
        tier = *iter;
        ret = tier->RemoveDisk(key);
        if(YWB_SUCCESS == ret)
        {
            if(0 == tier->GetDiskCnt())
            {
                mTiers.erase(iter);
                AdjustTierComb(tier->GetType(), false);
                PutTier(tier);
            }

            break;
        }
    }
    mTiersLock.Unlock();

    return ret;
}


ywb_status_t
SubPool::AddOBJ(OBJKey& key, OBJVal* obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Disk* disk = NULL;

    ast_log_debug("Add OBJ [" << key.GetStorageId()
            << ", " << key.GetInodeId()
            << ", " << key.GetChunkId()
            << ", " << key.GetChunkIndex()
            << ", " << key.GetChunkVersion()
            << "] into subpool [" << mKey.GetOss()
            << ", " << mKey.GetPoolId()
            << ", " << mKey.GetSubPoolId() << "]");

    diskkey.SetDiskId(key.GetStorageId());
    diskkey.SetSubPoolId(mKey.GetSubPoolId());

    ret = GetDisk(diskkey, &disk);
    /*find the disk*/
    if(YWB_SUCCESS == ret)
    {
        ret = disk->AddOBJ(key, obj);
        PutDisk(disk);
    }

    return ret;
}

ywb_status_t
SubPool::GetOBJ(OBJKey& key, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Disk* disk = NULL;

    diskkey.SetDiskId(key.GetStorageId());
    diskkey.SetSubPoolId(mKey.GetSubPoolId());

    ret = GetDisk(diskkey, &disk);
    /*find the disk*/
    if(YWB_SUCCESS == ret)
    {
        ret = disk->GetOBJ(key, obj);
        PutDisk(disk);
    }

    return ret;
}

ywb_status_t
SubPool::RemoveOBJ(OBJKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Disk* disk = NULL;

    ast_log_debug("Remove OBJ [" << key.GetStorageId()
            << ", " << key.GetInodeId()
            << ", " << key.GetChunkId()
            << ", " << key.GetChunkIndex()
            << ", " << key.GetChunkVersion()
            << "] from subpool [" << mKey.GetOss()
            << ", " << mKey.GetPoolId()
            << ", " << mKey.GetSubPoolId() << "]");

    diskkey.SetDiskId(key.GetStorageId());
    diskkey.SetSubPoolId(mKey.GetSubPoolId());

    ret = GetDisk(diskkey, &disk);
    /*find the disk*/
    if(YWB_SUCCESS == ret)
    {
        ret = disk->RemoveOBJ(key);
        PutDisk(disk);
    }

    return ret;
}

ywb_status_t
SubPool::SummarizeDiskIO()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tiertype = Tier::TIER_0;
    Tier* tier = NULL;
    Disk* disk = NULL;

    ast_log_debug("Summarize subpool wise disks' IO for subpool: ["
            << mKey.GetOss() << ", " << mKey.GetPoolId()
            << ", " << mKey.GetSubPoolId() << "]");

    for(; tiertype < Tier::TIER_cnt; tiertype++)
    {
        ret = GetTier(tiertype, &tier);
        if((YWB_SUCCESS == ret) && (tier != NULL))
        {
            ret = tier->GetFirstDisk(&disk);
            while((YWB_SUCCESS == ret) && (disk != NULL))
            {
                ret = disk->SummarizeDiskIO();

                if((ret != YWB_SUCCESS))
                {
                    tier->PutDisk(disk);
                    PutTier(tier);
                    return ret;
                }

                tier->PutDisk(disk);
                ret = tier->GetNextDisk(&disk);
            }

            PutTier(tier);
        }
    }

    /*Take it as success when comes here*/
    return YWB_SUCCESS;
}

//#ifdef WITH_INITIAL_PLACEMENT
//ywb_status_t
//SubPool::AddSwapDisk(DiskKey& diskkey, ywb_int32_t val)
//{
//    map<DiskKey, ywb_int32_t, DiskKeyCmp>::iterator iter;
//
//    iter = mSwapRequiredDisks.find(diskkey);
//    if(mSwapRequiredDisks.end() != iter)
//    {
//        iter->second = val;
//    }
//    else
//    {
//        mSwapRequiredDisks.insert(std::make_pair(diskkey, val));
//    }
//
//    return YWB_SUCCESS;
//}
//
//ywb_status_t
//SubPool::RemoveSwapDisk(DiskKey& diskkey)
//{
//    map<DiskKey, ywb_int32_t, DiskKeyCmp>::iterator iter;
//
//    iter = mSwapRequiredDisks.find(diskkey);
//    if(mSwapRequiredDisks.end() != iter)
//    {
//        mSwapRequiredDisks.erase(iter);
//        return YWB_SUCCESS;
//    }
//
//    return YFS_ENOENT;
//}
//#endif

LogicalConfig::LogicalConfig(AST* ast) :
        mSubPoolsLock("SubPoolsLock"), mAst(ast)
{
    mCollectConfigReq = new CollectLogicalConfigRequest(Constant::OSS_ID);
    YWB_ASSERT(mCollectConfigReq != NULL);
}

LogicalConfig::~LogicalConfig()
{
    Reset();

    if(mCollectConfigReq)
    {
        mCollectConfigReq->DecRef();
    }

    mAst = NULL;
}

ywb_status_t
LogicalConfig::GetDiskKeyFromDiskId(ywb_uint64_t diskid, DiskKey* key)
{
    map<ywb_uint64_t, DiskKey>::iterator iter;

    iter = mDiskId2Key.find(diskid);
    if(iter != mDiskId2Key.end())
    {
        *key = iter->second;
        return YWB_SUCCESS;
    }

    return YFS_ENOENT;
}

ywb_status_t
LogicalConfig::GetSubPoolKeyFromSubPoolId(
        ywb_uint32_t subpoolid, SubPoolKey* key)
{
    map<ywb_uint32_t, SubPoolKey>::iterator iter;

    iter = mSubPoolId2Key.find(subpoolid);
    if(iter != mSubPoolId2Key.end())
    {
        *key = iter->second;
        return YWB_SUCCESS;
    }

    return YFS_ENOENT;
}

ywb_status_t
LogicalConfig::GetOBJ(OBJKey& key, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Disk* disk = NULL;

    ret = GetDiskKeyFromDiskId(key.GetStorageId(), &diskkey);
    if(YWB_SUCCESS == ret)
    {
        ret = GetDisk(diskkey, &disk);
        if(YWB_SUCCESS == ret)
        {
            ret = disk->GetOBJ(key, obj);
            PutDisk(disk);
        }
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
LogicalConfig::GetDisk(DiskKey& key, Disk** disk)
{
    ywb_status_t ret = YFS_ENOENT;
    SubPoolKey subpoolkey;
    SubPool* subpool = NULL;

    ret = GetSubPoolKeyFromSubPoolId(key.GetSubPoolId(), &subpoolkey);
    if(YWB_SUCCESS == ret)
    {
        ret = GetSubPool(subpoolkey, &subpool);
        if((YWB_SUCCESS == ret) && (subpool != NULL))
        {
            ret = subpool->GetDisk(key, disk);
            PutSubPool(subpool);
        }
    }
    else
    {
        *disk = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
LogicalConfig::GetSubPool(SubPoolKey& key, SubPool** subpool)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPool*, SubPoolKeyCmp>::iterator iter;

    mSubPoolsLock.Lock();
    iter = mSubPools.find(key);
    if(iter != mSubPools.end())
    {
        *subpool = iter->second;
        (*subpool)->IncRef();
    }
    else
    {
        ret = YFS_ENOENT;
        *subpool = NULL;
    }
    mSubPoolsLock.Unlock();

    return ret;
}

ywb_status_t
LogicalConfig::AddOBJ(OBJKey& key, OBJVal* obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Disk* disk = NULL;

    ret = GetDiskKeyFromDiskId(key.GetStorageId(), &diskkey);
    if(YWB_SUCCESS == ret)
    {
        ret = GetDisk(diskkey, &disk);
        if(YWB_SUCCESS == ret)
        {
            ret = disk->AddOBJ(key, obj);
            PutDisk(disk);
        }
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
LogicalConfig::AddDisk(DiskKey& key, Disk* disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<ywb_uint64_t, DiskKey>::iterator, bool> pairret;
    SubPoolKey subpoolkey;
    SubPool* subpool = NULL;

    pairret = mDiskId2Key.insert(std::make_pair(key.GetDiskId(), key));
    if(pairret.second)
    {
//        ast_log_debug("Add disk [" << key.GetSubPoolId()
//                << ", " << key.GetDiskId()
//                << "] into logical configuration");

        ret = GetSubPoolKeyFromSubPoolId(key.GetSubPoolId(), &subpoolkey);
        /*subpool id to subpool key mapping is already setup*/
        if(YWB_SUCCESS == ret)
        {
            ret = GetSubPool(subpoolkey, &subpool);
            /*
             * we are sure that subpool is added into logical configuration
             * prior to disk
             **/
            YWB_ASSERT(YWB_SUCCESS == ret);
            ret = subpool->AddDisk(key, disk);
            PutSubPool(subpool);
        }
        else
        {
            /*corresponding subpool does not exist*/
            mDiskId2Key.erase(key.GetDiskId());
            ret = YFS_EINVAL;
        }
    }
    else
    {
        ast_log_debug("Disk [" << key.GetSubPoolId()
                        << ", " << key.GetDiskId()
                        << "] already exist!");
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t
LogicalConfig::AddSubPool(SubPoolKey& key, SubPool* subpool)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<ywb_uint32_t, SubPoolKey>::iterator, bool> ret1;
    pair<map<SubPoolKey, SubPool*, SubPoolKeyCmp>::iterator, bool> ret2;
    map<SubPoolKey, SubPool*, SubPoolKeyCmp>::iterator iter;

    mSubPoolsLock.Lock();
    ret1 = mSubPoolId2Key.insert(std::make_pair(key.GetSubPoolId(), key));
    if(ret1.second)
    {
        ast_log_debug("Add subpool [" << key.GetOss()
                << ", " << key.GetPoolId()
                << ", " << key.GetSubPoolId()
                << "] into logical configuration");

        subpool->IncRef();
        ret2 = mSubPools.insert(std::make_pair(key, subpool));
        YWB_ASSERT(ret2.second);
    }
    else
    {
        ast_log_debug("Subpool [" << key.GetOss()
                << ", " << key.GetPoolId()
                << ", " << key.GetSubPoolId()
                << "] already exist!");
        iter = mSubPools.find(key);
        YWB_ASSERT(iter != mSubPools.end());
        ret = YFS_EEXIST;
    }
    mSubPoolsLock.Unlock();

    return ret;
}

ywb_status_t
LogicalConfig::RemoveOBJ(OBJKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    Disk* disk = NULL;

    ast_log_debug("Remove OBJ [" << key.GetStorageId()
            << ", " << key.GetInodeId()
            << ", " << key.GetChunkId()
            << ", " << key.GetChunkIndex()
            << ", " << key.GetChunkVersion()
            << "] from logical configuration");

    ret = GetDiskKeyFromDiskId(key.GetStorageId(), &diskkey);
    if(YWB_SUCCESS == ret)
    {
        ret = GetDisk(diskkey, &disk);
        if(YWB_SUCCESS == ret)
        {
            ret = disk->RemoveOBJ(key);
            PutDisk(disk);
        }
    }

    return ret;
}

ywb_status_t
LogicalConfig::RemoveDisk(DiskKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    SubPool* subpool = NULL;
    map<ywb_uint64_t, DiskKey>::iterator iter;

    ast_log_debug("Remove disk [" << key.GetSubPoolId()
            << ", " << key.GetDiskId()
            << "] from logical configuration");

    iter = mDiskId2Key.find(key.GetDiskId());
    if(iter != mDiskId2Key.end())
    {
        mDiskId2Key.erase(iter);
    }

    ret = GetSubPoolKeyFromSubPoolId(key.GetSubPoolId(), &subpoolkey);
    if(YWB_SUCCESS == ret)
    {
        ret = GetSubPool(subpoolkey, &subpool);
        YWB_ASSERT(YWB_SUCCESS == ret);
        ret = subpool->RemoveDisk(key);
        PutSubPool(subpool);
    }

    return ret;
}

ywb_status_t
LogicalConfig::RemoveSubPool(SubPoolKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ywb_uint32_t, SubPoolKey>::iterator iter;
    map<SubPoolKey, SubPool*, SubPoolKeyCmp>::iterator iter2;
    SubPool* subpool = NULL;

    ast_log_debug("Remove subpool [" << key.GetOss()
            << ", " << key.GetPoolId()
            << ", " << key.GetSubPoolId()
            << "] from logical configuration");

    mSubPoolsLock.Lock();
    iter = mSubPoolId2Key.find(key.GetSubPoolId());
    if(iter != mSubPoolId2Key.end())
    {
        mSubPoolId2Key.erase(iter);
    }

    iter2 = mSubPools.find(key);
    if(iter2 != mSubPools.end())
    {
        subpool = iter2->second;
        mSubPools.erase(iter2);
        PutSubPool(subpool);
//        subpool = NULL;
    }
    mSubPoolsLock.Unlock();

    return ret;
}

ywb_status_t
LogicalConfig::AddDeletedOBJ(OBJKey& objkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    mDeletedOBJs.insert(objkey);
    return ret;
}

ywb_status_t
LogicalConfig::AddDeletedDisk(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJKey* objkey = NULL;

    /*should also mark all contained OBJs as deleted*/
    ret = GetDisk(diskkey, &disk);
    if((YWB_SUCCESS == ret) && (disk != NULL))
    {
        ret = disk->GetFirstOBJ(&obj);
        while((YWB_SUCCESS == ret) && (obj != NULL))
        {
            obj->GetOBJKey(&objkey);
            mDeletedOBJs.insert(*objkey);
            PutOBJ(obj);
            ret = disk->GetNextOBJ(&obj);
        }

        ret = YWB_SUCCESS;
        PutDisk(disk);
    }

    mDeletedDisks.insert(diskkey);
    return ret;
}

ywb_status_t
LogicalConfig::AddDeletedSubPool(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tiertype = Tier::TIER_0;
    DiskKey diskkey;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJKey* objkey = NULL;

    ret = GetSubPool(subpoolkey, &subpool);
    if((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        /*also mark its contained disks/OBJs as deleted*/
        for(; tiertype < Tier::TIER_cnt; tiertype++)
        {
            ret = subpool->GetTier(tiertype, &tier);
            if(!((YWB_SUCCESS == ret) && (tier != NULL)))
            {
                continue;
            }

            ret = tier->GetFirstDisk(&disk, &diskkey);
            while((YWB_SUCCESS == ret) && (disk != NULL))
            {
                ret = disk->GetFirstOBJ(&obj);
                while((YWB_SUCCESS == ret) && (obj != NULL))
                {
                    obj->GetOBJKey(&objkey);
                    mDeletedOBJs.insert(*objkey);
                    PutOBJ(obj);
                    ret = disk->GetNextOBJ(&obj);
                }

                PutDisk(disk);
                mDeletedDisks.insert(diskkey);
                ret = tier->GetNextDisk(&disk);
            }

            subpool->PutTier(tier);
        }

        ret = YWB_SUCCESS;
        PutSubPool(subpool);
    }

    mDeletedSubPools.insert(subpoolkey);
    return ret;
}

ywb_bool_t
LogicalConfig::CheckOBJDeleted(OBJKey& key)
{
    set<OBJKey, OBJKeyCmp>::iterator iter;
    bool deleted = false;

    /*OBJ is in reported deleted OBJs set*/
    iter = mDeletedOBJs.find(key);
    if(iter != mDeletedOBJs.end())
    {
        deleted = true;
    }
    else
    {
        deleted = false;
    }

    ast_log_debug("OBJ [" << key.GetStorageId()
            << ", " << key.GetInodeId()
            << ", " << key.GetChunkId()
            << ", " << key.GetChunkIndex()
            << ", " << key.GetChunkVersion()
            << "] has been deleted: " << deleted);
    return deleted;
}

ywb_bool_t
LogicalConfig::CheckDiskDeleted(DiskKey& key)
{
    set<DiskKey, DiskKeyCmp>::iterator iter;
    bool deleted = false;

    /*disk is in reported deleted disks set*/
    iter = mDeletedDisks.find(key);
    if(iter != mDeletedDisks.end())
    {
        deleted = true;
        ast_log_debug("Disk [" << key.GetSubPoolId()
               << ", " << key.GetDiskId()
               << "] has been deleted:");
    }
    else
    {
        deleted = false;
    }

    return deleted;
}

ywb_bool_t
LogicalConfig::CheckSubPoolDeleted(SubPoolKey& key)
{
    set<SubPoolKey, SubPoolKeyCmp>::iterator iter;
    bool deleted = false;

    /*subpool is in reported deleted subpools set*/
    iter = mDeletedSubPools.find(key);
    if(iter != mDeletedSubPools.end())
    {
        deleted = true;
        ast_log_debug("SubPool [" << key.GetOss()
                << ", " << key.GetPoolId()
                << ", " << key.GetSubPoolId()
                << "] has been deleted");
    }
    else
    {
        deleted = false;
    }

    return deleted;
}

ywb_bool_t
LogicalConfig::CheckOBJExistence(OBJKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJVal* obj = NULL;
    DiskKey diskkey;

    /*OBJ is in deleted set*/
    if(CheckOBJDeleted(key))
    {
        ast_log_debug("OBJ [" << key.GetStorageId()
                << ", " << key.GetInodeId()
                << ", " << key.GetChunkId()
                << ", " << key.GetChunkIndex()
                << ", " << key.GetChunkVersion()
                << "] does not exist");
        return false;
    }

    /*associated disk does not exist*/
    GetDiskKeyFromDiskId(key.GetStorageId(), &diskkey);
    if(!CheckDiskExistence(diskkey))
    {
        ast_log_debug("OBJ [" << key.GetStorageId()
                << ", " << key.GetInodeId()
                << ", " << key.GetChunkId()
                << ", " << key.GetChunkIndex()
                << ", " << key.GetChunkVersion()
                << "] does not exist");
        return false;
    }

    ret = GetOBJ(key, &obj);
    /*OBJ is in reported configuration*/
    if((YWB_SUCCESS == ret) && (obj != NULL))
    {
        ast_log_debug("OBJ [" << key.GetStorageId()
                << ", " << key.GetInodeId()
                << ", " << key.GetChunkId()
                << ", " << key.GetChunkIndex()
                << ", " << key.GetChunkVersion()
                << "] still exist");

        PutOBJ(obj);
        return true;
    }
    else
    {
        ast_log_debug("OBJ [" << key.GetStorageId()
                << ", " << key.GetInodeId()
                << ", " << key.GetChunkId()
                << ", " << key.GetChunkIndex()
                << ", " << key.GetChunkVersion()
                << "] does not exist");

        return false;
    }
}

ywb_bool_t
LogicalConfig::CheckDiskExistence(DiskKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    Disk* disk = NULL;

    /*disk is in deleted set*/
    if(CheckDiskDeleted(key))
    {
        ast_log_debug("Disk [" << key.GetSubPoolId()
                << ", " << key.GetDiskId()
                << "] does not exist");
        return false;
    }

    ret = GetDisk(key, &disk);
    /*disk is in reported configuration*/
    if((YWB_SUCCESS == ret))
    {
//        ast_log_debug("Disk [" << key.GetSubPoolId()
//                << ", " << key.GetDiskId()
//                << "] still exist");

        PutDisk(disk);
        return true;
    }
    else
    {
        ast_log_debug("Disk [" << key.GetSubPoolId()
                << ", " << key.GetDiskId()
                << "] does not exist");
        return false;
    }
}

ywb_bool_t
LogicalConfig::CheckSubPoolExistence(SubPoolKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPool* subpool = NULL;

    /*subpool is in deleted set*/
    if(CheckSubPoolDeleted(key))
    {
        return false;
    }

    ret = GetSubPool(key, &subpool);
    /*subpool is in reported configuration*/
    if((YWB_SUCCESS == ret))
    {
        PutSubPool(subpool);
        return true;
    }
    else
    {
        return false;
    }
}

ywb_status_t
LogicalConfig::GetFirstSubPool(SubPool** subpool, SubPoolKey* key)
{
    ywb_status_t ret = YWB_SUCCESS;

    mSubPoolsLock.Lock();
    if(!mSubPools.empty())
    {
        mIt = mSubPools.begin();
        *subpool = mIt->second;
        *key = mIt->first;
        (*subpool)->IncRef();
        mIt++;
    }
    else
    {
        *subpool = NULL;
        ret = YFS_ENOENT;
    }
    mSubPoolsLock.Unlock();

    return ret;
}

ywb_status_t
LogicalConfig::GetNextSubPool(SubPool** subpool, SubPoolKey* key)
{
    ywb_status_t ret = YWB_SUCCESS;

    mSubPoolsLock.Lock();
    if(mIt != mSubPools.end())
    {
        *subpool = mIt->second;
        *key = mIt->first;
        (*subpool)->IncRef();
        mIt++;
    }
    else
    {
        *subpool = NULL;
        ret = YFS_ENOENT;
    }
    mSubPoolsLock.Unlock();

    return ret;
}

ywb_uint32_t
LogicalConfig::GetKeysAndSubPools(
        list<SubPoolKey>& subpoolkeys, list<SubPool*>& subpools)
{
    map<SubPoolKey, SubPool*, SubPoolKeyCmp>::iterator iter;
    SubPool* subpool = NULL;

    mSubPoolsLock.Lock();
    iter = mSubPools.begin();
    for(; iter != mSubPools.end(); iter++)
    {
        subpool = iter->second;
        subpool->IncRef();
        subpoolkeys.push_back(iter->first);
        subpools.push_back(subpool);
    }
    mSubPoolsLock.Unlock();

    return subpoolkeys.size();
}

ywb_status_t
LogicalConfig::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPool*, SubPoolKeyCmp>::iterator subpooliter;
    SubPool* subpool = NULL;

    ast_log_debug("LogicalConfig reset");
    mSubPoolsLock.Lock();
    mDiskId2Key.clear();
    mSubPoolId2Key.clear();
    subpooliter = mSubPools.begin();
    for(; subpooliter != mSubPools.end(); )
    {
        subpool = subpooliter->second;
        mSubPools.erase(subpooliter++);
        subpool->DecRef();
    }
//    mSubPools.clear();
    mSubPoolsLock.Unlock();

    mDeletedOBJs.clear();
    mDeletedDisks.clear();
    mDeletedSubPools.clear();
    mCollectConfigReq->SetStatus(YWB_SUCCESS);

    return ret;
}

ywb_status_t
LogicalConfig::Consolidate()
{
    ywb_status_t ret = YWB_SUCCESS;
    set<OBJKey, OBJKeyCmp>::iterator objiter;
    set<DiskKey, DiskKeyCmp>::iterator diskiter;
    set<SubPoolKey, SubPoolKeyCmp>::iterator subpooliter;
    OBJKey objkey;
    DiskKey diskkey;
    SubPoolKey subpoolkey;

    ast_log_debug("Logical configuration consolidate");
    objiter = mDeletedOBJs.begin();
    diskiter = mDeletedDisks.begin();
    subpooliter = mDeletedSubPools.begin();

    for(; subpooliter != mDeletedSubPools.end(); )
    {
        subpoolkey = *subpooliter;
        subpooliter++;
        RemoveSubPool(subpoolkey);
    }
    mDeletedSubPools.clear();

    for(; diskiter != mDeletedDisks.end(); )
    {
        diskkey = *diskiter;
        diskiter++;
        RemoveDisk(diskkey);
    }
    mDeletedDisks.clear();

    for(; objiter != mDeletedOBJs.end(); )
    {
        objkey = *objiter;
        objiter++;
        RemoveOBJ(objkey);
    }
    mDeletedOBJs.clear();

    return ret;
}

/*
 * currently @ossid is ignored
 **/
ywb_status_t
LogicalConfig::CollectLogicalConfig(ywb_uint32_t ossid)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    Generator* gen = NULL;

    ast_log_debug("Send collect logical configuration request");
    GetAST(&ast);
    ast->GetGenerator(&gen);
//    gen->SendSync(mCollectConfigReq);
    gen->Send(mCollectConfigReq);
    return ret;
}

ywb_status_t
LogicalConfig::ResolveLogicalConfig(CollectLogicalConfigResponse* resp)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_bool_t subpoolresolutionsuccess = ywb_false;
    ywb_bool_t diskresolutionsuccess = ywb_false;
    SubPoolKey subpoolkey;
    DiskInfo diskinfo;
    SubPool* subpool = NULL;
    DiskBaseProp* baseprop = NULL;
    DiskKey* diskkey = NULL;
    Disk* disk = NULL;
    CollectLogicalConfigResponse* collectconfigresp = NULL;

    ast_log_debug("Resolve logical configuration response");
    collectconfigresp = resp;
    YWB_ASSERT(collectconfigresp != NULL);

    do
    {
        if(YWB_SUCCESS != collectconfigresp->GetStatus())
        {
            ret = collectconfigresp->GetStatus();
            ast_log_debug("Resolve failure for failing to collect"
                    " logical configuration");
            break;
        }

        ast_log_debug("Resolve subpools start ---->>>>");
        ret = collectconfigresp->GetFirstSubPool(&subpoolkey);
        while((YWB_SUCCESS == ret))
        {
            ast_log_debug("subpools [" << subpoolkey.GetOss()
                    << ", " << subpoolkey.GetPoolId()
                    << ", " << subpoolkey.GetSubPoolId() << "]");

            subpoolresolutionsuccess = ywb_true;
            /*check whether the subpool already exist*/
            ret = GetSubPool(subpoolkey, &subpool);
            if(!((YWB_SUCCESS == ret) && (subpool != NULL)))
            {
                subpool = new SubPool(subpoolkey);
                if(subpool != NULL)
                {
                   ret = AddSubPool(subpoolkey, subpool);
                }
                else
                {
                    ast_log_debug("Out of memory!");
                    ret = YFS_EOUTOFMEMORY;
                    break;
                }

                if(ret != YWB_SUCCESS)
                {
                    /*DO NOT use PutSubPool(subpool) here*/
                    delete subpool;
                    subpool = NULL;
                }
            }
            else
            {
                subpool->ClearFlag(SubPool::SPF_suspicious);
                PutSubPool(subpool);
            }

            ret = collectconfigresp->GetNextSubPool(&subpoolkey);
        }
        ast_log_debug("Resolve subpools done <<<<----");

        /*only if subpool resolution is successful*/
        if((subpoolresolutionsuccess == true))
        {
            ast_log_debug("Resolve disks start ---->>>>");
            /*resolve disk configuration from @mCollectConfigResp*/
            ret = collectconfigresp->GetFirstDisk(&diskinfo);
            while((ret == YWB_SUCCESS))
            {
                diskresolutionsuccess = ywb_true;
                diskinfo.GetBaseProp(&baseprop);
                diskinfo.GetDiskKey(&diskkey);

                /*check whether the subpoolkey exists*/
                ret = GetSubPoolKeyFromSubPoolId(
                        diskkey->GetSubPoolId(), &subpoolkey);
                if(YWB_SUCCESS != ret)
                {
                    ret = collectconfigresp->GetNextDisk(&diskinfo);
                    continue;
                }

                /*check whether the subpool exists*/
                ret = GetSubPool(subpoolkey, &subpool);
                if(!((YWB_SUCCESS == ret) && (subpool != NULL)))
                {
                    ret = collectconfigresp->GetNextDisk(&diskinfo);
                    continue;
                }

                /*check whether the subpool has SubPool::SPF_suspicious set*/
                if(subpool->TestFlag(SubPool::SPF_suspicious))
                {
                    ret = collectconfigresp->GetNextDisk(&diskinfo);
                    PutSubPool(subpool);
                    continue;
                }

                ast_log_debug("Disk [" << diskkey->GetSubPoolId()
                        << ", " << diskkey->GetDiskId() << "]");

                /*check whether the disk already exist*/
                ret = subpool->GetDisk(*diskkey, &disk);
                if(!((YWB_SUCCESS == ret) && (disk != NULL)))
                {
                    disk = new Disk(*baseprop);
                    if(disk != NULL)
                    {
                        ret = AddDisk(*diskkey, disk);
                    }
                    else
                    {
                        ast_log_debug("Out of memory!");
                        ret = YFS_EOUTOFMEMORY;
                        break;
                    }

                    if(ret != YWB_SUCCESS)
                    {
                        /*DO NOT use PutDisk(disk) here*/
                        delete disk;
                        disk = NULL;
                    }
                }
                else
                {
                    disk->ClearFlag(Disk::DF_suspicious);
                    subpool->PutDisk(disk);
                }

                PutSubPool(subpool);
                ret = collectconfigresp->GetNextDisk(&diskinfo);
            }

            ast_log_debug("Resolve disks done <<<<----");
            if((YFS_EOUTOFMEMORY != ret) && (ywb_true == diskresolutionsuccess))
            {
                ret = YWB_SUCCESS;
            }
        }
    } while(0);

    collectconfigresp->DecRef();
    return ret;
}

ywb_status_t
LogicalConfig::ResolveDeletedOBJs(ReportDeleteOBJsMessage* resp)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJKey objkey;
    ReportDeleteOBJsMessage* deleteobjmsg = NULL;

    deleteobjmsg = resp;
    YWB_ASSERT(deleteobjmsg != NULL);
    ast_log_debug("Resolve deleted OBJs start ---->>>>");
    ret = deleteobjmsg->GetFirstOBJ(&objkey);
    while((YWB_SUCCESS == ret))
    {
        ast_log_debug("OBJ [" << objkey.GetStorageId()
                << ", " << objkey.GetInodeId()
                << ", " << objkey.GetChunkId()
                << ", " << objkey.GetChunkIndex()
                << ", " << objkey.GetChunkVersion()
                << "]");
        mDeletedOBJs.insert(objkey);
        ret = deleteobjmsg->GetNextOBJ(&objkey);
    }

    ast_log_debug("Resolve deleted OBJs done <<<<----");
    deleteobjmsg->DecRef();
    return YWB_SUCCESS;
}

ywb_status_t
LogicalConfig::ResolveDeletedDisks(ReportDeleteDisksMessage* resp)
{
    ywb_status_t ret = YWB_SUCCESS;
    ReportDeleteDisksMessage* deletediskmsg = NULL;
    DiskKey diskkey;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJKey *objkey = NULL;

    deletediskmsg = resp;
    YWB_ASSERT(deletediskmsg != NULL);
    ast_log_debug("Resolve deleted disks start ---->>>>");
    ret = deletediskmsg->GetFirstDisk(&diskkey);
    while((YWB_SUCCESS == ret))
    {
        ast_log_debug("Disk [" << diskkey.GetSubPoolId()
                << ", " << diskkey.GetDiskId()
                << "]");

        /*should also mark all contained OBJs as deleted*/
        ret = GetDisk(diskkey, &disk);
        if((YWB_SUCCESS == ret) && (disk != NULL))
        {
            ret = disk->GetFirstOBJ(&obj);
            while((YWB_SUCCESS == ret) && (obj != NULL))
            {
                obj->GetOBJKey(&objkey);
                mDeletedOBJs.insert(*objkey);
                PutOBJ(obj);
                disk->GetNextOBJ(&obj);
            }

            PutDisk(disk);
        }

        mDeletedDisks.insert(diskkey);
        ret = deletediskmsg->GetNextDisk(&diskkey);
    }

    ast_log_debug("Resolve deleted disks done <<<<----");
    deletediskmsg->DecRef();
    return YWB_SUCCESS;
}

ywb_status_t
LogicalConfig::ResolveDeletedSubPools(ReportDeleteSubPoolsMessage* resp)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    DiskKey diskkey;
    ywb_uint32_t tiertype = Tier::TIER_cnt;
    ReportDeleteSubPoolsMessage* deletesubpoolmsg = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJKey* objkey = NULL;

    deletesubpoolmsg = resp;
    YWB_ASSERT(deletesubpoolmsg != NULL);
    ast_log_debug("Resolve deleted subpools start ---->>>>");
    ret = deletesubpoolmsg->GetFirstSubPool(&subpoolkey);
    while((YWB_SUCCESS == ret))
    {
        ast_log_debug("SubPool [" << subpoolkey.GetOss()
                << ", " << subpoolkey.GetPoolId()
                << ", " << subpoolkey.GetSubPoolId()
                << "]");

        /*should also mark all contained disks as deleted*/
        ret = GetSubPool(subpoolkey, &subpool);
        if((YWB_SUCCESS == ret) && (subpool != NULL))
        {
            tiertype = Tier::TIER_0;
            for(; tiertype < Tier::TIER_cnt; tiertype++)
            {
                ret = subpool->GetTier(tiertype, &tier);
                if(!((YWB_SUCCESS == ret) && (tier != NULL)))
                {
                    continue;
                }

                ret = tier->GetFirstDisk(&disk, &diskkey);
                while((YWB_SUCCESS == ret) && (disk != NULL))
                {
                    ret = disk->GetFirstOBJ(&obj);
                    while((YWB_SUCCESS == ret) && (obj != NULL))
                    {
                        obj->GetOBJKey(&objkey);
                        mDeletedOBJs.insert(*objkey);
                        PutOBJ(obj);
                        disk->GetNextOBJ(&obj);
                    }

                    PutDisk(disk);
                    mDeletedDisks.insert(diskkey);
                    ret = tier->GetNextDisk(&disk);
                }

                subpool->PutTier(tier);
            }

            PutSubPool(subpool);
        }

        mDeletedSubPools.insert(subpoolkey);
        ret = deletesubpoolmsg->GetNextSubPool(&subpoolkey);
    }

    ast_log_debug("Resolve deleted subpools done <<<<----");
    deletesubpoolmsg->DecRef();
    return YWB_SUCCESS;
}

ywb_status_t
LogicalConfig::ResolveDeletedOBJ(ReportDeleteOBJMessage* resp)
{
    OBJKey *objkey = NULL;
    ReportDeleteOBJMessage* deleteobjmsg = NULL;

    deleteobjmsg = resp;
    YWB_ASSERT(deleteobjmsg != NULL);
    deleteobjmsg->GetOBJ(&objkey);
    ast_log_debug("Resolve deleted OBJ ["
            << objkey->GetStorageId()
            << ", " << objkey->GetInodeId()
            << ", " << objkey->GetChunkId()
            << ", " << objkey->GetChunkIndex()
            << ", " << objkey->GetChunkVersion()
            << "]");
    mDeletedOBJs.insert(*objkey);
    deleteobjmsg->DecRef();
    return YWB_SUCCESS;
}

ywb_status_t
LogicalConfig::ResolveDeletedDisk(ReportDeleteDiskMessage* resp)
{
    ywb_status_t ret = YWB_SUCCESS;
    ReportDeleteDiskMessage* deletediskmsg = NULL;
    DiskKey* diskkey = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJKey *objkey = NULL;

    deletediskmsg = resp;
    YWB_ASSERT(deletediskmsg != NULL);
    deletediskmsg->GetDisk(&diskkey);
    ast_log_debug("Resolve deleted disk ["
            << diskkey->GetSubPoolId()
            << ", " << diskkey->GetDiskId() << "]");
    /*should also mark all contained OBJs as deleted*/
    ret = GetDisk(*diskkey, &disk);
    if((YWB_SUCCESS == ret) && (disk != NULL))
    {
        ret = disk->GetFirstOBJ(&obj);
        while((YWB_SUCCESS == ret) && (obj != NULL))
        {
            obj->GetOBJKey(&objkey);
            mDeletedOBJs.insert(*objkey);
            PutOBJ(obj);
            disk->GetNextOBJ(&obj);
        }

        PutDisk(disk);
    }

    mDeletedDisks.insert(*diskkey);
    deletediskmsg->DecRef();
    return YWB_SUCCESS;
}

ywb_status_t
LogicalConfig::ResolveDeletedSubPool(ReportDeleteSubPoolMessage* resp)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    ywb_uint32_t tiertype = Tier::TIER_cnt;
    ReportDeleteSubPoolMessage* deletesubpoolmsg = NULL;
    SubPoolKey* subpoolkey = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    Disk* disk = NULL;
    OBJ* obj = NULL;
    OBJKey* objkey = NULL;

    deletesubpoolmsg = resp;
    YWB_ASSERT(deletesubpoolmsg != NULL);
    deletesubpoolmsg->GetSubPool(&subpoolkey);
    ast_log_debug("Resolve deleted SubPool ["
            << subpoolkey->GetOss()
            << ", " << subpoolkey->GetPoolId()
            << ", " << subpoolkey->GetSubPoolId()
            << "]");
    /*should also mark all contained disks as deleted*/
    ret = GetSubPool(*subpoolkey, &subpool);
    if((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        tiertype = Tier::TIER_0;
        for(; tiertype < Tier::TIER_cnt; tiertype++)
        {
            ret = subpool->GetTier(tiertype, &tier);
            if(!((YWB_SUCCESS == ret) && (tier != NULL)))
            {
                continue;
            }

            ret = tier->GetFirstDisk(&disk, &diskkey);
            while((YWB_SUCCESS == ret) && (disk != NULL))
            {
                ret = disk->GetFirstOBJ(&obj);
                while((YWB_SUCCESS == ret) && (obj != NULL))
                {
                    obj->GetOBJKey(&objkey);
                    mDeletedOBJs.insert(*objkey);
                    PutOBJ(obj);
                    disk->GetNextOBJ(&obj);
                }

                PutDisk(disk);
                mDeletedDisks.insert(diskkey);
                ret = tier->GetNextDisk(&disk);
            }

            subpool->PutTier(tier);
        }

        PutSubPool(subpool);
    }

    mDeletedSubPools.insert(*subpoolkey);
    deletesubpoolmsg->DecRef();
    return YWB_SUCCESS;
}

#ifdef WITH_INITIAL_PLACEMENT
ywb_status_t
LogicalConfig::ResolveRelocationType(NotifyRelocationType* resp)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    ywb_int32_t val = 0;
    NotifyRelocationType* relocationtypes = NULL;
    SubPool* subpool = NULL;

    relocationtypes = (NotifyRelocationType*)resp;
    YWB_ASSERT(relocationtypes != NULL);
    ret = relocationtypes->GetFirstRelocationType(subpoolkey, val);
    while(YWB_SUCCESS == ret)
    {
        if((RelocationType::RT_no_data_movement | val))
        {
            ret = GetSubPool(subpoolkey, &subpool);
            if(YWB_SUCCESS == ret)
            {
                subpool->SetFlagComb(SubPool::SPF_no_migration);
                PutSubPool(subpool);
            }
        }

        ret = relocationtypes->GetNextRelocationType(subpoolkey, val);
    }

    return YWB_SUCCESS;
}
#endif

/* vim:set ts=4 sw=4 expandtab */
