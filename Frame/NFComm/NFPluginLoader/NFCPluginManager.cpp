// -------------------------------------------------------------------------
//    @FileName			:    NFCPluginManager.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFCPluginManager
//
// -------------------------------------------------------------------------

#include "NFCPluginManager.h"
#include "NFComm/RapidXML/rapidxml.hpp"
#include "NFComm/RapidXML/rapidxml_iterators.hpp"
#include "NFComm/RapidXML/rapidxml_print.hpp"
#include "NFComm/RapidXML/rapidxml_utils.hpp"
#include "NFComm/NFPluginModule/NFIPlugin.h"
#include "NFComm/NFPluginModule/NFPlatform.h"

#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma comment( lib, "ws2_32.lib" )
#endif

#ifdef NF_DEBUG_MODE

#if NF_PLATFORM == NF_PLATFORM_WIN

#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "libtherond.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#pragma comment( lib, "libtherond.a" )
#endif

#else

#if NF_PLATFORM == NF_PLATFORM_WIN

#elif NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID
#pragma comment( lib, "libtheron.a" )
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_APPLE_IOS
#pragma comment( lib, "libtheron.a" )
#endif

#endif

NFCPluginManager::NFCPluginManager() : NFIPluginManager()
{
   mnAppID = 0;
   mnInitTime = time(NULL);
   mnNowTime = mnInitTime;

   mstrConfigPath = "";
   mstrConfigName = "Plugin.xml";
}

NFCPluginManager::~NFCPluginManager()
{

}

inline bool NFCPluginManager::Init()
{
	LoadPluginConfig();

	PluginNameMap::iterator it = mPluginNameMap.begin();
	for (it; it != mPluginNameMap.end(); ++it)
	{
#ifdef NF_DYNAMIC_PLUGIN
		LoadPluginLibrary(it->first);
#else
		LoadStaticPlugin(it->first);
#endif
	}


	PluginInstanceMap::iterator itInstance = mPluginInstanceMap.begin();
	for (itInstance; itInstance != mPluginInstanceMap.end(); itInstance++)
	{
		itInstance->second->Init();
	}

	return true;
}

bool NFCPluginManager::LoadPluginConfig()
{
    rapidxml::file<> fdoc(mstrConfigName.c_str());
    rapidxml::xml_document<>  doc;
    doc.parse<0>(fdoc.data());

    rapidxml::xml_node<>* pRoot = doc.first_node();
    for (rapidxml::xml_node<>* pPluginNode = pRoot->first_node("Plugin"); pPluginNode; pPluginNode = pPluginNode->next_sibling("Plugin"))
    {
        const char* strPluginName = pPluginNode->first_attribute("Name")->value();
        const char* strMain = pPluginNode->first_attribute("Main")->value();

        mPluginNameMap.insert(PluginNameMap::value_type(strPluginName, true));

    }

    rapidxml::xml_node<>* pPluginAppNode = pRoot->first_node("APPID");
    if (!pPluginAppNode)
    {
        NFASSERT(0, "There are no App ID", __FILE__, __FUNCTION__);
        return false;
    }

    const char* strAppID = pPluginAppNode->first_attribute("Name")->value();
    if (!strAppID)
    {
        NFASSERT(0, "There are no App ID", __FILE__, __FUNCTION__);
        return false;
    }

    if (!NF_StrTo(strAppID, mnAppID))
    {
        NFASSERT(0, "App ID Convert Error", __FILE__, __FUNCTION__);
        return false;
    }

    rapidxml::xml_node<>* pPluginConfigPathNode = pRoot->first_node("ConfigPath");
    if (!pPluginConfigPathNode)
    {
        NFASSERT(0, "There are no ConfigPath", __FILE__, __FUNCTION__);
        return false;
    }

    if (NULL == pPluginConfigPathNode->first_attribute("Name"))
    {
        NFASSERT(0, "There are no ConfigPath.Name", __FILE__, __FUNCTION__);
        return false;
    }

    mstrConfigPath = pPluginConfigPathNode->first_attribute("Name")->value();

    return true;
}

bool NFCPluginManager::LoadStaticPlugin(const std::string& strPluginDLLName)
{
	//     PluginNameList::iterator it = mPluginNameList.begin();
	//     for (it; it != mPluginNameList.end(); it++)
	//     {
	//         const std::string& strPluginName = *it;
	//         CREATE_PLUGIN( this, strPluginName );
	//     }

	//     CREATE_PLUGIN(this, NFKernelPlugin)
	//     CREATE_PLUGIN(this, NFEventProcessPlugin)
	//     CREATE_PLUGIN(this, NFConfigPlugin)

	return false;
}

void NFCPluginManager::Registered(NFIPlugin* plugin)
{
    std::string strPluginName = plugin->GetPluginName();
    if (!FindPlugin(strPluginName))
    {
        bool bFind = false;
        PluginNameMap::iterator it = mPluginNameMap.begin();
        for (it; it != mPluginNameMap.end(); ++it)
        {
            if (strPluginName == it->first)
            {
                bFind = true;
                break;
            }
        }

        if (bFind)
        {
            mPluginInstanceMap.insert(PluginInstanceMap::value_type(strPluginName, plugin));
            plugin->Install();
        }
    }
}

void NFCPluginManager::UnRegistered(NFIPlugin* plugin)
{
    PluginInstanceMap::iterator it = mPluginInstanceMap.find(plugin->GetPluginName());
    if (it != mPluginInstanceMap.end())
    {
        it->second->Uninstall();
        delete it->second;
        it->second = NULL;
        mPluginInstanceMap.erase(it);
    }
}

NFIPlugin* NFCPluginManager::FindPlugin(const std::string& strPluginName)
{
    PluginInstanceMap::iterator it = mPluginInstanceMap.find(strPluginName);
    if (it != mPluginInstanceMap.end())
    {
        return it->second;
    }

    return NULL;
}

bool NFCPluginManager::Execute()
{
    mnNowTime = time(NULL);

    bool bRet = true;

    PluginInstanceMap::iterator it = mPluginInstanceMap.begin();
    for (; it != mPluginInstanceMap.end(); ++it)
    {
        bool tembRet = it->second->Execute();
        bRet = bRet && tembRet;
    }

    return bRet;
}

inline int NFCPluginManager::AppID() const
{
	return mnAppID;
}

inline NFINT64 NFCPluginManager::GetInitTime() const
{
	return mnInitTime;
}

inline NFINT64 NFCPluginManager::GetNowTime() const
{
	return mnNowTime;
}

inline const std::string & NFCPluginManager::GetConfigPath() const
{
	return mstrConfigPath;
}

void NFCPluginManager::SetConfigName(const std::string & strFileName)
{
	if (strFileName.empty())
	{
		return;
	}

	if (strFileName.find(".xml") == string::npos)
	{
		return;
	}

	mstrConfigName = strFileName;
}

void NFCPluginManager::AddModule(const std::string& strModuleName, NFIModule* pModule)
{
    if (!FindModule(strModuleName))
    {
        mModuleInstanceMap.insert(ModuleInstanceMap::value_type(strModuleName, pModule));
    }
}

void NFCPluginManager::RemoveModule(const std::string& strModuleName)
{
    ModuleInstanceMap::iterator it = mModuleInstanceMap.find(strModuleName);
    if (it != mModuleInstanceMap.end())
    {
        mModuleInstanceMap.erase(it);
    }
}


NFIModule* NFCPluginManager::FindModule(const std::string& strModuleName)
{
	std::string strSubModuleName = strModuleName;

#if NF_PLATFORM == NF_PLATFORM_WIN
	std::size_t position = strSubModuleName.find(" ");
	if (string::npos != position)
	{
		strSubModuleName = strSubModuleName.substr(position + 1, strSubModuleName.length());
	}
#else
	for (int i = 0; i < strSubModuleName.length(); i++)
	{
		std::string s = strSubModuleName.substr(0, i + 1);
		int n = atof(s.c_str());
		if (strSubModuleName.length() == i + 1 + n)
		{
			strSubModuleName = strSubModuleName.substr(i + 1, strSubModuleName.length());
			break;
		}
	}
#endif

	ModuleInstanceMap::iterator it = mModuleInstanceMap.find(strSubModuleName);
	if (it != mModuleInstanceMap.end())
	{
		return it->second;
	}
    
    return NULL;
}

bool NFCPluginManager::AfterInit()
{
    PluginInstanceMap::iterator itAfterInstance = mPluginInstanceMap.begin();
    for (itAfterInstance; itAfterInstance != mPluginInstanceMap.end(); itAfterInstance++)
    {
        itAfterInstance->second->AfterInit();
    }

    return true;
}

bool NFCPluginManager::CheckConfig()
{
    PluginInstanceMap::iterator itCheckInstance = mPluginInstanceMap.begin();
    for (itCheckInstance; itCheckInstance != mPluginInstanceMap.end(); itCheckInstance++)
    {
        itCheckInstance->second->CheckConfig();
    }

    return true;
}

bool NFCPluginManager::BeforeShut()
{
    PluginInstanceMap::iterator itBeforeInstance = mPluginInstanceMap.begin();
    for (itBeforeInstance; itBeforeInstance != mPluginInstanceMap.end(); itBeforeInstance++)
    {
        itBeforeInstance->second->BeforeShut();
    }

    return true;
}

bool NFCPluginManager::Shut()
{
    PluginInstanceMap::iterator itInstance = mPluginInstanceMap.begin();
    for (itInstance; itInstance != mPluginInstanceMap.end(); ++itInstance)
    {
        itInstance->second->Shut();
    }



    PluginNameMap::iterator it = mPluginNameMap.begin();
    for (it; it != mPluginNameMap.end(); it++)
    {
#ifdef NF_DYNAMIC_PLUGIN
        UnLoadPluginLibrary(it->first);
#else
		UnLoadStaticPlugin(it->first);
#endif
    }



    mPluginInstanceMap.clear();
    mPluginNameMap.clear();
    return true;
}

bool NFCPluginManager::LoadPluginLibrary(const std::string& strPluginDLLName)
{
    PluginLibMap::iterator it = mPluginLibMap.find(strPluginDLLName);
    if (it == mPluginLibMap.end())
    {
        NFCDynLib* pLib = new NFCDynLib(strPluginDLLName);
        bool bLoad = pLib->Load();

        if (bLoad)
        {
            mPluginLibMap.insert(PluginLibMap::value_type(strPluginDLLName, pLib));

            DLL_START_PLUGIN_FUNC pFunc = (DLL_START_PLUGIN_FUNC)pLib->GetSymbol("DllStartPlugin");
            if (!pFunc)
            {
                std::cout << "Find function DllStartPlugin Failed in [" << pLib->GetName() << "]" << std::endl;
                assert(0);
                return false;
            }

            pFunc(this);

            return true;
        }
        else
        {
#if NF_PLATFORM == NF_PLATFORM_LINUX
            char* error = dlerror();
            if (error)
            {
                std::cout << stderr << " Load shared lib[" << pLib->GetName() << "] failed, ErrorNo. = [" << error << "]" << std::endl;
                std::cout << "Load [" << pLib->GetName() << "] failed" << std::endl;
                assert(0);
                return false;
            }
#elif NF_PLATFORM == NF_PLATFORM_WIN
            std::cout << stderr << " Load DLL[" << pLib->GetName() << "] failed, ErrorNo. = [" << GetLastError() << "]" << std::endl;
            std::cout << "Load [" << pLib->GetName() << "] failed" << std::endl;
            assert(0);
            return false;
#endif // NF_PLATFORM
        }
    }

    return false;
}

bool NFCPluginManager::UnLoadPluginLibrary(const std::string& strPluginDLLName)
{
    PluginLibMap::iterator it = mPluginLibMap.find(strPluginDLLName);
    if (it != mPluginLibMap.end())
    {
        NFCDynLib* pLib = it->second;

        DLL_STOP_PLUGIN_FUNC pFunc = (DLL_STOP_PLUGIN_FUNC)pLib->GetSymbol("DllStopPlugin");

        if (pFunc)
        {
            pFunc(this);
        }

        pLib->UnLoad();

        delete pLib;
        pLib = NULL;
        mPluginLibMap.erase(it);

        return true;
    }

    return false;
}

bool NFCPluginManager::UnLoadStaticPlugin(const std::string & strPluginDLLName)
{
	//     DESTROY_PLUGIN(this, NFConfigPlugin)
	//     DESTROY_PLUGIN(this, NFEventProcessPlugin)
	//     DESTROY_PLUGIN(this, NFKernelPlugin)
	return false;
}

bool NFCPluginManager::StartReLoadState()
{
    NFIModule::StartReLoadState();

    PluginInstanceMap::iterator itBeforeInstance = mPluginInstanceMap.begin();
    for (itBeforeInstance; itBeforeInstance != mPluginInstanceMap.end(); itBeforeInstance++)
    {
        itBeforeInstance->second->StartReLoadState();
    }

    return true;
}

bool NFCPluginManager::EndReLoadState()
{
    PluginInstanceMap::iterator itBeforeInstance = mPluginInstanceMap.begin();
    for (itBeforeInstance; itBeforeInstance != mPluginInstanceMap.end(); itBeforeInstance++)
    {
        itBeforeInstance->second->EndReLoadState();
    }

    NFIModule::EndReLoadState();

    return true;
}