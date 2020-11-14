#ifndef __AST_ADVICE_HPP__
#define __AST_ADVICE_HPP__

#include <list>
#include <string>
#include <sstream>
#include "common/FsStatus.h"
#include "common/FsInts.h"
#include "common/FsAtomic.h"
#include "FsCommon/YfsMutex.hpp"
#include "AST/ASTConstant.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTIO.hpp"
#include "AST/ASTAdviceRule.hpp"
#include "AST/ASTHeatDistribution.hpp"

using namespace std;

class Advice {
public:
    enum {
        /*advice already been chosen*/
        ADV_chosen = 1,
    };

    Advice(OBJKey& key, ywb_uint32_t srctier,
            ywb_uint32_t tgttier, ChunkIOStat& stat,
            ywb_uint32_t heattype, ywb_uint32_t cap) :
                mKey(key), mSrcTier(srctier), mTgtTier(tgttier),
                mCap(cap), mHeatType(heattype), mStat(stat), mRef(0)
    {
        mFlag.Reset();
    }

    inline void GetOBJKey(OBJKey* key)
    {
        *key = mKey;
    }

    inline ywb_uint32_t GetSourceTier()
    {
        return mSrcTier;
    }

    inline ywb_uint32_t GetTargetTier()
    {
        return mTgtTier;
    }

    inline ywb_uint32_t GetHeatType()
    {
        return mHeatType;
    }

    inline void GetStat(ChunkIOStat** stat)
    {
        *stat = &mStat;
    }

    inline ywb_uint32_t GetCap()
    {
        return mCap;
    }

    inline void SetOBJKey(OBJKey*& key)
    {
        mKey = *key;
    }

    inline void SetSourceTier(ywb_uint32_t tier)
    {
        mSrcTier = tier;
    }

    inline void SetTargetTier(ywb_uint32_t tier)
    {
        mTgtTier = tier;
    }

    inline void SetHeatType(ywb_uint32_t heattype)
    {
        mHeatType = heattype;
    }

    inline void SetStat(ChunkIOStat*& stat)
    {
        mStat = *stat;
    }

    inline void SetCap(ywb_uint32_t cap)
    {
        mCap = cap;
    }

    inline void IncRef()
    {
        fs_atomic_inc(&mRef);
    }

    inline void DecRef()
    {
        if(fs_atomic_dec_and_test(&mRef))
        {
            delete this;
        }
    }

    inline void SetFlagComb(ywb_uint32_t flag)
    {
        mFlag.SetFlagComb(flag);
    }

    inline void SetFlagUniq(ywb_uint32_t flag)
    {
        mFlag.SetFlagUniq(flag);
    }

    inline void ClearFlag(ywb_uint32_t flag)
    {
        mFlag.ClearFlag(flag);
    }

    inline bool TestFlag(ywb_uint32_t flag)
    {
        return mFlag.TestFlag(flag);
    }

private:
    /*OBJ key*/
    OBJKey mKey;
    /*source tier*/
    ywb_uint32_t mSrcTier;
    /*target tier*/
    ywb_uint32_t mTgtTier;
    /*capacity requirement, currently use the number of OBJ instead*/
    ywb_uint32_t mCap;
    /*of which IOStat::Type the @mStat is*/
    ywb_uint32_t mHeatType;
    /*
     * OBJ raw/EMA IO of decision cycle, migration plan will
     * be generated in following cycles during which the OBJ
     * new RAW/EMA IO(say IO_new) may change, if the IO_new
     * is far away from the one in decision cycle, the OBJ
     * will not be migrated
     **/
    ChunkIOStat mStat;
    /*reference count*/
    fs_atomic_t mRef;
    Flag mFlag;
};

/*advise of one type are managed here*/
class TierBasedAdviceSet {
public:
    TierBasedAdviceSet() : mLastChosenTier(Constant::TIER_NUM), mAdviseLock("AdviceLock")
    {
        ywb_uint32_t tier = 0;

        for(; tier < Constant::TIER_NUM; tier++)
        {
            mAdviseChosen[tier] = 0;
        }
    }

    ~TierBasedAdviceSet()
    {
        Destroy();
    }

    inline void PutAdvice(Advice* advice)
    {
        advice->DecRef();
    }

    /*add advice into advice set*/
    ywb_status_t AddAdvice(ywb_uint32_t tier, Advice* advice);

    /*
     * NOTE: following 3 pairs of function method are not thread-safe
     * See comments in Class Tier(ASTLogicalConfig.hpp).
     **/
    /*get first advice from specified tier*/
    ywb_status_t GetFirstAdvice(ywb_uint32_t tier, Advice** advice);
    /*get next advice from specified tier*/
    ywb_status_t GetNextAdvice(ywb_uint32_t tier, Advice** advice);

    /*choose the first tier with tier balance considered*/
    ywb_uint32_t GetFirstTier();
    /*choose next tier with tier balance considered*/
    ywb_uint32_t GetNextTier();

    /*
     * get first advice from whole advise set, used by combining
     * with GetFirstTier
     **/
    ywb_status_t GetFirstAdvice(Advice** advice);
    /*
     * get next advice from whole advise set, used by combining
     * with GetNextTier
     **/
    ywb_status_t GetNextAdvice(Advice** advice);

    /*
     * get first advice satisfy given condition
     * @flag: to be tested flag
     * @set: true or false
     *              set to true if the given flag should already been set
     *              set to false if the given flag should already been cleared
     * @advice: the returned advice if any
     **/
    ywb_status_t GetFirstAdviceSatisfy(ywb_uint32_t flag,
            ywb_bool_t set, Advice** advice);
    /*
     * get next advice satisfy given condition
     * @flag: to be tested flag
     * @set: true or false
     *              set to true if the given flag should already been set
     *              set to false if the given flag should already been cleared
     * @advice: the returned advice if any
     **/
    ywb_status_t GetNextAdviceSatisfy(ywb_uint32_t flag,
            ywb_bool_t setorclear, Advice** advice);

    /*is it the first time to get from this AdviceSet*/
    ywb_bool_t IsFirstTimeToGetFrom();

    /*destroy advice set*/
    ywb_status_t Destroy();

private:
    list<Advice*> mAdvise[Constant::TIER_NUM];
    /*used for traversing advise of specified tier*/
    list<Advice*>::iterator mIt[Constant::TIER_NUM];
    /*
     * used for traversing advise among tiers with
     * balance considered
     **/
    list<Advice*>::iterator mIt2[Constant::TIER_NUM];
    /*from which tier last advice is chosen*/
    ywb_uint32_t mLastChosenTier;
    /*how many advise are chosen from one tier*/
    ywb_uint32_t mAdviseChosen[Constant::TIER_NUM];
    /*protecting all fields above*/
    Mutex mAdviseLock;
};

class DiskAdvise {
public:
    DiskAdvise() : mLock("DiskAdviseLock")
    {

    }

    virtual ~DiskAdvise()
    {
        Destroy();
    }

    inline void PutAdvice(Advice* advice)
    {
        advice->DecRef();
    }

    ywb_status_t AddAdvice(Advice* advice);
    ywb_status_t GetFirstAdviceLocked(Advice** advice);
    ywb_status_t GetNextAdviceLocked(Advice** advice);
    ywb_status_t GetFirstAdvice(Advice** advice);
    ywb_status_t GetNextAdvice(Advice** advice);
    /*get first advice satisfying given condition*/
    ywb_status_t GetFirstAdviceSatisfy(ywb_uint32_t flag,
            ywb_bool_t set, Advice** advice);
    /*get next advice satisfying given condition*/
    ywb_status_t GetNextAdviceSatisfy(ywb_uint32_t flag,
            ywb_bool_t set, Advice** advice);

    void Destroy();

private:
    list<Advice*> mAdvise;
    list<Advice*>::iterator mIt;
    Mutex mLock;
};

class DiskBasedAdviceSet {
public:
    DiskBasedAdviceSet()
    {

    }

    virtual ~DiskBasedAdviceSet()
    {
        Destroy();
    }

    inline void PutAdvice(Advice* advice)
    {
        advice->DecRef();
    }

    ywb_status_t AddAdvice(DiskKey& diskkey, Advice* advice);
    ywb_status_t GetFirstAdvice(DiskKey& diskkey, Advice** advice);
    ywb_status_t GetNextAdvice(DiskKey& diskkey, Advice** advice);
    /*
     * get first advice satisfying given condition from specified disk
     * @flag: to be tested flag
     * @set: true or false
     *              set to true if the given flag should already been set
     *              set to false if the given flag should already been cleared
     * @advice: the returned advice if any
     **/
    ywb_status_t GetFirstAdviceSatisfy(DiskKey& diskkey,
            ywb_uint32_t flag, ywb_bool_t set, Advice** advice);
    /*
     * get next advice satisfying given condition from specified disk
     * @flag: to be tested flag
     * @set: true or false
     *              set to true if the given flag should already been set
     *              set to false if the given flag should already been cleared
     * @advice: the returned advice if any
     **/
    ywb_status_t GetNextAdviceSatisfy(DiskKey& diskkey,
            ywb_uint32_t flag, ywb_bool_t set, Advice** advice);

    void Destroy();

private:
    std::map<DiskKey, DiskAdvise*, DiskKeyCmp> mAdvise;
};

/*sub-pool level advice*/
class SubPoolAdvice {
public:
    SubPoolAdvice(ywb_uint32_t tiercomb) :
        mTierComb(tiercomb), mRef(0)
    {
        ywb_uint32_t type = 0;

        for(; type < Constant::ADVICE_TYPE_CNT; type++)
        {
            mAdviceSet1[type] = NULL;
            mAdviceSet2[type] = NULL;
        }
    }

    ~SubPoolAdvice()
    {
        Destroy();
    }

    inline void IncRef()
    {
        fs_atomic_inc(&mRef);
    }

    inline void DecRef()
    {
        if(fs_atomic_dec_and_test(&mRef))
        {
            delete this;
        }
    }

    inline void PutAdvice(Advice* advice)
    {
        advice->DecRef();
    }

    inline ywb_uint32_t GetTierComb()
    {
        return mTierComb;
    }

    inline void SetTierComb(ywb_uint32_t val)
    {
        mTierComb = val;
    }

    ywb_uint32_t GetAdviceTargetTier(
            ywb_uint32_t srctier, AdviceRule& rule);

    /*add advice into tier based advice set of given type*/
    ywb_status_t AddAdvice(ywb_uint32_t tier, ywb_uint32_t type, Advice* advice);
    /*add advice into disk based advice set of given type*/
    ywb_status_t AddAdvice(DiskKey& diskkey, ywb_uint32_t type, Advice* advice);

    /*
     * NOTE: following 6 pairs of function method are not thread-safe
     * for the underlying function method(AdviseSet's GetFirst***
     * and GetNext*** are not thread-safe)
     *
     * But currently, these 6 pairs of function method are only called
     * by PlanManager::Schedule***, that is only one thread(currently
     * only one thread for PlanManager) accessing it, so no "shared-
     * iterator problem", but the "deletion problem" still exists.
     *
     * The "deletion problem" here is, advise are generated and destroyed
     * (when about to generate new advise for coming new cycle)by Generator
     * thread, and accessed by Executor thread, the Executor thread may
     * access it while the Generator thread destroy advise. So, deletion
     * will be processed with caution, to be specific, advise destroy will
     * only be executed after the PlanManager stop its work, the work flow
     * is:
     * PlanManager stops ---> AdviceManager destroyes all advise --->
     * AdviceManager generates new advise ---> PlanManager restores
     **/
    /*get first/next tier-based advice of certain type from given tier*/
    ywb_status_t GetFirstAdvice(ywb_uint32_t tier,
            ywb_uint32_t type, Advice** advice);
    ywb_status_t GetNextAdvice(ywb_uint32_t tier,
            ywb_uint32_t type, Advice** advice);

    /*get first/next disk-based advice of certain type from given disk*/
    ywb_status_t GetFirstAdvice(DiskKey& diskkey,
            ywb_uint32_t type, Advice** advice);
    ywb_status_t GetNextAdvice(DiskKey& diskkey,
            ywb_uint32_t type, Advice** advice);

    /*get first/next tier-based advice of certain type*/
    ywb_status_t GetFirstAdvice(ywb_uint32_t type, Advice** advice);
    ywb_status_t GetNextAdvice(ywb_uint32_t type, Advice** advice);

    /*get first/next tier-based advice of certain type satisfying specified condition*/
    ywb_status_t GetFirstAdviceSatisfy(ywb_uint32_t type,
            ywb_uint32_t flag, ywb_bool_t set, Advice** advice);
    ywb_status_t GetNextAdviceSatisfy(ywb_uint32_t type,
            ywb_uint32_t flag, ywb_bool_t set, Advice** advice);
    /*
     * get first/next disk-based advice of certain type satisfying
     * specified condition from given disk
     * */
    ywb_status_t GetFirstAdviceSatisfy(DiskKey& diskkey, ywb_uint32_t type,
            ywb_uint32_t flag, ywb_bool_t set, Advice** advice);
    ywb_status_t GetNextAdviceSatisfy(DiskKey& diskkey, ywb_uint32_t type,
            ywb_uint32_t flag, ywb_bool_t set, Advice** advice);

    /*(Deprecated)is it the first time to get from AdviceSet of given advice type*/
    ywb_bool_t IsFirstTimeToGetFromTierBasedAdvice(ywb_uint32_t type);

    /*destroy tier-based advice set of given type*/
    ywb_status_t DestroyTierBasedAdviceSet(ywb_uint32_t type);
    /*destroy disk-based advice set of given type*/
    ywb_status_t DestroyDiskBasedAdviceSet(ywb_uint32_t type);
    /*destroy all managed advice set*/
    ywb_status_t Destroy();
    /*destroy advice set of given type*/
    ywb_status_t Destroy(ywb_int32_t advicetype);

private:
    TierBasedAdviceSet* mAdviceSet1[Constant::ADVICE_TYPE_CNT];
    DiskBasedAdviceSet* mAdviceSet2[Constant::ADVICE_TYPE_CNT];
    ywb_uint32_t mTierComb;
    /*reference count*/
    fs_atomic_t mRef;
};

class AdviceManager{
public:
    AdviceManager(AST* ast) : mAdviseLock("Advice manager"), mAst(ast)
    {

    }

    virtual ~AdviceManager()
    {
        Destroy();
    }

    class Filter{
    public:
        Filter();
        virtual ~Filter();

        virtual bool DoMyWork();
        bool DoFilter(){
            bool pass = false;

            pass = DoMyWork();

            if(false == pass)
                return false;
            else if(mNextFilter)
                pass = mNextFilter->DoFilter();
            else
                return true;
        }

    private:
        Filter* mNextFilter;
    };

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    inline void PutAdvice(Advice* advice)
    {
        advice->DecRef();
    }

    inline void PutSubPoolAdvice(SubPoolAdvice* subpooladvice)
    {
        subpooladvice->DecRef();
    }

    inline void Lock()
    {
        mAdviseLock.Lock();
    }

    inline void Unlock()
    {
        mAdviseLock.Unlock();
    }

    ywb_status_t GetSubPoolAdvice(SubPoolKey&, SubPoolAdvice** subpooladvise);
    ywb_status_t AddSubPoolAdvice(SubPoolKey&, SubPoolAdvice*);
    ywb_status_t RemoveSubPoolAdvice(SubPoolKey&);

    /*
     * calculate theoretical number of advice on one tier, may take
     * following factors into consideration:
     * target tier related to advice type do exist;
     * target tier/disk performance limitation;
     * source tier/disk performance limitation;
     * benefit cost ratio;
     * and so on...
     **/
    ywb_status_t CalculateTheoreticalAdviceNum(SubPoolKey& key,
            ywb_uint32_t tier, ywb_uint32_t advicetype,
            ywb_uint32_t* adviceabove, ywb_uint32_t* advicebelow);
    ywb_status_t CalculateTheoreticalAdviceNum(SubPoolKey& key,
            ywb_uint32_t tier, DiskKey& disk, ywb_uint32_t advicetype,
            ywb_uint32_t* adviceabove, ywb_uint32_t* advicebelow);
    /*
     * calculate theoretical number of advice on one disk
     **/
    ywb_status_t CalculateTheoreticalAdviceNum(SubPoolKey& key,
            DiskKey& disk, ywb_uint32_t advicetype,
            ywb_uint32_t* adviceabove, ywb_uint32_t* advicebelow);

    /*execute advice rule on specified tier*/
    ywb_status_t AdoptAdviceRule(SubPoolKey& subpool,
            ywb_uint32_t tier, AdviceRule& rule);
    /*execute advice rule on specified tier and disk*/
    ywb_status_t AdoptAdviceRule(SubPoolKey& subpool,
            ywb_uint32_t tier, DiskKey& disk, AdviceRule& rule);
    /*execute advice rule on specified disk*/
    ywb_status_t AdoptAdviceRule(SubPoolKey& subpool,
            DiskKey& disk, AdviceRule& rule);

    /*execute filter rule on specified tier*/
    ywb_status_t AdoptFilterRule(SubPoolKey& subpool, ywb_uint32_t tier,
            AdviceRule* filterrule, ywb_uint32_t filternum);
    /*execute filter rule on specified tier and disk*/
    ywb_status_t AdoptFilterRule(SubPoolKey& subpool, ywb_uint32_t tier,
            DiskKey& disk, AdviceRule* filterrule, ywb_uint32_t filternum);
    /*execute filter rule on specified disk*/
    ywb_status_t AdoptFilterRule(SubPoolKey& subpool, DiskKey& disk,
            AdviceRule* filterrule, ywb_uint32_t filternum);

    /*get the intersection on specified tier*/
    ywb_status_t IntersectAdviceAndFilterRule(SubPoolKey& subpoolkey,
            ywb_uint32_t tiertype, AdviceRule& rule,
            AdviceRule* filterrule, ywb_uint32_t filternum);
    /*get the intersection on specified disk in certain tier*/
    ywb_status_t IntersectAdviceAndFilterRule(SubPoolKey& subpoolkey,
            ywb_uint32_t tiertype, DiskKey& diskkey, AdviceRule& rule,
            AdviceRule* filterrule, ywb_uint32_t filternum);
    /*get the intersection on specified disk*/
    ywb_status_t IntersectAdviceAndFilterRule(SubPoolKey& subpoolkey,
            DiskKey& diskkey, AdviceRule& rule,
            AdviceRule* filterrule, ywb_uint32_t filternum);

    /*check before generate advise for given subpool*/
    ywb_status_t PreGenerateCheck(SubPool* subpool);
    /*
     * generate advice for specified advice type
     * @key: subpool key
     * @rule: partition and sorting rule
     * @filterrule: filter rules
     * @filternum: number of filter rules
     **/
    virtual ywb_status_t GenerateAdvice(SubPoolKey& subpol, AdviceRule& rule,
            AdviceRule* filterrule, ywb_uint32_t filternum);

    /*FIXME: override, can specify tier from which advise are generated*/
    virtual ywb_status_t GenerateAdvice(SubPoolKey& subpol, ywb_uint32_t tier,
            AdviceRule& rule, AdviceRule* filterrule, ywb_uint32_t filternum);

    /*FIXME: override, can specify tier and disk from which advise are generated*/
    virtual ywb_status_t GenerateAdvice(
            SubPoolKey& subpol, ywb_uint32_t tier, DiskKey& disk,
            AdviceRule& rule, AdviceRule* filterrule, ywb_uint32_t filternum);

    /*FIXME: override, can specify disk from which advise are generated*/
    virtual ywb_status_t GenerateAdvice(SubPoolKey& subpol, DiskKey& disk,
            AdviceRule& rule, AdviceRule* filterrule, ywb_uint32_t filternum);

    /*
     * FIXME: override
     * using this function only if the other two are not suitable
     * @key: sub-pool key
     * @rule: mainly specify partition rule, sorting rule will be ignored
     * @less: compare function used for sorting
     * @filterchain: filter rules are defined here, filterChain must be of type
     *               Filter or sub-class of Filter, eg:
     *               class Filter1:Filter{
     *               public:
     *                   bool DoMyWork(){
     *                       do something;
     *                   }
     *
     *               private:
     *                   void* mNextFilter;
     *               }
     **/
    virtual ywb_status_t GenerateAdvice(SubPoolKey& key, AdviceRule& rule,
            void* less(void* key1, void*key2), void* filterchain);

    /*destroy specified subpool's advise*/
    ywb_status_t Destroy(SubPoolKey& key);
    /*destroy all subpool's advise*/
    ywb_status_t Destroy();

    ywb_status_t Reset();
    ywb_status_t Init();

private:
    /*all subpools' advise*/
    map<SubPoolKey, SubPoolAdvice*, SubPoolKeyCmp> mAdvise;
    Mutex mAdviseLock;
    /*back reference to AST*/
    AST* mAst;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
