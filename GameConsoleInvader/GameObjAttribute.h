#pragma once
#include "CoordUtility.h"
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <memory>
#include <type_traits>
#include "CharacterSprite.h"

#define SPACE_SHIP_IDENTIFIER   0x0001
#define MISSILE_IDENTIFIER      0x0002
#define ENEMY_IDENTIFIER        0x0004
#define DESTORY_TAG_IDENTIFIER  0x0008

#define MS      std::chrono::milliseconds

#define FULL_BLOCK_WCHAR     0x2588

#define CONST_PER_CONTROL_SHIP_IN_MS         15
#define CONST_PER_CONTROL_MISSILE_IN_MS      25
#define CONST_PER_CONTROL_ENEMY_PER_MS       100
#define CONST_REFRESH_DESTROY_ANIME_IN_MS    250

#define DEFAULT_SHIP_PIXEL_MOVE_SPEED        1
#define DEFAULT_ENEMY_PIXEL_MOVE_SPEED_IN_X  20
#define DEFAULT_ENEMY_PIXEL_MOVE_SPEED_IN_Y  1
#define DEFAULT_MISSILE_PIXEL_MOVE_SPEED     1

enum COLOUR
{
    eBLACK = 0,
    eDARK_BLUE = 0x0001,
    eDARK_GREEN = 0x0002,
    eDARK_RED = 0x0004,
    eFG_INTEN = 0x0008,
    eWHITE = 0x000F
};

// overwrite enum COLOUR can be operated with |
inline COLOUR operator|(COLOUR a, COLOUR b)
{
    return static_cast<COLOUR>(static_cast<int>(a) | static_cast<int>(b));
}

// Set the enum COLOUR can be operated with | & ^
//DEFINE_ENUM_FLAG_OPERATORS(COLOUR);

class CGameEngine;

struct stGameObjAttrib
{
    stGameObjAttrib(CGameEngine *pobjGame, int objType, unsigned int life, st2D responseRate, st2D pixelSpeedPerCtrl, COLOUR color) :
        m_pObjGame{ pobjGame }, m_iObjectType{ objType },
        m_stResponseRate{ responseRate }, m_stPixelSpeedPerCtrl{ pixelSpeedPerCtrl },
        m_iLife{ life }, m_eColor{ color },
        m_bDestroySoundPlayed{false}
    {}

    virtual void Draw(void) = 0;

    void CheckAndSetLmtBoundaryToObj(st2D coordStart, st2DRange stXLmt, st2DRange stYLmt);

    template<typename T>
    static void CheckAndRemoveObj(std::vector<T> &vecObj, std::function<bool(T &obj)> funcConditionNeedRemove)
    {
        vecObj.erase(remove_if(begin(vecObj), end(vecObj), funcConditionNeedRemove), end(vecObj));
    }

    unsigned int GetLife(void) const
    {
        return m_iLife;
    }
    void Life(int iModSize)
    {
        m_iLife += iModSize;
    }
    int GetObjType(void)
    {
        return m_iObjectType;
    }
    const CGameEngine* GetGameEngine(void)
    {
        return m_pObjGame;
    }
    
protected:
    CGameEngine *m_pObjGame;
    // A Basic Rectangle
    //  (Pos Pt)+ ---- +              |--> x (+ve)
    //          |      |              V
    //          + ---- +              y (+ve)
    st2DXY m_stObjTLPos;
    std::vector<std::string> m_vecSprite;
    std::chrono::steady_clock::time_point m_tLastDrawTime[2]; // 2D Refresh Capable
    st2D m_stResponseRate;                                    // Sampling for object control
    st2D m_stPixelSpeedPerCtrl;                               // Move Pixel Per Control
    bool m_bDestroySoundPlayed;
    COLOUR m_eColor;

private:
    unsigned int m_iLife;
    int m_iObjectType; // SPACE_SHIP_IDENTIFIER, MISSILE_IDENTIFIER, ENEMY_IDENTIFIER
};