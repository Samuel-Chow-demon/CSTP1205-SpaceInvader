#include "CEnemyFactory.h"
#include "CGameEngine.h"

#include <random>

using namespace std;

//template<>
//void stGameObjAttrib::CheckAndRemoveObj(std::vector<CEnemy>& vecObj, std::function<bool(CEnemy& obj)> funcConditionNeedRemove);

CEnemyFactory::CEnemyFactory(CGameEngine *pobjGame, st2D responseRate, st2D pixelSpeedPerCtrl, int iMaxEnemyNumInScreen, COLOUR enemyColor) :
    m_pObjGame{ pobjGame },
    m_stResponseRate{ responseRate }, m_stPixelSpeedPerCtrl{ pixelSpeedPerCtrl },
    m_iMaxEnemyInScreen{ iMaxEnemyNumInScreen }, m_eColor{ enemyColor },
    m_iNextSpawnDuration{-1}
{}

void CEnemyFactory::Draw(void)
{
    lock_guard<std::mutex> lock(m_mutEnemyObj);

    if (m_vecEnemy.size())
    {
        for (auto &enemy : m_vecEnemy)
        {
            enemy.Draw();
        }
    }
}

int CEnemyFactory::Control(void)
{
    int iCurFrameTotalEnemyDestroyed = 0;
    if (m_pObjGame)
    {
        lock_guard<std::mutex> lock(m_mutEnemyObj);

        if (static_cast<int>(m_vecEnemy.size()) < m_iMaxEnemyInScreen &&
            (m_iNextSpawnDuration < 0 || m_pObjGame->IsNeedRefresh(m_tLastSpawnTime, m_iNextSpawnDuration))) // would update the last time inside
        {
            const int iScreenWidth = m_pObjGame->GetScreenSize().X();
            int iStartXPos = std::rand() % iScreenWidth + 1;

            random_device rd;
            mt19937 gen(rd());
            uniform_int_distribution<> dis(1500, 3000); // Range Between 1500 and 3000 ms to spawn next enemy

            m_iNextSpawnDuration = dis(gen);

            // use move to pass over the memory, get rid of the copy constructor
            m_vecEnemy.push_back(move(CEnemy(m_pObjGame, m_stResponseRate, m_stPixelSpeedPerCtrl, st2D(iStartXPos, 0), m_eColor)));
        }

        if (m_vecEnemy.size())
        {
            for (auto &enemy : m_vecEnemy)
            {
                iCurFrameTotalEnemyDestroyed += enemy.Control();
            }

            // Check Any Enemy Out Of Screen, OR
            // If the Life become zero or less AND finished the Destroy Anime
            // To Remove Obj          
            stGameObjAttrib::CheckAndRemoveObj<CEnemy>(m_vecEnemy, [](CEnemy& obj) {return obj.IsNeedDestroy(); });
        }
    }
    return iCurFrameTotalEnemyDestroyed;
}

void CEnemyFactory::ClearAllEnemy(void)
{
    lock_guard<std::mutex> lock(m_mutEnemyObj);
    m_vecEnemy.clear();
}