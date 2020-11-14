#ifndef __OSS_SIMULATOR_HPP__
#define __OSS_SIMULATOR_HPP__

#include <sstream>
#include "common/FsInts.h"
#include "common/FsStatus.h"
#include "AST/AST.hpp"
#include "AST/MessageExpress.hpp"
#include "AST/MessageImpl.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "OSSCommon/ControlCenter.hpp"
#include "OSSCommon/ServiceMonClient.hpp"

/*for coordinating addition/removal of subpool/disk/OBJ*/
class OSSCoordinator
{
public:
    enum OpType
    {
        OP_none = 0,
        OP_delete_obj,
        OP_delete_disk,
        OP_delete_subpool,
        OP_add_subpool,
        OP_add_disk,
        OP_add_obj,
        OP_cnt,
    };

    OSSCoordinator() : mOssId(Constant::OSS_ID),
    mNextPoolId(Constant::POOL_ID),
    mNextSubPoolId(Constant::SUBPOOL_ID),
    mNextDiskId(Constant::DISK_ID),
    mNextChunkIndex(Constant::DEFAULT_CHUNK_INDEX),
    mNextInodeId(Constant::DEFAULT_INODE)
    {

    }

    void AddOneSubPool(SubPoolKey** subpool);
    void AddOneDisk(SubPoolKey& subpoolkey,
            ywb_uint32_t diskclass, DiskInfo** disk);
    void AddOneOBJ(DiskKey& diskkey, OBJInfo** obj, ywb_uint32_t seed);
    void AddBatchOfOBJ(DiskKey& diskkey, ywb_uint32_t batchnum);
    void AddOneThreeTierSubPoolWithDiskAndOBJ(SubPoolKey** subpool);

    /*deletion will always from the beginning of list*/
    ywb_status_t DeleteOneSubPool(SubPoolKey* subpool);
    /*deletion will always from the beginning of list*/
    ywb_status_t DeleteOneDisk(DiskInfo* disk);
    /*deletion will always from the beginning of list*/
    ywb_status_t DeleteOneOBJ(OBJInfo* obj);

    ywb_status_t GetLastSubPool(SubPoolKey* subpoolkey);
    ywb_status_t GetLastDisk(DiskInfo* diskinfo);

    ywb_status_t GetFirstDiskFromBack(DiskInfo* diskinfo);
    ywb_status_t GetNextDiskFromBack(DiskInfo* diskinfo);

    bool IsSubPoolEmpty();

    ywb_status_t FillCollectConfigResp(CollectLogicalConfigResponse*);
    ywb_status_t FillCollectOBJResp(CollectOBJResponse*);

    ywb_status_t Initialize();

private:
    ywb_uint32_t mOssId;

    SubPoolKey mNextSubPoolKey;

    DiskKey mNextDiskKey;
    DiskBaseProp mNextDiskProp;
    DiskInfo mDiskInfo;

    OBJKey mNextOBJKey;
    ChunkIOStat mNextChunkIOStat;
    OBJInfo mNextOBJInfo;

    ywb_uint32_t mNextPoolId;
    ywb_uint32_t mNextSubPoolId;
    ywb_uint64_t mNextDiskId;
    ywb_uint32_t mNextChunkIndex;
    ywb_uint64_t mNextInodeId;

    list<SubPoolKey> mSubPools;
    list<DiskInfo> mDisks;
    list<OBJInfo> mOBJs;

    list<DiskInfo>::reverse_iterator mDiskRIter;
};

class OSSSimulator;
class OSSControlCenterMock;
class OSSMonClientMock : public StorageServiceMonClient
{
public:
    OSSMonClientMock(OSSSimulator* osssim,
            OSSControlCenterMock* osscontrolcenter);
    ~OSSMonClientMock();

    /*all these are pure virtual functions derived from MonClient*/
//    ywb_status_t HandleMonitorMessage(yfs_sid_t sid,
//            ywb_uint64_t connid,NMsgblock* msg);
//    void HandleDropped(int type, ywb_uint32_t sid, ywb_uint64_t connid);
    ywb_status_t Init();
//    ywb_uint64_t AllocConnId();
    ywb_status_t initialize();

    /*
     * Deprecated.
     * Retrive logical configuration via ControlCenter instead
     **/
    ywb_status_t RetrieveLogicalConfig(ywb_uint32_t parentmsgno,
            ywb_uint32_t ossid);
    /*
     * Deprecated.
     * Ditto
     **/
    ywb_status_t ReceiveLogicalConfig(NMsgblock *msg);

private:
    OSSSimulator* mOSSSim;
    OSSControlCenterMock* mOSSCenter;
    /*Deprecated. Retrive logical configuration via ControlCenter instead*/
    CollectLogicalConfigResponse* mCollectConfigResp;
    /*sequence number of received message*/
    ywb_uint32_t mSeqno;
};

class OSSControlCenterMock : public ControlCenter
{
public:
    OSSControlCenterMock(OSSSimulator* osssim);
    ~OSSControlCenterMock();

    ywb_status_t RetrieveLogicalConfig(ywb_uint32_t parentmsgno,
            ywb_uint32_t ossid);
    ywb_status_t RetrieveOBJ(
            ywb_uint32_t parentmsgno, ywb_uint32_t ossid);

private:
    OSSSimulator* mOSSSim;
    OSSMonClientMock* mMonClntMock;
    CollectLogicalConfigResponse* mCollectConfigResp;
    CollectOBJResponse* mCollectOBJResp;
    /*sequence number of collect configuration message*/
    ywb_uint32_t mCollectConfigSeqno;
    /*sequence number of collect IO message*/
    ywb_uint32_t mCollectIOSeqno;
};

class OSSSimulator
{
public:
    OSSSimulator();
    ~OSSSimulator();

    inline void GetOSSCoordinator(OSSCoordinator** coordinator)
    {
        *coordinator = mOSSCoordinator;
    }

    inline void GetOSSControlCenterMock(OSSControlCenterMock** center)
    {
        *center = mOSSCenter;
    }

    ywb_status_t Initialize();
    ywb_status_t Start();
    ywb_status_t Stop();
    void StartMessageService();
    void StopMessageService();

    /*dynamically generate configuration and OBJ IO*/
    ywb_status_t DynamicalConfig();
    void Send(Message*);

private:
    OSSCoordinator* mOSSCoordinator;
    OSSControlCenterMock* mOSSCenter;

    AST* mAst;
    MessageManager* mMsgMgr;
    Message* mEnableASTMsg;
    Message* mDisableASTMsg;

    /*current cycle, start from 0*/
    ywb_uint32_t mCurCycle;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
