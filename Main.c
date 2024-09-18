#include "Main.h"
#include "TitleScreen.h"
#include "OpeningSplash.h"
#include "CharacterNaming.h"
#include "GamepadUnplugged.h"
#include "ExitYesNo.h"
#include "OptionsScreen.h"
#include "Overworld.h"
#include "stb_vorbis.h"
#include "miniz.h"

REGISTRY_PARAMS gRegistryParams;
BOOL gWindowHasFocus;
XINPUT_STATE gGamepadState;
IXAudio2* gXAudio;
IXAudio2MasteringVoice* gXAudioMasteringVoice;
uint8_t gSFXSourceVoiceSelector;
CRITICAL_SECTION gLogCritSec;
int32_t gFontCharacterPixelOffset[] = {
    //  .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ..
        93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
    //     !  "  #  $  %  &  '  (  )  *  +  ,  -  .  /  0  1  2  3  4  5  6  7  8  9  :  ;  <  =  >  ?
        94,64,87,66,67,68,70,85,72,73,71,77,88,74,91,92,52,53,54,55,56,57,58,59,60,61,86,84,89,75,90,93,
    //  @  A  B  C  D  E  F  G  H  I  J  K  L  M  N  O  P  Q  R  S  T  U  V  W  X  Y  Z  [  \  ]  ^  _
        65,0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,80,78,81,69,76,
    //  `  a  b  c  d  e  f  g  h  i  j  k  l  m  n  o  p  q  r  s  t  u  v  w  x  y  z  {  |  }  ~  ..
        62,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,82,79,83,63,93,
    //  .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ..
        93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
    //  .. .. .. .. .. .. .. .. .. .. .. bb .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ab .. .. .. ..
        93,93,93,93,93,93,93,93,93,93,93,96,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,95,93,93,93,93,
    //  .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. ..
        93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,
    //  .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. F2 .. .. .. .. .. .. .. .. .. .. .. .. ..
        93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,97,93,93,93,93,93,93,93,93,93,93,93,93,93
};

int __stdcall WinMain(_In_ HINSTANCE Instance, _In_opt_ HINSTANCE PrevInstance, _In_ PSTR CmdLine, _In_ INT CmdShow) {
	UNREFERENCED_PARAMETER(Instance);
	UNREFERENCED_PARAMETER(PrevInstance);
	UNREFERENCED_PARAMETER(CmdLine);
	UNREFERENCED_PARAMETER(CmdShow);

	MSG Message = { 0 };
	int64_t FrameStart = 0; int64_t FrameEnd = 0;
	int64_t ElapsedMicrosecondAccumulator = 0;
	int64_t ElapsedMicrosecondAccumulatorCooked = 0;
	int64_t ElapsedMicroSecond = 0;
	HMODULE NtDllModuleHandle = NULL;
	FILETIME ProcessCreateTime = { 0 };
	FILETIME ProcessExitTime = { 0 };
	int64_t CurrentUserCPUTime = 0;
	int64_t CurrentKernelCPUTime = 0;
	int64_t PreviousUserCPUTime = 0;
	int64_t PreviousKernelCPUTime = 0;
	HANDLE ProcessHandle = GetCurrentProcess();

#pragma warning(suppress: 6031)
	InitializeCriticalSectionAndSpinCount(&gLogCritSec, 0x400);
	gSplashScreenLoadedEvent = CreateEventA(NULL, TRUE, FALSE, "gSplashScreenLoadedEvent");

	gCurrentGameState = GAMESTATE_OPENING_SPLASH_SCREEN;

	gGamepadID = -1;
	//gOverworldArea.left = 0; // = { 0, 0, 4240, 2400 };
	//gOverworldArea.top = 0;
	//gOverworldArea.right = 3840;
	//gOverworldArea.bottom = 2400;
	gOverworldArea = (RECT){ .left = 0, .top = 0, .right = 3840, .bottom = 2400 };

	gPassableTiles[0] = TILE_GRASS_01;
	gPassableTiles[1] = TILE_SNOW_01;
	gPassableTiles[2] = TILE_BRIDGE_01;
	gPassableTiles[3] = TILE_SAND_01;
	gPassableTiles[4] = TILE_SWAMP_01;
	/*for (uint8_t i = 0; i < 99; i++) {
		gPassableTiles[i] = i;
	}*/

	SYSTEMTIME Time = { 0 };
	char DateTimeString[96] = { 0 };

	GetLocalTime(&Time);
	_snprintf_s(DateTimeString, sizeof(DateTimeString), _TRUNCATE, "%02u/%02u/%u %02u:%02u:%02u.%03u", Time.wMonth, Time.wDay, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);
	LogMessageA(LL_ALWAYS, "---------- %s -----------", DateTimeString);

	if (LoadRegistryParams() != ERROR_SUCCESS) {
		goto Exit;
	}

	LogMessageA(LL_INFO, "[%s] Starting \"" GAME_NAME "\" version " GAME_VER, __FUNCTION__);

	if ((NtDllModuleHandle = GetModuleHandleA("ntdll.dll")) == NULL) {
		LogMessageA(LL_ERROR, "[%s] Cannot load ntdll.dll; Error code 0x%08lx", __FUNCTION__, GetLastError());
		MessageBoxA(NULL, "Cannot load ntdll.dll", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	if ((NtQueryTimerResolution = (_NtQueryTimerResolution)GetProcAddress(NtDllModuleHandle, "NtQueryTimerResolution")) == NULL) {
		LogMessageA(LL_ERROR, "[%s] Cannot find function NtQueryTimerResolution in ntdll.dll; GetProcAddress failed with error code 0x%08lx", __FUNCTION__, GetLastError());
		MessageBoxA(NULL, "Cannot find function NtQueryTimerResolution in ntdll.dll", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	NtQueryTimerResolution(&gPerformanceData.MaximumTimerResolution, &gPerformanceData.MinimumTimerResolution, &gPerformanceData.CurrentTimerResolution);
	GetSystemInfo(&gPerformanceData.SystemInfo);
	GetSystemTimeAsFileTime((FILETIME*)&gPerformanceData.PreviousSystemTime);

	LogMessageA(LL_INFO, "[%s] Number of logical cores: %d", __FUNCTION__, gPerformanceData.SystemInfo.dwNumberOfProcessors);

	if (AlreadyRunning() == TRUE) {
		LogMessageA(LL_WARNING, "[%s] The game is already running", __FUNCTION__);
		MessageBoxA(NULL, "Game already running", "Warning", MB_ICONWARNING | MB_OK);
		goto Exit;
	}

	if (timeBeginPeriod(1) == TIMERR_NOCANDO) {
		LogMessageA(LL_ERROR, "[%s] Failed to set timer resolution to 1ms", __FUNCTION__);
		MessageBoxA(NULL, "Failed to set timer resolution to 1ms", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	LogMessageA(LL_INFO, "[%s] System timer resolution: Min %.4fms, Max %.4fms, Current %.4fms", __FUNCTION__, gPerformanceData.MinimumTimerResolution / 10000.0f, gPerformanceData.MaximumTimerResolution / 10000.0f, gPerformanceData.CurrentTimerResolution / 10000.0f);

	if (SetPriorityClass(ProcessHandle, HIGH_PRIORITY_CLASS) == 0) {
		LogMessageA(LL_ERROR, "[%s] Failed to set priority class; Error code 0x%08lx", __FUNCTION__, GetLastError());
		MessageBoxA(NULL, "Failed to set priority class", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	if (SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST) == 0) {
		LogMessageA(LL_ERROR, "[%s] Failed to set thread priority; Error code 0x%08lx", __FUNCTION__, GetLastError());
		MessageBoxA(NULL, "Failed to set thread priority", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	if (CreateMainGameWindow() != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to create window", __FUNCTION__);

		goto Exit;
	}
	
	if ((gAssetLoadingThread = CreateThread(NULL, 0, AssetLoadingThreadProc, NULL, 0, NULL)) == NULL) {
		LogMessageA(LL_ERROR, "[%s] Failed to create asset loading thread", __FUNCTION__);
		MessageBoxA(NULL, "Failed creating asset loading thread", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	if (InitalizeSoundEngine() != S_OK) {
		MessageBoxA(NULL, "InitalizeSoundEngine() failed to initalize.", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	QueryPerformanceFrequency((LARGE_INTEGER*)&gPerformanceData.PerfFrequency);

#ifdef _DEBUG
	gPerformanceData.DisplayDebugInfo = TRUE;
#else
	gPerformanceData.DisplayDebugInfo = FALSE;
#endif

	gCanvas.bitmapInfo.bmiHeader.biSize = sizeof(gCanvas.bitmapInfo.bmiHeader);
	gCanvas.bitmapInfo.bmiHeader.biWidth = GAME_RES_WIDTH;
	gCanvas.bitmapInfo.bmiHeader.biHeight = GAME_RES_HEIGHT;
	gCanvas.bitmapInfo.bmiHeader.biBitCount = GAME_BPP;
	gCanvas.bitmapInfo.bmiHeader.biCompression = BI_RGB;
	gCanvas.bitmapInfo.bmiHeader.biPlanes = 1;

	if ((gCanvas.Memory = VirtualAlloc(NULL, GAME_CANVAS_MEMORY_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE)) == NULL) {
		LogMessageA(LL_ERROR, "[%s] Failed to allocate memory for canvas; Error code 0x%08lx", __FUNCTION__, GetLastError());
		MessageBoxA(NULL, "Failed to allocate memory", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	memset(gCanvas.Memory, 0x7F, GAME_CANVAS_MEMORY_SIZE);

	if (InitalizeHero() != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to initalize player", __FUNCTION__);
		MessageBoxA(NULL, "Failed to initalize player", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}
	
	gGameRunning = TRUE;
	while (gGameRunning) {
		QueryPerformanceCounter((LARGE_INTEGER*)&FrameStart);

		while (PeekMessageA(&Message, gWHandle, 0, 0, PM_REMOVE)) {
			DispatchMessageA(&Message);
		}

		ProcessPlayerInput();
		RenderFrameGraphic();

		QueryPerformanceCounter((LARGE_INTEGER*)&FrameEnd);

		ElapsedMicroSecond = FrameEnd - FrameStart;
		ElapsedMicroSecond *= 1000000;
		ElapsedMicroSecond /= gPerformanceData.PerfFrequency;

		gPerformanceData.totalFramesRendered++;

		ElapsedMicrosecondAccumulator += ElapsedMicroSecond;

		while (ElapsedMicroSecond < TARGET_MICROSECONDS_PER_FRAME) {
			ElapsedMicroSecond = FrameEnd - FrameStart;
			ElapsedMicroSecond *= 1000000;
			ElapsedMicroSecond /= gPerformanceData.PerfFrequency;

			QueryPerformanceCounter((LARGE_INTEGER*)&FrameEnd);

			if (ElapsedMicroSecond < (TARGET_MICROSECONDS_PER_FRAME * 0.75f)) {
				Sleep(1);
			}
		}

		ElapsedMicrosecondAccumulatorCooked += ElapsedMicroSecond;

		if ((gPerformanceData.totalFramesRendered % (int8_t)CALCULATE_AVG_FPS_Every_X_Frames) == 0) {
			GetSystemTimeAsFileTime((FILETIME*)&gPerformanceData.CurrentSystemTime);
			GetProcessTimes(ProcessHandle,
				&ProcessCreateTime,
				&ProcessExitTime,
				(FILETIME*)&CurrentKernelCPUTime,
				(FILETIME*)&CurrentUserCPUTime);
			gPerformanceData.CPUPercent = (double)(CurrentKernelCPUTime - PreviousKernelCPUTime) + (CurrentUserCPUTime - PreviousUserCPUTime);
			gPerformanceData.CPUPercent /= (gPerformanceData.CurrentSystemTime - gPerformanceData.PreviousSystemTime);
			gPerformanceData.CPUPercent /= gPerformanceData.SystemInfo.dwNumberOfProcessors;
			gPerformanceData.CPUPercent *= 100;

			GetProcessHandleCount(ProcessHandle, &gPerformanceData.HandleCount);
			K32GetProcessMemoryInfo(ProcessHandle, (PROCESS_MEMORY_COUNTERS*)&gPerformanceData.MemInfo, sizeof(gPerformanceData.MemInfo));
			gPerformanceData.rawAvgFPS = 1.0f / ((float)(ElapsedMicrosecondAccumulator / CALCULATE_AVG_FPS_Every_X_Frames) * 0.000001f);
			gPerformanceData.cookedAvgFPS = 1.0f / ((float)(ElapsedMicrosecondAccumulatorCooked / CALCULATE_AVG_FPS_Every_X_Frames) * 0.000001f);

			FindFirstConnectedGamepad();

			ElapsedMicrosecondAccumulator = 0;
			ElapsedMicrosecondAccumulatorCooked = 0;
			PreviousUserCPUTime = CurrentUserCPUTime;
			PreviousKernelCPUTime = CurrentKernelCPUTime;
			gPerformanceData.PreviousSystemTime = gPerformanceData.CurrentSystemTime;
		}
	}

Exit:
	return 0;
}

LRESULT CALLBACK MainWindProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
	LRESULT Result = 0;

	switch (uMsg) {
	case WM_CLOSE:
		gGameRunning = FALSE;
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		if (wParam == 0) {
			gWindowHasFocus = FALSE;
		}
		else {
			gWindowHasFocus = TRUE;
			ShowCursor(FALSE);
		}
		break;
	default:
		Result = DefWindowProcA(hwnd, uMsg, wParam, lParam);
	}

	return Result;
}

DWORD CreateMainGameWindow(void) {
	DWORD Result = ERROR_SUCCESS;
	WNDCLASSEXA wClass = { 0 };

	wClass.cbSize = sizeof(WNDCLASSEXA);
	wClass.style = 0;
	wClass.lpfnWndProc = MainWindProc;
	wClass.cbClsExtra = 0;
	wClass.cbWndExtra = 0;
	wClass.hInstance = GetModuleHandleA(NULL);
	wClass.hIcon = LoadIconA(NULL, IDI_APPLICATION);
	wClass.hCursor = LoadCursorA(NULL, IDC_ARROW);
//#ifdef _DEBUG
//	wClass.hbrBackground = CreateSolidBrush(RGB(255, 0, 255));
//#else
	wClass.hbrBackground = CreateSolidBrush(RGB(0, 0, 0));
//#endif
	wClass.lpszMenuName = NULL;
	wClass.lpszClassName = GAME_NAME "_WindowClass";
	wClass.hIconSm = LoadIconA(NULL, IDI_APPLICATION);

	if (!RegisterClassExA(&wClass)) {
		Result = GetLastError();
		LogMessageA(LL_ERROR, "[%s] Window Registration failed; Error code 0x%08lx", __FUNCTION__, Result);
		MessageBoxA(NULL, "Window Registration Failed", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	gWHandle = CreateWindowExA(
		0,
		wClass.lpszClassName,
		GAME_NAME " - " GAME_VER,
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_BORDER,
		CW_USEDEFAULT, CW_USEDEFAULT, GAME_RES_WIDTH, GAME_RES_HEIGHT,
		NULL, NULL, GetModuleHandleA(NULL), NULL
	);

	if (gWHandle == NULL) {
		Result = GetLastError();
		LogMessageA(LL_ERROR, "[%s] Window Creation failed; Error code 0x%08lx", __FUNCTION__, Result);
		MessageBoxA(NULL, "Window Creation Failed", "Error", MB_ICONERROR | MB_OK);
		goto Exit;
	}

	gPerformanceData.MonitorInfo.cbSize = sizeof(MONITORINFO);

	if (GetMonitorInfoA(MonitorFromWindow(gWHandle, MONITOR_DEFAULTTOPRIMARY), &gPerformanceData.MonitorInfo) == 0) {
		Result = ERROR_MONITOR_NO_DESCRIPTOR;

		LogMessageA(LL_ERROR, "[%s] GetMonitorInfoA failed; Error code 0x%08lx", __FUNCTION__, Result);

		goto Exit;
	}

	/*gPerformanceData.MonitorRect.left = gPerformanceData.MonitorInfo.rcMonitor.right - gPerformanceData.MonitorInfo.rcMonitor.left;
	gPerformanceData.MonitorRect.top = gPerformanceData.MonitorInfo.rcMonitor.bottom - gPerformanceData.MonitorInfo.rcMonitor.top;*/

	/*if (gPerformanceData.WindowWidth == 0) {
		gPerformanceData.WindowWidth = gPerformanceData.MonitorWidth;
	}

	if (gPerformanceData.WindowHeight == 0) {
		gPerformanceData.WindowHeight = gPerformanceData.MonitorHeight;
	}

	if (gPerformanceData.WindowHeight > gPerformanceData.MonitorHeight || gPerformanceData.WindowHeight > gPerformanceData.MonitorWidth) {
		LogMessageA(LL_INFO, "[%s] Window size bigger than saved monitor size; Resizing window to fit", __FUNCTION__);
		gPerformanceData.WindowHeight = gPerformanceData.MonitorHeight;
		gPerformanceData.WindowWidth = gPerformanceData.MonitorWidth;
	}*/

	for (uint8_t Counter = 1; Counter < 12; Counter++) {
		if (GAME_RES_WIDTH * Counter > (gPerformanceData.MonitorInfo.rcMonitor.right - gPerformanceData.MonitorInfo.rcMonitor.left) ||
			GAME_RES_HEIGHT * Counter > (gPerformanceData.MonitorInfo.rcMonitor.bottom - gPerformanceData.MonitorInfo.rcMonitor.top)) {
			gPerformanceData.MaxScaleFactor = Counter - 1;
			break;
		}
	}

	if (gRegistryParams.ScaleFactor == 0) {
		gPerformanceData.CurrentScaleFactor = gPerformanceData.MaxScaleFactor;
	}
	else {
		gPerformanceData.CurrentScaleFactor = (uint8_t)min(gRegistryParams.ScaleFactor, gPerformanceData.MaxScaleFactor);
	}

	LogMessageA(LL_INFO, "[%s] Current Scale Factor: %u; Max Scale Factor: %u", __FUNCTION__, gPerformanceData.CurrentScaleFactor, gPerformanceData.MaxScaleFactor);
	LogMessageA(LL_INFO, "[%s] Drawing at %dx%d", __FUNCTION__, GAME_RES_WIDTH * gPerformanceData.CurrentScaleFactor, GAME_RES_HEIGHT * gPerformanceData.CurrentScaleFactor);

	if (SetWindowLongPtrA(gWHandle, GWL_STYLE, (WS_OVERLAPPEDWINDOW | WS_VISIBLE) & ~WS_OVERLAPPEDWINDOW) == 0) {
		Result = GetLastError();
		LogMessageA(LL_ERROR, "[%s] SetWindowLongPtrA failed; Error code 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}
	if (SetWindowPos(gWHandle,
		HWND_TOP,
		gPerformanceData.MonitorInfo.rcMonitor.left,
		gPerformanceData.MonitorInfo.rcMonitor.top,
		gPerformanceData.MonitorInfo.rcMonitor.right - gPerformanceData.MonitorInfo.rcMonitor.left,
		gPerformanceData.MonitorInfo.rcMonitor.bottom - gPerformanceData.MonitorInfo.rcMonitor.top,
		SWP_FRAMECHANGED | SWP_NOOWNERZORDER) == 0) {
		Result = GetLastError();
		LogMessageA(LL_ERROR, "[%s] SetWindowPos failed; Error code 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}
Exit:
	return Result;
}

BOOL AlreadyRunning(void) {
	HANDLE Mutex = NULL;
	Mutex = CreateMutexA(NULL, FALSE, GAME_NAME "_Mutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS) {
		return TRUE;
	}
	else {
		return FALSE;
	}
}

void ProcessPlayerInput(void) {
	if (gWindowHasFocus == FALSE)
		return;

	gPlayerInput.EscapeKeyDown = GetAsyncKeyState(VK_ESCAPE);
	gPlayerInput.DebugKeyDown = GetAsyncKeyState(VK_F1);
	gPlayerInput.LeftKeyDown = GetAsyncKeyState(VK_LEFT) | GetAsyncKeyState('A');
	gPlayerInput.RightKeyDown = GetAsyncKeyState(VK_RIGHT) | GetAsyncKeyState('D');
	gPlayerInput.UpKeyDown = GetAsyncKeyState(VK_UP) | GetAsyncKeyState('W');
	gPlayerInput.DownKeyDown = GetAsyncKeyState(VK_DOWN) | GetAsyncKeyState('S');
	gPlayerInput.ChooseKeyDown = GetAsyncKeyState(VK_RETURN);

	if (gGamepadID > -1) {
		if (XInputGetState(gGamepadID, &gGamepadState) == ERROR_SUCCESS) {
			gPlayerInput.EscapeKeyDown |= gGamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_BACK;
			gPlayerInput.LeftKeyDown |= gGamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT;
			gPlayerInput.RightKeyDown |= gGamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT;
			gPlayerInput.UpKeyDown |= gGamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP;
			gPlayerInput.DownKeyDown |= gGamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN;
			gPlayerInput.ChooseKeyDown |= gGamepadState.Gamepad.wButtons & XINPUT_GAMEPAD_A;
		}
		else {
			gGamepadID = -1;

			gPreviousGameState = gCurrentGameState;
			gCurrentGameState = GAMESTATE_GAMEPAD_UNPLUGGED;
		}
	}

	if (gPlayerInput.DebugKeyDown && !gPlayerInput.DebugKeyWasDown)
		gPerformanceData.DisplayDebugInfo = !gPerformanceData.DisplayDebugInfo;

	switch (gCurrentGameState) {
	case GAMESTATE_OPENING_SPLASH_SCREEN:
		PPI_OpeningSplashScreen();
		break;
	case GAMESTATE_OPTION_SCREEN:
		PPI_OptionsScreen();
		break;
	case GAMESTATE_GAMEPAD_UNPLUGGED:
		PPI_GamepadUnplugged();
		break;
	case GAMESTATE_TITLE_SCREEN:
		PPI_TitleScreen();
		break;
	case GAMESTATE_OVERWORLD:
		PPI_Overworld();
		break;
	case GAMESTATE_EXIT_YES_NO_SCREEN:
		PPI_ExitYesNo();
		break;
	case GAMESTATE_CHARACTER_NAMING:
		PPI_CharacterNaming();
		break;
	default:
		ASSERT(FALSE, "GAMESTATE not implemented");
	}

	gPlayerInput.EscapeKeyWasDown = gPlayerInput.EscapeKeyDown;
	gPlayerInput.DebugKeyWasDown = gPlayerInput.DebugKeyDown;
	gPlayerInput.LeftKeyWasDown = gPlayerInput.LeftKeyDown;
	gPlayerInput.RightKeyWasDown = gPlayerInput.RightKeyDown;
	gPlayerInput.UpKeyWasDown = gPlayerInput.UpKeyDown;
	gPlayerInput.DownKeyWasDown = gPlayerInput.DownKeyDown;
	gPlayerInput.ChooseKeyWasDown = gPlayerInput.ChooseKeyDown;
}

void FindFirstConnectedGamepad(void) {
	gGamepadID = -1;

	for (int8_t GamepadIndex = 0; GamepadIndex < XUSER_MAX_COUNT && GamepadIndex == -1; GamepadIndex++) {
		XINPUT_STATE State = { 0 };

		if (XInputGetState(GamepadIndex, &State) == ERROR_SUCCESS) {
			gGamepadID = GamepadIndex;
		}
	}
}

DWORD Load32BppBitmapFromFile(_In_ char* Filename, _Inout_ GAME_BITMAP* GameBitmap) {
	DWORD Error = ERROR_SUCCESS;
	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	WORD BitmapHeader = 0;
	DWORD PixelDataOffset = 0;
	DWORD NumBytesRead = 2;

	if ((FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		Error = GetLastError();

		goto Exit;
	}

	if (ReadFile(FileHandle, &BitmapHeader, 2, &NumBytesRead, NULL) == 0) {
		Error = GetLastError();

		goto Exit;
	}

	if (BitmapHeader != 0x4d42) {
		Error = ERROR_FILE_INVALID;

		goto Exit;
	}

	if (SetFilePointer(FileHandle, 0xA, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		Error = GetLastError();

		goto Exit;
	}

	if (ReadFile(FileHandle, &PixelDataOffset, sizeof(DWORD), &NumBytesRead, NULL) == 0) {
		Error = GetLastError();

		goto Exit;
	}

	if (SetFilePointer(FileHandle, 0xE, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		Error = GetLastError();

		goto Exit;
	}

	if (ReadFile(FileHandle, &GameBitmap->bitmapInfo.bmiHeader, sizeof(BITMAPINFOHEADER), &NumBytesRead, NULL) == 0) {
		Error = GetLastError();

		goto Exit;
	}

	if ((GameBitmap->Memory = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, GameBitmap->bitmapInfo.bmiHeader.biSizeImage)) == NULL) {
		Error = ERROR_NOT_ENOUGH_MEMORY;
		
		goto Exit;
	}

	if (SetFilePointer(FileHandle, PixelDataOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		Error = GetLastError();

		goto Exit;
	}

	if (ReadFile(FileHandle, GameBitmap->Memory, GameBitmap->bitmapInfo.bmiHeader.biSizeImage, &NumBytesRead, NULL) == 0) {
		Error = GetLastError();

		goto Exit;
	}

Exit:
	if (FileHandle && FileHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(FileHandle);
	}

	if (Error == ERROR_SUCCESS) {
		LogMessageA(LL_INFO, "[%s] Successfully loaded file %s", __FUNCTION__, Filename);
	}
	else {
		LogMessageA(LL_ERROR, "[%s] Failed to load file %s; Error code 0x%08lx", __FUNCTION__, Filename, Error);
	}

	return Error;
}

DWORD Load32BppBitmapFromMemory(_In_ void* Buffer, _Inout_ GAME_BITMAP* GameBitmap) {
	DWORD Error = ERROR_SUCCESS;
	WORD BitmapHeader = 0;
	DWORD PixelDataOffset = 0;

	memcpy(&BitmapHeader, Buffer, 2);

	if (BitmapHeader != 0x4d42) {
		Error = ERROR_INVALID_DATA;

		goto Exit;
	}

	memcpy(&PixelDataOffset, (BYTE*)Buffer + 0xA, sizeof(DWORD));
	memcpy(&GameBitmap->bitmapInfo.bmiHeader, (BYTE*)Buffer + 0xE, sizeof(BITMAPINFOHEADER));

	GameBitmap->Memory = (BYTE*)Buffer + PixelDataOffset;
Exit:
	if (Error == ERROR_SUCCESS) {
		LogMessageA(LL_INFO, "[%s] Successfully loaded data", __FUNCTION__);
	}
	else {
		LogMessageA(LL_ERROR, "[%s] Failed to load data. 0x%08lx", __FUNCTION__, Error);
	}

	return Error;
}

DWORD LoadWavFromFile(_In_ char* Filename, _Inout_ GAME_SOUND* GameSound) {
	DWORD Error = ERROR_SUCCESS;
	DWORD NumBytesRead = 0;
	DWORD RIFF = 0;
	uint16_t DataChunkOffset = 0;
	DWORD DataChunkSearcher = 0;
	BOOL DataChunkFound = FALSE;
	DWORD DataChunkSize = 0;
	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	void* AudioData = NULL;

	if ((FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] CreateFileA() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (ReadFile(FileHandle, &RIFF, sizeof(DWORD), &NumBytesRead, NULL) == 0) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] ReadFile() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (RIFF != 0x46464952) {
		Error = ERROR_FILE_INVALID;
		LogMessageA(LL_ERROR, "[%s] Invalid file type 0x%08lx", __FUNCTION__, RIFF);
		goto Exit;
	}

	if (SetFilePointer(FileHandle, 20, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] SetFilePointer() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (ReadFile(FileHandle, &GameSound->WaveFormat, sizeof(WAVEFORMATEX), &NumBytesRead, NULL) == 0) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] ReadFile() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (GameSound->WaveFormat.nBlockAlign != ((GameSound->WaveFormat.nChannels * GameSound->WaveFormat.wBitsPerSample) / 8) ||
		(GameSound->WaveFormat.wFormatTag != WAVE_FORMAT_PCM) ||
		(GameSound->WaveFormat.wBitsPerSample != 16)) {
		Error = ERROR_DATATYPE_MISMATCH;
		LogMessageA(LL_ERROR, "[%s] Wav file didn't meet format requirements; Only PCM format, 44.1KHz, and 16 bit per sample wav files are supported. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	while (DataChunkFound == FALSE) {
		if (SetFilePointer(FileHandle, DataChunkOffset, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
			Error = GetLastError();
			LogMessageA(LL_ERROR, "[%s] SetFilePointer() failed with 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}
		if (ReadFile(FileHandle, &DataChunkSearcher, sizeof(DWORD), &NumBytesRead, NULL) == 0) {
			Error = GetLastError();
			LogMessageA(LL_ERROR, "[%s] ReadFile() failed with 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}

		if (DataChunkSearcher == 0x61746164) {
			DataChunkFound = TRUE;
			break;
		}
		else {
			DataChunkOffset += 4;
		}

		if (DataChunkOffset > 256) {
			Error = ERROR_DATATYPE_MISMATCH;
			LogMessageA(LL_ERROR, "[%s] Data chunk not found within 256 bytes. 0x%08lx", __FUNCTION__, Error);

			goto Exit;
		}
	}

	if (SetFilePointer(FileHandle, DataChunkOffset + 4, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] SetFilePointer() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (ReadFile(FileHandle, &DataChunkSize, sizeof(DWORD), &NumBytesRead, NULL) == 0) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] ReadFile() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	AudioData = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, DataChunkSize);

	if (AudioData == NULL) {
		Error = ERROR_NOT_ENOUGH_MEMORY;
		LogMessageA(LL_ERROR, "[%s] HeapAlloc() failed. 0x%08lx", __FUNCTION__, Error);

		goto Exit;
	}

	GameSound->Buffer.Flags = XAUDIO2_END_OF_STREAM;
	GameSound->Buffer.AudioBytes = DataChunkSize;

	if (SetFilePointer(FileHandle, DataChunkOffset + 8, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] SetFilePointer() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (ReadFile(FileHandle, AudioData, DataChunkSize, &NumBytesRead, NULL) == 0) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] ReadFile() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	GameSound->Buffer.pAudioData = AudioData;
Exit:
	if (Error == ERROR_SUCCESS)
		LogMessageA(LL_INFO, "[%s] Successfully loaded %s", __FUNCTION__, Filename);
	else
		LogMessageA(LL_ERROR, "[%s] Failed to load file %s. 0x%08lx", __FUNCTION__, Filename, Error);


	if (FileHandle && FileHandle != INVALID_HANDLE_VALUE)
		CloseHandle(FileHandle);

	return Error;
}

DWORD LoadWavFromMemory(_In_ void* Buffer, _Inout_ GAME_SOUND* GameSound) {
	DWORD Error = ERROR_SUCCESS;
	DWORD RIFF = 0;
	uint16_t DataChunkOffset = 0;
	DWORD DataChunkSearcher = 0;
	BOOL DataChunkFound = FALSE;
	DWORD DataChunkSize = 0;

	memcpy(&RIFF, Buffer, sizeof(DWORD));

	if (RIFF != 0x46464952) {
		Error = ERROR_FILE_INVALID;
		LogMessageA(LL_ERROR, "[%s] Invalid memory 0x%08lx", __FUNCTION__, RIFF);
		goto Exit;
	}

	memcpy(&GameSound->WaveFormat, (BYTE*)Buffer + 20, sizeof(WAVEFORMATEX));

	if (GameSound->WaveFormat.nBlockAlign != ((GameSound->WaveFormat.nChannels * GameSound->WaveFormat.wBitsPerSample) / 8) ||
		(GameSound->WaveFormat.wFormatTag != WAVE_FORMAT_PCM) ||
		(GameSound->WaveFormat.wBitsPerSample != 16)) {
		Error = ERROR_DATATYPE_MISMATCH;
		LogMessageA(LL_ERROR, "[%s] Wav data didn't meet format requirements; Only PCM format, 44.1KHz, and 16 bit per sample wav files are supported. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	while (DataChunkFound == FALSE) {
		memcpy(&DataChunkSearcher, (BYTE*)Buffer + DataChunkOffset, sizeof(DWORD));

		if (DataChunkSearcher == 0x61746164) {
			DataChunkFound = TRUE;
			break;
		}
		else {
			DataChunkOffset += 4;
		}

		if (DataChunkOffset > 256) {
			Error = ERROR_DATATYPE_MISMATCH;
			LogMessageA(LL_ERROR, "[%s] Data chunk not found within 256 bytes 0x%08lx", __FUNCTION__, Error);

			goto Exit;
		}
	}

	memcpy(&DataChunkSize, (BYTE*)Buffer + DataChunkOffset + 4, sizeof(DWORD));

	GameSound->Buffer.Flags = XAUDIO2_END_OF_STREAM;
	GameSound->Buffer.AudioBytes = DataChunkSize;
	GameSound->Buffer.pAudioData = (BYTE*)Buffer + DataChunkOffset + 8;
Exit:
	if (Error == ERROR_SUCCESS)
		LogMessageA(LL_INFO, "[%s] Successfully loaded audio data", __FUNCTION__);
	else
		LogMessageA(LL_ERROR, "[%s] Failed to load audio data. 0x%08lx", __FUNCTION__, Error);

	return Error;
}

DWORD LoadOggFromFile(_In_ char* Filename, _Inout_ GAME_SOUND* GameSound) {
	DWORD Error = ERROR_SUCCESS;
	LARGE_INTEGER FileSize = { 0 };
	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	DWORD NumBytesRead = 0;
	void* FileBuffer = NULL;
	int SamplesDecoded = 0;
	int Channels = 0;
	int SampleRate = 0;
	short* DecodedAudio = NULL;

	if ((FileHandle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] CreateFileA() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (GetFileSizeEx(FileHandle, &FileSize) == 0) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] GetFileSizeEx() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	LogMessageA(LL_INFO, "[%s] Size of file %s: %lu", __FUNCTION__, Filename, FileSize.QuadPart);

	FileBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, FileSize.QuadPart);

	if (FileBuffer == NULL) {
		Error = ERROR_OUTOFMEMORY;
		LogMessageA(LL_ERROR, "[%s] HeapAlloc() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (ReadFile(FileHandle, FileBuffer, (DWORD)FileSize.QuadPart, &NumBytesRead, NULL) == 0) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] ReadFile() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	SamplesDecoded = stb_vorbis_decode_memory(FileBuffer, (int)FileSize.QuadPart, &Channels, &SampleRate, &DecodedAudio);

	if (SamplesDecoded < 1) {
		Error = ERROR_BAD_COMPRESSION_BUFFER;
		LogMessageA(LL_ERROR, "[%s] stb_vorbis_decode_memory() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	GameSound->WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	GameSound->WaveFormat.nChannels = Channels;
	GameSound->WaveFormat.nSamplesPerSec = SampleRate;
	GameSound->WaveFormat.nAvgBytesPerSec = GameSound->WaveFormat.nSamplesPerSec * GameSound->WaveFormat.nChannels * 2;
	GameSound->WaveFormat.nBlockAlign = GameSound->WaveFormat.nChannels * 2;
	GameSound->WaveFormat.wBitsPerSample = 16;
	GameSound->Buffer.Flags = XAUDIO2_END_OF_STREAM;
	GameSound->Buffer.AudioBytes = SamplesDecoded * GameSound->WaveFormat.nChannels * 2;
	GameSound->Buffer.pAudioData = DecodedAudio;

Exit:
	if (FileHandle && (FileHandle != INVALID_HANDLE_VALUE)) {
		CloseHandle(FileHandle);
	}

	if (FileBuffer) {
		HeapFree(GetProcessHeap(), 0, FileBuffer);
	}

	return Error;
}

DWORD LoadOggFromMemory(_In_ void* Buffer, _In_ uint32_t BufferSize, _Inout_ GAME_SOUND* GameSound) {
	DWORD Error = ERROR_SUCCESS;
	int SamplesDecoded = 0;
	int Channels = 0;
	int SampleRate = 0;
	short* DecodedAudio = NULL;

	SamplesDecoded = stb_vorbis_decode_memory(Buffer, (int)BufferSize, &Channels, &SampleRate, &DecodedAudio);

	if (SamplesDecoded < 1) {
		Error = ERROR_BAD_COMPRESSION_BUFFER;
		LogMessageA(LL_ERROR, "[%s] stb_vorbis_decode_memory() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	GameSound->WaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	GameSound->WaveFormat.nChannels = Channels;
	GameSound->WaveFormat.nSamplesPerSec = SampleRate;
	GameSound->WaveFormat.nAvgBytesPerSec = GameSound->WaveFormat.nSamplesPerSec * GameSound->WaveFormat.nChannels * 2;
	GameSound->WaveFormat.nBlockAlign = GameSound->WaveFormat.nChannels * 2;
	GameSound->WaveFormat.wBitsPerSample = 16;
	GameSound->Buffer.Flags = XAUDIO2_END_OF_STREAM;
	GameSound->Buffer.AudioBytes = SamplesDecoded * GameSound->WaveFormat.nChannels * 2;
	GameSound->Buffer.pAudioData = DecodedAudio;

Exit:
	 
	return Error;
}

void Blit32BppBitmapToBuffer(_In_ GAME_BITMAP* GameBitmap, _In_ uint16_t x, _In_ uint16_t y) {
	int32_t StartingScreenPixel = ((GAME_RES_WIDTH * GAME_RES_HEIGHT) - GAME_RES_WIDTH) - (GAME_RES_WIDTH * y) + x;
	int32_t StartingBitmapPixel = ((GameBitmap->bitmapInfo.bmiHeader.biWidth * GameBitmap->bitmapInfo.bmiHeader.biHeight) - \
		GameBitmap->bitmapInfo.bmiHeader.biWidth);
	int32_t MemoryOffset = 0;
	int32_t BitmapOffset = 0;
	PIXEL32 BitmapPixel = { 0 };
	//PIXEL32 BackgroundPixel = { 0 };

	for (int16_t YPixel = 0; YPixel < GameBitmap->bitmapInfo.bmiHeader.biHeight; YPixel++) {
		for (int16_t XPixel = 0; XPixel < GameBitmap->bitmapInfo.bmiHeader.biWidth; XPixel++) {
			MemoryOffset = StartingScreenPixel + XPixel - (GAME_RES_WIDTH * YPixel);
			BitmapOffset = StartingBitmapPixel + XPixel - (GameBitmap->bitmapInfo.bmiHeader.biWidth * YPixel);

			memcpy_s(&BitmapPixel, sizeof(PIXEL32), (PIXEL32*)GameBitmap->Memory + BitmapOffset, sizeof(PIXEL32));

			if (BitmapPixel.Alpha != 0) {
				memcpy_s((PIXEL32*)gCanvas.Memory + MemoryOffset, sizeof(PIXEL32), &BitmapPixel, sizeof(PIXEL32));
			}
		}
	}
}

void BlitStringToBuffer(_In_ char* String, _In_ GAME_BITMAP* FontSheet, _In_ PIXEL32* Color, _In_ uint16_t x, _In_ uint16_t y) {
	uint16_t CharWidth = (uint16_t)FontSheet->bitmapInfo.bmiHeader.biWidth / FONT_SHEET_CHARACTERS_PER_ROW;
	uint16_t CharHeight = (uint16_t)FontSheet->bitmapInfo.bmiHeader.biHeight;
	uint16_t BytesPerCharacter = (CharWidth * CharHeight * (FontSheet->bitmapInfo.bmiHeader.biBitCount / 8));
	uint16_t StringLength = (uint16_t)strlen(String);
	GAME_BITMAP StringBitmap = { 0 };

	StringBitmap.bitmapInfo.bmiHeader.biBitCount = GAME_BPP;
	StringBitmap.bitmapInfo.bmiHeader.biHeight = CharHeight;
	StringBitmap.bitmapInfo.bmiHeader.biWidth = CharWidth * StringLength;
	StringBitmap.bitmapInfo.bmiHeader.biPlanes = 1;
	StringBitmap.bitmapInfo.bmiHeader.biCompression = BI_RGB;
	StringBitmap.Memory = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, ((size_t)BytesPerCharacter * (size_t)StringLength));

	for (int Character = 0; Character < StringLength; Character++) {
		int StartingFontSheetPixel = 0;
		int FontSheetOffset = 0;
		int StringBitmapOffset = 0;
		PIXEL32 FontSheetPixel = { 0 };

		StartingFontSheetPixel = (FontSheet->bitmapInfo.bmiHeader.biWidth * FontSheet->bitmapInfo.bmiHeader.biHeight) - FontSheet->bitmapInfo.bmiHeader.biWidth + (CharWidth * gFontCharacterPixelOffset[(uint8_t)String[Character]]);

		for (int YPixel = 0; YPixel < CharHeight; YPixel++) {
			for (int XPixel = 0; XPixel < CharWidth; XPixel++) {
				FontSheetOffset = StartingFontSheetPixel + XPixel - (FontSheet->bitmapInfo.bmiHeader.biWidth * YPixel);
				StringBitmapOffset = (Character * CharWidth) + (StringBitmap.bitmapInfo.bmiHeader.biWidth * StringBitmap.bitmapInfo.bmiHeader.biHeight) - \
					StringBitmap.bitmapInfo.bmiHeader.biWidth + XPixel - (StringBitmap.bitmapInfo.bmiHeader.biWidth) * YPixel;

				memcpy_s(&FontSheetPixel, sizeof(PIXEL32), (PIXEL32*)FontSheet->Memory + FontSheetOffset, sizeof(PIXEL32));

				FontSheetPixel.Red = Color->Red;
				FontSheetPixel.Green = Color->Green;
				FontSheetPixel.Blue = Color->Blue;

				memcpy_s((PIXEL32*)StringBitmap.Memory + StringBitmapOffset, sizeof(PIXEL32), &FontSheetPixel, sizeof(PIXEL32));
			}
		}
	}

	Blit32BppBitmapToBuffer(&StringBitmap, x, y);

	if (StringBitmap.Memory) {
		HeapFree(GetProcessHeap(), 0, StringBitmap.Memory);
	}
}

void BlitTilemapToBuffer(_In_ GAME_BITMAP* GameBitmap) {
	int32_t StartingScreenPixel = (GAME_RES_WIDTH * GAME_RES_HEIGHT) - GAME_RES_WIDTH;
	int32_t StartingBitmapPixel = ((GameBitmap->bitmapInfo.bmiHeader.biWidth * GameBitmap->bitmapInfo.bmiHeader.biHeight) - \
		GameBitmap->bitmapInfo.bmiHeader.biWidth) + gCamera.x - (GameBitmap->bitmapInfo.bmiHeader.biWidth * gCamera.y);
	int32_t MemoryOffset = 0;
	int32_t BitmapOffset = 0;
#ifdef AVX
	__m256i BitmapOctoPixel;

	for (int16_t YPixel = 0; YPixel < GAME_RES_HEIGHT; YPixel++) {
		for (int16_t XPixel = 0; XPixel < GAME_RES_WIDTH; XPixel += 8) {
			MemoryOffset = StartingScreenPixel + XPixel - (GAME_RES_WIDTH * YPixel);
			BitmapOffset = StartingBitmapPixel + XPixel - (GameBitmap->bitmapInfo.bmiHeader.biWidth * YPixel);
			BitmapOctoPixel = _mm256_loadu_si256((PIXEL32*)GameBitmap->Memory + BitmapOffset);

			_mm256_store_si256((PIXEL32*)gCanvas.Memory + MemoryOffset, BitmapOctoPixel);
		}
	}
#elif defined SSE2
	__m128i BitmapQuadPixel;

	for (int16_t YPixel = 0; YPixel < GAME_RES_HEIGHT; YPixel++) {
		for (int16_t XPixel = 0; XPixel < GAME_RES_WIDTH; XPixel += 4) {
			MemoryOffset = StartingScreenPixel + XPixel - (GAME_RES_WIDTH * YPixel);
			BitmapOffset = StartingBitmapPixel + XPixel - (GameBitmap->bitmapInfo.bmiHeader.biWidth * YPixel);
			BitmapQuadPixel = _mm_load_si128((PIXEL32*)GameBitmap->Memory + BitmapOffset);

			_mm_store_si128((PIXEL32*)gCanvas.Memory + MemoryOffset, BitmapQuadPixel);
		}
	}
#else
	PIXEL32 BitmapPixel = { 0 };

	for (int16_t YPixel = 0; YPixel < GAME_RES_HEIGHT; YPixel++) {
		for (int16_t XPixel = 0; XPixel < GAME_RES_WIDTH; XPixel++) {
			MemoryOffset = StartingScreenPixel + XPixel - (GAME_RES_WIDTH * YPixel);
			BitmapOffset = StartingBitmapPixel + XPixel - (GameBitmap->bitmapInfo.bmiHeader.biWidth * YPixel);

			memcpy_s(&BitmapPixel, sizeof(PIXEL32), (PIXEL32*)GameBitmap->Memory + BitmapOffset, sizeof(PIXEL32));
			memcpy_s((PIXEL32*)gCanvas.Memory + MemoryOffset, sizeof(PIXEL32), &BitmapPixel, sizeof(PIXEL32));
		}
	}
#endif
}

void PlaySoundSfx(_In_ GAME_SOUND* GameSound) {
	gXAudioSFXSourceVoice[gSFXSourceVoiceSelector]->lpVtbl->SubmitSourceBuffer(gXAudioSFXSourceVoice[gSFXSourceVoiceSelector], &GameSound->Buffer, NULL);
	gXAudioSFXSourceVoice[gSFXSourceVoiceSelector]->lpVtbl->Start(gXAudioSFXSourceVoice[gSFXSourceVoiceSelector], 0, XAUDIO2_COMMIT_NOW);

	gSFXSourceVoiceSelector++;

	if (gSFXSourceVoiceSelector > (NUMBER_SFX_SOURCE_VOICES - 1))
		gSFXSourceVoiceSelector = 0;
}

void PlaySoundMusic(_In_ GAME_SOUND* GameSound) {
	gXAudioMusicSourceVoice->lpVtbl->Stop(gXAudioMusicSourceVoice, 0, 0);
	gXAudioMusicSourceVoice->lpVtbl->FlushSourceBuffers(gXAudioMusicSourceVoice);
	GameSound->Buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	gXAudioMusicSourceVoice->lpVtbl->SubmitSourceBuffer(gXAudioMusicSourceVoice, &GameSound->Buffer, NULL);
	gXAudioMusicSourceVoice->lpVtbl->Start(gXAudioMusicSourceVoice, 0, XAUDIO2_COMMIT_NOW);
}

DWORD InitalizeHero(void) {
	//DWORD Error = ERROR_SUCCESS;

	gPlayer.ScreenPos.x = 192;
	gPlayer.ScreenPos.y = 64;
	gCamera.x = 2048;
	gCamera.y = 1920;
	gPlayer.WorldPos.x = gCamera.x + gPlayer.ScreenPos.x;
	gPlayer.WorldPos.y = gCamera.y + gPlayer.ScreenPos.y;
	gPlayer.CurrentArmor = SUIT_0;
	gPlayer.Direction = DIR_DOWN;

	return 0;
	//return Error;
}

HRESULT InitalizeSoundEngine(void) {
	HRESULT Result = S_OK;
	WAVEFORMATEX SfxWaveFormat = { 0 };
	WAVEFORMATEX MusicWaveFormat = { 0 };


	Result = CoInitializeEx(NULL, COINIT_MULTITHREADED);

	if (Result != S_OK) {
		LogMessageA(LL_ERROR, "[%s] CoInitalizeEx() failed with 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}

	Result = XAudio2Create(&gXAudio, 0, XAUDIO2_ANY_PROCESSOR);

	if (FAILED(Result)) {
		LogMessageA(LL_ERROR, "[%s] XAudio2Create() failed with 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}

	Result = gXAudio->lpVtbl->CreateMasteringVoice(gXAudio, &gXAudioMasteringVoice, XAUDIO2_DEFAULT_CHANNELS, XAUDIO2_DEFAULT_SAMPLERATE, 0, 0, NULL, 0);

	if (FAILED(Result)) {
		LogMessageA(LL_ERROR, "[%s] CreateMasteringVoice() for XAudio failed with 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}

	SfxWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	SfxWaveFormat.nChannels = 1;
	SfxWaveFormat.nSamplesPerSec = 44100;
	SfxWaveFormat.nAvgBytesPerSec = SfxWaveFormat.nSamplesPerSec * SfxWaveFormat.nChannels * 2;
	SfxWaveFormat.nBlockAlign = SfxWaveFormat.nChannels * 2;
	SfxWaveFormat.wBitsPerSample = 16;
	SfxWaveFormat.cbSize = 0x6164;

	for (uint8_t Counter = 0; Counter < NUMBER_SFX_SOURCE_VOICES; Counter++) {
		Result = gXAudio->lpVtbl->CreateSourceVoice(gXAudio, &gXAudioSFXSourceVoice[Counter], &SfxWaveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);

		if (Result != S_OK) {
			LogMessageA(LL_ERROR, "[%s] CreateSourceVoice() for XAudio failed with 0x%08lx", __FUNCTION__, Result);
			goto Exit;
		}

		gXAudioSFXSourceVoice[Counter]->lpVtbl->SetVolume(gXAudioSFXSourceVoice[Counter], gSFXVolume, XAUDIO2_COMMIT_NOW);
	}

	MusicWaveFormat.wFormatTag = WAVE_FORMAT_PCM;
	MusicWaveFormat.nChannels = 2;
	MusicWaveFormat.nSamplesPerSec = 44100;
	MusicWaveFormat.nAvgBytesPerSec = MusicWaveFormat.nSamplesPerSec * MusicWaveFormat.nChannels * 2;
	MusicWaveFormat.nBlockAlign = MusicWaveFormat.nChannels * 2;
	MusicWaveFormat.wBitsPerSample = 16;
	MusicWaveFormat.cbSize = 0;

	Result = gXAudio->lpVtbl->CreateSourceVoice(gXAudio, &gXAudioMusicSourceVoice, &MusicWaveFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, NULL, NULL, NULL);

	if (Result != S_OK) {
		LogMessageA(LL_ERROR, "[%s] CreateSourceVoice() for XAudio failed with 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}

	gXAudioMusicSourceVoice->lpVtbl->SetVolume(gXAudioMusicSourceVoice, gMusicVolume, XAUDIO2_COMMIT_NOW);
Exit:
	return Result;
}

DWORD LoadRegistryParams(void) {
	DWORD Result = ERROR_SUCCESS;
	HKEY RegKey = NULL;
	DWORD RegDisposition = 0;
	DWORD RegBytesRead = sizeof(DWORD);
	gRegistryParams.LogLevel = LL_INFO;

	Result = RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\" GAME_NAME, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &RegKey, &RegDisposition);

	if (Result != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] RegCreateKeyExA failed with error code 0x%08lx", __FUNCTION__, Result);

		goto Exit;
	}

	if (RegDisposition == REG_CREATED_NEW_KEY) {
		LogMessageA(LL_INFO, "[%s] Created new Registry key due to it not existing; New key located at \"HKEY_CURRENT_USER\\Software\\%s\"", __FUNCTION__, GAME_NAME);
	}
	else {
		LogMessageA(LL_INFO, "[%s] Opened Registry key located at \"HKEY_CURRENT_USER\\Software\\%s\"", __FUNCTION__, GAME_NAME);
	}

	Result = RegGetValueA(RegKey, NULL, "LogLevel", RRF_RT_DWORD, NULL, (BYTE*)&gRegistryParams.LogLevel, &RegBytesRead);

	if (Result != ERROR_SUCCESS) {
		if (Result == ERROR_FILE_NOT_FOUND) {
			Result = ERROR_SUCCESS;

			LogMessageA(LL_INFO, "[%s] Registry value \"LogLevel\" not found; Creating and using default of 3 (LL_WARNING)", __FUNCTION__);
			gRegistryParams.LogLevel = LL_WARNING;


		}
		else {
			LogMessageA(LL_ERROR, "[%s] Failed to read \"LogLevel\" in Registry; Error code 0x%08lx", __FUNCTION__, Result);
			goto Exit;
		}
	}

	LogMessageA(LL_INFO, "[%s] LogLevel found is %d", __FUNCTION__, gRegistryParams.LogLevel);

	Result = RegGetValueA(RegKey, NULL, "ScaleFactor", RRF_RT_DWORD, NULL, (BYTE*)&gRegistryParams.ScaleFactor, &RegBytesRead);

	if (Result != ERROR_SUCCESS) {
		if (Result == ERROR_FILE_NOT_FOUND) {
			Result = ERROR_SUCCESS;

			LogMessageA(LL_INFO, "[%s] Registry value \"ScaleFactor\" not found; Using default of 0", __FUNCTION__);
			gRegistryParams.ScaleFactor = 0;
		}
		else {
			LogMessageA(LL_ERROR, "[%s] Failed to read \"ScaleFactor\" in Registry; Error code 0x%08lx", __FUNCTION__, Result);
			goto Exit;
		}
	}

	LogMessageA(LL_INFO, "[%s] ScaleFactor found is %d", __FUNCTION__, gRegistryParams.ScaleFactor);

	Result = RegGetValueA(RegKey, NULL, "SFXVolume", RRF_RT_DWORD, NULL, (BYTE*)&gRegistryParams.SFX_Volume, &RegBytesRead);

	if (Result != ERROR_SUCCESS) {
		if (Result == ERROR_FILE_NOT_FOUND) {
			Result = ERROR_SUCCESS;

			LogMessageA(LL_INFO, "[%s] Registry value \"SFXVolume\" not found; Using default of 0.5", __FUNCTION__);
			gRegistryParams.SFX_Volume = 50;
		}
		else {
			LogMessageA(LL_ERROR, "[%s] Failed to read \"SFXVolume\" in Registry; Error code 0x%08lx", __FUNCTION__, Result);
			goto Exit;
		}
	}

	LogMessageA(LL_INFO, "[%s] SFXVolume found is %.1f", __FUNCTION__, gRegistryParams.SFX_Volume / 10.0f);

	Result = RegGetValueA(RegKey, NULL, "MusicVolume", RRF_RT_DWORD, NULL, (BYTE*)&gRegistryParams.Music_Volume, &RegBytesRead);

	if (Result != ERROR_SUCCESS) {
		if (Result == ERROR_FILE_NOT_FOUND) {
			Result = ERROR_SUCCESS;

			LogMessageA(LL_INFO, "[%s] Registry value \"MusicVolume\" not found; Using default of 0.5", __FUNCTION__);
			gRegistryParams.Music_Volume = 50;
		}
		else {
			LogMessageA(LL_ERROR, "[%s] Failed to read \"MusicVolume\" in Registry; Error code 0x%08lx", __FUNCTION__, Result);
			goto Exit;
		}
	}

	LogMessageA(LL_INFO, "[%s] MusicVolume found is %.1f", __FUNCTION__, gRegistryParams.Music_Volume / 10.0f);

	gMusicVolume = gRegistryParams.Music_Volume / 10.0f;
	gSFXVolume = gRegistryParams.SFX_Volume / 10.0f;
Exit:
	if (RegKey) {
		RegCloseKey(RegKey);
	}

	return Result;
}

DWORD SaveRegistryParams(void) {
	DWORD Result = ERROR_SUCCESS;
	HKEY RegKey = NULL;
	DWORD RegDispositon = 0;
	DWORD SFXVolume = (DWORD)(gSFXVolume * 10.0f);
	DWORD MusicVolume = (DWORD)(gMusicVolume * 10.0f);
	Result = RegCreateKeyExA(HKEY_CURRENT_USER, "SOFTWARE\\" GAME_NAME, 0, NULL, 0, KEY_ALL_ACCESS, NULL, &RegKey, &RegDispositon);

	if (Result != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] RegCreateKeyExA failed with error code 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}

	Result = RegSetValueExA(RegKey, "ScaleFactor", 0, REG_DWORD, &gPerformanceData.CurrentScaleFactor, sizeof(DWORD));

	if (Result != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to set \"ScaleFactor\" in Registry; Error code 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}

	Result = RegSetValueExA(RegKey, "SFXVolume", 0, REG_DWORD, (const BYTE*)&SFXVolume, sizeof(DWORD));

	if (Result != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to set \"SFXVolume\" in Registry; Error code 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}

	Result = RegSetValueExA(RegKey, "MusicVolume", 0, REG_DWORD, (const BYTE*)&MusicVolume, sizeof(DWORD));

	if (Result != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to set \"MusicVolume\" in Registry; Error code 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}

	Result = RegSetValueExA(RegKey, "LogLevel", 0, REG_DWORD, (const BYTE*)&gRegistryParams.LogLevel, sizeof(DWORD));

	if (Result != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to set \"LogLevel\" in Registry; Error code 0x%08lx", __FUNCTION__, Result);
		goto Exit;
	}
Exit:
	if (RegKey)
		RegCloseKey(RegKey);

	return Result;
}

void LogMessageA(_In_ LOG_LEVEL LogLevel, _In_ char* Message, _In_ ...) {
	size_t MessageLength = strlen(Message);
	SYSTEMTIME Time = { 0 };
	HANDLE LogFileHandle = INVALID_HANDLE_VALUE;
	DWORD EndOfFile = 0;
	DWORD NumBytesWritten = 0;
	char DateTimeString[96] = { 0 };
	char SeverityString[12] = { 0 };
	char FormattedString[4096] = { 0 };
	int Error = 0;

	if (gRegistryParams.LogLevel < (DWORD)LogLevel) {
		return;
	}

	if (MessageLength < 1 || MessageLength > 4096) {
		ASSERT(FALSE, "Message either too short or too long");
		return;
	}

	switch (LogLevel) {
	case LL_ALWAYS:
		strcpy_s(SeverityString, sizeof(SeverityString), " [ALWAYS] ");

		break;
	case LL_NONE:
		return;
	case LL_INFO:
		strcpy_s(SeverityString, sizeof(SeverityString), " [INFO] ");

		break;
	case LL_WARNING:
		strcpy_s(SeverityString, sizeof(SeverityString), " [WARN] ");

		break;
	case LL_ERROR:
		strcpy_s(SeverityString, sizeof(SeverityString), " [ERROR] ");

		break;
	case LL_DEBUG:
		strcpy_s(SeverityString, sizeof(SeverityString), " [DEBUG] ");

		break;
	default:
		ASSERT(FALSE, "Unrecognized log level");
	}

	GetLocalTime(&Time);
	va_list ArgPointer = NULL;
	va_start(ArgPointer, Message);

	_vsnprintf_s(FormattedString, sizeof(FormattedString), _TRUNCATE, Message, ArgPointer);

	va_end(ArgPointer);

	Error = _snprintf_s(DateTimeString, sizeof(DateTimeString), _TRUNCATE, "\r\n[%02u/%02u/%u %02u:%02u:%02u.%03u]", Time.wMonth, Time.wDay, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond, Time.wMilliseconds);

	EnterCriticalSection(&gLogCritSec);

	if ((LogFileHandle = CreateFileA(LOG_FILE_NAME, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		ASSERT(FALSE, "Failed to access log file");
	}

	EndOfFile = SetFilePointer(LogFileHandle, 0, NULL, FILE_END);
	WriteFile(LogFileHandle, DateTimeString, (DWORD)strlen(DateTimeString), &NumBytesWritten, NULL);
	WriteFile(LogFileHandle, SeverityString, (DWORD)strlen(SeverityString), &NumBytesWritten, NULL);
	WriteFile(LogFileHandle, FormattedString, (DWORD)strlen(FormattedString), &NumBytesWritten, NULL);

	if (LogFileHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(LogFileHandle);
	}

	LeaveCriticalSection(&gLogCritSec);
}

void RenderFrameGraphic(void) {
	switch (gCurrentGameState) {
	case GAMESTATE_OPENING_SPLASH_SCREEN:
		DrawOpeningSplashScreen();
		break;
	case GAMESTATE_OPTION_SCREEN:
		DrawOptionsScreen();
		break;
	case GAMESTATE_GAMEPAD_UNPLUGGED:
		DrawGamepadUnpluggedScreen();
		break;
	case GAMESTATE_TITLE_SCREEN:
		DrawTitleScreen();
		break;
	case GAMESTATE_EXIT_YES_NO_SCREEN:
		DrawExitYesNoScreen();
		break;
	case GAMESTATE_CHARACTER_NAMING:
		DrawCharacterNaming();
		break;
	case GAMESTATE_OVERWORLD:
		DrawOverworld();
		break;
	default:
		ASSERT(FALSE, "GAMESTATE not implemented");
	}

	if (gPerformanceData.DisplayDebugInfo == TRUE)
		DrawDebugInfo();

	HDC DeviceContext = GetDC(gWHandle);

	StretchDIBits(DeviceContext,
		((gPerformanceData.MonitorInfo.rcMonitor.right - gPerformanceData.MonitorInfo.rcMonitor.left) / 2) - ((GAME_RES_WIDTH * gPerformanceData.CurrentScaleFactor) / 2),
		((gPerformanceData.MonitorInfo.rcMonitor.bottom - gPerformanceData.MonitorInfo.rcMonitor.top) / 2) - ((GAME_RES_HEIGHT * gPerformanceData.CurrentScaleFactor) / 2),
		GAME_RES_WIDTH * gPerformanceData.CurrentScaleFactor,
		GAME_RES_HEIGHT * gPerformanceData.CurrentScaleFactor,
		0,
		0,
		GAME_RES_WIDTH,
		GAME_RES_HEIGHT,
		gCanvas.Memory,
		&gCanvas.bitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	ReleaseDC(gWHandle, DeviceContext);
}

void DrawDebugInfo(void) {
	char DebugTextBuffer[64] = { 0 };

	PIXEL32 White = { 0xFF, 0xFF, 0xFF, 0xFF };

	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "FPS Raw: %.01f", gPerformanceData.rawAvgFPS);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 0);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "FPS: %.01f", gPerformanceData.cookedAvgFPS);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 8);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "Max Timer Res: %.03f", gPerformanceData.MaximumTimerResolution / 10000.0f);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 16);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "Min Timer Res: %.03f", gPerformanceData.MinimumTimerResolution / 10000.0f);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 24);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "Current Timer Res: %.03f", gPerformanceData.CurrentTimerResolution / 10000.0f);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 32);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "Handles: %lu", gPerformanceData.HandleCount);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 40);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "Memory: %zuKB", gPerformanceData.MemInfo.PrivateUsage / 1024);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 48);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "CPU: %.02f%%", gPerformanceData.CPUPercent);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 56);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "Total Frames: %llu", gPerformanceData.totalFramesRendered);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 64);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "Screen Position: (%hu,%hu)", gPlayer.ScreenPos.x, gPlayer.ScreenPos.y);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 72);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "World Position: (%hu,%hu)", gPlayer.WorldPos.x, gPlayer.WorldPos.y);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 80);
	sprintf_s(DebugTextBuffer, _countof(DebugTextBuffer), "Camera Position: (%hu,%hu)", gCamera.x, gCamera.y);
	BlitStringToBuffer(DebugTextBuffer, &g6x7Font, &White, 0, 88);
}

#ifdef AVX
__forceinline void ClearScreen(_In_ __m256i* Color) {
	for (int x = 0; x < (GAME_RES_WIDTH * GAME_RES_HEIGHT) / 8; x++) {
		_mm256_store_si256((__m256i*)gCanvas.Memory + x, *Color);
	}
}
#elif defined SSE2
__forceinline void ClearScreen(_In_ __m128i* Color) {
	for (int x = 0; x < (GAME_RES_WIDTH * GAME_RES_HEIGHT) / 4; x++) {
		_mm_store_si128((__m128i*)gCanvas.Memory + x, *Color);
	}
}
#else
__forceinline void ClearScreen(_In_ PIXEL32* Pixel) {
	for (int x = 0; x < GAME_RES_WIDTH * GAME_RES_HEIGHT; x++) {
		memcpy((PIXEL32*)gCanvas.Memory + x, Pixel, sizeof(PIXEL32));
	}
}
DWORD LoadOggFromMemory(void* Buffer, uint64_t BufferSize, GAME_SOUND* GameSound)
{
	return 0;
}
#endif

DWORD LoadTilemapFromFile(_In_ char* FileName, _Inout_ TILEMAP* TileMap) {
	DWORD Error = ERROR_SUCCESS;
	LARGE_INTEGER FileSize = { 0 };
	HANDLE FileHandle = INVALID_HANDLE_VALUE;
	DWORD NumBytesRead = 0;
	void* FileBuffer = NULL;
	char* Cursor = NULL;
	char TempBuffer[16] = { 0 };
	uint16_t rows = 0;
	uint16_t columns = 0;

	if ((FileHandle = CreateFileA(FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL)) == INVALID_HANDLE_VALUE) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] CreateFileA() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (GetFileSizeEx(FileHandle, &FileSize) == 0) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] GetFileSizeEx() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	LogMessageA(LL_INFO, "[%s] Size of file %s: %lu", __FUNCTION__, FileName, FileSize.QuadPart);

	if (FileSize.QuadPart < 300) {
		Error = ERROR_FILE_INVALID;
		LogMessageA(LL_ERROR, "[%s] File size too small to be a valid Tilemap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	FileBuffer = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, FileSize.QuadPart);

	if (FileBuffer == NULL) {
		Error = ERROR_OUTOFMEMORY;
		LogMessageA(LL_ERROR, "[%s] HeapAlloc() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if (ReadFile(FileHandle, FileBuffer, (DWORD)FileSize.QuadPart, &NumBytesRead, NULL) == 0) {
		Error = GetLastError();
		LogMessageA(LL_ERROR, "[%s] ReadFile() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Cursor = strstr(FileBuffer, "width=")) == NULL) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] strstr() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	NumBytesRead = 0;

	for (;;) {
		if (NumBytesRead > 8) {
			Error = ERROR_INVALID_DATA;
			LogMessageA(LL_ERROR, "[%s] Could not locate opening quote before width attribute: 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}

		if (*Cursor == '\"') {
			Cursor++;
			break;
		}
		else {
			Cursor++;
		}

		NumBytesRead++;
	}

	NumBytesRead = 0;

	for (uint8_t Counter = 0; Counter < 6; Counter++) {
		if (*Cursor == '\"') {
			Cursor++;
			break;
		}
		else {
			TempBuffer[Counter] = *Cursor;
			Cursor++;
		}
	}

	TileMap->width = (uint16_t)atoi(TempBuffer);

	if (TileMap->width == 0) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] TileMap width is 0: 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	memset(TempBuffer, 0, sizeof(TempBuffer));

	if ((Cursor = strstr(FileBuffer, "height=")) == NULL) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] strstr() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	NumBytesRead = 0;

	for (;;) {
		if (NumBytesRead > 8) {
			Error = ERROR_INVALID_DATA;
			LogMessageA(LL_ERROR, "[%s] Could not locate opening quote before height attribute: 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}

		if (*Cursor == '\"') {
			Cursor++;
			break;
		}
		else {
			Cursor++;
		}

		NumBytesRead++;
	}

	NumBytesRead = 0;

	for (uint8_t Counter = 0; Counter < 6; Counter++) {
		if (*Cursor == '\"') {
			Cursor++;
			break;
		}
		else {
			TempBuffer[Counter] = *Cursor;
			Cursor++;
		}
	}

	TileMap->height = (uint16_t)atoi(TempBuffer);

	if (TileMap->height == 0) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] TileMap height is 0: 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	rows = TileMap->height;
	columns = TileMap->width;

	TileMap->map = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, rows * sizeof(void*));

	if (TileMap->map == NULL) {
		Error = ERROR_OUTOFMEMORY;
		LogMessageA(LL_ERROR, "[%s] HeapAlloc() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	for (uint16_t Counter = 0; Counter < TileMap->height; Counter++) {
		TileMap->map[Counter] = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, columns * sizeof(void*));

		if (TileMap->map[Counter] == NULL) {
			Error = ERROR_OUTOFMEMORY;
			LogMessageA(LL_ERROR, "[%s] HeapAlloc() failed with 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}
	}

	NumBytesRead = 0;
	
	memset(TempBuffer, 0, sizeof(TempBuffer));

	if ((Cursor = strstr(FileBuffer, ",")) == NULL) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] strstr() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	while (*Cursor != '\r' && *Cursor != '\n') {
		if (NumBytesRead > 3) {
			Error = ERROR_INVALID_DATA;
			LogMessageA(LL_ERROR, "[%s] Cannot find beginning of map tmx file data. 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}

		NumBytesRead++;
		Cursor--;
	}

	Cursor++;

	for (uint16_t row = 0; row < rows; row++) {
		for (uint16_t column = 0; column < columns; column++) {
			memset(TempBuffer, 0, sizeof(TempBuffer));

		Skip:


			if (*Cursor == '\r' || *Cursor == '\n') {
				Cursor++;
				goto Skip;
			}

			for (uint8_t Counter = 0; Counter < 8; Counter++) {
				if (*Cursor == ',' || *Cursor == '<') {
					if (((TileMap->map[row][column]) = (uint8_t)atoi(TempBuffer)) == 0) {
						Error = ERROR_INVALID_DATA;
						LogMessageA(LL_ERROR, "[%s] atoi() failed. 0x%08lx", __FUNCTION__, Error);
						goto Exit;
					}

					Cursor++;

					break;
				}

				TempBuffer[Counter] = *Cursor;
				Cursor++;
			}
		}
	}

Exit:
	if (FileHandle && (FileHandle != INVALID_HANDLE_VALUE)) {
		CloseHandle(FileHandle);
	}

	if (FileBuffer) {
		HeapFree(GetProcessHeap(), 0, FileBuffer);
	}

	return Error;
}

DWORD LoadTilemapFromMemory(_In_ void* Buffer, _In_ uint32_t BufferSize, _Inout_ TILEMAP* TileMap) {
	DWORD Error = ERROR_SUCCESS;
	DWORD NumBytesRead = 0;
	char* Cursor = NULL;
	char TempBuffer[16] = { 0 };
	uint16_t rows = 0;
	uint16_t columns = 0;

	if (BufferSize < 300) {
		Error = ERROR_FILE_INVALID;
		LogMessageA(LL_ERROR, "[%s] Size too small to be a valid Tilemap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}
	
	if ((Cursor = strstr(Buffer, "width=")) == NULL) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] strstr() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	NumBytesRead = 0;

	for (;;) {
		if (NumBytesRead > 8) {
			Error = ERROR_INVALID_DATA;
			LogMessageA(LL_ERROR, "[%s] Could not locate opening quote before width attribute: 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}

		if (*Cursor == '\"') {
			Cursor++;
			break;
		}
		else {
			Cursor++;
		}

		NumBytesRead++;
	}

	NumBytesRead = 0;

	for (uint8_t Counter = 0; Counter < 6; Counter++) {
		if (*Cursor == '\"') {
			Cursor++;
			break;
		}
		else {
			TempBuffer[Counter] = *Cursor;
			Cursor++;
		}
	}

	TileMap->width = (uint16_t)atoi(TempBuffer);

	if (TileMap->width == 0) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] TileMap width is 0: 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	memset(TempBuffer, 0, sizeof(TempBuffer));

	if ((Cursor = strstr(Buffer, "height=")) == NULL) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] strstr() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	NumBytesRead = 0;

	for (;;) {
		if (NumBytesRead > 8) {
			Error = ERROR_INVALID_DATA;
			LogMessageA(LL_ERROR, "[%s] Could not locate opening quote before height attribute: 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}

		if (*Cursor == '\"') {
			Cursor++;
			break;
		}
		else {
			Cursor++;
		}

		NumBytesRead++;
	}

	NumBytesRead = 0;

	for (uint8_t Counter = 0; Counter < 6; Counter++) {
		if (*Cursor == '\"') {
			Cursor++;
			break;
		}
		else {
			TempBuffer[Counter] = *Cursor;
			Cursor++;
		}
	}

	TileMap->height = (uint16_t)atoi(TempBuffer);

	if (TileMap->height == 0) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] TileMap height is 0: 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	rows = TileMap->height;
	columns = TileMap->width;

	TileMap->map = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, rows * sizeof(void*));

	if (TileMap->map == NULL) {
		Error = ERROR_OUTOFMEMORY;
		LogMessageA(LL_ERROR, "[%s] HeapAlloc() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	for (uint16_t Counter = 0; Counter < TileMap->height; Counter++) {
		TileMap->map[Counter] = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, columns * sizeof(void*));

		if (TileMap->map[Counter] == NULL) {
			Error = ERROR_OUTOFMEMORY;
			LogMessageA(LL_ERROR, "[%s] HeapAlloc() failed with 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}
	}

	NumBytesRead = 0;

	memset(TempBuffer, 0, sizeof(TempBuffer));

	if ((Cursor = strstr(Buffer, ",")) == NULL) {
		Error = ERROR_INVALID_DATA;
		LogMessageA(LL_ERROR, "[%s] strstr() failed with 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	while (*Cursor != '\r' && *Cursor != '\n') {
		if (NumBytesRead > 3) {
			Error = ERROR_INVALID_DATA;
			LogMessageA(LL_ERROR, "[%s] Cannot find beginning of map tmx data. 0x%08lx", __FUNCTION__, Error);
			goto Exit;
		}

		NumBytesRead++;
		Cursor--;
	}

	Cursor++;

	for (uint16_t row = 0; row < rows; row++) {
		for (uint16_t column = 0; column < columns; column++) {
			memset(TempBuffer, 0, sizeof(TempBuffer));

		Skip:


			if (*Cursor == '\r' || *Cursor == '\n') {
				Cursor++;
				goto Skip;
			}

			for (uint8_t Counter = 0; Counter < 8; Counter++) {
				if (*Cursor == ',' || *Cursor == '<') {
					if (((TileMap->map[row][column]) = (uint8_t)atoi(TempBuffer)) == 0) {
						Error = ERROR_INVALID_DATA;
						LogMessageA(LL_ERROR, "[%s] atoi() failed. 0x%08lx", __FUNCTION__, Error);
						goto Exit;
					}

					Cursor++;

					break;
				}

				TempBuffer[Counter] = *Cursor;
				Cursor++;
			}
		}
	}

Exit:
	return Error;
}

DWORD LoadAssetFromArchive(_In_ char* ArchiveName, _In_ char* ArchiveFilename, _In_ RESOURCE_TYPE ResourceType, _Inout_ void* Resource) {
	DWORD Error = ERROR_SUCCESS;
	mz_zip_archive Archive = { 0 };
	BYTE* DecompressedBuffer = NULL;
	size_t DecompressedSize = 0;
	BOOL FileFoundInArchive = FALSE;

	if (mz_zip_reader_init_file(&Archive, ArchiveName, 0) == MZ_FALSE) {
		Error = mz_zip_get_last_error(&Archive);
		LogMessageA(LL_ERROR, "[%s] mz_zip_reader_init_file() failed. 0x%08lx, Error: %s", __FUNCTION__, Error, mz_zip_get_error_string(Error));
		goto Exit;
	}

	for (int FileIndex = 0; FileIndex < (int)mz_zip_reader_get_num_files(&Archive); FileIndex++) {
		mz_zip_archive_file_stat CompressedFileStats = { 0 };

		if (mz_zip_reader_file_stat(&Archive, FileIndex, &CompressedFileStats) == MZ_FALSE) {
			Error = mz_zip_get_last_error(&Archive);
			LogMessageA(LL_ERROR, "[%s] mz_zip_reader_file_stat() failed. 0x%08lx, Error: %s", __FUNCTION__, Error, mz_zip_get_error_string(Error));
			goto Exit;
		}

		if (_stricmp(CompressedFileStats.m_filename, ArchiveFilename) == 0) {
			FileFoundInArchive = TRUE;

			if ((DecompressedBuffer = mz_zip_reader_extract_to_heap(&Archive, FileIndex, &DecompressedSize, 0)) == NULL) {
				Error = mz_zip_get_last_error(&Archive);
				LogMessageA(LL_ERROR, "[%s] mz_zip_reader_extract_to_heap() failed. 0x%08lx, Error: %s", __FUNCTION__, Error, mz_zip_get_error_string(Error));
				goto Exit;
			}

			break;
		}
	}

	if (FileFoundInArchive == FALSE) {
		Error = ERROR_FILE_NOT_FOUND;
		LogMessageA(LL_ERROR, "[%s] Failed to find file in archive. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	switch (ResourceType) {
	case RESOURCE_TYPE_WAV:
		Error = LoadWavFromMemory(DecompressedBuffer, Resource);
		break;
	case RESOURCE_TYPE_OGG:
		Error = LoadOggFromMemory(DecompressedBuffer, (uint32_t)DecompressedSize, Resource);
		break;
	case RESOURCE_TYPE_TILEMAP:
		Error = LoadTilemapFromMemory(DecompressedBuffer, (uint32_t)DecompressedSize, Resource);
		break;
	case RESOURCE_TYPE_BMPX:
		Error = Load32BppBitmapFromMemory(DecompressedBuffer, Resource);
		break;
	default:
		ASSERT(FALSE, "ResourceType is not a valid resource type")
		break;
	}

Exit:

	mz_zip_reader_end(&Archive);

	return Error;
}

DWORD WINAPI AssetLoadingThreadProc(_In_ LPVOID lpParam) {
	UNREFERENCED_PARAMETER(lpParam);
	DWORD Error = ERROR_SUCCESS;

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "6x7Font.bmpx", RESOURCE_TYPE_BMPX, &g6x7Font)) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to load font. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "SplashScreen.wav", RESOURCE_TYPE_WAV, &gSound_SplashScreen)) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to load SplashScreen.wav. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	SetEvent(gSplashScreenLoadedEvent);

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Overworld01.bmpx", RESOURCE_TYPE_BMPX, &gOverworld01.gamebitmap)) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to load overworld map. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Overworld01.tmx", RESOURCE_TYPE_TILEMAP, &gOverworld01.tilemap)) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to load overworld map. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "MenuNavigate.wav", RESOURCE_TYPE_WAV, &gSound_MenuNavigate)) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to load MenuNavigate.wav. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "MenuChoose.wav", RESOURCE_TYPE_WAV, &gSound_MenuChoose)) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to load MenuChoose.wav. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Overworld01.ogg", RESOURCE_TYPE_OGG, &gMusic_Overworld01)) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed to load Overworld01.ogg. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Down_Standing.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_DOWN_0])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Down_Walk1.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_DOWN_1])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Down_Walk2.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_DOWN_2])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Left_Standing.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_LEFT_0])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Left_Walk1.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_LEFT_1])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Left_Walk2.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_LEFT_2])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Right_Standing.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_RIGHT_0])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Right_Walk1.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_RIGHT_1])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Right_Walk2.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_RIGHT_2])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Up_Standing.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_UPWARD_0])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Up_Walk1.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_UPWARD_1])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}

	if ((Error = LoadAssetFromArchive(ASSET_FILE, "Hero_Suit0_Up_Walk2.bmpx", RESOURCE_TYPE_BMPX, &gPlayer.Sprite[SUIT_0][FACING_UPWARD_2])) != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Failed loading player bitmap. 0x%08lx", __FUNCTION__, Error);
		goto Exit;
	}
Exit:
	return Error;
}