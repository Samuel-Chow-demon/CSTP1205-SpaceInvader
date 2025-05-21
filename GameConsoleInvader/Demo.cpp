#include <iostream>
#include "olcConsoleGameEngine.h"

using namespace std;

class Demo : public olcConsoleGameEngine
{
public:
	Demo() = default;

protected:
	virtual bool OnUserCreate()
	{
		return true;
	}
	// 0x2588 is a square wchar, 0x2580 is a top half square wchar
	virtual bool OnUserUpdate(float fElapsedTime)
	{
		for (auto x = 0; x < m_nScreenWidth; x++)
		{
			for (auto y = 0; y < m_nScreenHeight; y++)
			{
				Draw(x, y, 0x2588, rand() % 16);
			}
		}
		//Fill(10, 5, 20, 10, 0x2588, FG_WHITE);
		return true;
	}

};

int main()
{
	Demo game;
	game.ConstructConsole(160, 100, 100, 50);
	game.Start();

	return 0;
}