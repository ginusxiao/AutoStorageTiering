#ifndef __AST_LOGGER_HPP__
#define __AST_LOGGER_HPP__

#include "common/FsStatus.h"

/*
 * DO NOT delete this file for AST should have its own log
 * infrastructure if it is independent module in the long run,
 * besides, it acts as log utility for AST test.
 * */
#ifdef WITH_AST_OWN_LOG
#include <syslog.h>
#include <stdarg.h>
#include <string>
#include <ostream>
#include <sstream>
#include "FsCommon/FsLogger.hpp"

extern YwuLogger *g_ast_logger;

#define AST_LOGGROUP_NAMES \
    {\
        YFS_LOGGROUP_NAMES, \
        "AST_default",  \
        "AST_config", \
        "AST_time", \
        "AST_diskprofile", \
        "AST_relocation", \
        "AST_logicalconfig", \
        "AST_IOstat", \
        "AST_heatdistribution", \
        "AST_advice", \
        "AST_diskperf", \
        "AST_plan", \
        "AST_arb", \
        "AST_mig", \
        "AST_message", \
        "AST_ast", \
    }

enum e_ASTLogGroups {
    ASTLG_default = YLG_first_program,
    ASTLG_config,
    ASTLG_time,
    ASTLG_diskprofile,
    ASTLG_relocation,
    ASTLG_logicalconfig,
    ASTLG_iostat,
    ASTLG_heatdistribution,
    ASTLG_advice,
    ASTLG_diskperf,
    ASTLG_plan,
    ASTLG_arb,
    ASTLG_mig,
    ASTLG_message,
    ASTLG_ast,
    ASTLG_logend,
};

class ControlCenter;

class ASTLogger {
public:
    ASTLogger():
        mCenter(NULL),
        mLoggerHandle(NULL),
        mSyslogEnabled(false),
        mName(""),
        mOption(0),
        mFacility(0),
        mLogLevel(1)
    { }

    ~ASTLogger()
    {
        Finalize();
    }

public:
    int Initialize(ControlCenter *center, bool using_syslog,
                   const char *name, int opt, int facility);
    int Finalize();

    inline YwuLogger* GetLoggerHandle();
    inline void SetLoggerHandle(YwuLogger* handle);
    inline int GetLogLevel() { return mLogLevel; }
    inline void SetLogLevel(int level) { mLogLevel = level; }
    void LogTrace(unsigned int group, const std::ostringstream &oss);
    void LogDebug(unsigned int group, const std::ostringstream &oss);
    void LogInfo(unsigned int group, const std::ostringstream &oss);
    void LogWarn(unsigned int group, const std::ostringstream &oss);
    void LogError(unsigned int group, const std::ostringstream &oss);
    void LogFatal(unsigned int group, const std::ostringstream &oss);
    void Syslog(int priority, const std::ostringstream &oss);

public:
    static ASTLogger &Instance();

private:
    ControlCenter *mCenter;
    YwuLogger* mLoggerHandle;
    bool mSyslogEnabled;
    std::string mName;
    int mOption;
    int mFacility;
    int mLogLevel;

private:
    ASTLogger(const ASTLogger &right);
    ASTLogger &operator=(ASTLogger &right);
};

#define _ast_log_syslog(priority, log)\
    do { \
        std::ostringstream  temp_log_sstream; \
        temp_log_sstream << log << "[" <<  __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "]"; \
        ASTLogger::Instance().Syslog(priority, temp_log_sstream); \
    } while(0)

#define _ast_log_trace(grp, log)\
    do { \
        std::ostringstream  temp_log_sstream; \
        temp_log_sstream << log << "[" <<  __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "]"; \
        ASTLogger::Instance().LogTrace(grp, temp_log_sstream); \
    } while(0)

#define _ast_log_debug(grp, log)\
    do { \
        std::ostringstream  temp_log_sstream; \
        temp_log_sstream << log << "[" <<  __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "]"; \
        ASTLogger::Instance().LogDebug(grp, temp_log_sstream); \
    } while (0)

#define _ast_log_info(grp, log)\
    do { \
        std::ostringstream  temp_log_sstream; \
        temp_log_sstream << log << "[" <<  __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "]"; \
        ASTLogger::Instance().LogInfo(grp, temp_log_sstream); \
    } while (0)

#define _ast_log_warn(grp, log)\
    do { \
        std::ostringstream  temp_log_sstream; \
        temp_log_sstream << log << "[" <<  __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "]"; \
        ASTLogger::Instance().LogWarn(grp, temp_log_sstream); \
    } while (0)

#define _ast_log_error(grp, log)\
    do { \
        std::ostringstream  temp_log_sstream; \
        temp_log_sstream << log << "[" <<  __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "]"; \
        ASTLogger::Instance().LogError(grp, temp_log_sstream); \
    } while (0)

#define _ast_log_fatal(grp, log)\
    do { \
        std::ostringstream  temp_log_sstream; \
        temp_log_sstream << log << "[" <<  __FUNCTION__ << ":" << __FILE__ << ":" << __LINE__ << "]"; \
        ASTLogger::Instance().LogFatal(grp, temp_log_sstream); \
    } while (0)

/*AST related log macros*/
#define ast_log_trace(msg)\
    _ast_log_trace(ASTLG_ast, msg)

#define ast_log_debug(msg)\
    _ast_log_debug(ASTLG_ast, msg)

#define ast_log_info(msg)\
    _ast_log_info(ASTLG_ast, msg)

#define ast_log_warn(msg)\
    _ast_log_warn(ASTLG_ast, msg)

#define ast_log_error(msg)\
    _ast_log_error(ASTLG_ast, msg)

#define ast_log_fatal(msg)\
    _ast_log_fatal(ASTLG_ast, msg)

#else

#endif

ywb_status_t create_logger(std::string logfile,
        std::string loglevel,
        ywb_bool_t ignoregyfslogger = false);

#endif

/* vim:set ts=4 sw=4 expandtab */
