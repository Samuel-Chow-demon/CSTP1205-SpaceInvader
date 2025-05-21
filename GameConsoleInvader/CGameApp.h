#pragma once

#include "CGameEngine.h"
#include "CSpaceShip.h"
#include "CEnemyFactory.h"
#include <functional>
#include <map>

const int iKILL_ENEMY_TO_WIN			= 10;
const int iDEFAULT_MAX_ENEMY_PER_SCREEN = 10;
const char CONST_CHAR_TO_START			= 'G';
const char CONST_CHAR_TO_QUIT			= 'Q';

enum eGAME_STATE
{
    eSTART = 0,
    eGAME,
    eGAME_2,
    eEND,
    eQUIT,
    eTOTAL_GAME_STATE
};

enum eGAME_SUB_STATE
{
    eSUB_STATE_1 = 0,
    eSUB_STATE_2,
    eSUB_STATE_3
};

enum eGAME_RESULT
{
    eWIN = 0,
    eLOSE,
    eINVALID
};

class CGameApp
{
public:
    CGameApp();

    eGAME_STATE GetState(void)
    {
        return m_eGameState;
    }
    void SetState(eGAME_STATE eState)
    {
        m_eGameState = eState;
        m_mapGameStageCfg[m_eGameState].ResetSubState();
    }

    eGAME_RESULT GameResult(void)
    {
        return m_eResult;
    }
    void SetGameResult(eGAME_RESULT result)
    {
        m_eResult = result;
    }

    void GameDrawEmptyBoard(void);
    void GameEndHandle(eGAME_RESULT eResult);

    // Game Different Stage
    int GameStart(void);
    int GamePlay(void);
    int GamePlay2(void);
    int GameEnd(void);

    // Game Loop
    int GameLoop(void);


private:
    void GameStageDesign(void);
    void RestartGameHandle(void);

    static CGameEngine sm_GameEng;
    std::string m_szGameBoardStatus;
    eGAME_STATE m_eGameState;
    eGAME_RESULT m_eResult;

    struct stStageDesign
    {
        stStageDesign() :
            iMaxEnemyAtSameTime{ iDEFAULT_MAX_ENEMY_PER_SCREEN },
            eEnemyColor{eDARK_GREEN},
            iKillNumOfEnemyToWin{ iKILL_ENEMY_TO_WIN },
            bNeedSpaceShip{true},
            stSpaceShipResponseRate{ CONST_PER_CONTROL_SHIP_IN_MS },
            stSpaceShipPixelSpeed{ DEFAULT_SHIP_PIXEL_MOVE_SPEED, DEFAULT_SHIP_PIXEL_MOVE_SPEED },              // Move X and Y direct 1 pixel per control
            stEnemyResponseRate{ CONST_PER_CONTROL_ENEMY_PER_MS , CONST_PER_CONTROL_ENEMY_PER_MS * 4},
            stEnemyPixelSpeed{ DEFAULT_ENEMY_PIXEL_MOVE_SPEED_IN_X , DEFAULT_ENEMY_PIXEL_MOVE_SPEED_IN_Y },
            funcIsStageClearFulFilled{nullptr}
        {
            const auto stScreenSize = sm_GameEng.GetScreenSize();
            stShipStartCoord = st2D(stScreenSize.X() / 2, stScreenSize.Y() / 2);
        }
        int iMaxEnemyAtSameTime;
        st2D stEnemyResponseRate, stEnemyPixelSpeed;
        COLOUR eEnemyColor;
        int iKillNumOfEnemyToWin;

        bool bNeedSpaceShip;
        st2D stSpaceShipResponseRate, stSpaceShipPixelSpeed;
        st2D stShipStartCoord;

        std::function<bool(void)> funcIsStageClearFulFilled;
    };

    struct stStageObject
    {
        stStageObject() :
            stDesign{},
            pEnemyFactory{ nullptr }, pShip{ nullptr },
            iStageEnemyDestroyed{ 0 },
            iStageSubState{ eSUB_STATE_1 }
        {}
        stStageObject(const stStageDesign &stage) :
            stDesign{ stage },
            pEnemyFactory{ nullptr }, pShip{nullptr},
            iStageEnemyDestroyed{0},
            iStageSubState{ eSUB_STATE_1 }
        {}

        void CreateNewEnemyFactory(void)
        {
            // When assigning a new object, if the original object is not held by others
            // the original obj would be auto deleted
            pEnemyFactory = std::make_shared<CEnemyFactory>(&sm_GameEng, stDesign.stEnemyResponseRate, stDesign.stEnemyPixelSpeed,
                                                            stDesign.iMaxEnemyAtSameTime, stDesign.eEnemyColor);
        }
        void CreateNewSpaceShip(void)
        {
            // When assigning a new object, if the original object is not held by others
            // the original obj would be auto deleted
            pShip = std::make_shared<CSpaceShip>(&sm_GameEng, stDesign.stSpaceShipResponseRate, stDesign.stSpaceShipPixelSpeed, stDesign.stShipStartCoord);
        }

    private:
        void PassingSpaceShipObj(std::shared_ptr<CSpaceShip> *ppObj) // passing pointer of pointer
        {
            // When passing a object, if the original object is not held by others
            // the original obj would be auto deleted
            pShip = *ppObj;
        }

    public:
        void StartNewStage(std::shared_ptr<CSpaceShip> *ppObj = nullptr)
        {
            CreateNewEnemyFactory();
            if (stDesign.bNeedSpaceShip)
            {
                if (ppObj)
                {
                    PassingSpaceShipObj(ppObj);
                }
                else
                {
                    CreateNewSpaceShip();
                }
            }
        }
        bool IsStageClear(void)
        {
            if (stDesign.funcIsStageClearFulFilled)
            {
                return stDesign.funcIsStageClearFulFilled();
            }
            return false;
        }
        void ClearCounter(void)
        {
            iStageEnemyDestroyed = 0;
        }
        void ResetSubState(void)
        {
            iStageSubState = eSUB_STATE_1;
        }
        void ReleaseObject(void)
        {
            pEnemyFactory.reset();
            pShip.reset();

            pShip = nullptr;
            pEnemyFactory = nullptr;
        }
        void ResetStage(void)
        {
            ClearCounter();
            ResetSubState();
            ReleaseObject();
        }

    public:
        // Each Stage should have one EnemyFactory and one Ship Object
        std::shared_ptr<CEnemyFactory> pEnemyFactory;
        std::shared_ptr<CSpaceShip>  pShip;

        stStageDesign stDesign;
        int iStageEnemyDestroyed;
        eGAME_SUB_STATE iStageSubState;
        std::chrono::steady_clock::time_point tStageStarttime;
    };

    std::map<eGAME_STATE, stStageObject> m_mapGameStageCfg;
};

