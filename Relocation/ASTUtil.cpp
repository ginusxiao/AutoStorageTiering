#include "AST/ASTUtil.hpp"


void Flag::SetFlagComb(ywb_uint32_t bit)
{
    return Bitset::SetFlag(bit);
}

void Flag::SetFlagUniq(ywb_uint32_t bit)
{
    Bitset::Reset();
    return Bitset::SetFlag(bit);
}

/* vim:set ts=4 sw=4 expandtab */
