#pragma once
#include <windows.h>
#include <memory>
#include <unordered_set>

#include "CoordUtility.h"
#include "GameObjAttribute.h"
#include "CSound.h"

class CGameEngine
{
public:
    CGameEngine(unsigned int width = 100, unsigned int height = 100, unsigned int pixel = 8);
    ~CGameEngine();

    struct stOverlapDetection
    {
        stOverlapDetection()
        {
            setOverlappedObj.clear(); // ensure
        }
        std::unordered_set<stGameObjAttrib*> setOverlappedObj;
    };
   
    std::string ConstructConsoleWindow(void);
    st2DXY GetScreenSize(void);
   
    // "0x2588" is full block wchar
    void Draw(unsigned int x, unsigned int y, COLOUR color, wchar_t c,
                stGameObjAttrib *pObj, stOverlapDetection *pDetection);
    void Draw(unsigned int x, unsigned int y, COLOUR color = eBLACK, wchar_t c = FULL_BLOCK_WCHAR);
    void DrawRect(st2DXY stPt1, st2DXY stPt2, COLOUR color = eBLACK);
    void DrawSprite(st2DXY stPtStart, const std::vector<std::string> &vecSprite, COLOUR color,
                    stGameObjAttrib *pObj, stOverlapDetection *pDetection = nullptr);
    void DrawString(unsigned int x, unsigned int y, const std::wstring &str, COLOUR color = eWHITE);
    void DrawStringSprite(st2DXY stPtStart, const std::string &str, COLOUR color, eSPRITE_CHAR_SIZE eCharSize = eCHAR_SMALL);

    static int GetStringSpriteTotalWidth(const std::string &str, eSPRITE_CHAR_SIZE eCharSize = eCHAR_SMALL);
   
    //void StartBoard(std::wstring wstr, unsigned int iFontPixelSize = 0, int COLOUR color = eWHITE);
   
    bool IsNeedRefresh(std::chrono::steady_clock::time_point &tObjLastDrawTime, int iRefreshRate);
    void GameFrameDraw(void);

    bool StopSound(void) const;
    bool PlayGameSound(eGAME_SOUND eSound, DWORD flagOption) const;

private:
    unsigned int m_iWidth, m_iHeight, m_iPixelSize;
    HANDLE m_hConsole;
    SMALL_RECT m_rectWin;
    CSound m_objSound;

    // Board Info, CHAR_INFO -> rendering
    struct stBoardInfo
    {
        stBoardInfo() :
            mp_Screen(nullptr), mp_setObj(nullptr)
        {}
        ~stBoardInfo()
        {
            if (mp_Screen != nullptr)
            {
                delete[] mp_Screen;
                mp_Screen = nullptr;
            }

            if (mp_setObj != nullptr)
            {
                delete[] mp_setObj;
                mp_setObj = nullptr;
            }
        }

        CHAR_INFO *mp_Screen;
        std::unordered_set<stGameObjAttrib*> *mp_setObj;

    } m_stBoard;

    std::string ReturnError(std::string sMsg)
    {
        char cBuf[256];
        sprintf_s(cBuf, sizeof(cBuf), "%s, GetLast : %d", sMsg.c_str(), GetLastError());
        return std::string(cBuf);
    }

};

