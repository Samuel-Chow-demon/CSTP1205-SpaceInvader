#include "CMissile.h"
#include "CGameEngine.h"

CMissile::CMissile(CGameEngine *pobjGame, st2D responseRate, st2D pixelSpeedPerCtrl, st2D coordStart, COLOUR color) :
    stGameObjAttrib(pobjGame, MISSILE_IDENTIFIER, 1, responseRate, pixelSpeedPerCtrl, color)
{
    if (pobjGame)
    {
        /*m_vecSprite = {"*",
                         "*"};*/

        m_vecSprite = {"*"};

        /*m_vecSprite = { " *** *** ",
                        "*********", 
                        " ******* ",
                        "  *****  ",
                        "   ***   " };*/

        const int iSpriteHeight = static_cast<int>(m_vecSprite.size());
        const int iSpriteWidth = static_cast<int>(m_vecSprite[0].length());

        const int iScreenWidth = m_pObjGame->GetScreenSize().X();
        const int iScreenHeight = m_pObjGame->GetScreenSize().Y();

        coordStart.val1 = coordStart.val1 - (iSpriteWidth / 2);
        CheckAndSetLmtBoundaryToObj(coordStart,
                                    st2DRange(0, iScreenWidth - iSpriteWidth),          // X Limit
                                    st2DRange(-5, iScreenHeight - iSpriteHeight));      // Y Limit
    }
}

void CMissile::Draw(void)
{
    if (m_pObjGame)
    {
        m_pObjGame->DrawSprite(m_stObjTLPos, m_vecSprite, m_eColor, this);
    }
}

void CMissile::Control(void)
{
    if (m_pObjGame)
    {
        if (m_pObjGame->IsNeedRefresh(m_tLastDrawTime[0], m_stResponseRate.val2)) // would update the last time inside
        {
            // when Current Pos is still larger than the its lower limit
            if (m_stObjTLPos.Y() > m_stObjTLPos.GetYLmt().val1)
            {
                m_stObjTLPos.YMove(-1 * m_stPixelSpeedPerCtrl.val2);
            }
        }
    }
}

bool CMissile::IsNeedDestroy(void)
{
    return GetLife() <= 0 ||
            m_stObjTLPos.IsOutOfBoundary();
}
