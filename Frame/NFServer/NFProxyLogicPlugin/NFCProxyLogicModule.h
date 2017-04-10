// -------------------------------------------------------------------------
//    @FileName			:    NFCProxyLogicModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-05-06
//    @Module           :    NFCProxyLogicModule
//
// -------------------------------------------------------------------------

#ifndef NFC_PROXYLOGIC_MODULE_H
#define NFC_PROXYLOGIC_MODULE_H

#include "NFComm/NFCore/NFMap.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "NFComm/NFPluginModule/NFIGameLogicModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIProxyLogicModule.h"

class NFCProxyLogicModule
    : public NFIProxyLogicModule
{
public:
    NFCProxyLogicModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();


protected:


protected:
    NFIClassModule* m_pClassModule;
    NFIKernelModule* m_pKernelModule;
private:
};

#endif