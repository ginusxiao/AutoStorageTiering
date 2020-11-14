#ifndef __AST_RELOCATION_HPP__
#define __AST_RELOCATION_HPP__

#include "common/FsStatus.h"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTArb.hpp"
#include "AST/ASTEvent.hpp"

class AST;
class LogicalConfig;
class IOManager;
class SwapOutManager;
class HeatDistributionManager;
class AdviceManager;
class PerfManager;
class PlanManager;
class Arbitrator;
class Migration;

class RelocationTaskType{
public:
    enum {
        /*learning*/
        RTT_learning = 0,
        /*CTR advice*/
        RTT_ctr,
        /*ITR advice*/
        RTT_itr,
        /**/
        /*plan*/
        RTT_plan,
        /*arb*/
        RTT_arb,
        /*migration*/
        RTT_mig,

        RTT_cnt,
    };
};

class RelocationType {
public:
    enum {
        /*relocation types can be combined together in the future but not now*/
        RT_auto_tiering = 0x0001,
        RT_swap_out = 0x0002,
        RT_no_data_movement = 0x0004,
    };
};

/*base class for relocation work*/
class RelocationTask{
public:
    /*
     * put definition of constructor and de-constructor in .cpp
     * to avoid cross reference issue
     **/
    RelocationTask(AST* ast);
    virtual ~RelocationTask();

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    /*prepare for the cyclic work*/
    virtual ywb_status_t Prepare();
//    /*
//     * do cyclic work regarding one relocation task type
//     * of given phase, here "phase" is task type specific.
//     * each task type can have its self-defined phases
//     * according to its requirement. Of course one task
//     * type without self-defined phase is OK, then it
//     * should use DoCycleWork instead.
//     *
//     * for example, RelocationLearning(RelocationTaskType
//     * ::RTT_none) have two phase:
//     * phase0 - collect logical config
//     * phase1 - resolve logical config
//     * phase2 - collect OBJ IO
//     * phase3 - resolve OBJ IO
//     * phase4 - resolve deleted OBJ if have
//     * phase5 - resolve deleted disk if have
//     * phase6 - resolve deleted subpools if have
//     **/
//    virtual ywb_status_t DoCycleWork(ywb_uint32_t phase);
    /*override version, without specify phase*/
    virtual ywb_status_t DoCycleWork();

private:
    AST* mAst;
};

/*learning related relocation work*/
class RelocationLearning : public RelocationTask{
public:
    /*
     * put definition of constructor and de-constructor in .cpp
     * to avoid cross reference issue
     **/
    RelocationLearning(AST* ast);
    virtual ~RelocationLearning();

    inline void SetLearningEvent(GeneratorEventSet::LearningEvent* event)
    {
        mLearningEvent = event;
    }

    /*reset the state to newly constructed state*/
    ywb_status_t Reset();
//    /*
//     * Cleanup the logical configuration and OBJ IO by
//     *
//     * current implementation must retain the OBJ IO info
//     * between cycle switch, BUT OBJ IO's existence relies
//     * on logical configuration's existence, so must retain
//     * the logical configuration as well.
//     **/
//    ywb_status_t Cleanup();
    /*
     * Before collecting new logical configuration for new cycle
     * all logical configuration of last cycle should be taken
     * as suspicious, which will be validated by newly collected
     * logical configuration.
     **/
    ywb_status_t MarkLogicalConfigAsSuspicious();
    /*
     * Newly collected logical configuration will confirm those
     * subpools/disks/OBJs marked as suspicious, for those have
     * not been confirmed, feel free to remove them.
     **/
    ywb_status_t RemoveUnConfirmedSuspect();
    /*
     * Consolidate logical configuration by deleting those OBJs/
     * Disks/SubPools in to-be-deleted set.
     **/
    ywb_status_t Consolidate();

    ywb_status_t Prepare();
//    ywb_status_t DoCycleWork(ywb_uint32_t phase);
    ywb_status_t DoCycleWork();

private:
    LogicalConfig* mConfigManager;
    IOManager* mIOManager;
#ifdef WITH_INITIAL_PLACEMENT
    SwapOutManager* mSwapOutManager;
#endif
    GeneratorEventSet::LearningEvent* mLearningEvent;
    ywb_bool_t mAdviceAfterLearning;
};

/*cross-tier replacement related relocation work*/
class RelocationCTR : public RelocationTask{
public:
    /*
     * put definition of constructor and de-constructor in .cpp
     * to avoid cross reference issue
     **/
    RelocationCTR(AST* ast);
    virtual ~RelocationCTR();

    ywb_status_t Reset();
    ywb_status_t Init();

    ywb_status_t Prepare();
    ywb_status_t DoCycleWork();

private:
    AdviceManager* mAdviceManager;
    HeatDistributionManager* mHeatDistributionManager;
};

/*intra-tier replacement related relocation work*/
class RelocationITR : public RelocationTask{
public:
    /*
     * put definition of constructor and de-constructor in .cpp
     * to avoid cross reference issue
     **/
    RelocationITR(AST* ast);
    virtual ~RelocationITR();

    /*reset the state to newly constructed state*/
    ywb_status_t Reset();
    ywb_status_t Init();

    ywb_status_t Prepare();
    ywb_status_t DoCycleWork();

private:
    AdviceManager* mAdviceManager;
    HeatDistributionManager* mHeatDistributionManager;
};

/*plan related relocation work*/
class RelocationPlan : public RelocationTask{
public:
    /*
     * put definition of constructor and de-constructor in .cpp
     * to avoid cross reference issue
     **/
    RelocationPlan(AST* ast);
    virtual ~RelocationPlan();

    /*reset the state to newly constructed state*/
    ywb_status_t Reset();
    /*do necessary initialization*/
    ywb_status_t Init();
    /*called after one plan migrated successfully*/
    ywb_status_t HandleCompleted(SubPoolKey& subpoolkey,
            DiskKey& sourcekey, DiskKey& targetkey,
            ywb_bool_t triggermore, ywb_status_t migstatus);
    /*called when one arbitratee expires*/
    ywb_status_t HandleExpired(SubPoolKey& subpoolkey,
            DiskKey& sourcekey, DiskKey& targetkey,
            ywb_bool_t triggermore);

    ywb_status_t Prepare();
    ywb_status_t DoCycleWork();

private:
    PerfManager* mDiskPerfManager;
    PlanManager* mPlanManager;
};

/*arbitrator related relocation work*/
class RelocationARB : public RelocationTask{
public:
    /*
     * put definition of constructor and de-constructor in .cpp
     * to avoid cross reference issue
     **/
    RelocationARB(AST* ast);
    virtual ~RelocationARB();

    /*reset the state to newly constructed state*/
    ywb_status_t Reset();

    /*
     * called after one arbitratee migrated successfully
     * @curwindow: whether the completed migration is of current window
     * */
    ywb_status_t HandleCompleted(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, ywb_bool_t curwindow,
            ywb_status_t migstatus);
    /*
     * called when one arbitratee expired
     * @curwindow: whether the completed migration is of current window
     * */
    ywb_status_t HandleExpired(SubPoolKey& subpoolkey,
            ArbitrateeKey& arbitrateekey, ywb_bool_t curwindow);

    ywb_status_t Prepare();
    ywb_status_t DoCycleWork();

private:
    Arbitrator* mArb;
};

/*migration related relocation work*/
class RelocationMIG: public RelocationTask{
public:
    /*
     * put definition of constructor and de-constructor in .cpp
     * to avoid cross reference issue
     **/
    RelocationMIG(AST* ast);
    virtual ~RelocationMIG();

    /*reset the state to newly constructed state*/
    ywb_status_t Reset();
    /*init the state*/
    ywb_status_t Init();
    /*called after one migration plan migrated successfully*/
    ywb_status_t HandleCompleted(DiskKey& diskkey,
            ArbitrateeKey& arbitrateekey, ywb_status_t migerr,
            ywb_bool_t triggermore);
    /*called when one migration plan expired*/
    ywb_status_t HandleExpired(DiskKey& diskkey,
            ArbitrateeKey& arbitrateekey,
            ywb_bool_t triggermore);

    /*remove one item from in flight*/
    ywb_status_t RemoveInflight(ArbitrateeKey& arbitrateekey);
    /*get the number of items in flight*/
    ywb_uint32_t GetInflightEmpty();

    ywb_status_t Prepare();
    ywb_status_t DoCycleWork();

private:
    Migration* mMigrationManager;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
