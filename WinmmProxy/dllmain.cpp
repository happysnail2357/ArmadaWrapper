// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "PointerTypes.h"

// Forward declaration: see "Hooks.cpp" for definition
extern void InitializeWrapper();

#define API_STUB(retType, funcName, argTypes, argNames) \
    retType WINAPI funcName argTypes                    \
    {                                                   \
        /*DEBUG_PRINT_W(L"API call: " L#funcName);*/    \
        const int apiIndex = __COUNTER__;               \
        if (!apiPointers[apiIndex])                     \
        {                                               \
            apiPointers[apiIndex] = (void*)GetProcAddress(TrueWinmmLib, #funcName); \
            if (!apiPointers[apiIndex])                                             \
            {                                                                       \
                DEBUG_PRINT_W(L"Failed to load " L#funcName);                       \
                ExitProcess(1);                                                     \
            }                                                                       \
        }                                                                           \
        return ((funcName##_t)apiPointers[apiIndex])argNames;                       \
    }


static HMODULE TrueWinmmLib = nullptr;
static void* apiPointers[180] = { nullptr };

void ProxyThisLibrary()
{
    char systemPath[MAX_PATH];
    if (!GetSystemDirectoryA(systemPath, MAX_PATH))
    {
        DEBUG_PRINT_W(L"Failed to get system directory path");
        ExitProcess(1);
    }

    std::string dllPath(systemPath);
    dllPath += "\\winmm.dll";

    TrueWinmmLib = LoadLibraryA(dllPath.c_str());

    if (!TrueWinmmLib)
    {
        DEBUG_PRINT_W(L"Failed to load \"winmm.dll\" from system directory");
        ExitProcess(1);
    }
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DEBUG_PRINT_W(L"Successfully hijacked load order");
        ProxyThisLibrary();
        InitializeWrapper();
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


// Stubs

API_STUB(LRESULT, CloseDriver, (HDRVR hDriver, LPARAM lParam1, LPARAM lParam2), (hDriver, lParam1, lParam2))
API_STUB(LRESULT, DefDriverProc, (DWORD_PTR dwDriverIdentifier, HDRVR hDriver, UINT uMsg, LPARAM lParam1, LPARAM lParam2), (dwDriverIdentifier, hDriver, uMsg, lParam1, lParam2))
API_STUB(void, DriverCallback, (DWORD_PTR dwCallback, DWORD dwFlags, HDRVR hDev, DWORD_PTR dw1, DWORD_PTR dw2), (dwCallback, dwFlags, hDev, dw1, dw2))
API_STUB(HMODULE, DrvGetModuleHandle, (HDRVR hDriver), (hDriver))
API_STUB(HMODULE, GetDriverModuleHandle, (HDRVR hDriver), (hDriver))
API_STUB(HDRVR, OpenDriver, (LPCWSTR szDriverName, LPCWSTR szSectionName, LPARAM lParam), (szDriverName, szSectionName, lParam))
API_STUB(BOOL, PlaySound, (LPCSTR pszSound, HMODULE hmod, DWORD fdwSound), (pszSound, hmod, fdwSound))
API_STUB(BOOL, PlaySoundA, (LPCSTR pszSound, HMODULE hmod, DWORD fdwSound), (pszSound, hmod, fdwSound))
API_STUB(BOOL, PlaySoundW, (LPCWSTR pszSound, HMODULE hmod, DWORD fdwSound), (pszSound, hmod, fdwSound))
API_STUB(LRESULT, SendDriverMessage, (HDRVR hDriver, UINT uMsg, LPARAM lParam1, LPARAM lParam2), (hDriver, uMsg, lParam1, lParam2))
API_STUB(void, WOWAppExit, (HANDLE hTask), (hTask))
API_STUB(MMRESULT, auxGetDevCapsA, (UINT_PTR uDeviceID, LPAUXCAPSA pac, UINT cbac), (uDeviceID, pac, cbac))
API_STUB(MMRESULT, auxGetDevCapsW, (UINT_PTR uDeviceID, LPAUXCAPSW pac, UINT cbac), (uDeviceID, pac, cbac))
API_STUB(UINT, auxGetNumDevs, (void), ())
API_STUB(MMRESULT, auxGetVolume, (UINT uDeviceID, LPDWORD lpdwVolume), (uDeviceID, lpdwVolume))
API_STUB(MMRESULT, auxOutMessage, (UINT uDeviceID, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2), (uDeviceID, uMsg, dw1, dw2))
API_STUB(MMRESULT, auxSetVolume, (UINT uDeviceID, DWORD dwVolume), (uDeviceID, dwVolume))
API_STUB(void, joyConfigChanged, (DWORD dwFlags), (dwFlags))
API_STUB(MMRESULT, joyGetDevCapsA, (UINT_PTR uJoyID, LPJOYCAPSA pjc, UINT cbjc), (uJoyID, pjc, cbjc))
API_STUB(MMRESULT, joyGetDevCapsW, (UINT_PTR uJoyID, LPJOYCAPSW pjc, UINT cbjc), (uJoyID, pjc, cbjc))
API_STUB(UINT, joyGetNumDevs, (void), ())
API_STUB(MMRESULT, joyGetPos, (UINT uJoyID, LPJOYINFO pji), (uJoyID, pji))
API_STUB(MMRESULT, joyGetPosEx, (UINT uJoyID, LPJOYINFOEX pji), (uJoyID, pji))
API_STUB(MMRESULT, joyGetThreshold, (UINT uJoyID, LPUINT puThreshold), (uJoyID, puThreshold))
API_STUB(MMRESULT, joyReleaseCapture, (UINT uJoyID), (uJoyID))
API_STUB(MMRESULT, joySetCapture, (HWND hwnd, UINT uJoyID, UINT uPeriod, BOOL fChanged), (hwnd, uJoyID, uPeriod, fChanged))
API_STUB(MMRESULT, joySetThreshold, (UINT uJoyID, UINT uThreshold), (uJoyID, uThreshold))
API_STUB(void, mciDriverNotify, (HWND hwndCallback, UINT wDeviceID, UINT uStatus), (hwndCallback, wDeviceID, uStatus))
API_STUB(void, mciDriverYield, (UINT uDeviceID), (uDeviceID))
API_STUB(BOOL, mciExecute, (LPCSTR pszCommand), (pszCommand))
API_STUB(void, mciFreeCommandResource, (UINT_PTR resourceID), (resourceID))
API_STUB(HANDLE, mciGetCreatorTask, (UINT uDeviceID), (uDeviceID))
API_STUB(UINT, mciGetDeviceIDA, (LPCSTR pszDevice), (pszDevice))
API_STUB(UINT, mciGetDeviceIDFromElementIDA, (DWORD dwElementID, LPCSTR lpstrType), (dwElementID, lpstrType))
API_STUB(UINT, mciGetDeviceIDFromElementIDW, (DWORD dwElementID, LPCWSTR lpstrType), (dwElementID, lpstrType))
API_STUB(UINT, mciGetDeviceIDW, (LPCWSTR pszDevice), (pszDevice))
API_STUB(DWORD_PTR, mciGetDriverData, (UINT uDeviceID), (uDeviceID))
API_STUB(BOOL, mciGetErrorStringA, (DWORD fdwError, LPSTR lpszErrorText, UINT cchErrorText), (fdwError, lpszErrorText, cchErrorText))
API_STUB(BOOL, mciGetErrorStringW, (DWORD fdwError, LPWSTR lpszErrorText, UINT cchErrorText), (fdwError, lpszErrorText, cchErrorText))
API_STUB(YIELDPROC, mciGetYieldProc, (UINT uDeviceID, LPDWORD lpdwYieldData), (uDeviceID, lpdwYieldData))
API_STUB(UINT_PTR, mciLoadCommandResource, (HINSTANCE hInstance, LPCWSTR lpResName, UINT wType), (hInstance, lpResName, wType))
API_STUB(MCIERROR, mciSendCommandA, (UINT mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2), (mciId, uMsg, dwParam1, dwParam2))
API_STUB(MCIERROR, mciSendCommandW, (UINT mciId, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2), (mciId, uMsg, dwParam1, dwParam2))
API_STUB(MCIERROR, mciSendStringA, (LPCSTR lpszCommand, LPSTR lpszReturnString, UINT cchReturn, HWND hwndCallback), (lpszCommand, lpszReturnString, cchReturn, hwndCallback))
API_STUB(MCIERROR, mciSendStringW, (LPCWSTR lpszCommand, LPWSTR lpszReturnString, UINT cchReturn, HWND hwndCallback), (lpszCommand, lpszReturnString, cchReturn, hwndCallback))
API_STUB(BOOL, mciSetDriverData, (UINT uDeviceID, DWORD_PTR data), (uDeviceID, data))
API_STUB(void, mciSetYieldProc, (UINT uDeviceID, YIELDPROC yieldProc, DWORD dwYieldData), (uDeviceID, yieldProc, dwYieldData))
API_STUB(MMRESULT, midiConnect, (HMIDIOUT hmo, HMIDIN hmi, LPVOID pReserved), (hmo, hmi, pReserved))
API_STUB(MMRESULT, midiDisconnect, (HMIDIOUT hmo, HMIDIN hmi, LPVOID pReserved), (hmo, hmi, pReserved))
API_STUB(MMRESULT, midiInAddBuffer, (HMIDIIN hmi, LPMIDIHDR lpMidiInHdr, UINT cbMidiInHdr), (hmi, lpMidiInHdr, cbMidiInHdr))
API_STUB(MMRESULT, midiInClose, (HMIDIIN hMidiIn), (hMidiIn))
API_STUB(MMRESULT, midiInGetDevCapsA, (UINT_PTR uDeviceID, LPMIDIINCAPSA pmic, UINT cbmic), (uDeviceID, pmic, cbmic))
API_STUB(MMRESULT, midiInGetDevCapsW, (UINT_PTR uDeviceID, LPMIDIINCAPSW pmic, UINT cbmic), (uDeviceID, pmic, cbmic))
API_STUB(MMRESULT, midiInGetErrorTextA, (MMRESULT mmrError, LPSTR pszText, UINT cchText), (mmrError, pszText, cchText))
API_STUB(MMRESULT, midiInGetErrorTextW, (MMRESULT mmrError, LPWSTR pszText, UINT cchText), (mmrError, pszText, cchText))
API_STUB(MMRESULT, midiInGetID, (HMIDIIN hMidiIn, LPUINT puDeviceID), (hMidiIn, puDeviceID))
API_STUB(UINT, midiInGetNumDevs, (void), ())
API_STUB(MMRESULT, midiInMessage, (HMIDIIN hMidiIn, UINT uMessage, DWORD_PTR dw1, DWORD_PTR dw2), (hMidiIn, uMessage, dw1, dw2))
API_STUB(MMRESULT, midiInOpen, (LPHMIDIIN phMidiIn, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phMidiIn, uDeviceID, dwCallback, dwInstance, fdwOpen))
API_STUB(MMRESULT, midiInPrepareHeader, (HMIDIIN hMidiIn, LPMIDIHDR pMidiInHdr, UINT cbMidiInHdr), (hMidiIn, pMidiInHdr, cbMidiInHdr))
API_STUB(MMRESULT, midiInReset, (HMIDIIN hMidiIn), (hMidiIn))
API_STUB(MMRESULT, midiInStart, (HMIDIIN hMidiIn), (hMidiIn))
API_STUB(MMRESULT, midiInStop, (HMIDIIN hMidiIn), (hMidiIn))
API_STUB(MMRESULT, midiInUnprepareHeader, (HMIDIIN hMidiIn, LPMIDIHDR pMidiInHdr, UINT cbMidiInHdr), (hMidiIn, pMidiInHdr, cbMidiInHdr))
API_STUB(MMRESULT, midiOutCacheDrumPatches, (HMIDIOUT hmo, UINT uPatch, WORD wBank, UINT wCount), (hmo, uPatch, wBank, wCount))
API_STUB(MMRESULT, midiOutCachePatches, (HMIDIOUT hmo, UINT uBank, UINT wStartPatch, UINT wCount), (hmo, uBank, wStartPatch, wCount))
API_STUB(MMRESULT, midiOutClose, (HMIDIOUT hMidiOut), (hMidiOut))
API_STUB(MMRESULT, midiOutGetDevCapsA, (UINT_PTR uDeviceID, LPMIDIOUTCAPSA pmoc, UINT cbmoc), (uDeviceID, pmoc, cbmoc))
API_STUB(MMRESULT, midiOutGetDevCapsW, (UINT_PTR uDeviceID, LPMIDIOUTCAPSW pmoc, UINT cbmoc), (uDeviceID, pmoc, cbmoc))
API_STUB(MMRESULT, midiOutGetErrorTextA, (MMRESULT mmrError, LPSTR pszText, UINT cchText), (mmrError, pszText, cchText))
API_STUB(MMRESULT, midiOutGetErrorTextW, (MMRESULT mmrError, LPWSTR pszText, UINT cchText), (mmrError, pszText, cchText))
API_STUB(MMRESULT, midiOutGetID, (HMIDIOUT hMidiOut, LPUINT puDeviceID), (hMidiOut, puDeviceID))
API_STUB(UINT, midiOutGetNumDevs, (void), ())
API_STUB(MMRESULT, midiOutGetVolume, (HMIDIOUT hMidiOut, LPDWORD lpdwVolume), (hMidiOut, lpdwVolume))
API_STUB(MMRESULT, midiOutLongMsg, (HMIDIOUT hMidiOut, LPMIDIHDR pMidiOutHdr, UINT cbMidiOutHdr), (hMidiOut, pMidiOutHdr, cbMidiOutHdr))
API_STUB(MMRESULT, midiOutMessage, (HMIDIOUT hMidiOut, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2), (hMidiOut, uMsg, dw1, dw2))
API_STUB(MMRESULT, midiOutOpen, (LPHMIDIOUT phMidiOut, UINT uDeviceID, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phMidiOut, uDeviceID, dwCallback, dwInstance, fdwOpen))
API_STUB(MMRESULT, midiOutPrepareHeader, (HMIDIOUT hMidiOut, LPMIDIHDR pMidiOutHdr, UINT cbMidiOutHdr), (hMidiOut, pMidiOutHdr, cbMidiOutHdr))
API_STUB(MMRESULT, midiOutReset, (HMIDIOUT hMidiOut), (hMidiOut))
API_STUB(MMRESULT, midiOutSetVolume, (HMIDIOUT hMidiOut, DWORD dwVolume), (hMidiOut, dwVolume))
API_STUB(MMRESULT, midiOutShortMsg, (HMIDIOUT hMidiOut, DWORD dwMsg), (hMidiOut, dwMsg))
API_STUB(MMRESULT, midiOutUnprepareHeader, (HMIDIOUT hMidiOut, LPMIDIHDR pMidiOutHdr, UINT cbMidiOutHdr), (hMidiOut, pMidiOutHdr, cbMidiOutHdr))
API_STUB(MMRESULT, midiStreamClose, (HMIDISTRM hStream), (hStream))
API_STUB(MMRESULT, midiStreamOpen, (LPHMIDISTRM phStream, LPUINT puDeviceID, UINT cMidi, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phStream, puDeviceID, cMidi, dwCallback, dwInstance, fdwOpen))
API_STUB(MMRESULT, midiStreamOut, (HMIDISTRM hStream, LPMIDIHDR pmh, UINT cbmh), (hStream, pmh, cbmh))
API_STUB(MMRESULT, midiStreamPause, (HMIDISTRM hStream), (hStream))
API_STUB(MMRESULT, midiStreamPosition, (HMIDISTRM hStream, LPMMTIME pmmt, UINT cbmmt), (hStream, pmmt, cbmmt))
API_STUB(MMRESULT, midiStreamProperty, (HMIDISTRM hStream, LPBYTE lppropdata, DWORD dwproperty), (hStream, lppropdata, dwproperty))
API_STUB(MMRESULT, midiStreamRestart, (HMIDISTRM hStream), (hStream))
API_STUB(MMRESULT, midiStreamStop, (HMIDISTRM hStream), (hStream))
API_STUB(MMRESULT, mixerClose, (HMIXER hmx), (hmx))
API_STUB(MMRESULT, mixerGetControlDetailsA, (HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails), (hmxobj, pmxcd, fdwDetails))
API_STUB(MMRESULT, mixerGetControlDetailsW, (HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails), (hmxobj, pmxcd, fdwDetails))
API_STUB(MMRESULT, mixerGetDevCapsA, (UINT_PTR uMxId, LPMIXERCAPSA pmxcaps, UINT cbxcaps), (uMxId, pmxcaps, cbxcaps))
API_STUB(MMRESULT, mixerGetDevCapsW, (UINT_PTR uMxId, LPMIXERCAPSW pmxcaps, UINT cbxcaps), (uMxId, pmxcaps, cbxcaps))
API_STUB(MMRESULT, mixerGetID, (HMIXEROBJ hmxobj, LPUINT puMxId, DWORD fdwMeas), (hmxobj, puMxId, fdwMeas))
API_STUB(UINT, mixerGetLineControlsA, (HMIXEROBJ hmxobj, LPMIXERLINECONTROLSA pmxlc, DWORD fdwControls), (hmxobj, pmxlc, fdwControls))
API_STUB(UINT, mixerGetLineControlsW, (HMIXEROBJ hmxobj, LPMIXERLINECONTROLSW pmxlc, DWORD fdwControls), (hmxobj, pmxlc, fdwControls))
API_STUB(MMRESULT, mixerGetLineInfoA, (HMIXEROBJ hmxobj, LPMIXERLINEA pmxl, DWORD fdwInfo), (hmxobj, pmxl, fdwInfo))
API_STUB(MMRESULT, mixerGetLineInfoW, (HMIXEROBJ hmxobj, LPMIXERLINEW pmxl, DWORD fdwInfo), (hmxobj, pmxl, fdwInfo))
API_STUB(UINT, mixerGetNumDevs, (void), ())
API_STUB(MMRESULT, mixerMessage, (HMIXER hmx, UINT uMsg, DWORD_PTR dwParam1, DWORD_PTR dwParam2), (hmx, uMsg, dwParam1, dwParam2))
API_STUB(MMRESULT, mixerOpen, (LPHMIXER phmx, UINT uMxId, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phmx, uMxId, dwCallback, dwInstance, fdwOpen))
API_STUB(MMRESULT, mixerSetControlDetails, (HMIXEROBJ hmxobj, LPMIXERCONTROLDETAILS pmxcd, DWORD fdwDetails), (hmxobj, pmxcd, fdwDetails))
API_STUB(MMRESULT, mmDrvInstall, (LPMDRVLP lpmdrv, UINT uidDriver, UINT uFlags), (lpmdrv, uidDriver, uFlags))
API_STUB(DWORD, mmGetCurrentTask, (void), ())
API_STUB(BOOLEAN, mmTaskBlock, (DWORD dwTimeout), (dwTimeout))
API_STUB(DWORD, mmTaskCreate, (LPTASKENTRY lpTaskEntry, LPSTR lpszTaskName, WORD wStackSize, WORD wPriority, LPARAM lParamInit), (lpTaskEntry, lpszTaskName, wStackSize, wPriority, lParamInit))
API_STUB(BOOLEAN, mmTaskSignal, (DWORD dwTaskID, DWORD dwEvent), (dwTaskID, dwEvent))
API_STUB(void, mmTaskYield, (void), ())
API_STUB(MMRESULT, mmioAdvance, (HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT wFlags), (hmmio, lpmmioinfo, wFlags))
API_STUB(MMRESULT, mmioAscend, (HMMIO hmmio, LPMMCKINFO lpck, UINT wFlags), (hmmio, lpck, wFlags))
API_STUB(MMRESULT, mmioClose, (HMMIO hmmio, UINT wFlags), (hmmio, wFlags))
API_STUB(MMRESULT, mmioCreateChunk, (HMMIO hmmio, LPMMCKINFO lpck, UINT wFlags), (hmmio, lpck, wFlags))
API_STUB(MMRESULT, mmioDescend, (HMMIO hmmio, LPMMCKINFO lpck, const MMCKINFO* lpckParent, UINT wFlags), (hmmio, lpck, lpckParent, wFlags))
API_STUB(MMRESULT, mmioFlush, (HMMIO hmmio, UINT wFlags), (hmmio, wFlags))
API_STUB(MMRESULT, mmioGetInfo, (HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT wFlags), (hmmio, lpmmioinfo, wFlags))
API_STUB(FOURCC, mmioInstallIOProcA, (FOURCC fccIOProc, LPMMIOINFO lpmmioinfo, DWORD dwFlags), (fccIOProc, lpmmioinfo, dwFlags))
API_STUB(FOURCC, mmioInstallIOProcW, (FOURCC fccIOProc, LPMMIOINFO lpmmioinfo, DWORD dwFlags), (fccIOProc, lpmmioinfo, dwFlags))
API_STUB(HMMIO, mmioOpenA, (LPCSTR szFilename, LPMMIOINFO lpmmioinfo, DWORD dwOpenFlags), (szFilename, lpmmioinfo, dwOpenFlags))
API_STUB(HMMIO, mmioOpenW, (LPCWSTR szFilename, LPMMIOINFO lpmmioinfo, DWORD dwOpenFlags), (szFilename, lpmmioinfo, dwOpenFlags))
API_STUB(LONG, mmioRead, (HMMIO hmmio, LPSTR pch, LONG cch), (hmmio, pch, cch))
API_STUB(MMRESULT, mmioRenameA, (LPCSTR pszFileOld, LPCSTR pszFileNew, UINT wFlags), (pszFileOld, pszFileNew, wFlags))
API_STUB(MMRESULT, mmioRenameW, (LPCWSTR pszFileOld, LPCWSTR pszFileNew, UINT wFlags), (pszFileOld, pszFileNew, wFlags))
API_STUB(LONG, mmioSeek, (HMMIO hmmio, LONG lOffset, int iOrigin), (hmmio, lOffset, iOrigin))
API_STUB(LRESULT, mmioSendMessage, (HMMIO hmmio, UINT uMsg, LPARAM lParam1, LPARAM lParam2), (hmmio, uMsg, lParam1, lParam2))
API_STUB(MMRESULT, mmioSetBuffer, (HMMIO hmmio, LPSTR pchBuffer, LONG cchBuffer, UINT wFlags), (hmmio, pchBuffer, cchBuffer, wFlags))
API_STUB(MMRESULT, mmioSetInfo, (HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT wFlags), (hmmio, lpmmioinfo, wFlags))
API_STUB(FOURCC, mmioStringToFOURCCA, (LPCSTR sz), (sz))
API_STUB(FOURCC, mmioStringToFOURCCW, (LPCWSTR sz), (sz))
API_STUB(LONG, mmioWrite, (HMMIO hmmio, const char* pch, LONG cch), (hmmio, pch, cch))
API_STUB(UINT, mmsystemGetVersion, (void), ())
API_STUB(BOOL, sndPlaySoundA, (LPCSTR pszSound, UINT fuSound), (pszSound, fuSound))
API_STUB(BOOL, sndPlaySoundW, (LPCWSTR pszSound, UINT fuSound), (pszSound, fuSound))
API_STUB(MMRESULT, timeBeginPeriod, (UINT uPeriod), (uPeriod))
API_STUB(MMRESULT, timeEndPeriod, (UINT uPeriod), (uPeriod))
API_STUB(MMRESULT, timeGetDevCaps, (LPTIMECAPS ptc, UINT cbtc), (ptc, cbtc))
API_STUB(void, timeGetSystemTime, (LPTIMEINFO pti, UINT cbti), (pti, cbti))
API_STUB(DWORD, timeGetTime, (void), ())
API_STUB(MMRESULT, timeKillEvent, (UINT uTimerID), (uTimerID))
API_STUB(UINT, timeSetEvent, (UINT uDelay, UINT uResolution, LPTIMECALLBACK ptc, DWORD_PTR dwUser, UINT fuEvent), (uDelay, uResolution, ptc, dwUser, fuEvent))
API_STUB(MMRESULT, waveInAddBuffer, (HWAVEIN hWaveIn, LPWAVEHDR pwh, UINT cbwh), (hWaveIn, pwh, cbwh))
API_STUB(MMRESULT, waveInClose, (HWAVEIN hWaveIn), (hWaveIn))
API_STUB(MMRESULT, waveInGetDevCapsA, (UINT_PTR uDeviceID, LPWAVEINCAPSA pwic, UINT cbwic), (uDeviceID, pwic, cbwic))
API_STUB(MMRESULT, waveInGetDevCapsW, (UINT_PTR uDeviceID, LPWAVEINCAPSW pwic, UINT cbwic), (uDeviceID, pwic, cbwic))
API_STUB(MMRESULT, waveInGetErrorTextA, (MMRESULT mmrError, LPSTR pszText, UINT cchText), (mmrError, pszText, cchText))
API_STUB(MMRESULT, waveInGetErrorTextW, (MMRESULT mmrError, LPWSTR pszText, UINT cchText), (mmrError, pszText, cchText))
API_STUB(MMRESULT, waveInGetID, (HWAVEIN hWaveIn, LPUINT puDeviceID), (hWaveIn, puDeviceID))
API_STUB(UINT, waveInGetNumDevs, (void), ())
API_STUB(MMRESULT, waveInGetPosition, (HWAVEIN hWaveIn, LPMMTIME pmmt, UINT cbmmt), (hWaveIn, pmmt, cbmmt))
API_STUB(MMRESULT, waveInMessage, (HWAVEIN hWaveIn, UINT uMessage, DWORD_PTR dwParam1, DWORD_PTR dwParam2), (hWaveIn, uMessage, dwParam1, dwParam2))
API_STUB(MMRESULT, waveInOpen, (LPHWAVEIN phwi, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phwi, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen))
API_STUB(MMRESULT, waveInPrepareHeader, (HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh), (hwi, pwh, cbwh))
API_STUB(MMRESULT, waveInReset, (HWAVEIN hwi), (hwi))
API_STUB(MMRESULT, waveInStart, (HWAVEIN hwi), (hwi))
API_STUB(MMRESULT, waveInStop, (HWAVEIN hwi), (hwi))
API_STUB(MMRESULT, waveInUnprepareHeader, (HWAVEIN hwi, LPWAVEHDR pwh, UINT cbwh), (hwi, pwh, cbwh))
API_STUB(MMRESULT, waveOutBreakLoop, (HWAVEOUT hwo), (hwo))
API_STUB(MMRESULT, waveOutClose, (HWAVEOUT hwo), (hwo))
API_STUB(MMRESULT, waveOutGetDevCapsA, (UINT_PTR uDeviceID, LPWAVEOUTCAPSA pwoc, UINT cbwoc), (uDeviceID, pwoc, cbwoc))
API_STUB(MMRESULT, waveOutGetDevCapsW, (UINT_PTR uDeviceID, LPWAVEOUTCAPSW pwoc, UINT cbwoc), (uDeviceID, pwoc, cbwoc))
API_STUB(MMRESULT, waveOutGetErrorTextA, (MMRESULT mmrError, LPSTR pszText, UINT cchText), (mmrError, pszText, cchText))
API_STUB(MMRESULT, waveOutGetErrorTextW, (MMRESULT mmrError, LPWSTR pszText, UINT cchText), (mmrError, pszText, cchText))
API_STUB(MMRESULT, waveOutGetID, (HWAVEOUT hwo, LPUINT puDeviceID), (hwo, puDeviceID))
API_STUB(UINT, waveOutGetNumDevs, (void), ())
API_STUB(MMRESULT, waveOutGetPitch, (HWAVEOUT hwo, LPDWORD pdwPitch), (hwo, pdwPitch))
API_STUB(MMRESULT, waveOutGetPlaybackRate, (HWAVEOUT hwo, LPDWORD pdwRate), (hwo, pdwRate))
API_STUB(MMRESULT, waveOutGetPosition, (HWAVEOUT hwo, LPMMTIME pmmt, UINT cbmmt), (hwo, pmmt, cbmmt))
API_STUB(MMRESULT, waveOutGetVolume, (HWAVEOUT hwo, LPDWORD pdwVolume), (hwo, pdwVolume))
API_STUB(MMRESULT, waveOutMessage, (HWAVEOUT hwo, UINT uMsg, DWORD_PTR dw1, DWORD_PTR dw2), (hwo, uMsg, dw1, dw2))
API_STUB(MMRESULT, waveOutOpen, (LPHWAVEOUT phwo, UINT uDeviceID, LPCWAVEFORMATEX pwfx, DWORD_PTR dwCallback, DWORD_PTR dwInstance, DWORD fdwOpen), (phwo, uDeviceID, pwfx, dwCallback, dwInstance, fdwOpen))
API_STUB(MMRESULT, waveOutPause, (HWAVEOUT hwo), (hwo))
API_STUB(MMRESULT, waveOutPrepareHeader, (HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh), (hwo, pwh, cbwh))
API_STUB(MMRESULT, waveOutReset, (HWAVEOUT hwo), (hwo))
API_STUB(MMRESULT, waveOutRestart, (HWAVEOUT hwo), (hwo))
API_STUB(MMRESULT, waveOutSetPitch, (HWAVEOUT hwo, DWORD dwPitch), (hwo, dwPitch))
API_STUB(MMRESULT, waveOutSetPlaybackRate, (HWAVEOUT hwo, DWORD dwRate), (hwo, dwRate))
API_STUB(MMRESULT, waveOutSetVolume, (HWAVEOUT hwo, DWORD dwVolume), (hwo, dwVolume))
API_STUB(MMRESULT, waveOutUnprepareHeader, (HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh), (hwo, pwh, cbwh))
API_STUB(MMRESULT, waveOutWrite, (HWAVEOUT hwo, LPWAVEHDR pwh, UINT cbwh), (hwo, pwh, cbwh))

