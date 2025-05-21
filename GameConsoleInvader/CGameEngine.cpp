#include "CGameEngine.h"

#include <ctime>
#include <chrono>
#include <cctype>
#include <numeric>

using namespace std;

CGameEngine::CGameEngine(unsigned int width, unsigned int height, unsigned int pixel) :
    m_iWidth{ width }, m_iHeight{ height }, m_iPixelSize{ pixel },
    m_objSound{"./sound/"}
{
    m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // default to min
    m_rectWin = { 0, 0, 1, 1 };
}

CGameEngine::~CGameEngine()
{
    // m_stBoard already had destructor to handle data release

    CloseHandle(m_hConsole);
}

string CGameEngine::ConstructConsoleWindow(void)
{
    if (m_hConsole == INVALID_HANDLE_VALUE)
    {
        return "Console Invalid Handle\0";
    }

    // 1
    // using default m_rectWin = { 0, 0, 1, 1 }; to set a valid min Window Info
    // before Set Screen Buffer Size success
    // Well-Known Microsoft Bug
    m_rectWin = { 0, 0, 1, 1 };
    SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWin);

    // Set Output Console Screen Buffer Size
    COORD coord = { (short)m_iWidth, (short)m_iHeight };
    if (!SetConsoleScreenBufferSize(m_hConsole, coord))
    {
        return ReturnError("Set Console Screen Buffer Size Error");
    }

    // 2
    // Set the Pixel Size Attribute to Pixel Buffer Handle
    CONSOLE_FONT_INFOEX cfont;
    cfont.cbSize = sizeof(cfont);
    cfont.nFont = 0;
    cfont.dwFontSize.X = m_iPixelSize;
    cfont.dwFontSize.Y = m_iPixelSize;
    cfont.FontFamily = FF_DONTCARE;
    cfont.FontWeight = FW_NORMAL;
    wcscpy_s(cfont.FaceName, L"Consolas");
    if (!SetCurrentConsoleFontEx(m_hConsole, false, &cfont))
    {
        return ReturnError("Set Pixel Font Size Error");
    }

    // 3
    // Set The Real Console Window Size
    m_rectWin = { 0, 0, (short)m_iWidth - 1, (short)m_iHeight - 1 };
    if (!SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWin))
    {
        return ReturnError("Set Console Window Info");
    }

    // 4
    // Create Screen Buffer Character Memory and init all to 0
    m_stBoard.mp_Screen = new CHAR_INFO[m_iWidth * m_iHeight];
    if (m_stBoard.mp_Screen == nullptr)
    {
        return ReturnError("Set Screen Buffer - Out Of Memory");
    }
    memset(m_stBoard.mp_Screen, 0, sizeof(CHAR_INFO) * m_iWidth * m_iHeight);

    m_stBoard.mp_setObj = new unordered_set<stGameObjAttrib*>[m_iWidth * m_iHeight];
    if (m_stBoard.mp_setObj == nullptr)
    {
        return ReturnError("Set Screen Obj Buffer - Out Of Memory");
    }
    //memset(m_stBoard.mp_setObj, 0, sizeof(unordered_set<stGameObjAttrib*>) * m_iWidth * m_iHeight);

    // 5
    // Setup CallBack Handler when close
    //SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);


    // 6
    // Not allow console window to be resized
    HWND hwndConsole = GetConsoleWindow();
    DWORD dwStyle = GetWindowLong(hwndConsole, GWL_STYLE);
    dwStyle &= ~(WS_SIZEBOX | WS_MAXIMIZEBOX); // Remove size box and now allow maximize
    SetWindowLong(hwndConsole, GWL_STYLE, dwStyle);

    return "\0";
}

st2DXY CGameEngine::GetScreenSize(void)
{
    return st2DXY(m_iWidth, m_iHeight);
}

// "0x2588" is full block wchar
void CGameEngine::Draw(unsigned int x, unsigned int y, COLOUR color, wchar_t c,
                        stGameObjAttrib *pObj, stOverlapDetection* pDetection)
{
    if (x >= 0 && x < m_iWidth && y >= 0 && y < m_iHeight)
    {
        const auto iPos = y * m_iWidth + x;

        // Get the Location reference
        auto &pixel = m_stBoard.mp_Screen[iPos];

        pixel.Char.UnicodeChar = c;
        pixel.Attributes = color;

        // Get And Set Obj
        auto &setObj = m_stBoard.mp_setObj[iPos];
        if (!setObj.empty() && pDetection)
        {
            pDetection->setOverlappedObj = setObj;
        }
        // If had Obj to place, insert
        if (pObj)
        {
            setObj.insert(pObj);
        }
        // If had no Obj to place, clear that location Obj info
        else
        {
            if (!setObj.empty())
            {
                setObj.clear();
            }
        }
    }
}

void CGameEngine::Draw(unsigned int x, unsigned int y, COLOUR color, wchar_t c)
{
    Draw(x, y, color, c, nullptr, nullptr);
}

//  (pt1)+ ---- +              |--> x (+ve)
//       |      |              V
//       + ---- + (pt2)        y (+ve)
void CGameEngine::DrawRect(st2DXY stPt1, st2DXY stPt2, COLOUR color)
{
    for (auto i = stPt1.X(); i <= stPt2.X(); i++)
    {
        for (auto j = stPt1.Y(); j <= stPt2.Y(); j++)
        {
            Draw(i, j, color);
        }
    }
}

void CGameEngine::DrawSprite(st2DXY stPtStart, const vector<string> &vecSprite, COLOUR color,
                                stGameObjAttrib *pObj, stOverlapDetection* pDetection)
{
    const int iSpriteHeight = static_cast<int>(vecSprite.size());
    const int iSpriteWidth = static_cast<int>(vecSprite[0].length());

    for (auto i = stPtStart.X(); i < (stPtStart.X() + iSpriteWidth); i++)
    {
        for (auto j = stPtStart.Y(); j < (stPtStart.Y() + iSpriteHeight); j++)
        {
            const auto iSprite_X_idx = i - stPtStart.X();
            const auto iSprite_Y_idx = j - stPtStart.Y();

            // Check if sprite char is not space to draw the Sprite
            // if it is a space car in the spirte just leave not to draw
            // Keep what it is already had in the background
            if (!isspace(vecSprite[iSprite_Y_idx][iSprite_X_idx]))
            {
                Draw(i, j, color, FULL_BLOCK_WCHAR, pObj, pDetection);
            }
        }
    }
}

void CGameEngine::DrawString(unsigned int x, unsigned int y, const wstring &str, COLOUR color)
{
    for (size_t i = 0; i < str.size(); i++)
    {
        m_stBoard.mp_Screen[y * m_iWidth + x + i].Char.UnicodeChar = str[i];
        m_stBoard.mp_Screen[y * m_iWidth + x + i].Attributes = color;
    }
}

int CGameEngine::GetStringSpriteTotalWidth(const string& str, eSPRITE_CHAR_SIZE eCharSize)
{
    int iPrevCharSpriteWidth = 0;
    const auto &mapCharSprite = eCharSize == eCHAR_LARGE ? GetLargeCharSprite() : GetCharSprite();

    return accumulate(begin(str), end(str), 0, [&mapCharSprite, &iPrevCharSpriteWidth](int accSize, const auto &c)
        {
            if (mapCharSprite.find(c) != mapCharSprite.end())
            {
                const auto &vecSprite = mapCharSprite.at(c);
                const auto iSpriteWidth = static_cast<int>(vecSprite[0].length());
                iPrevCharSpriteWidth = iSpriteWidth + 1;
                return accSize + iPrevCharSpriteWidth;
            }
            else if (isspace(c)) // if is space char, just add an extra size from previous char size
            {
                return accSize + iPrevCharSpriteWidth / 2;
            }
            else
            {
                return accSize;
            }
        });
}

void CGameEngine::DrawStringSprite(st2DXY stPtStart, const string &str, COLOUR color, eSPRITE_CHAR_SIZE eCharSize)
{
    int iPrevCharSpriteWidth = 0;
    for (const auto &c : str)
    {
        const auto &mapCharSprite = eCharSize == eCHAR_LARGE ? GetLargeCharSprite() : GetCharSprite();
        if (mapCharSprite.find(c) != mapCharSprite.end())
        {
            const auto &vecSprite = mapCharSprite.at(c);
            const auto iSpriteHeight = static_cast<int>(vecSprite.size());
            const auto iSpriteWidth = static_cast<int>(vecSprite[0].length());

            stPtStart.XMove(iPrevCharSpriteWidth + 1);
            for (auto i = stPtStart.X(); i < (stPtStart.X() + iSpriteWidth); i++)
            {
                for (auto j = stPtStart.Y(); j < (stPtStart.Y() + iSpriteHeight); j++)
                {
                    const auto iSprite_X_idx = i - stPtStart.X();
                    const auto iSprite_Y_idx = j - stPtStart.Y();

                    // Check if sprite char is not space to draw the Sprite
                    // if it is a space car in the spirte just leave not to draw
                    // Keep what it is already had in the background
                    if (!isspace(vecSprite[iSprite_Y_idx][iSprite_X_idx]))
                    {
                        Draw(i, j, color, FULL_BLOCK_WCHAR);
                        iPrevCharSpriteWidth = iSpriteWidth;
                    }
                }
            }
        }
        else if (isspace(c))
        {
            stPtStart.XMove(iPrevCharSpriteWidth / 2);
        }
    }
}

bool CGameEngine::IsNeedRefresh(std::chrono::steady_clock::time_point &tObjLastDrawTime, int iRefreshRate)
{
    const auto tCheckTime = chrono::steady_clock::now();
    const auto tDurationCountInMS = std::chrono::duration_cast<MS>(tCheckTime - tObjLastDrawTime).count();

    if (tDurationCountInMS <= 0 ||
        tDurationCountInMS > iRefreshRate)
    {
        tObjLastDrawTime = tCheckTime;
        return true;
    }
    return false;
}

void CGameEngine::GameFrameDraw(void)
{
    COORD bufferSize = { (short)GetScreenSize().X(), (short)GetScreenSize().Y() };
    COORD coordDraw = { 0, 0 };
    WriteConsoleOutput(m_hConsole, m_stBoard.mp_Screen, bufferSize, coordDraw, &m_rectWin);

    // Every 1ms update the whole screen
    SetConsoleActiveScreenBuffer(m_hConsole);
}

bool CGameEngine::StopSound(void) const
{
    return m_objSound.StopSound();
}

bool CGameEngine::PlayGameSound(eGAME_SOUND eSound, DWORD flagOption) const
{
    return m_objSound.PlayGameSound(eSound, flagOption);
}