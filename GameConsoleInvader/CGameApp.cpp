#include "CGameApp.h"

#include <iostream>
#include "CGameEngine.h"

#include <cstdlib>
#include <ctime>
#include <memory>

using namespace std;

// Static Engine Board 100 x 100 char with each char 8 x 8 pixel
CGameEngine CGameApp::sm_GameEng{ 100, 100, 8 };

CGameApp::CGameApp() :
    m_szGameBoardStatus{"\0"},
    m_eGameState{ eSTART }, m_eResult{ eINVALID }
{
    // Set the random seed by time
    srand(static_cast<unsigned int>(time(0)));

    // Create the Game Engine Console Handle
    m_szGameBoardStatus = sm_GameEng.ConstructConsoleWindow();

    // Create the Game Design
    GameStageDesign();
}

void CGameApp::GameStageDesign(void)
{
    // 0 - Design Start Stage
    stStageDesign stStageStart;
    stStageStart.bNeedSpaceShip = false;
    stStageStart.eEnemyColor = eDARK_BLUE;
    m_mapGameStageCfg[eSTART] = stStageObject(stStageStart);


    // 1A - Design Playing Stage 1
    m_mapGameStageCfg[eGAME] = stStageObject(); // the default design already fulfill for playing
    auto &stage = m_mapGameStageCfg[eGAME];
    auto &stDesign = stage.stDesign;
    stDesign.iKillNumOfEnemyToWin = 10;
    // When enemy destroy to a designed value
    stDesign.funcIsStageClearFulFilled = [&stage, &stDesign]()->bool
    {
        return stage.iStageEnemyDestroyed >= stDesign.iKillNumOfEnemyToWin;
    };


    // 1B - Design Playing Stage 2
    stStageDesign stDesignGame2;
    stDesignGame2.eEnemyColor = eDARK_BLUE | eDARK_RED;
    stDesignGame2.iKillNumOfEnemyToWin = 5;
    stDesignGame2.iMaxEnemyAtSameTime = 30;
    stDesignGame2.stEnemyResponseRate = st2D{ CONST_PER_CONTROL_ENEMY_PER_MS / 2, CONST_PER_CONTROL_ENEMY_PER_MS / 2};
    stDesignGame2.stEnemyPixelSpeed = st2D{ DEFAULT_ENEMY_PIXEL_MOVE_SPEED_IN_X * 3 , DEFAULT_ENEMY_PIXEL_MOVE_SPEED_IN_Y * 3 };

    auto &stage2 = m_mapGameStageCfg[eGAME_2];
    stage2 = stStageObject(stDesignGame2);
    auto &stDesign2 = stage2.stDesign;
    stDesign2.funcIsStageClearFulFilled = [&stage2, &stDesign2]()->bool
    {
        return stage2.iStageEnemyDestroyed >= stDesign2.iKillNumOfEnemyToWin;
    };


    // 2 - Design End Stage
    stStageDesign stStageEnd;
    stStageEnd.bNeedSpaceShip = false;
    stStageEnd.eEnemyColor = eDARK_BLUE | eDARK_RED; // Display in Purple 
    m_mapGameStageCfg[eEND] = stStageObject(stStageEnd);
}

void CGameApp::GameDrawEmptyBoard(void)
{
    const auto &stScreenSize = sm_GameEng.GetScreenSize();
    sm_GameEng.DrawRect(st2DXY(0, 0), st2DXY(stScreenSize.X() - 1, stScreenSize.Y() - 1), eBLACK);
}

void CGameApp::GameEndHandle(eGAME_RESULT eResult)
{
    // Although using the SND_NOSTOP, it doesn't mean can play two sounds alongside, it is either wait or would not play the new sound at all
    // Thus, a acceptable wait for the ship or enemy explosion sound before game end music to play
    Sleep(600);
    sm_GameEng.PlayGameSound(eResult == eWIN ? eSOUND_WIN : eSOUND_LOSE, SND_ASYNC | SND_NOSTOP);
    m_mapGameStageCfg[eEND].StartNewStage(); // Refresh Create necessary object
    SetState(eEND);
}

int CGameApp::GameStart(void)
{
    auto &stage = m_mapGameStageCfg[eSTART];

    // If just start the App
    if (stage.pEnemyFactory == nullptr)
    {
        sm_GameEng.PlayGameSound(eSTART_BACKGROUND, SND_ASYNC | SND_LOOP);
        stage.StartNewStage(); // Create necessary object
    }
        
    const int iMsgBufSize = 32;
    char cBuf[iMsgBufSize];

    const int iScreenWidth = sm_GameEng.GetScreenSize().X();

    // 1.1 - Draw the Enemy As the Background
    stage.pEnemyFactory->Draw();
    stage.pEnemyFactory->Control();

    // 1.2 - Draw Title and Control Description
    sprintf_s(cBuf, iMsgBufSize, "SPACE INVADER");
    const auto str1 = string(cBuf);
    const auto str1Width = CGameEngine::GetStringSpriteTotalWidth(str1, eCHAR_LARGE);
    sm_GameEng.DrawStringSprite(st2DXY((iScreenWidth - str1Width) / 2, 25), str1, eDARK_RED, eCHAR_LARGE);

    sprintf_s(cBuf, iMsgBufSize, "MOVE ^ ! > <");
    const auto str2 = string(cBuf);
    const auto str2Width = CGameEngine::GetStringSpriteTotalWidth(str2);
    sm_GameEng.DrawStringSprite(st2DXY((iScreenWidth - str2Width) / 2, 40), str2, eWHITE);

    sprintf_s(cBuf, iMsgBufSize, "FIRE \"S\"");
    const auto str3 = string(cBuf);
    const auto str3Width = CGameEngine::GetStringSpriteTotalWidth(str3);
    sm_GameEng.DrawStringSprite(st2DXY((iScreenWidth - str3Width) / 2, 50), str3, eWHITE);

    sprintf_s(cBuf, iMsgBufSize, "PRESS \"%c\" START", CONST_CHAR_TO_START);
    const auto str4 = string(cBuf);
    const auto str4Width = CGameEngine::GetStringSpriteTotalWidth(str4);
    sm_GameEng.DrawStringSprite(st2DXY((iScreenWidth - str4Width) / 2, 80), str4, eWHITE);

    // 1.3 Check if G pressed to switch to game state
    if (GetAsyncKeyState(CONST_CHAR_TO_START) & 0x8000)
    {
        sm_GameEng.PlayGameSound(eSTART_BUTTON, SND_ASYNC);
        m_mapGameStageCfg[eGAME].StartNewStage(); // Refresh Create necessary object
        SetState(eGAME);
    }

    return 0;
}

// 2 - Game Playing Stage 1
int CGameApp::GamePlay(void)
{
    auto &stage = m_mapGameStageCfg[eGAME];

    // If object invalid
    if (stage.pEnemyFactory == nullptr ||
        stage.pShip == nullptr)
    {
        stage.StartNewStage(); // Create necessary object
    }
    
    const int iMsgBufSize = 10;
    char cBufScore[iMsgBufSize];

    // 2.1 - Draw and Control the Ship
    stage.pShip->Draw();
    stage.pShip->Control();

    // 2.2 - Draw the Enemy That Control By Factory
    stage.pEnemyFactory->Draw();
    stage.iStageEnemyDestroyed += stage.pEnemyFactory->Control();

    // 2.3 - Print Information
    sprintf_s(cBufScore, iMsgBufSize, "%02d/%02d", stage.iStageEnemyDestroyed, stage.stDesign.iKillNumOfEnemyToWin);
    sm_GameEng.DrawStringSprite(st2DXY(5, 5), string(cBufScore), eWHITE);

    // 2.4 - Check if Game End - WIN or LOSE
    // WIN - required enemy numbers that are destroyed
    // LOSE - Ship is destroyed

    // Check Ship if Destoryed first
    if (stage.pShip->IsShipDestroyed())
    {
        SetGameResult(eLOSE);
    }
    // If Stage Clear, goto Stage 2
    else if (stage.IsStageClear())
    {
        SetState(eGAME_2);
    }

    const auto result = GameResult();
    if (result != eINVALID)
    {
        GameEndHandle(result);
    }

    return 0;
}

// 3 - Game Playing Stage 2
int CGameApp::GamePlay2(void)
{
    auto& stage = m_mapGameStageCfg[eGAME_2];

    // When Stage 2 started
    switch (stage.iStageSubState)
    {
        // Init Start Timer
        case eSUB_STATE_1:
        {
            // update the duetime for display "Stage TWO"
            stage.tStageStarttime = chrono::steady_clock::now() + chrono::seconds(2);
            stage.iStageSubState = eSUB_STATE_2;
            // no need break
        }

        // Display "Stage TWO"
        case eSUB_STATE_2:
        {
            // Do print the Stage 2 Start
            const int iMsgBufSize = 32;
            char cBuf[iMsgBufSize];

            const auto tCurTime = chrono::steady_clock::now();

            const int iScreenWidth = sm_GameEng.GetScreenSize().X();

            // 3A.1 - Draw Title
            sprintf_s(cBuf, iMsgBufSize, "STAGE TWO");
            const auto str1 = string(cBuf);
            const auto str1Width = CGameEngine::GetStringSpriteTotalWidth(str1, eCHAR_LARGE);
            sm_GameEng.DrawStringSprite(st2DXY((iScreenWidth - str1Width) / 2, 25), str1, eWHITE, eCHAR_LARGE);

            if (tCurTime >= stage.tStageStarttime)
            {
                // Create New Enemry Object for New Stage and passing the Last Stage Ship Object to New Stage
                stage.StartNewStage(&m_mapGameStageCfg[eGAME].pShip);
                stage.iStageSubState = eSUB_STATE_3;
            }
            break;
        }

        // Start Game Stage TWO
        case eSUB_STATE_3:
        {
            const int iMsgBufSize = 10;
            char cBufScore[iMsgBufSize];

            // 3B.1 - Draw and Control the Ship
            stage.pShip->Draw();
            stage.pShip->Control();

            // 3B.2 - Draw the Enemy That Control By Factory
            stage.pEnemyFactory->Draw();
            stage.iStageEnemyDestroyed += stage.pEnemyFactory->Control();

            // 3B.3 - Print Information
            sprintf_s(cBufScore, iMsgBufSize, "%02d/%02d", stage.iStageEnemyDestroyed, stage.stDesign.iKillNumOfEnemyToWin);
            sm_GameEng.DrawStringSprite(st2DXY(5, 5), string(cBufScore), eWHITE);

            // 3B.4 - Check if Game End - WIN or LOSE
            // WIN - required enemy numbers that are destroyed
            // LOSE - Ship is destroyed

            // Check Ship if Destoryed first
            if (stage.pShip->IsShipDestroyed())
            {
                SetGameResult(eLOSE);
            }
            else if (stage.IsStageClear())
            {
                SetGameResult(eWIN);
            }

            const auto result = GameResult();
            if (result != eINVALID)
            {
                GameEndHandle(result);
            }
            break;
        }

        // For invalid sub state
        default:
            GameEndHandle(eLOSE);
            break;
    }
 
    return 0;
}

// 3 - Game End Stage
int CGameApp::GameEnd(void)
{
    auto &stage = m_mapGameStageCfg[eEND];

    // If object invalid
    if (stage.pEnemyFactory == nullptr)
    {
        stage.StartNewStage(); // Create necessary object
    }

    const int iMsgBufSize = 32;
    char cBuf[iMsgBufSize];

    const int iScreenWidth = sm_GameEng.GetScreenSize().X();

    // 1.1 - Draw the Enemy As the Background
    stage.pEnemyFactory->Draw();
    stage.pEnemyFactory->Control();

    // 1.2 - Draw Game Result
    string szGameResult = "ERROR";
    switch (GameResult())
    {
        case eWIN:
            szGameResult = "CONTZ YOU WIN";
            break;
        case eLOSE:
            szGameResult = "YOU LOSE";
            break;
        case eINVALID:
        default:
            break;
    }

    sprintf_s(cBuf, iMsgBufSize, szGameResult.c_str());
    const auto str1 = string(cBuf);
    const auto str1Width = CGameEngine::GetStringSpriteTotalWidth(str1, eCHAR_LARGE);
    sm_GameEng.DrawStringSprite(st2DXY((iScreenWidth - str1Width) / 2, 25), str1, eDARK_BLUE, eCHAR_LARGE);

    sprintf_s(cBuf, iMsgBufSize, "PRESS \"%c\" RE-START", CONST_CHAR_TO_START);
    const auto str2 = string(cBuf);
    const auto str2Width = CGameEngine::GetStringSpriteTotalWidth(str2);
    sm_GameEng.DrawStringSprite(st2DXY((iScreenWidth - str2Width) / 2, 60), str2, eWHITE);

    sprintf_s(cBuf, iMsgBufSize, "PRESS \"%c\" QUIT", CONST_CHAR_TO_QUIT);
    const auto str3 = string(cBuf);
    const auto str3Width = CGameEngine::GetStringSpriteTotalWidth(str3);
    sm_GameEng.DrawStringSprite(st2DXY((iScreenWidth - str3Width) / 2, 80), str3, eWHITE);

    // 1.3 Check if G pressed to restart to game state
    if (GetAsyncKeyState(CONST_CHAR_TO_START) & 0x8000)
    {
        sm_GameEng.PlayGameSound(eSTART_BUTTON, SND_ASYNC);
        RestartGameHandle();
        SetState(eGAME);
    }
    // If Q to quit
    else if (GetAsyncKeyState(CONST_CHAR_TO_QUIT) & 0x8000)
    {
        SetState(eQUIT);
    }

    return 0;
}

void CGameApp::RestartGameHandle(void)
{
    for (auto iter = begin(m_mapGameStageCfg); iter != end(m_mapGameStageCfg); iter++)
    {
        iter->second.ResetStage(); // Do reset counter and release shared pointer object
    }

    auto &stage = m_mapGameStageCfg[eGAME];
    stage.StartNewStage(); // Refresh Create necessary object
    stage.ClearCounter();
    SetGameResult(eINVALID);
}

int CGameApp::GameLoop(void)
{
    if (m_szGameBoardStatus == "\0")
    {
        const auto stScreenSize = sm_GameEng.GetScreenSize();

        // Gaming Loop
        while (1)
        {
            // 1 - Must Draw the Base Board Empty Condition first
            // BOARD
            GameDrawEmptyBoard();

            // 2 - Switch To Run Different State
            switch (GetState())
            {
                case eSTART:
                    GameStart();
                    break;

                case eGAME:
                    GamePlay();
                    break;

                case eGAME_2:
                    GamePlay2();
                    break;

                case eEND:
                    GameEnd();
                    break;

                case eQUIT:
                default:
                    break;
            }

            if (GetState() == eQUIT)
            {
                break;
            }

            // 3 - Output The Frame to Screen
            sm_GameEng.GameFrameDraw();
            Sleep(1);
        }
    }
    else
    {
        cout << "Create Game Engine Board Fail :" << m_szGameBoardStatus << endl;
    }

    return 0;
}