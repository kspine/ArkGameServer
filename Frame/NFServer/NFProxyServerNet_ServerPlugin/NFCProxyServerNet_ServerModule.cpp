// -------------------------------------------------------------------------
//    @FileName         :    NFCGameServerNet_ServerModule.cpp
//    @Author           :    Ark Game Tech
//    @Date             :    2013-01-02
//    @Module           :    NFCGameServerNet_ServerModule
//    @Desc             :
// -------------------------------------------------------------------------

#include "NFCProxyServerNet_ServerModule.h"
#include "NFProxyServerNet_ServerPlugin.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"

bool NFCProxyServerNet_ServerModule::Init()
{
    m_pNetModule = NF_NEW NFINetModule(pPluginManager);
    return true;
}

bool NFCProxyServerNet_ServerModule::AfterInit()
{
    m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
    m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
    m_pProxyToWorldModule = pPluginManager->FindModule<NFIProxyServerToWorldModule>();
    m_pLogModule = pPluginManager->FindModule<NFILogModule>();
    m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
    m_pUUIDModule = pPluginManager->FindModule<NFIUUIDModule>();
    m_pProxyServerToGameModule = pPluginManager->FindModule<NFIProxyServerToGameModule>();

    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_CONNECT_KEY, this, &NFCProxyServerNet_ServerModule::OnConnectKeyProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_WORLD_LIST, this, &NFCProxyServerNet_ServerModule::OnReqServerListProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_SELECT_SERVER, this, &NFCProxyServerNet_ServerModule::OnSelectServerProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_ROLE_LIST, this, &NFCProxyServerNet_ServerModule::OnReqRoleListProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_CREATE_ROLE, this, &NFCProxyServerNet_ServerModule::OnReqCreateRoleProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_DELETE_ROLE, this, &NFCProxyServerNet_ServerModule::OnReqDelRoleProcess);
    m_pNetModule->AddReceiveCallBack(NFMsg::EGMI_REQ_ENTER_GAME, this, &NFCProxyServerNet_ServerModule::OnReqEnterGameServer);
    m_pNetModule->AddReceiveCallBack(this, &NFCProxyServerNet_ServerModule::OnOtherMessage);

    m_pNetModule->AddEventCallBack(this, &NFCProxyServerNet_ServerModule::OnSocketClientEvent);

    NF_SHARE_PTR<NFIClass> xLogicClass = m_pClassModule->GetElement("Server");
    if(nullptr != xLogicClass)
    {
        NFList<std::string>& xNameList = xLogicClass->GetConfigNameList();
        std::string strConfigName;
        for(bool bRet = xNameList.First(strConfigName); bRet; bRet = xNameList.Next(strConfigName))
        {
            const int nServerType = m_pElementModule->GetPropertyInt(strConfigName, "Type");
            const int nServerID = m_pElementModule->GetPropertyInt(strConfigName, "ServerID");
            if(nServerType == NF_SERVER_TYPES::NF_ST_PROXY && pPluginManager->AppID() == nServerID)
            {
                const int nPort = m_pElementModule->GetPropertyInt(strConfigName, "Port");
                const int nMaxConnect = m_pElementModule->GetPropertyInt(strConfigName, "MaxOnline");
                const int nCpus = m_pElementModule->GetPropertyInt(strConfigName, "CpuCount");
                const std::string& strName = m_pElementModule->GetPropertyString(strConfigName, "Name");
                const std::string& strIP = m_pElementModule->GetPropertyString(strConfigName, "IP");

                m_pUUIDModule->SetIdentID(nServerID);

                int nRet = m_pNetModule->Initialization(nMaxConnect, nPort, nCpus, nServerID);
                if(nRet < 0)
                {
                    std::ostringstream strLog;
                    strLog << "Cannot init server net, Port = " << nPort;
                    m_pLogModule->LogError(NULL_GUID, strLog, __FUNCTION__, __LINE__);
                    NFASSERT(nRet, "Cannot init server net", __FILE__, __FUNCTION__);
                    exit(0);
                }
            }
        }
    }

    return true;
}

bool NFCProxyServerNet_ServerModule::Shut()
{

    return true;
}

bool NFCProxyServerNet_ServerModule::Execute()
{
    return m_pNetModule->Execute();
}

int NFCProxyServerNet_ServerModule::HB_OnConnectCheckTime(const NFGUID& self, const std::string& strHeartBeat, const float fTime, const int nCount, const AFDataList& var)
{
    m_pKernelModule->DestroyObject(self);

    return 0;
}

void NFCProxyServerNet_ServerModule::OnOtherMessage(const int nSockIndex, const int nMsgID, const char * msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFMsg::MsgBase xMsg;
    if(!xMsg.ParseFromArray(msg, nLen))
    {
        char szData[MAX_PATH] = { 0 };
        sprintf(szData, "Parse Message Failed from Packet to MsgBase, MessageID: %d\n", nMsgID);

        return;
    }

    NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(!pSessionData || pSessionData->mnLogicState <= 0 || pSessionData->mnGameID <= 0)
    {
        //state error
        return;
    }
    if(xMsg.has_hash_ident())
    {
        //special for distributed
        if(!pSessionData->mnHashIdentID.IsNull())
        {
            NFCMachineNode xNode;
            if(mxConsistentHash.GetSuitNode(pSessionData->mnHashIdentID.ToString(), xNode))
            {
                m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(xNode.GetDataID(), nMsgID, msg, nLen);
            }
        }
        else
        {
            NFGUID xHashIdent = NFINetModule::PBToNF(xMsg.hash_ident());

            NFCMachineNode xNode;
            if(mxConsistentHash.GetSuitNode(xHashIdent.ToString(), xNode))
            {
                m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(xNode.GetDataID(), nMsgID, msg, nLen);
            }
        }
    }
    else
    {
        m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, msg, nLen);
    }
}

void NFCProxyServerNet_ServerModule::OnConnectKeyProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFGUID nPlayerID;
    NFMsg::ReqAccountLogin xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    bool bRet = m_pProxyToWorldModule->VerifyConnectData(xMsg.account(), xMsg.security_code());
    if(bRet)
    {
        //���Խ���,���ñ�־��ѡ����,�����ӳ�,����gs������ɫ��ɾ����ɫ,����ֻ��ת��
        NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData)
        {
            pSessionData->mnLogicState = 1;
            pSessionData->mstrAccout  = xMsg.account();

            NFMsg::AckEventResult xSendMsg;
            xSendMsg.set_event_code(NFMsg::EGEC_VERIFY_KEY_SUCCESS);
            *xSendMsg.mutable_event_client() = NFINetModule::NFToPB(pSessionData->mnClientID);//��ǰ�˼ǵ��Լ���fd��������һЩ��֤

            m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_CONNECT_KEY, xSendMsg, nSockIndex);
        }
    }
    else
    {
        m_pNetModule->GetNet()->CloseNetObject(nSockIndex);
    }
}

void NFCProxyServerNet_ServerModule::OnSocketClientEvent(const int nSockIndex, const NF_NET_EVENT eEvent, const NFGUID& xClientID, const int nSeverID)
{
    if(eEvent & NF_NET_EVENT_EOF)
    {
        m_pLogModule->LogInfo(NFGUID(0, nSockIndex), "NF_NET_EVENT_EOF", "Connection closed", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex, xClientID);
    }
    else if(eEvent & NF_NET_EVENT_ERROR)
    {
        m_pLogModule->LogInfo(NFGUID(0, nSockIndex), "NF_NET_EVENT_ERROR", "Got an error on the connection", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex, xClientID);
    }
    else if(eEvent & NF_NET_EVENT_TIMEOUT)
    {
        m_pLogModule->LogInfo(NFGUID(0, nSockIndex), "NF_NET_EVENT_TIMEOUT", "read timeout", __FUNCTION__, __LINE__);
        OnClientDisconnect(nSockIndex, xClientID);
    }
    else  if(eEvent == NF_NET_EVENT_CONNECTED)
    {
        m_pLogModule->LogInfo(NFGUID(0, nSockIndex), "NF_NET_EVENT_CONNECTED", "connectioned success", __FUNCTION__, __LINE__);
        OnClientConnected(nSockIndex, xClientID);
    }
}

void NFCProxyServerNet_ServerModule::OnClientDisconnect(const int nAddress, const NFGUID& xClientID)
{
    NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(pSessionData)
    {
        if(pSessionData->mnGameID > 0)
        {
            if(!pSessionData->mnUserID.IsNull())
            {
                //����
                NFMsg::ReqLeaveGameServer xData;

                NFMsg::MsgBase xMsg;
                //playerid��Ҫ������ת����Ϣ��ʱ����ʶ��ʹ�ã�����ʹ�ò�ʹ��
                *xMsg.mutable_player_id() = NFINetModule::NFToPB(pSessionData->mnUserID);

                if(!xData.SerializeToString(xMsg.mutable_msg_data()))
                {
                    return;
                }

                std::string strMsg;
                if(!xMsg.SerializeToString(&strMsg))
                {
                    return;
                }

                m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, NFMsg::EGameMsgID::EGMI_REQ_LEAVE_GAME, strMsg);
            }
        }

        mmSessionData.RemoveElement(xClientID);
    }
}

void NFCProxyServerNet_ServerModule::OnSelectServerProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFGUID nPlayerID;
    NFMsg::ReqSelectServer xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    NF_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xMsg.world_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData)
        {
            //now this client bind a game server, after this time, all message will be sent to this game server who bind with client
            pSessionData->mnGameID = xMsg.world_id();

            NFMsg::AckEventResult xMsg;
            xMsg.set_event_code(NFMsg::EGameEventCode::EGEC_SELECTSERVER_SUCCESS);
            m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xMsg, nSockIndex);
            return;
        }
    }

    NFMsg::AckEventResult xSendMsg;
    xSendMsg.set_event_code(NFMsg::EGameEventCode::EGEC_SELECTSERVER_FAIL);
    m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_SELECT_SERVER, xMsg, nSockIndex);
}

void NFCProxyServerNet_ServerModule::OnReqServerListProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFGUID nPlayerID;
    NFMsg::ReqServerList xMsg;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xMsg, nPlayerID))
    {
        return;
    }

    if(xMsg.type() != NFMsg::RSLT_GAMES_ERVER)
    {
        return;
    }

    NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(pSessionData && pSessionData->mnLogicState > 0)
    {
        //ack all gameserver data
        NFMsg::AckServerList xData;
        xData.set_type(NFMsg::RSLT_GAMES_ERVER);

        NFMapEx<int, ConnectData>& xServerList = m_pProxyServerToGameModule->GetClusterModule()->GetServerList();
        ConnectData* pGameData = xServerList.FirstNude();
        while(NULL != pGameData)
        {
            if(ConnectDataState::NORMAL == pGameData->eState)
            {
                NFMsg::ServerInfo* pServerInfo = xData.add_info();

                pServerInfo->set_name(pGameData->strName);
                pServerInfo->set_status(NFMsg::EServerState::EST_NARMAL);
                pServerInfo->set_server_id(pGameData->nGameID);
                pServerInfo->set_wait_count(0);
            }

            pGameData = xServerList.NextNude();
        }

        m_pNetModule->SendMsgPB(NFMsg::EGameMsgID::EGMI_ACK_WORLD_LIST, xData, nSockIndex);
    }
}

int NFCProxyServerNet_ServerModule::Transpond(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    NFMsg::MsgBase xMsg;
    if(!xMsg.ParseFromArray(msg, nLen))
    {
        char szData[MAX_PATH] = { 0 };
        sprintf(szData, "Parse Message Failed from Packet to MsgBase, MessageID: %d\n", nMsgID);

        return false;
    }

    //broadcast many palyer
    for(int i = 0; i < xMsg.player_client_list_size(); ++i)
    {
        NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(NFINetModule::PBToNF(xMsg.player_client_list(i)));
        if(pSessionData)
        {
            if(xMsg.has_hash_ident())
            {
                NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
                if(pSessionData)
                {
                    pSessionData->mnHashIdentID = NFINetModule::PBToNF(xMsg.hash_ident());
                }
            }

            m_pNetModule->GetNet()->SendMsgWithOutHead(nMsgID, msg, nLen, pSessionData->mnFD);
        }
    }

    //send message to one player
    if(xMsg.player_client_list_size() <= 0)
    {
        //playerid==ClientID;

        NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(NFINetModule::PBToNF(xMsg.player_id()));
        if(pSessionData)
        {
            if(xMsg.has_hash_ident())
            {
                NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
                if(pSessionData)
                {
                    pSessionData->mnHashIdentID = NFINetModule::PBToNF(xMsg.hash_ident());
                }
            }

            m_pNetModule->GetNet()->SendMsgWithOutHead(nMsgID, msg, nLen, pSessionData->mnFD);
        }
    }

    return true;
}

void NFCProxyServerNet_ServerModule::OnClientConnected(const int nAddress, const NFGUID& xClientID)
{
    NF_SHARE_PTR<SessionData> pSessionData(NF_NEW SessionData());

    pSessionData->mnClientID = xClientID;
    pSessionData->mnFD = nAddress;
    mmSessionData.AddElement(xClientID, pSessionData);
}

void NFCProxyServerNet_ServerModule::OnReqRoleListProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    //��û����ʽ������Ϸ֮ǰ��nPlayerID����FD
    NFGUID nPlayerID;
    NFMsg::ReqRoleList xData;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xData, nPlayerID))
    {
        return;
    }

    NF_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xData.game_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        //����ƥ��
        NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData
                && pSessionData->mnLogicState > 0
                && pSessionData->mnGameID == xData.game_id()
                && pSessionData->mstrAccout == xData.account())
        {
            NFMsg::MsgBase xMsg;
            if(!xData.SerializeToString(xMsg.mutable_msg_data()))
            {
                return;
            }

            //playerid��Ҫ������ת����Ϣ��ʱ����ʶ��ʹ�ã�����ʹ�ò�ʹ��
            xMsg.mutable_player_id()->CopyFrom(NFINetModule::NFToPB(pSessionData->mnClientID));

            std::string strMsg;
            if(!xMsg.SerializeToString(&strMsg))
            {
                return;
            }

            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, NFMsg::EGameMsgID::EGMI_REQ_ROLE_LIST, strMsg);
        }
    }
}

void NFCProxyServerNet_ServerModule::OnReqCreateRoleProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    //��û����ʽ������Ϸ֮ǰ��nPlayerID����FD


    NFGUID nPlayerID;
    NFMsg::ReqCreateRole xData;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xData, nPlayerID))
    {
        return;
    }

    NF_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xData.game_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        //����ƥ��
        NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData
                && pSessionData->mnLogicState > 0
                && pSessionData->mnGameID == xData.game_id()
                && pSessionData->mstrAccout == xData.account())
        {
            NFMsg::MsgBase xMsg;
            if(!xData.SerializeToString(xMsg.mutable_msg_data()))
            {
                return;
            }

            //playerid��Ҫ������ת����Ϣ��ʱ����ʶ��ʹ�ã�����ʹ�ò�ʹ��
            xMsg.mutable_player_id()->CopyFrom(NFINetModule::NFToPB(pSessionData->mnClientID));

            std::string strMsg;
            if(!xMsg.SerializeToString(&strMsg))
            {
                return;
            }

            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, strMsg);
        }
    }
}

void NFCProxyServerNet_ServerModule::OnReqDelRoleProcess(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    //��û����ʽ������Ϸ֮ǰ��nPlayerID����FD
    NFGUID nPlayerID;
    NFMsg::ReqDeleteRole xData;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xData, nPlayerID))
    {
        return;
    }

    NF_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xData.game_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        //����ƥ��
        NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData
                && pSessionData->mnLogicState > 0
                && pSessionData->mnGameID == xData.game_id()
                && pSessionData->mstrAccout == xData.account())
        {
            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, nMsgID, std::string(msg, nLen));
        }
    }
}

void NFCProxyServerNet_ServerModule::OnReqEnterGameServer(const int nSockIndex, const int nMsgID, const char* msg, const uint32_t nLen, const NFGUID& xClientID)
{
    //��û����ʽ������Ϸ֮ǰ��nPlayerID����FD
    NFGUID nPlayerID;
    NFMsg::ReqEnterGameServer xData;
    if(!m_pNetModule->ReceivePB(nSockIndex, nMsgID, msg, nLen, xData, nPlayerID))
    {
        return;
    }

    NF_SHARE_PTR<ConnectData> pServerData = m_pProxyServerToGameModule->GetClusterModule()->GetServerNetInfo(xData.game_id());
    if(pServerData && ConnectDataState::NORMAL == pServerData->eState)
    {
        //����ƥ��
        NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
        if(pSessionData
                && pSessionData->mnLogicState > 0
                && pSessionData->mnGameID == xData.game_id()
                && pSessionData->mstrAccout == xData.account()
                && !xData.name().empty()
                && !xData.account().empty())
        {
            NFMsg::MsgBase xMsg;
            if(!xData.SerializeToString(xMsg.mutable_msg_data()))
            {
                return;
            }

            //playerid�ڽ�����Ϸ֮ǰ����FD������ʱ������ʵ��ID
            xMsg.mutable_player_id()->CopyFrom(NFINetModule::NFToPB(pSessionData->mnClientID));

            std::string strMsg;
            if(!xMsg.SerializeToString(&strMsg))
            {
                return;
            }

            m_pProxyServerToGameModule->GetClusterModule()->SendByServerID(pSessionData->mnGameID, NFMsg::EGameMsgID::EGMI_REQ_ENTER_GAME, strMsg);
        }
    }
}

int NFCProxyServerNet_ServerModule::EnterGameSuccessEvent(const NFGUID xClientID, const NFGUID xPlayerID)
{
    NF_SHARE_PTR<SessionData> pSessionData = mmSessionData.GetElement(xClientID);
    if(pSessionData)
    {
        pSessionData->mnUserID = xPlayerID;
    }

    return 0;
}
