#include "AST/ASTConstant.hpp"
#include "AST/ASTTime.hpp"
#include "AST/ASTRelocation.hpp"
#include "AST/ASTAdviceRule.hpp"
#include "AST/ASTPlan.hpp"

const string Constant::DISK_PROFILE_PATH("/etc/YeeSAN/diskprofile.xml");
const string Constant::DISKPROFILE("diskprofile");
const string Constant::PRODUCTFAMILY("productfamily");
const string Constant::PRODUCT("product");
const string Constant::DEVICE("device");
const string Constant::PERF("perf");
const string Constant::DDMCLASS("ddmclass");
const string Constant::RPM("rpm");
const string Constant::RAID("raid");
const string Constant::WIDTH("width");
const string Constant::CAPACITY("cap");
const string Constant::READRATIO("readratio");
const string Constant::IOSIZE("iosize");
const string Constant::BANDWIDTH("bandwidth");
const string Constant::IOPS("iops");
const string Constant::CONFIG_PATH("/etc/YeeSAN/ast.conf");
const string Constant::LOG_PATH("ASTUnitTests.log");
const string Constant::EMPTY_STR("");
const string Constant::PLAN_FILE_PATH("/etc/testplan");

/*validate all the defined constant*/
Constant::Constant()
{
    ConstantCheck(CYCLE_TYPE_CNT, CycleManager::CT_cnt);
    ConstantCheck(DECISION_WINDOW_CNT, DecisionWindowManager::DW_cnt);
    ConstantCheck(EMA_CNT, DecisionWindowManager::DW_cnt);
    ConstantCheck(IO_STAT_TYPE_CNT, EMA_CNT + 1);
    ConstantCheck(RELOCATION_TASK_CNT, RelocationTaskType::RTT_cnt);
    ConstantCheck(ADVICE_TYPE_CNT, AdviceType::AT_cnt);
    ConstantCheck(PLAN_DIR_CNT, Plan::PD_cnt);
}

/* vim:set ts=4 sw=4 expandtab */
