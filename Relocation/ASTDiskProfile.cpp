#include "stdlib.h"
#include "libxml/parser.h"
#include "libxml/tree.h"
#include "AST/ASTConfig.hpp"
#include "AST/ASTDiskProfile.hpp"


void
SubDiskProfile::AddDiskProfileEntry(DiskProfileEntry* entry)
{
    mPerfEntries.push_back(entry);
}

ywb_status_t
SubDiskProfile::GetFirstEntry(DiskProfileEntry** entry)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mPerfEntries.empty())
    {
        *entry = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        mIt = mPerfEntries.begin();
        *entry = *mIt;
        mIt++;
    }

    return ret;
}

ywb_status_t
SubDiskProfile::GetNextEntry(DiskProfileEntry** entry)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mIt != mPerfEntries.end())
    {
        *entry = *mIt;
        mIt++;
    }
    else
    {
        *entry = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubDiskProfile::GetFirstEntryReverse(DiskProfileEntry** entry)
{
    ywb_status_t ret = YWB_SUCCESS;
    list<DiskProfileEntry*>::reverse_iterator iter;

    if(mPerfEntries.empty())
    {
        *entry = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        iter = mPerfEntries.rbegin();
        *entry = *iter;
    }

    return ret;
}

bool SubDiskProfile::Empty()
{
    return mPerfEntries.empty();
}

void SubDiskProfile::Sort()
{
    if(!mPerfEntries.empty())
    {
        mPerfEntries.sort(DiskProfileEntryCmp());
    }
}

//void DiskPerfProfile::SetRndPerf(
//        ywb_uint32_t workloadclass, SubDiskProfile* entry)
//{
//    YWB_ASSERT(mRndPerf[workloadclass] == NULL);
//    mRndPerf[workloadclass] = entry;
//}
//
//void DiskPerfProfile::SetSeqPerf(
//        ywb_uint32_t workloadclass, SubDiskProfile* entry)
//{
//    YWB_ASSERT(mRndPerf[workloadclass] == NULL);
//    mSeqPerf[workloadclass] = entry;
//}

void
DiskPerfProfile::SetRndPerf(ywb_uint32_t rwratio, SubDiskProfile* entry)
{
    pair<map<ywb_uint32_t, SubDiskProfile*>::iterator, bool> ret;

    ret = mRndPerf.insert(std::make_pair(rwratio, entry));
    YWB_ASSERT(true == ret.second);
}

void
DiskPerfProfile::SetSeqPerf(ywb_uint32_t rwratio, SubDiskProfile* entry)
{
    pair<map<ywb_uint32_t, SubDiskProfile*>::iterator, bool> ret;

    ret = mSeqPerf.insert(std::make_pair(rwratio, entry));
    YWB_ASSERT(true == ret.second);
}

ywb_status_t
DiskPerfProfile::GetRndPerf(ywb_uint32_t rwratio, SubDiskProfile** entry)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ywb_uint32_t, SubDiskProfile*>::iterator iter;

    iter = mRndPerf.find(rwratio);
    if(iter != mRndPerf.end())
    {
        *entry = iter->second;
    }
    else
    {
        *entry = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
DiskPerfProfile::GetSeqPerf(ywb_uint32_t rwratio, SubDiskProfile** entry)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ywb_uint32_t, SubDiskProfile*>::iterator iter;

    iter = mSeqPerf.find(rwratio);
    if(iter != mSeqPerf.end())
    {
        *entry = iter->second;
    }
    else
    {
        *entry = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_bool_t
DiskPerfProfile::RndPerfEmpty()
{
    return mRndPerf.empty();
}

ywb_bool_t
DiskPerfProfile::SeqPerfEmpty()
{
    return mSeqPerf.empty();
}

ywb_uint32_t DiskPerfProfile::LinearInterpolation(
        ywb_uint32_t pointax, ywb_uint32_t pointay,
        ywb_uint32_t pointbx, ywb_uint32_t pointby,
        ywb_uint32_t targetx)
{
    ywb_uint32_t targety = 0;

    if((pointax > targetx) && (targetx > pointbx))
    {
        if(pointby < pointay)
        {
            targety = (((pointay - pointby) * (targetx - pointbx)) /
                    (pointax - pointbx)) + pointby;
        }
        else
        {
            targety = pointby - (((pointby - pointay) * (targetx - pointbx)) /
                    (pointax - pointbx));
        }
    }
    else if((pointbx > targetx) && (targetx > pointax))
    {
        if(pointay < pointby)
        {
            targety = (((pointby - pointay) * (targetx - pointax)) /
                    (pointbx - pointax)) + pointay;
        }
        else
        {
            targety = pointay - (((pointay - pointby) * (targetx - pointax)) /
                    (pointbx - pointax));
        }
    }

    return targety;
}

ywb_uint32_t DiskPerfProfile::WeightedInterpolation(
        ywb_uint32_t weight1, ywb_uint32_t value1,
        ywb_uint32_t weight2, ywb_uint32_t value2)
{
    ywb_uint32_t weight = 0;
    ywb_uint32_t ratio1 = 0;
    ywb_uint32_t ratio2 = 0;
    ywb_uint32_t value = 0;

    weight = weight1 + weight2;
    ratio1 = (weight == 0) ? 0 : ((100 * weight1) / weight);
    ratio2 = 100 - ratio1;

    if((ratio1 == 0) || (value1 == 0))
    {
        value = value2;
    }
    else if((ratio2 == 0) || (value2 == 0))
    {
        value = value1;
    }
    else if(ratio1 * value2 + ratio2 * value1 != 0)
    {
        value = ((value1 * value2) / (ratio1 * value2 + ratio2 * value1)) * 100;
    }

    return value;
}

ywb_bool_t DiskPerfProfile::IsRWRatioFuzzyMatch(
        ywb_uint32_t readratio1, ywb_uint32_t readratio2)
{
    ywb_uint32_t lowerval = 0;
    ywb_uint32_t upperval = 0;

    lowerval = readratio1 * (100 - Constant::RWRATIO_DIFF);
    upperval = readratio1 * (100 + Constant::RWRATIO_DIFF);

    return (lowerval <= (readratio2 * 100)) && ((readratio2 * 100) <= upperval);
}

ywb_bool_t DiskPerfProfile::IsIOSizeFuzzyMatch(
        ywb_uint32_t iosize1, ywb_uint32_t iosize2)
{
    ywb_uint32_t lowerval = 0;
    ywb_uint32_t upperval = 0;

    lowerval = iosize1 * (100 - Constant::IOSIZE_DIFF);
    upperval = iosize1 * (100 + Constant::IOSIZE_DIFF);

    return (lowerval <= (iosize2 * 100)) && ((iosize2 * 100) <= upperval);
}

ywb_status_t
DiskPerfProfile::GetFirstRWRatioLess(
        ywb_uint32_t rwratio, ywb_uint32_t* val, ywb_bool_t random)
{
    ywb_status_t ret = YFS_ENOENT;
    map<ywb_uint32_t, SubDiskProfile*>::iterator iter;

    if(random && (!mRndPerf.empty()))
    {
        iter = mRndPerf.find(rwratio);
        if((iter != mRndPerf.end()) && (iter != mRndPerf.begin()))
        {
            iter--;
            *val = iter->first;
            ret = YWB_SUCCESS;
        }
    }
    else if((!random) && (!mSeqPerf.empty()))
    {
        iter = mSeqPerf.find(rwratio);
        if((iter != mSeqPerf.end()) && (iter != mSeqPerf.begin()))
        {
            iter--;
            *val = iter->first;
            ret = YWB_SUCCESS;
        }
    }

    return ret;
}

ywb_status_t
DiskPerfProfile::GetFirstRWRatioGreater(
        ywb_uint32_t rwratio, ywb_uint32_t* val, ywb_bool_t random)
{
    ywb_status_t ret = YFS_ENOENT;
    map<ywb_uint32_t, SubDiskProfile*>::iterator iter;

    if(random && (!mRndPerf.empty()))
    {
        iter = mRndPerf.find(rwratio);
        if(iter != mRndPerf.end())
        {
            iter++;
            if(iter != mRndPerf.end())
            {
                *val = iter->first;
                ret = YWB_SUCCESS;
            }
        }
    }
    else if((!random) && (!mSeqPerf.empty()))
    {
        iter = mSeqPerf.find(rwratio);
        if(iter != mSeqPerf.end())
        {
            iter++;
            if(iter != mSeqPerf.end())
            {
                *val = iter->first;
                ret = YWB_SUCCESS;
            }
        }
    }

    return ret;
}

/*
 * calculate bandwidth and iops threshold of specified
 * @rwratio and @iosize
 **/
ywb_status_t DiskPerfProfile::CalculateDiskPerf(
        ywb_uint32_t rwratio, ywb_uint32_t iosize,
        ywb_uint32_t *bw, ywb_uint32_t *ios, ywb_bool_t random)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ywb_uint32_t, SubDiskProfile*>::iterator iter;
    SubDiskProfile* subdiskperf = NULL;
    DiskProfileEntry* firstdisk = NULL;
    DiskProfileEntry* lastdisk = NULL;
    DiskProfileEntry* diskentry = NULL;
    DiskProfileEntry* prevdisk = NULL;

    if(random && !RndPerfEmpty())
    {
        iter = mRndPerf.find(rwratio);
        if(iter != mRndPerf.end())
        {
            subdiskperf = iter->second;
        }
    }
    else if(!random && !SeqPerfEmpty())
    {
        iter = mSeqPerf.find(rwratio);
        if(iter != mSeqPerf.end())
        {
            subdiskperf = iter->second;
        }
    }

    if(NULL == subdiskperf)
    {
        return YFS_ENOENT;
    }

    /*
     * check if there is entry fuzzy matching given @iosize,
     * if not, find iosize1 and iosize2 which satisfy iosize1
     * < iosize < iosize2
     *
     * NOTE: here the implementation relies on the fact that
     * SubDiskProfile::mPerfEntries is in descending order
     **/
    ret = subdiskperf->GetFirstEntry(&firstdisk);
    ret = subdiskperf->GetFirstEntryReverse(&lastdisk);
    if((NULL == firstdisk) && (NULL == lastdisk))
    {
        return YFS_ENOENT;
    }
    else if(firstdisk == lastdisk)
    {
        /*
         * FIXME: return bandwidth and iops anyway if there is only
         * one entry in the list
         **/
        *bw = firstdisk->GetBW();
        *ios = firstdisk->GetIOPS();
        return YWB_SUCCESS;
    }
    else
    {
        /*iosize is larger than the maximum in the profile*/
        if(firstdisk->GetIOSize() <= iosize)
        {
            *bw = firstdisk->GetBW();
            *ios = firstdisk->GetIOPS();
            return YWB_SUCCESS;
        }
        else if(lastdisk->GetIOSize() >= iosize)
        {
            *bw = lastdisk->GetBW();
            *ios = lastdisk->GetIOPS();
            return YWB_SUCCESS;
        }

        diskentry = firstdisk;
        prevdisk = NULL;
        ret = YWB_SUCCESS;

        /*
         * when comes here, we are sure that @iosize is less than
         * firstdisk and larger than lastdisk
         **/
        while((YWB_SUCCESS == ret) && (diskentry != NULL) &&
                (diskentry->GetIOSize() > iosize))
        {
            prevdisk = diskentry;
            ret = subdiskperf->GetNextEntry(&diskentry);
        }

        YWB_ASSERT(YWB_SUCCESS == ret);
        YWB_ASSERT(diskentry != NULL);

        if(diskentry->GetIOSize() == iosize)
        {
            *bw = diskentry->GetBW();
            *ios = diskentry->GetIOPS();
            return YWB_SUCCESS;
        }
        else if(diskentry->GetIOSize() < iosize)
        {
            YWB_ASSERT(prevdisk != NULL);
            /*interpolation with @prevdisk and @diskentry*/
            *bw = DiskPerfProfile::LinearInterpolation(
                    prevdisk->GetIOSize(), prevdisk->GetBW(),
                    diskentry->GetIOSize(), diskentry->GetBW(), iosize);
            *ios = DiskPerfProfile::LinearInterpolation(
                    prevdisk->GetIOSize(), prevdisk->GetIOPS(),
                    diskentry->GetIOSize(), diskentry->GetIOPS(), iosize);
        }
    }

    return ret;
}

//ywb_status_t DiskPerfProfile::CalculateDiskPerf(
//        ywb_uint32_t rndseqratio, ywb_uint32_t rndrwratio,
//        ywb_uint32_t rndiosize, ywb_uint32_t seqrwratio,
//        ywb_uint32_t seqiosize, ywb_uint32_t *bw,
//        ywb_uint32_t *ios)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    ywb_uint32_t curclass = Constant::WORKLOAD_CLASSES;
//    ywb_uint32_t prevclass = Constant::WORKLOAD_CLASSES;
//    ywb_uint32_t nextclass = Constant::WORKLOAD_CLASSES;
//    ywb_uint32_t classcap = 0;
//    ywb_uint32_t bw1 = 0;
//    ywb_uint32_t iops1 = 0;
//    ywb_uint32_t bw2 = 0;
//    ywb_uint32_t iops2 = 0;
//    ywb_uint32_t rndbw = 0;
//    ywb_uint32_t rndiops = 0;
//    ywb_uint32_t seqbw = 0;
//    ywb_uint32_t seqiops = 0;
//
//    /*preset @bw and @ios*/
//    *bw = 0;
//    *ios = 0;
//
//    /*
//     * firstly take random rwratio into consideration
//     *
//     * check if there is entry matching given @rndrwratio, if
//     * not, find rwratio1 and rwratio2 which satisfy rwratio1 <
//     * rndrwratio < rwratio2
//     *
//     * NOTE: here the implementation relies on the fact that
//     * mRndPerf[0] and mRndPerf[Constant::WORKLOAD_CLASSES] are
//     * non-NULL.
//     **/
//    /*rwratio ranges from 0 to 100*/
//    classcap = (100 / Constant::WORKLOAD_CLASSES);
//    YWB_ASSERT(classcap != 0);
//    curclass = (rndrwratio / classcap);
//
//    if(IsRWRatioFuzzyMatch(mRndPerf[curclass]->GetRWRatio(), rndrwratio))
//    {
//        ret = CalculateDiskPerf(curclass, rndiosize, &rndbw, &rndiops, true);
//    }
//    else
//    {
//        if(0 == curclass)
//        {
//            prevclass = 0;
//            nextclass = curclass + 1;
//            /*find the successive non-NULL class*/
//            while(nextclass <= Constant::WORKLOAD_CLASSES &&
//                    ((mRndPerf[nextclass] == NULL) ||
//                    mRndPerf[nextclass]->Empty()))
//            {
//                nextclass++;
//            }
//        }
//        else if(Constant::WORKLOAD_CLASSES == curclass)
//        {
//            nextclass = Constant::WORKLOAD_CLASSES;
//            prevclass = Constant::WORKLOAD_CLASSES - 1;
//            /*find the precedent non-NULL class*/
//            while(prevclass >= 0 &&
//                    ((mRndPerf[prevclass] == NULL) ||
//                    mRndPerf[prevclass]->Empty()))
//            {
//                prevclass--;
//            }
//        }
//        else
//        {
//            prevclass = curclass;
//            /*find the precedent(including current class) non-NULL class*/
//            while(prevclass >= 0 &&
//                    ((mRndPerf[prevclass] == NULL) ||
//                    mRndPerf[prevclass]->Empty()))
//            {
//                prevclass--;
//            }
//            /*
//             * base on the fact that mRndPerf[0] is non-NULL
//             **/
//            YWB_ASSERT(prevclass >= 0);
//            YWB_ASSERT(mRndPerf[prevclass] != NULL);
//
//            nextclass = curclass + 1;
//            while(nextclass <= Constant::WORKLOAD_CLASSES &&
//                    mRndPerf[nextclass] == NULL)
//            {
//                nextclass++;
//            }
//            /*
//             * base on the fact that mRndPerf[Constant::WORKLOAD_CLASSES]
//             * is non-NULL
//             **/
//            YWB_ASSERT(nextclass <= Constant::WORKLOAD_CLASSES);
//            YWB_ASSERT(mRndPerf[nextclass] != NULL);
//        }
//
//        /*
//         * it is not possible for precedent class to fuzzy matching with
//         * @rndrwratio, but it is possible for successive class
//         **/
//        if(IsRWRatioFuzzyMatch(mRndPerf[nextclass]->GetRWRatio(), rndrwratio))
//        {
//            ret = CalculateDiskPerf(nextclass, rndiosize, &rndbw, &rndiops, true);
//        }
//        else
//        {
//            /*calculate bw and iops of @prevclass and @nextclass respectively*/
//            ret = CalculateDiskPerf(prevclass, rndiosize, &bw1, &iops1, true);
//            if((ret != YWB_SUCCESS))
//            {
//                return ret;
//            }
//
//            ret = CalculateDiskPerf(nextclass, rndiosize, &bw2, &iops2, true);
//            if((ret != YWB_SUCCESS))
//            {
//                return ret;
//            }
//
//            /*FIXME: is there a more sophisticated interpolation*/
//            rndbw = DiskPerfProfile::LinearInterpolation(
//                    mRndPerf[prevclass]->GetRWRatio(),
//                    bw1, mRndPerf[nextclass]->GetRWRatio(),
//                    bw2, rndrwratio);
//            rndiops = DiskPerfProfile::LinearInterpolation(
//                    mRndPerf[prevclass]->GetRWRatio(),
//                    iops1, mRndPerf[nextclass]->GetRWRatio(),
//                    iops2, rndrwratio);
//        }
//    }
//
//    if((ret != YWB_SUCCESS))
//    {
//        return ret;
//    }
//
//#ifdef IGNORE_SEQ_PERF
//    /*
//     * secondly take sequential rwratio into consideration
//     *
//     * check if there is entry matching given @seqrwratio, if
//     * not, find rwratio1 and rwratio2 which satisfy rwratio1 <=
//     * seqrwratio <= rwratio2
//     *
//     * NOTE: here the implementation relies on the fact that
//     * mSeqPerf[0] and mSeqPerf[Constant::WORKLOAD_CLASSES] are
//     * non-NULL.
//     **/
//    /*rwratio ranges from 0 to 100*/
//    classcap = (100 / Constant::WORKLOAD_CLASSES);
//    YWB_ASSERT(classcap != 0);
//    curclass = (seqrwratio / classcap);
//
//    if(IsRWRatioFuzzyMatch(mSeqPerf[curclass]->GetRWRatio(), seqrwratio))
//    {
//        ret = CalculateDiskPerf(curclass, seqiosize, &seqbw, &seqiops, false);
//    }
//    else
//    {
//        if(0 == curclass)
//        {
//            prevclass = 0;
//            nextclass = curclass + 1;
//            /*find the successive non-NULL class*/
//            while(nextclass <= Constant::WORKLOAD_CLASSES &&
//                    ((mSeqPerf[nextclass] == NULL) ||
//                    mSeqPerf[nextclass]->Empty()))
//            {
//                nextclass++;
//            }
//        }
//        else if(Constant::WORKLOAD_CLASSES == curclass)
//        {
//            nextclass = Constant::WORKLOAD_CLASSES;
//            prevclass = Constant::WORKLOAD_CLASSES - 1;
//            /*find the precedent non-NULL class*/
//            while(prevclass >= 0 &&
//                    ((mSeqPerf[prevclass] == NULL) ||
//                    mSeqPerf[prevclass]->Empty()))
//            {
//                prevclass--;
//            }
//        }
//        else
//        {
//            prevclass = curclass;
//            /*find the precedent(including current class) non-NULL class*/
//            while(prevclass >= 0 &&
//                    ((mSeqPerf[prevclass] == NULL) ||
//                    mSeqPerf[prevclass]->Empty()))
//            {
//                prevclass--;
//            }
//            /*
//             * base on the fact that mRndPerf[0] is non-NULL
//             **/
//            YWB_ASSERT(nextclass <= Constant::WORKLOAD_CLASSES);
//            YWB_ASSERT(mSeqPerf[nextclass] != NULL);
//
//            nextclass = curclass + 1;
//            while(nextclass <= Constant::WORKLOAD_CLASSES &&
//                    mSeqPerf[nextclass] == NULL)
//            {
//                nextclass++;
//            }
//            /*
//             * base on the fact that mRndPerf[Constant::WORKLOAD_CLASSES]
//             * is non-NULL
//             **/
//            YWB_ASSERT(nextclass <= Constant::WORKLOAD_CLASSES);
//            YWB_ASSERT(mSeqPerf[nextclass] != NULL);
//        }
//
//        /*
//         * it is not possible for precedent class to fuzzy matching with
//         * @rndrwratio, but it is possible for successive class
//         **/
//        if(IsRWRatioFuzzyMatch(mSeqPerf[nextclass]->GetRWRatio(), seqrwratio))
//        {
//            ret = CalculateDiskPerf(nextclass, seqiosize, &seqbw, &seqiops, false);
//        }
//        else
//        {
//            /*calculate bw and iops of @prevclass and @nextclass respectively*/
//            ret = CalculateDiskPerf(prevclass, seqiosize, &bw1, &iops1, false);
//            if((ret != YWB_SUCCESS))
//            {
//                return ret;
//            }
//
//            ret = CalculateDiskPerf(nextclass, seqiosize, &bw2, &iops2, false);
//            if((ret != YWB_SUCCESS))
//            {
//                return ret;
//            }
//
//            /*FIXME: is there a more sophisticated interpolation*/
//            seqbw = DiskPerfProfile::LinearInterpolation(
//                    mSeqPerf[prevclass]->GetRWRatio(),
//                    bw1, mSeqPerf[nextclass]->GetRWRatio(),
//                    bw2, seqrwratio);
//            seqiops = DiskPerfProfile::LinearInterpolation(
//                    mSeqPerf[prevclass]->GetRWRatio(),
//                    iops1, mSeqPerf[nextclass]->GetRWRatio(),
//                    iops2, seqrwratio);
//        }
//    }
//
//    if((ret != YWB_SUCCESS))
//    {
//        return ret;
//    }
//#endif
//
//    /*thirdly, interpolation*/
//    *bw = DiskPerfProfile::WeightedInterpolation(
//            rndseqratio, rndbw, (100 - rndseqratio), seqbw);
//    *ios = DiskPerfProfile::WeightedInterpolation(
//            rndseqratio, rndiops, (100 - rndseqratio), seqiops);
//
//    return ret;
//}

ywb_status_t DiskPerfProfile::CalculateDiskPerf(
        ywb_uint32_t rndseqratio, ywb_uint32_t rndrwratio,
        ywb_uint32_t rndiosize, ywb_uint32_t seqrwratio,
        ywb_uint32_t seqiosize, ywb_uint32_t *bw,
        ywb_uint32_t *ios)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t prevratio = 0;
    ywb_uint32_t nextratio = 0;
    ywb_uint32_t bw1 = 0;
    ywb_uint32_t iops1 = 0;
    ywb_uint32_t bw2 = 0;
    ywb_uint32_t iops2 = 0;
    ywb_uint32_t rndbw = 0;
    ywb_uint32_t rndiops = 0;
    ywb_uint32_t seqbw = 0;
    ywb_uint32_t seqiops = 0;
    map<ywb_uint32_t, SubDiskProfile*>::iterator iter;
    ywb_bool_t exactmatch = false;

    /*preset @bw and @ios*/
    *bw = 0;
    *ios = 0;

    /*
     * firstly take random rwratio into consideration
     *
     * check if there is entry matching given @rndrwratio, if
     * not, find rwratio1 and rwratio2 which satisfy rwratio1 <
     * rndrwratio < rwratio2
     *
     * NOTE: here the implementation relies on the fact that
     * mRndPerf[0] and mRndPerf[Constant::WORKLOAD_CLASSES] are
     * non-NULL.
     **/
    if((rndseqratio != 0) && (!RndPerfEmpty()))
    {
        prevratio = 0;
        nextratio = 100;
        if(IsRWRatioFuzzyMatch(prevratio, rndrwratio))
        {
            ret = CalculateDiskPerf(prevratio, rndiosize, &rndbw, &rndiops, true);
        }
        else if(IsRWRatioFuzzyMatch(nextratio, rndrwratio))
        {
            ret = CalculateDiskPerf(nextratio, rndiosize, &rndbw, &rndiops, true);
        }
        else
        {
            iter = mRndPerf.begin();
            while((iter != mRndPerf.end()) && (iter->first < rndrwratio))
            {
                prevratio = iter->first;
                iter++;
            }

            /*we are sure rwratio of 100 do exist*/
            YWB_ASSERT(iter != mRndPerf.end());
            if(iter->first == rndrwratio)
            {
                exactmatch = true;
            }
            else
            {
                nextratio = iter->first;
            }

            if(false == exactmatch)
            {
                /*calculate bw and iops of @prevratio and @nextratio respectively*/
                ret = CalculateDiskPerf(prevratio, rndiosize, &bw1, &iops1, true);
                if((ret != YWB_SUCCESS))
                {
                    return ret;
                }

                ret = CalculateDiskPerf(nextratio, rndiosize, &bw2, &iops2, true);
                if((ret != YWB_SUCCESS))
                {
                    return ret;
                }

                /*FIXME: is there a more sophisticated interpolation*/
                rndbw = DiskPerfProfile::LinearInterpolation(
                        prevratio, bw1, nextratio, bw2, rndrwratio);
                rndiops = DiskPerfProfile::LinearInterpolation(
                        prevratio, iops1, nextratio, iops2, rndrwratio);
            }
            else
            {
                ret = CalculateDiskPerf(rndrwratio, rndiosize, &rndbw, &rndiops, true);
            }
        }

        if((ret != YWB_SUCCESS))
        {
            return ret;
        }
    }

    /*
     * secondly take sequential rwratio into consideration
     *
     * check if there is entry matching given @seqrwratio, if
     * not, find rwratio1 and rwratio2 which satisfy rwratio1 <=
     * seqrwratio <= rwratio2
     *
     * NOTE: here the implementation relies on the fact that
     * mSeqPerf[0] and mSeqPerf[Constant::WORKLOAD_CLASSES] are
     * non-NULL.
     **/
    if((rndseqratio != 100) && (!SeqPerfEmpty()))
    {
        /*use sequential performance metadata if it does exist*/
        prevratio = 0;
        nextratio = 100;
        exactmatch = false;
        if(IsRWRatioFuzzyMatch(prevratio, seqrwratio))
        {
            ret = CalculateDiskPerf(prevratio, seqiosize, &seqbw, &seqiops, false);
        }
        else if(IsRWRatioFuzzyMatch(nextratio, seqrwratio))
        {
            ret = CalculateDiskPerf(nextratio, seqiosize, &seqbw, &seqiops, false);
        }
        else
        {
            iter = mSeqPerf.begin();
            while((iter != mSeqPerf.end()) && (iter->first < seqrwratio))
            {
                prevratio = iter->first;
                iter++;
            }

            /*we are sure rwratio of 100 do exist*/
            YWB_ASSERT(iter != mSeqPerf.end());
            if(iter->first == seqrwratio)
            {
                exactmatch = true;
            }
            else
            {
                nextratio = iter->first;
            }

            if(false == exactmatch)
            {
                bw1 = 0;
                bw2 = 0;
                iops1 = 0;
                iops2 = 0;

                /*calculate bw and iops of @prevratio and @nextratio respectively*/
                ret = CalculateDiskPerf(prevratio, seqiosize, &bw1, &iops1, false);
                if((ret != YWB_SUCCESS))
                {
                    return ret;
                }

                ret = CalculateDiskPerf(nextratio, seqiosize, &bw2, &iops2, false);
                if((ret != YWB_SUCCESS))
                {
                    return ret;
                }

                /*FIXME: is there a more sophisticated interpolation*/
                seqbw = DiskPerfProfile::LinearInterpolation(
                        prevratio, bw1, nextratio, bw2, seqrwratio);
                seqiops = DiskPerfProfile::LinearInterpolation(
                        prevratio, iops1, nextratio, iops2, seqrwratio);
            }
            else
            {
                ret = CalculateDiskPerf(seqrwratio, seqiosize, &seqbw, &seqiops, false);
            }
        }

        if((ret != YWB_SUCCESS))
        {
            return ret;
        }
    }
    else if((rndseqratio != 100) && (!RndPerfEmpty()))
    {
        /*use random performance metadata instead*/
        prevratio = 0;
        nextratio = 100;
        exactmatch = false;
        if(IsRWRatioFuzzyMatch(prevratio, seqrwratio))
        {
            ret = CalculateDiskPerf(prevratio, seqiosize, &seqbw, &seqiops, true);
        }
        else if(IsRWRatioFuzzyMatch(nextratio, seqrwratio))
        {
            ret = CalculateDiskPerf(nextratio, seqiosize, &seqbw, &seqiops, true);
        }
        else
        {
            iter = mRndPerf.begin();
            while((iter != mRndPerf.end()) && (iter->first < seqrwratio))
            {
                prevratio = iter->first;
                iter++;
            }

            /*we are sure rwratio of 100 do exist*/
            YWB_ASSERT(iter != mRndPerf.end());
            if(iter->first == seqrwratio)
            {
                exactmatch = true;
            }
            else
            {
                nextratio = iter->first;
            }

            if(false == exactmatch)
            {
                bw1 = 0;
                bw2 = 0;
                iops1 = 0;
                iops2 = 0;

                /*calculate bw and iops of @prevratio and @nextratio respectively*/
                ret = CalculateDiskPerf(prevratio, seqiosize, &bw1, &iops1, true);
                if((ret != YWB_SUCCESS))
                {
                    return ret;
                }

                ret = CalculateDiskPerf(nextratio, seqiosize, &bw2, &iops2, true);
                if((ret != YWB_SUCCESS))
                {
                    return ret;
                }

                /*FIXME: is there a more sophisticated interpolation*/
                seqbw = DiskPerfProfile::LinearInterpolation(
                        prevratio, bw1, nextratio, bw2, seqrwratio);
                seqiops = DiskPerfProfile::LinearInterpolation(
                        prevratio, iops1, nextratio, iops2, seqrwratio);
            }
            else
            {
                ret = CalculateDiskPerf(seqrwratio, seqiosize, &seqbw, &seqiops, true);
            }
        }

        if((ret != YWB_SUCCESS))
        {
            return ret;
        }
    }

    /*thirdly, interpolation*/
    *bw = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndbw, (100 - rndseqratio), seqbw);
    *ios = DiskPerfProfile::WeightedInterpolation(
            rndseqratio, rndiops, (100 - rndseqratio), seqiops);

    return ret;
}

ywb_status_t ParseDevice(xmlDocPtr doc, xmlNodePtr cur,
        PerfProfileManager* perfmanager)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t prevreadratio = 101;
    ywb_uint32_t curreadratio = 100;
    DiskBaseProp diskbaseprop;
    DiskPerfProfile* diskperf = NULL;
    SubDiskProfile* subdiskperf = NULL;
    DiskProfileEntry* diskperfentry = NULL;
    char* ddmclassstr = NULL;
    char* rpmstr = NULL;
    char* raidstr = NULL;
    char* widthstr = NULL;
    char* capacitystr = NULL;
    char* readratiostr = NULL;
    char* iosizestr = NULL;
    char* bandwidthstr = NULL;
    char* iopsstr = NULL;
    ywb_uint32_t readratio = 0;
    ywb_uint32_t iosize = 0;
    ywb_uint32_t bandwidth = 0;
    ywb_uint32_t iops = 0;

    ddmclassstr = (char*)(xmlGetProp(cur,
            (const xmlChar*)(Constant::DDMCLASS.c_str())));
    rpmstr = (char*)(xmlGetProp(cur,
            (const xmlChar*)(Constant::RPM.c_str())));
    raidstr = (char*)(xmlGetProp(cur,
            (const xmlChar*)(Constant::RAID.c_str())));
    widthstr = (char*)(xmlGetProp(cur,
            (const xmlChar*)(Constant::WIDTH.c_str())));
    capacitystr = (char*)(xmlGetProp(cur,
            (const xmlChar*)(Constant::CAPACITY.c_str())));

    diskbaseprop.SetDiskClass(atoi(ddmclassstr));
    diskbaseprop.SetDiskRPM(atoi(rpmstr));
    diskbaseprop.SetRaidType(atoi(raidstr));
    diskbaseprop.SetRaidWidth(atoi(widthstr));
    diskbaseprop.SetDiskCap(atoi(capacitystr));

    xmlFree(ddmclassstr);
    xmlFree(rpmstr);
    xmlFree(raidstr);
    xmlFree(widthstr);
    xmlFree(capacitystr);

    cur = cur->xmlChildrenNode;
    if(cur != NULL)
    {
        diskperf = new DiskPerfProfile();
        if(NULL == diskperf)
        {
            return YFS_EOUTOFMEMORY;
        }

        while(cur != NULL)
        {
            if(!xmlStrcmp(cur->name, (const xmlChar*)(Constant::PERF.c_str())))
            {
                readratiostr = (char*)(xmlGetProp(cur,
                        (const xmlChar*)(Constant::READRATIO.c_str())));
                iosizestr = (char*)(xmlGetProp(cur,
                        (const xmlChar*)(Constant::IOSIZE.c_str())));
                bandwidthstr = (char*)(xmlGetProp(cur,
                        (const xmlChar*)(Constant::BANDWIDTH.c_str())));
                iopsstr = (char*)(xmlGetProp(cur,
                        (const xmlChar*)(Constant::IOPS.c_str())));

                readratio = atoi(readratiostr);
                iosize = atoi(iosizestr);
                bandwidth = atoi(bandwidthstr);
                iops = atoi(iopsstr);

                xmlFree(readratiostr);
                xmlFree(iosizestr);
                xmlFree(bandwidthstr);
                xmlFree(iopsstr);

                curreadratio = readratio;
                /*a new subdiskprofile begins*/
                if(prevreadratio != curreadratio)
                {
                    if(subdiskperf != NULL)
                    {
                        subdiskperf->Sort();
                    }

                    subdiskperf = new SubDiskProfile(curreadratio);

                    if(NULL == subdiskperf)
                    {
                        ast_log_debug("Out of memory!");
                        delete diskperf;
                        diskperf = NULL;
                        return YFS_EOUTOFMEMORY;
                    }

                    prevreadratio = curreadratio;
                    /*currently only performance for random io*/
                    diskperf->SetRndPerf(curreadratio, subdiskperf);
                }

                diskperfentry = new DiskProfileEntry(readratio,
                        iosize, bandwidth, iops);
                if(diskperfentry != NULL)
                {
                    subdiskperf->AddDiskProfileEntry(diskperfentry);
                }
                else
                {
                    ast_log_debug("Out of memory!");
                    delete diskperf;
                    diskperf = NULL;
                    /*@subdiskperf will be deleted as deletion of @diskperf*/
//                    delete subdiskperf;
//                    subdiskperf = NULL;
                    return YFS_EOUTOFMEMORY;
                }
            }

            cur = cur->next;
        }

        if(subdiskperf != NULL)
        {
            subdiskperf->Sort();
        }

        if(!diskperf->RndPerfEmpty())
        {
            /*make sure rwratio of 0 and 100 do exist*/
            ret = diskperf->GetRndPerf(0, &subdiskperf);
            YWB_ASSERT(YWB_SUCCESS == ret);
            ret = diskperf->GetRndPerf(100, &subdiskperf);
            YWB_ASSERT(YWB_SUCCESS == ret);
        }

        if(!diskperf->SeqPerfEmpty())
        {
            /*make sure rwratio of 0 and 100 do exist*/
            ret = diskperf->GetSeqPerf(0, &subdiskperf);
            YWB_ASSERT(YWB_SUCCESS == ret);
            ret = diskperf->GetSeqPerf(100, &subdiskperf);
            YWB_ASSERT(YWB_SUCCESS == ret);
        }

        ret = perfmanager->AddDiskPerfProfile(diskbaseprop, diskperf);
        if(YFS_EEXIST == ret)
        {
            delete diskperf;
            diskperf = NULL;
        }
    }

    return ret;
}

ywb_status_t ParseProduct(xmlDocPtr doc, xmlNodePtr cur,
        PerfProfileManager* perfmanager)
{
    ywb_status_t ret = YWB_SUCCESS;

    cur = cur->xmlChildrenNode;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)(Constant::DEVICE.c_str())))
        {
            ret = ParseDevice(doc, cur, perfmanager);
        }

        if(YWB_SUCCESS == ret)
        {
            cur = cur->next;
        }
        else
        {
            break;
        }
    }

    return ret;
}

ywb_status_t ParseProductFamily(xmlDocPtr doc, xmlNodePtr cur,
        PerfProfileManager* perfmanager)
{
    ywb_status_t ret = YWB_SUCCESS;

    cur = cur->xmlChildrenNode;
    while(cur != NULL)
    {
        if(!xmlStrcmp(cur->name, (const xmlChar*)(Constant::PRODUCT.c_str())))
        {
            ret = ParseProduct(doc, cur, perfmanager);
        }

        if(YWB_SUCCESS == ret)
        {
            cur = cur->next;
        }
        else
        {
            break;
        }
    }

    return ret;
}

ywb_status_t
PerfProfileManager::BuildDiskPerfProfile(const string& filename)
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    ConfigManager* config = NULL;
    string diskprofilepath;
    string* diskprofilepathp;
    xmlDocPtr doc = NULL;
    xmlNodePtr cur = NULL;

    if(filename.empty())
    {
        GetAST(&ast);
        ast->GetConfigManager(&config);
        config->GetDiskProfile(&diskprofilepathp);
        YWB_ASSERT(diskprofilepathp != NULL);
    }
    else
    {
        diskprofilepath = filename;
        diskprofilepathp = &diskprofilepath;
    }

    doc = xmlParseFile(diskprofilepathp->c_str());
    if(doc != NULL)
    {
        cur = xmlDocGetRootElement(doc);
        if((NULL == cur))
        {
            xmlFreeDoc(doc);
            return YFS_EINVAL;
        }

        if(xmlStrcmp(cur->name, (const xmlChar*)(Constant::DISKPROFILE.c_str())))
        {
            xmlFreeDoc(doc);
            return YFS_EINVAL;
        }

        cur = cur->xmlChildrenNode;
        while(cur != NULL)
        {
            if(!xmlStrcmp(cur->name, (const xmlChar*)(Constant::PRODUCTFAMILY.c_str())))
            {
                ret = ParseProductFamily(doc, cur, this);
            }

            if(YWB_SUCCESS == ret)
            {
                cur = cur->next;
            }
            else
            {
                break;
            }
        }

        xmlFreeDoc(doc);
    }
    else
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
PerfProfileManager::CalculateDiskPerf(DiskBaseProp& prop,
        ywb_uint32_t rndseqratio, ywb_uint32_t rndrwratio,
        ywb_uint32_t rndiosize, ywb_uint32_t seqrwratio,
        ywb_uint32_t seqiosize, ywb_uint32_t *bw,
        ywb_uint32_t *iops)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t ddmclass = DiskBaseProp::DCT_cnt;
    DiskPerfProfile* diskperf = NULL;

    ret = GetDiskPerfProfile(prop, &diskperf);
    if((YWB_SUCCESS == ret) && (diskperf != NULL))
    {
        ret = diskperf->CalculateDiskPerf(rndseqratio,
                rndrwratio, rndiosize, seqrwratio,
                seqiosize, bw, iops);
    }

    /*use hard coded value instead if not find*/
    if((YFS_ENOENT == ret))
    {
        ddmclass = prop.GetDiskClass();
        if(DiskBaseProp::DCT_ssd == ddmclass)
        {
            *bw = Constant::DEFAULT_SSD_BW;
            *iops = Constant::DEFAULT_SSD_IOPS;
            ret = YWB_SUCCESS;
        }
        else if(DiskBaseProp::DCT_ent == ddmclass)
        {
            *bw = Constant::DEFAULT_ENT_BW;
            *iops = Constant::DEFAULT_ENT_IOPS;
            ret = YWB_SUCCESS;
        }
        else if(DiskBaseProp::DCT_sata == ddmclass)
        {
            *bw = Constant::DEFAULT_SATA_BW;
            *iops = Constant::DEFAULT_SATA_IOPS;
            ret = YWB_SUCCESS;
        }
        else
        {
            ret = YFS_EINVAL;
        }
    }

    return ret;
}

ywb_status_t
PerfProfileManager::AddDiskPerfProfile(
        DiskBaseProp& prop, DiskPerfProfile* profile)
{
    ywb_status_t ret = YWB_SUCCESS;
    pair<map<DiskBaseProp, DiskPerfProfile*,
            DiskBasePropCmp>::iterator, bool> pairret;

    pairret = mDiskProfile.insert(make_pair(prop, profile));
    if(!pairret.second)
    {
        ret = YFS_EEXIST;
    }

    return ret;
}

ywb_status_t
PerfProfileManager::GetDiskPerfProfile(
        DiskBaseProp& prop, DiskPerfProfile** profile)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<DiskBaseProp, DiskPerfProfile*, DiskBasePropCmp>::iterator iter;

    iter = mDiskProfile.find(prop);
    if(iter != mDiskProfile.end())
    {
        /*exactly match*/
        *profile = iter->second;
    }
    else
    {
        ret = YFS_ENOENT;
        *profile = NULL;

        /*no exactly matched item, then try fuzzy match*/
        iter = mDiskProfile.begin();
        for(; iter != mDiskProfile.end(); iter++)
        {
            if(prop.MatchFuzzy(iter->first))
            {
                ret = YWB_SUCCESS;
                *profile = iter->second;
                break;
            }
        }
    }

    return ret;
}

ywb_status_t
PerfProfileManager::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;

    Reset();
    mAst = NULL;
    return ret;
}

void
PerfProfileManager::Reset()
{
    map<DiskBaseProp, DiskPerfProfile*, DiskBasePropCmp>::iterator iter;
    DiskPerfProfile* diskperf = NULL;

    ast_log_debug("PerfProfileManager reset");
    iter = mDiskProfile.begin();
    for(; iter != mDiskProfile.end(); )
    {
        diskperf = iter->second;
        mDiskProfile.erase(iter++);
        delete diskperf;
        diskperf = NULL;
    }
}

/* vim:set ts=4 sw=4 expandtab */
