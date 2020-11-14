/*
 * ASTDiskUltimatePerfCache.hpp
 *
 *  Created on: 2016年5月24日
 *      Author: Administrator
 */

#ifndef SRC_OSS_AST_ASTDISKULTIMATEPERFCACHE_HPP_
#define SRC_OSS_AST_ASTDISKULTIMATEPERFCACHE_HPP_

#include <map>
#include "AST/ASTLogicalConfig.hpp"


using namespace std;

/*rule for calculating ultimate performance for disk*/
class DiskUltimatePerfRule
{
public:
    DiskUltimatePerfRule(ywb_uint32_t rwratio,
            ywb_uint32_t iosize, ywb_bool_t random)
    {
        mRWRatio = rwratio;
        mIOSize = iosize;
        mRandom = random;
    }

    virtual ~DiskUltimatePerfRule()
    {

    }

    DiskUltimatePerfRule(const DiskUltimatePerfRule& rule)
    {
        mRWRatio = rule.mRWRatio;
        mIOSize = rule.mIOSize;
        mRandom = rule.mRandom;
    }

    DiskUltimatePerfRule& operator= (const DiskUltimatePerfRule& rule)
    {
        if(this == &rule)
        {
            return *this;
        }

        mRWRatio = rule.mRWRatio;
        mIOSize = rule.mIOSize;
        mRandom = rule.mRandom;

        return *this;
    }

    inline ywb_uint32_t GetRWRatio()
    {
        return mRWRatio;
    }

    inline ywb_uint32_t GetIOSize()
    {
        return mIOSize;
    }

    inline ywb_bool_t GetRndom()
    {
        return mRandom;
    }

    inline void SetRWRatio(ywb_uint32_t ratio)
    {
        mRWRatio = ratio;
    }

    inline void SetIOSize(ywb_uint32_t iosize)
    {
        mIOSize = iosize;
    }

    inline void SetRandom(ywb_bool_t rand)
    {
        mRandom = rand;
    }

private:
    /*RWRatio used for calculating ultimate performance*/
    ywb_uint32_t mRWRatio;
    /*IOSize used for calculating ultimate performance*/
    ywb_uint32_t mIOSize;
    /*use random IO or sequential IO*/
    ywb_bool_t mRandom;
};

/*
 * manage all kinds of disks and their ultimate performance,
 * it acts as cache for ultimate performance of all kinds of
 * disks. Mainly used by AdviceManager.
 *
 * If fail to search from the cache for any kind of
 * disk, then its ultimate performance will be calculated
 * and then added into cache, subsequent search will hit
 * the cache which avoid the re-calculation.
 *
 * for target disk, ultimate performance will be calculated
 * by default under: 100% readratio, maximum iosize, with
 * bandwidth considered only
 *
 * for source disk, ultimate performance will be calculated
 * by default under: 0% readratio, maximum iosize, with
 * banwidth considered only
 * */
class DiskUltimatePerfCache
{
public:
    enum {
        /*F_bandwidth and F_iops are exclusive, either of them should be specified*/
        F_bandwidth = 0x0001,
        F_iops = 0x0002,
        /*F_read && F_write are exclusive, either of them should be specified*/
        F_read = 0x0004,
        F_write = 0x0008,
    };

    DiskUltimatePerfCache(AST* ast) :
            mReadBandwidthRule(100, 64, ywb_false),
            mWriteBandwidthRule(0, 64, ywb_false),
            mReadIOPSRule(100, 64, ywb_false),
            mWriteIOPSRule(0, 64, ywb_false),
            mAST(ast)
    {
        YWB_ASSERT(mAST != NULL);
    }

    DiskUltimatePerfCache(AST* ast,
            DiskUltimatePerfRule& readbwrule,
            DiskUltimatePerfRule& writebwrule,
            DiskUltimatePerfRule& readiopsrule,
            DiskUltimatePerfRule& writeiopsrule) :
                mReadBandwidthRule(readbwrule),
                mWriteBandwidthRule(writebwrule),
                mReadIOPSRule(readiopsrule),
                mWriteIOPSRule(writeiopsrule),
                mAST(ast)
    {
        YWB_ASSERT(mAST != NULL);
    }

    virtual ~DiskUltimatePerfCache()
    {
        mUltimateReadBandwidth.clear();
        mUltimateWriteBandwidth.clear();
        mUltimateReadIOPS.clear();
        mUltimateWriteIOPS.clear();

        mAST = NULL;
    }

    inline AST* GetAST()
    {
        return mAST;
    }

    /*@hint: combination of read/write and bandwidth/iops*/
    ywb_uint64_t GetUltimatePerf(DiskBaseProp prop, ywb_uint32_t hint);
    /*for test ONLY*/
    ywb_status_t SetUltimatePerfForTest(DiskBaseProp& diskprop,
            ywb_uint64_t readbw, ywb_uint64_t writebw,
            ywb_uint64_t readios, ywb_uint64_t writeios);
    void Reset();

private:
    /*rule for ultimate read bandwidth*/
    DiskUltimatePerfRule mReadBandwidthRule;
    /*rule for ultimate write bandwidth*/
    DiskUltimatePerfRule mWriteBandwidthRule;
    /*rule for ultimate read IOPS*/
    DiskUltimatePerfRule mReadIOPSRule;
    /*rule for ultimate write IOPS*/
    DiskUltimatePerfRule mWriteIOPSRule;

    /*ultimate bandwidth(in measure of MB) for read of all kind of disks*/
    map<DiskBaseProp, ywb_uint64_t, DiskBasePropCmp> mUltimateReadBandwidth;
    /*ultimate bandwidth(in measure of MB) for write of all kind of disks*/
    map<DiskBaseProp, ywb_uint64_t, DiskBasePropCmp> mUltimateWriteBandwidth;
    /*ultimate iops for read of all kind of disks*/
    map<DiskBaseProp, ywb_uint64_t, DiskBasePropCmp> mUltimateReadIOPS;
    /*ultimate iops for write of all kind of disks*/
    map<DiskBaseProp, ywb_uint64_t, DiskBasePropCmp> mUltimateWriteIOPS;

    /*back reference to AST*/
    AST* mAST;
};

#endif /* SRC_OSS_AST_ASTDISKULTIMATEPERFCACHE_HPP_ */
