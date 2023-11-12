#include "Main.h"
#include "OpeningSplash.h"

void PPI_OpeningSplashScreen(void) {
	if (gPlayerInput.EscapeKeyDown && !gPlayerInput.EscapeKeyWasDown) {
		gPreviousGameState = gCurrentGameState;
		gCurrentGameState = GAMESTATE_TITLE_SCREEN;
	}
}

void DrawOpeningSplashScreen(void) {
	static uint64_t localFrameCounter;
	static uint64_t lastFrameSeen = 0;
	static uint8_t brightnessModifier = 0;
	PIXEL32 textColor = { 0xFF, 0xFF, 0xFF, 0xFF };

	if (gPerformanceData.totalFramesRendered > (lastFrameSeen + 1)) {
		localFrameCounter = 0;
	}

	memset(gCanvas.Memory, 0, GAME_CANVAS_MEMORY_SIZE);

	if (localFrameCounter >= 60) {
		if (localFrameCounter == 60) {
			PlaySound2(&gSound_SplashScreen);
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
		else if (localFrameCounter == 300) {
			brightnessModifier = 64;
		}
		else if (localFrameCounter == 315) {
			brightnessModifier = 128;
		}
		else if (localFrameCounter == 330) {
			brightnessModifier = 196;
		}
		else if (localFrameCounter == 345) {
			brightnessModifier = 255;
		}

		textColor.Red -= brightnessModifier;
		textColor.Green -= brightnessModifier;
		textColor.Blue -= brightnessModifier;

		if (localFrameCounter >= 225) {
			if (localFrameCounter == 225) {
				PlaySound2(&gSound_MenuNavigate);
			}

			BlitStringToBuffer(GAME_NAME, &g6x7Font, &textColor, CALCXPOS((uint8_t)strlen(GAME_NAME)), (GAME_RES_HEIGHT / 2) - 3);

			if (localFrameCounter >= 405) {
				gPreviousGameState = gCurrentGameState;
				gCurrentGameState = GAMESTATE_TITLE_SCREEN;
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