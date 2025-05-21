#pragma once
#include "GameObjAttribute.h"

class CGameEngine;

class CMissile : public stGameObjAttrib
{
public:
    CMissile(CGameEngine *pobjGame, st2D responseRate, st2D pixelSpeedPerCtrl, st2D coordStart, COLOUR color);

    void Draw(void) override;
    void Control(void);
    bool IsNeedDestroy(void);

private:
};

