#ifndef __AST_UTIL_HPP__
#define __AST_UTIL_HPP__

#include "common/FsInts.h"
#include "OSSCommon/Bitset.hpp"

class Flag : public Bitset
{
public:
    Flag()
    {
        Bitset::Reset();
    }

    ~Flag() {}

    /*combinational bit operation, do not clear previously set bit*/
    void SetFlagComb(ywb_uint32_t bit);
    /*clear any previously set bit and set this new bit*/
    void SetFlagUniq(ywb_uint32_t bit);
};

#endif

/* vim:set ts=4 sw=4 expandtab */
