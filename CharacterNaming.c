#include "Main.h"
#include "CharacterNaming.h"

MENU_ITEM gMI_CharacterNaming_A = { "A", CALCXPOS(36), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_B = { "B", CALCXPOS(30), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_C = { "C", CALCXPOS(24), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_D = { "D", CALCXPOS(18), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_E = { "E", CALCXPOS(12), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_F = { "F", CALCXPOS(6), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_G = { "G", CALCXPOS(0), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_H = { "H", CALCXPOS(-6), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_I = { "I", CALCXPOS(-12), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_J = { "J", CALCXPOS(-18), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_K = { "K", CALCXPOS(-24), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_L = { "L", CALCXPOS(-30), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_M = { "M", CALCXPOS(-36), 145, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_N = { "N", CALCXPOS(36), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_O = { "O", CALCXPOS(30), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_P = { "P", CALCXPOS(24), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_Q = { "Q", CALCXPOS(18), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_R = { "R", CALCXPOS(12), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_S = { "S", CALCXPOS(6), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_T = { "T", CALCXPOS(0), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_U = { "U", CALCXPOS(-6), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_V = { "V", CALCXPOS(-12), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_W = { "W", CALCXPOS(-18), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_X = { "X", CALCXPOS(-24), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_Y = { "Y", CALCXPOS(-30), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_Z = { "Z", CALCXPOS(-36), 160, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_a = { "a", CALCXPOS(36), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_b = { "b", CALCXPOS(30), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_c = { "c", CALCXPOS(24), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_d = { "d", CALCXPOS(18), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_e = { "e", CALCXPOS(12), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_f = { "f", CALCXPOS(6), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_g = { "g", CALCXPOS(0), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_h = { "h", CALCXPOS(-6), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_i = { "i", CALCXPOS(-12), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_j = { "j", CALCXPOS(-18), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_k = { "k", CALCXPOS(-24), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_l = { "l", CALCXPOS(-30), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_m = { "m", CALCXPOS(-36), 175, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_n = { "n", CALCXPOS(36), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_o = { "o", CALCXPOS(30), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_p = { "p", CALCXPOS(24), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_q = { "q", CALCXPOS(18), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_r = { "r", CALCXPOS(12), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_s = { "s", CALCXPOS(6), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_t = { "t", CALCXPOS(0), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_u = { "u", CALCXPOS(-6), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_v = { "v", CALCXPOS(-12), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_w = { "w", CALCXPOS(-18), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_x = { "x", CALCXPOS(-24), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_y = { "y", CALCXPOS(-30), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_z = { "z", CALCXPOS(-36), 190, TRUE, MenuItem_CharacterNaming_Add };
MENU_ITEM gMI_CharacterNaming_Back = { "Back", CALCXPOS(36), 205, TRUE, MenuItem_CharacterNaming_Back };
MENU_ITEM gMI_CharacterNaming_Ok = { "Ok", CALCXPOS(-35), 205, TRUE, MenuItem_CharacterNaming_Ok };
MENU_ITEM* gMI_CharacterNamingItems[] = { &gMI_CharacterNaming_A, &gMI_CharacterNaming_B, &gMI_CharacterNaming_C, &gMI_CharacterNaming_D, &gMI_CharacterNaming_E, &gMI_CharacterNaming_F, &gMI_CharacterNaming_G, &gMI_CharacterNaming_H, &gMI_CharacterNaming_I, &gMI_CharacterNaming_J, &gMI_CharacterNaming_K, &gMI_CharacterNaming_L, &gMI_CharacterNaming_M,
										  &gMI_CharacterNaming_N, &gMI_CharacterNaming_O, &gMI_CharacterNaming_P, &gMI_CharacterNaming_Q, &gMI_CharacterNaming_R, &gMI_CharacterNaming_S, &gMI_CharacterNaming_T, &gMI_CharacterNaming_U, &gMI_CharacterNaming_V, &gMI_CharacterNaming_W, &gMI_CharacterNaming_X, &gMI_CharacterNaming_Y, &gMI_CharacterNaming_Z,
										  &gMI_CharacterNaming_a, &gMI_CharacterNaming_b, &gMI_CharacterNaming_c, &gMI_CharacterNaming_d, &gMI_CharacterNaming_e, &gMI_CharacterNaming_f, &gMI_CharacterNaming_g, &gMI_CharacterNaming_h, &gMI_CharacterNaming_i, &gMI_CharacterNaming_j, &gMI_CharacterNaming_k, &gMI_CharacterNaming_l, &gMI_CharacterNaming_m,
										  &gMI_CharacterNaming_n, &gMI_CharacterNaming_o, &gMI_CharacterNaming_p, &gMI_CharacterNaming_q, &gMI_CharacterNaming_r, &gMI_CharacterNaming_s, &gMI_CharacterNaming_t, &gMI_CharacterNaming_u, &gMI_CharacterNaming_v, &gMI_CharacterNaming_w, &gMI_CharacterNaming_x, &gMI_CharacterNaming_y, &gMI_CharacterNaming_z,
										  &gMI_CharacterNaming_Back, &gMI_CharacterNaming_Ok };
MENU gMenu_CharacterNaming = { "Select your name", 0, _countof(gMI_CharacterNamingItems), gMI_CharacterNamingItems };

void PPI_CharacterNaming(void) {
#define BACK_BUTTON 52
#define OK_BUTTON 53

	if (gPlayerInput.UpKeyDown && !gPlayerInput.UpKeyWasDown) {
		if (gMenu_CharacterNaming.SelectedItem > 12) {
			gMenu_CharacterNaming.SelectedItem -= 13;
		}
		else {
			gMenu_CharacterNaming.SelectedItem = BACK_BUTTON;
		}

		PlaySoundSfx(&gSound_MenuNavigate);
	}

	if (gPlayerInput.DownKeyDown && !gPlayerInput.DownKeyWasDown) {
		if (gMenu_CharacterNaming.SelectedItem < 41) {
			gMenu_CharacterNaming.SelectedItem += 13;

			PlaySoundSfx(&gSound_MenuNavigate);
		}
		else {
			if (gMenu_CharacterNaming.SelectedItem >= 39) {
				gMenu_CharacterNaming.SelectedItem = BACK_BUTTON;

				PlaySoundSfx(&gSound_MenuNavigate);
			}
		}
	}

	if (gPlayerInput.LeftKeyDown && !gPlayerInput.LeftKeyWasDown) {
		if (gMenu_CharacterNaming.SelectedItem > 0 && gMenu_CharacterNaming.SelectedItem != BACK_BUTTON) {
			gMenu_CharacterNaming.SelectedItem--;

			gMenu_CharacterNaming.SelectedItem = (gMenu_CharacterNaming.SelectedItem % 13 == 12 ?
				gMenu_CharacterNaming.SelectedItem + 13 :
				gMenu_CharacterNaming.SelectedItem);

			PlaySoundSfx(&gSound_MenuNavigate);
		}
	}

	if (gPlayerInput.RightKeyDown && !gPlayerInput.RightKeyWasDown) {
		if (gMenu_CharacterNaming.SelectedItem < OK_BUTTON) {
			gMenu_CharacterNaming.SelectedItem++;

			gMenu_CharacterNaming.SelectedItem = (gMenu_CharacterNaming.SelectedItem % 13 == 0 ?
				gMenu_CharacterNaming.SelectedItem - 13 :
				gMenu_CharacterNaming.SelectedItem);

			PlaySoundSfx(&gSound_MenuNavigate);
		}
		else {
			if (gMenu_CharacterNaming.SelectedItem < OK_BUTTON) {
				gMenu_CharacterNaming.SelectedItem++;
			}
		}
	}

	if (gPlayerInput.ChooseKeyDown && !gPlayerInput.ChooseKeyWasDown) {
		gMenu_CharacterNaming.Items[gMenu_CharacterNaming.SelectedItem]->Action();
	}

	if (gPlayerInput.EscapeKeyDown && !gPlayerInput.EscapeKeyWasDown) {
		MenuItem_CharacterNaming_Back();
	}
}

void DrawCharacterNaming(void) {
	static PIXEL32 textColor = { 0x00, 0x00, 0x00, 0xFF };
	static uint64_t LocalFrameCounter = 0;
	static uint64_t LastFrameSeen = 0;

	if (gPerformanceData.totalFramesRendered > (LastFrameSeen + 1)) {
		LocalFrameCounter = 0;
		textColor.Red = 0;
		textColor.Green = 0;
		textColor.Blue = 0;
		gMenu_CharacterNaming.SelectedItem = 0;
	}

	memset(gCanvas.Memory, 0, GAME_CANVAS_MEMORY_SIZE);

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

	BlitStringToBuffer(gMenu_CharacterNaming.Name, &g6x7Font, &textColor, CALCXPOS((uint8_t)strlen(gMenu_CharacterNaming.Name)), 60);
	Blit32BppBitmapToBuffer(&gPlayer.Sprite[SUIT_0][FACING_DOWN_0], 208, 80);

	for (uint8_t Counter = 0; Counter < 8; Counter++) {
		if (gPlayer.Name[Counter] == '\0') {
			BlitStringToBuffer("_", &g6x7Font, &textColor, 150 + (Counter * 6), 80);
		}
		else {
			BlitStringToBuffer(&gPlayer.Name[Counter], &g6x7Font, &textColor, 150 + (Counter * 6), 80);
		}
	}

	for (uint8_t Counter = 0; Counter < gMenu_CharacterNaming.NumMenuItems; Counter++) {
		BlitStringToBuffer(gMenu_CharacterNaming.Items[Counter]->Name, &g6x7Font, &textColor, gMenu_CharacterNaming.Items[Counter]->x, gMenu_CharacterNaming.Items[Counter]->y);
	}

	BlitStringToBuffer("»", &g6x7Font, &textColor, gMenu_CharacterNaming.Items[gMenu_CharacterNaming.SelectedItem]->x - 6, gMenu_CharacterNaming.Items[gMenu_CharacterNaming.SelectedItem]->y);

	LocalFrameCounter++;
	LastFrameSeen = gPerformanceData.totalFramesRendered;
}

void MenuItem_CharacterNaming_Add(void) {
	if (strlen(gPlayer.Name) < 8) {
		gPlayer.Name[strlen(gPlayer.Name)] = gMenu_CharacterNaming.Items[gMenu_CharacterNaming.SelectedItem]->Name[0];
		PlaySoundSfx(&gSound_MenuChoose);
	}
}

void MenuItem_CharacterNaming_Back(void) {
	if (strlen(gPlayer.Name) < 1) {
		gPreviousGameState = gCurrentGameState;
		gCurrentGameState = GAMESTATE_TITLE_SCREEN;
	}
	else {
		gPlayer.Name[strlen(gPlayer.Name) - 1] = '\0';
	}

	PlaySoundSfx(&gSound_MenuChoose);
}

void MenuItem_CharacterNaming_Ok(void) {
	if (strlen(gPlayer.Name) > 0) {
		gPreviousGameState = gCurrentGameState;
		gCurrentGameState = GAMESTATE_OVERWORLD;
		gPlayer.Active = TRUE;
		PlaySoundSfx(&gSound_MenuChoose);
	}
}