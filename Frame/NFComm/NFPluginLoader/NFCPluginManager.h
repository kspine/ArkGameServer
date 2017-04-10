// -------------------------------------------------------------------------
//    @FileName			:    NFCPluginManager.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFCPluginManager
//
// -------------------------------------------------------------------------

#ifndef NFC_PLUGIN_MANAGER_H
#define NFC_PLUGIN_MANAGER_H

#include <map>
#include <string>
#include <time.h>
#include "NFCDynLib.h"
#include "NFComm/NFCore/NFSingleton.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

class NFCPluginManager
    : public NFIPluginManager,
	public NFSingleton<NFCPluginManager>
{
public:
    NFCPluginManager();
    virtual ~NFCPluginManager();

	virtual bool Init();

    virtual bool AfterInit();

    virtual bool CheckConfig();

    virtual bool BeforeShut();

    virtual bool Shut();

    //////////////////////////////////////////////////////////////////////////

    virtual void Registered(NFIPlugin* pPlugin);

    virtual void UnRegistered(NFIPlugin* pPlugin);

    virtual bool StartReLoadState();

    virtual bool EndReLoadState();
    //////////////////////////////////////////////////////////////////////////

    virtual NFIPlugin* FindPlugin(const std::string& strPluginName);

    virtual void AddModule(const std::string& strModuleName, NFIModule* pModule);

    virtual void RemoveModule(const std::string& strModuleName);

    virtual NFIModule* FindModule(const std::string& strModuleName);

    virtual bool Execute();

	virtual int AppID() const;

	virtual NFINT64 GetInitTime() const;

	virtual NFINT64 GetNowTime() const;

	virtual const std::string& GetConfigPath() const;

	virtual void SetConfigName(const std::string& strFileName);
protected:
	bool LoadPluginConfig();

	bool LoadStaticPlugin(const std::string& strPluginDLLName);
    bool LoadPluginLibrary(const std::string& strPluginDLLName);
    bool UnLoadPluginLibrary(const std::string& strPluginDLLName);
	bool UnLoadStaticPlugin(const std::string& strPluginDLLName);

private:
    int mnAppID;
    NFINT64 mnInitTime;
    NFINT64 mnNowTime;
    std::string mstrConfigPath;
	std::string mstrConfigName;

    typedef std::map<std::string, bool> PluginNameMap;
    typedef std::map<std::string, NFCDynLib*> PluginLibMap;
    typedef std::map<std::string, NFIPlugin*> PluginInstanceMap;
    typedef std::map<std::string, NFIModule*> ModuleInstanceMap;

    typedef void(* DLL_START_PLUGIN_FUNC)(NFIPluginManager* pm);
    typedef void(* DLL_STOP_PLUGIN_FUNC)(NFIPluginManager* pm);

    PluginNameMap mPluginNameMap;
    PluginLibMap mPluginLibMap;
    PluginInstanceMap mPluginInstanceMap;
    ModuleInstanceMap mModuleInstanceMap;
};

#endif
