#pragma once
#include "GameObjAttribute.h"
#include "CDestroyAnime.h"

class CGameEngine;

inline std::vector<std::vector<std::string> > AllEnemyType(void)
{
    return { {" **   ** ",
              "  ** **  ",
              "   * *   ",
              "*********",
              "* ***** *",
              "*  ***  *",
              "    *    "} };
};

class CEnemy : public stGameObjAttrib
{
public:
    CEnemy(CGameEngine *pobjGame, st2D responseRate, st2D pixelSpeedPerCtrl, st2D coordStart, COLOUR enemyColor);

    void Draw(void) override;
    int Control(void);
    bool IsNeedDestroy(void);

private:
    CDestroyAnime m_objDestory;
    int m_iLastXPos;
    int m_iLastXMoveLimit;
};
