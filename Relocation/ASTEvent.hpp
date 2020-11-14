#ifndef __AST_EVENT_HPP__
#define __AST_EVENT_HPP__

#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTArb.hpp"

class Event {
public:
    enum Type
    {
        ET_none = 0,

        /*start learning*/
        ET_gen_learning = 0x0001,
//        /*
//         * consolidate logical configuration, also destroy all advise
//         * for past decision window.
//         **/
//        ET_gen_consolidate,
        /*generate advise*/
        ET_gen_advise,
        /*stop generator*/
        ET_gen_stop,

        /*start generating first batch of plans for each subpool*/
        ET_exe_planning = 0x0101,
        /*
         * one plan migrated successfully, will trigger successive
         * plan generation
         **/
        ET_exe_migration_completed,
//        /*deprecated, plan for new decision window is coming*/
//        ET_exe_timedout,
        /*
         * one migration expired
         * */
        ET_exe_migration_expired,
        /*
         * stop work for the past decision window, promise not touch
         * logical configuration and advise until Generator thread
         * generates new advise.
         **/
        ET_exe_stall,
        /*stop executor*/
        ET_exe_stop,

        /*event against HUB*/
        ET_hub_stop = 0x0201,
        ET_hub_enable_ast,
        ET_hub_disable_ast,
        ET_hub_enable_ast_echo,
        ET_hub_disable_ast_echo,
        ET_hub_add_disk,
        ET_hub_remove_disk,
        ET_hub_add_obj,
        ET_hub_delete_obj,
        ET_hub_access_obj,
        ET_hub_collect_config_view,
        ET_hub_collect_io,
        ET_hub_migrate,
        ET_hub_migration_completed,
#ifdef WITH_INITIAL_PLACEMENT
        /*update one subpool's relocation type*/
        ET_hub_update_relocation_type,
        ET_hub_require_swap_disk,
        ET_hub_cancel_swap_disk,
#endif

//#ifdef AST_INTEGRATION_WITH_OSS_SIMULATOR
        ET_osssimulator_migrate,
        ET_osssimulator_dynamical_config,
        ET_migration_done_simulate,
        ET_osssimulator_stop,
//#endif

        ET_end,
    };

    Event() : mType(ET_none), mRefCnt(0) {}
    Event(ywb_uint32_t type) : mType(type), mRefCnt(0) {}
    virtual ~Event() {}

    inline ywb_uint32_t GetType()
    {
        return mType;
    }

    inline void SetType(ywb_uint32_t type)
    {
        mType = type;
    }

    inline void IncRef()
    {
        fs_atomic_inc(&mRefCnt);
    }

    inline void DecRef()
    {
        if(fs_atomic_dec_and_test(&mRefCnt))
        {
            delete this;
        }
    }

private:
    ywb_uint32_t mType;
    fs_atomic_t mRefCnt;
};

namespace GeneratorEventSet {
    /*special for learning task*/
    class LearningEvent : public Event
    {
    public:
        LearningEvent(ywb_uint32_t phase, void* resp) :
            Event(Event::ET_gen_learning), mPhase(phase),
            mAdviceAfterLearning(false), mResp(resp) {}

        inline ywb_uint32_t GetPhase()
        {
            return mPhase;
        }

        inline ywb_bool_t GetAdviceAfterLearning()
        {
            return mAdviceAfterLearning;
        }

        inline void GetResp(void** resp)
        {
            *resp = mResp;
        }

        inline void SetAdviceAfterLearning(ywb_uint32_t val)
        {
            mAdviceAfterLearning = val;
        }

    private:
        /*
         * refer to RelocationLearning::Phase, reflect in
         * which phase current event is
         **/
        ywb_uint32_t mPhase;
        /*generate advise after learning*/
        ywb_bool_t mAdviceAfterLearning;
        /*pointer to message buffer corresponding*/
        void* mResp;
    };

    class AdviceEvent : public Event
    {
    public:
        AdviceEvent() : Event(Event::ET_gen_advise) {}
        virtual ~AdviceEvent() {}
    };

    class StopEvent : public Event
    {
    public:
        StopEvent() : Event(Event::ET_gen_stop) {}
        virtual ~StopEvent() {}
    };
}

namespace ExecutorEventSet {
    class PlanningEvent : public Event
    {
    public:
        PlanningEvent() : Event(Event::ET_exe_planning) {}
        virtual ~PlanningEvent() {}
    };

    /*special for migration completion event*/
    class MigrationCompletedEvent : public Event
    {
    public:
        MigrationCompletedEvent(SubPoolKey& subpool, DiskKey& sourcekey,
                DiskKey& targetkey, ArbitrateeKey& arbitratee,
                ywb_uint64_t epoch, ywb_status_t err) :
                    Event(Event::ET_exe_migration_completed),
                    mSubPoolKey(subpool), mSourceKey(sourcekey),
                    mTargetKey(targetkey), mArbitrateeKey(arbitratee),
                    mEpoch(epoch), mErrCode(err) {}

        inline void GetSubPoolKey(SubPoolKey** key)
        {
            *key = &mSubPoolKey;
        }

        inline void GetSourceKey(DiskKey** sourcekey)
        {
            *sourcekey = &mSourceKey;
        }

        inline void GetTargetKey(DiskKey** targetkey)
        {
            *targetkey = &mTargetKey;
        }

        inline void GetArbitrateeKey(ArbitrateeKey** key)
        {
            *key = &mArbitrateeKey;
        }

        inline ywb_uint64_t GetEpoch()
        {
            return mEpoch;
        }

        inline ywb_status_t GetErrCode()
        {
            return mErrCode;
        }

    private:
        /*which subpool the completed migration is against*/
        SubPoolKey mSubPoolKey;
        /*source disk the completed migration is against*/
        DiskKey mSourceKey;
        /*target disk the completed migration is against*/
        DiskKey mTargetKey;
        /*which arbitratee the completed migration is against*/
        ArbitrateeKey mArbitrateeKey;
        /*to which epoch this event belongs*/
        ywb_uint64_t mEpoch;
        /*migration error code*/
        ywb_status_t mErrCode;
    };

    class MigrationExpiredEvent : public Event
    {
    public:
        MigrationExpiredEvent(SubPoolKey& subpool,
                DiskKey& sourcekey, DiskKey& targetkey,
                ArbitrateeKey& arbitratee, ywb_uint64_t epoch) :
                    Event(Event::ET_exe_migration_expired),
                    mSubPoolKey(subpool), mSourceKey(sourcekey),
                    mTargetKey(targetkey), mArbitrateeKey(arbitratee),
                    mEpoch(epoch) {}

        inline void GetSubPoolKey(SubPoolKey** key)
        {
            *key = &mSubPoolKey;
        }

        inline void GetSourceKey(DiskKey** sourcekey)
        {
            *sourcekey = &mSourceKey;
        }

        inline void GetTargetKey(DiskKey** targetkey)
        {
            *targetkey = &mTargetKey;
        }

        inline void GetArbitrateeKey(ArbitrateeKey** key)
        {
            *key = &mArbitrateeKey;
        }

        inline ywb_uint64_t GetEpoch()
        {
            return mEpoch;
        }

    private:
        /*which subpool the completed migration is against*/
        SubPoolKey mSubPoolKey;
        /*source disk the completed migration is against*/
        DiskKey mSourceKey;
        /*target disk the completed migration is against*/
        DiskKey mTargetKey;
        /*which arbitratee the completed migration is against*/
        ArbitrateeKey mArbitrateeKey;
        /*to which epoch this event belongs*/
        ywb_uint64_t mEpoch;
    };

    class StallEvent : public Event
    {
    public:
        StallEvent() : Event(Event::ET_exe_stall) {}
        virtual ~StallEvent() {}
    };

    class StopEvent : public Event
    {
    public:
        StopEvent() : Event(Event::ET_exe_stop) {}
        virtual ~StopEvent() {}
    };
}

namespace HUBEventSet {
    class StopHUBEvent : public Event
    {
    public:
        StopHUBEvent() : Event(Event::ET_hub_stop) {}
        virtual ~StopHUBEvent() {}

    private:

    };

    class EnableASTEvent : public Event
    {
    public:
        EnableASTEvent() :
            Event(Event::ET_hub_enable_ast), mBatchMode(true) {}

        EnableASTEvent(SubPoolKey& subpoolkey) :
            Event(Event::ET_hub_enable_ast), mSubPoolKey(subpoolkey),
            mBatchMode(false) {}

        virtual ~EnableASTEvent() {}

        inline void GetSubPoolKey(SubPoolKey* subpoolkey)
        {
            YWB_ASSERT(subpoolkey != NULL);
            *subpoolkey = mSubPoolKey;
        }

        inline ywb_bool_t InBatchMode()
        {
            return mBatchMode;
        }

    private:
        /*On which SubPool the AST will be enabled*/
        SubPoolKey mSubPoolKey;
        /*
         * whether to Enable AST on all SubPools in batch
         *
         * exclusive to @mSubPoolKey
         *
         * for historical reason, this is used mainly for test
         * */
        ywb_bool_t mBatchMode;
    };

    class DisableASTEvent : public Event
    {
    public:
        DisableASTEvent() :
            Event(Event::ET_hub_disable_ast), mBatchMode(true) {}

        DisableASTEvent(SubPoolKey& subpoolkey) :
            Event(Event::ET_hub_disable_ast), mSubPoolKey(subpoolkey),
            mBatchMode(false) {}

        virtual ~DisableASTEvent() {}

        inline void GetSubPoolKey(SubPoolKey* subpoolkey)
        {
            YWB_ASSERT(subpoolkey != NULL);
            *subpoolkey = mSubPoolKey;
        }

        inline ywb_bool_t InBatchMode()
        {
            return mBatchMode;
        }

    private:
        /*On which SubPool the AST will be disabled*/
        SubPoolKey mSubPoolKey;
        /*
         * whether to Disable AST on all SubPools in batch
         *
         * exclusive to @mSubPoolKey
         *
         * for historical reason, this is used mainly for test
         * */
        ywb_bool_t mBatchMode;
    };

    class EnableASTEchoEvent : public Event
    {
    public:
        EnableASTEchoEvent() :
            Event(Event::ET_hub_enable_ast_echo), mStatus(YWB_SUCCESS) {}

        virtual ~EnableASTEchoEvent() {}

        inline ywb_status_t GetStatus()
        {
            return mStatus;
        }

        inline void SetStatus(ywb_status_t status)
        {
            mStatus = status;
        }

    private:
        ywb_status_t mStatus;
    };

    class DisableASTEchoEvent : public Event
    {
    public:
        DisableASTEchoEvent() :
            Event(Event::ET_hub_disable_ast_echo), mStatus(YWB_SUCCESS) {}

        virtual ~DisableASTEchoEvent() {}

        inline ywb_status_t GetStatus()
        {
            return mStatus;
        }

        inline void SetStatus(ywb_status_t status)
        {
            mStatus = status;
        }

    private:
        ywb_status_t mStatus;
    };

    class AddDiskEvent : public Event
    {
    public:
        AddDiskEvent(DiskKey diskkey, DiskBaseProp diskprop) :
            Event(Event::ET_hub_add_disk)
        {
            mDiskKey = diskkey;
            mDiskProp = diskprop;
        }

        virtual ~AddDiskEvent() {}

        inline void GetDiskKey(DiskKey** diskkey)
        {
            *diskkey = &mDiskKey;
        }

        inline void GetDiskProp(DiskBaseProp** diskprop)
        {
            *diskprop = &mDiskProp;
        }

    private:
        DiskKey mDiskKey;
        DiskBaseProp mDiskProp;
    };

    class RemoveDiskEvent : public Event
    {
    public:
        RemoveDiskEvent(DiskKey diskkey) :
            Event(Event::ET_hub_remove_disk)
        {
            mDiskKey = diskkey;
        }

        virtual ~RemoveDiskEvent() {}

        inline void GetDiskKey(DiskKey** diskkey)
        {
            *diskkey = &mDiskKey;
        }

    private:
        DiskKey mDiskKey;
    };

    class AddOBJEvent : public Event
    {
    public:
        AddOBJEvent(OBJKey objkey) :
            Event(Event::ET_hub_add_obj)
        {
            mOBJKey = objkey;
        }

        virtual ~AddOBJEvent() {}

        inline void GetOBJKey(OBJKey** objkey)
        {
            *objkey = &mOBJKey;
        }

    private:
        OBJKey mOBJKey;
    };

    class DeleteOBJEvent : public Event
    {
    public:
        DeleteOBJEvent(OBJKey objkey) :
            Event(Event::ET_hub_delete_obj)
        {
            mOBJKey = objkey;
        }

        virtual ~DeleteOBJEvent() {}

        inline void GetOBJKey(OBJKey** objkey)
        {
            *objkey = &mOBJKey;
        }

    private:
        OBJKey mOBJKey;
    };

    class AccessOBJEvent : public Event
    {
    public:
        AccessOBJEvent(OBJKey objkey, off_t offset, size_t len,
                ywb_uint32_t ios, ywb_uint32_t bw,
                ywb_uint32_t rt, ywb_bool_t read) :
            Event(Event::ET_hub_access_obj)
        {
            mOBJKey = objkey;
            mOffset = offset;
            mLen = len;
            mIOs = ios;
            mBW = bw;
            mRT = rt;
            mRead = read;
        }

        virtual ~AccessOBJEvent() {}

        inline void GetOBJKey(OBJKey** objkey)
        {
            *objkey = &mOBJKey;
        }

        inline off_t GetOffset()
        {
            return mOffset;
        }

        inline size_t GetLen()
        {
            return mLen;
        }

        inline ywb_uint32_t GetIOs()
        {
            return mIOs;
        }

        inline ywb_uint32_t GetBW()
        {
            return mBW;
        }

        inline ywb_uint32_t GetRT()
        {
            return mRT;
        }

        inline ywb_bool_t GetReadOrWrite()
        {
            return mRead;
        }

    private:
        OBJKey mOBJKey;
        off_t mOffset;
        size_t mLen;
        /*IO times involved*/
        ywb_uint32_t mIOs;
        /*
         * bandwidth involved
         * FIXME: duplicate to @mLen???
         * */
        ywb_uint32_t mBW;
        /*response time involved*/
        ywb_uint32_t mRT;
        ywb_bool_t mRead;
    };

    class CollectConfigViewEvent : public Event
    {
    public:
        CollectConfigViewEvent(ywb_uint32_t seq) :
            Event(Event::ET_hub_collect_config_view), mSeqno(seq) {}

        virtual ~CollectConfigViewEvent() {}

        inline ywb_uint32_t GetSeqNo()
        {
            return mSeqno;
        }

        inline void SetSeqNo(ywb_uint32_t seq)
        {
            mSeqno = seq;
        }

    private:
        ywb_uint32_t mSeqno;
    };

    class CollectIOEvent : public Event
    {
    public:
        enum {
            F_prep = 0,
            F_normal,
            F_post,
            F_max,
        };

        CollectIOEvent(ywb_uint32_t seq, ywb_uint32_t flag) :
            Event(Event::ET_hub_collect_io), mSeqno(seq), mFlag(flag) {}

        virtual ~CollectIOEvent() {}

        inline ywb_uint32_t GetSeqNo()
        {
            return mSeqno;
        }

        inline ywb_uint32_t GetFlag()
        {
            return mFlag;
        }

        void SetSeqNo(ywb_uint32_t seq)
        {
            mSeqno = seq;
        }

        void SetFlag(ywb_uint32_t flag)
        {
            mFlag = flag;
        }

    private:
        ywb_uint32_t mSeqno;
        ywb_uint32_t mFlag;
    };

    class MigrateEvent : public Event
    {
    public:
        MigrateEvent(OBJKey objkey, DiskKey tgtdisk) :
            Event(Event::ET_hub_migrate)
        {
            mOBJKey = objkey;
            mTgtDisk = tgtdisk;
        }

        virtual ~MigrateEvent() {}

        inline void GetOBJKey(OBJKey** objkey)
        {
            *objkey = &mOBJKey;
        }

        inline void GetTargetDisk(DiskKey** diskkey)
        {
            *diskkey = &mTgtDisk;
        }

    private:
        OBJKey mOBJKey;
        DiskKey mTgtDisk;
    };

    class MigrationCompletedEvent : public Event
    {
    public:
        MigrationCompletedEvent(OBJKey objkey, ywb_status_t status) :
            Event(Event::ET_hub_migration_completed),
            mOBJKey(objkey), mMigrationStatus(status)
        {

        }

        virtual ~MigrationCompletedEvent() {}

        inline void GetOBJKey(OBJKey** objkey)
        {
            *objkey = &mOBJKey;
        }

        inline ywb_status_t GetMigrationStatus()
        {
            return mMigrationStatus;
        }

    private:
        OBJKey mOBJKey;
        ywb_status_t mMigrationStatus;
    };

#ifdef WITH_INITIAL_PLACEMENT
    class UpdateSubPoolRelocationTypeEvent : public Event
    {
    public:
        UpdateSubPoolRelocationTypeEvent(SubPoolKey subpoolkey, ywb_int32_t val) :
                Event(Event::ET_hub_update_relocation_type),
                mSubPoolKey(subpoolkey), mRelocationType(val)
        {
        }

        virtual ~UpdateSubPoolRelocationTypeEvent() {}

        inline void GetSubPoolKey(SubPoolKey** subpoolkey)
        {
            *subpoolkey = &mSubPoolKey;
        }

        inline ywb_int32_t GetRelocationType()
        {
            return mRelocationType;
        }

    private:
        SubPoolKey mSubPoolKey;
        ywb_int32_t mRelocationType;
    };

    class RequireSwapDiskEvent : public Event
    {
    public:
        RequireSwapDiskEvent(DiskKey diskkey, ywb_int32_t level,
                ywb_int32_t direction) : Event(Event::ET_hub_require_swap_disk),
                mDiskKey(diskkey), mSwapLevel(level), mSwapDirection(direction)
        {
        }

        virtual ~RequireSwapDiskEvent() {}

        inline void GetDiskKey(DiskKey** diskkey)
        {
            *diskkey = &mDiskKey;
        }

        inline ywb_int32_t GetSwapLevel()
        {
            return mSwapLevel;
        }

        inline ywb_int32_t GetSwapDirection()
        {
            return mSwapDirection;
        }

    private:
        DiskKey mDiskKey;
        /*high/medium/low swap*/
        ywb_int32_t mSwapLevel;
        /*swap up or down*/
        ywb_int32_t mSwapDirection;
    };

    class CancelSwapDiskEvent : public Event
    {
    public:
        CancelSwapDiskEvent(DiskKey diskkey) :
            Event(Event::ET_hub_cancel_swap_disk), mDiskKey(diskkey)
        {
        }

        virtual ~CancelSwapDiskEvent() {}

        inline void GetDiskKey(DiskKey** diskkey)
        {
            *diskkey = &mDiskKey;
        }

    private:
        DiskKey mDiskKey;
    };
#endif
}

//#ifdef AST_INTEGRATION_WITH_OSS_SIMULATOR
namespace OSSSimulatorEventSet{
    class MigrateEvent : public Event
    {
    public:
        MigrateEvent(OBJKey& objkey) : Event(Event::ET_osssimulator_migrate),
            mOBJKey(objkey)
        {

        }

        virtual ~MigrateEvent() {}

        void GetOBJKey(OBJKey& objkey)
        {
            objkey = mOBJKey;
        }

    private:
        OBJKey mOBJKey;
    };

    class DynamicalConfigEvent : public Event
    {
    public:
        DynamicalConfigEvent() : Event(Event::ET_osssimulator_dynamical_config) {}
        virtual ~DynamicalConfigEvent() {}

    private:

    };

    class MigrationDoneSimulateEvent : public Event
    {
    public:
        MigrationDoneSimulateEvent() : Event(Event::ET_migration_done_simulate) {}
        virtual ~MigrationDoneSimulateEvent() {}

    private:

    };

    class StopEvent : public Event
    {
    public:
        StopEvent() : Event(Event::ET_osssimulator_stop) {}
        virtual ~StopEvent() {}

    private:

    };
}
//#endif

#endif

/* vim:set ts=4 sw=4 expandtab */
