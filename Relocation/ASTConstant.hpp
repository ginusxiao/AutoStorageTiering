#ifndef __AST_CONSTANT_HPP__
#define __AST_CONSTANT_HPP__

#include <string>
#include "common/FsInts.h"
#include "common/FsAssert.h"

using namespace std;

class Constant{
public:
    /*=============================DISK PPROFILE===========================*/
    /*default disk performance profile path*/
    const static string DISK_PROFILE_PATH;

    const static string DISKPROFILE;

    const static string PRODUCTFAMILY;

    const static string PRODUCT;

    const static string DEVICE;

    const static string PERF;

    const static string DDMCLASS;

    const static string RPM;

    const static string RAID;

    const static string WIDTH;

    const static string CAPACITY;

    const static string READRATIO;

    const static string IOSIZE;

    const static string BANDWIDTH;

    const static string IOPS;

    /*
     * disk workload classification, this should be adjust according
     * to the external disk performance profile
     **/
    const static ywb_uint32_t WORKLOAD_CLASSES = 2;

    /*hard coded ssd bandwidth, in measure of MB*/
    const static ywb_uint32_t DEFAULT_SSD_BW = 300;

    /*hard coded ssd iops*/
    const static ywb_uint32_t DEFAULT_SSD_IOPS = 300000;

    /*hard coded ent bandwidth, in measure of MB*/
    const static ywb_uint32_t DEFAULT_ENT_BW = 300;

    /*hard coded ent iops*/
    const static ywb_uint32_t DEFAULT_ENT_IOPS = 300000;

    /*hard coded sata bandwidth, in measure of MB*/
    const static ywb_uint32_t DEFAULT_SATA_BW = 300;

    /*hard coded sata iops*/
    const static ywb_uint32_t DEFAULT_SATA_IOPS = 300000;

    /*how much deviation will be tolerated, in measure of percentage*/
    const static ywb_uint32_t RWRATIO_DIFF = 5;

    /*how much deviation will be tolerated, in measure of percentage*/
    const static ywb_uint32_t IOSIZE_DIFF = 5;

    /*=============================TIME===================================*/
    /*default cycle period, in measure of seconds*/
    const static ywb_uint32_t CYCLE_PERIOD = (3* 10);

    const static ywb_uint32_t SHORT_TERM_DECISION_WINDOW = 6;

    const static ywb_uint32_t LONG_TERM_DECISION_WINDOW = 12;

    /*keep this consistent with CycleManager::CycleType::CT_cnt*/
    const static ywb_uint32_t CYCLE_TYPE_CNT = 2;

    /*keep this consistent with DecisionWindowManager::DECISION_WINDOW_CNT*/
    const static ywb_uint32_t DECISION_WINDOW_CNT = 2;

    /*keep this consistent with DECISION_WINDOW_CNT*/
    const static ywb_uint32_t EMA_CNT = 2;

    /*=========================LOGICAL CONFIG============================*/
    const static ywb_uint32_t TIER_NUM = 3;

    /*=================================IO================================*/
    /*keep this consistent with EMA_CNT + 1*/
    const static ywb_uint32_t IO_STAT_TYPE_CNT = (EMA_CNT + 1);
    /*
     * an OBJ can participate into AST only if it is monitored long enough,
     * in unit of percentage, say it is set to 60, then means at least 60%
     * cycles of a short-term decision window
     **/
    const static ywb_uint32_t EFFECTIVE_MONITOR_CYCLE_PERCENT = 60;

    /*=========================HEAT DISTRIBUTION==========================*/
    const static ywb_uint32_t BUCKET_NUM = 10;

    const static ywb_uint32_t DEFAULT_BUCKET_BOUNDARY = 3;

    /*keep this consistent with IO_STAT_TYPE_CNT*/
    const static ywb_uint32_t HEAT_RANGE_TYPE_CNT = IO_STAT_TYPE_CNT;

    /*=============================ADVICE==================================*/
    const static ywb_uint32_t ADVICE_TYPE_CNT = 4;
    /*maximum number of filter rules*/
    const static ywb_uint32_t MAX_FILTER_NUM = 2;
    /*gain factor for calculating theoretical advice number*/
    const static ywb_uint32_t DEFAULT_ADVICE_NUM_GAIN_FACTOR = 2;

    /*=============================RELOCATION==============================*/
    /*keep this consistent with RelocationTaskType::DATA_RELOCATION_CNT*/
    const static ywb_uint32_t RELOCATION_TASK_CNT = 6;

    /*===========================DISK PERFORMANCE==========================*/
    /*keep this consistent with IO_STAT_TYPE_CNT*/
    const static ywb_uint32_t DISK_PERF_TYPE_CNT = IO_STAT_TYPE_CNT;

    /*==============================PLAN===================================*/
    /*plan direction count*/
    const static ywb_uint32_t PLAN_DIR_CNT = 4;

    /*retry times until a successful plan schedule*/
    const static ywb_uint32_t RESCHEDULE_PLAN_TIMES = 3;

    /*in measure of usec*/
    const static ywb_uint64_t TIMED_DELAY_INTERVAL = 30000000;

    /*===============================ARB===================================*/
    /*default lease time for cross-tier relocation, in unit of cycle*/
    const static ywb_uint32_t DEFAULT_CTR_LEASE_TIME = 2;

    /*default lease time for intra-tier relocation, in unit of cycle*/
    const static ywb_uint32_t DEFAULT_ITR_LEASE_TIME = 4;

    /*default lease time for warm demote relocation, in unit of cycle*/
    const static ywb_uint32_t DEFAULT_WD_LEASE_TIME = 2;

    /*how long a plan can binding to one disk*/
    const static ywb_uint32_t PLAN_BIND_DISK_TIME_LIMIT = 3;

    /*===============================MIG===================================*/
    /*in measure of micro seconds*/
    const static ywb_uint64_t ARBITRATEE_EXPIRE_TIME = (CYCLE_PERIOD * 1000000);


    /*===========================STORAGE POLICY============================*/
    /*how much kind of storage policies are there*/
    const static ywb_uint32_t POLICY_KINDS = 2;

    /*the number of initial placement policy supported*/
    const static ywb_uint32_t INITIAL_PLACEMENT_POLICIES_NUM = 5;

    /*low SWAP out is necessary if single disk's available capacity reaches this limit*/
    const static ywb_uint32_t DISK_AVAIL_CAP_RATIO_THRESHOLD_LOW = 20;

    /*medium SWAP out is necessary if single disk's available capacity reaches this limit*/
    const static ywb_uint32_t DISK_AVAIL_CAP_RATIO_THRESHOLD_MEDIUM = 10;

    /*high SWAP out is necessary if single disk's available capacity reaches this limit*/
    const static ywb_uint32_t DISK_AVAIL_CAP_RATIO_THRESHOLD_HIGH = 3;

    /*SWAP out is necessary if tier wise disks' available capacity reaches this limit*/
    const static ywb_uint32_t TIER_AVAIL_CAP_RATIO_THRESHOLD = 25;

    const static ywb_uint32_t MINIMUM_RESERVED_TICKET_AT_ONCE = 100;

    /*when remaining ticket number is less than this value, more will be applied*/
    const static ywb_uint32_t RESERVED_TICKET_WATER_MARK_CORDON = 20;

    /*reserved space ratio for hot swap, in measure of OBJ num*/
    const static ywb_uint32_t RESERVED_CAP_FOR_HOT_SWAP = 32;

    /*reserved space ratio for cold swap, in measure of OBJ num*/
    const static ywb_uint32_t RESERVED_CAP_FOR_COLD_SWAP = 64;

    /*
     * maximum number of OBJs can be removed before updating disk selector again
     * in measure of percentage(5‰)
     * */
    const static ywb_uint32_t REMOVED_OBJ_THRESHOLD_BEFORE_UPDATE = 5;

    /*===============================HUB===================================*/
    /*
     * these OBJ IOs will be responded in batch, but each in its own
     * response buffer individually
     **/
    const static ywb_uint32_t OBJ_IO_RESPONSE_BATCH_NUM = 16;

    /*in measure of Byte, 64KB*/
    const static ywb_uint32_t SINGLE_OBJ_IO_RESPONSE_SIZE = (64 * 1024);

    /*=============================MISC===================================*/
    /*maximum size of path string*/
    const static ywb_uint32_t PATH_SIZE_MAX = 128;

    /*default configuration file path*/
    const static string CONFIG_PATH;

    /*default iosize, in unit of KB*/
    const static ywb_uint32_t DEFAULT_IOSIZE = 4;

    /*default OBJ size, in unit of MB*/
    const static ywb_uint32_t DEFAULT_OBJ_SIZE = 64;

    /*=============================TEST===================================*/
    const static string LOG_PATH;

    const static ywb_uint64_t DISK_ID = 1;
    const static ywb_uint32_t OSS_ID = 1;
    const static ywb_uint32_t SUBPOOL_ID = 1;
    const static ywb_uint32_t POOL_ID = 1;

    const static ywb_uint32_t DEFAULT_RPM = 10000;
    const static ywb_uint32_t DEFAULT_RAID_WIDTH = 1;
    /*in measure of OBJ num*/
    const static ywb_uint32_t DEFAULT_DISK_CAP = 16384;
    const static ywb_uint32_t DEFAULT_CHUNK_INDEX = 1;
    const static ywb_uint32_t DEFAULT_CHUNK_ID = 1;
    const static ywb_uint32_t DEFAULT_VERSION = 1;
    const static ywb_uint64_t DEFAULT_INODE = 1;
    /*in measure of OBJ num*/
    const static ywb_uint32_t DEFAULT_OBJ_CAP = 1;

    const static ywb_uint32_t META_RT_READ = 3;
    const static ywb_uint32_t META_RT_WRITE = 2;

    const static ywb_uint32_t DEFAULT_SUBPOOL_NUM = 5;
    const static ywb_uint32_t DEFAULT_DISK_NUM_PER_SUBPOOL = 3;
    const static ywb_uint32_t DEFAULT_DISK_NUM_PER_TIER = 2;
    const static ywb_uint32_t DEFAULT_OBJ_NUM_PER_DISK = 32;
    /*OSSSimulator should simulator large amount of OBJs*/
    const static ywb_uint32_t DEFAULT_OBJ_NUM_PER_DISK_FOR_SIMULATOR = 16384;

    /*used for generating random seed*/
    const static ywb_uint32_t RAND_SEED_MOD = 100;
    const static ywb_uint32_t RAND_SEED_INCREMENTAL = 100;
    const static ywb_uint32_t RAND_NUM_MOD = 10;

    const static ywb_uint32_t DEFAULT_ADVISE_NUM = 6;

    /*used for test plan generation*/
    const static string EMPTY_STR;
    const static string PLAN_FILE_PATH;

    /*used for AST integration test with OSS simulated*/
//#ifdef AST_INTEGRATION_WITH_OSS_SIMULATOR
    const static ywb_uint64_t MIGRATION_DONE_CHECK_INTERVAL_SEC = 5;
    const static ywb_uint64_t MIGRATION_DONE_CHECK_INTERVAL_USEC = 0;
    const static ywb_uint32_t NORMAL_MIGRATION_RATIO = 90;
    const static ywb_uint32_t TIMED_OUT_MIGRATION_RATIO = 8;
    const static ywb_uint32_t LOST_MIGRATION_RATIO = 2;
//#endif

    inline static void ConstantCheck(ywb_uint32_t constant, ywb_uint32_t val)
    {
        YWB_ASSERT(constant == val);
    }

    Constant();
};

#endif

/* vim:set ts=4 sw=4 expandtab */
