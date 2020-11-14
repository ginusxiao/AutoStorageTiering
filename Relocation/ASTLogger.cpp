#include "FsLogger.hpp"
#include "AST/ASTLogger.hpp"

#ifdef WITH_AST_OWN_LOG
const char* g_Log_Group_Names[] = AST_LOGGROUP_NAMES;
YwuLogger *g_ast_logger = NULL;
extern YwuLogger* g_yfs_logger;

ASTLogger &
ASTLogger::Instance()
{
    /*singleton*/
    static ASTLogger __singleton_logger;
    return __singleton_logger;
}

int
ASTLogger::Initialize(ControlCenter *center,
                                 bool using_syslog, const char *name,
                                 int opt, int facility)
{
    mSyslogEnabled = using_syslog;
    mName = std::string(name);
    mOption = opt;
    mFacility = facility;
    openlog(name, opt, facility);
    return 0;
}

int
ASTLogger::Finalize()
{
    mCenter = NULL;
    mLoggerHandle = NULL;
    mSyslogEnabled = false;
    closelog();
    return 0;
}

YwuLogger* ASTLogger::GetLoggerHandle()
{
    return mLoggerHandle;
}

void ASTLogger::SetLoggerHandle(YwuLogger* handle)
{
    mLoggerHandle = handle;
}

void
ASTLogger::LogTrace(unsigned int group,
                               const std::ostringstream &oss)
{
    ywu_log(GetLoggerHandle(), YLGF_trace, group, YLD_file, oss.str());
}

void
ASTLogger::LogDebug(unsigned int group,
                               const std::ostringstream &oss)
{
    ywu_log(GetLoggerHandle(), YLGF_debug, group, YLD_file, oss.str());
}

void
ASTLogger::LogInfo(unsigned int group,
                              const std::ostringstream &oss)
{
    ywu_log(GetLoggerHandle(), YLGF_info, group, YLD_file, oss.str());
}

void
ASTLogger::LogWarn(unsigned int group,
                              const std::ostringstream &oss)
{
    ywu_log(GetLoggerHandle(), YLGF_warn, group, YLD_file | YLD_stdout,
                       oss.str());

    if (mSyslogEnabled) {
        syslog(LOG_WARNING, "%s", oss.str().c_str());
    }
}

void
ASTLogger::LogError(unsigned int group,
                               const std::ostringstream &oss)
{
    ywu_log(GetLoggerHandle(), YLGF_error, group, YLD_file | YLD_stderr,
                       oss.str());

    if (mSyslogEnabled) {
        syslog(LOG_ERR, "%s", oss.str().c_str());
    }
}

void
ASTLogger::LogFatal(unsigned int group,
                               const std::ostringstream &oss)
{
    ywu_log(GetLoggerHandle(), YLGF_fatal, group, YLD_file | YLD_stderr,
                       oss.str());

    if (mSyslogEnabled) {
        syslog(LOG_CRIT, "%s", oss.str().c_str());
    }
}

void
ASTLogger::Syslog(int priority, const std::ostringstream &oss)
{
    if (mSyslogEnabled) {
        syslog(priority, "%s", oss.str().c_str());
    }
}


#if 0

#define yfs_log_trace(grp, log)\
    ywu_log(ASTLogger::Instance().GetLoggerHandle(), YLGF_trace, grp, YLD_file, log)

#define yfs_log_debug(grp, log)\
    ywu_log(ASTLogger::Instance().GetLoggerHandle(), YLGF_debug, grp, YLD_file, log)

#define yfs_log_info(grp, log)\
    ywu_log(ASTLogger::Instance().GetLoggerHandle(), YLGF_info, grp, YLD_file, log)

#define yfs_log_warn(grp, log)\
    ywu_log(ASTLogger::Instance().GetLoggerHandle(), YLGF_warn, grp, YLD_file | YLD_stdout, log)

#define yfs_log_error(grp, log)\
    ywu_log(ASTLogger::Instance().GetLoggerHandle(), YLGF_error, grp, YLD_file | YLD_stderr, log)

#define yfs_log_fatal(msg)\
    ywu_log(ASTLogger::Instance().GetLoggerHandle(), YLGF_fatal, SSLG_default, YLD_file | YLD_stderr, msg)

#endif
#else
#include "OSSCommon/Logger.hpp"
//YwuLogger *g_yfs_logger = NULL;
const char *g_ast_log_group_names[] = OSS_LOGGROUP_NAMES;
#endif

ywb_status_t
create_logger(std::string logfile,
        std::string loglevel,
        ywb_bool_t ignoregyfslogger)
{
    ywb_status_t err = YWB_SUCCESS;
    char errbuf[256];
    const char* sLogOption[] = {"+all.enable.trace.debug.warn.info.error.fatal",
            "+all.enable.debug.warn.info.error.fatal",
            "+all.enable.warn.info.error.fatal",
            "+all.enable.info.error.fatal"};
    YwuLogger *logger = NULL;
    int index = 0;

    if(loglevel == "trace"){
        index = 0;
    } else if(loglevel == "debug"){
        index = 1;
    } else if(loglevel == "warn"){
        index = 2;
    } else if(loglevel == "info"){
        index = 3;
    } else {
        index = 1;
    }

#ifdef WITH_AST_OWN_LOG
    logger = new YwuLogger();
    YWB_ASSERT(logger != NULL);
    err = logger->open(YLF_append, 104857600, 100, sLogOption[index],
                        NULL, ASTLG_logend, g_Log_Group_Names,
                        YLD_file, logfile.c_str());

    if (err != YWB_SUCCESS) {
        printf("create logger failed, error: %s\r\n",
                ywb_strerror(err, errbuf, 256));
    }
    else {
        g_ast_logger = logger;
        if(false == ignoregyfslogger)
        {
            g_yfs_logger = logger;
        }

        ASTLogger::Instance().SetLoggerHandle(g_ast_logger);
    }
#else
    if (g_yfs_logger == NULL) {
        logger = new YwuLogger();
        g_yfs_logger = logger;
    }

    err = g_yfs_logger->open(YLF_append, 104857600, 100, sLogOption[index],
                          NULL, SSLG_oss_end, g_ast_log_group_names,
                          YLD_file, logfile.c_str());
    if (err != YWB_SUCCESS) {
        printf("create logger failed, error: %s\r\n",
               ywb_strerror(err, errbuf, 256));
    }
#endif

    return err;
}

/* vim:set ts=4 sw=4 expandtab */

