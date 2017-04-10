// -------------------------------------------------------------------------
//    @FileName         :    NFILogModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2012-12-15
//    @Module           :    NFILogModule
//
// -------------------------------------------------------------------------

#ifndef NFI_LOG_MODULE_H
#define NFI_LOG_MODULE_H

#include "NFIModule.h"

class NFILogModule
    : public NFIModule
{

public:

    enum NF_LOG_LEVEL
    {
        LOG_DEBUG,        // debug������Ϣ(Debug��Release�汾�����ӡ)
        LOG_INFO,         // info����
        LOG_WARNING,      // warning����
        LOG_ERROR,        // error����
        LOG_FATAL,        // fatal����
        LOG_MAX,
    };

    virtual void LogDebug(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogInfo(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogWarning(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogError(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogFatal(const NFGUID self, const std::string& strDesc, const std::string& strInfo = NULL_STR, const char* pFunc = "", int nLine = 0) = 0;

    virtual void LogDebug(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogInfo(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogWarning(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogError(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogFatal(const NFGUID self, const std::string& strDesc, const NFINT64 nInfo, const char* pFunc = "", int nLine = 0) = 0;

    virtual void LogDebug(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogInfo(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogWarning(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogError(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;
    virtual void LogFatal(const NFGUID self, const std::ostringstream& strDesc, const char* pFunc = "", int nLine = 0) = 0;

    virtual bool LogDebugFunctionDump(const NFGUID ident, const int nMsg, const std::string& strArg, const char* pFunc = "", int nLine = 0) = 0;
    virtual bool ChangeLogLevel(const std::string& strLevel, const std::string& strStatus) = 0;
    virtual bool SetSwitchingValue(const bool bValue) = 0;
};

#endif