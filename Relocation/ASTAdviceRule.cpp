#include "common/FsStatus.h"
#include "AST/ASTAdviceRule.hpp"

ywb_status_t
AdviceRule::ValidateRule()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t partitionscope = 0;
    ywb_uint32_t partitionbase = 0;
    ywb_uint32_t sortbase = 0;

//    if(this->GetAdviceType() >= AdviceType::AT_cnt ||
//            this->GetPartitionScope() >= PartitionScope::PS_cnt ||
//            this->GetPartitionBase() >= PartitionBase::PB_cnt ||
//            this->GetSortBase() >= SortBase::SB_cnt ||
//            this->GetSelector() >= Selector::SEL_cnt)
//    {
//         return YFS_EINVAL;
//    }

    partitionscope = this->GetPartitionScope();
    partitionbase = this->GetPartitionBase();
    sortbase = this->GetSortBase();

    /*only support tier wise and tier internal disk wise heat distribution*/
    if((PartitionScope::PS_tier_wise != partitionscope) &&
            (PartitionScope::PS_tier_internal_disk_wise != partitionscope))
    {
        return YFS_EINVAL;
    }

    /*partition base must be set*/
    if(PartitionBase::PB_none == partitionbase)
    {
        return YFS_EINVAL;
    }

    /*
     * exclusive options can not coexist
     * PB_long_term, PB_short_term and PB_raw are exclusive
     * PB_ema, PB_raw are exclusive
     * PB_random, PB_sequential and PB_rnd_seq are exclusive
     * PB_bw, PB_iops and PB_rt are exclusive
     **/
    if((((PartitionBase::PB_long_term & partitionbase) ==
            PartitionBase::PB_long_term) &&
            ((PartitionBase::PB_short_term & partitionbase) ==
                    PartitionBase::PB_short_term)) ||
            (((PartitionBase::PB_ema & partitionbase) ==
                    PartitionBase::PB_ema) &&
            ((PartitionBase::PB_raw & partitionbase) ==
                    PartitionBase::PB_raw)) ||
            (((PartitionBase::PB_long_term & partitionbase) ==
                    PartitionBase::PB_long_term) &&
            ((PartitionBase::PB_raw & partitionbase) ==
                    PartitionBase::PB_raw)) ||
            (((PartitionBase::PB_short_term & partitionbase) ==
                    PartitionBase::PB_short_term) &&
            ((PartitionBase::PB_raw & partitionbase) ==
                    PartitionBase::PB_raw)) ||
            (((PartitionBase::PB_random & partitionbase) ==
                    PartitionBase::PB_random) &&
            ((PartitionBase::PB_sequential & partitionbase) ==
                PartitionBase::PB_sequential)) ||
            (((PartitionBase::PB_random & partitionbase) ==
                    PartitionBase::PB_random) &&
            ((PartitionBase::PB_rnd_seq & partitionbase) ==
                    PartitionBase::PB_rnd_seq)) ||
            (((PartitionBase::PB_sequential & partitionbase) ==
                    PartitionBase::PB_sequential) &&
            ((PartitionBase::PB_rnd_seq & partitionbase) ==
                    PartitionBase::PB_rnd_seq)) ||
            (((PartitionBase::PB_bw & partitionbase) ==
                    PartitionBase::PB_bw) &&
            ((PartitionBase::PB_iops & partitionbase) ==
                    PartitionBase::PB_iops)) ||
            (((PartitionBase::PB_bw & partitionbase) ==
                    PartitionBase::PB_bw) &&
            ((PartitionBase::PB_rt & partitionbase) ==
                    PartitionBase::PB_rt)) ||
            (((PartitionBase::PB_iops & partitionbase) ==
                    PartitionBase::PB_iops) &&
            ((PartitionBase::PB_rt & partitionbase) ==
                    PartitionBase::PB_rt)))
    {
        return YFS_EINVAL;
    }

    /*sort base must be set*/
    if(SortBase::SB_none == sortbase)
    {
        return YFS_EINVAL;
    }

    /*
     * exclusive options can not coexist
     * SB_long_term, SB_short_term and SB_raw are exclusive
     * SB_ema, SB_raw are exclusive
     * SB_random, SB_sequential and SB_rnd_seq are exclusive
     * SB_bw, SB_iops and SB_rt are exclusive
     **/
    if((((SortBase::SB_long_term & sortbase) ==
            SortBase::SB_long_term) &&
            ((SortBase::SB_short_term & sortbase) ==
                    SortBase::SB_short_term)) ||
            (((SortBase::SB_ema & sortbase) ==
                    SortBase::SB_ema) &&
            ((SortBase::SB_raw & sortbase) ==
                    SortBase::SB_raw)) ||
            (((SortBase::SB_long_term & sortbase) ==
                    SortBase::SB_long_term) &&
            ((SortBase::SB_raw & sortbase) ==
                    SortBase::SB_raw)) ||
            (((SortBase::SB_short_term & sortbase) ==
                    SortBase::SB_short_term) &&
            ((SortBase::SB_raw & sortbase) ==
                    SortBase::SB_raw)) ||
            (((SortBase::SB_random & sortbase) ==
                    SortBase::SB_random) &&
            ((SortBase::SB_sequential & sortbase) ==
                    SortBase::SB_sequential)) ||
            (((SortBase::SB_random & partitionbase) ==
                    SortBase::SB_random) &&
            ((SortBase::SB_rnd_seq & partitionbase) ==
                    SortBase::SB_rnd_seq)) ||
            (((SortBase::SB_sequential & partitionbase) ==
                    SortBase::SB_sequential) &&
            ((SortBase::SB_rnd_seq & partitionbase) ==
                    SortBase::SB_rnd_seq)) ||
            (((SortBase::SB_bw & sortbase) ==
                    SortBase::SB_bw) &&
            ((SortBase::SB_iops & sortbase) ==
                    SortBase::SB_iops)) ||
            (((SortBase::SB_bw & sortbase) ==
                    SortBase::SB_bw) &&
            ((SortBase::SB_rt & sortbase) ==
                    SortBase::SB_rt)) ||
            (((SortBase::SB_iops & sortbase) ==
                    SortBase::SB_iops) &&
            ((SortBase::SB_rt & sortbase) ==
                    SortBase::SB_rt)))
    {
        return YFS_EINVAL;
    }

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
