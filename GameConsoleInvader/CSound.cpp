#include "CSound.h"

using namespace std;

bool CSound::StopSound(void) const
{
    return PlaySound(NULL, NULL, SND_FILENAME);
}

bool CSound::PlayGameSound(eGAME_SOUND eSound, DWORD flagOption) const
{
    if (eSound == eSTART_BACKGROUND)
    {
        // Use filename
        DWORD flag = SND_FILENAME | flagOption;

        const string szFile = m_szDirectory + mapGameSound.at(eSound);
        bool bSuccessPlay = false;

        // Since the project use the wchar to print board that set to use Unicode
#ifdef UNICODE
        wstring wszFile(szFile.begin(), szFile.end());
        bSuccessPlay = PlaySound(wszFile.c_str(), NULL, flag);
#else
        bSuccessPlay = PlaySound(szFile.c_str(), NULL, flag);
#endif
        return bSuccessPlay;
    }
    else
    {
        // Use filename and run as Async
        DWORD flag = SND_RESOURCE | flagOption;
        auto handle = GetModuleHandle(NULL);
        return PlaySound(MAKEINTRESOURCE(eSound), handle, flag);
    }
}

