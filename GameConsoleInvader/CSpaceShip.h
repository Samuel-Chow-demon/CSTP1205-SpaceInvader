#pragma once
#include "GameObjAttribute.h"
#include "CMissile.h"
#include "CDestroyAnime.h"

//class CMissile;

class CSpaceShip : public stGameObjAttrib
{
public:
    CSpaceShip(CGameEngine *pobjGame, st2D responseRate, st2D pixelSpeedPerCtrl, st2D coordStart);

    void Draw(void) override;
    void Control(void);

    st2DXY ReturnCurPos(void) const
    {
        return m_stObjTLPos;
    }

    bool IsShipDestroyed(void)
    {
        return m_objDestory.IsDestroyAnimeFinshed();
    }

private:
    std::vector<std::unique_ptr<CMissile>> m_vecFiredMissile;
    CDestroyAnime m_objDestory;
    bool m_bFirePressed;
};
