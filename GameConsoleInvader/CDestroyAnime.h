#pragma once
#include "GameObjAttribute.h"

class CGameEngine;

inline std::vector<std::vector<std::string> > vecDestroyAnime(void)
{
    return { {"         ",
              "         ",
              "    *    ",
              "  * * *  ",
              "    *    ",
              "         ",
              "         "},

             {"         ",
              "    *    ",
              "  *   *  ",
              "*   *   *",
              "  *   *  ",
              "    *    ",
              "         "},
                
             {"    *    ",
              "*       *",
              "  *   *  ",
              "*   *   *",
              "  *   *  ",
              "*       *",
              "    *    "}};
};

class CDestroyAnime : public stGameObjAttrib
{
public:
    CDestroyAnime(CGameEngine* pobjGame, st2D responseRate, st2D coordStart, COLOUR color);

    void Draw(void) override;

    void Control(void);

    void SetPos(const st2DXY &coord)
    {
        m_stObjTLPos = coord;
    }

    bool IsDestroyAnimeFinshed(void)
    {
        return GetLife() <= 0;
    }

private:
    unsigned int m_iAnimeIndex;
    bool m_bIsFirstDraw, m_bIsFirstControl;
};
