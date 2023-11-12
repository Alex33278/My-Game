#include "Main.h"
#include "GamepadUnplugged.h"

void PPI_GamepadUnplugged(void) {
	if (gGamepadID > -1 || (gPlayerInput.EscapeKeyDown && !gPlayerInput.EscapeKeyWasDown)) {
		gCurrentGameState = gPreviousGameState;
		gPreviousGameState = GAMESTATE_GAMEPAD_UNPLUGGED;
	}
}

void DrawGamepadUnpluggedScreen(void) {

	PIXEL32 White = { 0xFF, 0xFF, 0xFF, 0xFF };

	memset(gCanvas.Memory, 0, GAME_CANVAS_MEMORY_SIZE);

	BlitStringToBuffer(GAMEPAD_UNPLUGGED_STRING_1, &g6x7Font, &White, CALCXPOS((uint8_t)strlen(GAMEPAD_UNPLUGGED_STRING_1)), 60);
	BlitStringToBuffer(GAMEPAD_UNPLUGGED_STRING_2, &g6x7Font, &White, CALCXPOS((uint8_t)strlen(GAMEPAD_UNPLUGGED_STRING_2)), 80);
}