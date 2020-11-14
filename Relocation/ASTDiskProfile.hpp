#ifndef __AST_DISK_PROFILE_HPP__
#define __AST_DISK_PROFILE_HPP__

#include <map>
#include <string>
#include "common/FsInts.h"
#include "common/FsStatus.h"
#include "AST/ASTConstant.hpp"
#include "AST/ASTLogicalConfig.hpp"

using namespace std;

class DiskProfileEntry {
public:
    friend class DiskProfileEntryCmp;
    DiskProfileEntry() : mRWRatio(0), mIOSize(0), mBW(0), mIOPS(0) {}
    DiskProfileEntry(ywb_uint32_t rwratio, ywb_uint32_t iosize,
            ywb_uint32_t bw, ywb_uint32_t iops) : mRWRatio(rwratio),
            mIOSize(iosize), mBW(bw), mIOPS(iops) {}

    inline ywb_uint32_t GetRWRatio()
    {
        return mRWRatio;
    }

    inline ywb_uint32_t GetIOSize()
    {
        return mIOSize;
    }

    inline ywb_uint32_t GetBW()
    {
        return mBW;
    }

    inline ywb_uint32_t GetIOPS()
    {
        return mIOPS;
    }

private:
    /*read/write ratio*/
    ywb_uint32_t mRWRatio;
    /*iosize, in measure of KB*/
    ywb_uint32_t mIOSize;
    /*bandwidth, in measure of MB*/
    ywb_uint32_t mBW;
    /*IOPS*/
    ywb_uint32_t mIOPS;
};

class DiskProfileEntryCmp
{
public:
    bool operator()(
            DiskProfileEntry* entry1, DiskProfileEntry* entry2)
    {
        if(entry1->mIOSize != entry2->mIOSize)
        {
            return entry1->mIOSize > entry2->mIOSize;
        }
        else
        {
            return false;
        }
    }
};

/*organizing disk profile entries of same readratio but different iosize*/
class SubDiskProfile {
public:
    SubDiskProfile() : mRWRatio(0) {}
    SubDiskProfile(ywb_uint32_t rwratio) : mRWRatio(rwratio) {}

    ~SubDiskProfile()
    {
        list<DiskProfileEntry*>::iterator iter;
        DiskProfileEntry* perfentry = NULL;

        iter = mPerfEntries.begin();
        for(; iter != mPerfEntries.end(); )
        {
            perfentry = *iter;
            mPerfEntries.erase(iter++);
            delete perfentry;
            perfentry = NULL;
        }
    }

    inline ywb_uint32_t GetRWRatio()
    {
        return mRWRatio;
    }

    void AddDiskProfileEntry(DiskProfileEntry* entry);

    ywb_status_t GetFirstEntry(DiskProfileEntry** entry);
    ywb_status_t GetNextEntry(DiskProfileEntry** entry);
    /*get first entry reversely*/
    ywb_status_t GetFirstEntryReverse(DiskProfileEntry** entry);

    bool Empty();
    /*sort in decending order*/
    void Sort();

private:
    list<DiskProfileEntry*> mPerfEntries;
    list<DiskProfileEntry*>::iterator mIt;
    ywb_uint32_t mRWRatio;
};

/*disk performance profile*/
class DiskPerfProfile {
public:
//    DiskPerfProfile()
//    {
//        ywb_uint32_t workloadclass = 0;
//
//        for(; workloadclass < Constant::WORKLOAD_CLASSES + 1; workloadclass++)
//        {
//            mRndPerf[workloadclass] = NULL;
//            mSeqPerf[workloadclass] = NULL;
//        }
//    }

    ~DiskPerfProfile()
    {
//        ywb_uint32_t workloadclass = 0;
//
//        for(; workloadclass < Constant::WORKLOAD_CLASSES + 1; workloadclass++)
//        {
//            if(mRndPerf[workloadclass] != NULL)
//            {
//                delete mRndPerf[workloadclass];
//                mRndPerf[workloadclass] = NULL;
//            }
//
//            if(mSeqPerf[workloadclass] != NULL)
//            {
//                delete mSeqPerf[workloadclass];
//                mSeqPerf[workloadclass] = NULL;
//            }
//        }

        map<ywb_uint32_t, SubDiskProfile*>::iterator iter;
        SubDiskProfile* subdiskprofile = NULL;

        if(!mRndPerf.empty())
        {
            iter = mRndPerf.begin();
            while(iter != mRndPerf.end())
            {
                subdiskprofile = iter->second;
                mRndPerf.erase(iter++);
                delete subdiskprofile;
                subdiskprofile = NULL;
            }
        }

        if(!mSeqPerf.empty())
        {
            iter = mSeqPerf.begin();
            while(iter != mSeqPerf.end())
            {
                subdiskprofile = iter->second;
                mSeqPerf.erase(iter++);
                delete subdiskprofile;
                subdiskprofile = NULL;
            }
        }
    }

//    void SetRndPerf(ywb_uint32_t workloadclass, SubDiskProfile* entry);
//    void SetSeqPerf(ywb_uint32_t workloadclass, SubDiskProfile* entry);

    void SetRndPerf(ywb_uint32_t rwratio, SubDiskProfile* entry);
    void SetSeqPerf(ywb_uint32_t rwratio, SubDiskProfile* entry);
    ywb_status_t GetRndPerf(ywb_uint32_t rwratio, SubDiskProfile** entry);
    ywb_status_t GetSeqPerf(ywb_uint32_t rwratio, SubDiskProfile** entry);

    ywb_bool_t RndPerfEmpty();
    ywb_bool_t SeqPerfEmpty();

    ywb_bool_t IsRWRatioFuzzyMatch(ywb_uint32_t readratio1,
            ywb_uint32_t readratio2);
    ywb_bool_t IsIOSizeFuzzyMatch(ywb_uint32_t iosize1, ywb_uint32_t iosize2);

    /*find the first adjacent rwratio less than given rwratio*/
    ywb_status_t GetFirstRWRatioLess(
            ywb_uint32_t rwratio, ywb_uint32_t* val, ywb_bool_t random);
    /*find the first adjacent rwratio greater than given rwratio*/
    ywb_status_t GetFirstRWRatioGreater(
            ywb_uint32_t rwratio, ywb_uint32_t* val, ywb_bool_t random);

    static ywb_uint32_t LinearInterpolation(ywb_uint32_t pointax,
            ywb_uint32_t pointay, ywb_uint32_t pointbx,
            ywb_uint32_t pointby, ywb_uint32_t targetx);
    static ywb_uint32_t WeightedInterpolation(ywb_uint32_t weight1,
            ywb_uint32_t value1, ywb_uint32_t weight2,
            ywb_uint32_t value2);
    /*
     * calculate disk performance under certain workload with
     * special performance model
     **/
    ywb_status_t CalculateDiskPerf(ywb_uint32_t rwratio,
            ywb_uint32_t iosize, ywb_uint32_t *bw,
            ywb_uint32_t *ios, ywb_bool_t random);
    ywb_status_t CalculateDiskPerf(ywb_uint32_t rndseqratio,
            ywb_uint32_t rndrwratio, ywb_uint32_t rndiosize,
            ywb_uint32_t seqrwratio, ywb_uint32_t seqiosize,
            ywb_uint32_t *bw, ywb_uint32_t *ios);

private:
    /*
     * NOTE:
     * rwratio of 0 and 100 should be provided anyway.
     **/
    /*random performance under different workload*/
    map<ywb_uint32_t, SubDiskProfile*> mRndPerf;
    /*sequential performance under different workload*/
    map<ywb_uint32_t, SubDiskProfile*> mSeqPerf;
};

class PerfProfileManager{
public:
    PerfProfileManager(AST* ast) : mAst(ast) {}
    ~PerfProfileManager()
    {
        Destroy();
    }

    inline void GetAST(AST** ast)
    {
        *ast = mAst;
    }

    /*read the performance profile and build disk performance profile in memory*/
    ywb_status_t BuildDiskPerfProfile(const string& filename);
    /*calculate disk performance under certain workload with special performance model*/
    ywb_status_t CalculateDiskPerf(DiskBaseProp& prop, ywb_uint32_t rndseqratio,
                ywb_uint32_t rndrwratio, ywb_uint32_t rndiosize,
                ywb_uint32_t seqrwratio, ywb_uint32_t seqiosize,
                ywb_uint32_t *bw, ywb_uint32_t *iops);

    /*add disk performance profile*/
    ywb_status_t AddDiskPerfProfile(DiskBaseProp& prop,
            DiskPerfProfile* profile);
    /*get performance profile of specified base property*/
    ywb_status_t GetDiskPerfProfile(DiskBaseProp& prop,
            DiskPerfProfile** profile);
    /*destroy performance profile*/
    ywb_status_t Destroy();
    void Reset();

private:
    /*manage all disks' performance profile*/
    map<DiskBaseProp, DiskPerfProfile*, DiskBasePropCmp> mDiskProfile;
    /*reference to AST*/
    AST* mAst;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
