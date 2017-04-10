// -------------------------------------------------------------------------
//    @FileName			:    NFCGameServerModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFCGameServerModule
//
// -------------------------------------------------------------------------

#ifndef NFC_GAMESERVER_MODULE_H
#define NFC_GAMESERVER_MODULE_H

#include "NFComm/NFCore/NFMap.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIGameServerModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIUUIDModule.h"

class NFCGameServerModule
    : public NFIGameServerModule
{
public:
    NFCGameServerModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }
    virtual ~NFCGameServerModule() {};

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();
    virtual bool BeforeShut();


protected:


protected:
    NFIUUIDModule* m_pUUIDModule;
    NFIClassModule* m_pClassModule;
    NFIKernelModule* m_pKernelModule;
private:
};

#endif
