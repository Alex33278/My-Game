#pragma once
#define SSE2
#define AVX

#pragma warning(disable: 4820) // Padding
#pragma warning(disable: 5045) // Qspectre
#pragma warning(disable: 4191) // unsafe conversion from "FARPROC" to "_NtQueryTimerResolution"
#pragma warning(disable: 4018) // signed/unsigned mismatch for < operator
#pragma warning(disable: 4061) // Not explicitly handled by a case label
#pragma warning(disable: 4133) // incompadible types (Pixel32 -> __m256i)
#pragma warning(disable: 4668) // Win32 stuff
#pragma warning(disable: 4711) // Auto inline
#pragma warning(disable: 4710) // Not inline

#pragma warning(push, 3)
#include <Windows.h>
#include <xaudio2.h>
#pragma comment(lib, "XAudio2.lib")
#include <stdio.h>
#include <stdint.h>
#pragma comment(lib, "Winmm.lib")
#include <Psapi.h>
#include <Xinput.h>
#pragma comment(lib, "XInput.lib")

#ifdef AVX
#include <immintrin.h>
#elif defined SSE2
#include <emmintrin.h>
#endif
#pragma warning(pop)

#include "Tiles.h"

#ifdef _DEBUG
#define ASSERT(Expression, Message) if (!(Expression)) { *(int*)0 = 0; }
#else
#define ASSERT(Expression, Message) ((void)0);
#endif

#define GAME_NAME "A Unlikely Hero"
#ifdef _DEBUG
#define GAME_VER "DEV"
#else
#define GAME_VER "ALPHA 1.0"
#endif
#define GAME_STUDIO "Alex Games"
#define ASSET_FILE "Assets.dat"
#define GAME_RES_WIDTH 384
#define GAME_RES_HEIGHT 240
#define GAME_BPP 32
#define GAME_CANVAS_MEMORY_SIZE (GAME_RES_WIDTH * GAME_RES_HEIGHT * (GAME_BPP / 8))

#define CALCXPOS(LettersLong) (LettersLong != 0 ? (GAME_RES_WIDTH / 2) - ((LettersLong * 6) / 2) : GAME_RES_WIDTH / 2)

#define CALCULATE_AVG_FPS_Every_X_Frames 120.0
#define TARGET_MICROSECONDS_PER_FRAME 16667ULL

#define SUIT_0	0
#define SUIT_1	1
#define SUIT_2	2

#define FACING_DOWN_0	0
#define FACING_DOWN_1	1
#define FACING_DOWN_2	2
#define FACING_LEFT_0	3
#define FACING_LEFT_1	4
#define FACING_LEFT_2	5
#define FACING_RIGHT_0	6
#define FACING_RIGHT_1	7
#define FACING_RIGHT_2	8
#define FACING_UPWARD_0	9
#define FACING_UPWARD_1	10
#define FACING_UPWARD_2	11

#define FONT_SHEET_CHARACTERS_PER_ROW 98
#define LOG_FILE_NAME GAME_NAME " Log.log"
#define NUMBER_SFX_SOURCE_VOICES 4

typedef enum DIRECTION {
	DIR_DOWN = 0,
	DIR_LEFT = 3,
	DIR_RIGHT = 6,
	DIR_UP = 9
} DIRECTION;


typedef enum LOG_LEVEL {
	LL_ALWAYS = 0,
	LL_NONE = 1,
	LL_INFO = 4,
	LL_WARNING = 3,
	LL_ERROR = 2,
	LL_DEBUG = 5
} LOG_LEVEL;


typedef enum GAME_STATE {
	GAMESTATE_OPENING_SPLASH_SCREEN,
	GAMESTATE_TITLE_SCREEN,
	GAMESTATE_OVERWORLD,
	GAMESTATE_BATTLE,
	GAMESTATE_OPTION_SCREEN,
	GAMESTATE_EXIT_YES_NO_SCREEN,
	GAMESTATE_GAMEPAD_UNPLUGGED,
	GAMESTATE_CHARACTER_NAMING
} GAME_STATE;

typedef enum RESOURCE_TYPE {
	RESOURCE_TYPE_WAV,
	RESOURCE_TYPE_OGG,
	RESOURCE_TYPE_TILEMAP,
	RESOURCE_TYPE_BMPX
} RESOURCE_TYPE;

typedef struct GAME_BITMAP {
	BITMAPINFO bitmapInfo;
	void* Memory;
} GAME_BITMAP;

typedef struct UPOINT {
	uint16_t x;
	uint16_t y;
} UPOINT;

typedef struct GAME_SOUND {
	WAVEFORMATEX WaveFormat;
	XAUDIO2_BUFFER Buffer;
} GAME_SOUND;

typedef struct PIXEL32 {
	uint8_t Blue;
	uint8_t Green;
	uint8_t Red;
	uint8_t Alpha;
} PIXEL32;

typedef struct GAME_PERFORMANCE_DATA {
	uint64_t totalFramesRendered;
	float rawAvgFPS;
	float cookedAvgFPS;

	int64_t PerfFrequency;

	MONITORINFO MonitorInfo;
	/*RECT MonitorRect;
	RECT WindowRect;
	int32_t MonitorWidth;
	int32_t MonitorHeight;

	int32_t WindowWidth;
	int32_t WindowHeight;*/

	BOOL DisplayDebugInfo;

	ULONG MinimumTimerResolution;
	ULONG MaximumTimerResolution;
	ULONG CurrentTimerResolution;

	DWORD HandleCount;
	PROCESS_MEMORY_COUNTERS_EX MemInfo;
	uint16_t CPUCount;
	double CPUPercent;
	SYSTEM_INFO SystemInfo;

	int64_t CurrentSystemTime;
	int64_t PreviousSystemTime;

	uint8_t MaxScaleFactor;
	uint8_t CurrentScaleFactor;
} GAME_PERFORMANCE_DATA;

typedef struct TILEMAP {
	uint16_t width;
	uint16_t height;
	uint8_t** map;

} TILEMAP;

typedef struct GAMEMAP {
	TILEMAP tilemap;
	GAME_BITMAP gamebitmap;
} GAMEMAP;

typedef struct HERO {
	char Name[9];
	GAME_BITMAP Sprite[3][12];
	BOOL Active;
	UPOINT ScreenPos;
	UPOINT WorldPos;
	uint8_t MovementRemaining;
	DIRECTION Direction;
	uint8_t CurrentArmor;
	uint8_t SpriteIndex;
	int32_t HP;
	int32_t Strength;
	int32_t MP;
} HERO;

typedef struct REGISTRY_PARAMS {
	DWORD LogLevel;
	DWORD SFX_Volume;
	DWORD Music_Volume;
	DWORD ScaleFactor;
} REGISTRY_PARAMS;

typedef struct PlayerInput {
	int16_t EscapeKeyDown;
	int16_t DebugKeyDown;
	int16_t LeftKeyDown;
	int16_t RightKeyDown;
	int16_t UpKeyDown;
	int16_t DownKeyDown;
	int16_t ChooseKeyDown;
	int16_t EscapeKeyWasDown;
	int16_t DebugKeyWasDown;
	int16_t LeftKeyWasDown;
	int16_t RightKeyWasDown;
	int16_t UpKeyWasDown;
	int16_t DownKeyWasDown;
	int16_t ChooseKeyWasDown;
} PlayerInput;

typedef struct MENU_ITEM {
	char* Name;
	int16_t x;
	int16_t	y;
	BOOL Enabled;
	void(*Action)(void);
} MENU_ITEM;

typedef struct MENU {
	char* Name;
	uint8_t SelectedItem;
	uint8_t NumMenuItems;
	MENU_ITEM** Items;
} MENU;

LRESULT CALLBACK MainWindProc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam);
DWORD CreateMainGameWindow(void);
BOOL AlreadyRunning(void);

DWORD InitalizeHero(void);
HRESULT InitalizeSoundEngine(void);

void ProcessPlayerInput(void);
//---
GAME_BITMAP gCanvas;
GAME_PERFORMANCE_DATA gPerformanceData;
GAME_STATE gCurrentGameState;
GAME_STATE gPreviousGameState;
PlayerInput gPlayerInput;
GAME_BITMAP g6x7Font;
GAME_SOUND gSound_SplashScreen;
GAME_SOUND gSound_MenuNavigate;
GAME_SOUND gSound_MenuChoose;
GAME_SOUND gMusic_Overworld01;
HERO gPlayer;
int8_t gGamepadID;
HWND gWHandle;
IXAudio2SourceVoice* gXAudioMusicSourceVoice;
IXAudio2SourceVoice* gXAudioSFXSourceVoice[NUMBER_SFX_SOURCE_VOICES];
typedef LONG(NTAPI* _NtQueryTimerResolution) (OUT PULONG MinimumResolution, OUT PULONG MaximumResolution, OUT PULONG CurrentResolution);
_NtQueryTimerResolution NtQueryTimerResolution;
GAMEMAP gOverworld01;
uint8_t gPassableTiles[16];
//uint8_t gPassableTiles[100];
// GAME_BITMAP gOverworld01;
UPOINT gCamera;
HANDLE gAssetLoadingThread;
HANDLE gSplashScreenLoadedEvent;
BOOL gGameRunning;

float gSFXVolume;
float gMusicVolume;

// Gamepad
void FindFirstConnectedGamepad(void);

// Load asset from file
DWORD Load32BppBitmapFromFile(_In_ char* Filename, _Inout_ GAME_BITMAP* GameBitmap);
DWORD LoadWavFromFile(_In_ char* Filename, _Inout_ GAME_SOUND* GameSound);
DWORD LoadOggFromFile(_In_ char* Filename, _Inout_ GAME_SOUND* GameSound);
DWORD LoadTilemapFromFile(_In_ char* FileName, _Inout_ TILEMAP* TileMap);
DWORD LoadAssetFromArchive(_In_ char* ArchiveName, _In_ char* Filename, _In_ RESOURCE_TYPE ResourceType, _Inout_ void* Resource);

// Load asset from memory
DWORD Load32BppBitmapFromMemory(_In_ void* Buffer, _Inout_ GAME_BITMAP* GameBitmap);
DWORD LoadWavFromMemory(_In_ void* Buffer, _Inout_ GAME_SOUND* GameSound);
DWORD LoadOggFromMemory(_In_ void* Buffer, _In_ uint32_t BufferSize, _Inout_ GAME_SOUND* GameSound);
DWORD LoadTilemapFromMemory(_In_ void* Buffer, _In_ uint32_t BufferSize, _Inout_ TILEMAP* TileMap);

// Blit to buffer
void Blit32BppBitmapToBuffer(_In_ GAME_BITMAP* GameBitmap, _In_ uint16_t x, _In_ uint16_t y);
void BlitStringToBuffer(_In_ char* String, _In_ GAME_BITMAP* FontSheet, _In_ PIXEL32* Color, _In_ uint16_t x, _In_ uint16_t y);
void BlitTilemapToBuffer(_In_ GAME_BITMAP* GameBitmap);

// Audio
void PlaySoundSfx(_In_ GAME_SOUND* GameSound);
void PlaySoundMusic(_In_ GAME_SOUND* GameSound);

// Registry
DWORD LoadRegistryParams(void);
DWORD SaveRegistryParams(void);

// Log
void LogMessageA(_In_ LOG_LEVEL LogLevel, _In_ char* Message, _In_ ...);

// Render
void RenderFrameGraphic(void);
void DrawDebugInfo(void);

// Threads
DWORD WINAPI AssetLoadingThreadProc(_In_ LPVOID lpParam);

#ifdef AVX
	void ClearScreen(_In_ __m256i* Color);
#elif defined SSE2
	void ClearScreen(_In_ __m128i* Color);
#else
	void ClearScreen(_In_ PIXEL32* Pixel);
#endif