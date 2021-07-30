#pragma once
#define GameName "Knight Training"
#define GameResWidth 384
#define GameResHeight 240
#define GameBPP 32
#define GameCanvasMemorySize (GameResWidth * GameResHeight * (GameBPP / 8))
#define CalAvgFpsEveryXFrames 100

typedef struct GameBitMap
{
	BITMAPINFO BitMapInfo;
	void* Memory;
} GameBitMap;

typedef struct Pixel32
{
	uint8_t Blue;
	uint8_t Green;
	uint8_t Red;
	uint8_t Alpha;

} Pixel32;

typedef struct PerformanceData
{
	uint64_t FramesRendered;
	uint32_t RawFramesPerSecondAverage;
	uint32_t CookedFramesPerSecondAverage;
	uint16_t CalAverageFPSEveryXFrames;

	LARGE_INTEGER PerFrequency;
	LARGE_INTEGER FrameStart;
	LARGE_INTEGER FrameEnd;
	LARGE_INTEGER ElapsedMicrosecondsPerFrame;

	MONITORINFO MonitorInfo;
	int32_t MonitorWidth;
	int32_t MonitorHeight;
} PerformanceData;

LRESULT CALLBACK MainWinProc(HWND WinHandle, UINT Msg, WPARAM WParam, LPARAM LParam);
DWORD CreateMainGameWindow(void);
BOOL GameRunning(void);
void ProccessPlayerInput(void);
void RenderGraphics(void);