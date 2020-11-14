#include <algorithm>
#include <stdlib.h>
#include "AST/AST.hpp"
#include "AST/ASTLogicalConfig.hpp"
#include "AST/ASTHeatDistribution.hpp"

ywb_int32_t
HeatCmp::LongTermEMARndIOsCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t rndstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);

    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_long_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_long_term_ema, &stat2);

    rndstat1 = stat1->GetRndReadIOs() + stat1->GetRndWriteIOs();
    rndstat2 = stat2->GetRndReadIOs() + stat2->GetRndWriteIOs();

    if(rndstat1 > rndstat2)
    {
        return 1;
    }
    else if(rndstat1 == rndstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::LongTermEMARndBWCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t rndstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_long_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_long_term_ema, &stat2);

    rndstat1 = stat1->GetRndReadBW() + stat1->GetRndWriteBW();
    rndstat2 = stat2->GetRndReadBW() + stat2->GetRndWriteBW();

    if(rndstat1 > rndstat2)
    {
        return 1;
    }
    else if(rndstat1 == rndstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::LongTermEMARndRTCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t rndstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_long_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_long_term_ema, &stat2);

    rndstat1 = stat1->GetRndTotalRT();
    rndstat2 = stat2->GetRndTotalRT();

    if(rndstat1 > rndstat2)
    {
        return 1;
    }
    else if(rndstat1 == rndstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::ShortTermEMARndIOsCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t rndstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_short_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_short_term_ema, &stat2);

    rndstat1 = stat1->GetRndReadIOs() + stat1->GetRndWriteIOs();
    rndstat2 = stat2->GetRndReadIOs() + stat2->GetRndWriteIOs();

    if(rndstat1 > rndstat2)
    {
        return 1;
    }
    else if(rndstat1 == rndstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::ShortTermEMARndBWCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t rndstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_short_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_short_term_ema, &stat2);

    rndstat1 = stat1->GetRndReadBW() + stat1->GetRndWriteBW();
    rndstat2 = stat2->GetRndReadBW() + stat2->GetRndWriteBW();

    if(rndstat1 > rndstat2)
    {
        return 1;
    }
    else if(rndstat1 == rndstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::ShortTermEMARndRTCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t rndstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_short_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_short_term_ema, &stat2);

    rndstat1 = stat1->GetRndTotalRT();
    rndstat2 = stat2->GetRndTotalRT();

    if(rndstat1 > rndstat2)
    {
        return 1;
    }
    else if(rndstat1 == rndstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::RawRndIOsCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t rndstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_raw, &stat1);
    objio2->GetIOStat(IOStat::IOST_raw, &stat2);

    rndstat1 = stat1->GetRndReadIOs() + stat1->GetRndWriteIOs();
    rndstat2 = stat2->GetRndReadIOs() + stat2->GetRndWriteIOs();

    if(rndstat1 > rndstat2)
    {
        return 1;
    }
    else if(rndstat1 == rndstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::RawRndBWCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t rndstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_raw, &stat1);
    objio2->GetIOStat(IOStat::IOST_raw, &stat2);

    rndstat1 = stat1->GetRndReadBW() + stat1->GetRndWriteBW();
    rndstat2 = stat2->GetRndReadBW() + stat2->GetRndWriteBW();

    if(rndstat1 > rndstat2)
    {
        return 1;
    }
    else if(rndstat1 == rndstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::RawRndRTCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t rndstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_raw, &stat1);
    objio2->GetIOStat(IOStat::IOST_raw, &stat2);

    rndstat1 = stat1->GetRndTotalRT();
    rndstat2 = stat2->GetRndTotalRT();

    if(rndstat1 > rndstat2)
    {
        return 1;
    }
    else if(rndstat1 == rndstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::LongTermEMASeqIOsCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t seqstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);

    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_long_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_long_term_ema, &stat2);

    seqstat1 = stat1->GetSeqReadIOs() + stat1->GetSeqWriteIOs();
    seqstat2 = stat2->GetSeqReadIOs() + stat2->GetSeqWriteIOs();

    if(seqstat1 > seqstat2)
    {
        return 1;
    }
    else if(seqstat1 == seqstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::LongTermEMASeqBWCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t seqstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_long_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_long_term_ema, &stat2);

    seqstat1 = stat1->GetSeqReadBW() + stat1->GetSeqWriteBW();
    seqstat2 = stat2->GetSeqReadBW() + stat2->GetSeqWriteBW();

    if(seqstat1 > seqstat2)
    {
        return 1;
    }
    else if(seqstat1 == seqstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::LongTermEMASeqRTCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t seqstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_long_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_long_term_ema, &stat2);

    seqstat1 = stat1->GetSeqTotalRT();
    seqstat2 = stat2->GetSeqTotalRT();

    if(seqstat1 > seqstat2)
    {
        return 1;
    }
    else if(seqstat1 == seqstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::ShortTermEMASeqIOsCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t seqstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_short_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_short_term_ema, &stat2);

    seqstat1 = stat1->GetSeqReadIOs() + stat1->GetSeqWriteIOs();
    seqstat2 = stat2->GetSeqReadIOs() + stat2->GetSeqWriteIOs();

    if(seqstat1 > seqstat2)
    {
        return 1;
    }
    else if(seqstat1 == seqstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::ShortTermEMASeqBWCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t seqstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_short_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_short_term_ema, &stat2);

    seqstat1 = stat1->GetSeqReadBW() + stat1->GetSeqWriteBW();
    seqstat2 = stat2->GetSeqReadBW() + stat2->GetSeqWriteBW();

    if(seqstat1 > seqstat2)
    {
        return 1;
    }
    else if(seqstat1 == seqstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::ShortTermEMASeqRTCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t seqstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_short_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_short_term_ema, &stat2);

    seqstat1 = stat1->GetSeqTotalRT();
    seqstat2 = stat2->GetSeqTotalRT();

    if(seqstat1 > seqstat2)
    {
        return 1;
    }
    else if(seqstat1 == seqstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::RawSeqIOsCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t seqstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_raw, &stat1);
    objio2->GetIOStat(IOStat::IOST_raw, &stat2);

    seqstat1 = stat1->GetSeqReadIOs() + stat1->GetSeqWriteIOs();
    seqstat2 = stat2->GetSeqReadIOs() + stat2->GetSeqWriteIOs();

    if(seqstat1 > seqstat2)
    {
        return 1;
    }
    else if(seqstat1 == seqstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::RawSeqBWCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t seqstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_raw, &stat1);
    objio2->GetIOStat(IOStat::IOST_raw, &stat2);

    seqstat1 = stat1->GetSeqReadBW() + stat1->GetSeqWriteBW();
    seqstat2 = stat2->GetSeqReadBW() + stat2->GetSeqWriteBW();

    if(seqstat1 > seqstat2)
    {
        return 1;
    }
    else if(seqstat1 == seqstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::RawSeqRTCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t seqstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_raw, &stat1);
    objio2->GetIOStat(IOStat::IOST_raw, &stat2);

    seqstat1 = stat1->GetSeqTotalRT();
    seqstat2 = stat2->GetSeqTotalRT();

    if(seqstat1 > seqstat2)
    {
        return 1;
    }
    else if(seqstat1 == seqstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::LongTermEMAAccumIOsCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t rndstat2 = 0;
    ywb_uint32_t seqstat2 = 0;
    ywb_uint32_t accumstat1 = 0;
    ywb_uint32_t accumstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_long_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_long_term_ema, &stat2);

    rndstat1 = stat1->GetRndReadIOs() + stat1->GetRndWriteIOs();
    rndstat2 = stat2->GetRndReadIOs() + stat2->GetRndWriteIOs();
    seqstat1 = stat1->GetSeqReadIOs() + stat1->GetSeqWriteIOs();
    seqstat2 = stat2->GetSeqReadIOs() + stat2->GetSeqWriteIOs();
    accumstat1 = rndstat1 + seqstat1;
    accumstat2 = rndstat2 + seqstat2;

    if(accumstat1 > accumstat2)
    {
        return 1;
    }
    else if(accumstat1 == accumstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::LongTermEMAAccumBWCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t rndstat2 = 0;
    ywb_uint32_t seqstat2 = 0;
    ywb_uint32_t accumstat1 = 0;
    ywb_uint32_t accumstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_long_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_long_term_ema, &stat2);

    rndstat1 = stat1->GetRndReadBW() + stat1->GetRndWriteBW();
    rndstat2 = stat2->GetRndReadBW() + stat2->GetRndWriteBW();
    seqstat1 = stat1->GetSeqReadBW() + stat1->GetSeqWriteBW();
    seqstat2 = stat2->GetSeqReadBW() + stat2->GetSeqWriteBW();
    accumstat1 = rndstat1 + seqstat1;
    accumstat2 = rndstat2 + seqstat2;

    if(accumstat1 > accumstat2)
    {
        return 1;
    }
    else if(accumstat1 == accumstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::LongTermEMAAccumRTCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t rndstat2 = 0;
    ywb_uint32_t seqstat2 = 0;
    ywb_uint32_t accumstat1 = 0;
    ywb_uint32_t accumstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_long_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_long_term_ema, &stat2);

    rndstat1 = stat1->GetRndTotalRT();
    rndstat2 = stat2->GetRndTotalRT();
    seqstat1 = stat1->GetSeqTotalRT();
    seqstat2 = stat2->GetSeqTotalRT();
    accumstat1 = rndstat1 + seqstat1;
    accumstat2 = rndstat2 + seqstat2;

    if(accumstat1 > accumstat2)
    {
        return 1;
    }
    else if(accumstat1 == accumstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::ShortTermEMAAccumIOsCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t rndstat2 = 0;
    ywb_uint32_t seqstat2 = 0;
    ywb_uint32_t accumstat1 = 0;
    ywb_uint32_t accumstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_short_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_short_term_ema, &stat2);

    rndstat1 = stat1->GetRndReadIOs() + stat1->GetRndWriteIOs();
    rndstat2 = stat2->GetRndReadIOs() + stat2->GetRndWriteIOs();
    seqstat1 = stat1->GetSeqReadIOs() + stat1->GetSeqWriteIOs();
    seqstat2 = stat2->GetSeqReadIOs() + stat2->GetSeqWriteIOs();
    accumstat1 = rndstat1 + seqstat1;
    accumstat2 = rndstat2 + seqstat2;

    if(accumstat1 > accumstat2)
    {
        return 1;
    }
    else if(accumstat1 == accumstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::ShortTermEMAAccumBWCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t rndstat2 = 0;
    ywb_uint32_t seqstat2 = 0;
    ywb_uint32_t accumstat1 = 0;
    ywb_uint32_t accumstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_short_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_short_term_ema, &stat2);

    rndstat1 = stat1->GetRndReadBW() + stat1->GetRndWriteBW();
    rndstat2 = stat2->GetRndReadBW() + stat2->GetRndWriteBW();
    seqstat1 = stat1->GetSeqReadBW() + stat1->GetSeqWriteBW();
    seqstat2 = stat2->GetSeqReadBW() + stat2->GetSeqWriteBW();
    accumstat1 = rndstat1 + seqstat1;
    accumstat2 = rndstat2 + seqstat2;

    if(accumstat1 > accumstat2)
    {
        return 1;
    }
    else if(accumstat1 == accumstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::ShortTermEMAAccumRTCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t rndstat2 = 0;
    ywb_uint32_t seqstat2 = 0;
    ywb_uint32_t accumstat1 = 0;
    ywb_uint32_t accumstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_short_term_ema, &stat1);
    objio2->GetIOStat(IOStat::IOST_short_term_ema, &stat2);

    rndstat1 = stat1->GetRndTotalRT();
    rndstat2 = stat2->GetRndTotalRT();
    seqstat1 = stat1->GetSeqTotalRT();
    seqstat2 = stat2->GetSeqTotalRT();
    accumstat1 = rndstat1 + seqstat1;
    accumstat2 = rndstat2 + seqstat2;

    if(accumstat1 > accumstat2)
    {
        return 1;
    }
    else if(accumstat1 == accumstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::RawAccumIOsCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t rndstat2 = 0;
    ywb_uint32_t seqstat2 = 0;
    ywb_uint32_t accumstat1 = 0;
    ywb_uint32_t accumstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_raw, &stat1);
    objio2->GetIOStat(IOStat::IOST_raw, &stat2);

    rndstat1 = stat1->GetRndReadIOs() + stat1->GetRndWriteIOs();
    rndstat2 = stat2->GetRndReadIOs() + stat2->GetRndWriteIOs();
    seqstat1 = stat1->GetSeqReadIOs() + stat1->GetSeqWriteIOs();
    seqstat2 = stat2->GetSeqReadIOs() + stat2->GetSeqWriteIOs();
    accumstat1 = rndstat1 + seqstat1;
    accumstat2 = rndstat2 + seqstat2;

    if(accumstat1 > accumstat2)
    {
        return 1;
    }
    else if(accumstat1 == accumstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::RawAccumBWCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t rndstat2 = 0;
    ywb_uint32_t seqstat2 = 0;
    ywb_uint32_t accumstat1 = 0;
    ywb_uint32_t accumstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_raw, &stat1);
    objio2->GetIOStat(IOStat::IOST_raw, &stat2);

    rndstat1 = stat1->GetRndReadBW() + stat1->GetRndWriteBW();
    rndstat2 = stat2->GetRndReadBW() + stat2->GetRndWriteBW();
    seqstat1 = stat1->GetSeqReadBW() + stat1->GetSeqWriteBW();
    seqstat2 = stat2->GetSeqReadBW() + stat2->GetSeqWriteBW();
    accumstat1 = rndstat1 + seqstat1;
    accumstat2 = rndstat2 + seqstat2;

    if(accumstat1 > accumstat2)
    {
        return 1;
    }
    else if(accumstat1 == accumstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

ywb_int32_t
HeatCmp::RawAccumRTCmp(const OBJVal* obja, const OBJVal* objb)
{
    OBJVal* obj1 = NULL;
    OBJVal* obj2 = NULL;
    OBJIO* objio1 = NULL;
    OBJIO* objio2 = NULL;
    ChunkIOStat *stat1 = NULL;
    ChunkIOStat *stat2 = NULL;
    ywb_uint32_t rndstat1 = 0;
    ywb_uint32_t seqstat1 = 0;
    ywb_uint32_t rndstat2 = 0;
    ywb_uint32_t seqstat2 = 0;
    ywb_uint32_t accumstat1 = 0;
    ywb_uint32_t accumstat2 = 0;

    obj1 = const_cast<OBJVal*>(obja);
    obj2 = const_cast<OBJVal*>(objb);
    obj1->GetOBJIO(&objio1);
    obj2->GetOBJIO(&objio2);
    YWB_ASSERT(objio1);
    YWB_ASSERT(objio2);

    objio1->GetIOStat(IOStat::IOST_raw, &stat1);
    objio2->GetIOStat(IOStat::IOST_raw, &stat2);

    rndstat1 = stat1->GetRndTotalRT();
    rndstat2 = stat2->GetRndTotalRT();
    seqstat1 = stat1->GetSeqTotalRT();
    seqstat2 = stat2->GetSeqTotalRT();
    accumstat1 = rndstat1 + seqstat1;
    accumstat2 = rndstat2 + seqstat2;

    if(accumstat1 > accumstat2)
    {
        return 1;
    }
    else if(accumstat1 == accumstat2)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

bool
HeatCmp::GreaterByLongTermEMARndIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByLongTermEMARndBW(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByLongTermEMARndRT(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByShortTermEMARndIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == ShortTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByShortTermEMARndBW(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == ShortTermEMARndBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByShortTermEMARndRT(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == ShortTermEMARndRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByRawRndIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == RawRndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByRawRndBW(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == RawRndBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByRawRndRT(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == RawRndRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByLongTermEMASeqIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMASeqIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByLongTermEMASeqBW(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMASeqBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByLongTermEMASeqRT(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMASeqRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByShortTermEMASeqIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == ShortTermEMASeqIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByShortTermEMASeqBW(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == ShortTermEMASeqBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByShortTermEMASeqRT(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == ShortTermEMASeqRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByRawSeqIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == RawSeqIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByRawSeqBW(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == RawSeqBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByRawSeqRT(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == RawSeqRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByLongTermEMAAccumIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMAAccumIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByLongTermEMAAccumBW(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByLongTermEMAAccumRT(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByShortTermEMAAccumIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByShortTermEMAAccumBW(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByShortTermEMAAccumRT(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByRawAccumIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByRawAccumBW(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::GreaterByRawAccumRT(const OBJVal* obja, const OBJVal* objb)
{
    if(1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByLongTermEMARndIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == LongTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByLongTermEMARndBW(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == LongTermEMARndBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByLongTermEMARndRT(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == LongTermEMARndRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByShortTermEMARndIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == ShortTermEMARndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByShortTermEMARndBW(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == ShortTermEMARndBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByShortTermEMARndRT(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == ShortTermEMARndRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByRawRndIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawRndIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByRawRndBW(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawRndBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByRawRndRT(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawRndRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}


bool
HeatCmp::LessByLongTermEMASeqIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == LongTermEMASeqIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByLongTermEMASeqBW(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == LongTermEMASeqBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByLongTermEMASeqRT(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == LongTermEMASeqRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByShortTermEMASeqIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == ShortTermEMASeqIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByShortTermEMASeqBW(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == ShortTermEMASeqBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByShortTermEMASeqRT(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawSeqIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByRawSeqIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawSeqIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByRawSeqBW(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawSeqBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByRawSeqRT(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawSeqRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByLongTermEMAAccumIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == LongTermEMAAccumIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByLongTermEMAAccumBW(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == LongTermEMAAccumBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByLongTermEMAAccumRT(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == LongTermEMAAccumRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByShortTermEMAAccumIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == ShortTermEMAAccumIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByShortTermEMAAccumBW(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == ShortTermEMAAccumBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByShortTermEMAAccumRT(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == ShortTermEMAAccumRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByRawAccumIOs(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawAccumIOsCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByRawAccumBW(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawAccumBWCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool
HeatCmp::LessByRawAccumRT(const OBJVal* obja, const OBJVal* objb)
{
    if(-1 == RawAccumRTCmp(obja, objb))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void HeatRange::Calculate(ywb_uint32_t objnum)
{
    /*heat range is not updated*/
    if((YWB_UINT32_MAX == mMin) && (0 == mMax))
    {
        return;
    }
    else
    {
        YWB_ASSERT(mMin <= mMax);
        YWB_ASSERT(objnum > 0);
        if(mMax == 0)
        {
            mBucketCap = 0;
            mAvg = 0;
            mMinPortionBoundary = 0;
            mMaxPortionBoundary = 0;
        }
        else
        {
            mBucketCap = (mMax - mMin) / (Constant::BUCKET_NUM);
            mAvg = mSum / objnum;
            mMinPortionBoundary =
                    (((mMin + mAvg) / 2) > ((3 * mMin) / 4 + mMax / 4)) ?
                    ((mMin + mAvg) / 2) : ((3 * mMin) / 4 + mMax / 4);
            mMaxPortionBoundary =
                    (((mMax + mAvg) / 2) > (mMin / 4 + (3 * mMax) / 4)) ?
                    ((mMax + mAvg) / 2) : (mMin / 4 + (3 * mMax) / 4);
        }
    }
}

void HeatRange::Dump(ostringstream* ostr)
{
    *ostr << "[min: " << mMin << ", max: " << mMax
            << ", cap: " << mBucketCap << "]\n";
}

void HeatRangeSummary::Dump(ostringstream* ostr)
{
    *ostr << "Heat range summary:\n"
            << "Heat range type: " << mHeatRangeType
            << "[IOST_raw: 0, IOST_short_term_ema: 1,"
            << " IOST_long_term_ema: 2]\n";
    *ostr << "Heat range for rnd read IOs: ";
    mRndReadCnt.Dump(ostr);
    *ostr << "Heat range for rnd write IOs: ";
    mRndWriteCnt.Dump(ostr);
    *ostr << "Heat range for rnd accumulative IOs: ";
    mRndAccumIOs.Dump(ostr);
    *ostr << "Heat range for rnd accumulative RESP: ";
    mRndAccumRT.Dump(ostr);
    *ostr << "Heat range for rnd accumulative BW: ";
    mRndAccumBW.Dump(ostr);
    *ostr << "Heat range for seq read IOs: ";
    mSeqReadCnt.Dump(ostr);
    *ostr << "Heat range for seq write IOs: ";
    mSeqWriteCnt.Dump(ostr);
    *ostr << "Heat range for seq accumulative IOs: ";
    mSeqAccumIOs.Dump(ostr);
    *ostr << "Heat range for seq accumulative RESP: ";
    mSeqAccumRT.Dump(ostr);
    *ostr << "Heat range for seq accumulative BW: ";
    mSeqAccumBW.Dump(ostr);
    *ostr << "Heat range for rnd/seq accumulative IOs: ";
    mRndSeqAccumIOs.Dump(ostr);
    *ostr << "Heat range for rnd/seq accumulative BW: ";
    mRndSeqAccumBW.Dump(ostr);
    *ostr << "Heat range for rnd/seq accumulative RESP: ";
    mRndSeqAccumRT.Dump(ostr);
}

HeatBucket::HeatBucket(HeatDistribution* heatdistribution) :
        mPartitionBase(PartitionBase::PB_cnt),
        mSortBase(SortBase::SB_cnt),
        mSortedAboveBoundary(0),
        mSortedBelowBoundary(0),
        mBucketBoundary(Constant::DEFAULT_BUCKET_BOUNDARY),
        mOBJsAboveBoundary(0),
        mOBJsBelowBoundary(0),
        mLastSortedBucketAbove(Constant::BUCKET_NUM),
        mLastSortedBucketBelow(Constant::BUCKET_NUM),
        mCurHotBucket(Constant::BUCKET_NUM - 1),
        mCurColdBucket(0),
        mHeatDistribution(heatdistribution)
{
    SubPoolHeatDistribution* subpoolheat = NULL;

    YWB_ASSERT(heatdistribution);
    heatdistribution->GetSubPoolHeat(&subpoolheat);
    YWB_ASSERT(subpoolheat);
    subpoolheat->GetHeatDistributuinManager(&mHeatManager);
    YWB_ASSERT(mHeatManager);
}

HeatBucket::HeatBucket(ywb_uint32_t partitionbase, ywb_uint32_t sortbase,
        HeatDistribution* heatdistribution) :
        mPartitionBase(partitionbase), mSortBase(sortbase),
        mSortedAboveBoundary(0),
        mSortedBelowBoundary(0),
        mBucketBoundary(Constant::DEFAULT_BUCKET_BOUNDARY),
        mOBJsAboveBoundary(0),
        mOBJsBelowBoundary(0),
        mLastSortedBucketAbove(Constant::BUCKET_NUM),
        mLastSortedBucketBelow(Constant::BUCKET_NUM),
        mCurHotBucket(Constant::BUCKET_NUM - 1),
        mCurColdBucket(0),
        mHeatDistribution(heatdistribution)
{
    SubPoolHeatDistribution* subpoolheat = NULL;

    YWB_ASSERT(heatdistribution);
    heatdistribution->GetSubPoolHeat(&subpoolheat);
    YWB_ASSERT(subpoolheat);
    subpoolheat->GetHeatDistributuinManager(&mHeatManager);
    YWB_ASSERT(mHeatManager);
}

HeatBucket::~HeatBucket()
{
    Destroy();
}

//ywb_status_t
//HeatBucket::RegisterGreater(ywb_uint32_t partitionbase, HeatCmp::Greater greater)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//
//    mGreaters.insert(make_pair(partitionbase, greater));
//    return ret;
//}
//
//ywb_status_t
//HeatBucket::RegisterLess(ywb_uint32_t partitionbase, HeatCmp::Less less)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//
//    mLesses.insert(make_pair(partitionbase, less));
//    return ret;
//}
//
//ywb_status_t
//HeatBucket::GetGreater(ywb_uint32_t sortbase, HeatCmp::Greater* greater)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    map<ywb_uint32_t, HeatCmp::Greater>::iterator iter;
//
//    iter = mGreaters.find(sortbase);
//    if(iter != mGreaters.end())
//    {
//        *greater = iter->second;
//    }
//    else
//    {
//        *greater = NULL;
//        ret = YFS_ENOENT;
//    }
//
//    return ret;
//}
//
//ywb_status_t
//HeatBucket::GetLess(ywb_uint32_t sortbase, HeatCmp::Less* less)
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    map<ywb_uint32_t, HeatCmp::Less>::iterator iter;
//
//    iter = mLesses.find(sortbase);
//    if(iter != mLesses.end())
//    {
//        *less = iter->second;
//    }
//    else
//    {
//        *less = NULL;
//        ret = YFS_ENOENT;
//    }
//
//    return ret;
//}
//
//ywb_status_t
//HeatBucket::Initialize()
//{
//    ywb_status_t ret = YWB_SUCCESS;
//    ywb_uint32_t sortbase = 0;
//
//    /*random*/
//    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
//            SortBase::SB_random | SortBase::SB_iops);
//    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMARndIOs);
//    RegisterLess(sortbase, HeatCmp::LessByLongTermEMARndIOs);
//
//    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
//            SortBase::SB_random | SortBase::SB_bw);
//    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMARndBW);
//    RegisterLess(sortbase, HeatCmp::LessByLongTermEMARndBW);
//
//    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
//            SortBase::SB_random | SortBase::SB_rt);
//    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMARndRT);
//    RegisterLess(sortbase, HeatCmp::LessByLongTermEMARndRT);
//
//    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
//            SortBase::SB_random | SortBase::SB_iops);
//    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMARndIOs);
//    RegisterLess(sortbase, HeatCmp::LessByShortTermEMARndIOs);
//
//    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
//            SortBase::SB_random | SortBase::SB_bw);
//    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMARndBW);
//    RegisterLess(sortbase, HeatCmp::LessByShortTermEMARndBW);
//
//    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
//            SortBase::SB_random | SortBase::SB_rt);
//    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMARndRT);
//    RegisterLess(sortbase, HeatCmp::LessByShortTermEMARndRT);
//
//    sortbase = (SortBase::SB_raw | SortBase::SB_random |
//            SortBase::SB_iops);
//    RegisterGreater(sortbase, HeatCmp::GreaterByRawRndIOs);
//    RegisterLess(sortbase, HeatCmp::LessByRawRndIOs);
//
//    sortbase = (SortBase::SB_raw | SortBase::SB_random |
//            SortBase::SB_bw);
//    RegisterGreater(sortbase, HeatCmp::GreaterByRawRndBW);
//    RegisterLess(sortbase, HeatCmp::LessByRawRndBW);
//
//    sortbase = (SortBase::SB_raw | SortBase::SB_random |
//            SortBase::SB_rt);
//    RegisterGreater(sortbase, HeatCmp::GreaterByRawRndRT);
//    RegisterLess(sortbase, HeatCmp::LessByRawRndRT);
//
//
//    /*sequential*/
//    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
//            SortBase::SB_sequential | SortBase::SB_iops);
//    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMASeqIOs);
//    RegisterLess(sortbase, HeatCmp::LessByLongTermEMASeqIOs);
//
//    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
//            SortBase::SB_sequential | SortBase::SB_bw);
//    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMASeqBW);
//    RegisterLess(sortbase, HeatCmp::LessByLongTermEMASeqBW);
//
//    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
//            SortBase::SB_sequential | SortBase::SB_rt);
//    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMASeqRT);
//    RegisterLess(sortbase, HeatCmp::LessByLongTermEMASeqRT);
//
//    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
//            SortBase::SB_sequential | SortBase::SB_iops);
//    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMASeqIOs);
//    RegisterLess(sortbase, HeatCmp::LessByShortTermEMASeqIOs);
//
//    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
//            SortBase::SB_sequential | SortBase::SB_bw);
//    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMASeqBW);
//    RegisterLess(sortbase, HeatCmp::LessByShortTermEMASeqBW);
//
//    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
//            SortBase::SB_sequential | SortBase::SB_rt);
//    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMASeqRT);
//    RegisterLess(sortbase, HeatCmp::LessByShortTermEMASeqRT);
//
//    sortbase = (SortBase::SB_raw | SortBase::SB_sequential |
//            SortBase::SB_iops);
//    RegisterGreater(sortbase, HeatCmp::GreaterByRawSeqIOs);
//    RegisterLess(sortbase, HeatCmp::LessByRawSeqIOs);
//
//    sortbase = (SortBase::SB_raw | SortBase::SB_sequential |
//            SortBase::SB_bw);
//    RegisterGreater(sortbase, HeatCmp::GreaterByRawSeqBW);
//    RegisterLess(sortbase, HeatCmp::LessByRawSeqIOs);
//
//    sortbase = (SortBase::SB_raw | SortBase::SB_sequential |
//            SortBase::SB_rt);
//    RegisterGreater(sortbase, HeatCmp::GreaterByRawSeqRT);
//    RegisterLess(sortbase, HeatCmp::LessByRawSeqIOs);
//
//
//    /*random + sequential*/
//    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
//            SortBase::SB_rnd_seq | SortBase::SB_iops);
//    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMAAccumIOs);
//    RegisterLess(sortbase, HeatCmp::LessByLongTermEMAAccumIOs);
//
//    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
//            SortBase::SB_rnd_seq | SortBase::SB_bw);
//    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMAAccumBW);
//    RegisterLess(sortbase, HeatCmp::LessByLongTermEMAAccumBW);
//
//    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
//            SortBase::SB_rnd_seq | SortBase::SB_rt);
//    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMAAccumRT);
//    RegisterLess(sortbase, HeatCmp::LessByLongTermEMAAccumRT);
//
//    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
//            SortBase::SB_rnd_seq | SortBase::SB_iops);
//    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMAAccumIOs);
//    RegisterLess(sortbase, HeatCmp::LessByShortTermEMAAccumIOs);
//
//    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
//            SortBase::SB_rnd_seq | SortBase::SB_bw);
//    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMAAccumBW);
//    RegisterLess(sortbase, HeatCmp::LessByShortTermEMAAccumBW);
//
//    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
//            SortBase::SB_rnd_seq | SortBase::SB_rt);
//    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMAAccumRT);
//    RegisterLess(sortbase, HeatCmp::LessByShortTermEMAAccumRT);
//
//    sortbase = (SortBase::SB_raw | SortBase::SB_rnd_seq |
//            SortBase::SB_iops);
//    RegisterGreater(sortbase, HeatCmp::GreaterByRawAccumIOs);
//    RegisterLess(sortbase, HeatCmp::LessByRawAccumIOs);
//
//    sortbase = (SortBase::SB_raw | SortBase::SB_rnd_seq |
//            SortBase::SB_bw);
//    RegisterGreater(sortbase, HeatCmp::GreaterByRawAccumBW);
//    RegisterLess(sortbase, HeatCmp::LessByRawAccumBW);
//
//    sortbase = (SortBase::SB_raw | SortBase::SB_rnd_seq |
//            SortBase::SB_rt);
//    RegisterGreater(sortbase, HeatCmp::GreaterByRawAccumRT);
//    RegisterLess(sortbase, HeatCmp::LessByRawAccumRT);
//
//    return ret;
//}

ywb_status_t
HeatBucket::AddOBJ(ywb_uint32_t bucketindex, OBJVal*& obj)
{
    ywb_status_t ret = YWB_SUCCESS;

    obj->IncRef();
    mBuckets[bucketindex].push_back(obj);

    return ret;
}

/*
 * FIXME: a more sophisticated algorithm should be adopted
 **/
void HeatBucket::DeterminBucketBoundary()
{
    ywb_uint32_t bucketboundary = Constant::DEFAULT_BUCKET_BOUNDARY;

    ast_log_debug("Bucket boundary: " << bucketboundary);
    SetBucketBoundary(bucketboundary);
}

ywb_uint32_t
HeatBucket::CalculateOBJsAboveBoundary()
{
    ywb_uint32_t bucketboundary = 0;
    ywb_uint32_t bucketindex = Constant::BUCKET_NUM - 1;
    ywb_uint32_t total = 0;

    bucketboundary = GetBucketBoundary();
    for(; bucketindex > bucketboundary; bucketindex--)
    {
        total += mBuckets[bucketindex].size();
    }

    ast_log_debug("OBJs num above boundary: " << total);
    return total;
}

ywb_uint32_t
HeatBucket::CalculateOBJsBelowBoundary()
{
    ywb_uint32_t bucketboundary = 0;
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t total = 0;

    bucketboundary = GetBucketBoundary();
    for(; bucketindex <= bucketboundary; bucketindex++)
    {
        total += mBuckets[bucketindex].size();
    }

    ast_log_debug("OBJs num below boundary: " << total);
    return total;
}

ywb_status_t HeatBucket::GetGreater(
        ywb_uint32_t sortbase, HeatCmp::Greater* greater)
{
    return mHeatManager->GetGreater(sortbase, greater);
}

ywb_status_t HeatBucket::GetLess(
        ywb_uint32_t sortbase, HeatCmp::Less* less)
{
    return mHeatManager->GetLess(sortbase, less);
}

ywb_status_t
HeatBucket::SortAbove(ywb_uint32_t maxn)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t totalobjabove = 0;
    ywb_uint32_t bucketindex = Constant::BUCKET_NUM - 1;
    ywb_uint32_t bucketboundary = 0;
    ywb_uint32_t sorted = 0;
    HeatCmp::Greater greatthan = NULL;

    bucketboundary = GetBucketBoundary();
    totalobjabove = GetOBJsAboveBoundary();
    if(maxn > totalobjabove)
    {
        maxn = totalobjabove;
    }

    ast_log_debug("Sort buckets above boundary, participants num: " << maxn);
    ret = GetGreater(mSortBase, &greatthan);
    if(YFS_ENOENT != ret && NULL != greatthan)
    {
        while((sorted < maxn) && (bucketindex > bucketboundary))
        {
            ast_log_debug("Sort bucket: " << bucketindex);
            if(!mBuckets[bucketindex].empty())
            {
                sort(mBuckets[bucketindex].begin(), mBuckets[bucketindex].end(), greatthan);
                sorted += mBuckets[bucketindex].size();
            }

            bucketindex--;
        }

        if((sorted == 0) || (bucketindex < (Constant::BUCKET_NUM - 1)))
        {
            bucketindex++;
        }

        SetSortedAboveBoundary(sorted);
        if(bucketindex > bucketboundary)
        {
            SetLastSortedBucketAbove(bucketindex);
        }
        else
        {
            SetLastSortedBucketAbove(bucketboundary + 1);
        }
    }
    else
    {
        SetSortedAboveBoundary(0);
        SetLastSortedBucketAbove(Constant::BUCKET_NUM);
    }

    return ret;
}

ywb_status_t
HeatBucket::SortBelow(ywb_uint32_t minn)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t totalobjbelow = 0;
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t bucketboundary = 0;
    ywb_uint32_t sorted = 0;
    HeatCmp::Less lessthan = NULL;

    bucketboundary = GetBucketBoundary();
    totalobjbelow = GetOBJsBelowBoundary();
    if(minn > totalobjbelow)
    {
        minn = totalobjbelow;
    }

    ast_log_debug("Sort buckets below boundary, participants num: " << minn);
    ret = GetLess(mSortBase, &lessthan);
    if(YFS_ENOENT != ret && NULL != lessthan)
    {
        while((sorted < minn) && (bucketindex <= bucketboundary))
        {
            ast_log_debug("Sort bucket: " << bucketindex);
            if(!mBuckets[bucketindex].empty())
            {
                sort(mBuckets[bucketindex].begin(), mBuckets[bucketindex].end(), lessthan);
                sorted += mBuckets[bucketindex].size();
            }

            bucketindex++;
        }

        if(0 == sorted)
        {
            bucketindex = Constant::BUCKET_NUM;
        }
        else if(bucketindex > 0)
        {
            bucketindex--;
        }

        SetSortedBelowBoundary(sorted);
        if((bucketindex <= bucketboundary) ||
                (Constant::BUCKET_NUM == bucketindex))
        {
            SetLastSortedBucketBelow(bucketindex);
        }
        else
        {
            SetLastSortedBucketBelow(bucketboundary);
        }
    }
    else
    {
        SetSortedBelowBoundary(0);
        SetLastSortedBucketBelow(Constant::BUCKET_NUM);
    }

    return ret;
}

ywb_status_t
HeatBucket::ExtendSortAboveBoundary(ywb_uint32_t required)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t totalobjabove = 0;
    ywb_uint32_t startbucketindex = 0;
    ywb_uint32_t bucketindex = Constant::BUCKET_NUM - 1;
    ywb_uint32_t bucketboundary = 0;
    ywb_uint32_t prevsorted = 0;
    ywb_uint32_t sorted = 0;
    HeatCmp::Greater greatthan = NULL;

    bucketboundary = GetBucketBoundary();
    totalobjabove = GetOBJsAboveBoundary();
    prevsorted = GetSortedAboveBoundary();

    if(required > totalobjabove)
    {
        required = totalobjabove;
    }

    ret = GetGreater(mSortBase, &greatthan);
    if(YFS_ENOENT != ret && NULL != greatthan)
    {
        /*get the last sorted bucket above the boundary*/
        startbucketindex = GetLastSortedBucketAbove() - 1;
        bucketindex = startbucketindex;
        sorted = prevsorted;

        ast_log_debug("Extend sorting above boundary, "
                << " required sorted num: "<< required
                << " already sorted num: " << sorted
                << " last sorted bucket: " << bucketindex);
        while((sorted < required) && (bucketindex > bucketboundary))
        {
            ast_log_debug("Sort bucket: " << bucketindex);
            if(!mBuckets[bucketindex].empty())
            {
                sort(mBuckets[bucketindex].begin(), mBuckets[bucketindex].end(), greatthan);
                sorted += mBuckets[bucketindex].size();
            }

            bucketindex--;
        }

        if((sorted == prevsorted) || (bucketindex < startbucketindex))
        {
            bucketindex++;
        }

        SetSortedAboveBoundary(sorted);
        if(bucketindex > bucketboundary)
        {
            SetLastSortedBucketAbove(bucketindex);
        }
        else
        {
            SetLastSortedBucketAbove(bucketboundary + 1);
        }
    }

    return ret;
}

ywb_status_t
HeatBucket::ExtendSortBelowBoundary(ywb_uint32_t required)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t totalobjbelow = 0;
    ywb_uint32_t startbucketindex = 0;
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t bucketboundary = 0;
    ywb_uint32_t prevsorted = 0;
    ywb_uint32_t sorted = 0;
    HeatCmp::Less lessthan = NULL;

    bucketboundary = GetBucketBoundary();
    totalobjbelow = GetOBJsBelowBoundary();
    prevsorted = GetSortedBelowBoundary();

    if(required > totalobjbelow)
    {
        required = totalobjbelow;
    }

    ret = GetLess(mSortBase, &lessthan);
    if(YFS_ENOENT != ret && NULL != lessthan)
    {
        /*get the last sorted bucket below the boundary*/
        bucketindex = GetLastSortedBucketBelow();
        sorted = prevsorted;

        if(Constant::BUCKET_NUM == bucketindex)
        {
            startbucketindex = 0;
        }
        else
        {
            bucketindex++;
            startbucketindex = bucketindex;
        }

        bucketindex = startbucketindex;
        ast_log_debug("Extend sorting below boundary,"
                << " required sorted num: "<< required
                << " already sorted num: " << sorted
                << " last sorted bucket: " << bucketindex);

        while((sorted < required) && (bucketindex <= bucketboundary))
        {
            ast_log_debug("Sort bucket: " << bucketindex);
            if(!mBuckets[bucketindex].empty())
            {
                sort(mBuckets[bucketindex].begin(), mBuckets[bucketindex].end(), lessthan);
                sorted += mBuckets[bucketindex].size();
            }

            bucketindex++;
        }

        if(sorted == prevsorted)
        {
            if(0 == sorted)
            {
                bucketindex = Constant::BUCKET_NUM;
            }
            else
            {
                bucketindex--;
            }
        }
        else if((bucketindex > startbucketindex))
        {
            bucketindex--;
        }

        SetSortedBelowBoundary(sorted);
        SetLastSortedBucketBelow(bucketindex);
        if((bucketindex <= bucketboundary) ||
                (Constant::BUCKET_NUM == bucketindex))
        {
            SetLastSortedBucketBelow(bucketindex);
        }
        else
        {
            SetLastSortedBucketBelow(bucketboundary);
        }
    }

    return ret;
}

ywb_status_t
HeatBucket::GetFirstHotOBJFromBucketIndexOn(
        ywb_uint32_t bucketindex, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t lastsortedbucketabove = GetLastSortedBucketAbove();

    YWB_ASSERT(Constant::BUCKET_NUM > bucketindex);
    while((bucketindex >= lastsortedbucketabove) && mBuckets[bucketindex].empty())
    {
        bucketindex--;
    }

    if(bucketindex < lastsortedbucketabove)
    {
        ret = YFS_ENOENT;
        mCurHotBucket = lastsortedbucketabove - 1;
        mCurHotIt = mBuckets[mCurHotBucket].begin();
        *obj = NULL;
    }
    else
    {
        mCurHotBucket = bucketindex;
        mCurHotIt = mBuckets[bucketindex].begin();
        *obj = *mCurHotIt;
        mCurHotIt++;
    }

    return ret;
}

ywb_status_t
HeatBucket::GetFirstHotOBJ(OBJVal** obj)
{
    return GetFirstHotOBJFromBucketIndexOn(Constant::BUCKET_NUM - 1, obj);
}

ywb_status_t
HeatBucket::GetNextHotOBJ(OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t lastsortedbucketabove = GetLastSortedBucketAbove();

    if(mCurHotBucket < lastsortedbucketabove)
    {
        ret = YFS_ENOENT;
        *obj = NULL;
    }
    else
    {
        if(mCurHotIt != mBuckets[mCurHotBucket].end())
        {
            *obj = *mCurHotIt;
            mCurHotIt++;
        }
        else
        {
            mCurHotBucket--;
            ret = GetFirstHotOBJFromBucketIndexOn(mCurHotBucket, obj);
        }
    }

    return ret;
}

ywb_status_t
HeatBucket::GetFirstColdOBJFromBucketIndexOn(
        ywb_uint32_t bucketindex, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t lastsortedbucketbelow = GetLastSortedBucketBelow();

    YWB_ASSERT(bucketindex >= 0);
    /*has not been sorted*/
    if(Constant::BUCKET_NUM == lastsortedbucketbelow)
    {
        ret = YFS_ENOENT;
        mCurColdBucket = 0;
        mCurColdIt = mBuckets[mCurColdBucket].begin();
        *obj = NULL;
        return ret;
    }

    while((bucketindex <= lastsortedbucketbelow) &&
            mBuckets[bucketindex].empty())
    {
        bucketindex++;
    }

    if(bucketindex > lastsortedbucketbelow)
    {
        ret = YFS_ENOENT;
        mCurColdBucket = lastsortedbucketbelow + 1;
        mCurColdIt = mBuckets[mCurColdBucket].begin();
        *obj = NULL;
    }
    else
    {
        mCurColdBucket = bucketindex;
        mCurColdIt = mBuckets[bucketindex].begin();
        *obj = *mCurColdIt;
        mCurColdIt++;
    }

    return ret;
}

ywb_status_t
HeatBucket::GetFirstColdOBJ(OBJVal **obj)
{
    return GetFirstColdOBJFromBucketIndexOn(0, obj);
}

ywb_status_t
HeatBucket::GetNextColdOBJ(OBJVal **obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t lastsortedbucketbelow = GetLastSortedBucketBelow();

    if(mCurColdBucket > lastsortedbucketbelow)
    {
        ret = YFS_ENOENT;
        *obj = NULL;
    }
    else
    {
        if(mCurColdIt != mBuckets[mCurColdBucket].end())
        {
            *obj = *mCurColdIt;
            mCurColdIt++;
        }
        else
        {
            mCurColdBucket++;
            ret = GetFirstColdOBJFromBucketIndexOn(mCurColdBucket, obj);
        }
    }

    return ret;
}

ywb_status_t
HeatBucket::GetAllSatisfyingOBJs(AdviceRule& rule, set<OBJVal*>* OBJSet)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t startbucket = 0;
    ywb_uint32_t endbucket = 0;
    ywb_uint32_t curbucket = 0;

    if(Selector::SEL_from_larger == rule.GetSelector())
    {
        startbucket = Constant::BUCKET_NUM - 1;
        /*endbucket = mBucketBoundary + 1;*/
        endbucket = GetLastSortedBucketAbove();
        for(curbucket = startbucket; curbucket >= endbucket; curbucket--)
        {
            if(!mBuckets[curbucket].empty())
            {
                OBJSet->insert(mBuckets[curbucket].begin(),
                        mBuckets[curbucket].end());
            }
        }
    }
    else
    {
        YWB_ASSERT(Selector::SEL_from_smaller == rule.GetSelector());
        startbucket = 0;
        /*endbucket = mBucketBoundary;*/
        endbucket = GetLastSortedBucketBelow();
        if(Constant::BUCKET_NUM == endbucket)
        {
            return YWB_SUCCESS;
        }

        for(curbucket = startbucket; curbucket <= endbucket; curbucket++)
        {
            if(!mBuckets[curbucket].empty())
            {
                OBJSet->insert(mBuckets[curbucket].begin(),
                        mBuckets[curbucket].end());
            }
        }
    }

    return ret;
}

ywb_status_t
HeatBucket::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t bucketindex = 0;
    OBJVal* obj = NULL;
    vector<OBJVal*>::iterator iter;
//    map<ywb_uint32_t, HeatCmp::Greater>::iterator mIt;
//    map<ywb_uint32_t, HeatCmp::Less>::iterator mIt2;

    ast_log_debug("Destroy heat bucket");
    for(; bucketindex < Constant::BUCKET_NUM; bucketindex++)
    {
        if(!mBuckets[bucketindex].empty())
        {
            iter = mBuckets[bucketindex].begin();
            for(; iter != mBuckets[bucketindex].end(); )
            {
                obj = *iter;
                iter = mBuckets[bucketindex].erase(iter);
                obj->DecRef();
            }
        }
    }

//    mIt = mGreaters.begin();
//    for(; mIt != mGreaters.end(); )
//    {
//        mGreaters.erase(mIt++);
//    }
//
//    mIt2 = mLesses.begin();
//    for(; mIt2 != mLesses.end(); )
//    {
//        mLesses.erase(mIt2++);
//    }

    mHeatDistribution = NULL;
    mHeatManager = NULL;

    return ret;
}

ywb_status_t
HeatDistribution::GetFirstDisk(Disk **disk)
{
    *disk = NULL;
    return YFS_ENOENT;
}

ywb_status_t
HeatDistribution::GetNextDisk(Disk **disk)
{
    *disk = NULL;
    return YFS_ENOENT;
}

ywb_status_t
HeatDistribution::GetFirstOBJ(OBJVal **obj)
{
    *obj = NULL;
    return YFS_ENOENT;
}

ywb_status_t
HeatDistribution::GetNextOBJ(OBJVal **obj)
{
    *obj = NULL;
    return YFS_ENOENT;
}

ywb_status_t
HeatDistribution::PreSetupCheck()
{
    return YWB_SUCCESS;
}

void HeatDistribution::PutDisk(Disk* disk)
{

}

void HeatDistribution::PutOBJ(OBJVal* obj)
{

}

ywb_status_t
HeatDistribution::SetupHeatRangeSummary(IOStat::Type type)
{
    ywb_status_t ret = YWB_SUCCESS;
    Disk* disk = NULL;
    DiskIO* diskio = NULL;
    HeatRangeSummary* summary = NULL;
    HeatRange* scoperange = NULL;
    DiskIOStat* diskstat = NULL;
    DiskIOStatRange* diskrange = NULL;
    ywb_uint64_t totalobjsnum = 0;

    ast_log_debug("Setup heat range summary of IOStat type: " << type
            << "[IOST_raw: 0, IOST_short_term_ema: 1, IOST_long_term_ema: 2]");
    summary = new HeatRangeSummary();
    if(NULL == summary)
    {
        return YFS_EOUTOFMEMORY;
    }

    GetFirstDisk(&disk);
    while(NULL != disk)
    {
        disk->GetDiskIO(&diskio);
        totalobjsnum += disk->GetOBJCnt();

        diskio->GetStatIOs(type, &diskstat);
        diskio->GetIOsRange(type, &diskrange);
        /*minimum/maximum random read IOs*/
        summary->GetRndReadCnt(&scoperange);
        scoperange->UpdateSum(diskstat->GetRndReadStat(), ywb_true);
        if(diskrange->GetRndReadMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetRndReadMin());
        }

        if(diskrange->GetRndReadMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetRndReadMax());
        }

        /*minimum/maximum random write IOs*/
        summary->GetRndWriteCnt(&scoperange);
        scoperange->UpdateSum(diskstat->GetRndWriteStat(), ywb_true);
        if(diskrange->GetRndWriteMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetRndWriteMin());
        }

        if(diskrange->GetRndWriteMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetRndWriteMax());
        }

        /*minimum/maximum sequential read IOs*/
        summary->GetSeqReadCnt(&scoperange);
        scoperange->UpdateSum(diskstat->GetSeqReadStat(), ywb_true);
        if(diskrange->GetSeqReadMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetSeqReadMin());
        }

        if(diskrange->GetSeqReadMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetSeqReadMax());
        }

        /*minimum/maximum sequential write IOs*/
        summary->GetSeqWriteCnt(&scoperange);
        scoperange->UpdateSum(diskstat->GetSeqWriteStat(), ywb_true);
        if(diskrange->GetSeqWriteMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetSeqWriteMin());
        }

        if(diskrange->GetSeqWriteMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetSeqWriteMax());
        }

        /*minimum/maximum random total IOs*/
        summary->GetRndAccumIOs(&scoperange);
        scoperange->UpdateSum(diskstat->GetRndStat(), ywb_true);
        if(diskrange->GetRndMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetRndMin());
        }

        if(diskrange->GetRndMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetRndMax());
        }

        /*minimum/maximum sequential total IOs*/
        summary->GetSeqAccumIOs(&scoperange);
        scoperange->UpdateSum(diskstat->GetSeqStat(), ywb_true);
        if(diskrange->GetSeqMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetSeqMin());
        }

        if(diskrange->GetSeqMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetSeqMax());
        }

        /*minimum/maximum random+sequential total IOs*/
        summary->GetRndSeqAccumIOs(&scoperange);
        scoperange->UpdateSum((diskstat->GetRndStat() +
                diskstat->GetSeqStat()), ywb_true);
        if(diskrange->GetRndSeqMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetRndSeqMin());
        }

        if(diskrange->GetRndSeqMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetRndSeqMax());
        }

        diskio->GetStatBW(type, &diskstat);
        diskio->GetBWRange(type, &diskrange);
        /*minimum/maximum random total BW*/
        summary->GetRndAccumBW(&scoperange);
        scoperange->UpdateSum(diskstat->GetRndStat(), ywb_true);
        if(diskrange->GetRndMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetRndMin());
        }

        if(diskrange->GetRndMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetRndMax());
        }

        /*minimum/maximum sequential total BW*/
        summary->GetSeqAccumBW(&scoperange);
        scoperange->UpdateSum(diskstat->GetSeqStat(), ywb_true);
        if(diskrange->GetSeqMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetSeqMin());
        }

        if(diskrange->GetSeqMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetSeqMax());
        }

        /*minimum/maximum random+sequential total BW*/
        summary->GetRndSeqAccumBW(&scoperange);
        scoperange->UpdateSum((diskstat->GetRndStat() +
                diskstat->GetSeqStat()), ywb_true);
        if(diskrange->GetRndSeqMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetRndSeqMin());
        }

        if(diskrange->GetRndSeqMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetRndSeqMax());
        }

        diskio->GetStatRT(type, &diskstat);
        diskio->GetRTRange(type, &diskrange);
        /*minimum/maximum random total RT*/
        summary->GetRndAccumRT(&scoperange);
        scoperange->UpdateSum(diskstat->GetRndStat(), ywb_true);
        if(diskrange->GetRndMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetRndMin());
        }

        if(diskrange->GetRndMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetRndMax());
        }

        /*minimum/maximum sequential total RT*/
        summary->GetSeqAccumRT(&scoperange);
        scoperange->UpdateSum(diskstat->GetSeqStat(), ywb_true);
        if(diskrange->GetSeqMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetSeqMin());
        }

        if(diskrange->GetSeqMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetSeqMax());
        }

        /*minimum/maximum random+sequential total RT*/
        summary->GetRndSeqAccumRT(&scoperange);
        scoperange->UpdateSum((diskstat->GetRndStat() +
                diskstat->GetSeqStat()), ywb_true);
        if(diskrange->GetRndSeqMin() < scoperange->GetMin())
        {
            scoperange->SetMin(diskrange->GetRndSeqMin());
        }

        if(diskrange->GetRndSeqMax() > scoperange->GetMax())
        {
            scoperange->SetMax(diskrange->GetRndSeqMax());
        }

        PutDisk(disk);
        GetNextDisk(&disk);
    }

    /*IO related*/
    summary->GetRndReadCnt(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetRndWriteCnt(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetSeqReadCnt(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetSeqWriteCnt(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetRndAccumIOs(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetSeqAccumIOs(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetRndSeqAccumIOs(&scoperange);
    scoperange->Calculate(totalobjsnum);

    /*BW related*/
    summary->GetRndAccumBW(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetSeqAccumBW(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetRndSeqAccumBW(&scoperange);
    scoperange->Calculate(totalobjsnum);

    /*RT related*/
    summary->GetRndAccumRT(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetSeqAccumRT(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->GetRndSeqAccumRT(&scoperange);
    scoperange->Calculate(totalobjsnum);

    summary->SetHeatRangeType(type);
    mHeatSummary.push_back(summary);

    return ret;
}

ywb_uint32_t
HeatDistribution::GetOBJBucketIndex(ywb_uint32_t partitionbase,
        HeatRangeSummary*& summary, OBJVal*& obj)
{
    IOStat::Type type = IOStat::IOST_cnt;
    OBJIO* objio = NULL;
    ChunkIOStat *stat = NULL;
    HeatRange* heatrange = NULL;
    ywb_uint32_t minheat = 0;
    ywb_uint32_t objheat = 0;
    ywb_uint32_t heatdiff = 0;
    ywb_uint32_t bucketcap = 0;
    ywb_uint32_t bucketindex = Constant::BUCKET_NUM;

    YWB_ASSERT(summary);
    YWB_ASSERT(obj);

    obj->GetOBJIO(&objio);
    YWB_ASSERT(objio);

    summary->GetHeatRangeType(&type);
    if(IOStat::IOST_raw == type ||
            IOStat::IOST_short_term_ema == type ||
            IOStat::IOST_long_term_ema == type)
    {
        objio->GetIOStat(type, &stat);
    }
    else
    {
        ast_log_debug("invalid IOStat type");
        /*other heat range type is not supported*/
        return Constant::BUCKET_NUM;
    }

    /*if OBJ has no enough learning, it will not participate into partition*/
    if(!objio->IsMonitoredLongEnough())
    {
        ast_log_debug("OBJ is not monitored long enough");
        return Constant::BUCKET_NUM;
    }

    if((PartitionBase::PB_iops & partitionbase))
    {
        ywb_uint32_t rndreadios = 0;
        ywb_uint32_t seqreadios = 0;
        ywb_uint32_t rndwriteios = 0;
        ywb_uint32_t seqwriteios = 0;
        ywb_uint32_t rndios = 0;
        ywb_uint32_t seqios = 0;
        ywb_uint32_t rndseqios = 0;

        rndreadios = stat->GetRndReadIOs();
        seqreadios = stat->GetSeqReadIOs();
        rndwriteios = stat->GetRndWriteIOs();
        seqwriteios = stat->GetSeqWriteIOs();
        rndios = rndreadios + rndwriteios;
        seqios = seqreadios + seqwriteios;
        rndseqios = rndios + seqios;

        if((PartitionBase::PB_random & partitionbase))
        {
            summary->GetRndAccumIOs(&heatrange);
            objheat = rndios;
        }
        else if((PartitionBase::PB_sequential & partitionbase))
        {
            summary->GetSeqAccumIOs(&heatrange);
            objheat = seqios;
        }
        else if((PartitionBase::PB_rnd_seq & partitionbase))
        {
            summary->GetRndSeqAccumIOs(&heatrange);
            objheat = rndseqios;
        }
    }
    else if((PartitionBase::PB_bw & partitionbase))
    {
        ywb_uint32_t rndbw = 0;
        ywb_uint32_t seqbw = 0;
        ywb_uint32_t rndseqbw = 0;

        rndbw = stat->GetRndReadBW() + stat->GetRndWriteBW();
        seqbw = stat->GetSeqReadBW() + stat->GetSeqWriteBW();
        rndseqbw = rndbw + seqbw;

        if((PartitionBase::PB_random & partitionbase))
        {
            summary->GetRndAccumBW(&heatrange);
            objheat = rndbw;
        }
        else if((PartitionBase::PB_sequential & partitionbase))
        {
            summary->GetSeqAccumBW(&heatrange);
            objheat = seqbw;
        }
        else if((PartitionBase::PB_rnd_seq & partitionbase))
        {
            summary->GetRndSeqAccumBW(&heatrange);
            objheat = rndseqbw;
        }
    }
    else if((PartitionBase::PB_rt & partitionbase))
    {
        ywb_uint32_t rndresp = 0;
        ywb_uint32_t seqresp = 0;
        ywb_uint32_t rndseqresp = 0;

        rndresp = stat->GetRndTotalRT();
        seqresp = stat->GetSeqTotalRT();
        rndseqresp = rndresp + seqresp;

        if((PartitionBase::PB_random & partitionbase))
        {
            summary->GetRndAccumRT(&heatrange);
            objheat = rndresp;
        }
        else if((PartitionBase::PB_sequential & partitionbase))
        {
            summary->GetSeqAccumRT(&heatrange);
            objheat = seqresp;
        }
        else if((PartitionBase::PB_rnd_seq & partitionbase))
        {
            summary->GetRndSeqAccumRT(&heatrange);
            objheat = rndseqresp;
        }
    }

    if(NULL == heatrange)
    {
        ast_log_debug("heat range is empty");
        return Constant::BUCKET_NUM;
    }

    minheat = heatrange->GetMin();
    bucketcap = heatrange->GetBucketCap();
    if(YWB_UINT32_MAX == bucketcap)
    {
        ast_log_debug("bucket capacity is YWB_UINT32_MAX");
        return Constant::BUCKET_NUM;
    }

    /*randomly select bucket if bucket capacity is 0*/
    if(0 == bucketcap)
    {
        bucketindex = (rand()) % Constant::BUCKET_NUM;
    }
    else
    {
        heatdiff = (objheat > minheat) ? (objheat - minheat) : 0;
        bucketindex = (heatdiff / bucketcap);
        if(bucketindex >= Constant::BUCKET_NUM)
        {
            bucketindex = Constant::BUCKET_NUM - 1;
        }
    }

    return bucketindex;
}

ywb_uint32_t
HeatDistribution::GetOBJBucketIndex(ywb_uint32_t advicetype,
        ywb_uint32_t partitionbase,
        HeatRangeSummary*& summary, OBJVal*& obj)
{
    IOStat::Type type = IOStat::IOST_cnt;
    OBJIO* objio = NULL;
    ChunkIOStat *stat = NULL;
    HeatRange* heatrange = NULL;
    ywb_uint32_t minheat = 0;
    ywb_uint32_t minheatboundary = 0;
    ywb_uint32_t maxheatboundary = 0;
    ywb_uint32_t objheat = 0;
    ywb_uint32_t heatdiff = 0;
    ywb_uint32_t bucketcap = 0;
    ywb_uint32_t bucketindex = Constant::BUCKET_NUM;

    YWB_ASSERT(summary);
    YWB_ASSERT(obj);

    obj->GetOBJIO(&objio);
    YWB_ASSERT(objio);

    summary->GetHeatRangeType(&type);
    if(IOStat::IOST_raw == type ||
            IOStat::IOST_short_term_ema == type ||
            IOStat::IOST_long_term_ema == type)
    {
        objio->GetIOStat(type, &stat);
    }
    else
    {
        ast_log_debug("invalid IOStat type");
        /*other heat range type is not supported*/
        return Constant::BUCKET_NUM;
    }

    /*if OBJ has no enough learning, it will not participate into partition*/
    if(!objio->IsMonitoredLongEnough())
    {
        ast_log_debug("OBJ is not monitored long enough");
        return Constant::BUCKET_NUM;
    }

    if((PartitionBase::PB_iops & partitionbase))
    {
        ywb_uint32_t rndreadios = 0;
        ywb_uint32_t seqreadios = 0;
        ywb_uint32_t rndwriteios = 0;
        ywb_uint32_t seqwriteios = 0;
        ywb_uint32_t rndios = 0;
        ywb_uint32_t seqios = 0;
        ywb_uint32_t rndseqios = 0;

        rndreadios = stat->GetRndReadIOs();
        seqreadios = stat->GetSeqReadIOs();
        rndwriteios = stat->GetRndWriteIOs();
        seqwriteios = stat->GetSeqWriteIOs();
        rndios = rndreadios + rndwriteios;
        seqios = seqreadios + seqwriteios;
        rndseqios = rndios + seqios;

        if((PartitionBase::PB_random & partitionbase))
        {
            summary->GetRndAccumIOs(&heatrange);
            objheat = rndios;
        }
        else if((PartitionBase::PB_sequential & partitionbase))
        {
            summary->GetSeqAccumIOs(&heatrange);
            objheat = seqios;
        }
        else if((PartitionBase::PB_rnd_seq & partitionbase))
        {
            summary->GetRndSeqAccumIOs(&heatrange);
            objheat = rndseqios;
        }
    }
    else if((PartitionBase::PB_bw & partitionbase))
    {
        ywb_uint32_t rndbw = 0;
        ywb_uint32_t seqbw = 0;
        ywb_uint32_t rndseqbw = 0;

        rndbw = stat->GetRndReadBW() + stat->GetRndWriteBW();
        seqbw = stat->GetSeqReadBW() + stat->GetSeqWriteBW();
        rndseqbw = rndbw + seqbw;

        if((PartitionBase::PB_random & partitionbase))
        {
            summary->GetRndAccumBW(&heatrange);
            objheat = rndbw;
        }
        else if((PartitionBase::PB_sequential & partitionbase))
        {
            summary->GetSeqAccumBW(&heatrange);
            objheat = seqbw;
        }
        else if((PartitionBase::PB_rnd_seq & partitionbase))
        {
            summary->GetRndSeqAccumBW(&heatrange);
            objheat = rndseqbw;
        }
    }
    else if((PartitionBase::PB_rt & partitionbase))
    {
        ywb_uint32_t rndresp = 0;
        ywb_uint32_t seqresp = 0;
        ywb_uint32_t rndseqresp = 0;

        rndresp = stat->GetRndTotalRT();
        seqresp = stat->GetSeqTotalRT();
        rndseqresp = rndresp + seqresp;

        if((PartitionBase::PB_random & partitionbase))
        {
            summary->GetRndAccumRT(&heatrange);
            objheat = rndresp;
        }
        else if((PartitionBase::PB_sequential & partitionbase))
        {
            summary->GetSeqAccumRT(&heatrange);
            objheat = seqresp;
        }
        else if((PartitionBase::PB_rnd_seq & partitionbase))
        {
            summary->GetRndSeqAccumRT(&heatrange);
            objheat = rndseqresp;
        }
    }

    if(NULL == heatrange)
    {
        ast_log_debug("heat range is empty");
        return Constant::BUCKET_NUM;
    }

    minheat = heatrange->GetMin();
    minheatboundary = heatrange->GetMinPortionBoundary();
    maxheatboundary = heatrange->GetMaxPortionBoundary();
    bucketcap = heatrange->GetBucketCap();
    if((YWB_UINT32_MAX == bucketcap))
    {
        return Constant::BUCKET_NUM;
    }

    if((0 == bucketcap))
    {
        if((AdviceType::AT_ctr_hot == advicetype) &&
                (maxheatboundary < objheat))
        {
            bucketindex = Constant::DEFAULT_BUCKET_BOUNDARY + ((rand()) %
                    (Constant::BUCKET_NUM - Constant::DEFAULT_BUCKET_BOUNDARY));
        }
        else if((AdviceType::AT_ctr_cold == advicetype) &&
                (minheatboundary > objheat))
        {
            bucketindex = (rand()) % Constant::DEFAULT_BUCKET_BOUNDARY;
        }
        else
        {
            bucketindex = rand() % 100;
            /*80%  probability*/
            if(bucketindex < 80)
            {
                bucketindex = Constant::BUCKET_NUM;
            }
            else
            {
                bucketindex = bucketindex % (Constant::BUCKET_NUM);
            }
        }
    }
    else
    {
        heatdiff = (objheat > minheat) ? (objheat - minheat) : 0;
        bucketindex = (heatdiff / bucketcap);
        if(bucketindex >= Constant::BUCKET_NUM)
        {
            bucketindex = Constant::BUCKET_NUM - 1;
        }
    }

    return bucketindex;
}

ywb_status_t
HeatDistribution::AddHeatBucket(HeatBucket* bucket)
{
    ywb_status_t ret = YWB_SUCCESS;
    AdviceRule advicerule;
    HeatBucket* existbucket = NULL;

    YWB_ASSERT(bucket != NULL);
    advicerule.SetPartitionBase(bucket->GetPartitionBase());
    advicerule.SetSortBase(bucket->GetSortBase());
    ret = GetHeatBucket(advicerule, &existbucket);
    if((YWB_SUCCESS == ret) && (existbucket != NULL))
    {
        return YFS_EEXIST;
    }
    else
    {
        mHeatBucket.push_back(bucket);
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t
HeatDistribution::Partition(AdviceRule& rule,
        HeatRangeSummary*& summary, HeatBucket*& bucket)
{
    ywb_status_t ret = YWB_SUCCESS;
    OBJVal* obj = NULL;
    ywb_uint32_t partitionbase = 0;
    ywb_uint32_t bucketindex = 0;
    ywb_uint32_t objnumabove = 0;
    ywb_uint32_t objnumbelow = 0;
    ywb_uint32_t totalobjs = 0;
    ywb_uint32_t partitionedobjs = 0;
//    ostringstream ostr;

//    ostr << "Partition ---->>>>\n";
//    ostr << "Partition rule [" << rule.GetAdviceType()
//            << ", " << rule.GetPartitionScope()
//            << ", " << rule.GetPartitionBase()
//            << ", " << rule.GetSortBase()
//            << ", " << rule.GetSelector() << "]\n";
    ast_log_debug("Partition ---->>>>"
            << "Partition rule [" << rule.GetAdviceType()
            << ", " << rule.GetPartitionScope()
            << ", " << rule.GetPartitionBase()
            << ", " << rule.GetSortBase()
            << ", " << rule.GetSelector() << "]\n");

//    summary->Dump(&ostr);
//    ast_log_debug(ostr.str());

    partitionbase = bucket->GetPartitionBase();
    ret = GetFirstOBJ(&obj);
    /*add all OBJs into bucket*/
    while((YWB_SUCCESS == ret) && (NULL != obj))
    {
        totalobjs++;
        bucketindex = GetOBJBucketIndex(rule.GetAdviceType(),
                partitionbase, summary, obj);
//        obj->Dump(&ostr);

        if(0 <= bucketindex && bucketindex < Constant::BUCKET_NUM)
        {
            partitionedobjs++;
            bucket->AddOBJ(bucketindex, obj);
        }

        PutOBJ(obj);
        ret = GetNextOBJ(&obj);
    }

    ast_log_debug("Partition done, total OBJs num " << totalobjs
            << "partitioned OBJs num " << partitionedobjs << " <<<<----\n");

    if(partitionedobjs != 0)
    {
        bucket->DeterminBucketBoundary();
        /*calculate number of OBJs above and below the boundary*/
        objnumabove = bucket->CalculateOBJsAboveBoundary();
        objnumbelow = bucket->CalculateOBJsBelowBoundary();
        bucket->SetOBJsAboveBoundary(objnumabove);
        bucket->SetOBJsBelowBoundary(objnumbelow);

        ret = YWB_SUCCESS;
    }
    else
    {
        ret = YFS_ENODATA;
    }

    return ret;
}

ywb_status_t
HeatDistribution::Sort(AdviceRule& rule, HeatBucket*& bucket,
        ywb_uint32_t sortnumaboveboundary,
        ywb_uint32_t sortnumbelowboundary)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(sortnumaboveboundary > 0)
    {
        bucket->SortAbove(sortnumaboveboundary);
    }

    if(sortnumbelowboundary > 0)
    {
        bucket->SortBelow(sortnumbelowboundary);
    }

    return ret;
}

ywb_status_t
HeatDistribution::Setup(AdviceRule& rule,
        ywb_uint32_t sortnumaboveboundary,
        ywb_uint32_t sortnumbelowboundary)
{
    ywb_status_t ret = YWB_SUCCESS;
    list<HeatBucket*>::iterator iter;
    HeatBucket* bucket = NULL;
    HeatRangeSummary* summary = NULL;
    IOStat::Type heattype = IOStat::IOST_cnt;
    /*ywb_uint32_t objsabove = 0;*/
    /*ywb_uint32_t objsbelow = 0;*/

    ret = PreSetupCheck();
    if(ret != YWB_SUCCESS)
    {
        return ret;
    }

    ret = GetHeatBucket(rule, &bucket);
    /*not find a matched heat bucket*/
    if(YFS_ENOENT == ret)
    {
        ret = GetHeatRangeTypeFromAdviceRule(rule, &heattype);
        if(YFS_EINVAL == ret)
        {
            return ret;
        }

        ret = GetHeatRangeSummary(heattype, &summary);
        /*not find the heat range summary*/
        if(YFS_ENOENT == ret)
        {
            /*set up heat range summary*/
            ret = SetupHeatRangeSummary(heattype);
        }

        /*fail to setup heat range summary*/
        if(YWB_SUCCESS != ret)
        {
            return ret;
        }

        bucket = new HeatBucket(rule.GetPartitionBase(),
                rule.GetSortBase(), this);
        if(NULL == bucket)
        {
            return YFS_EOUTOFMEMORY;
        }

        ret = GetHeatRangeSummary(heattype, &summary);
        YWB_ASSERT(YWB_SUCCESS == ret);
        YWB_ASSERT(summary != NULL);

        /*partition all OBJs into heat bucket*/
        ret = Partition(rule, summary, bucket);
        if(YWB_SUCCESS == ret)
        {
            /*sort bucket*/
            Sort(rule, bucket, sortnumaboveboundary, sortnumbelowboundary);
            ret = AddHeatBucket(bucket);
            YWB_ASSERT(YWB_SUCCESS == ret);
        }
    }
    else
    {
        YWB_ASSERT(bucket);
        /*objsabove = bucket->GetOBJsAboveBoundary();*/
        /*objsbelow = bucket->GetOBJsBelowBoundary();*/

        /*if require more sorted OBJs above boundary*/
        if(/*(objsabove > sortnumaboveboundary) &&*/
                (sortnumaboveboundary > bucket->GetSortedAboveBoundary()))
        {
            bucket->ExtendSortAboveBoundary(sortnumaboveboundary);
        }

        /*if require more sorted OBJs below boundary*/
        if(/*(objsbelow > sortnumbelowboundary) &&*/
                (sortnumbelowboundary > bucket->GetSortedBelowBoundary()))
        {
            bucket->ExtendSortBelowBoundary(sortnumbelowboundary);
        }
    }

    return ret;
}

ywb_status_t
HeatDistribution::GetHeatRangeTypeFromAdviceRule(
        AdviceRule& rule, IOStat::Type* type)
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t partitionbase = 0;

    partitionbase = rule.GetPartitionBase();
    if((partitionbase & (PartitionBase::PB_long_term)) &&
            (partitionbase & (PartitionBase::PB_ema)))
    {
        *type = IOStat::IOST_long_term_ema;
    }
    else if((partitionbase & (PartitionBase::PB_short_term)) &&
            (partitionbase & (PartitionBase::PB_ema)))
    {
        *type = IOStat::IOST_short_term_ema;
    }
    else if(partitionbase & (PartitionBase::PB_raw))
    {
        *type = IOStat::IOST_raw;
    }
    else
    {
        *type = IOStat::IOST_cnt;
    }

    if(IOStat::IOST_cnt == *type)
    {
        ret = YFS_EINVAL;
    }

    return ret;
}

ywb_status_t
HeatDistribution::GetHeatRangeSummary(IOStat::Type type,
        HeatRangeSummary** summary)
{
    ywb_status_t ret = YWB_SUCCESS;
    list<HeatRangeSummary*>::iterator iter;
    HeatRangeSummary* cursummary = NULL;
    IOStat::Type curtype = IOStat::IOST_cnt;

    iter = mHeatSummary.begin();
    for(; iter != mHeatSummary.end(); iter++)
    {
        cursummary = *iter;
        cursummary->GetHeatRangeType(&curtype);
        if(curtype == type)
        {
            break;
        }
    }

    if(iter != mHeatSummary.end())
    {
        *summary = *iter;
    }
    else
    {
        *summary = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistribution::GetHeatBucket(AdviceRule& rule,
        HeatBucket** heatdistribution)
{
    ywb_status_t ret = YWB_SUCCESS;
    list<HeatBucket*>::iterator iter;
    HeatBucket* curbucket = NULL;

    iter = mHeatBucket.begin();
    for(; iter != mHeatBucket.end(); iter++)
    {
        curbucket = *iter;
        if((curbucket->GetPartitionBase() == rule.GetPartitionBase()) &&
                (curbucket->GetSortBase() == rule.GetSortBase()))
        {
            break;
        }
    }

    if(iter != mHeatBucket.end())
    {
        *heatdistribution = *iter;
    }
    else
    {
        *heatdistribution = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistribution::GetFirstHotOBJ(AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    HeatBucket* distribution = NULL;

    ret = GetHeatBucket(rule, &distribution);
    if((YFS_ENOENT != ret) && (distribution != NULL))
    {
        ret = distribution->GetFirstHotOBJ(obj);
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistribution::GetNextHotOBJ(AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    HeatBucket* distribution = NULL;

    ret = GetHeatBucket(rule, &distribution);
    if((YFS_ENOENT != ret) && (distribution != NULL))
    {
        ret = distribution->GetNextHotOBJ(obj);
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistribution::GetFirstColdOBJ(AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    HeatBucket* distribution = NULL;

    ret = GetHeatBucket(rule, &distribution);
    if((YFS_ENOENT != ret) && (distribution != NULL))
    {
        ret = distribution->GetFirstColdOBJ(obj);
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistribution::GetNextColdOBJ(AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    HeatBucket* distribution = NULL;

    ret = GetHeatBucket(rule, &distribution);
    if((YFS_ENOENT != ret) && (distribution != NULL))
    {
        ret = distribution->GetNextColdOBJ(obj);
    }
    else
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistribution::GetAllSatisfyingOBJs(AdviceRule& rule, set<OBJVal*>* OBJSet)
{
    ywb_status_t ret = YWB_SUCCESS;
    HeatBucket* heatbucket = NULL;

    YWB_ASSERT(OBJSet != NULL);
    OBJSet->clear();
    ret = GetHeatBucket(rule, &heatbucket);
    if((YWB_SUCCESS == ret) && (heatbucket != NULL))
    {
        ret = heatbucket->GetAllSatisfyingOBJs(rule, OBJSet);
    }

    return ret;
}

ywb_status_t
HeatDistribution::Destroy(AdviceRule& rule)
{
    ywb_status_t ret = YWB_SUCCESS;
    HeatBucket* distribution = NULL;

    ret = GetHeatBucket(rule, &distribution);
    if((YFS_ENOENT != ret) && (distribution != NULL))
    {
        delete distribution;
        distribution = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistribution::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    HeatBucket* distribution = NULL;
    HeatRangeSummary* summary = NULL;
    list<HeatBucket*>::iterator iter1;
    list<HeatRangeSummary*>::iterator iter2;

    iter1 = mHeatBucket.begin();
    for(; iter1 != mHeatBucket.end(); )
    {
        distribution = *iter1;
        iter1 = mHeatBucket.erase(iter1);
        delete distribution;
        distribution = NULL;
    }

    iter2 = mHeatSummary.begin();
    for(; iter2 != mHeatSummary.end(); )
    {
        summary = *iter2;
        iter2 = mHeatSummary.erase(iter2);
        delete summary;
        summary = NULL;
    }

    mSubPoolHeat = NULL;
    return ret;
}

ywb_status_t
DiskHeatDistribution::GetFirstDisk(Disk **disk)
{
    ywb_status_t ret = YWB_SUCCESS;

    GetDisk(disk);
    return ret;
}

ywb_status_t
DiskHeatDistribution::GetNextDisk(Disk **disk)
{
    *disk = NULL;
    return YFS_ENOENT;
}

ywb_status_t
DiskHeatDistribution::GetFirstOBJ(OBJVal **obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    Disk* disk = NULL;

    *obj = NULL;
    GetDisk(&disk);
    if(disk != NULL)
    {
        ret = disk->GetFirstOBJ(obj);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
DiskHeatDistribution::GetNextOBJ(OBJVal **obj)
{
    Disk* disk = NULL;

    GetDisk(&disk);
    /*assume the disk exist when invoking GetNextOBJ*/
    return disk->GetNextOBJ(obj);
}

ywb_status_t
DiskHeatDistribution::PreSetupCheck()
{
    ywb_status_t ret = YWB_SUCCESS;
    Disk* disk = NULL;

    GetDisk(&disk);
    /*disk has no OBJ*/
    if(0 == disk->GetOBJCnt())
    {
        disk->SetFlagComb(Disk::DF_no_obj);
        ret = YFS_ENODATA;
    }

    return ret;
}

void
DiskHeatDistribution::PutDisk(Disk* disk)
{

}

void
DiskHeatDistribution::PutOBJ(OBJVal* obj)
{
    mDisk->PutOBJ(obj);
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::SetupDiskHeatDistribution(
        DiskKey& diskkey, AdviceRule& rule,
        ywb_uint32_t sortnumaboveboundary,
        ywb_uint32_t sortnumbelowboundary)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskHeatDistribution* diskheat = NULL;
    Disk* disk = NULL;
    ywb_bool_t newdiskheat = ywb_false;

    ast_log_debug("Setup disk wise heat distribution, disk: ["
            << diskkey.GetSubPoolId() << ", " << diskkey.GetDiskId() << "]");

    ret = GetDiskHeatDistribution(diskkey, &diskheat);
    if(YFS_ENOENT == ret)
    {
        mTier->GetDisk(diskkey, &disk);
        diskheat = new DiskHeatDistribution(disk, mSubPoolHeat);
        if(NULL == diskheat)
        {
            mTier->PutDisk(disk);
            return YFS_EOUTOFMEMORY;
        }

        mTier->PutDisk(disk);
        newdiskheat = ywb_true;
    }

    ret = diskheat->Setup(rule, sortnumaboveboundary, sortnumbelowboundary);
    if(YWB_SUCCESS == ret && ywb_true == newdiskheat)
    {
        mDiskHeat.insert(std::make_pair(diskkey, diskheat));
    }
    else if(ywb_true == newdiskheat)
    {
        delete diskheat;
        diskheat = NULL;
    }

    return ret;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::SetupDiskHeatDistribution(
        AdviceRule& rule, ywb_uint32_t sortnumaboveboundary,
        ywb_uint32_t sortnumbelowboundary)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::GetDiskHeatDistribution(
        DiskKey& key, DiskHeatDistribution** heatdistribution)
{
    ywb_status_t ret = YFS_ENOENT;
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp>::iterator iter;

    *heatdistribution = NULL;
    iter = mDiskHeat.find(key);
    if(mDiskHeat.end() != iter)
    {
        *heatdistribution = iter->second;
        ret = YWB_SUCCESS;
    }

    return ret;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::CalculatePerformanceSkew()
{
    return YFS_ENOTIMPL;
}

/*TODO*/
ywb_status_t
TierInternalDiskWiseHeatDistribution::GetFirstHotOBJ(
        AdviceRule& rule, OBJVal** obj)
{
    return YFS_ENOENT;
}

/*TODO*/
ywb_status_t
TierInternalDiskWiseHeatDistribution::GetNextHotOBJ(
        AdviceRule& rule, OBJVal** obj)
{
    return YFS_ENOENT;
}

/*TODO*/
ywb_status_t
TierInternalDiskWiseHeatDistribution::GetFirstColdOBJ(
        AdviceRule& rule, OBJVal** obj)
{
    return YFS_ENOENT;
}

/*TODO*/
ywb_status_t
TierInternalDiskWiseHeatDistribution::GetNextColdOBJ(
        AdviceRule& rule, OBJVal** obj)
{
    return YFS_ENOENT;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::GetFirstHotOBJ(
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YFS_ENOENT;
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp>::iterator iter;
    DiskHeatDistribution* diskheat = NULL;

    iter = mDiskHeat.find(diskkey);
    if(mDiskHeat.end() != iter)
    {
        diskheat = iter->second;
        ret = diskheat->GetFirstHotOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::GetNextHotOBJ(
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YFS_ENOENT;
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp>::iterator iter;
    DiskHeatDistribution* diskheat = NULL;

    iter = mDiskHeat.find(diskkey);
    if(mDiskHeat.end() != iter)
    {
        diskheat = iter->second;
        ret = diskheat->GetNextHotOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::GetFirstColdOBJ(
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YFS_ENOENT;
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp>::iterator iter;
    DiskHeatDistribution* diskheat = NULL;

    iter = mDiskHeat.find(diskkey);
    if(mDiskHeat.end() != iter)
    {
        diskheat = iter->second;
        ret = diskheat->GetFirstColdOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::GetNextColdOBJ(
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YFS_ENOENT;
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp>::iterator iter;
    DiskHeatDistribution* diskheat = NULL;

    iter = mDiskHeat.find(diskkey);
    if(mDiskHeat.end() != iter)
    {
        diskheat = iter->second;
        ret = diskheat->GetNextColdOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::Destroy(DiskKey& diskkey, AdviceRule& rule)
{
    ywb_status_t ret = YFS_ENOENT;
    DiskHeatDistribution* distribution = NULL;
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp>::iterator iter;

    iter = mDiskHeat.find(diskkey);
    if(iter != mDiskHeat.end())
    {
        distribution = iter->second;
        ret = distribution->Destroy(rule);
    }

    return ret;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::Destroy(DiskKey& diskkey)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskHeatDistribution* distribution = NULL;
    list<DiskKey>* disklist = NULL;
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp>::iterator iter1;
    map<AdviceRule, list<DiskKey>*>::iterator iter2;

    iter1 = mDiskHeat.find(diskkey);
    if(iter1 != mDiskHeat.end())
    {
        distribution = iter1->second;
        mDiskHeat.erase(iter1);
        delete distribution;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    iter2 = mMostUtilizedDisks.begin();
    for(; iter2 != mMostUtilizedDisks.end(); iter2++)
    {
        disklist = iter2->second;
        disklist->remove(diskkey);
    }

    iter2 = mLeastUtilizedDisks.begin();
    for(; iter2 != mLeastUtilizedDisks.end(); iter2++)
    {
        disklist = iter2->second;
        disklist->remove(diskkey);
    }

    return ret;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::Destroy(AdviceRule& rule)
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskHeatDistribution* distribution = NULL;
    list<DiskKey>* disklist = NULL;
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp>::iterator iter1;
    map<AdviceRule, list<DiskKey>*, AdviceRuleCmp>::iterator iter2;

    iter1 = mDiskHeat.begin();
    for(; iter1 != mDiskHeat.end(); iter1++)
    {
        distribution = iter1->second;
        distribution->Destroy(rule);
    }

    iter2 = mMostUtilizedDisks.find(rule);
    if(iter2 != mMostUtilizedDisks.end())
    {
        disklist = iter2->second;
        mMostUtilizedDisks.erase(iter2);
        disklist->clear();
        delete disklist;
    }
    else
    {
        return YFS_ENOENT;
    }

    iter2 = mLeastUtilizedDisks.find(rule);
    YWB_ASSERT(iter2 != mLeastUtilizedDisks.end());
    disklist = iter2->second;
    mLeastUtilizedDisks.erase(iter2);
    disklist->clear();
    delete disklist;

    return ret;
}

ywb_status_t
TierInternalDiskWiseHeatDistribution::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    DiskHeatDistribution* distribution = NULL;
    list<DiskKey>* disklist = NULL;
    map<DiskKey, DiskHeatDistribution*, DiskKeyCmp>::iterator iter1;
    map<AdviceRule, list<DiskKey>*, AdviceRuleCmp>::iterator iter2;

    iter1 = mDiskHeat.begin();
    for(; iter1 != mDiskHeat.end(); )
    {
        distribution = iter1->second;
        mDiskHeat.erase(iter1++);
        delete distribution;
    }

    iter2 = mMostUtilizedDisks.begin();
    for(; iter2 != mMostUtilizedDisks.end(); )
    {
        disklist = iter2->second;
        mMostUtilizedDisks.erase(iter2++);
        disklist->clear();
        delete disklist;
    }

    iter2 = mLeastUtilizedDisks.begin();
    for(; iter2 != mLeastUtilizedDisks.end(); )
    {
        disklist = iter2->second;
        mLeastUtilizedDisks.erase(iter2++);
        disklist->clear();
        delete disklist;
    }

    if(mTier)
    {
        mTier->DecRef();
        mTier = NULL;
    }

    mSubPoolHeat = NULL;
    return ret;
}

ywb_status_t
TierWiseHeatDistribution::GetFirstDisk(Disk **disk)
{
    ywb_status_t ret = YFS_ENOENT;
    Tier* tier = NULL;

    *disk = NULL;
    GetTier(&tier);
    if(tier != NULL)
    {
        ret = tier->GetFirstDisk(disk);
    }

    return ret;
}

ywb_status_t
TierWiseHeatDistribution::GetNextDisk(Disk **disk)
{
    ywb_status_t ret = YFS_ENOENT;
    Tier* tier = NULL;

    GetTier(&tier);
    if(tier != NULL)
    {
        ret = tier->GetNextDisk(disk);
    }
    else
    {
        *disk = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
TierWiseHeatDistribution::GetFirstOBJ(OBJVal **obj)
{
    ywb_status_t ret = YFS_ENOENT;
    Tier* tier = NULL;

    *obj = NULL;
    GetTier(&tier);
    if(tier != NULL)
    {
        ret = tier->GetFirstOBJ(obj);
    }

    return ret;
}

ywb_status_t
TierWiseHeatDistribution::GetNextOBJ(OBJVal **obj)
{
    ywb_status_t ret = YFS_ENOENT;
    Tier* tier = NULL;

    *obj = NULL;
    GetTier(&tier);
    if(tier != NULL)
    {
        ret = tier->GetNextOBJ(obj);
    }

    return ret;
}

ywb_status_t
TierWiseHeatDistribution::PreSetupCheck()
{
    ywb_status_t ret = YWB_SUCCESS;
    Tier* tier = NULL;

    GetTier(&tier);
    /*tier has no disk*/
    if((tier->TestFlag(Tier::TF_no_disk)) ||
            (0 == tier->GetDiskCnt()))
    {
        tier->SetFlagComb(Tier::TF_no_disk);
        return YFS_ENODATA;
    }

    /*tier has no OBJ*/
    if((tier->TestFlag(Tier::TF_no_obj)) ||
            (0 == tier->GetOBJCnt()))
    {
        tier->SetFlagComb(Tier::TF_no_obj);
        return YFS_ENODATA;
    }

    return ret;
}

void
TierWiseHeatDistribution::PutDisk(Disk* disk)
{
    mTier->PutDisk(disk);
}

void
TierWiseHeatDistribution::PutOBJ(OBJVal* obj)
{
    mTier->PutOBJ(obj);
}

ywb_status_t
SubPoolHeatDistribution::SetupTierWiseHeatDistribution(ywb_uint32_t tiertype,
        AdviceRule& rule, ywb_uint32_t sortnumaboveboundary,
        ywb_uint32_t sortnumbelowboundary)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierWiseHeatDistribution* heatdistribution = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    ywb_bool_t newtierheatdistribution = ywb_false;

    ast_log_debug("Setup tier wise heat distribution, tier: " << tiertype);
    subpool = mSubPool;
    ret = subpool->GetTier(tiertype, &tier);
    if((ret != YWB_SUCCESS))
    {
        return ret;
    }

    ret = GetTierWiseHeatDistribution(tiertype, &heatdistribution);
    if(YFS_ENOENT == ret)
    {
        heatdistribution = new TierWiseHeatDistribution(tier, this);
        if(NULL == heatdistribution)
        {
            subpool->PutTier(tier);
            return YFS_EOUTOFMEMORY;
        }

        newtierheatdistribution = ywb_true;
    }

    subpool->PutTier(tier);
    ret = heatdistribution->Setup(rule,
            sortnumaboveboundary, sortnumbelowboundary);

    if(YWB_SUCCESS == ret && ywb_true == newtierheatdistribution)
    {
        mTierHeat[tiertype] = heatdistribution;
    }
    else if(ywb_true == newtierheatdistribution)
    {
        delete heatdistribution;
        heatdistribution = NULL;
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::SetupTierInternalDiskWiseHeatDistribution(
        ywb_uint32_t tier, AdviceRule& rule,
        ywb_uint32_t sortnumaboveboundary,
        ywb_uint32_t sortnumbelowboundary)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
SubPoolHeatDistribution::SetupTierInternalDiskWiseHeatDistribution(
        ywb_uint32_t tiertype, DiskKey& diskkey, AdviceRule& rule,
        ywb_uint32_t sortnumaboveboundary, ywb_uint32_t sortnumbelowboundary)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierInternalDiskWiseHeatDistribution* tierinternalheat = NULL;
    SubPool* subpool = NULL;
    Tier* tier = NULL;
    ywb_bool_t newtierinternal = ywb_false;

    ast_log_debug("Setup tier-disk wise heat distribution, tier: "
            << tiertype << ", disk: [" << diskkey.GetSubPoolId()
            << ", " << diskkey.GetDiskId() << "]");

    subpool = mSubPool;
    ret = subpool->GetTier(tiertype, &tier);
    if((ret != YWB_SUCCESS))
    {
        return ret;
    }

    ret = GetTierInternalDiskWiseHeatDistribution(tiertype, &tierinternalheat);
    if(YFS_ENOENT == ret)
    {
        tierinternalheat = new TierInternalDiskWiseHeatDistribution(tier, this);
        if(NULL == tierinternalheat)
        {
            subpool->PutTier(tier);
            return YFS_EOUTOFMEMORY;
        }

        newtierinternal = ywb_true;
    }

    subpool->PutTier(tier);
    ret = tierinternalheat->SetupDiskHeatDistribution(diskkey, rule,
            sortnumaboveboundary, sortnumbelowboundary);

    if(YWB_SUCCESS == ret && 1 == newtierinternal)
    {
        mTierDiskHeat[tiertype] = tierinternalheat;
    }
    else if(1 == newtierinternal)
    {
        delete tierinternalheat;
        tierinternalheat = NULL;
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::SetupDiskWiseHeatDistribution(
        DiskKey& diskkey, AdviceRule& rule,
        ywb_uint32_t sortnumaboveboundary,
        ywb_uint32_t sortnumbelowboundary)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseHeatDistribution(
        ywb_uint32_t tier,
        TierInternalDiskWiseHeatDistribution** heatdistribution)
{
    ywb_status_t ret = YWB_SUCCESS;

    *heatdistribution = mTierDiskHeat[tier];
    if(NULL == mTierDiskHeat[tier])
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierWiseHeatDistribution(ywb_uint32_t tier,
        TierWiseHeatDistribution** heatdistribution)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(tier < Tier::TIER_0 || tier > Tier::TIER_cnt)
    {
        ret = YFS_ENOENT;
    }
    else
    {
        *heatdistribution = mTierHeat[tier];
        if(NULL == mTierHeat[tier])
        {
            ret = YFS_ENOENT;
        }
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetDiskWiseHeatDistribution(
        DiskKey& diskkey, DiskHeatDistribution** diskheatdistribution)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
SubPoolHeatDistribution::GetTierWiseFirstHotOBJ(
        ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierWiseHeatDistribution* heatdistribution = NULL;

    heatdistribution = mTierHeat[tier];
    if(NULL == mTierHeat[tier])
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = heatdistribution->GetFirstHotOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierWiseNextHotOBJ(
        ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierWiseHeatDistribution* heatdistribution = NULL;

    heatdistribution = mTierHeat[tier];
    if(NULL == mTierHeat[tier])
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = heatdistribution->GetNextHotOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierWiseFirstColdOBJ(
        ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierWiseHeatDistribution* heatdistribution = NULL;

    heatdistribution = mTierHeat[tier];
    if(NULL == mTierHeat[tier])
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = heatdistribution->GetFirstColdOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierWiseNextColdOBJ(
        ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierWiseHeatDistribution* heatdistribution = NULL;

    heatdistribution = mTierHeat[tier];
    if(NULL == mTierHeat[tier])
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = heatdistribution->GetNextColdOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseFirstHotOBJ(
        ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierInternalDiskWiseHeatDistribution* heatdistribution = NULL;

    heatdistribution = mTierDiskHeat[tier];
    if(NULL == mTierDiskHeat[tier])
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = heatdistribution->GetFirstHotOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseNextHotOBJ(
        ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierInternalDiskWiseHeatDistribution* heatdistribution = NULL;

    heatdistribution = mTierDiskHeat[tier];
    if(NULL == mTierDiskHeat[tier])
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = heatdistribution->GetNextHotOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseFirstColdOBJ(
        ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierInternalDiskWiseHeatDistribution* heatdistribution = NULL;

    heatdistribution = mTierDiskHeat[tier];
    if(NULL == mTierDiskHeat[tier])
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = heatdistribution->GetFirstColdOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseNextColdOBJ(
        ywb_uint32_t tier, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YWB_SUCCESS;
    TierInternalDiskWiseHeatDistribution* heatdistribution = NULL;

    heatdistribution = mTierDiskHeat[tier];
    if(NULL == mTierDiskHeat[tier])
    {
        *obj = NULL;
        ret = YFS_ENOENT;
    }
    else
    {
        ret = heatdistribution->GetNextColdOBJ(rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseFirstHotOBJ(ywb_uint32_t tier,
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YFS_ENOENT;
    TierInternalDiskWiseHeatDistribution* tierinternalheat = NULL;
    /*DiskHeatDistribution* diskheat = NULL;*/

    *obj = NULL;
    tierinternalheat = mTierDiskHeat[tier];
    if(NULL != mTierDiskHeat[tier])
    {
#if 0
        ret = tierinternalheat->GetDiskHeatDistribution(diskkey, &diskheat);
        if(YWB_SUCCESS == ret)
        {
            ret = diskheat->GetFirstHotOBJ(rule, obj);
        }
#endif
        ret = tierinternalheat->GetFirstHotOBJ(diskkey, rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseNextHotOBJ(ywb_uint32_t tier,
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YFS_ENOENT;
    TierInternalDiskWiseHeatDistribution* tierinternalheat = NULL;
    /*DiskHeatDistribution* diskheat = NULL;*/

    *obj = NULL;
    tierinternalheat = mTierDiskHeat[tier];
    if(NULL != mTierDiskHeat[tier])
    {
#if 0
        ret = tierinternalheat->GetDiskHeatDistribution(diskkey, &diskheat);
        if(YWB_SUCCESS == ret)
        {
            ret = diskheat->GetNextHotOBJ(rule, obj);
        }
#endif
        ret = tierinternalheat->GetNextHotOBJ(diskkey, rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseFirstColdOBJ(ywb_uint32_t tier,
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YFS_ENOENT;
    TierInternalDiskWiseHeatDistribution* tierinternalheat = NULL;
    /*DiskHeatDistribution* diskheat = NULL;*/

    *obj = NULL;
    tierinternalheat = mTierDiskHeat[tier];
    if(NULL != mTierDiskHeat[tier])
    {
#if 0
        ret = tierinternalheat->GetDiskHeatDistribution(diskkey, &diskheat);
        if(YWB_SUCCESS == ret)
        {
            ret = diskheat->GetFirstColdOBJ(rule, obj);
        }
#endif
        ret = tierinternalheat->GetFirstColdOBJ(diskkey, rule, obj);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseNextColdOBJ(ywb_uint32_t tier,
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    ywb_status_t ret = YFS_ENOENT;
    TierInternalDiskWiseHeatDistribution* tierinternalheat = NULL;
    /*DiskHeatDistribution* diskheat = NULL;*/

    *obj = NULL;
    tierinternalheat = mTierDiskHeat[tier];
    if(NULL != mTierDiskHeat[tier])
    {
#if 0
        ret = tierinternalheat->GetDiskHeatDistribution(diskkey, &diskheat);
        if(YWB_SUCCESS == ret)
        {
            ret = diskheat->GetNextColdOBJ(rule, obj);
        }
#endif
        ret = tierinternalheat->GetNextColdOBJ(diskkey, rule, obj);
    }

    return ret;
}

/*get disk wise first/next hot/cod OBJ*/
ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseFirstHotOBJ(
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseNextHotOBJ(
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseFirstColdOBJ(
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
SubPoolHeatDistribution::GetTierInternalDiskWiseNextColdOBJ(
        DiskKey& diskkey, AdviceRule& rule, OBJVal** obj)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
SubPoolHeatDistribution::DestroyTierWiseHeatDistribution(
            ywb_uint32_t tier, AdviceRule& rule)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mTierHeat[tier] != NULL)
    {
        ret = mTierHeat[tier]->Destroy(rule);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::DestroyTierWiseHeatDistribution(
        ywb_uint32_t tier)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mTierHeat[tier] != NULL)
    {
        delete mTierHeat[tier];
        mTierHeat[tier] = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::DestroyTierInternalDiskWiseHeatDistribution(
        ywb_uint32_t tier, AdviceRule& rule)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mTierDiskHeat[tier] != NULL)
    {
        ret = mTierDiskHeat[tier]->Destroy(rule);
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::DestroyTierInternalDiskWiseHeatDistribution(
        ywb_uint32_t tier)
{
    ywb_status_t ret = YWB_SUCCESS;

    if(mTierDiskHeat[tier] != NULL)
    {
        delete mTierDiskHeat[tier];
        mTierDiskHeat[tier] = NULL;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::DestroyTierInternalDiskWiseHeatDistribution(
        ywb_uint32_t tier, DiskKey& diskkey, AdviceRule& rule)
{
    ywb_status_t ret = YFS_ENOENT;

    if(mTierDiskHeat[tier] != NULL)
    {
        ret = mTierDiskHeat[tier]->Destroy(diskkey, rule);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::DestroyTierInternalDiskWiseHeatDistribution(
        ywb_uint32_t tier, DiskKey& diskkey)
{
    ywb_status_t ret = YFS_ENOENT;

    if(mTierDiskHeat[tier] != NULL)
    {
        ret = mTierDiskHeat[tier]->Destroy(diskkey);
    }

    return ret;
}

ywb_status_t
SubPoolHeatDistribution::DestroyDiskWiseHeatDistribution(
        DiskKey& diskkey, AdviceRule& rule)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
SubPoolHeatDistribution::DestroyDiskWiseHeatDistribution(DiskKey& diskkey)
{
    return YFS_ENOTIMPL;
}

ywb_status_t
SubPoolHeatDistribution::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t tier = 0;

    for(; tier < Constant::TIER_NUM; tier++)
    {
        if(mTierDiskHeat[tier] != NULL)
        {
            delete mTierDiskHeat[tier];
            mTierDiskHeat[tier] = NULL;
        }

        if(mTierHeat[tier] != NULL)
        {
            delete mTierHeat[tier];
            mTierHeat[tier] = NULL;
        }
    }

    if(mSubPool)
    {
        mSubPool->DecRef();
        mSubPool = NULL;
    }

    mHeatManager = NULL;
    return ret;
}

ywb_status_t
HeatDistributionManager::RegisterGreater(
        ywb_uint32_t sortbase, HeatCmp::Greater greater)
{
    ywb_status_t ret = YWB_SUCCESS;

    mGreaters.insert(make_pair(sortbase, greater));
    return ret;
}

ywb_status_t
HeatDistributionManager::RegisterLess(
        ywb_uint32_t sortbase, HeatCmp::Less less)
{
    ywb_status_t ret = YWB_SUCCESS;

    mLesses.insert(make_pair(sortbase, less));
    return ret;
}

ywb_status_t
HeatDistributionManager::GetGreater(
        ywb_uint32_t sortbase, HeatCmp::Greater* greater)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ywb_uint32_t, HeatCmp::Greater>::iterator iter;

    iter = mGreaters.find(sortbase);
    if(iter != mGreaters.end())
    {
        *greater = iter->second;
    }
    else
    {
        *greater = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistributionManager::GetLess(
        ywb_uint32_t sortbase, HeatCmp::Less* less)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ywb_uint32_t, HeatCmp::Less>::iterator iter;

    iter = mLesses.find(sortbase);
    if(iter != mLesses.end())
    {
        *less = iter->second;
    }
    else
    {
        *less = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistributionManager::Initialize()
{
    ywb_status_t ret = YWB_SUCCESS;
    ywb_uint32_t sortbase = 0;

    /*random*/
    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_random | SortBase::SB_iops);
    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMARndIOs);
    RegisterLess(sortbase, HeatCmp::LessByLongTermEMARndIOs);

    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_random | SortBase::SB_bw);
    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMARndBW);
    RegisterLess(sortbase, HeatCmp::LessByLongTermEMARndBW);

    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_random | SortBase::SB_rt);
    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMARndRT);
    RegisterLess(sortbase, HeatCmp::LessByLongTermEMARndRT);

    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_random | SortBase::SB_iops);
    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMARndIOs);
    RegisterLess(sortbase, HeatCmp::LessByShortTermEMARndIOs);

    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_random | SortBase::SB_bw);
    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMARndBW);
    RegisterLess(sortbase, HeatCmp::LessByShortTermEMARndBW);

    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_random | SortBase::SB_rt);
    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMARndRT);
    RegisterLess(sortbase, HeatCmp::LessByShortTermEMARndRT);

    sortbase = (SortBase::SB_raw | SortBase::SB_random |
            SortBase::SB_iops);
    RegisterGreater(sortbase, HeatCmp::GreaterByRawRndIOs);
    RegisterLess(sortbase, HeatCmp::LessByRawRndIOs);

    sortbase = (SortBase::SB_raw | SortBase::SB_random |
            SortBase::SB_bw);
    RegisterGreater(sortbase, HeatCmp::GreaterByRawRndBW);
    RegisterLess(sortbase, HeatCmp::LessByRawRndBW);

    sortbase = (SortBase::SB_raw | SortBase::SB_random |
            SortBase::SB_rt);
    RegisterGreater(sortbase, HeatCmp::GreaterByRawRndRT);
    RegisterLess(sortbase, HeatCmp::LessByRawRndRT);


    /*sequential*/
    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_sequential | SortBase::SB_iops);
    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMASeqIOs);
    RegisterLess(sortbase, HeatCmp::LessByLongTermEMASeqIOs);

    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_sequential | SortBase::SB_bw);
    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMASeqBW);
    RegisterLess(sortbase, HeatCmp::LessByLongTermEMASeqBW);

    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_sequential | SortBase::SB_rt);
    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMASeqRT);
    RegisterLess(sortbase, HeatCmp::LessByLongTermEMASeqRT);

    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_sequential | SortBase::SB_iops);
    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMASeqIOs);
    RegisterLess(sortbase, HeatCmp::LessByShortTermEMASeqIOs);

    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_sequential | SortBase::SB_bw);
    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMASeqBW);
    RegisterLess(sortbase, HeatCmp::LessByShortTermEMASeqBW);

    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_sequential | SortBase::SB_rt);
    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMASeqRT);
    RegisterLess(sortbase, HeatCmp::LessByShortTermEMASeqRT);

    sortbase = (SortBase::SB_raw | SortBase::SB_sequential |
            SortBase::SB_iops);
    RegisterGreater(sortbase, HeatCmp::GreaterByRawSeqIOs);
    RegisterLess(sortbase, HeatCmp::LessByRawSeqIOs);

    sortbase = (SortBase::SB_raw | SortBase::SB_sequential |
            SortBase::SB_bw);
    RegisterGreater(sortbase, HeatCmp::GreaterByRawSeqBW);
    RegisterLess(sortbase, HeatCmp::LessByRawSeqIOs);

    sortbase = (SortBase::SB_raw | SortBase::SB_sequential |
            SortBase::SB_rt);
    RegisterGreater(sortbase, HeatCmp::GreaterByRawSeqRT);
    RegisterLess(sortbase, HeatCmp::LessByRawSeqIOs);


    /*random + sequential*/
    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_iops);
    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMAAccumIOs);
    RegisterLess(sortbase, HeatCmp::LessByLongTermEMAAccumIOs);

    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_bw);
    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMAAccumBW);
    RegisterLess(sortbase, HeatCmp::LessByLongTermEMAAccumBW);

    sortbase = (SortBase::SB_long_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_rt);
    RegisterGreater(sortbase, HeatCmp::GreaterByLongTermEMAAccumRT);
    RegisterLess(sortbase, HeatCmp::LessByLongTermEMAAccumRT);

    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_iops);
    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMAAccumIOs);
    RegisterLess(sortbase, HeatCmp::LessByShortTermEMAAccumIOs);

    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_bw);
    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMAAccumBW);
    RegisterLess(sortbase, HeatCmp::LessByShortTermEMAAccumBW);

    sortbase = (SortBase::SB_short_term | SortBase::SB_ema |
            SortBase::SB_rnd_seq | SortBase::SB_rt);
    RegisterGreater(sortbase, HeatCmp::GreaterByShortTermEMAAccumRT);
    RegisterLess(sortbase, HeatCmp::LessByShortTermEMAAccumRT);

    sortbase = (SortBase::SB_raw | SortBase::SB_rnd_seq |
            SortBase::SB_iops);
    RegisterGreater(sortbase, HeatCmp::GreaterByRawAccumIOs);
    RegisterLess(sortbase, HeatCmp::LessByRawAccumIOs);

    sortbase = (SortBase::SB_raw | SortBase::SB_rnd_seq |
            SortBase::SB_bw);
    RegisterGreater(sortbase, HeatCmp::GreaterByRawAccumBW);
    RegisterLess(sortbase, HeatCmp::LessByRawAccumBW);

    sortbase = (SortBase::SB_raw | SortBase::SB_rnd_seq |
            SortBase::SB_rt);
    RegisterGreater(sortbase, HeatCmp::GreaterByRawAccumRT);
    RegisterLess(sortbase, HeatCmp::LessByRawAccumRT);

    return ret;
}

ywb_status_t
HeatDistributionManager::SetupSubPoolHeatDistribution(SubPoolKey& key,
        ywb_uint32_t tier, AdviceRule& rule,
        ywb_uint32_t sortnumaboveboundary,
        ywb_uint32_t sortnumbelowboundary)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolHeatDistribution* subpoolheat = NULL;

    ast_log_debug("Setup subpool-tier wise heat distribution, subpool: ["
            << "" << key.GetOss()
            << ", " << key.GetPoolId()
            << ", " << key.GetSubPoolId()
            << "], tier " << tier );

    ret = GetSubPoolHeatDistribution(key, &subpoolheat);
    YWB_ASSERT((YWB_SUCCESS == ret) && (subpoolheat != NULL));

    if(PartitionScope::PS_tier_wise == rule.GetPartitionScope())
    {
        ret = subpoolheat->SetupTierWiseHeatDistribution(tier, rule,
                sortnumaboveboundary, sortnumbelowboundary);
    }
    else if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
    {
        ret = subpoolheat->SetupTierInternalDiskWiseHeatDistribution(tier,
                rule, sortnumaboveboundary, sortnumbelowboundary);
    }

    return ret;
}

ywb_status_t
HeatDistributionManager::SetupSubPoolHeatDistribution(SubPoolKey& subpoolkey,
        ywb_uint32_t tier, DiskKey& diskkey, AdviceRule& rule,
        ywb_uint32_t sortnumaboveboundary, ywb_uint32_t sortnumbelowboundary)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolHeatDistribution* subpoolheat = NULL;

    ast_log_debug("Setup subpool-tier-disk wise heat distribution, subpool: ["
            << "" << subpoolkey.GetOss()
            << ", " << subpoolkey.GetPoolId()
            << ", " << subpoolkey.GetSubPoolId()
            << "], tier " << tier
            << "disk: [" << diskkey.GetSubPoolId()
            << ", " << diskkey.GetDiskId() << "]");

    ret = GetSubPoolHeatDistribution(subpoolkey, &subpoolheat);
    YWB_ASSERT((YWB_SUCCESS == ret) && (subpoolheat != NULL));

    if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
    {
        ret = subpoolheat->SetupTierInternalDiskWiseHeatDistribution(tier,
                diskkey, rule, sortnumaboveboundary, sortnumbelowboundary);
    }

    return ret;
}

ywb_status_t
HeatDistributionManager::SetupSubPoolHeatDistribution(
        SubPoolKey& subpoolkey, DiskKey& diskkey, AdviceRule& rule,
        ywb_uint32_t sortnumaboveboundary, ywb_uint32_t sortnumbelowboundary)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolHeatDistribution* subpoolheat = NULL;

    ast_log_debug("Setup subpool-disk wise heat distribution, subpool: ["
            << "" << subpoolkey.GetOss()
            << ", " << subpoolkey.GetPoolId()
            << ", " << subpoolkey.GetSubPoolId()
            << "], disk: [" << diskkey.GetSubPoolId()
            << ", " << diskkey.GetDiskId() << "]");

    ret = GetSubPoolHeatDistribution(subpoolkey, &subpoolheat);
    YWB_ASSERT((YWB_SUCCESS == ret) && (subpoolheat != NULL));

    if(PartitionScope::PS_tier_internal_disk_wise == rule.GetPartitionScope())
    {
        ret = subpoolheat->SetupDiskWiseHeatDistribution(diskkey, rule,
                sortnumaboveboundary, sortnumbelowboundary);
    }

    return ret;
}

ywb_status_t
HeatDistributionManager::GetSubPoolHeatDistribution(
        SubPoolKey& key, SubPoolHeatDistribution** subpoolheat)
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolHeatDistribution*, SubPoolKeyCmp>::iterator iter;

    iter = mSubPools.find(key);
    if(mSubPools.end() != iter)
    {
        *subpoolheat = iter->second;
    }
    else
    {
        *subpoolheat = NULL;
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistributionManager::DestroySubPoolHeatDistribution(SubPoolKey& key)
{
    ywb_status_t ret = YWB_SUCCESS;
    SubPoolHeatDistribution* subpoolheat = NULL;
    map<SubPoolKey, SubPoolHeatDistribution*, SubPoolKeyCmp>::iterator iter;

    iter = mSubPools.find(key);
    if(mSubPools.end() != iter)
    {
        subpoolheat = iter->second;
        mSubPools.erase(iter);
        delete subpoolheat;
    }
    else
    {
        ret = YFS_ENOENT;
    }

    return ret;
}

ywb_status_t
HeatDistributionManager::Destroy()
{
    ywb_status_t ret = YWB_SUCCESS;
    map<ywb_uint32_t, HeatCmp::Greater>::iterator mIt;
    map<ywb_uint32_t, HeatCmp::Less>::iterator mIt2;

    Reset();
    mIt = mGreaters.begin();
    for(; mIt != mGreaters.end(); )
    {
        mGreaters.erase(mIt++);
    }

    mIt2 = mLesses.begin();
    for(; mIt2 != mLesses.end(); )
    {
        mLesses.erase(mIt2++);
    }

    mAst = NULL;
    return ret;
}

ywb_status_t
HeatDistributionManager::Reset()
{
    ywb_status_t ret = YWB_SUCCESS;
    map<SubPoolKey, SubPoolHeatDistribution*, SubPoolKeyCmp>::iterator iter;
    SubPoolHeatDistribution* subpoolheat = NULL;
    SubPoolKey subpoolkey;

    ast_log_debug("HeatDistributionManager reset");
    iter = mSubPools.begin();
    for(; iter != mSubPools.end(); )
    {
        subpoolkey = iter->first;
        ast_log_debug("Destroy heat distribution of subpool: ["
                << subpoolkey.GetOss()
                << ", " << subpoolkey.GetPoolId()
                << ", " << subpoolkey.GetSubPoolId()
                << "]");
        subpoolheat = iter->second;
        mSubPools.erase(iter++);
        delete subpoolheat;
        subpoolheat = NULL;
    }

    return ret;
}

ywb_status_t
HeatDistributionManager::Init()
{
    ywb_status_t ret = YWB_SUCCESS;
    AST* ast = NULL;
    LogicalConfig* config = NULL;
    SubPoolKey subpoolkey;
    SubPool* subpool = NULL;
    SubPoolHeatDistribution* subpoolheat = NULL;
    pair<map<SubPoolKey, SubPoolHeatDistribution*>::iterator, bool> pairret;

    GetAST(&ast);
    ast->GetLogicalConfig(&config);

    ret = config->GetFirstSubPool(&subpool, &subpoolkey);
    while((YWB_SUCCESS == ret) && (subpool != NULL))
    {
        subpoolheat = new SubPoolHeatDistribution(subpool, this);
        if(NULL == subpoolheat)
        {
            ast_log_debug("Out of memory!");
            config->PutSubPool(subpool);
            ret = YFS_EOUTOFMEMORY;
            return ret;
        }
        else
        {
            pairret = mSubPools.insert(make_pair(subpoolkey, subpoolheat));
            YWB_ASSERT(pairret.second);
        }

        config->PutSubPool(subpool);
        ret = config->GetNextSubPool(&subpool, &subpoolkey);
    }

    return ret;
}

/* vim:set ts=4 sw=4 expandtab */
