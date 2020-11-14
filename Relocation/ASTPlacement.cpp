/*
 * ASTPlacement.cpp
 *
 *  Created on: 2016年6月11日
 *      Author: Administrator
 */

#include "common/FsStatus.h"
#include "AST/ASTSwapOut.hpp"
#include "AST/ASTPlacement.hpp"
#include "AST/ASTRelocation.hpp"
#include "AST/ASTHUB.hpp"
#include "AST/ASTStoragePolicy.hpp"

void DiskCandidate::Reset()
{
    mTotalCap = 0;
    mReservedCapForHotSwap = 0;
    mReservedCapForColdSwap = 0;
    mAvailCap = 0;
    mRecentlyRemovedOBJNum = 0;
    mIOs = 0;
    mBWs = 0;
    mAvailTickets = 0;
    mFlag.Reset();
}

ywb_status_t TierWiseDiskCandidates::AddDiskCandidate(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    std::pair<map<DiskKey, DiskCandidate*, DiskKeyCmp>::iterator, bool> pairret;
    DiskCandidate* candidate = NULL;

    candidate = new DiskCandidate(diskkey);
    YWB_ASSERT(candidate != NULL);
    pairret = mDisksMap.insert(std::make_pair(diskkey, candidate));
    if(false == pairret.second)
    {
        delete candidate;
        candidate = NULL;
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t TierWiseDiskCandidates::GetDiskCandidate(
        DiskKey& diskkey, DiskCandidate** candidate)
{
    ywb_status_t ret = YFS_ENOENT;
    map<DiskKey, DiskCandidate*, DiskKeyCmp>::iterator iter;

    *candidate = NULL;
    iter = mDisksMap.find(diskkey);
    if(mDisksMap.end() != iter)
    {
        *candidate = iter->second;
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t TierWiseDiskCandidates::RemoveDiskCandidate(DiskKey& diskkey)
{
    ywb_status_t ret = YFS_ENOENT;
    map<DiskKey, DiskCandidate*, DiskKeyCmp>::iterator iter;
    DiskCandidate* candidate = NULL;

    iter = mDisksMap.find(diskkey);
    if(mDisksMap.end() != iter)
    {
        candidate = iter->second;
        delete candidate;
        candidate = NULL;
        mDisksMap.erase(iter);
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t TierWiseDiskCandidates::AddDiskCandidateVec(DiskCandidate* dc)
{
    YWB_ASSERT(dc != NULL);
    mDisksVec.push_back(dc);
    return YWB_SUCCESS;
}

void TierWiseDiskCandidates::MarkCandidatesAsSuspect()
{
    map<DiskKey, DiskCandidate*, DiskKeyCmp>::iterator iter;

    iter = mDisksMap.begin();
    while(mDisksMap.end() != iter)
    {
        iter->second->SetFlagComb(DiskCandidate::DCF_suspecious);
        iter++;
    }
}

void TierWiseDiskCandidates::RemoveUnConfirmedSuspect()
{
    map<DiskKey, DiskCandidate*, DiskKeyCmp>::iterator iter;
    DiskCandidate* candidate = NULL;

    iter = mDisksMap.begin();
    while(mDisksMap.end() != iter)
    {
        candidate = iter->second;
        if(candidate->TestFlag(DiskCandidate::DCF_suspecious))
        {
            mDisksMap.erase(iter++);
            delete candidate;
            candidate = NULL;
        }
        else
        {
            iter++;
        }
    }
}

ywb_bool_t TierWiseDiskCandidates::ShouldExcludeDiskCandidate(DiskKey& diskkey)
{
    std::map<DiskKey, DiskCandidate*, DiskKeyCmp>::iterator iter;
    DiskCandidate* candidate = NULL;

    iter = mDisksMap.find(diskkey);
    if(mDisksMap.end() != iter)
    {
        candidate = iter->second;
        if(candidate->TestFlag(DiskCandidate::DCF_removing) ||
                candidate->TestFlag(DiskCandidate::DCF_removed) ||
                candidate->TestFlag(DiskCandidate::DCF_medium_err) ||
                candidate->TestFlag(DiskCandidate::DCF_busy))
        {
            return ywb_true;
        }

        return ywb_false;
    }

    return ywb_true;
}

ywb_bool_t TierWiseDiskCandidates::ShouldExcludeDiskCandidate(DiskCandidate* candidate)
{
    if(candidate->TestFlag(DiskCandidate::DCF_removing) ||
            candidate->TestFlag(DiskCandidate::DCF_removed) ||
            candidate->TestFlag(DiskCandidate::DCF_medium_err) ||
            candidate->TestFlag(DiskCandidate::DCF_busy))
    {
        return ywb_true;
    }

    return ywb_false;
}

ywb_bool_t TierWiseDiskCandidates::HasAnyDiskCandidate()
{
    return !(TestFlag(TC_no_candidate) || mDisksMap.empty());
}

ywb_bool_t TierWiseDiskCandidates::HasDiskCandidate(DiskKey& diskkey)
{
    return ((mDisksMap.end() != mDisksMap.find(diskkey)) ? ywb_true : ywb_false);
}

ywb_bool_t TierWiseDiskCandidates::FrequentlyRemovedRecently()
{
    std::vector<DiskCandidate*>::iterator iter;
    ywb_uint32_t disktotalcap = 0;
    ywb_uint32_t tiertotalcap = 0;

    iter = mDisksVec.begin();
    while(mDisksVec.end() != iter)
    {
        disktotalcap = (*iter)->GetTotalCap();
        tiertotalcap += disktotalcap;
        if(((*iter)->GetRecentlyRemovedOBJNum() * 1000) > (disktotalcap *
                (Constant::REMOVED_OBJ_THRESHOLD_BEFORE_UPDATE)))
        {
            return ywb_true;
        }

        iter++;
    }

    if((GetRecentlyRemovedOBJNum() * 1000) > (tiertotalcap *
            (Constant::REMOVED_OBJ_THRESHOLD_BEFORE_UPDATE)))
    {
        return ywb_true;
    }

    return ywb_false;
}

void TierWiseDiskCandidates::UpdateRemovedOBJNum(
        DiskKey& diskkey, ywb_int32_t val)
{
    ywb_uint32_t curremoved = 0;
    std::map<DiskKey, DiskCandidate*, DiskKeyCmp>::iterator iter;
    DiskCandidate* candidate = NULL;

    iter = mDisksMap.find(diskkey);
    if(mDisksMap.end() != iter)
    {
        candidate = iter->second;
        curremoved = candidate->GetRecentlyRemovedOBJNum();
        candidate->SetRecentlyRemovedOBJNum(curremoved + val);
    }
}

ywb_status_t
TierWiseDiskCandidates::GetNextDiskCandidate(ywb_int32_t mode, DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    YWB_ASSERT(DiskSelector::DSM_disk_by_disk_round_robin <= mode &&
            mode < DiskSelector::DSM_cnt);
    if(mAvailTicketsNum > 0)
    {
        switch(mode)
        {
        case DiskSelector::DSM_disk_by_disk_round_robin:
            ret = GetNextCandidateDiskByDiskRR(diskkey);
            break;
        case DiskSelector::DSM_disk_by_disk_random:
            ret = GetNextCandidateDiskByDiskRandom(diskkey);
            break;
        case DiskSelector::DSM_ticket_by_ticket_round_robin:
            ret = GetNextCandidateTicketByTicketRR(diskkey);
            break;
        case DiskSelector::DSM_ticket_by_ticket_random:
            ret = GetNextCandidateTicketByTicketRandom(diskkey);
            break;
        default:
            YWB_ASSERT(0);
        }
    }
    else
    {
        ret = YFS_ENOSPACE;
    }

    return ret;
}

ywb_uint32_t TierWiseDiskCandidates::Update(ywb_int32_t base)
{
    ywb_uint32_t reserved = 0;
    ywb_uint32_t index = 0;
    ywb_uint32_t totalavailcap = 0;
    ywb_uint32_t totalcap = 0;
    ywb_uint32_t availratio = 0;
    ywb_uint32_t overloadavailratiosum = 0;
    ywb_uint32_t overloadavailratioavg = 0;
    ywb_uint32_t availratiosum = 0;
    ywb_uint32_t availratioavg = 0;
    ywb_uint32_t maxavailratio = 0;
    ywb_uint32_t minavailratio = 0;
    ywb_int32_t swapdirection = SwapOutManager::SD_max;
    DiskKey diskkey;
    DiskCandidate* candidate = NULL;
    ywb_bool_t tierthresholdreached = ywb_false;
    ywb_uint32_t thresholdreachednum = 0;
    ywb_uint32_t curreserved = 0;
    DiskSelector* diskselector = NULL;
    PolicyManager* policymgr = NULL;
    HUB* hub = NULL;
    std::map<DiskKey, DiskCandidate*, DiskKeyCmp>::iterator iter;

    YWB_ASSERT(DiskSelector::DSB_capacity_proportional <= base &&
            base < DiskSelector::DSB_cnt);

    mDisksVec.clear();
    mAvailTicketsNum = 0;
    mCurIndex = 0;

    if(mDisksMap.empty())
    {
        return 0;
    }

    diskselector = GetDiskSelector();
    YWB_ASSERT(diskselector != NULL);
    policymgr = diskselector->GetPolicyManager();
    YWB_ASSERT(policymgr != NULL);
    hub = policymgr->GetHUB();
    YWB_ASSERT(hub != NULL);

    for(iter = mDisksMap.begin(); iter != mDisksMap.end(); iter++)
    {
        mDisksVec.push_back(iter->second);
    }

    /*sort all disk candidates by specified base*/
    if(DiskSelector::DSB_capacity_proportional == base)
    {
        std::sort(mDisksVec.begin(), mDisksVec.end(),
                DiskSelector::Sort::SortByCapacityProportional);

        for(index = 0; index < mDisksVec.size(); index++)
        {
            candidate = mDisksVec.at(index);
            diskkey = candidate->GetDiskKey();
            /*reset candidate's reserved tickets*/
            candidate->SetAvailTickets(0);
            totalavailcap += candidate->GetAvailCap();
            totalcap += candidate->GetTotalCap();
            availratio = (100 * candidate->GetAvailCap()) / candidate->GetTotalCap();
            availratiosum += availratio;
            if(availratio <= Constant::DISK_AVAIL_CAP_RATIO_THRESHOLD_HIGH)
            {
                overloadavailratiosum += availratio;
                candidate->SetFlagComb(DiskCandidate::DCF_swap_high);
                candidate->ClearFlag(DiskCandidate::DCF_swap_medium);
                candidate->ClearFlag(DiskCandidate::DCF_swap_low);
                swapdirection = GetSwapDirection(diskkey);
                /*will not swap if swap direction is SwapOutManager::SD_max*/
                if(SwapOutManager::SD_max != swapdirection)
                {
                    hub->LaunchSwapRequiredDiskEvent(diskkey,
                            SwapOutManager::SL_high, swapdirection);
                }
            }
            else if(availratio <= Constant::DISK_AVAIL_CAP_RATIO_THRESHOLD_MEDIUM)
            {
                overloadavailratiosum += availratio;
                candidate->SetFlagComb(DiskCandidate::DCF_swap_medium);
                candidate->ClearFlag(DiskCandidate::DCF_swap_high);
                candidate->ClearFlag(DiskCandidate::DCF_swap_low);
                swapdirection = GetSwapDirection(diskkey);
                if(SwapOutManager::SD_max != swapdirection)
                {
                    hub->LaunchSwapRequiredDiskEvent(diskkey,
                            SwapOutManager::SL_medium, swapdirection);
                }
            }
            else if(availratio <= Constant::DISK_AVAIL_CAP_RATIO_THRESHOLD_LOW)
            {
                overloadavailratiosum += availratio;
                candidate->SetFlagComb(DiskCandidate::DCF_swap_low);
                candidate->ClearFlag(DiskCandidate::DCF_swap_high);
                candidate->ClearFlag(DiskCandidate::DCF_swap_medium);
                swapdirection = GetSwapDirection(diskkey);
                if(SwapOutManager::SD_max != swapdirection)
                {
                    hub->LaunchSwapRequiredDiskEvent(diskkey,
                            SwapOutManager::SL_low, swapdirection);
                }
            }
            else
            {
                if((candidate->TestFlag(DiskCandidate::DCF_swap_high)) ||
                        (candidate->TestFlag(DiskCandidate::DCF_swap_medium)) ||
                        (candidate->TestFlag(DiskCandidate::DCF_swap_low)))
                {
                    candidate->ClearFlag(DiskCandidate::DCF_swap_high);
                    candidate->ClearFlag(DiskCandidate::DCF_swap_medium);
                    candidate->ClearFlag(DiskCandidate::DCF_swap_low);
                    /*FIXME: Notify that this candidate will not swap any more*/
                    hub->LaunchSwapCancelledDiskEvent(diskkey);
                }
            }
        }

        if(totalcap != 0)
        {
            availratio = (100 * totalavailcap) / totalcap;
            if(availratio <= Constant::TIER_AVAIL_CAP_RATIO_THRESHOLD)
            {
                tierthresholdreached = ywb_true;
            }
        }

        if(ywb_true == tierthresholdreached)
        {
            for(index = 0; index < mDisksVec.size(); index++)
            {
                candidate = mDisksVec.at(index);
                availratio = (100 * candidate->GetAvailCap()) / candidate->GetTotalCap();
                if((availratio > Constant::DISK_AVAIL_CAP_RATIO_THRESHOLD_LOW) &&
                        (ApproximateEqual(availratio, Constant::DISK_AVAIL_CAP_RATIO_THRESHOLD_LOW, 20)))
                {
                    /*swap out is necessary*/
                    overloadavailratiosum += availratio;
                    candidate->SetFlagComb(DiskCandidate::DCF_swap_low);
                    candidate->ClearFlag(DiskCandidate::DCF_swap_high);
                    candidate->ClearFlag(DiskCandidate::DCF_swap_medium);
                    swapdirection = GetSwapDirection(diskkey);
                    if(SwapOutManager::SD_max != swapdirection)
                    {
                        hub->LaunchSwapRequiredDiskEvent(diskkey,
                                SwapOutManager::SL_low, swapdirection);
                    }
                }
            }
        }

        for(index = 0; index < mDisksVec.size(); index++)
        {
            candidate = mDisksVec.at(index);
            if(candidate->TestFlag(DiskCandidate::DCF_swap_low) ||
                    candidate->TestFlag(DiskCandidate::DCF_swap_medium) ||
                    candidate->TestFlag(DiskCandidate::DCF_swap_high))
            {
                thresholdreachednum++;
            }
        }

        /*take those "lower-capacity utilized" disks at first */
        if((mDisksVec.size() != thresholdreachednum) &&
                (0 != thresholdreachednum))
        {
            overloadavailratioavg = overloadavailratiosum / thresholdreachednum;
            for(index = 0; index < mDisksVec.size(); index++)
            {
                candidate = mDisksVec.at(index);
                if(!(candidate->TestFlag(DiskCandidate::DCF_swap_low) ||
                        candidate->TestFlag(DiskCandidate::DCF_swap_medium) ||
                        candidate->TestFlag(DiskCandidate::DCF_swap_high)))
                {
                    availratio = (100 * candidate->GetAvailCap()) / candidate->GetTotalCap();
                    YWB_ASSERT(availratio >= Constant::DISK_AVAIL_CAP_RATIO_THRESHOLD_LOW);
                    if(0 != overloadavailratioavg)
                    {
                        curreserved = ((availratio - overloadavailratioavg) *
                                (candidate->GetTotalCap())) / 100;
                    }
                    else
                    {
                        /*@availratiosum is too small*/
                        curreserved = ((availratio - Constant::DISK_AVAIL_CAP_RATIO_THRESHOLD_LOW) *
                                (candidate->GetTotalCap())) / 100;
                    }

                    candidate->SetAvailTickets(curreserved);
                    reserved += curreserved;
                }
            }
        }
        else if(0 == thresholdreachednum)
        {
            availratioavg = availratiosum / (mDisksVec.size());
            for(index = 0; index < mDisksVec.size(); index++)
            {
                candidate = mDisksVec.at(index);
                availratio = (100 * candidate->GetAvailCap()) / candidate->GetTotalCap();
                if(availratio > availratioavg)
                {
                    curreserved = ((availratio - availratioavg) * candidate->GetTotalCap()) / 100;
                    candidate->SetAvailTickets(curreserved);
                    reserved += curreserved;
                }
            }
        }

        /*if can not reserve enough tickets on all "lower-capacity utilized" disks*/
        if((mDisksVec.size() == thresholdreachednum) ||
                (reserved < Constant::MINIMUM_RESERVED_TICKET_AT_ONCE))
        {
            candidate = mDisksVec.at(0);
            maxavailratio = (100 * candidate->GetAvailCap()) / candidate->GetTotalCap();
            candidate = mDisksVec.at(mDisksVec.size() - 1);
            minavailratio = (100 * candidate->GetAvailCap()) / candidate->GetTotalCap();
            reserved = 0;
            if(ApproximateEqual(maxavailratio, minavailratio, 5))
            {
                for(index = 0; index < mDisksVec.size(); index++)
                {
                    candidate = mDisksVec.at(index);
                    curreserved =
                            (candidate->GetAvailCap() >
                            Constant::MINIMUM_RESERVED_TICKET_AT_ONCE) ?
                            Constant::MINIMUM_RESERVED_TICKET_AT_ONCE :
                            candidate->GetAvailCap();
                    candidate->SetAvailTickets(curreserved);
                    reserved += curreserved;
                }
            }
            else
            {
                /*try to make all disks' capacity utilization progress to the maximum one*/
                for(index = 0; index < mDisksVec.size(); index++)
                {
                    candidate = mDisksVec.at(index);
                    availratio = (100 * candidate->GetAvailCap()) / candidate->GetTotalCap();
                    YWB_ASSERT(availratio >= minavailratio);
                    curreserved = ((availratio - minavailratio) * candidate->GetTotalCap()) / 100;
                    candidate->SetAvailTickets(curreserved);
                    reserved += curreserved;
                }
            }
        }

        mAvailTicketsNum = reserved;
        if((totalavailcap - reserved) <
            Constant::RESERVED_TICKET_WATER_MARK_CORDON)
        {
            SetFlagComb(TierWiseDiskCandidates::TC_exhausted);
        }
        else
        {
            ClearFlag(TierWiseDiskCandidates::TC_exhausted);
        }
    }
    else if(DiskSelector::DSB_io_proportional == base)
    {
        std::sort(mDisksVec.begin(), mDisksVec.end(),
                DiskSelector::Sort::SortByIOProportional);
    }
    else if(DiskSelector::DSB_bw_proportional == base)
    {
        std::sort(mDisksVec.begin(), mDisksVec.end(),
                DiskSelector::Sort::SortByBWProportional);
    }
    else if(DiskSelector::DSB_performance_based_v1 == base)
    {
        std::sort(mDisksVec.begin(), mDisksVec.end(),
                DiskSelector::Sort::SortByPerformanceV1);
    }

    return reserved;
}

void TierWiseDiskCandidates::Reset()
{
    std::map<DiskKey, DiskCandidate*, DiskKeyCmp>::iterator iter;
    DiskCandidate* disk = NULL;

    iter = mDisksMap.begin();
    while(mDisksMap.end() != iter)
    {
        disk = iter->second;
        iter++;
        delete disk;
        disk = NULL;
    }

    mDisksMap.clear();
    mDisksVec.clear();
    mAvailTicketsNum = 0;
    mCurIndex = 0;
    mRecentlyRemovedOBJNum = 0;
    mFlag.Reset();
    mDiskSelector = NULL;
}

ywb_bool_t TierWiseDiskCandidates::ApproximateEqual(
        ywb_uint32_t vala, ywb_uint32_t valb, ywb_uint32_t ratio)
{
    YWB_ASSERT((0 <= ratio) && (ratio <100));
    if((((100 - ratio) * vala < (100 * valb)) && ((100 * valb) < (100 + ratio) * vala)) ||
            (((100 - ratio) * valb < (100 * vala)) && ((100 * vala) < (100 + ratio) * valb)))
    {
        return ywb_true;
    }

    return ywb_false;
}

ywb_status_t
TierWiseDiskCandidates::GetNextCandidateDiskByDiskRR(DiskKey& diskkey)
{
    DiskCandidate* candidate = NULL;
    ywb_uint32_t anchor = 0;
    ywb_bool_t found = ywb_false;

    if(mAvailTicketsNum <= 0 || mDisksVec.empty())
    {
        return YFS_ENOSPACE;
    }

    candidate = mDisksVec.at(mCurIndex);
    if(0 != candidate->GetAvailTickets())
    {
        candidate->DecreaseAvailTickets(1);
        mAvailTicketsNum--;
        diskkey = candidate->GetDiskKey();
    }
    else
    {
        mCurIndex++;
        anchor = mCurIndex;
        while(mCurIndex < mDisksVec.size())
        {
            candidate = mDisksVec.at(mCurIndex);
            if(0 != candidate->GetAvailTickets())
            {
                candidate->DecreaseAvailTickets(1);
                mAvailTicketsNum--;
                diskkey = candidate->GetDiskKey();
                found = ywb_true;
                break;
            }

            mCurIndex++;
        }

        if(ywb_false == found)
        {
            mCurIndex = 0;
            while(mCurIndex < anchor)
            {
                candidate = mDisksVec.at(mCurIndex);
                if(0 != candidate->GetAvailTickets())
                {
                    candidate->DecreaseAvailTickets(1);
                    mAvailTicketsNum--;
                    diskkey = candidate->GetDiskKey();
                    found = ywb_true;
                    break;
                }

                mCurIndex++;
            }
        }

        YWB_ASSERT(ywb_true == found);
    }

    return YWB_SUCCESS;
}

/*Not implemented yet*/
ywb_status_t
TierWiseDiskCandidates::GetNextCandidateDiskByDiskRandom(DiskKey& diskkey)
{
    return YFS_ENOSPACE;
}

ywb_status_t
TierWiseDiskCandidates::GetNextCandidateTicketByTicketRR(DiskKey& diskkey)
{
    DiskCandidate* candidate = NULL;
    ywb_bool_t found = ywb_false;
    ywb_uint32_t anchor = 0;

    if(mAvailTicketsNum <= 0)
    {
        return YFS_ENOSPACE;
    }

    candidate = mDisksVec.at(mCurIndex);
    if(0 != candidate->GetAvailTickets())
    {
        candidate->DecreaseAvailTickets(1);
        mAvailTicketsNum--;
        diskkey = candidate->GetDiskKey();

        /*prepare next disk candidate index for next invocation*/
        mCurIndex++;
        anchor = mCurIndex;
        while((mAvailTicketsNum > 0) && (mCurIndex < mDisksVec.size()))
        {
            candidate = mDisksVec.at(mCurIndex);
            if(0 != candidate->GetAvailTickets())
            {
                found = ywb_true;
                break;
            }

            mCurIndex++;
        }

        if(ywb_false == found)
        {
            mCurIndex = 0;
            while((mAvailTicketsNum > 0) && (mCurIndex < anchor))
            {
                candidate = mDisksVec.at(mCurIndex);
                if(0 != candidate->GetAvailTickets())
                {
                    found = ywb_true;
                    break;
                }

                mCurIndex++;
            }
        }
    }
    else
    {
        YWB_ASSERT(0);
    }

    return YWB_SUCCESS;
}

/*Not implemented yet*/
ywb_status_t
TierWiseDiskCandidates::GetNextCandidateTicketByTicketRandom(DiskKey& diskkey)
{
    return YFS_ENOSPACE;
}

ywb_int32_t TierWiseDiskCandidates::GetSwapDirection(DiskKey& diskkey)
{
    YWB_ASSERT(mDiskSelector != NULL);
    return mDiskSelector->GetSwapDirection(this);
}

bool DiskSelector::Sort::SortByCapacityProportional(
        const DiskCandidate* disk1, const DiskCandidate* disk2)
{
    ywb_uint32_t availratio1 = 0;
    ywb_uint32_t availratio2 = 0;
    DiskCandidate* cand1 = const_cast<DiskCandidate*>(disk1);
    DiskCandidate* cand2 = const_cast<DiskCandidate*>(disk2);

    availratio1 = (cand1->GetAvailCap() * 100) / (cand1->GetTotalCap());
    availratio2 = (cand2->GetAvailCap() * 100) / (cand2->GetTotalCap());
    if(availratio1 > availratio2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool DiskSelector::Sort::SortByIOProportional(
        const DiskCandidate* disk1, const DiskCandidate* disk2)
{
    /*Not yet implemented*/
    return false;
}

bool DiskSelector::Sort::SortByBWProportional(
        const DiskCandidate* disk1, const DiskCandidate* disk2)
{
    /*Not yet implemented*/
    return false;
}

bool DiskSelector::Sort::SortByPerformanceV1(
        const DiskCandidate* disk1, const DiskCandidate* disk2)
{
    /*Not yet implemented*/
    return false;
}

ywb_status_t DiskSelector::GetNextDiskCandidate(ywb_int32_t tier, DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;

    YWB_ASSERT((Tier::TIER_0 <= tier) && (tier < Tier::TIER_cnt));
    mLocks[tier]->Lock();
    if(!(mCandidates[tier]->TestFlag(TierWiseDiskCandidates::TC_no_candidate) ||
            (mCandidates[tier]->TestFlag(TierWiseDiskCandidates::TC_no_avail_candidate))))
    {
        ret = mCandidates[tier]->GetNextDiskCandidate(mMode, diskkey);
        if((mCandidates[tier]->GetAvailTicketsNum() <
                Constant::RESERVED_TICKET_WATER_MARK_CORDON) &&
                (!(mCandidates[tier]->TestFlag(TierWiseDiskCandidates::TC_exhausted))))
        {
            UpdateLocked(tier, DSE_lack_of_ticket);
        }
    }
    else
    {
        ret = YFS_ENOSPACE;
    }

    mLocks[tier]->Unlock();
    return ret;
}

ywb_status_t DiskSelector::UpdateLocked(ywb_int32_t tier, ywb_int32_t signal)
{
    ywb_status_t ret = YWB_SUCCESS;
    PolicyManager* pm = NULL;
    HUB* hub = NULL;
    ywb_uint32_t reserved = 0;

    YWB_ASSERT((Tier::TIER_0 <= tier) && (tier < Tier::TIER_cnt));
    pm = GetPolicyManager();
    YWB_ASSERT(pm != NULL);
    hub = pm->GetHUB();
    YWB_ASSERT(hub != NULL);

    /*
     * following signals will trigger the updating of DiskSelector
     * DSE_add_disk: lead to configuration change
     * DSE_remove_disk: lead to configuration change
     * DSE_remove_obj:
     *  (1) removed lots of OBJs recently, which cause the previously sorted
     *      DiskCandidates no longer valid, so update is necessary
     *  (2) TierWiseDiskCandidates in "exhausted" state, which is extremely
     *      lack of capacity, update the DiskSelector will make the capacity
     *      freed by the removed OBJ reusable
     * */
    if(DSE_add_disk == signal || DSE_remove_disk == signal ||
            DSE_lack_of_ticket == signal || ((DSE_remove_obj == signal) &&
            ((ywb_true == mCandidates[tier]->FrequentlyRemovedRecently()) ||
            (mCandidates[tier]->TestFlag(TierWiseDiskCandidates::TC_exhausted)))))
    {
        mCandidates[tier]->MarkCandidatesAsSuspect();
        ret = hub->GetTierWiseDisks(mSubPoolKey, tier, mCandidates[tier]);
        mCandidates[tier]->RemoveUnConfirmedSuspect();
        if(YFS_ENOENT == ret || YFS_ENODATA == ret)
        {
            mCandidates[tier]->SetRecentlyRemovedOBJNum(0);
            mCandidates[tier]->SetFlagComb(TierWiseDiskCandidates::TC_no_candidate);
            return ret;
        }
        else
        {
            mCandidates[tier]->ClearFlag(TierWiseDiskCandidates::TC_no_candidate);
            reserved = mCandidates[tier]->Update(mBase);
            if(0 != reserved)
            {
                mCandidates[tier]->ClearFlag(TierWiseDiskCandidates::TC_no_avail_candidate);
            }
            else
            {
                mCandidates[tier]->SetFlagComb(TierWiseDiskCandidates::TC_no_avail_candidate);
            }
        }
    }

    return ret;
}

ywb_status_t DiskSelector::Update(ywb_int32_t tier, ywb_int32_t signal)
{
    ywb_status_t ret = YWB_SUCCESS;

    mLocks[tier]->Lock();
    if(DSE_remove_obj == signal)
    {
        UpdateRemovedOBJNumLocked(tier, 1);
    }

    ret = UpdateLocked(tier, signal);
    mLocks[tier]->Unlock();

    return ret;
}

ywb_status_t DiskSelector::Update(DiskKey& diskkey, ywb_int32_t signal)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_int32_t tier = Tier::TIER_0;

    for(; tier < Tier::TIER_cnt; tier++)
    {
        mLocks[tier]->Lock();
        if(mCandidates[tier]->HasDiskCandidate(diskkey))
        {
            /*
             * assume "DSE_remove_obj" is rare, so will not update DiskSelector
             * again until lots of "DSE_remove_obj" signals comes which cause
             * the previously updated DiskSelector no longer valid
             * */
            if(DSE_remove_obj == signal)
            {
                UpdateRemovedOBJNumLocked(tier, diskkey, 1);
            }

            ret = UpdateLocked(tier, signal);
            mLocks[tier]->Unlock();
            break;
        }

        mLocks[tier]->Unlock();
    }

    return ret;
}

ywb_int32_t DiskSelector::GetSwapDirection(TierWiseDiskCandidates* tdc)
{
    ywb_int32_t direction  = SwapOutManager::SD_max;
    ywb_int32_t tier = Tier::TIER_0;
    ywb_int32_t curtier = Tier::TIER_0;
    ywb_bool_t tierhascandidate[Tier::TIER_cnt] = {ywb_false, ywb_false, ywb_false};
    PolicyManager* pm = NULL;

    pm = GetPolicyManager();
    YWB_ASSERT(pm != NULL);
    /*get swap direction determined by InitialPlacementPolicy*/
    direction = pm->GetSwapDirection(mSubPoolKey);

    for(; tier < Tier::TIER_cnt; tier++)
    {
        mLocks[tier]->Lock();
        if(mCandidates[tier]->HasAnyDiskCandidate())
        {
            tierhascandidate[tier] = ywb_true;
        }

        if(tdc == mCandidates[tier])
        {
            curtier = tier;
        }

        mLocks[tier]->Unlock();
    }

    YWB_ASSERT(Tier::TIER_0 <= curtier);
    YWB_ASSERT(Tier::TIER_2 >= curtier);
    /*fix swap direction by considering tier combination*/
    if((SwapOutManager::SD_up == direction) &&
            (curtier > Tier::TIER_0))
    {
        for(tier = (curtier - 1); tier >= Tier::TIER_0; tier--)
        {
            if(ywb_true == tierhascandidate[tier])
            {
                return direction;
            }
        }
    }
    else if((SwapOutManager::SD_down == direction) && (curtier < Tier::TIER_2))
    {
        for(tier = (curtier + 1); tier <= Tier::TIER_2; tier++)
        {
            if(ywb_true == tierhascandidate[tier])
            {
                return direction;
            }
        }
    }

    return SwapOutManager::SD_max;
}

void DiskSelector::UpdateRemovedOBJNumLocked(ywb_int32_t tier, ywb_int32_t val)
{
    ywb_uint32_t curremoved = 0;

    YWB_ASSERT((Tier::TIER_0 <= tier) && (tier < Tier::TIER_cnt));
    curremoved = mCandidates[tier]->GetRecentlyRemovedOBJNum();
    mCandidates[tier]->SetRecentlyRemovedOBJNum(curremoved + val);
}

void DiskSelector::UpdateRemovedOBJNum(ywb_int32_t tier, ywb_int32_t val)
{
    YWB_ASSERT((Tier::TIER_0 <= tier) && (tier < Tier::TIER_cnt));
    mLocks[tier]->Lock();
    UpdateRemovedOBJNumLocked(tier, val);
    mLocks[tier]->Unlock();
}

void DiskSelector::UpdateRemovedOBJNumLocked(ywb_int32_t tier,
        DiskKey& diskkey, ywb_int32_t val)
{
    ywb_uint32_t curremoved = 0;

    YWB_ASSERT((Tier::TIER_0 <= tier) && (tier < Tier::TIER_cnt));
    curremoved = mCandidates[tier]->GetRecentlyRemovedOBJNum();
    mCandidates[tier]->SetRecentlyRemovedOBJNum(curremoved + val);
    mCandidates[tier]->UpdateRemovedOBJNum(diskkey, val);
}

void DiskSelector::UpdateRemovedOBJNum(DiskKey& diskkey, ywb_int32_t val)
{
    ywb_int32_t tier = Tier::TIER_0;

    for(; tier < Tier::TIER_cnt; tier++)
    {
        mLocks[tier]->Lock();
        if(mCandidates[tier]->HasDiskCandidate(diskkey))
        {
            UpdateRemovedOBJNumLocked(tier, diskkey, val);
            mLocks[tier]->Unlock();
            break;
        }

        mLocks[tier]->Unlock();
    }
}

InitialPlacementDriver::InitialPlacementDriver(PolicyManager* pm) : mPolicyMgr(pm)
{
    YWB_ASSERT(pm != NULL);
}

InitialPlacementDriver::~InitialPlacementDriver()
{
    mPolicyMgr = NULL;
}

ywb_int32_t InitialPlacementDriver::GetSwapDirection()
{
    return SwapOutManager::SD_max;
}

ywb_status_t PerformancePreferredDriver::Adjust(
        SubPoolKey& subpoolkey, ywb_int32_t& applied)
{
    ywb_status_t ret = YWB_SUCCESS;
    PolicyManager* pm = NULL;
    HUB* hub = NULL;
    SubPoolView* subpoolview = NULL;
    ywb_uint32_t tiercomb = SubPool::TC_cnt;

    pm = GetPolicyManager();
    YWB_ASSERT(pm != NULL);
    hub = pm->GetHUB();
    YWB_ASSERT(hub != NULL);

    /*no lock protection which will sacrifice the consistency to some extent*/
    ret = hub->GetSubPoolView(subpoolkey, &subpoolview);
    if((YWB_SUCCESS == ret) && (subpoolview != NULL))
    {
        tiercomb = subpoolview->GetCurTierComb();
        if(SubPool::TC_ssd_ent == tiercomb ||
                SubPool::TC_ssd_sata == tiercomb ||
                SubPool::TC_ssd_ent_sata == tiercomb)
        {
            applied = InitialPlacementType::IP_performance_preferred;
        }
        else
        {
            applied = InitialPlacementType::IP_auto;
            /*will silently take this situation as success*/
        }
    }

    return ret;
}

ywb_status_t PerformancePreferredDriver::ChooseAppropriateDisk(
        SubPoolKey& subpoolkey, DiskKey& disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    PolicyManager* pm = NULL;
    DiskSelector* diskselector = NULL;
    ywb_int32_t tier = Tier::TIER_0;

    pm = GetPolicyManager();
    YWB_ASSERT(pm != NULL);
    ret = pm->GetDiskSelector(subpoolkey, &diskselector);
    YWB_ASSERT(YWB_SUCCESS == ret);

    /*from highest performance tier downward to lowest tier*/
    for(; tier < Tier::TIER_cnt; tier++)
    {
        ret = diskselector->GetNextDiskCandidate(tier, disk);
        if(YWB_SUCCESS == ret)
        {
            break;
        }
    }

    return ret;
}

ywb_int32_t PerformancePreferredDriver::GetSwapDirection()
{
    return SwapOutManager::SD_down;
}

ywb_int32_t PerformancePreferredDriver::GetRelocationType()
{
    return RelocationType::RT_swap_out;
}

ywb_status_t AutoDriver::Adjust(SubPoolKey& subpoolkey, ywb_int32_t& applied)
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_status_t AutoDriver::ChooseAppropriateDisk(SubPoolKey& subpool, DiskKey& disk)
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_int32_t AutoDriver::GetRelocationType()
{
    return RelocationType::RT_auto_tiering;
}

/*FIXME: currently same as PerformancePreferred mode*/
ywb_status_t PerformanceFirstAndThenAutoDriver::Adjust(
        SubPoolKey& subpoolkey, ywb_int32_t& applied)
{
    ywb_status_t ret = YWB_SUCCESS;
    PolicyManager* pm = NULL;
    HUB* hub = NULL;
    SubPoolView* subpoolview = NULL;
    ywb_uint32_t tiercomb = SubPool::TC_cnt;

    pm = GetPolicyManager();
    YWB_ASSERT(pm != NULL);
    hub = pm->GetHUB();
    YWB_ASSERT(hub != NULL);

    /*no lock protection which will sacrifice the consistency to some extent*/
    ret = hub->GetSubPoolView(subpoolkey, &subpoolview);
    if((YWB_SUCCESS == ret) && (subpoolview != NULL))
    {
        tiercomb = subpoolview->GetCurTierComb();
        if(SubPool::TC_ssd_ent == tiercomb ||
                SubPool::TC_ssd_sata == tiercomb ||
                SubPool::TC_ssd_ent_sata == tiercomb)
        {
            applied = InitialPlacementType::IP_performance_first_and_then_auto;
        }
        else
        {
            applied = InitialPlacementType::IP_auto;
            /*will silently take this situation as success*/
        }
    }

    return ret;
}

/*FIXME: currently same as PerformancePreferred mode*/
ywb_status_t PerformanceFirstAndThenAutoDriver::ChooseAppropriateDisk(
        SubPoolKey& subpoolkey, DiskKey& disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    PolicyManager* pm = NULL;
    DiskSelector* diskselector = NULL;
    ywb_int32_t tier = Tier::TIER_0;

    pm = GetPolicyManager();
    YWB_ASSERT(pm != NULL);
    ret = pm->GetDiskSelector(subpoolkey, &diskselector);
    YWB_ASSERT(YWB_SUCCESS == ret);

    /*from highest performance tier downward to lowest tier*/
    for(; tier < Tier::TIER_cnt; tier++)
    {
        ret = diskselector->GetNextDiskCandidate(tier, disk);
        if(YWB_SUCCESS == ret)
        {
            break;
        }
    }

    return ret;
}

ywb_int32_t PerformanceFirstAndThenAutoDriver::GetRelocationType()
{
    return RelocationType::RT_auto_tiering;
}

ywb_status_t CostPreferredDriver::Adjust(
        SubPoolKey& subpoolkey, ywb_int32_t& applied)
{
    ywb_status_t ret = YWB_SUCCESS;
    PolicyManager* pm = NULL;
    HUB* hub = NULL;
    SubPoolView* subpoolview = NULL;
    ywb_uint32_t tiercomb = SubPool::TC_cnt;

    pm = GetPolicyManager();
    YWB_ASSERT(pm != NULL);
    hub = pm->GetHUB();
    YWB_ASSERT(hub != NULL);

    /*no lock protection which will sacrifice the consistency to some extent*/
    ret = hub->GetSubPoolView(subpoolkey, &subpoolview);
    if((YWB_SUCCESS == ret) && (subpoolview != NULL))
    {
        tiercomb = subpoolview->GetCurTierComb();
        if(SubPool::TC_ssd_sata == tiercomb ||
                SubPool::TC_ent_sata == tiercomb ||
                SubPool::TC_ssd_ent_sata == tiercomb)
        {
            applied = InitialPlacementType::IP_cost_preferred;
        }
        else
        {
            applied = InitialPlacementType::IP_auto;
            /*will silently take this situation as success*/
        }
    }

    return ret;
}

ywb_status_t CostPreferredDriver::ChooseAppropriateDisk(
        SubPoolKey& subpoolkey, DiskKey& disk)
{
    ywb_status_t ret = YWB_SUCCESS;
    PolicyManager* pm = NULL;
    DiskSelector* diskselector = NULL;
    ywb_int32_t tier = Tier::TIER_cnt;

    pm = GetPolicyManager();
    YWB_ASSERT(pm != NULL);
    ret = pm->GetDiskSelector(subpoolkey, &diskselector);
    YWB_ASSERT(YWB_SUCCESS == ret);

    /*from lowest performance tier upward to highest tier*/
    for(; tier >= Tier::TIER_0; tier--)
    {
        ret = diskselector->GetNextDiskCandidate(tier, disk);
        if(YWB_SUCCESS == ret)
        {
            break;
        }
    }

    return ret;
}

ywb_int32_t CostPreferredDriver::GetSwapDirection()
{
    return SwapOutManager::SD_up;
}

ywb_int32_t CostPreferredDriver::GetRelocationType()
{
    return RelocationType::RT_swap_out;
}

ywb_status_t NoDataMovementDriver::Adjust(
        SubPoolKey& subpoolkey, ywb_int32_t& applied)
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_status_t NoDataMovementDriver::ChooseAppropriateDisk(
        SubPoolKey& subpoolkey, DiskKey& disk)
{
    ywb_status_t ret = YWB_SUCCESS;

    return ret;
}

ywb_int32_t NoDataMovementDriver::GetRelocationType()
{
    return RelocationType::RT_no_data_movement;
}


