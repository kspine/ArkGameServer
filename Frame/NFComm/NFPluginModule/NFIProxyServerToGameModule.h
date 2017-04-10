// -------------------------------------------------------------------------
//    @FileName         ��    NFIProxyServerToGameModule.h
//    @Author           ��    Ark Game Tech
//    @Date             ��    2012-12-15
//    @Module           ��    NFIProxyServerToGameModule
//
// -------------------------------------------------------------------------

#ifndef NFI_PROXYNET_TO_GAME_H
#define NFI_PROXYNET_TO_GAME_H

#include <iostream>
#include "NFIModule.h"
#include "NFINetClientModule.hpp"

class NFIProxyServerToGameModule
    : public  NFIModule
{
public:
	virtual NFINetClientModule* GetClusterModule() = 0;
};

#endif