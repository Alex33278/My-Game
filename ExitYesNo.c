#include "Main.h"
#include "ExitYesNo.h"

MENU_ITEM gMI_Exit_YesNo_Yes = { "Yes", CALCXPOS(3), 100, TRUE, MenuItem_ExitYesNo_Yes };
MENU_ITEM gMI_Exit_YesNo_No = { "No", CALCXPOS(2), 115, TRUE, MenuItem_ExitYesNo_No };
MENU_ITEM* gMI_Exit_YesNoItems[] = { &gMI_Exit_YesNo_Yes, &gMI_Exit_YesNo_No };
MENU gMenu_Exit_YesNo = { "Are you sure you want to exit?", 1, _countof(gMI_Exit_YesNoItems), gMI_Exit_YesNoItems };

void PPI_ExitYesNo(void) {
	if (gPlayerInput.DownKeyDown && !gPlayerInput.DownKeyWasDown) {
		if (gMenu_Exit_YesNo.SelectedItem < gMenu_Exit_YesNo.NumMenuItems - 1) {
			gMenu_Exit_YesNo.SelectedItem++;

			PlaySound2(&gSound_MenuNavigate);
		}
	}

	if (gPlayerInput.UpKeyDown && !gPlayerInput.UpKeyWasDown) {
		if (gMenu_Exit_YesNo.SelectedItem > 0) {
			gMenu_Exit_YesNo.SelectedItem--;
			PlaySound2(&gSound_MenuNavigate);
		}
	}

	if (gPlayerInput.ChooseKeyDown && !gPlayerInput.ChooseKeyWasDown) {
		gMenu_Exit_YesNo.Items[gMenu_Exit_YesNo.SelectedItem]->Action();
		PlaySound2(&gSound_MenuChoose);
	}
}

void DrawExitYesNoScreen(void) {
	PIXEL32 White = { 0xFF, 0xFF, 0xFF, 0xFF };

	static uint64_t LocalFrameCounter = 0;
	static uint64_t LastFrameSeen = 0;

	memset(gCanvas.Memory, 0, GAME_CANVAS_MEMORY_SIZE);

	BlitStringToBuffer(gMenu_Exit_YesNo.Name, &g6x7Font, &White, CALCXPOS((uint8_t)strlen(gMenu_Exit_YesNo.Name)), 60);

	BlitStringToBuffer(gMenu_Exit_YesNo.Items[0]->Name, &g6x7Font, &White, gMenu_Exit_YesNo.Items[0]->x, gMenu_Exit_YesNo.Items[0]->y);
	BlitStringToBuffer(gMenu_Exit_YesNo.Items[1]->Name, &g6x7Font, &White, gMenu_Exit_YesNo.Items[1]->x, gMenu_Exit_YesNo.Items[1]->y);

	BlitStringToBuffer("»", &g6x7Font, &White, gMenu_Exit_YesNo.Items[gMenu_Exit_YesNo.SelectedItem]->x - 12, gMenu_Exit_YesNo.Items[gMenu_Exit_YesNo.SelectedItem]->y);
}

void MenuItem_ExitYesNo_Yes(void) {
	SendMessageA(gWHandle, WM_CLOSE, 0, 0);
}

void MenuItem_ExitYesNo_No(void) {
	gCurrentGameState = gPreviousGameState;
	gPreviousGameState = GAMESTATE_EXIT_YES_NO_SCREEN;
}