#pragma once
#include <map>
#include <string>
#include <windows.h>

enum eGAME_SOUND
{
    eSTART_BACKGROUND = 0,
    eSTART_BUTTON,
    eSHOOT,
    eENEMY_DIE,
    eSHIP_DIE,
    eSOUND_WIN,
    eSOUND_LOSE
};

const std::map<eGAME_SOUND, std::string> mapGameSound = {
    {eSTART_BACKGROUND, "background_start.wav"},
    {eSTART_BUTTON, "game_start_v2.wav"},
    {eSHOOT, "shoot.wav"},
    {eENEMY_DIE, "enemy_die.wav"},
    {eSHIP_DIE, "ship_die.wav"},
    {eSOUND_WIN, "win.wav"},
    {eSOUND_LOSE, "game_over.wav"}
};

class CSound
{
public:
    CSound(const std::string &dir) :
        m_szDirectory(dir)
    {
        if (m_szDirectory[m_szDirectory.length() - 1] != '/')
        {
            m_szDirectory += "/";
        }
    }

    bool StopSound(void) const;
    bool PlayGameSound(eGAME_SOUND eSound, DWORD flagOption) const;

private:
    std::string m_szDirectory;
};

