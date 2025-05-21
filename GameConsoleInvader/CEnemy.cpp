#include "CEnemy.h"
#include "CDestroyAnime.h"
#include "CGameEngine.h"

#include <cstdlib>

CEnemy::CEnemy(CGameEngine *pobjGame, st2D responseRate, st2D pixelSpeedPerCtrl, st2D coordStart, COLOUR enemyColor) :
    stGameObjAttrib(pobjGame, ENEMY_IDENTIFIER, 1, responseRate, pixelSpeedPerCtrl, enemyColor),
    m_iLastXMoveLimit{ 0 }, m_iLastXPos{ -1 },
    m_objDestory{pobjGame, CONST_REFRESH_DESTROY_ANIME_IN_MS, coordStart, enemyColor }
{
    if (pobjGame)
    {
        // Temp has the only one type
        m_vecSprite = AllEnemyType()[0];

        const int iSpriteHeight = static_cast<int>(m_vecSprite.size());
        const int iSpriteWidth = static_cast<int>(m_vecSprite[0].length());

        const int iScreenWidth = m_pObjGame->GetScreenSize().X();
        const int iScreenHeight = m_pObjGame->GetScreenSize().Y();

        CheckAndSetLmtBoundaryToObj(coordStart,
                                    st2DRange(0 - iSpriteWidth, iScreenWidth + iSpriteWidth),
                                    st2DRange(0 - iSpriteHeight, iScreenHeight + iSpriteHeight));
    }
}

void CEnemy::Draw(void)
{
    if (m_pObjGame)
    {
        if (GetLife() > 0)
        {
            CGameEngine::stOverlapDetection stDetect;
            m_pObjGame->DrawSprite(m_stObjTLPos, m_vecSprite, m_eColor, this, &stDetect);

            for (const auto &pOverlapObj : stDetect.setOverlappedObj)
            {
                const auto iObjType = pOverlapObj->GetObjType();

                switch (iObjType)
                {
                    // Decrease the Enemy Life by 1 when hit by every MISSILE_IDENTIFIER
                    // At the same time destroy the missile 
                    case MISSILE_IDENTIFIER:
                        Life(-1);
                        pOverlapObj->Life(-1);
                        break;
                        // If hit space ship, decrease the space ship life by 1
                    case SPACE_SHIP_IDENTIFIER:
                        pOverlapObj->Life(-1);
                        break;
                    default:
                        break;
                }
            }

            // If Enemy Got killed
            if (GetLife() <= 0)
            {
                // Play Destroy Sound 
                if (!m_bDestroySoundPlayed)
                {
                    GetGameEngine()->PlayGameSound(eENEMY_DIE, SND_ASYNC | SND_NOSTOP);
                    m_bDestroySoundPlayed = true;
                }

                m_objDestory.SetPos(m_stObjTLPos);
            }
        }
        else
        {
            m_objDestory.Draw();
        }
    }
}

int CEnemy::Control(void)
{
    int iDestroyed = 0;
    if (m_pObjGame)
    {
        if (GetLife() > 0)
        {
            if (m_pObjGame->IsNeedRefresh(m_tLastDrawTime[0], m_stResponseRate.val2)) // would update the last time inside
            {
                // Motion pattern
                // <-----> Random from 1 to pixel speed left and right 
                //    |
                //    V    continue move down 1 pixel speed
                // when Current Pos is still smaller than the its lower limit
                // Continue to move downward

                if (m_stObjTLPos.Y() < m_stObjTLPos.GetYLmt().val2)
                {
                    // move 1 pixel downward
                    m_stObjTLPos.YMove(m_stPixelSpeedPerCtrl.val2);
                }
            }

            if (m_pObjGame->IsNeedRefresh(m_tLastDrawTime[1], m_stResponseRate.val1)) // would update the last time inside
            {
                int iMoveStep = m_iLastXMoveLimit >= 0 ? 1 : -1;

                if (m_iLastXPos == -1 || abs(m_stObjTLPos.X() - m_iLastXPos) > abs(m_iLastXMoveLimit))
                {
                    m_iLastXPos = m_stObjTLPos.X();
                    int iMoveRange = std::rand() % m_stPixelSpeedPerCtrl.val1 + 1;

                    if (m_iLastXMoveLimit >= 0)
                    {
                        iMoveRange *= -1;
                    }
                    m_iLastXMoveLimit = iMoveRange;
                }
                m_stObjTLPos.XMove(iMoveStep);
            }
        }
        else
        {
            m_objDestory.Control();
            iDestroyed = m_objDestory.GetLife() <= 0 ? 1 : 0;
        }
    }
    return iDestroyed;
}

bool CEnemy::IsNeedDestroy(void)
{
    return (GetLife() <= 0 && m_objDestory.GetLife() <= 0) ||
            m_stObjTLPos.IsOutOfBoundary();
}
