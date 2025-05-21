#include "CSpaceShip.h"
#include "CGameEngine.h"
#include <algorithm>

//template<>
//void stGameObjAttrib::CheckAndRemoveObj(std::vector<CMissile>& vecObj, std::function<bool(CMissile& obj)> funcConditionNeedRemove);

CSpaceShip::CSpaceShip(CGameEngine *pobjGame, st2D responseRate, st2D pixelSpeedPerCtrl, st2D coordStart):
    stGameObjAttrib(pobjGame, SPACE_SHIP_IDENTIFIER, 1, responseRate, pixelSpeedPerCtrl, eWHITE),
    m_objDestory{ pobjGame, st2D{CONST_REFRESH_DESTROY_ANIME_IN_MS}, coordStart, eWHITE },
    m_bFirePressed(false)
{
    if (pobjGame)
    {
        // 11 X 5
        /*m_vecSprite = {"     *     ",
                         "    ***    ",
                         "*  *****  *",
                         "***********",
                         "***********" };*/

        m_vecSprite = {"      *      ",
                       "     ***     ",
                       "   * *** *   ",
                       "  ** *** **  ",
                       " *********** ",
                       "*************",
                       "   * *** *   ",
                       "      *      "};

        /*m_vecSprite = {  "     *     ",
                         "    ***    ",
                         "    ***    ",
                         "  * *** *  ",
                         "  *******  ",
                         " ********* ",
                         "****   ****",
                         " *       * " };*/

        const int iSpriteHeight = static_cast<int>(m_vecSprite.size());
        const int iSpriteWidth = static_cast<int>(m_vecSprite[0].length());

        const int iScreenWidth = m_pObjGame->GetScreenSize().X();
        const int iScreenHeight = m_pObjGame->GetScreenSize().Y();

        CheckAndSetLmtBoundaryToObj(coordStart,
                                    st2DRange(0, iScreenWidth - iSpriteWidth),      // X Limit
                                    st2DRange(0, iScreenHeight - iSpriteHeight));   // Y Limit
    }
}

void CSpaceShip::Draw(void)
{
    if (m_pObjGame)
    {
        if (GetLife() > 0)
        {
            m_pObjGame->DrawSprite(m_stObjTLPos, m_vecSprite, m_eColor, this);
        }
        else
        {
            // Play Destroy Sound 
            if (!m_bDestroySoundPlayed)
            {
                GetGameEngine()->PlayGameSound(eSHIP_DIE, SND_ASYNC | SND_NOSTOP);
                m_bDestroySoundPlayed = true;
            }

            // If SpaceShip Got killed
            m_objDestory.SetPos(m_stObjTLPos);
            m_objDestory.Draw();
        }

        for (auto &pobjMissile : m_vecFiredMissile)
        {
            pobjMissile->Draw();
        }
    }
}

void CSpaceShip::Control(void)
{
    if (m_pObjGame)
    {
        if (GetLife() > 0)
        {
            if (m_pObjGame->IsNeedRefresh(m_tLastDrawTime[0], m_stResponseRate.val1)) // would update the last time inside
            {
                if (GetAsyncKeyState(VK_UP) & 0x8000)
                {
                    m_stObjTLPos.YMove(-1 * m_stPixelSpeedPerCtrl.val2);
                }
                else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
                {
                    m_stObjTLPos.YMove(m_stPixelSpeedPerCtrl.val2);
                }

                if (GetAsyncKeyState(VK_LEFT) & 0x8000)
                {
                    m_stObjTLPos.XMove(-1 * m_stPixelSpeedPerCtrl.val1);
                }
                else if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
                {
                    m_stObjTLPos.XMove(m_stPixelSpeedPerCtrl.val1);
                }

                if (GetAsyncKeyState('S') & 0x8000)
                {
                    if (!m_bFirePressed)
                    {
                        m_bFirePressed = true;

                        GetGameEngine()->PlayGameSound(eSHOOT, SND_ASYNC | SND_NOSTOP);

                        const int iSpriteWidthHalf = static_cast<int>(m_vecSprite[0].length() / 2);
                        // Fire at the top tip of the space ship
                        m_vecFiredMissile.emplace_back(std::make_unique<CMissile>(m_pObjGame, 
                                                                            st2D{ 0, CONST_PER_CONTROL_MISSILE_IN_MS },                     // control in Y dir only
                                                                            st2D{ 0, DEFAULT_MISSILE_PIXEL_MOVE_SPEED },                    // move in Y dir only
                                                                            st2D{ m_stObjTLPos.X() + iSpriteWidthHalf, m_stObjTLPos.Y() },  // start pos
                                                                            eWHITE));                                                       // color
                    }
                }
                else
                {
                    m_bFirePressed = false;
                }
            }
        }
        else
        {
            m_objDestory.Control();
        }

        // Missile Fire Path Run
        for (auto& pobjMissile : m_vecFiredMissile)
        {
            pobjMissile->Control();
        }

        // Check Any Missile Out Of Screen, OR
        // If the Life become zero or less
        // To Remove Obj
        stGameObjAttrib::CheckAndRemoveObj<std::unique_ptr<CMissile>>(m_vecFiredMissile,
                                            [](std::unique_ptr<CMissile>& pobj) {return pobj->IsNeedDestroy(); });
    }
}
