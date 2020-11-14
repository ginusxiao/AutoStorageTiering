#include "AST/ASTConfig.hpp"

/*TODO*/
ywb_status_t
ConfigManager::ConfigParseFromOptions(const string& options)
{
    return YWB_SUCCESS;
}

/*TODO*/
ywb_status_t
ConfigManager::ConfigParseFromFile(const string& fileName)
{
    return YWB_SUCCESS;
}

/*TODO*/
ywb_status_t
ConfigManager::Destroy()
{
    return YWB_SUCCESS;
}

void
ConfigManager::Reset()
{
    ast_log_debug("ConfigManager reset");
    /*nothing to do at current*/
}


/* vim:set ts=4 sw=4 expandtab */
