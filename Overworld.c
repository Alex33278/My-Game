#include "Main.h"
#include "Overworld.h"

void PPI_Overworld(void) {
	if (!gPlayer.MovementRemaining) {
		if (gPlayerInput.DownKeyDown) {
			if (gPlayer.ScreenPos.y < GAME_RES_HEIGHT - 16) {
				gPlayer.MovementRemaining = 16;
				gPlayer.Direction = DIR_DOWN;
			}
		}
		else if (gPlayerInput.LeftKeyDown) {
			if (gPlayer.ScreenPos.x > 0) {
				gPlayer.MovementRemaining = 16;
				gPlayer.Direction = DIR_LEFT;
			}
		}
		else if (gPlayerInput.RightKeyDown) {
			if (gPlayer.ScreenPos.x < GAME_RES_WIDTH - 16) {
				gPlayer.MovementRemaining = 16;
				gPlayer.Direction = DIR_RIGHT;
			}
		}
		else if (gPlayerInput.UpKeyDown) {
			if (gPlayer.ScreenPos.y > 0) {
				gPlayer.MovementRemaining = 16;
				gPlayer.Direction = DIR_UP;
			}
		}
	}
	else {
		if (gPlayer.Direction == DIR_DOWN) {
			gPlayer.ScreenPos.y++;
		}

		if (gPlayer.Direction == DIR_LEFT) {
			gPlayer.ScreenPos.x--;
		}

		if (gPlayer.Direction == DIR_RIGHT) {
			gPlayer.ScreenPos.x++;
		}

		if (gPlayer.Direction == DIR_UP) {
			gPlayer.ScreenPos.y--;
		}

		switch (gPlayer.MovementRemaining) {
		case 16:
			gPlayer.SpriteIndex = 1;
			break;
		case 12:
			gPlayer.SpriteIndex = 0;
			break;
		case 8:
			gPlayer.SpriteIndex = 2;
			break;
		case 4:
			gPlayer.SpriteIndex = 0;
			break;
		default: {}
		}

		gPlayer.MovementRemaining--;
	}
}

void DrawOverworld(void) {
	static uint64_t LocalFrameCounter;
	static uint64_t LastFrameSeen;
	static PIXEL32 TextColor;

	if (gPerformanceData.totalFramesRendered > (LastFrameSeen + 1)) {
		LocalFrameCounter = 0;
		memset(&TextColor, 0, sizeof(PIXEL32));
	}

	//memset(gCanvas.Memory, 0, GAME_CANVAS_MEMORY_SIZE);
	BlitTilemapToBuffer(&gOverworld01.gamebitmap);
	Blit32BppBitmapToBuffer(&gPlayer.Sprite[gPlayer.CurrentArmor][gPlayer.SpriteIndex + gPlayer.Direction], gPlayer.ScreenPos.x, gPlayer.ScreenPos.y);

	LocalFrameCounter++;
	LastFrameSeen = gPerformanceData.totalFramesRendered;
}