// -------------------------------------------------------------------------
//    @FileName         ��    NFIGameServerNet_ServerModule.h
//    @Author           ��    Ark Game Tech
//    @Date             ��    2012-12-15
//    @Module           ��    NFIGameServerNet_ServerModule
//
// -------------------------------------------------------------------------

#ifndef NFI_GAMESERVERNET_SERVERMODULE_H
#define NFI_GAMESERVERNET_SERVERMODULE_H

#include <iostream>
#include "NFIModule.h"
#include "NFINetModule.h"

class NFIGameServerNet_ServerModule
    : public NFIModule
{
public:
    //Ҫ����ǰ���еĶ������ڵ�actor,gateid,fd��
    struct GateBaseInfo
    {
        GateBaseInfo()
        {
            nActorID = 0;
            nGateID = 0;
        }

        GateBaseInfo(const int gateID, const NFGUID xIdent)
        {
            nActorID = 0;
            nGateID = gateID;
            xClientID = xIdent;
        }

        int nActorID;
        int nGateID;
        NFGUID xClientID;
    };

    struct GateServerInfo
    {
        ServerData xServerData;
        //�����������еĶ���<��ɫID,gate_FD>
        std::map<NFGUID, int> xRoleInfo;
    };

public:
    virtual NFINetModule* GetNetModule() = 0;
    virtual void SendMsgPBToGate(const uint16_t nMsgID, google::protobuf::Message& xMsg, const NFGUID& self) = 0;
    virtual void SendMsgPBToGate(const uint16_t nMsgID, const std::string& strMsg, const NFGUID& self) = 0;
    virtual bool AddPlayerGateInfo(const NFGUID& nRoleID, const NFGUID& nClientID, const int nGateID) = 0;
    virtual bool RemovePlayerGateInfo(const NFGUID& nRoleID) = 0;
    virtual NF_SHARE_PTR<GateBaseInfo> GetPlayerGateInfo(const NFGUID& nRoleID) = 0;

    virtual int OnPropertyEnter(const AFDataList& argVar, const NFGUID& self) = 0;
    virtual int OnRecordEnter(const AFDataList& argVar, const NFGUID& self) = 0;

    virtual int OnObjectListEnter(const AFDataList& self, const AFDataList& argVar) = 0;
    virtual int OnObjectListLeave(const AFDataList& self, const AFDataList& argVar) = 0;
};

#endif