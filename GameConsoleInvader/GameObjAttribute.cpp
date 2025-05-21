#include "GameObjAttribute.h"
#include "CGameEngine.h"

void stGameObjAttrib::CheckAndSetLmtBoundaryToObj(st2D coordStart, st2DRange stXLmt, st2DRange stYLmt)

{
    if (m_pObjGame)
    {
        const int iScreenWidth = m_pObjGame->GetScreenSize().X();
        const int iScreenHeight = m_pObjGame->GetScreenSize().Y();

        // Set Limit And Check X, Y
        m_stObjTLPos = st2DXY(coordStart);
        m_stObjTLPos.SetXLmt(stXLmt);
        m_stObjTLPos.SetYLmt(stYLmt);

        // Try move 0 to auto correct to limit boundary if necessary
        m_stObjTLPos.XMove(0);
        m_stObjTLPos.YMove(0);
    }
}