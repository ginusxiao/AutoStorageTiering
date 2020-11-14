#ifndef __AST_MESSAGE_IMPL_HPP__
#define __AST_MESSAGE_IMPL_HPP__

#include "common/FsInts.h"
#include "OSSCommon/ControlCenter.hpp"
#include "OSSCommon/ServiceMonClient.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "AST/Deprecated/ASTMessageExpress.hpp"
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
    CollectOBJRequest(ywb_uint32_t oss) :
        Message("oss", OP_collect_OBJ_req)
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
        Message("ast", OP_collect_logical_config_resp)
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

    inline void SetSeqNo(ywb_uint32_t seq)
    {
        mSeqno = seq;
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
class CollectOBJResponse : public Message{
public:
    CollectOBJResponse() : Message("ast", OP_collect_OBJ_resp)
    {
        mSeqno = YWB_UINT32_MAX;
    }

    ~CollectOBJResponse()
    {
        mOBJs.clear();
    }

    inline ywb_uint32_t GetSeqNo()
    {
        return mSeqno;
    }

    inline void SetSeqNo(ywb_uint32_t seq)
    {
        mSeqno = seq;
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
    /*sequence number for the request-response pair*/
    ywb_uint32_t mSeqno;
};

/*request for reporting deleted OBJs*/
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

/*request for reporting deleted disks*/
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

/*request for reporting deleted subpools*/
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
