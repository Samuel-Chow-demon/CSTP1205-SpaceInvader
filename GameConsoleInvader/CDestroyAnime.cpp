#include "CDestroyAnime.h"
#include "CGameEngine.h"

CDestroyAnime::CDestroyAnime(CGameEngine *pobjGame, st2D responseRate, st2D coordStart, COLOUR color) :
    stGameObjAttrib(pobjGame, DESTORY_TAG_IDENTIFIER, 1, responseRate, st2D(), color),            // Destroy Anime no pixel speed
    m_iAnimeIndex{ 0 }, m_bIsFirstDraw{ true }, m_bIsFirstControl{true}
{
    if (pobjGame)
    {
        // Get the Sprite For dimension
        m_vecSprite = vecDestroyAnime()[m_iAnimeIndex];

        const int iSpriteHeight = static_cast<int>(m_vecSprite.size());
        const int iSpriteWidth = static_cast<int>(m_vecSprite[0].length());

        const int iScreenWidth = m_pObjGame->GetScreenSize().X();
        const int iScreenHeight = m_pObjGame->GetScreenSize().Y();

        CheckAndSetLmtBoundaryToObj(coordStart,
                                    st2DRange(0 - iSpriteWidth, iScreenWidth + iSpriteWidth),
                                    st2DRange(0 - iSpriteHeight, iScreenHeight + iSpriteHeight));
    }
}

void CDestroyAnime::Draw(void)
{
    if (m_pObjGame && GetLife() > 0)
    {
        m_pObjGame->DrawSprite(m_stObjTLPos, m_vecSprite, m_eColor, this);
        m_bIsFirstDraw = false;
    }
}

void CDestroyAnime::Control(void)
{
    if (m_pObjGame)
    {
        if (GetLife() > 0 && 
            m_pObjGame->IsNeedRefresh(m_tLastDrawTime[0], m_stResponseRate.val1) && // would update the last time inside
            !(m_bIsFirstDraw && m_bIsFirstControl))
        {
            const auto &vec = vecDestroyAnime();

           // No Move for Destroy Anime
            m_iAnimeIndex++;
            if (m_iAnimeIndex < vec.size())
            {
                m_vecSprite = vec[m_iAnimeIndex];
            }

            // If all the anime frame displayed, set life to -1 for destroy
            if (m_iAnimeIndex >= vec.size())
            {
                Life(-1);
            }
        }

        m_bIsFirstControl = false;
    }
}
