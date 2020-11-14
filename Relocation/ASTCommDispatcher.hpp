#ifndef __AST_COMM_DISPATCHER_HPP__
#define __AST_COMM_DISPATCHER_HPP__

#include "Template/Queue.hpp"
#include "FsCommon/YfsThread.h"
#include "common/FsAssert.h"
#include "common/FsStatus.h"
#include "OSSNetwork/CommDispatcher.hpp"
#include "OSSCommon/Proceduer.hpp"
#include "OSSNetwork/CommunicationLayout.hpp"


/*
 * specially designed for migration completed event
 * */
class ASTCommDispatcher : public CommDispatcher
{
public:
    ASTCommDispatcher(CommunicationLayout* cl, const char *name);
    virtual ~ASTCommDispatcher();

    CommAdapter* AllocAdapter()
    {
        YWB_ASSERT(0);
        return NULL;
    }

    void GetAllServiceId(std::vector<yfs_sid_t> &sidvec)
    {
        YWB_ASSERT(0);
        return;
    }

    ywb_status_t Initialize()
    {
        CommDispatcher::Initialize();
        WakeUp();
        return YWB_SUCCESS;
    }

    void HandleTimeout()
    {

    }

    void LaunchMigrationCompletedEvent(OBJKey objkey,
            ywb_status_t migrationstatus);

protected:
    class ASTMigrateProcedure : public OperationProceduer {
    public:
        ASTMigrateProcedure () :
            OperationProceduer("ASTCommDispatcher::ASTMigrationProcedure") {}

        ~ASTMigrateProcedure () {}

        ywb_status_t Process(void *dispatcher, void *ctx);
    };

private:
};

#endif

/* vim:set ts=4 sw=4 expandtab */
