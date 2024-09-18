#include "Main.h"
#include "TitleScreen.h"

MENU_ITEM gMI_ResumeGame = { "Resume", CALCXPOS(6), 100, FALSE, MenuItem_TitleScreen_Resume };
MENU_ITEM gMI_StartNewGame = { "New Game", CALCXPOS(8), 115, TRUE, MenuItem_TitleScreen_StartNewGame };
MENU_ITEM gMI_Options = { "Options", CALCXPOS(7), 130, TRUE, MenuItem_TitleScreen_Options };
MENU_ITEM gMI_Exit = { "Exit", CALCXPOS(4), 145, TRUE, MenuItem_TitleScreen_Exit };
MENU_ITEM* gMI_TitleScreenItems[] = { &gMI_ResumeGame, &gMI_StartNewGame, &gMI_Options, &gMI_Exit };
MENU gMenu_TitleScreen = { "Title Screen Menu", 1, _countof(gMI_TitleScreenItems), gMI_TitleScreenItems };

void PPI_TitleScreen(void) {
	if (gPlayerInput.DownKeyDown && !gPlayerInput.DownKeyWasDown) {
		if (gMenu_TitleScreen.SelectedItem < gMenu_TitleScreen.NumMenuItems - 1) {
			gMenu_TitleScreen.SelectedItem++;

			PlaySoundSfx(&gSound_MenuNavigate);
		}
	}

	if (gPlayerInput.UpKeyDown && !gPlayerInput.UpKeyWasDown) {
		if (gMenu_TitleScreen.SelectedItem > (gPlayer.Active == TRUE ? 0 : 1)) {
			gMenu_TitleScreen.SelectedItem--;
			PlaySoundSfx(&gSound_MenuNavigate);
		}
	}

	if (gPlayerInput.ChooseKeyDown && !gPlayerInput.ChooseKeyWasDown) {
		gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->Action();
		PlaySoundSfx(&gSound_MenuChoose);
	}
}

void DrawTitleScreen(void) {
	static PIXEL32 textColor = { 0x00, 0x00, 0x00, 0xFF };
	static uint64_t LocalFrameCounter = 0;
	static uint64_t LastFrameSeen = 0;

	if (gPerformanceData.totalFramesRendered > (LastFrameSeen + 1)) {
		LocalFrameCounter = 0;
		textColor.Red = 0;
		textColor.Green = 0;
		textColor.Blue = 0;
		gMenu_TitleScreen.SelectedItem = (gPlayer.Active ? 0 : 1);
	}

	memset(gCanvas.Memory, 0, GAME_CANVAS_MEMORY_SIZE);
	//ClearScreen(&(__m256) { 0 });
	//__stosd(gCanvas.Memory, 0xFF000000, GAME_CANVAS_MEMORY_SIZE / sizeof(PIXEL32));

	if (LocalFrameCounter <= 60 && LocalFrameCounter != 0 && LocalFrameCounter % 15 == 0) {
		textColor.Red += 64;
		textColor.Green += 64;
		textColor.Blue += 64;

		if (LocalFrameCounter == 60) {
			textColor.Red = 0xFF;
			textColor.Green = 0xFF;
			textColor.Blue = 0xFF;
		}
	}

	BlitStringToBuffer(GAME_NAME " - " GAME_VER, &g6x7Font, &textColor, CALCXPOS((uint8_t)strlen(GAME_NAME " - " GAME_VER)), 60);

	for (uint8_t MenuItem = 0; MenuItem < gMenu_TitleScreen.NumMenuItems; MenuItem++) {
		if (gMenu_TitleScreen.Items[MenuItem]->Enabled == TRUE) {
			BlitStringToBuffer(gMenu_TitleScreen.Items[MenuItem]->Name, &g6x7Font, &textColor, gMenu_TitleScreen.Items[MenuItem]->x, gMenu_TitleScreen.Items[MenuItem]->y);
		}
	}

	BlitStringToBuffer("»", &g6x7Font, &textColor, gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->x - 12, gMenu_TitleScreen.Items[gMenu_TitleScreen.SelectedItem]->y);

	LocalFrameCounter++;
	LastFrameSeen = gPerformanceData.totalFramesRendered;
}

void MenuItem_TitleScreen_Resume(void) {

}

void MenuItem_TitleScreen_StartNewGame(void) {
	gPreviousGameState = gCurrentGameState;
	gCurrentGameState = GAMESTATE_CHARACTER_NAMING;
}

void MenuItem_TitleScreen_Options(void) {
	gPreviousGameState = gCurrentGameState;
	gCurrentGameState = GAMESTATE_OPTION_SCREEN;
}

void MenuItem_TitleScreen_Exit(void) {
	gPreviousGameState = gCurrentGameState;
	gCurrentGameState = GAMESTATE_EXIT_YES_NO_SCREEN;
}