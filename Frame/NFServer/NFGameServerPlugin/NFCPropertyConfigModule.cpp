// -------------------------------------------------------------------------
//    @FileName			:    NFCPropertyConfigModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-09-30
//    @Module           :    NFCPropertyConfigModule
//
// -------------------------------------------------------------------------

#include "NFCPropertyConfigModule.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"

bool NFCPropertyConfigModule::Init()
{
    return true;
}

bool NFCPropertyConfigModule::Shut()
{
    return true;
}

bool NFCPropertyConfigModule::Execute()
{
    return true;
}

bool NFCPropertyConfigModule::AfterInit()
{
    m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
    m_pElementModule = pPluginManager->FindModule<NFIElementModule>();

    Load();

    return true;
}

int NFCPropertyConfigModule::CalculateBaseValue(const int nJob, const int nLevel, const std::string& strProperty)
{
	NF_SHARE_PTR <NFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);
	if (xPropertyMap)
	{
		NF_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);
		if (xRefPropertyIDName)
		{
			return m_pElementModule->GetPropertyInt(*xRefPropertyIDName, strProperty);
		}
    }

    return 0;
}

void NFCPropertyConfigModule::Load()
{
    NF_SHARE_PTR<NFIClass> pLogicClass = m_pClassModule->GetElement(NFrame::InitProperty::ThisName());
    if (nullptr != pLogicClass)
    {
        NFList<std::string>& xList = pLogicClass->GetConfigNameList();
        std::string strData;
        bool bRet = xList.First(strData);
        while (bRet)
        {
            NF_SHARE_PTR<NFIPropertyManager> pPropertyManager = m_pElementModule->GetPropertyManager(strData);
            if (nullptr != pPropertyManager)
            {
                int nJob = m_pElementModule->GetPropertyInt(strData, NFrame::InitProperty::Job());
                int nLevel = m_pElementModule->GetPropertyInt(strData, NFrame::InitProperty::Level());
                std::string strEffectData = m_pElementModule->GetPropertyString(strData, NFrame::InitProperty::EffectData());

				NF_SHARE_PTR <NFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);
				if (!xPropertyMap)
				{
					xPropertyMap = NF_SHARE_PTR<NFMapEx<int, std::string>>(NF_NEW NFMapEx<int, std::string>());
					mhtCoefficienData.AddElement(nJob, xPropertyMap);

					NF_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);
					if (!xRefPropertyIDName)
					{
						xRefPropertyIDName = NF_SHARE_PTR<std::string>(NF_NEW std::string(strEffectData));
						xPropertyMap->AddElement(nLevel, xRefPropertyIDName);
					}
				}
            }

            bRet = xList.Next(strData);
        }
    }
}

bool NFCPropertyConfigModule::LegalLevel(const int nJob, const int nLevel)
{
	NF_SHARE_PTR <NFMapEx<int, std::string> > xPropertyMap = mhtCoefficienData.GetElement(nJob);
	if (xPropertyMap)
	{
		NF_SHARE_PTR<std::string> xRefPropertyIDName = xPropertyMap->GetElement(nLevel);
		if (xRefPropertyIDName)
		{
			return true;
		}
	}


    return false;
}
