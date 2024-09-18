#include "Main.h"
#include "OpeningSplash.h"

void PPI_OpeningSplashScreen(void) {
	if (WaitForSingleObject(gSplashScreenLoadedEvent, 0) == WAIT_OBJECT_0 && WaitForSingleObject(gAssetLoadingThread, 0) == WAIT_OBJECT_0) {
		if (gPlayerInput.EscapeKeyDown && !gPlayerInput.EscapeKeyWasDown) {
			gPreviousGameState = gCurrentGameState;
			gCurrentGameState = GAMESTATE_TITLE_SCREEN;
		}
	}
}

void DrawOpeningSplashScreen(void) {
	static uint64_t localFrameCounter;
	static uint64_t lastFrameSeen = 0;
	static uint8_t brightnessModifier = 0;
	static uint64_t temp = 0;
	static BOOL blink = TRUE;
	PIXEL32 textColor = { 0xFF, 0xFF, 0xFF, 0xFF };
	//PIXEL32 loadTextColor = { 0x00, 0x00, 0x00, 0x00 };

	if (WaitForSingleObject(gSplashScreenLoadedEvent, 0) != WAIT_OBJECT_0) {
		return;
	}

	if (gPerformanceData.totalFramesRendered > (lastFrameSeen + 1)) {
		localFrameCounter = 0;
	}

	memset(gCanvas.Memory, 0, GAME_CANVAS_MEMORY_SIZE);

	if (localFrameCounter >= 60) {
		if (localFrameCounter == 60) {
			PlaySoundSfx(&gSound_SplashScreen);
		}
		if (localFrameCounter == 120) {
			brightnessModifier = 64;
		}
		else if (localFrameCounter == 135) {
			brightnessModifier = 128;
		}
		else if (localFrameCounter == 150) {
			brightnessModifier = 192;
		}
		else if (localFrameCounter == 165) {
			brightnessModifier = 255;
		}
		else if (localFrameCounter == 225) {
			brightnessModifier = 0;
		}

		textColor.Red -= brightnessModifier;
		textColor.Green -= brightnessModifier;
		textColor.Blue -= brightnessModifier;

		if (localFrameCounter >= 225) {
			if (localFrameCounter == 225) {
				PlaySoundSfx(&gSound_MenuNavigate);
			}

			BlitStringToBuffer(GAME_NAME, &g6x7Font, &textColor, CALCXPOS((uint8_t)strlen(GAME_NAME)), (GAME_RES_HEIGHT / 2) - 3);

			if (localFrameCounter >= 405) {
				if (WaitForSingleObject(gAssetLoadingThread, 0) == WAIT_OBJECT_0) {
					DWORD ThreadExitCode = ERROR_SUCCESS;
					GetExitCodeThread(&gAssetLoadingThread, &ThreadExitCode);

					if (ThreadExitCode != ERROR_SUCCESS) {
						LogMessageA(LL_ERROR, "[%s] Loading failed. 0x%08lx", __FUNCTION__, ThreadExitCode);
						gGameRunning = FALSE;
						MessageBoxA(NULL, "Loading failed. Check log for details.", "Error", MB_OK | MB_ICONERROR);
					}

					if (temp == 0) {
						temp = localFrameCounter;
					}

					if (localFrameCounter == temp) {
						brightnessModifier = 64;
					}
					else if (localFrameCounter == temp + 15) {
						brightnessModifier = 128;
					}
					else if (localFrameCounter == temp + 30) {
						brightnessModifier = 192;
					}
					else if (localFrameCounter == temp + 45) {
						brightnessModifier = 255;
					}
					else if (localFrameCounter >= temp + 105) {
						gPreviousGameState = gCurrentGameState;
						gCurrentGameState = GAMESTATE_TITLE_SCREEN;
					}
				}
				else {
					LogMessageA(LL_WARNING, "[%s] Loading is taking longer than expected", __FUNCTION__);
					BlitStringToBuffer("Loading...", &g6x7Font, &textColor, CALCXPOS(10), 220);
					if (gPerformanceData.totalFramesRendered % 60 == 0) {
						blink = !blink;
					}
					//if ((localFrameCounter / 60) % 2 == 0) {
					//	loadTextColor = (PIXEL32){ 0x00, 0x00, 0x00, 0xFF };
					//}
					//else {
					//	loadTextColor = (PIXEL32){ 0xFF, 0xFF, 0xFF, 0xFF };
					//}

					if (blink) {
						BlitStringToBuffer("\xf2", &g6x7Font, &(PIXEL32) {0xFF, 0xFF, 0xFF, 0xFF}, CALCXPOS(-10), 220);
					}
				}
			}
		}
		else {
			BlitStringToBuffer("- " GAME_STUDIO " -", &g6x7Font, &textColor, CALCXPOS((uint8_t)strlen("- " GAME_STUDIO " -")), (GAME_RES_HEIGHT / 2) - 3);
			BlitStringToBuffer("presents", &g6x7Font, &textColor, CALCXPOS(8), (GAME_RES_HEIGHT / 2) + 9);
		}
	}

	localFrameCounter++;
	lastFrameSeen = gPerformanceData.totalFramesRendered;
}