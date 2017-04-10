// -------------------------------------------------------------------------
//    @FileName         :    NFISceneProcessModule.h
//    @Author           :    Ark Game Tech
//    @Date             :    2013-04-14
//    @Module           :    NFISceneProcessModule
//
// -------------------------------------------------------------------------

#ifndef NFI_SCENEPROCESS_MODULE_H
#define NFI_SCENEPROCESS_MODULE_H

#include <iostream>
#include "NFIModule.h"

enum E_SCENE_TYPE
{
    SCENE_TYPE_NORMAL,//����
    SCENE_TYPE_CLONE_SCENE,//����

    SCENE_TYPE_ERROR,
};

class NFISceneProcessModule
    : public NFIModule
{

public:
    virtual bool IsCloneScene(const int nSceneID) = 0;
    virtual E_SCENE_TYPE GetCloneSceneType(const int nContainerID) = 0;
    virtual bool ApplyCloneGroup(const int nSceneID, int& nGroupID) = 0;
    virtual bool ExitCloneGroup(const int nSceneID, const int& nGroupID) = 0;
};

#endif