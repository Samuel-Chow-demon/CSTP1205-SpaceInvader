#pragma once
#include "CEnemy.h"
#include <mutex>

class CGameEngine;

class CEnemyFactory
{
public:
	CEnemyFactory(CGameEngine *pobjGame, st2D responseRate, st2D pixelSpeedPerCtrl, int iEnemyNum, COLOUR enemyColor);

	void Draw(void);
	int Control(void);
	void ClearAllEnemy(void);

private:
	int m_iMaxEnemyInScreen;
	std::vector<CEnemy> m_vecEnemy;
	CGameEngine *m_pObjGame;
	std::chrono::steady_clock::time_point m_tLastSpawnTime;
	int m_iNextSpawnDuration;
	st2D m_stResponseRate, m_stPixelSpeedPerCtrl;
	COLOUR m_eColor;
	std::mutex m_mutEnemyObj;

	// Not allow to be copied and assigned
	CEnemyFactory(const CEnemyFactory& other);
	CEnemyFactory& operator=(const CEnemyFactory& other);
};

