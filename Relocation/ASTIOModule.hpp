#ifndef __AST_IO_MODULE_HPP__
#define __AST_IO_MODULE_HPP__

#include "OSSCommon/ControlCenter.hpp"
#include "Storage/IOModule.hpp"
#include "AST/ASTHUB.hpp"

class ASTIOModule : public IOModule {
public:
    ASTIOModule(const char *name) : IOModule(name, IOModule::IOMT_ast)
    {

    }

    ~ASTIOModule()
    {
        this->Finalize();
    }

    inline HUB* GetHUB()
    {
        return mStorageManager->GetLayout()->GetControlCenter()->GetHUB();
    }

    inline ControlCenter* GetControlCenter()
    {
        return mStorageManager->GetLayout()->GetControlCenter();
    }

    static ywb_uint32_t RoundUp2KB(ywb_uint32_t byte)
    {
        return (0 == (byte % 1024)) ? ((byte / 1024)) : ((byte / 1024) + 1);
    }

    ywb_status_t Initialize(StorageManager *sm,
                            IOModule *iom, int workercnt);
    ywb_status_t Finalize();

    ywb_status_t Create(ChunkFileHandler *cfh, IOParam *param);
    ywb_status_t Write(ChunkFileHandler *cfh, IOParam *param);
    ywb_status_t Read(ChunkFileHandler *cfh, IOParam *param);
    ywb_status_t Truncate(ChunkFileHandler *cfh, IOParam *param);
    ywb_status_t Flush(ChunkFileHandler *cfh, IOParam *param);
    ywb_status_t Delete(ChunkFileHandler *cfh, IOParam *param);
    ywb_status_t Rename(ChunkFileHandler *cfh, IOParam *param);
    ywb_status_t SetDirty(ChunkFileHandler *cfh, IOParam *param);
    ywb_status_t Replicate(ChunkFileHandler *cfh, IOParam *param);
    ywb_status_t Snapshot(ChunkFileHandler *cfh, IOParam *param);

private:
};

#endif

/* vim:set ts=4 sw=4 expandtab */
