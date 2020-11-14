/*
 * ASTSwapOut.cpp
 *
 *  Created on: 2016年6月6日
 *      Author: Administrator
 */

#include "AST/ASTSwapOut.hpp"
#include "AST/ASTRelocation.hpp"

SubPoolSwapOut::SubPoolSwapOut() :mHighRatio(4), mMediumRatio(2), mLowRatio(1),
mScheduleRoundUp(0), mScheduleRoundDown(0),
mScheduledSwapLevelUp(SwapOutManager::SL_max),
mScheduledSwapLevelDown(SwapOutManager::SL_max)
{
    YWB_ASSERT(mHighRatio >= mMediumRatio);
    YWB_ASSERT(mMediumRatio >= mLowRatio);
    YWB_ASSERT(mLowRatio >= 0);
}

SubPoolSwapOut::~SubPoolSwapOut()
{
    mHighSwapDisksUp.clear();
    mMediumSwapDisksUp.clear();
    mLowSwapDisksUp.clear();
    mHighSwapDisksDown.clear();
    mMediumSwapDisksDown.clear();
    mLowSwapDisksDown.clear();
}

ywb_status_t SubPoolSwapOut::AddSwapDisk(DiskKey& diskkey, ywb_int32_t val)
{
    set<DiskKey, DiskKeyCmp>::iterator iter;
    ywb_int32_t swaplevel = SwapOutManager::SL_max;
    ywb_int32_t swapdir = SwapOutManager::SD_max;

    swaplevel = SwapOutManager::GetSwapLevel(val);
    swapdir = SwapOutManager::GetSwapDirection(val);
    if(SwapOutManager::SD_up == swapdir)
    {
        if(SwapOutManager::SL_high == swaplevel)
        {
            if(mMediumSwapDisksUp.end() !=
                    (iter = mMediumSwapDisksUp.find(diskkey)))
            {
                if(mMediumUpIt == iter)
                {
                    mMediumUpIt++;
                }

                mMediumSwapDisksUp.erase(iter);
            }
            else if(mLowSwapDisksUp.end() !=
                    (iter = mLowSwapDisksUp.find(diskkey)))
            {
                if(mLowUpIt == iter)
                {
                    mLowUpIt++;
                }

                mLowSwapDisksUp.erase(iter);
            }

            mHighSwapDisksUp.insert(diskkey);
        }
        else if(SwapOutManager::SL_medium == swaplevel)
        {
            if(mHighSwapDisksUp.end() !=
                    (iter = mHighSwapDisksUp.find(diskkey)))
            {
                if(mHighUpIt == iter)
                {
                    mHighUpIt++;
                }

                mHighSwapDisksUp.erase(iter);
            }
            else if(mLowSwapDisksUp.end() !=
                    (iter = mLowSwapDisksUp.find(diskkey)))
            {
                if(mLowUpIt == iter)
                {
                    mLowUpIt++;
                }

                mLowSwapDisksUp.erase(iter);
            }

            mMediumSwapDisksUp.insert(diskkey);
        }
        else if(SwapOutManager::SL_low == swaplevel)
        {
            if(mHighSwapDisksUp.end() !=
                    (iter = mHighSwapDisksUp.find(diskkey)))
            {
                if(mHighUpIt == iter)
                {
                    mHighUpIt++;
                }

                mHighSwapDisksUp.erase(iter);
            }
            else if(mMediumSwapDisksUp.end() !=
                    (iter = mMediumSwapDisksUp.find(diskkey)))
            {
                if(mMediumUpIt == iter)
                {
                    mMediumUpIt++;
                }

                mMediumSwapDisksUp.erase(iter);
            }

            mLowSwapDisksUp.insert(diskkey);
        }
    }
    else if(SwapOutManager::SD_down == swapdir)
    {
        if(SwapOutManager::SL_high == swaplevel)
        {
            if(mMediumSwapDisksDown.end() !=
                    (iter = mMediumSwapDisksDown.find(diskkey)))
            {
                if(mMediumDownIt == iter)
                {
                    mMediumUpIt++;
                }

                mMediumSwapDisksDown.erase(iter);
            }
            else if(mLowSwapDisksDown.end() !=
                    (iter = mLowSwapDisksDown.find(diskkey)))
            {
                if(mLowDownIt == iter)
                {
                    mLowDownIt++;
                }

                mLowSwapDisksDown.erase(iter);
            }

            mHighSwapDisksDown.insert(diskkey);
        }
        else if(SwapOutManager::SL_medium == swaplevel)
        {
            if(mHighSwapDisksDown.end() !=
                    (iter = mHighSwapDisksDown.find(diskkey)))
            {
                if(mHighDownIt == iter)
                {
                    mHighDownIt++;
                }

                mHighSwapDisksDown.erase(iter);
            }
            else if(mLowSwapDisksDown.end() !=
                    (iter = mLowSwapDisksDown.find(diskkey)))
            {
                if(mLowDownIt == iter)
                {
                    mLowDownIt++;
                }

                mLowSwapDisksDown.erase(iter);
            }

            mMediumSwapDisksDown.insert(diskkey);
        }
        else if(SwapOutManager::SL_low == swaplevel)
        {
            if(mHighSwapDisksDown.end() !=
                    (iter = mHighSwapDisksDown.find(diskkey)))
            {
                if(mHighDownIt == iter)
                {
                    mHighDownIt++;
                }

                mHighSwapDisksDown.erase(iter);
            }
            else if(mMediumSwapDisksDown.end() !=
                    (iter = mMediumSwapDisksDown.find(diskkey)))
            {
                if(mMediumDownIt == iter)
                {
                    mMediumUpIt++;
                }

                mMediumSwapDisksDown.erase(iter);
            }

            mLowSwapDisksDown.insert(diskkey);
        }
    }

    return YWB_SUCCESS;
}

ywb_status_t SubPoolSwapOut::RemoveSwapDisk(DiskKey& diskkey)
{
    set<DiskKey, DiskKeyCmp>::iterator iter;

    /*deal with swap up*/
    {
        if(mHighSwapDisksUp.end() !=
                (iter = mHighSwapDisksUp.find(diskkey)))
        {
            if(mHighUpIt == iter)
            {
                mHighUpIt++;
            }

            mHighSwapDisksUp.erase(iter);
        }
        else if(mMediumSwapDisksUp.end() !=
                (iter = mMediumSwapDisksUp.find(diskkey)))
        {
            if(mMediumUpIt == iter)
            {
                mMediumUpIt++;
            }

            mMediumSwapDisksUp.erase(iter);
        }
        else if(mLowSwapDisksUp.end() !=
                (iter = mLowSwapDisksUp.find(diskkey)))
        {
            if(mLowUpIt == iter)
            {
                mLowUpIt++;
            }

            mLowSwapDisksUp.erase(iter);
        }
    }

    /*deal with swap down*/
    {
        if(mHighSwapDisksDown.end() !=
                (iter = mHighSwapDisksDown.find(diskkey)))
        {
            if(mHighDownIt == iter)
            {
                mHighDownIt++;
            }

            mHighSwapDisksDown.erase(iter);
        }
        else if(mMediumSwapDisksDown.end() !=
                (iter = mMediumSwapDisksDown.find(diskkey)))
        {
            if(mMediumDownIt == iter)
            {
                mMediumDownIt++;
            }

            mMediumSwapDisksDown.erase(iter);
        }
        else if(mLowSwapDisksDown.end() !=
                (iter = mLowSwapDisksDown.find(diskkey)))
        {
            if(mLowDownIt == iter)
            {
                mLowDownIt++;
            }

            mLowSwapDisksDown.erase(iter);
        }
    }

    return YWB_SUCCESS;
}

ywb_uint32_t SubPoolSwapOut::GetSwapRequiredDiskNum(ywb_int32_t swapdirection)
{
    ywb_uint32_t num = 0;

    if(SwapOutManager::SD_up == swapdirection)
    {
        num = mHighSwapDisksUp.size() +
                mMediumSwapDisksUp.size() +
                mLowSwapDisksUp.size();
    }
    else if(SwapOutManager::SD_down == swapdirection)
    {
        num = mHighSwapDisksDown.size() +
                mMediumSwapDisksDown.size() +
                mLowSwapDisksDown.size();
    }

    return num;
}

ywb_uint32_t SubPoolSwapOut::GetSwapRequiredDiskNum(
        ywb_int32_t swapdirection, ywb_int32_t swaplevel)
{
    if(SwapOutManager::SD_up == swapdirection)
    {
        if(SwapOutManager::SL_high == swaplevel)
        {
            return mHighSwapDisksUp.size();
        }
        else if(SwapOutManager::SL_medium == swaplevel)
        {
            return mMediumSwapDisksUp.size();
        }
        else if(SwapOutManager::SL_low == swaplevel)
        {
            return mLowSwapDisksUp.size();
        }
        else
        {
            YWB_ASSERT(0);
            return 0;
        }
    }
    else if(SwapOutManager::SD_down == swapdirection)
    {
        if(SwapOutManager::SL_high == swaplevel)
        {
            return mHighSwapDisksDown.size();
        }
        else if(SwapOutManager::SL_medium == swaplevel)
        {
            return mMediumSwapDisksDown.size();
        }
        else if(SwapOutManager::SL_low == swaplevel)
        {
            return mLowSwapDisksDown.size();
        }
        else
        {
            YWB_ASSERT(0);
            return 0;
        }
    }
    else
    {
        YWB_ASSERT(0);
        return 0;
    }
}

ywb_status_t SubPoolSwapOut::ScheduleNextRound(
        ywb_int32_t swapdirection, DiskKey& diskkey)
{
    ywb_status_t ret = YFS_ENOENT;
    static ywb_int32_t fulltriptotalround = mHighRatio + mMediumRatio + mLowRatio;
    static ywb_int32_t lowbound = ((SwapOutManager::SL_max) * mLowRatio);
    static ywb_int32_t mediumbound = (lowbound +
            (SwapOutManager::SL_max - 1) * (mMediumRatio - mLowRatio));
    ywb_int32_t fulltripround = 0;
    ywb_int32_t trytimes = 0;
    ywb_bool_t scheduled = ywb_false;

    if(SwapOutManager::SD_up == swapdirection)
    {
        while((ywb_false == scheduled) && (trytimes < SwapOutManager::SL_max))
        {
            /*calculate the "should-be" swap level*/
            fulltripround = mScheduleRoundUp % fulltriptotalround;
            if(fulltripround < lowbound)
            {
                mScheduledSwapLevelUp = SwapOutManager::SL_high -
                        fulltripround % (SwapOutManager::SL_max);
            }
            else if(fulltripround < mediumbound)
            {
                mScheduledSwapLevelUp = SwapOutManager::SL_medium +
                        fulltripround % (SwapOutManager::SL_high);
            }
            else
            {
                mScheduledSwapLevelUp = SwapOutManager::SL_high;
            }

            /*check whether or not the "should-be" swap level can be satisfied*/
            if((SwapOutManager::SL_high == mScheduledSwapLevelUp) &&
                    (!mHighSwapDisksUp.empty()))
            {
                scheduled = ywb_true;
                mHighUpIt = mHighSwapDisksUp.begin();
                diskkey = *mHighUpIt;
                mHighUpIt++;
                mScheduleRoundUp++;
            }
            else if((SwapOutManager::SL_medium == mScheduledSwapLevelUp) &&
                    (!mMediumSwapDisksUp.empty()))
            {
                scheduled = ywb_true;
                mMediumUpIt = mMediumSwapDisksUp.begin();
                diskkey = *mMediumUpIt;
                mMediumUpIt++;
                mScheduleRoundUp++;
            }
            else if((SwapOutManager::SL_low == mScheduledSwapLevelUp) &&
                    (!mLowSwapDisksUp.empty()))
            {
                scheduled = ywb_true;
                mLowUpIt = mLowSwapDisksUp.begin();
                diskkey = *mLowUpIt;
                mLowUpIt++;
                mScheduleRoundUp++;
            }
            else
            {
                if(SwapOutManager::SL_high == mScheduledSwapLevelUp)
                {
                    /*no disk requires high swap up, then progress to the next medium*/
                    if(fulltripround < mediumbound)
                    {
                        mScheduleRoundUp++;
                    }
                    else
                    {
                        mScheduleRoundUp = ((mScheduleRoundUp + fulltriptotalround - 1) /
                                fulltriptotalround) * fulltriptotalround + 1;
                    }
                }
                else if(SwapOutManager::SL_medium == mScheduledSwapLevelUp)
                {
                    /*no disk requires medium swap up, then progress to the next low or high*/
                    if(fulltripround < lowbound)
                    {
                        mScheduleRoundUp++;
                    }
                    else
                    {
                        YWB_ASSERT(fulltripround < mediumbound);
                        mScheduleRoundUp++;
                    }
                }
                else if(SwapOutManager::SL_low == mScheduledSwapLevelUp)
                {
                    /*no disk requires low swap up, then progress to the next high*/
                    mScheduleRoundUp++;
                }

                trytimes++;
            }
        }

        if(ywb_true == scheduled)
        {
            ret = YWB_SUCCESS;
        }
        else
        {
            mScheduledSwapLevelUp = SwapOutManager::SL_max;
        }
    }
    else if(SwapOutManager::SD_down == swapdirection)
    {
        while((ywb_false == scheduled) && (trytimes < SwapOutManager::SL_max))
        {
            /*calculate the "should-be" swap level*/
            fulltripround = mScheduleRoundDown % fulltriptotalround;
            if(fulltripround < lowbound)
            {
                mScheduledSwapLevelDown = SwapOutManager::SL_high -
                        fulltripround % (SwapOutManager::SL_max);
            }
            else if(fulltripround < mediumbound)
            {
                mScheduledSwapLevelDown = SwapOutManager::SL_medium +
                        fulltripround % (SwapOutManager::SL_high);
            }
            else
            {
                mScheduledSwapLevelDown = SwapOutManager::SL_high;
            }

            /*check whether or not the "should-be" swap level can be satisfied*/
            if((SwapOutManager::SL_high == mScheduledSwapLevelDown) &&
                    (!mHighSwapDisksDown.empty()))
            {
                scheduled = ywb_true;
                mHighDownIt = mHighSwapDisksDown.begin();
                diskkey = *mHighDownIt;
                mHighDownIt++;
                mScheduleRoundDown++;
            }
            else if((SwapOutManager::SL_medium == mScheduledSwapLevelDown) &&
                    (!mMediumSwapDisksDown.empty()))
            {
                scheduled = ywb_true;
                mMediumDownIt = mMediumSwapDisksDown.begin();
                diskkey = *mMediumDownIt;
                mMediumDownIt++;
                mScheduleRoundDown++;
            }
            else if((SwapOutManager::SL_low == mScheduledSwapLevelDown) &&
                    (!mLowSwapDisksDown.empty()))
            {
                scheduled = ywb_true;
                mLowDownIt = mLowSwapDisksDown.begin();
                diskkey = *mLowDownIt;
                mLowDownIt++;
                mScheduleRoundDown++;
            }
            else
            {
                if(SwapOutManager::SL_high == mScheduledSwapLevelDown)
                {
                    /*no disk requires high swap down, then progress to the next medium*/
                    if(fulltripround < mediumbound)
                    {
                        mScheduleRoundDown++;
                    }
                    else
                    {
                        mScheduleRoundDown = ((mScheduleRoundDown + fulltriptotalround - 1) /
                                fulltriptotalround) * fulltriptotalround + 1;
                    }
                }
                else if(SwapOutManager::SL_medium == mScheduledSwapLevelDown)
                {
                    /*no disk requires medium swap down, then progress to the next low or high*/
                    if(fulltripround < lowbound)
                    {
                        mScheduleRoundDown++;
                    }
                    else
                    {
                        YWB_ASSERT(fulltripround < mediumbound);
                        mScheduleRoundDown++;
                    }
                }
                else if(SwapOutManager::SL_low == mScheduledSwapLevelDown)
                {
                    /*no disk requires low swap down, then progress to the next high*/
                    mScheduleRoundDown++;
                }

                trytimes++;
            }
        }

        if(ywb_true == scheduled)
        {
            ret = YWB_SUCCESS;
        }
        else
        {
            mScheduledSwapLevelDown = SwapOutManager::SL_max;
        }
    }

    return ret;
}

ywb_status_t SubPoolSwapOut::ScheduleNextSwapDisk(
        ywb_int32_t swapdirection, DiskKey& diskkey)
{
    ywb_status_t ret = YFS_ENOENT;

    if((SwapOutManager::SD_up == swapdirection) &&
            (!mHighSwapDisksUp.empty() ||
            !mMediumSwapDisksUp.empty() ||
            !mLowSwapDisksUp.empty()))
    {
        if(SwapOutManager::SL_high == mScheduledSwapLevelUp)
        {
            if(mHighSwapDisksUp.end() != mHighUpIt)
            {
                diskkey = *mHighUpIt;
                mHighUpIt++;
                ret = YWB_SUCCESS;
            }
            else
            {
                ret = ScheduleNextRound(SwapOutManager::SD_up, diskkey);
            }
        }
        else if(SwapOutManager::SL_medium == mScheduledSwapLevelUp)
        {
            if(mMediumSwapDisksUp.end() != mMediumUpIt)
            {
                diskkey = *mMediumUpIt;
                mMediumUpIt++;
                ret = YWB_SUCCESS;
            }
            else
            {
                ret = ScheduleNextRound(SwapOutManager::SD_up, diskkey);
            }
        }
        else if(SwapOutManager::SL_low == mScheduledSwapLevelUp)
        {
            if(mLowSwapDisksUp.end() != mLowUpIt)
            {
                diskkey = *mLowUpIt;
                mLowUpIt++;
                ret = YWB_SUCCESS;
            }
            else
            {
                ret = ScheduleNextRound(SwapOutManager::SD_up, diskkey);
            }
        }
        else
        {
            ret = ScheduleNextRound(SwapOutManager::SD_up, diskkey);
        }
    }
    else if((SwapOutManager::SD_down == swapdirection) &&
            (!mHighSwapDisksDown.empty() ||
            !mMediumSwapDisksDown.empty() ||
            !mLowSwapDisksDown.empty()))
    {
        if(SwapOutManager::SL_high == mScheduledSwapLevelDown)
        {
            if(mHighSwapDisksDown.end() != mHighDownIt)
            {
                diskkey = *mHighDownIt;
                mHighDownIt++;
                ret = YWB_SUCCESS;
            }
            else
            {
                ret = ScheduleNextRound(SwapOutManager::SD_down, diskkey);
            }
        }
        else if(SwapOutManager::SL_medium == mScheduledSwapLevelDown)
        {
            if(mMediumSwapDisksDown.end() != mMediumDownIt)
            {
                diskkey = *mMediumDownIt;
                mMediumDownIt++;
                ret = YWB_SUCCESS;
            }
            else
            {
                ret = ScheduleNextRound(SwapOutManager::SD_down, diskkey);
            }
        }
        else if(SwapOutManager::SL_low == mScheduledSwapLevelDown)
        {
            if(mLowSwapDisksDown.end() != mLowDownIt)
            {
                diskkey = *mLowDownIt;
                mLowDownIt++;
                ret = YWB_SUCCESS;
            }
            else
            {
                ret = ScheduleNextRound(SwapOutManager::SD_down, diskkey);
            }
        }
        else
        {
            ret = ScheduleNextRound(SwapOutManager::SD_down, diskkey);
        }
    }

    return ret;
}

ywb_bool_t SubPoolSwapOut::Empty()
{
    return (mHighSwapDisksUp.empty() &&
            mMediumSwapDisksUp.empty() &&
            mLowSwapDisksUp.empty() &&
            mHighSwapDisksDown.empty() &&
            mMediumSwapDisksDown.empty() &&
            mLowSwapDisksDown.empty());
}

ywb_int32_t SwapOutManager::GetCombinedLevelAndDirection(
        ywb_int32_t level, ywb_int32_t direction)
{
    return (level << 16) + direction;
}

ywb_int32_t SwapOutManager::GetSwapLevel(ywb_int32_t val)
{
    return (val & 0xffff0000) >> 16;
}

ywb_int32_t SwapOutManager::GetSwapDirection(ywb_int32_t val)
{
    return (val & 0xffff);
}

ywb_status_t SwapOutManager::AddSubPoolSwapOut(SubPoolKey& subpoolkey)
{
    ywb_status_t ret = YFS_EEXIST;
    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
    SubPoolSwapOut* subpoolswapout = NULL;

    mRWLock.wrlock();
    iter = mSwapSubPools.find(subpoolkey);
    if(mSwapSubPools.end() == iter)
    {
        subpoolswapout = new SubPoolSwapOut();
        YWB_ASSERT(subpoolswapout != NULL);
        mSwapSubPools.insert(std::make_pair(subpoolkey, subpoolswapout));
        ret = YWB_SUCCESS;
    }

    mRWLock.unlock();
    return ret;
}

ywb_status_t SwapOutManager::RemoveSubPoolSwapOut(SubPoolKey& subpoolkey)
{
     ywb_status_t ret = YFS_ENOENT;
     map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
     SubPoolSwapOut* subpoolswapout = NULL;

     mRWLock.wrlock();
     iter = mSwapSubPools.find(subpoolkey);
     if(mSwapSubPools.end() != iter)
     {
         subpoolswapout = iter->second;
         mSwapSubPools.erase(iter);
         delete subpoolswapout;
         subpoolswapout = NULL;
         ret = YWB_SUCCESS;
     }

     mRWLock.unlock();
     return ret;
}

ywb_status_t SwapOutManager::AddSwapDisk(DiskKey& diskkey, ywb_int32_t val)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
    LogicalConfig* config = NULL;
    SubPoolSwapOut* subpoolswapout = NULL;

    YWB_ASSERT(mAST != NULL);
    mAST->GetLogicalConfig(&config);
    YWB_ASSERT(config != NULL);
    ret = config->GetSubPoolKeyFromSubPoolId(diskkey.GetSubPoolId(), &subpoolkey);
    if(YWB_SUCCESS == ret)
    {
        mRWLock.wrlock();
        iter = mSwapSubPools.find(subpoolkey);
        if(mSwapSubPools.end() == iter)
        {
            subpoolswapout = new SubPoolSwapOut();
            YWB_ASSERT(subpoolswapout != NULL);
            mSwapSubPools.insert(std::make_pair(subpoolkey, subpoolswapout));
        }
        else
        {
            subpoolswapout = iter->second;
        }

        ret = subpoolswapout->AddSwapDisk(diskkey, val);
        mRWLock.unlock();
    }

    return ret;
}

ywb_status_t SwapOutManager::RemoveSwapDisk(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
    LogicalConfig* config = NULL;
    SubPoolSwapOut* subpoolswapout = NULL;

    YWB_ASSERT(mAST != NULL);
    mAST->GetLogicalConfig(&config);
    YWB_ASSERT(config != NULL);
    ret = config->GetSubPoolKeyFromSubPoolId(diskkey.GetSubPoolId(), &subpoolkey);
    if(YWB_SUCCESS == ret)
    {
        mRWLock.wrlock();
        iter = mSwapSubPools.find(subpoolkey);
        if(mSwapSubPools.end() != iter)
        {
            subpoolswapout = iter->second;
            ret = subpoolswapout->RemoveSwapDisk(diskkey);
        }

        mRWLock.unlock();
    }

    return ret;
}

ywb_bool_t SwapOutManager::GetSwapRequired(SubPoolKey& subpoolkey)
{
    ywb_bool_t ret = ywb_false;
    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
    SubPoolSwapOut* subpoolswapout = NULL;

    mRWLock.rdlock();
    iter = mSwapSubPools.find(subpoolkey);
    if(mSwapSubPools.end() != iter)
    {
        subpoolswapout = iter->second;
        ret = !(subpoolswapout->Empty());
    }

    mRWLock.unlock();
    return ret;
}

ywb_status_t SwapOutManager::GetSwapRequiredDiskNum(
        SubPoolKey& subpoolkey, ywb_int32_t swapdirection,
        ywb_uint32_t& disknum)
{
    ywb_status_t ret = YFS_ENOENT;
    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
    SubPoolSwapOut* subpoolswapout = NULL;

    mRWLock.rdlock();
    iter = mSwapSubPools.find(subpoolkey);
    if(mSwapSubPools.end() != iter)
    {
        subpoolswapout = iter->second;
        ret = YWB_SUCCESS;
        disknum = subpoolswapout->GetSwapRequiredDiskNum(swapdirection);
    }

    mRWLock.unlock();
    return ret;
}

ywb_bool_t SwapOutManager::GetSwapRelocationDue(SubPoolKey& subpoolkey)
{
    ywb_bool_t ret = ywb_false;
    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;

    mRWLock.rdlock();
    iter = mSwapSubPools.find(subpoolkey);
    if(mSwapSubPools.end() != iter)
    {
        ret = ywb_true;
    }

    mRWLock.unlock();
    return ret;
}

//ywb_bool_t SwapOutManager::GetSwapRequired(SubPoolKey& subpoolkey,
//        DiskKey& diskkey, ywb_int32_t& val)
//{
//    ywb_bool_t ret = ywb_false;
//    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
//    SubPoolSwapOut* subpoolswapout = NULL;
//
//    mRWLock.rdlock();
//    iter = mSwapSubPools.find(subpoolkey);
//    if(mSwapSubPools.end() != iter)
//    {
//        subpoolswapout = iter->second;
//        if(YWB_SUCCESS == subpoolswapout->GetSwapDisk(diskkey, val))
//        {
//            ret = ywb_true;
//        }
//    }
//
//    mRWLock.unlock();
//    return ret;
//}

//ywb_bool_t SwapOutManager::GetSwapRequired(DiskKey& diskkey, ywb_int32_t& val)
//{
//    ywb_bool_t ret = ywb_false;
//    SubPoolKey subpoolkey;
//    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
//    LogicalConfig* config = NULL;
//    SubPoolSwapOut* subpoolswapout = NULL;
//
//    YWB_ASSERT(mAST != NULL);
//    mAST->GetLogicalConfig(&config);
//    YWB_ASSERT(config != NULL);
//    if(YWB_SUCCESS == config->GetSubPoolKeyFromSubPoolId(
//            diskkey.GetSubPoolId(), &subpoolkey))
//    {
//        mRWLock.rdlock();
//        iter = mSwapSubPools.find(subpoolkey);
//        if(mSwapSubPools.end() != iter)
//        {
//            subpoolswapout = iter->second;
//            if(YWB_SUCCESS == subpoolswapout->GetSwapDisk(diskkey, val))
//            {
//                ret = ywb_true;
//            }
//        }
//
//        mRWLock.unlock();
//    }
//
//    return ret;
//}

ywb_status_t SwapOutManager::ScheduleNextSwapDisk(SubPoolKey& subpoolkey,
        ywb_int32_t swapdirection, DiskKey& diskkey)
{
    ywb_bool_t ret = ywb_false;
    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
    SubPoolSwapOut* subpoolswapout = NULL;

    mRWLock.rdlock();
    iter = mSwapSubPools.find(subpoolkey);
    if(mSwapSubPools.end() != iter)
    {
        subpoolswapout = iter->second;
        ret = subpoolswapout->ScheduleNextSwapDisk(swapdirection, diskkey);
    }

    mRWLock.unlock();
    return ret;
}

ywb_status_t SwapOutManager::ResolveRelocationType(
        NotifyRelocationType* relocationtype)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolKey subpoolkey;
    ywb_int32_t val = 0;

    YWB_ASSERT(relocationtype != NULL);
    ret = relocationtype->GetFirstRelocationType(subpoolkey, val);
    while(YWB_SUCCESS == ret)
    {
        /*only cares those subpools with swap out flag set*/
        if((RelocationType::RT_swap_out | val))
        {
            AddSubPoolSwapOut(subpoolkey);
        }
        else
        {
            RemoveSubPoolSwapOut(subpoolkey);
        }

        ret = relocationtype->GetNextRelocationType(subpoolkey, val);
    }

    return YWB_SUCCESS;
}

ywb_status_t SwapOutManager::ResolveSwapRequirement(
        NotifySwapRequirement* requirement)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskKey diskkey;
    ywb_int32_t val = 0;

    YWB_ASSERT(requirement != NULL);
    ret = requirement->GetFirstSwapDisk(diskkey, val);
    while(YWB_SUCCESS == ret)
    {
        AddSwapDisk(diskkey, val);
        ret = requirement->GetNextSwapDisk(diskkey, val);
    }

    return YWB_SUCCESS;
}

ywb_status_t SwapOutManager::ResolveSwapCancellation(
        NotifySwapCancellation* cancellation)
{
    ywb_status_t ret = YWB_SUCCESS;
    list<DiskKey> disks;
    list<DiskKey>::iterator iter;

    YWB_ASSERT(cancellation != NULL);
    ret = cancellation->GetSwapCancelledDisks(&disks);
    if(YWB_SUCCESS == ret)
    {
        iter = disks.begin();
        while(disks.end() != iter)
        {
            RemoveSwapDisk(*iter);
            iter++;
        }
    }

    disks.clear();
    return YWB_SUCCESS;
}

ywb_status_t SwapOutManager::Consolidate()
{
    /*nothing to do curently*/
    return YWB_SUCCESS;
}

ywb_status_t SwapOutManager::Reset()
{
    map<SubPoolKey, SubPoolSwapOut*, SubPoolKeyCmp>::iterator iter;
    SubPoolSwapOut* subpoolswapout = NULL;

    iter = mSwapSubPools.begin();
    while(mSwapSubPools.end() != iter)
    {
        subpoolswapout = iter->second;
        iter++;
        delete subpoolswapout;
        subpoolswapout = NULL;
    }

    mSwapSubPools.clear();
    return YWB_SUCCESS;
}

