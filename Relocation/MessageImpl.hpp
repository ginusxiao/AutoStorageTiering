#ifndef __MESSAGE_IMPL_HPP__
#define __MESSAGE_IMPL_HPP__

#include "common/FsInts.h"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "AST/MessageExpress.hpp"
#include "AST/AST.hpp"


/*request for collecting logical configuration*/
class CollectLogicalConfigRequest : public Message{
public:
    CollectLogicalConfigRequest(ywb_uint32_t oss) :
        Message("oss", OP_collect_logical_config_req)
    {
        mOss = oss;
    }

    inline ywb_uint32_t GetOss()
    {
        return mOss;
    }

    inline void SetOss(ywb_uint32_t oss)
    {
        mOss = oss;
    }

private:
    /*for which oss the collection is*/
    ywb_uint32_t mOss;
};

/*request for collecting OBJ IO*/
class CollectOBJRequest : public Message{
public:
    enum {
        F_prepare = 0,
        F_normal,
        F_post,
        F_max,
    };

    CollectOBJRequest(ywb_uint32_t oss) :
        Message("oss", OP_collect_OBJ_req)
    {
        mOss = oss;
        mFlag = CollectOBJRequest::F_max;
    }

    inline ywb_uint32_t GetOss()
    {
        return mOss;
    }

    inline ywb_uint32_t GetFlag()
    {
        return mFlag;
    }

    inline void SetOss(ywb_uint32_t oss)
    {
        mOss = oss;
    }

    inline void SetFlag(ywb_uint32_t flag)
    {
        mFlag = flag;
    }

private:
    /*for which oss the collection is*/
    ywb_uint32_t mOss;
    ywb_uint32_t mFlag;
};

/*Disk info collected from OSS*/
class DiskInfo
{
public:
    DiskInfo() {}

    DiskInfo(ywb_uint64_t diskid, ywb_uint32_t subpoolid,
            DiskBaseProp::DiskRaidType raid, ywb_uint32_t width,
            DiskBaseProp::DiskClassType type,
            ywb_uint32_t rpm, ywb_uint32_t cap)
    {
        mKey.SetDiskId(diskid);
        mKey.SetSubPoolId(subpoolid);

        mProp.SetRaidType(raid);
        mProp.SetRaidWidth(width);
        mProp.SetDiskClass(type);
        mProp.SetDiskRPM(rpm);
        mProp.SetDiskCap(cap);
    }

    DiskInfo(DiskKey& key, DiskBaseProp& baseprop) :
        mKey(key), mProp(baseprop) {}

    DiskInfo(const DiskInfo& diskinfo)
    {
        mKey = diskinfo.mKey;
        mProp = diskinfo.mProp;
    }

    DiskInfo& operator= (const DiskInfo& diskinfo)
    {
        if(this == &diskinfo)
        {
            return *this;
        }

        mKey = diskinfo.mKey;
        mProp = diskinfo.mProp;

        return *this;
    }

    inline void GetDiskKey(DiskKey** key)
    {
        *key = &mKey;
    }

    inline void GetBaseProp(DiskBaseProp** prop)
    {
        *prop = &mProp;
    }

    inline void SetDiskKey(DiskKey& key)
    {
        mKey = key;
    }

    inline void SetBaseProp(DiskBaseProp& prop)
    {
        mProp = prop;
    }

private:
    DiskKey mKey;
    DiskBaseProp mProp;
};

/*response for collect logical configuration*/
class CollectLogicalConfigResponse : public Message{
public:
    CollectLogicalConfigResponse() :
        Message("ast", OP_collect_logical_config_resp), mStatus(YWB_SUCCESS)
    {
        mSeqno = YWB_UINT32_MAX;
    }

    ~CollectLogicalConfigResponse()
    {
        mDisks.clear();
        mSubPools.clear();
    }

    inline ywb_uint32_t GetSeqNo()
    {
        return mSeqno;
    }

    inline ywb_status_t GetStatus()
    {
        return mStatus;
    }

    inline void SetSeqNo(ywb_uint32_t seq)
    {
        mSeqno = seq;
    }

    inline void SetStatus(ywb_status_t status)
    {
        mStatus = status;
    }

    void AddDisk(DiskInfo& key);
    void AddSubPool(SubPoolKey& key);

    void ShowMessage();

    ywb_status_t GetFirstSubPool(SubPoolKey* subpool);
    ywb_status_t GetNextSubPool(SubPoolKey* subpool);

    ywb_status_t GetFirstDisk(DiskInfo* disk);
    ywb_status_t GetNextDisk(DiskInfo* disk);

    bool IsSubPoolEmpty();
    bool IsDiskEmpty();

    void Reset();

//    void PackRequest();
//    void UnPackRequest();

private:
    /*subpool set*/
    vector<SubPoolKey> mSubPools;
    vector<SubPoolKey>::iterator mSubPoolIter;

    /*disk set*/
    vector<DiskInfo> mDisks;
    vector<DiskInfo>::iterator mDiskIter;

    /*sequence number for the request-response pair*/
    ywb_uint32_t mSeqno;
    /*response status*/
    ywb_status_t mStatus;
};

/*OBJ info collected from OSS*/
class OBJInfo{
public:
    OBJInfo() {}
    OBJInfo(OBJKey& objkey, ChunkIOStat& stat) :
        mKey(objkey), mIOStat(stat) {}

    OBJInfo(const OBJInfo& objinfo)
    {
        mKey = objinfo.mKey;
        mIOStat = objinfo.mIOStat;
    }

    OBJInfo& operator= (const OBJInfo& objinfo)
    {
        if(this == &objinfo)
        {
            return *this;
        }

        mKey = objinfo.mKey;
        mIOStat = objinfo.mIOStat;

        return *this;
    }

    inline void GetOBJKey(OBJKey** key)
    {
        *key = &mKey;
    }

    inline void GetChunkIOStat(ChunkIOStat** stat)
    {
        *stat = &mIOStat;
    }

    inline void SetOBJKey(OBJKey& key)
    {
        mKey = key;
    }

    inline void SetChunkIOStat(ChunkIOStat& stat)
    {
        mIOStat = stat;
    }

private:
    OBJKey mKey;
    ChunkIOStat mIOStat;
};

/*response for collect OBJ IO*/
class CollectOBJResponseFragment
{
public:
    CollectOBJResponseFragment() : mStatus(YFS_ENODATA)
    {

    }

    ~CollectOBJResponseFragment()
    {
        mOBJs.clear();
    }

    inline ywb_status_t GetStatus()
    {
        return mStatus;
    }

    inline void SetStatus(ywb_status_t status)
    {
        mStatus = status;
    }

    inline ywb_uint32_t GetOBJCnt()
    {
        return mOBJs.size();
    }

    void AddOBJ(OBJInfo& key);

    ywb_status_t GetFirstOBJ(OBJInfo* obj);
    ywb_status_t GetNextOBJ(OBJInfo* obj);

    void Reset();
//    void PackRequest();
//    void UnPackRequest();

private:
    /*OBJ set*/
    vector<OBJInfo> mOBJs;
    vector<OBJInfo>::iterator mOBJIter;
    /*response status*/
    ywb_status_t mStatus;
};

class CollectOBJResponse  : public Message
{
public:
    enum Flag
    {
        /*collect IO response has next batch/fragmentation*/
        F_next_batch = 0,
        /*end of collect IO response*/
        F_end,
        F_max,
    };

    CollectOBJResponse() : Message("ast", OP_collect_OBJ_resp),
    mFlag(F_max), mStatus(YFS_ENODATA), mFragmentsNum(0), mFragments(NULL)
    {
        mSeqno = YWB_UINT32_MAX;
    }

    virtual ~CollectOBJResponse()
    {
        mFragments = NULL;
        mFragmentsNum = 0;
    }

    inline ywb_uint32_t GetFlag()
    {
        return mFlag;
    }

    inline ywb_status_t GetStatus()
    {
        return mStatus;
    }

    inline ywb_uint32_t GetSeqNo()
    {
        return mSeqno;
    }

    inline ywb_uint32_t GetFragmentNum()
    {
        return mFragmentsNum;
    }

    inline CollectOBJResponseFragment** GetFragments()
    {
        return mFragments;
    }

    inline void SetFlag(ywb_uint32_t flag)
    {
        mFlag = flag;
    }

    inline void SetStatus(ywb_status_t status)
    {
        mStatus = status;
    }

    inline void SetSeqNo(ywb_uint32_t seq)
    {
        mSeqno = seq;
    }

    inline void SetFragmentNum(ywb_uint32_t num)
    {
        mFragmentsNum = num;
    }

    inline void SetFragments(CollectOBJResponseFragment** frag)
    {
        YWB_ASSERT(frag != NULL);
        mFragments = frag;
    }

    void Reset();

private:
    ywb_uint32_t mFlag;
    ywb_status_t mStatus;
    /*sequence number for the request-response pair*/
    ywb_uint32_t mSeqno;
    ywb_uint32_t mFragmentsNum;
    /*array of CollectOBJResponseFragment*/
    CollectOBJResponseFragment** mFragments;
};

/*
 * Deprecated.
 *
 * Request for reporting at least one deleted OBJs.
 * It is deprecated for:
 * (1) there is no such scenario that send multiple deleted OBJs at once,
 *     disk deletion may involve multiple deleted OBJs, but disk deletion
 *     can have its own message which can leads to cascading deletion of
 *     OBJs when processing disk deletion message
 * (2) To accommodate more than one deleted OBJs, STL vector is adopted,
 *     which cause the memory optimization such as memory cache impossible
 *
 * Use ReportDeletdOBJMessage instead.
 * */
class ReportDeleteOBJsMessage : public Message{
public:
    ReportDeleteOBJsMessage() : Message("ast", OP_report_delete_OBJs) {}

    ~ReportDeleteOBJsMessage()
    {
        mObjs.clear();
    }

    void AddOBJ(OBJKey& key);
    ywb_status_t GetFirstOBJ(OBJKey* obj);
    ywb_status_t GetNextOBJ(OBJKey* obj);

    void Reset();
//    void PackRequest();
//    void UnPackRequest();

private:
    /*deleted OBJ set*/
    vector<OBJKey> mObjs;
    vector<OBJKey>::iterator mIt;
};

class ReportDeleteOBJMessage : public Message
{
public:
    ReportDeleteOBJMessage(OBJKey obj) :
        Message("ast", OP_report_delete_OBJ), mOBJ(obj) {}

    ~ReportDeleteOBJMessage() {}

    inline void GetOBJ(OBJKey** objkey)
    {
        *objkey = &mOBJ;
    }

private:
    OBJKey mOBJ;
};

/*
 * Deprecated.
 *
 * Request for reporting at least one deleted disks
 * It is deprecated for:
 * (1) there is no such scenario that send multiple deleted Disks at once,
 *     SubPool deletion may involve multiple deleted Disks, but SubPool
 *     deletion can have its own message which can leads to cascading
 *     deletion of Disks when processing SubPool deletion message
 * (2) To accommodate more than one deleted Disks, STL vector is adopted,
 *     which cause the memory optimization such as memory cache impossible
 *
 * Use ReportDeleteDiskMessage instead.
 * */
class ReportDeleteDisksMessage : public Message{
public:
    ReportDeleteDisksMessage() : Message("ast", OP_report_delete_disks) {}

    ~ReportDeleteDisksMessage()
    {
        mDisks.clear();
    }

    void AddDisk(DiskKey& key);
    ywb_status_t GetFirstDisk(DiskKey* obj);
    ywb_status_t GetNextDisk(DiskKey* obj);

    void Reset();
//    void PackRequest();
//    void UnPackRequest();

private:
    /*deleted disk set*/
    vector<DiskKey> mDisks;
    vector<DiskKey>::iterator mIt;
};

class ReportDeleteDiskMessage : public Message
{
public:
    ReportDeleteDiskMessage(DiskKey disk) :
        Message("ast", OP_report_delete_disk), mDisk(disk) {}

    ~ReportDeleteDiskMessage() { }

    inline void GetDisk(DiskKey** diskkey)
    {
        *diskkey = &mDisk;
    }

private:
    DiskKey mDisk;
};

/*
 * Deprecated.
 *
 * Request for reporting at least one deleted SubPools
 * It is deprecated for:
 * (1) there is no such scenario that send multiple deleted SubPools at once
 * (2) To accommodate more than one deleted SubPools, STL vector is adopted,
 *     which cause the memory optimization such as memory cache impossible
 *
 * Use ReportDeleteSubPoolMessage instead.
 * */
class ReportDeleteSubPoolsMessage : public Message{
public:
    ReportDeleteSubPoolsMessage() : Message("ast", OP_report_delete_subpools) {}

    ~ReportDeleteSubPoolsMessage()
    {
        mSubPools.clear();
    }

    void AddSubPool(SubPoolKey& key);
    ywb_status_t GetFirstSubPool(SubPoolKey* obj);
    ywb_status_t GetNextSubPool(SubPoolKey* obj);

    void Reset();
//    void PackRequest();
//    void UnPackRequest();

private:
    /*deleted subpool set*/
    vector<SubPoolKey> mSubPools;
    vector<SubPoolKey>::iterator mIt;
};

class ReportDeleteSubPoolMessage : public Message
{
public:
    ReportDeleteSubPoolMessage(SubPoolKey subpool) :
        Message("ast", OP_report_delete_subpool), mSubPool(subpool) {}

    ~ReportDeleteSubPoolMessage() {}

    inline void GetSubPool(SubPoolKey** subpoolkey)
    {
        *subpoolkey = &mSubPool;
    }

private:
    SubPoolKey mSubPool;
};

#ifdef WITH_INITIAL_PLACEMENT
class NotifyRelocationType : public Message{
public:
    NotifyRelocationType() : Message("ast", OP_notify_relocation_type)
    {

    }

    virtual ~NotifyRelocationType()
    {
        mRelocationTypes.clear();
    }

    ywb_status_t AddRelocationType(SubPoolKey& subpoolkey, ywb_int32_t val);
    ywb_status_t Assign(map<SubPoolKey, ywb_int32_t, SubPoolKeyCmp>* relocationtypes);
    ywb_status_t GetFirstRelocationType(SubPoolKey& subpoolkey, ywb_int32_t& val);
    ywb_status_t GetNextRelocationType(SubPoolKey& subpoolkey, ywb_int32_t& val);
    ywb_bool_t Empty();
    void Reset();

private:
    map<SubPoolKey, ywb_int32_t, SubPoolKeyCmp> mRelocationTypes;
    map<SubPoolKey, ywb_int32_t, SubPoolKeyCmp>::iterator mIt;
};

class NotifySwapRequirement : public Message{
public:
    NotifySwapRequirement() : Message("ast", OP_notify_swap_requirement)
    {

    }

    virtual ~NotifySwapRequirement()
    {
        mSwapRequiredDisks.clear();
    }

    ywb_status_t AddRequireSwapDisk(DiskKey& diskkey, ywb_int32_t val);
    ywb_status_t Assign(map<DiskKey, ywb_int32_t, DiskKeyCmp>* disks);
    ywb_status_t GetFirstSwapDisk(DiskKey& diskkey, ywb_int32_t& val);
    ywb_status_t GetNextSwapDisk(DiskKey& diskkey, ywb_int32_t& val);
    ywb_bool_t Empty();
    void Reset();

private:
    /*
     * manage all disks requiring for swap in current cycle, the value is a
     * combination of swap level and swap direction(level << 16 + direction)
     *
     * set whenever one disk requires for swap or disk's swap level changes,
     * cleared whenever all its entries has been retrieved
     * */
    map<DiskKey, ywb_int32_t, DiskKeyCmp> mSwapRequiredDisks;
    map<DiskKey, ywb_int32_t, DiskKeyCmp>::iterator mIt;
};

class NotifySwapCancellation : public Message{
public:
    NotifySwapCancellation() : Message("ast", OP_notify_swap_cancellation)
    {

    }

    virtual ~NotifySwapCancellation()
    {
        mSwapCancelledDisks.clear();
    }

    ywb_status_t AddCancelSwapDisk(DiskKey&);
    ywb_status_t Assign(list<DiskKey>* disks);
    ywb_status_t GetSwapCancelledDisks(list<DiskKey>* disks);
    ywb_bool_t Empty();
    void Reset();

private:
    /*
     * manage all disks canceling swap in current cycle
     *
     * set whenever one disk no longer requires for swap or disk is in removing/
     * removed state, cleared whenever all its entries has been retrieved
     * */
    list<DiskKey> mSwapCancelledDisks;
};
#endif

class ASTProxy : public TargetProxy{
public:
    ASTProxy(AST*);
    ~ASTProxy();

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    inline void GetLogicalConfig(LogicalConfig** config)
    {
        *config = mConfig;
    }

    inline void GetIOManager(IOManager** mgr)
    {
        *mgr = mIO;
    }

    ywb_status_t Start();
    ywb_status_t Stop();
    ywb_status_t Handle(Message*);

private:
    //field used for storing AST related messages
    AST* mAst;
    LogicalConfig* mConfig;
    IOManager* mIO;
};

class ControlCenter;
class StorageServiceMonClient;
class OSSProxy : public TargetProxy{
public:
    OSSProxy(ControlCenter*);
    ~OSSProxy();

    inline void GetControlCenter(ControlCenter** center)
    {
        *center = mCenter;
    }

    inline void GetMonClnt(StorageServiceMonClient** clnt)
    {
        *clnt = mMonClnt;
    }

    ywb_status_t Start();
    ywb_status_t Stop();
    ywb_status_t Handle(Message*);

private:
    ControlCenter* mCenter;
    StorageServiceMonClient* mMonClnt;
};


#endif

/* vim:set ts=4 sw=4 expandtab */
