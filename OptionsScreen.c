#include "Main.h"
#include "OptionsScreen.h"

MENU_ITEM gMI_OptionsScreen_SFX_Volume = { "SFX Volume:", CALCXPOS(11), 100, TRUE, MenuItem_OptionsScreen_SFX_Volume };
MENU_ITEM gMI_OptionsScreen_Music_Volume = { "Music Volume:", CALCXPOS(13), 115, TRUE, MenuItem_OptionsScreen_Music_Volume };
MENU_ITEM gMI_OptionsScreen_ScreenSize = { "Screen Size:", CALCXPOS(12), 130, TRUE, MenuItem_OptionsScreen_Screen_Size };
MENU_ITEM gMI_OptionsScreen_Back = { "Back", CALCXPOS(4), 145, TRUE, MenuItem_OptionsScreen_Back };
MENU_ITEM* gMI_OptionsScreenItems[] = { &gMI_OptionsScreen_SFX_Volume, &gMI_OptionsScreen_Music_Volume, &gMI_OptionsScreen_ScreenSize, &gMI_OptionsScreen_Back };
MENU gMenu_OptionsScreen = { "Options", 0, _countof(gMI_OptionsScreenItems), gMI_OptionsScreenItems };

void PPI_OptionsScreen(void) {
	if (gPlayerInput.DownKeyDown && !gPlayerInput.DownKeyWasDown) {
		if (gMenu_OptionsScreen.SelectedItem < gMenu_OptionsScreen.NumMenuItems - 1) {
			gMenu_OptionsScreen.SelectedItem++;

			PlaySoundSfx(&gSound_MenuNavigate);
		}
	}

	if (gPlayerInput.UpKeyDown && !gPlayerInput.UpKeyWasDown) {
		if (gMenu_OptionsScreen.SelectedItem > 0) {
			gMenu_OptionsScreen.SelectedItem--;
			PlaySoundSfx(&gSound_MenuNavigate);
		}
	}

	if (gPlayerInput.ChooseKeyDown && !gPlayerInput.ChooseKeyWasDown) {
		gMenu_OptionsScreen.Items[gMenu_OptionsScreen.SelectedItem]->Action();
		PlaySoundSfx(&gSound_MenuChoose);
	}
}

void DrawOptionsScreen(void) {
	PIXEL32 Gray = { 0x7F, 0x7F, 0x7F, 0xFF };
	static PIXEL32 textColor = { 0x00, 0x00, 0x00, 0xFF };

	static uint64_t LocalFrameCounter = 0;
	static uint64_t LastFrameSeen = 0;

	char ScreenSizeString[64] = { 0 };

	memset(gCanvas.Memory, 0, GAME_CANVAS_MEMORY_SIZE);

	if (gPerformanceData.totalFramesRendered > (LastFrameSeen + 1)) {
		LocalFrameCounter = 0;
		textColor.Red = 0x00;
		textColor.Green = 0x00;
		textColor.Blue = 0x00;
		gMenu_OptionsScreen.SelectedItem = 0;
	}

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

	BlitStringToBuffer("Options", &g6x7Font, &textColor, CALCXPOS(7), 60);

	for (uint8_t MenuItem = 0; MenuItem < gMenu_OptionsScreen.NumMenuItems; MenuItem++) {
		if (gMenu_OptionsScreen.Items[MenuItem]->Enabled == TRUE) {
			BlitStringToBuffer(gMenu_OptionsScreen.Items[MenuItem]->Name, &g6x7Font, &textColor, gMenu_OptionsScreen.Items[MenuItem]->x, gMenu_OptionsScreen.Items[MenuItem]->y);
		}
	}

	for (uint8_t Volume = 0; Volume < 10; Volume++) {
		if (Volume >= (uint8_t)(gSFXVolume * 10)) {
			if (textColor.Red == 255) {
				BlitStringToBuffer("\xf2", &g6x7Font, &Gray, 240 + (Volume * 6), 100);
			}
		}
		else {
			BlitStringToBuffer("\xf2", &g6x7Font, &textColor, 240 + (Volume * 6), 100);
		}
	}

	for (uint8_t Volume = 0; Volume < 10; Volume++) {
		if (Volume >= (uint8_t)(gMusicVolume * 10)) {
			if (textColor.Red == 255) {
				BlitStringToBuffer("\xf2", &g6x7Font, &Gray, 240 + (Volume * 6), 115);
			}
		}
		else {
			BlitStringToBuffer("\xf2", &g6x7Font, &textColor, 240 + (Volume * 6), 115);
		}
	}

	snprintf(ScreenSizeString, sizeof(ScreenSizeString), "%dx%d", GAME_RES_WIDTH * gPerformanceData.CurrentScaleFactor, GAME_RES_HEIGHT * gPerformanceData.CurrentScaleFactor);
	BlitStringToBuffer(ScreenSizeString, &g6x7Font, &textColor, 240, 130);
	BlitStringToBuffer("»", &g6x7Font, &textColor, gMenu_OptionsScreen.Items[gMenu_OptionsScreen.SelectedItem]->x - 12, gMenu_OptionsScreen.Items[gMenu_OptionsScreen.SelectedItem]->y);

	LocalFrameCounter++;
	LastFrameSeen = gPerformanceData.totalFramesRendered;
}

void MenuItem_OptionsScreen_SFX_Volume(void) {
	gSFXVolume += 0.1f;

	if ((uint8_t)(gSFXVolume * 10) > 10)
		gSFXVolume = 0.0f;

	for (uint8_t counter = 0; counter < NUMBER_SFX_SOURCE_VOICES; counter++) {
		gXAudioSFXSourceVoice[counter]->lpVtbl->SetVolume(gXAudioSFXSourceVoice[counter], gSFXVolume, XAUDIO2_COMMIT_NOW);
	}
}

void MenuItem_OptionsScreen_Music_Volume(void) {
	gMusicVolume += 0.1f;

	if ((uint8_t)(gMusicVolume * 10) > 10)
		gMusicVolume = 0.0f;

	gXAudioMusicSourceVoice->lpVtbl->SetVolume(gXAudioMusicSourceVoice, gMusicVolume, XAUDIO2_COMMIT_NOW);

}

void MenuItem_OptionsScreen_Screen_Size(void) {
	if (gPerformanceData.CurrentScaleFactor < gPerformanceData.MaxScaleFactor) {
		gPerformanceData.CurrentScaleFactor++;
	}
	else {
		gPerformanceData.CurrentScaleFactor = 1;
	}

	InvalidateRect(gWHandle, NULL, TRUE);
}

void MenuItem_OptionsScreen_Back(void) {
	gCurrentGameState = gPreviousGameState;
	gPreviousGameState = GAMESTATE_OPTION_SCREEN;

	if (SaveRegistryParams() != ERROR_SUCCESS) {
		LogMessageA(LL_ERROR, "[%s] Options failed to save", __FUNCTION__);
	}
}