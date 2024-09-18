#include "Main.h"
#include "Overworld.h"

void PPI_Overworld(void) {
	if (gPlayerInput.EscapeKeyDown && !gPlayerInput.EscapeKeyWasDown) {
		gCurrentGameState = GAMESTATE_EXIT_YES_NO_SCREEN;
		gPreviousGameState = GAMESTATE_OVERWORLD;
		return;
	}

	if (!gPlayer.MovementRemaining) {
		if (gPlayerInput.DownKeyDown) {
			BOOL CanMoveToDesiredTile = FALSE;
			if (gPlayer.ScreenPos.y < GAME_RES_HEIGHT - 16) {
				for (uint8_t Counter = 0; Counter < _countof(gPassableTiles); Counter++) {
					if (gOverworld01.tilemap.map[(gPlayer.WorldPos.y / 16) + 1][gPlayer.WorldPos.x / 16] == gPassableTiles[Counter]) {
						CanMoveToDesiredTile = TRUE;
						break;
					}
				}
			}

			if (CanMoveToDesiredTile) {
				gPlayer.MovementRemaining = 16;
				gPlayer.Direction = DIR_DOWN;
			}
		}
		else if (gPlayerInput.LeftKeyDown) {
			BOOL CanMoveToDesiredTile = FALSE;

			if (gPlayer.ScreenPos.x > 0) {
				for (uint8_t Counter = 0; Counter < _countof(gPassableTiles); Counter++) {
					if (gOverworld01.tilemap.map[gPlayer.WorldPos.y / 16][(gPlayer.WorldPos.x / 16) - 1] == gPassableTiles[Counter]) {
						CanMoveToDesiredTile = TRUE;
						break;
					}
				}
			}
			if (CanMoveToDesiredTile) {
				gPlayer.MovementRemaining = 16;
				gPlayer.Direction = DIR_LEFT;
			}
		}
		else if (gPlayerInput.RightKeyDown) {
			BOOL CanMoveToDesiredTile = FALSE;
			if (gPlayer.ScreenPos.x < GAME_RES_WIDTH) {
				for (uint8_t Counter = 0; Counter < _countof(gPassableTiles); Counter++) {
					if (gOverworld01.tilemap.map[gPlayer.WorldPos.y / 16][(gPlayer.WorldPos.x / 16) + 1] == gPassableTiles[Counter]) {
						CanMoveToDesiredTile = TRUE;
						break;
					}
				}
			}

			if (CanMoveToDesiredTile) {
				gPlayer.MovementRemaining = 16;
				gPlayer.Direction = DIR_RIGHT;
			}
		}
		else if (gPlayerInput.UpKeyDown) {
			BOOL CanMoveToDesiredTile = FALSE;

			if (gPlayer.ScreenPos.y > 0) {
				for (uint8_t Counter = 0; Counter < _countof(gPassableTiles); Counter++) {
					if (gOverworld01.tilemap.map[(gPlayer.WorldPos.y / 16) - 1][gPlayer.WorldPos.x / 16] == gPassableTiles[Counter]) {
						CanMoveToDesiredTile = TRUE;
						break;
					}
				}
			}

			if (CanMoveToDesiredTile) {
				gPlayer.MovementRemaining = 16;
				gPlayer.Direction = DIR_UP;
			}
		}
	}
	else {
		if (gPlayer.Direction == DIR_DOWN) {
			if (gPlayer.ScreenPos.y >= GAME_RES_HEIGHT - 64 && gCamera.y < gOverworldArea.bottom - GAME_RES_HEIGHT) { //(gOverworld01.tilemap.height * 16) - GAME_RES_HEIGHT) {
				gPlayer.ScreenPos.y--;
				gCamera.y++;
			}

			gPlayer.ScreenPos.y++;
			gPlayer.WorldPos.y++;
		}

		if (gPlayer.Direction == DIR_LEFT) {
			if (gPlayer.ScreenPos.x <= 64 && gCamera.x > gOverworldArea.left) {
				gPlayer.ScreenPos.x++;
				gCamera.x--;
			}

			gPlayer.ScreenPos.x--;
			gPlayer.WorldPos.x--;
		}

		if (gPlayer.Direction == DIR_RIGHT) {
			if (gPlayer.ScreenPos.x >= GAME_RES_WIDTH - 64 && gCamera.x < gOverworldArea.right - GAME_RES_WIDTH) {// (gOverworld01.tilemap.width * 16) - GAME_RES_WIDTH) {
				gPlayer.ScreenPos.x--;
				gCamera.x++;
			}

			gPlayer.ScreenPos.x++;
			gPlayer.WorldPos.x++;
		}

		if (gPlayer.Direction == DIR_UP) {
			if (gPlayer.ScreenPos.y <= 64 && gCamera.y > gOverworldArea.top) {
				gPlayer.ScreenPos.y++;
				gCamera.y--;
			}

			gPlayer.ScreenPos.y--;
			gPlayer.WorldPos.y--;
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

	if (LocalFrameCounter == 0) {
		if (gPreviousGameState != GAMESTATE_EXIT_YES_NO_SCREEN) {
			PlaySoundMusic(&gMusic_Overworld01);
		}
	}

	//memset(gCanvas.Memory, 0, GAME_CANVAS_MEMORY_SIZE);
	BlitTilemapToBuffer(&gOverworld01.gamebitmap);
	Blit32BppBitmapToBuffer(&gPlayer.Sprite[gPlayer.CurrentArmor][gPlayer.SpriteIndex + gPlayer.Direction], gPlayer.ScreenPos.x, gPlayer.ScreenPos.y);

	if (gPerformanceData.DisplayDebugInfo) {
		char Buffer[4] = { 0 };

		for (int8_t x = -1; x <= 1; x++) {
			for (int8_t y = -1; y <= 1; y++) {
				if (gPlayer.ScreenPos.x > 16 && gPlayer.ScreenPos.y > 16) {
					_itoa_s(gOverworld01.tilemap.map[(gPlayer.WorldPos.y / 16) + y][(gPlayer.WorldPos.x / 16) + x], Buffer, sizeof(Buffer), 10);
					BlitStringToBuffer(&Buffer, &g6x7Font, &(PIXEL32) {0xFF, 0xFF, 0xFF, 0xFF}, gPlayer.ScreenPos.x + 5 + (x * 16), gPlayer.ScreenPos.y + 4 + (y * 16));
				}
			}
		}
	}

	LocalFrameCounter++;
	LastFrameSeen = gPerformanceData.totalFramesRendered;
}