/*
 * ASTDiskUltimatePerfCache.cpp
 *
 *  Created on: 2016年5月24日
 *      Author: Administrator
 */

#include "AST/AST.hpp"
#include "AST/ASTDiskProfile.hpp"
#include "AST/ASTTime.hpp"
#include "AST/ASTDiskUltimatePerfCache.hpp"


ywb_uint64_t DiskUltimatePerfCache::GetUltimatePerf(
        DiskBaseProp prop, ywb_uint32_t hint)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskBaseProp, ywb_uint64_t, DiskBasePropCmp>* mapp = NULL;
    DiskUltimatePerfRule* rulep = NULL;
    PerfProfileManager* profilemgr = NULL;
    DiskPerfProfile* diskprofile = NULL;
    TimeManager* timemgr = NULL;
    CycleManager* cyclemgr = NULL;
    DecisionWindowManager* shortwindowmgr = NULL;
    map<DiskBaseProp, ywb_uint64_t, DiskBasePropCmp>::iterator iter;
    ywb_uint32_t diskclass = DiskBaseProp::DCT_cnt;
    ywb_uint32_t bw = 0;
    ywb_uint32_t iops = 0;
    ywb_uint64_t perf = 0;

    YWB_ASSERT(((DiskUltimatePerfCache::F_bandwidth |
                    DiskUltimatePerfCache::F_iops) & hint) &&
                    (((DiskUltimatePerfCache::F_bandwidth |
                    DiskUltimatePerfCache::F_iops) & hint) !=
                    (DiskUltimatePerfCache::F_bandwidth |
                    DiskUltimatePerfCache::F_iops)));
    YWB_ASSERT(((DiskUltimatePerfCache::F_read |
                    DiskUltimatePerfCache::F_write) & hint) &&
                    (((DiskUltimatePerfCache::F_read |
                    DiskUltimatePerfCache::F_write) & hint) !=
                    (DiskUltimatePerfCache::F_read |
                    DiskUltimatePerfCache::F_write)));

    GetAST()->GetPerformanceProfile(&profilemgr);
    YWB_ASSERT(profilemgr != NULL);
    GetAST()->GetTimeManager(&timemgr);
    YWB_ASSERT(timemgr != NULL);
    timemgr->GetCycleManager(&cyclemgr);
    YWB_ASSERT(cyclemgr != NULL);
    timemgr->GetDecisionWindow(DecisionWindowManager::DW_short_term,
            &shortwindowmgr);
    YWB_ASSERT(shortwindowmgr != NULL);

    if((DiskUltimatePerfCache::F_bandwidth & hint) ==
            DiskUltimatePerfCache::F_bandwidth)
    {
        if((DiskUltimatePerfCache::F_read & hint) ==
                DiskUltimatePerfCache::F_read)
        {
            mapp = &mUltimateReadBandwidth;
            rulep = &mReadBandwidthRule;
        }
        else if((DiskUltimatePerfCache::F_write & hint) ==
                DiskUltimatePerfCache::F_write)
        {
            mapp = &mUltimateWriteBandwidth;
            rulep = &mWriteBandwidthRule;
        }
    }
    else if((DiskUltimatePerfCache::F_bandwidth & hint) ==
            DiskUltimatePerfCache::F_bandwidth)
    {
        if((DiskUltimatePerfCache::F_read & hint) ==
                DiskUltimatePerfCache::F_read)
        {
            mapp = &mUltimateReadIOPS;
            rulep = &mReadIOPSRule;
        }
        else if((DiskUltimatePerfCache::F_write & hint) ==
                DiskUltimatePerfCache::F_write)
        {
            mapp = &mUltimateWriteIOPS;
            rulep = &mWriteIOPSRule;
        }
    }

    iter = mapp->find(prop);
    if(mapp->end() != iter)
    {
        perf = iter->second;
    }
    else
    {
        ret = profilemgr->GetDiskPerfProfile(prop, &diskprofile);
        if(diskprofile != NULL)
        {
            ret = diskprofile->CalculateDiskPerf(rulep->GetRWRatio(),
                    rulep->GetIOSize(), &bw, &iops, rulep->GetRndom());
        }

        if(YWB_SUCCESS != ret)
        {
            /*use hard coded value instead*/
            diskclass = prop.GetDiskClass();
            if(DiskBaseProp::DCT_ssd == diskclass)
            {
                bw = Constant::DEFAULT_SSD_BW;
                iops = Constant::DEFAULT_SSD_IOPS;
            }
            else if(DiskBaseProp::DCT_ent == diskclass)
            {
                bw = Constant::DEFAULT_ENT_BW;
                iops = Constant::DEFAULT_ENT_IOPS;
            }
            else if(DiskBaseProp::DCT_sata == diskclass)
            {
                bw = Constant::DEFAULT_SATA_BW;
                iops = Constant::DEFAULT_SATA_IOPS;
            }
        }

        if((DiskUltimatePerfCache::F_bandwidth & hint) ==
                DiskUltimatePerfCache::F_bandwidth)
        {
            /*translate to the maximum bandwidth of one short term decision window*/
            perf = bw * (cyclemgr->GetCyclePeriod()) * (shortwindowmgr->GetCycles());
            mapp->insert(std::make_pair(prop, perf));
        }
        else if((DiskUltimatePerfCache::F_iops & hint) ==
                DiskUltimatePerfCache::F_iops)
        {
            /*translate to the maximum IOPS of one short term decision window*/
            perf = iops * (cyclemgr->GetCyclePeriod()) * (shortwindowmgr->GetCycles());
            mapp->insert(std::make_pair(prop, perf));
        }
    }

    return perf;
}

ywb_status_t DiskUltimatePerfCache::SetUltimatePerfForTest(
        DiskBaseProp& diskprop, ywb_uint64_t readbw,
        ywb_uint64_t writebw, ywb_uint64_t readios,
        ywb_uint64_t writeios)
{
    map<DiskBaseProp, ywb_uint64_t, DiskBasePropCmp>::iterator iter;

    iter = mUltimateReadBandwidth.find(diskprop);
    if(mUltimateReadBandwidth.end() == iter)
    {
        mUltimateReadBandwidth.insert(std::make_pair(diskprop, readbw));
    }

    iter = mUltimateWriteBandwidth.find(diskprop);
    if(mUltimateWriteBandwidth.end() == iter)
    {
        mUltimateWriteBandwidth.insert(std::make_pair(diskprop, writebw));
    }

    iter = mUltimateReadIOPS.find(diskprop);
    if(mUltimateReadIOPS.end() == iter)
    {
        mUltimateReadIOPS.insert(std::make_pair(diskprop, readios));
    }

    iter = mUltimateWriteIOPS.find(diskprop);
    if(mUltimateWriteIOPS.end() == iter)
    {
        mUltimateWriteIOPS.insert(std::make_pair(diskprop, writeios));
    }

    return YWB_SUCCESS;
}

void DiskUltimatePerfCache::Reset()
{
    ast_log_debug("DiskUltimatePerfCache reset");
    mUltimateReadBandwidth.clear();
    mUltimateWriteBandwidth.clear();
    mUltimateReadIOPS.clear();
    mUltimateWriteIOPS.clear();
}

