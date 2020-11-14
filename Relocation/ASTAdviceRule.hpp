#ifndef __AST_ADVICE_RULE_HPP__
#define __AST_ADVICE_RULE_HPP__

class AdviceType {
public:
    enum {
        /*cross-tier hot advice*/
        AT_ctr_hot = 0,
        /*cross-tier cold advice*/
        AT_ctr_cold,
#if 0
        /*warm demote*/
        AT_warm_demote,
        /*intra-tier hot advice*/
        AT_itr_hot,
        /*intra-tier cold advice*/
        AT_itr_cold,
#endif
        /*advice for swap up*/
        AT_swap_up,
        /*advice for swap down*/
        AT_swap_down,

        /*make sure Constant::ADVICE_TYPE_CNT equals to or larger than AT_cnt*/
        AT_cnt,
    };
};

class PartitionScope
{
public:
    enum {
        /*specified disk have its own disk-wise partition*/
        PS_disk_wise = 0,
        /*all disks in specified tier have their disk-wise partition*/
        PS_tier_internal_disk_wise,
        /*all OBJs in one tier, regardless of disk*/
        PS_tier_wise,
        /*all OBJs in one sub-pool, regardless of tier*/
        PS_subpool_wise,

        PS_cnt,
    };
};

/*on what partition is based*/
class PartitionBase {
public:
    enum {
        PB_none = 0x0000,
        /*partition base on long-term IO*/
        PB_long_term = 0x0001,
        /*partition base on short-term IO*/
        PB_short_term = 0x0002,
        /*partition base on RAW IO*/
        PB_raw = 0x0004,
        /*partition base on EMA IO*/
        PB_ema = 0x0008,
        /*partition base on random access*/
        PB_random = 0x0010,
        /*partition base on sequential access*/
        PB_sequential = 0x0020,
        /*partition base on random+sequential access*/
        PB_rnd_seq = 0x0040,
        /*partition base on bandwidth*/
        PB_bw = 0x0080,
        /*partition base on iops*/
        PB_iops = 0x0100,
        /*partition base on response time*/
        PB_rt = 0x0200,

        PB_cnt,
    };
};

/*on what sorting is based, can be combination of them*/
class SortBase{
public:
    enum {
        SB_none = 0x0000,
        /*sort base on long-term EMA*/
        SB_long_term = 0x0001,
        /*sort base on short-term EMA*/
        SB_short_term = 0x0002,
        /*sort base on RAW IO*/
        SB_raw = 0x0004,
        /*sort base on EMA IO*/
        SB_ema = 0x0008,
        /*sort base on random access*/
        SB_random = 0x0010,
        /*sort base on sequential access*/
        SB_sequential = 0x0020,
        /*sort base on random+sequential access*/
        SB_rnd_seq = 0x0040,
        /*sort base on bandwidth*/
        SB_bw = 0x0080,
        /*sort base on iops*/
        SB_iops = 0x0100,
        /*sort base on response time*/
        SB_rt = 0x0200,

        SB_cnt,
    };
};

/*sorting direction*/
class SortDir {
    enum {
        /*in ascending order*/
        SD_ascending = 0,
        /*in descending order*/
        SD_decending,
    };
};

/*filter direction*/
class Selector {
public:
    enum {
        /*
         * select from larger part, buckets above hot/cold bucket boundary
         * are larger part
         **/
        SEL_from_larger = 0,
        /*
         * select from smaller part, buckets below hot/cold bucket boundary
         * are smaller part
         **/
        SEL_from_smaller,

        SEL_cnt,
    };
};

class AdviceRule{
public:
    friend class AdviceRuleCmp;
    AdviceRule() : mAdviceType(AdviceType::AT_cnt),
            mPartitionScope(PartitionScope::PS_cnt),
            mPartitionBase(PartitionBase::PB_cnt),
            mSortBase(SortBase::SB_cnt),
            mSelector(Selector::SEL_cnt){}

    AdviceRule(ywb_uint32_t advicetype, ywb_uint32_t partitionscope,
            ywb_uint32_t partitionbase, ywb_uint32_t sortbase,
            ywb_uint32_t selector): mAdviceType(advicetype),
            mPartitionScope(partitionscope), mPartitionBase(partitionbase),
            mSortBase(sortbase), mSelector(selector){}

    AdviceRule(const AdviceRule& rule)
    {
        this->mAdviceType = rule.mAdviceType;
        this->mPartitionScope = rule.mPartitionScope;
        this->mPartitionBase = rule.mPartitionBase;
        this->mSortBase = rule.mSortBase;
        this->mSelector = rule.mSelector;
    }

    virtual ~AdviceRule(){}

    AdviceRule& operator= (const AdviceRule& rule)
    {
        if(this == &rule)
        {
            return *this;
        }

        this->mAdviceType = rule.mAdviceType;
        this->mPartitionScope = rule.mPartitionScope;
        this->mPartitionBase = rule.mPartitionBase;
        this->mSortBase = rule.mSortBase;
        this->mSelector = rule.mSelector;

        return *this;
    }

    inline ywb_uint32_t GetAdviceType()
    {
        return mAdviceType;
    }

    inline ywb_uint32_t GetPartitionScope()
    {
        return mPartitionScope;
    }

    inline ywb_uint32_t GetPartitionBase()
    {
        return mPartitionBase;
    }

    inline ywb_uint32_t GetSortBase()
    {
        return mSortBase;
    }

    inline ywb_uint32_t GetSelector()
    {
        return mSelector;
    }

    inline void SetAdviceType(ywb_uint32_t val)
    {
        mAdviceType = val;
    }

    inline void SetPartitionScope(ywb_uint32_t val)
    {
        mPartitionScope = val;
    }

    inline void SetPartitionBase(ywb_uint32_t val)
    {
        mPartitionBase = val;
    }

    inline void SetSortBase(ywb_uint32_t val)
    {
        mSortBase = val;
    }

    inline void SetSelector(ywb_uint32_t val)
    {
        mSelector = val;
    }

    virtual ywb_status_t ValidateRule();

private:
    /*advice type*/
    ywb_uint32_t mAdviceType;
    /*partition scope*/
    ywb_uint32_t mPartitionScope;
    /*partition base*/
    ywb_uint32_t mPartitionBase;
    /*sort base*/
    ywb_uint32_t mSortBase;
    /*selector*/
    ywb_uint32_t mSelector;
};

class AdviceRuleCmp {
public:
    bool operator()(const AdviceRule& rule1, const AdviceRule& rule2) const
    {
        if(rule1.mAdviceType != rule2.mAdviceType)
            return rule1.mAdviceType < rule2.mAdviceType;
        if(rule1.mPartitionScope != rule2.mPartitionScope)
            return rule1.mPartitionScope < rule2.mPartitionScope;
        if(rule1.mPartitionBase != rule2.mPartitionBase)
            return rule1.mPartitionBase < rule2.mPartitionBase;
        if(rule1.mSortBase != rule2.mSortBase)
            return rule1.mSortBase < rule2.mSortBase;
        if(rule1.mSelector != rule2.mSelector)
            return rule1.mSelector < rule2.mSelector;
        else
            return false;
    }
};

#endif

/* vim:set ts=4 sw=4 expandtab */
