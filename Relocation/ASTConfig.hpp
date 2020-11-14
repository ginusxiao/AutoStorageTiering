#ifndef __AST_CONFIG_HPP__
#define __AST_CONFIG_HPP__

#include <string>
#include "common/FsStatus.h"
#include "common/FsInts.h"
#include "AST/ASTConstant.hpp"
#include "AST/AST.hpp"

using namespace std;

class ConfigManager{
public:
    ConfigManager(AST* ast) : mCycleInterval(Constant::CYCLE_PERIOD),
    mShortTermWindowCycles(Constant::SHORT_TERM_DECISION_WINDOW),
    mLongTermWindowCycles(Constant::LONG_TERM_DECISION_WINDOW),
    mDiskProfile(Constant::DISK_PROFILE_PATH), mAST(ast) {}

    virtual ~ConfigManager()
    {
        mAST = NULL;
    }

    void GetAST(AST** ast)
    {
        *ast = mAST;
    }

    inline ywb_uint32_t GetCycleInterval()
    {
        return mCycleInterval;
    }

    inline ywb_uint32_t GetShortTermWindowCycles()
    {
        return mShortTermWindowCycles;
    }

    inline ywb_uint32_t GetLongTermWindowCycles()
    {
        return mLongTermWindowCycles;
    }

    inline void GetDiskProfile(string** path)
    {
        *path = &mDiskProfile;
    }

    inline void SetCycleInterval(ywb_uint32_t value)
    {
        mCycleInterval = value;
    }

    inline void SetShortTermWindowCycles(ywb_uint32_t value)
    {
        mShortTermWindowCycles = value;
    }

    inline void SetLongTermWindowCycles(ywb_uint32_t value)
    {
        mLongTermWindowCycles = value;
    }

    inline void SetDiskProfile(string& path)
    {
        YWB_ASSERT(path.size() < Constant::PATH_SIZE_MAX);
        mDiskProfile.assign(path);
    }

    /*Parse the CLI options passed by monitor*/
    ywb_status_t ConfigParseFromOptions(const string& options);
    /*Parse the options in config file*/
    ywb_status_t ConfigParseFromFile(const string& fileName);
    /*destroy config*/
    ywb_status_t Destroy();
    /*DO NOT reset it to default, but follow the value previously set instead*/
    void Reset();

public:
    /*all configurable values*/
    ywb_uint32_t    mCycleInterval;
    ywb_uint32_t    mShortTermWindowCycles;
    ywb_uint32_t    mLongTermWindowCycles;
    /*path for disk performance profile*/
    string  mDiskProfile;
    /*reference to AST*/
    AST* mAST;
};

#endif

/* vim:set ts=4 sw=4 expandtab */
