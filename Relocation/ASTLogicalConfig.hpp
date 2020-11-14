#ifndef __AST_LOGICAL_CONFIG_HPP__
#define __AST_LOGICAL_CONFIG_HPP__

#include <list>
#include <vector>
#include <map>
#include <set>
#include <sstream>
#include "common/FsInts.h"
#include "common/FsStatus.h"
#include "common/FsAtomic.h"
#include "FsCommon/YfsMutex.hpp"
#include "AST/ASTUtil.hpp"
#include "OSSCommon/Logger.hpp"
#include "OSSCommon/Globals.hpp"

using namespace std;

/*forward reference*/
class OBJIO;
class DiskIO;
class Tier;
class AST;

#ifdef WITH_INITIAL_PLACEMENT
class NotifyRelocationType;
#endif

class OBJKey {
public:
    friend class OBJKeyCmp;
    OBJKey()
    {
        mChunkId = YWB_UINT64_MAX;
        mInodeId = YWB_UINT64_MAX;
        mChunkIndex = YWB_UINT32_MAX;
        mChunkVersion = YWB_UINT32_MAX;
        mStorageId = YWB_UINT64_MAX;
    }

    OBJKey(ywb_uint64_t chunkid, ywb_uint64_t inodeid,
            ywb_uint32_t chunkindex, ywb_uint32_t chunkversion,
            ywb_uint64_t storageid) : mChunkId(chunkid),
            mInodeId(inodeid), mChunkIndex(chunkindex),
            mChunkVersion(chunkversion), mStorageId(storageid){}

    OBJKey(const OBJKey& key)
    {
        this->mChunkId = key.mChunkId;
        this->mInodeId = key.mInodeId;
        this->mChunkIndex =  key.mChunkIndex;
        this->mChunkVersion = key.mChunkVersion;
        this->mStorageId = key.mStorageId;
    }

    OBJKey& operator= (const OBJKey& key)
    {
        if(this == &key)
            return *this;

        this->mChunkId = key.mChunkId;
        this->mInodeId = key.mInodeId;
        this->mChunkIndex =  key.mChunkIndex;
        this->mChunkVersion = key.mChunkVersion;
        this->mStorageId = key.mStorageId;

        return *this;
    }

    bool operator==(const OBJKey& key)
    {
        return ((mStorageId == key.mStorageId) && (mInodeId == key.mInodeId)
                 && (mChunkId == key.mChunkId) && (mChunkIndex ==  key.mChunkIndex)
                 && (mChunkVersion == key.mChunkVersion));
    }

    inline ywb_uint64_t GetChunkId();
    inline ywb_uint64_t GetInodeId();
    inline ywb_uint32_t GetChunkIndex();
    inline ywb_uint32_t GetChunkVersion();
    inline ywb_uint64_t GetStorageId();

    inline void SetChunkId(ywb_uint64_t id);
    inline void SetInodeId(ywb_uint64_t id);
    inline void SetChunkIndex(ywb_uint32_t index);
    inline void SetChunkVersion(ywb_uint32_t ver);
    inline void SetStorageId(ywb_uint64_t id);

private:
    /*Extracted from ChunkStorageStruct*/
    ywb_uint64_t mChunkId;
    ywb_uint64_t mInodeId;
    ywb_uint32_t mChunkIndex;
    ywb_uint32_t mChunkVersion;
    ywb_uint64_t mStorageId;
};

/*for OBJ key comparison*/
class OBJKeyCmp {
public:
    bool operator()(const OBJKey& key1, const OBJKey& key2) const;
};

class OBJ {
public:
    enum {
        OF_suspicious = 0,
    };

//    OBJ();
    OBJ(OBJKey& key);
    /*
     * definition of OBJ's deconstructor must be in .cpp if delete @mObjIO
     * herein, otherwise there will be cross-reference issue
     **/
    ~OBJ();

    inline void GetOBJKey(OBJKey** key);
//    inline void GetOBJIOConst(const OBJIO** io);
    inline void GetOBJIO(OBJIO** io);
    inline void SetOBJKey(OBJKey& key);
    inline void SetOBJIO(OBJIO*& io);

    inline void SetFlagComb(ywb_uint32_t flag);
    inline void SetFlagUniq(ywb_uint32_t flag);
    inline void ClearFlag(ywb_uint32_t flag);
    inline bool TestFlag(ywb_uint32_t flag);

    inline void IncRef();
    inline void DecRef();

    void Dump(ostringstream* ostr);

private:
    /*object key*/
    OBJKey mKey;
    /*object IO*/
    OBJIO* mObjIO;
    /*flags*/
    Flag mFlag;
    /*reference count*/
    fs_atomic_t mRef;
};

typedef OBJ OBJVal;

class DiskKey {
public:
    friend class DiskKeyCmp;
    DiskKey()
    {
        mDiskId = YWB_UINT64_MAX;
        mSubPoolId = YWB_UINT32_MAX;
    }

    DiskKey(ywb_uint64_t diskid, ywb_uint32_t subpoolid) :
        mDiskId(diskid), mSubPoolId(subpoolid){}

    DiskKey(const DiskKey& key)
    {
        this->mDiskId = key.mDiskId;
        this->mSubPoolId = key.mSubPoolId;
    }

    DiskKey& operator= (const DiskKey& key)
    {
        if(this == &key)
        {
            return *this;
        }

        this->mDiskId = key.mDiskId;
        this->mSubPoolId = key.mSubPoolId;

        return *this;
    }

    bool operator==(const DiskKey& key)
    {
        if((mDiskId == key.mDiskId) &&
                (mSubPoolId == key.mSubPoolId))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    inline ywb_uint64_t GetDiskId();
    inline ywb_uint32_t GetSubPoolId();

    inline void SetDiskId(ywb_uint64_t id);
    inline void SetSubPoolId(ywb_uint32_t id);

private:
    /*disk id*/
    ywb_uint64_t mDiskId;
    /*sub-pool id*/
    ywb_uint32_t mSubPoolId;
};

/*for OBJ key comparison*/
class DiskKeyCmp {
public:
    bool operator()(const DiskKey& key1, const DiskKey& key2) const;
};

class DiskBaseProp {
public:
    friend class DiskBasePropCmp;
    enum DiskRaidType {
        DRT_raid0 = 0,
        DRT_raid1 = 1,
        DRT_raid3 = 3,
        DRT_raid5 = 5,
        DRT_raid6 = 6,
        DRT_raid10 = 10,
        DRT_cnt,
    };

    enum DiskClassType {
        DCT_extreme = 0,
        DCT_ssd,
        DCT_ent,
        DCT_sata,
        DCT_cloud,
        DCT_cnt,
    };

    DiskBaseProp()
    {
        mRaid = DRT_cnt;
        mWidth = 0;
        mDiskClass = DCT_cnt;
        mDiskRpm = 0;
        mDiskCap = 0;
    }

    DiskBaseProp(ywb_uint32_t raid, ywb_uint32_t width,
            ywb_uint32_t type, ywb_uint32_t rpm, ywb_uint32_t cap)
    {
        mRaid = raid;
        mWidth = width;
        mDiskClass = type;
        mDiskRpm = rpm;
        mDiskCap = cap;
    }

    DiskBaseProp(const DiskBaseProp& prop)
    {
        mRaid = prop.mRaid;
        mWidth = prop.mWidth;
        mDiskClass = prop.mDiskClass;
        mDiskRpm = prop.mDiskRpm;
        mDiskCap = prop.mDiskCap;
    }

    DiskBaseProp& operator= (const DiskBaseProp& prop)
    {
        if(this == &prop)
        {
            return *this;
        }

        mRaid = prop.mRaid;
        mWidth = prop.mWidth;
        mDiskClass = prop.mDiskClass;
        mDiskRpm = prop.mDiskRpm;
        mDiskCap = prop.mDiskCap;

        return *this;
    }

    inline ywb_uint32_t GetRaidType();
    inline ywb_uint32_t GetRaidWidth();
    inline ywb_uint32_t GetDiskClass();
    inline ywb_uint32_t GetDiskRPM();
    inline ywb_uint32_t GetDiskCap();

    inline void SetRaidType(ywb_uint32_t raid);
    inline void SetRaidWidth(ywb_uint32_t width);
    inline void SetDiskClass(ywb_uint32_t type);
    inline void SetDiskRPM(ywb_uint32_t rpm);
    inline void SetDiskCap(ywb_uint32_t cap);

    /*two disks are exactly match*/
    bool MatchExactly(const DiskBaseProp&);
    /*two disks are fuzzy match*/
    bool MatchFuzzy(const DiskBaseProp&);

    ywb_uint32_t DetermineDiskTier();

private:
    /*RAID level, comes from DiskRaidType*/
    ywb_uint32_t mRaid;
    /*disk width*/
    ywb_uint32_t mWidth;
    /*disk class, comes from DiskClassType*/
    ywb_uint32_t mDiskClass;
    /*disk RPM*/
    ywb_uint32_t mDiskRpm;
    /*disk total capacity, in unit of OBJ*/
    ywb_uint32_t mDiskCap;
};

class DiskBasePropCmp
{
public:
    bool operator()(const DiskBaseProp&, const DiskBaseProp&) const;
};

/*disk configuration*/
class Disk{
public:
    enum {
        DF_healthy = 0,
        DF_busy,
        DF_medium_err,
        DF_hardware_err,
        DF_removing,
        DF_no_obj,
        DF_suspicious,
        DF_no_advice,
    };

    Disk();
    Disk(DiskBaseProp& prop);
    /*
     * definition of Disk's deconstructor must be in .cpp if delete @mDiskIO
     * herein, otherwise there will be cross-reference issue
     **/
    ~Disk();

    inline ywb_uint32_t GetTier();
    inline void GetDiskIO(DiskIO** io);

    inline void SetBaseProp(DiskBaseProp& prop);
    inline void SetTier(ywb_uint32_t tier);
    inline void SetDiskIO(DiskIO*& io);

    inline void SetFlagComb(ywb_uint32_t flag);
    inline void SetFlagUniq(ywb_uint32_t flag);
    inline void ClearFlag(ywb_uint32_t flag);
    inline bool TestFlag(ywb_uint32_t flag);

//    inline void IncRef();
//    inline void DecRef();

    inline void PutOBJ(OBJVal*);
    inline ywb_uint32_t GetOBJCnt();

    void IncRef();
    void DecRef();
    ywb_status_t GetBaseProp(DiskBaseProp** prop);
    ywb_status_t AddOBJ(OBJKey&, OBJVal*);
    ywb_status_t GetOBJ(OBJKey&, OBJVal**);
    ywb_status_t RemoveOBJ(OBJKey&);

    /*
     * NOTE: following 2 pairs of function method are not thread-safe
     * See comments in Class Tier(ASTLogicalConfig.hpp).
     **/
    ywb_status_t GetFirstOBJ(OBJVal**);
    ywb_status_t GetNextOBJ(OBJVal**);

    ywb_status_t GetFirstOBJ(OBJVal**, OBJKey* objkey);
    ywb_status_t GetNextOBJ(OBJVal**, OBJKey* objkey);

    /*decide to which tier this disk belongs*/
    ywb_uint32_t DetermineDiskTier();
    /*summarize disk IO*/
    ywb_status_t SummarizeDiskIO();

private:
//    DiskKey mKey;
    /*disk basic property*/
    DiskBaseProp mProp;
    /*flags*/
    Flag mFlag;
    /*tier*/
    ywb_uint32_t mTier;
//    /*sub-pool id*/
//    ywb_uint32_t mSubPoolId;
    /*reference count*/
    fs_atomic_t mRef;

    /*used for traversing all OBJs of disk*/
    map<OBJKey, OBJVal*, OBJKeyCmp>::iterator mIt;
    /*
     * Manage all the OBJs belongs to this disk, use map to
     * accelerate the search/update/delete of OBJ
     *
     * No lock protection at current, but update to this map
     * is permitted, so must ensure no thread access it except
     * generator thread(which is responsible for updating it)
     **/
    map<OBJKey, OBJVal*, OBJKeyCmp> mObjsMap;
    /*disk IO statistics*/
    DiskIO* mDiskIO;
};

class Tier {
public:
    enum {
        TIER_0 = 0, TIER_1, TIER_2,

        /*MAKE SURE update Constant::TIER_NUM if TIER_cnt changes*/
        TIER_cnt,
    };

    enum {
        TF_no_disk = 0,
        TF_no_obj,
        TF_no_advice,
        TF_removing,
    };

    Tier() : mType(TIER_cnt), mRef(0), mDisksLock("DiskLock")
    {
        mFlag.Reset();
    }

    Tier(ywb_uint32_t tiertype) : mType(tiertype),
            mRef(0), mDisksLock("DiskLock")
    {
        mFlag.Reset();
    }

    ~Tier()
    {
        map<DiskKey, Disk*, DiskKeyCmp>::iterator iter;
        Disk* disk = NULL;

        iter = mDisks.begin();
        for(; iter != mDisks.end();)
        {
            disk = iter->second;
            mDisks.erase(iter++);
            disk->DecRef();
        }

//        mDisks.clear();
    }

    inline ywb_uint32_t GetType();
    inline void SetType(ywb_uint32_t val);

    inline void SetFlagComb(ywb_uint32_t flag);
    inline void SetFlagUniq(ywb_uint32_t flag);
    inline void ClearFlag(ywb_uint32_t val);
    inline bool TestFlag(ywb_uint32_t val);

    inline void IncRef();
    inline void DecRef();

    inline void PutOBJ(OBJVal*);
    inline void PutDisk(Disk*);

    ywb_uint32_t GetDiskCnt();
    ywb_uint32_t GetOBJCnt();

    ywb_status_t AddDisk(DiskKey&, Disk*);
    ywb_status_t GetDisk(DiskKey&, Disk**);
    ywb_status_t RemoveDisk(DiskKey&);

    /*
     * NOTE: following 3 pairs of function method are not thread-safe
     * though lock is used here for below reasons:
     * (1). Shared-iterator problem. Tier::mIt is shared among different
     *      threads, one thread may change it while another thread is
     *      accessing it.
     * (2). Deletion problem. GetFirst*** will prepare "next iterator"
     *      for GetNext***, but another thread may delete the object
     *      pointed by the "next iterator", which will lead to unexpected
     *      behavior when calling GetNext***.
     *
     * Currently, lock is used when adding/deleting/accessing one OBJ/
     * Disk, but it can only protect the atomicity of adding/deleting/
     * accessing one OBJ/Disk.
     *
     * For problem (1) mentioned above:
     * We must avoid calling GetFirst*** and GetNext*** from different
     * threads at the same time, if GetFirst*** and GetNext*** are used
     * in pair within one calling function methods continuously like:
     * ywb_status_t FunctionName(Parameter)
     * {
     *      ret = GetFirst(***);
     *      while((YWB_SUCCESS == ret) && (*** != NULL))
     *      {
     *          do something against ***;
     *          ret = GetNext(***);
     *      }
     * }
     *
     * then they can be replaced by using functions like this:
     * ywb_status_t Get***s(Vector<***>& ***vec)
     * {
     *      declare local iterator corresponding to container;
     *
     *      lock;
     *      initialize local iterator;
     *      traverse the container and put all the items into ***vec;
     *      unlock;
     *
     *      return;
     * }
     *
     * Otherwise, the calling threads must provide non-shared iterator
     * itself, and new function which take the thread-specific iterator
     * as parameter must be provided.
     *
     * For problem (2) mentioned above:
     * Currently we will deal with deletion cautiously, like below:
     * A. Receive deletion event;
     * B. Mark one entity as deleted but not delete from its container;
     * C. Delete at a certain time, at which time, make sure no access
     *    against the container of deleted entity.
     **/
    ywb_status_t GetFirstOBJ(OBJVal**);
    ywb_status_t GetNextOBJ(OBJVal**);

    ywb_status_t GetFirstDisk(Disk** disk);
    ywb_status_t GetNextDisk(Disk** disk);

    ywb_status_t GetFirstDisk(Disk** disk, DiskKey* key);
    ywb_status_t GetNextDisk(Disk** disk, DiskKey* key);

    /*
     * Following function method is provided to solve thread-
     * safety problem (1) mentioned above.
     * */
    ywb_uint32_t GetKeysAndDisks(list<DiskKey>& diskkeys,
            list<Disk*>& disks);

private:
    /*tier type*/
    ywb_uint32_t mType;
    fs_atomic_t mRef;
    Flag mFlag;

    /*used for traversing all disks in one tier*/
    map<DiskKey, Disk*, DiskKeyCmp>::iterator mIt;
    /*All the disks belongs to this disk*/
    map<DiskKey, Disk*, DiskKeyCmp> mDisks;
    /*protecting disks*/
    Mutex mDisksLock;
};

class SubPoolKey {
public:
    friend class SubPoolKeyCmp;
    SubPoolKey()
    {
        mOss = YWB_UINT32_MAX;
        mPoolId = YWB_UINT32_MAX;
        mSubPoolId = YWB_UINT32_MAX;
    }

    SubPoolKey(ywb_uint32_t ossid, ywb_uint32_t poolid,
            ywb_uint32_t subpoolid) : mOss(ossid),
            mPoolId(poolid), mSubPoolId(subpoolid){}

    SubPoolKey(const SubPoolKey& key)
    {
        this->mOss = key.mOss;
        this->mPoolId = key.mPoolId;
        this->mSubPoolId = key.mSubPoolId;
    }

    SubPoolKey& operator= (const SubPoolKey& key)
    {
        if(this == &key)
        {
            return *this;
        }

        this->mOss = key.mOss;
        this->mPoolId = key.mPoolId;
        this->mSubPoolId = key.mSubPoolId;

        return *this;
    }

    bool operator==(const SubPoolKey& key)
    {
        return ((mOss == key.mOss) && (mPoolId == key.mPoolId) &&
                (mSubPoolId == key.mSubPoolId));
    }

    inline ywb_uint32_t GetOss();
    inline ywb_uint32_t GetPoolId();
    inline ywb_uint32_t GetSubPoolId();

    inline void SetOss(ywb_uint32_t id);
    inline void SetPoolId(ywb_uint32_t id);
    inline void SetSubPoolId(ywb_uint32_t id);

private:
    ywb_uint32_t mOss;
    ywb_uint32_t mPoolId;
    ywb_uint32_t mSubPoolId;
};

/*for OBJ key comparison*/
class SubPoolKeyCmp {
public:
    bool operator()(const SubPoolKey& key1, const SubPoolKey& key2) const;
};

class SubPool {
public:
    enum TierComb{
        TC_ssd = 0x0001,
        TC_ent = 0x0002,
        TC_sata = 0x0004,
        TC_ssd_ent = 0x0003,
        TC_ssd_sata = 0x0005,
        TC_ent_sata = 0x0006,
        TC_ssd_ent_sata = 0x0007,
        TC_cnt,
    };

    enum {
        /*subpool has no disk*/
        SPF_no_disk = 0,
        /*subpool has no OBJ*/
        SPF_no_obj,
        /*subpool has no advice(though it may has OBJs)*/
        SPF_no_advice,
        /*subpool requires confirmation*/
        SPF_suspicious,
        /*subpool abandon migration*/
        SPF_no_migration,
        /*subpool is being removed*/
        SPF_removing,
    };

    SubPool(SubPoolKey& key) : mKey(key), mTiersLock("TiersLock"),
            mCurTierComb(TC_cnt), mRef(0)
    {
        mFlag.SetFlagComb(SubPool::SPF_no_disk);
        mFlag.SetFlagComb(SubPool::SPF_no_obj);
        mFlag.SetFlagComb(SubPool::SPF_no_advice);
    }

    ~SubPool()
    {
        list<Tier*>::iterator iter;
        Tier* tier = NULL;

        iter = mTiers.begin();
        for(; mTiers.end() != iter; )
        {
            tier = *iter;
            mTiers.erase(iter++);
            tier->DecRef();
        }

        mTiers.clear();
    }

    inline void GetSubPoolKey(SubPoolKey** key);
//    inline ywb_uint32_t GetLastTierComb();
    inline ywb_uint32_t GetCurTierComb();

    inline void SetSubPoolKey(SubPoolKey& key);
//    inline void SetLastTierComb(ywb_uint32_t val);
    inline void SetCurTierComb(ywb_uint32_t val);

    inline void SetFlagComb(ywb_uint32_t flag);
    inline void SetFlagUniq(ywb_uint32_t flag);
    inline void ClearFlag(ywb_uint32_t flag);
    inline bool TestFlag(ywb_uint32_t flag);

//    inline void IncRef();
//    inline void DecRef();

    inline void PutOBJ(OBJVal*);
    inline void PutDisk(Disk*);
    inline void PutTier(Tier*);

    void IncRef();
    void DecRef();

    ywb_status_t AddTier(Tier* tier);
    ywb_status_t GetTier(ywb_uint32_t tiertype, Tier** tier);
//    ywb_status_t RemoveTier(ywb_uint32_t tiertype);
    /*adjust current tier combination*/
    ywb_status_t AdjustTierComb(ywb_uint32_t tiertype, bool add);

    ywb_status_t AddDisk(DiskKey&, Disk*);
    ywb_status_t GetDisk(DiskKey&, Disk**);
    ywb_status_t RemoveDisk(DiskKey&);

    ywb_status_t AddOBJ(OBJKey&, OBJVal*);
    ywb_status_t GetOBJ(OBJKey&, OBJVal**);
    ywb_status_t RemoveOBJ(OBJKey&);

    ywb_status_t SummarizeDiskIO();

private:
    SubPoolKey mKey;
    list<Tier*> mTiers;
    /*protecting tiers*/
    Mutex mTiersLock;
    /*
     * FIXME:
     * mLastTierComb and mCurTierComb can not figure out
     * the history of subpool tier combination, so discard
     * mLastTierComb as a consequence.
     *
     * Recording the history of subpool tier combination is for
     * Arbitrator judging whether the configuration of subpool
     * changed since the advice is generated, the right solution
     * will be recording subpool's tier combination for each
     * generated advice with the tier combination value set to
     * tier combination at the cycle the advice is generated,
     * and Arbitrator taking current tier combination and the
     * record in advice as judgment. This will be done later
     * but not now!
     **/
//    /*previous tier combination in current cycle*/
//    ywb_uint32_t mLastTierComb;
    /*current tier combination in current cycle*/
    ywb_uint32_t mCurTierComb;
    /*reference count*/
    fs_atomic_t mRef;
    /*flags*/
    Flag mFlag;
};

class CollectLogicalConfigResponse;
class ReportDeleteOBJsMessage;
class ReportDeleteDisksMessage;
class ReportDeleteSubPoolsMessage;
class ReportDeleteOBJMessage;
class ReportDeleteDiskMessage;
class ReportDeleteSubPoolMessage;
class CollectLogicalConfigRequest;
class CollectLogicalConfigResponse;
class LogicalConfig {
public:
    /*
     * put the definition of constructor and de-constructor in .cpp
     * file to avoid cross reference issue
     **/
    LogicalConfig(AST* ast);
    virtual ~LogicalConfig();

    inline void GetAST(AST** ast);
    inline void PutOBJ(OBJVal*);
    inline void PutDisk(Disk*);
    inline void PutTier(Tier*);
    inline void PutSubPool(SubPool*);

    inline ywb_bool_t DiskId2KeyEmpty();
    inline ywb_bool_t SubPoolId2KeyEmpty();
    inline ywb_bool_t SubPoolsEmpty();
    inline ywb_bool_t DeletedSubPoolsEmpty();
    inline ywb_bool_t DeletedDisksEmpty();
    inline ywb_bool_t DeletedOBJsEmpty();

//    inline void SetCollectLogicalConfigResponse(
//            CollectLogicalConfigResponse*& response);
//    inline void SetDeleteOBJMessage(ReportDeleteOBJMessage*& msg);
//    inline void SetDeleteDiskMessage(ReportDeleteDiskMessage*& msg);
//    inline void SetDeleteSubPoolMessage(ReportDeleteSubPoolMessage*& msg);

    ywb_status_t GetDiskKeyFromDiskId(ywb_uint64_t diskid, DiskKey* key);
    ywb_status_t GetSubPoolKeyFromSubPoolId(
            ywb_uint32_t subpoolid, SubPoolKey* key);

    ywb_status_t GetOBJ(OBJKey&, OBJVal**);
    ywb_status_t GetDisk(DiskKey&, Disk**);
    ywb_status_t GetSubPool(SubPoolKey&, SubPool**);

    ywb_status_t AddOBJ(OBJKey&, OBJVal*);
    ywb_status_t AddDisk(DiskKey&, Disk* disk);
    ywb_status_t AddSubPool(SubPoolKey&, SubPool*);

    ywb_status_t RemoveOBJ(OBJKey&);
    ywb_status_t RemoveDisk(DiskKey&);
    /*remove subpool will implicitly remove all its contained disks*/
    ywb_status_t RemoveSubPool(SubPoolKey&);

    /*add OBJ/Disk/SubPool into deleted set*/
    ywb_status_t AddDeletedOBJ(OBJKey&);
    ywb_status_t AddDeletedDisk(DiskKey&);
    ywb_status_t AddDeletedSubPool(SubPoolKey&);

    /*only check whether the OBJ is in deleted set*/
    ywb_bool_t CheckOBJDeleted(OBJKey&);
    /*only check whether the disk is in deleted set*/
    ywb_bool_t CheckDiskDeleted(DiskKey&);
    /*only check whether the subpool is in deleted set*/
    ywb_bool_t CheckSubPoolDeleted(SubPoolKey&);

    /*
     * three steps will be taken:
     * 1. check whether the OBJ is in deleted set, if not, then
     * 2. check whether the corresponding disk is deleted, if not, then
     * 3. check whether the OBJ is in reported logical configuration
     **/
    ywb_bool_t CheckOBJExistence(OBJKey&);
    /*
     * two steps will be taken:
     * 1. check whether the disk is in deleted set, if not, then
     * 2. check whether the disk is in reported logical configuration
     *
     * NOTE: here will not check the existence of disk's backing subpool
     * for already adding disks corresponding to deleted subpool into
     * deleted disk set.
     **/
    ywb_bool_t CheckDiskExistence(DiskKey&);
    /*
     * two steps will be taken:
     * 1. check whether the subpool is in deleted set, if not, then
     * 2. check whether the subpool is in reported logical configuration
     **/
    ywb_bool_t CheckSubPoolExistence(SubPoolKey&);

    /*
     * NOTE: following 1 pair of function method are not thread-safe
     * See comments in Class Tier(ASTLogicalConfig.hpp).
     **/
    ywb_status_t GetFirstSubPool(SubPool** subpool, SubPoolKey* key);
    ywb_status_t GetNextSubPool(SubPool** subpool, SubPoolKey* key);

    /*
     * Following function method is provided to solve shared-
     * iterator problem.
     **/
    ywb_uint32_t GetKeysAndSubPools(list<SubPoolKey>& subpoolkeys,
            list<SubPool*>& subpools);

    /*reset logical configuration to newly constructed state*/
    ywb_status_t Reset();
    /*delete those OBJs/Disks/SubPools in deleted set*/
    ywb_status_t Consolidate();

    /*launch collect logical configuration and wait for response*/
    virtual ywb_status_t CollectLogicalConfig(ywb_uint32_t ossid);
    /*resolve logical config*/
    virtual ywb_status_t ResolveLogicalConfig(
            CollectLogicalConfigResponse* resp);
    /*(Deprecated)resolve delete OBJs message*/
    virtual ywb_status_t ResolveDeletedOBJs(ReportDeleteOBJsMessage* resp);
    /*(Deprecated)resolve delete disks message*/
    virtual ywb_status_t ResolveDeletedDisks(ReportDeleteDisksMessage* resp);
    /*(Deprecated)resolve delete subpools message*/
    virtual ywb_status_t ResolveDeletedSubPools(
            ReportDeleteSubPoolsMessage* resp);
    /*resolve delete OBJ message*/
    virtual ywb_status_t ResolveDeletedOBJ(ReportDeleteOBJMessage* resp);
    /*resolve delete disk message*/
    virtual ywb_status_t ResolveDeletedDisk(ReportDeleteDiskMessage* resp);
    /*resolve delete subpool message*/
    virtual ywb_status_t ResolveDeletedSubPool(
            ReportDeleteSubPoolMessage* resp);
#ifdef WITH_INITIAL_PLACEMENT
    virtual ywb_status_t ResolveRelocationType(NotifyRelocationType* resp);
#endif

private:
    /*
     * disk id to key mapping table
     * make sure disk id is universal unique
     **/
    map<ywb_uint64_t, DiskKey> mDiskId2Key;
    /*
     * subpool id to key mapping table
     * make sure sub-pool id is universal unique
     **/
    map<ywb_uint32_t, SubPoolKey> mSubPoolId2Key;
//    map<DiskKey, Disk*, DiskKeyCmp> mDisks;
    /*subpools management*/
    map<SubPoolKey, SubPool*, SubPoolKeyCmp> mSubPools;
    /*used for traversing subpools*/
    map<SubPoolKey, SubPool*, SubPoolKeyCmp>::iterator mIt;
    /*protect subpools*/
    Mutex mSubPoolsLock;

    /*
     * at the beginning of one cycle, logical configuration will
     * report the configuration of subpools/disks/OBJs, and set
     * deleted subpools/disks/OBJs to empty.
     *
     * during the cycle, subpools/disks/OBJs may be deleted, the
     * Monitor will report these deletion to AST, AST remember
     * deletions in deleted subpools/disks/OBJs set but without
     * updating the configuration reported at the beginning of
     * cycle.
     *
     * newly added subpools/disks/OBJs will not be involved in
     * current cycle's learning and decision.
     **/
    set<OBJKey, OBJKeyCmp> mDeletedOBJs;
    set<DiskKey, DiskKeyCmp> mDeletedDisks;
    set<SubPoolKey, SubPoolKeyCmp> mDeletedSubPools;

    /*collect config request, constructed internally*/
    CollectLogicalConfigRequest* mCollectConfigReq;
//    /*collect config response, set externally*/
//    CollectLogicalConfigResponse* mCollectConfigResp;
//    /*delete OBJ message, set externally*/
//    ReportDeleteOBJMessage* mDeleteOBJMsg;
//    /*delete disk message, set externally*/
//    ReportDeleteDiskMessage* mDeleteDiskMsg;
//    /*delete subpool message, set externally*/
//    ReportDeleteSubPoolMessage* mDeleteSubPoolMsg;

    /*back reference to AST*/
    AST* mAst;
};

#include "AST/ASTLogicalConfigInline.hpp"

#endif

/* vim:set ts=4 sw=4 expandtab */
