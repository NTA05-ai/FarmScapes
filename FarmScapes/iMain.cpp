#define _CRT_SECURE_NO_WARNINGS

#include "iGraphics.h"
#include "bitmap_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define STATE_MENU 0
#define STATE_LEVEL_1 1
#define STATE_SETTINGS 2
#define STATE_CREDITS 3

int gameState = STATE_MENU;
int musicOn = 1;

#include "toggleMusic.h"
#include "settings.h"
#include "credits.h"
#include "updatecropgrowth.h"
#include "drawlevel1.h"

// Player Earnings & Metrics
int playerGold = 100;
int riceGold = 0;
int tomatoGold = 0;
int berryGold = 0;

int selectedTool = 0;
int batchTimer = 0;
int batchActive = 0;

int tomatoUnlocked = 0;
int berryUnlocked = 0;
int hasRottenCrop = 0;

Tile farmGrid[GRID_ROWS][GRID_COLS];

void drawMenu() {
	iSetColor(255, 255, 255);
	iShowBMPAlternative(0, 0, "assets/menu_bg.bmp");

	iSetColor(20, 20, 20);
	iText(312, 538, "FARMSCAPES", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(255, 215, 0);
	iText(310, 540, "FARMSCAPES", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	iShowBMPAlternative2(300, 420, "assets/play.bmp", 0xFFFFFF);
	iShowBMPAlternative2(300, 330, "assets/settings.bmp", 0xFFFFFF);
	iShowBMPAlternative2(300, 240, "assets/credits.bmp", 0xFFFFFF);
	iShowBMPAlternative2(300, 150, "assets/exit.bmp", 0xFFFFFF);
}

void iDraw() {
	iClear();

	if (gameState == STATE_MENU) drawMenu();
	else if (gameState == STATE_LEVEL_1) drawLevel1();
	else if (gameState == STATE_SETTINGS) drawSettings();
	else if (gameState == STATE_CREDITS) drawCredits();
}

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		if (gameState == STATE_MENU) {
			if (mx >= 290 && mx <= 510 && my >= 410 && my <= 480) gameState = STATE_LEVEL_1;
			else if (mx >= 290 && mx <= 510 && my >= 320 && my <= 390) gameState = STATE_SETTINGS;
			else if (mx >= 290 && mx <= 510 && my >= 230 && my <= 300) gameState = STATE_CREDITS;
			else if (mx >= 290 && mx <= 510 && my >= 140 && my <= 210) {
				mciSendString("close bgmusic", NULL, 0, NULL);
				exit(0);
			}
		}
		else if (gameState == STATE_SETTINGS) {
			if (mx >= 290 && mx <= 510 && my >= 340 && my <= 410) {
				musicOn = !musicOn;
				if (musicOn) mciSendString("play bgmusic repeat", NULL, 0, NULL);
				else mciSendString("pause bgmusic", NULL, 0, NULL);
			}
			else if (mx >= 290 && mx <= 510 && my >= 220 && my <= 290) gameState = STATE_MENU;
		}
		else if (gameState == STATE_CREDITS) {
			if (mx >= 290 && mx <= 510 && my >= 140 && my <= 210) gameState = STATE_MENU;
		}
		else if (gameState == STATE_LEVEL_1) {
			if (mx >= 670 && mx <= 790 && my >= 550 && my <= 600) {
				gameState = STATE_MENU;
				return;
			}

			// Tool selections
			if (my >= 20 && my <= 80) {
				if (mx >= 160 && mx <= 270) selectedTool = 1; // PLOW
				if (mx >= 280 && mx <= 390) selectedTool = 2; // PLANT
				if (mx >= 400 && mx <= 510) selectedTool = 3; // WATER
				if (mx >= 520 && mx <= 640) selectedTool = 4; // HARVEST
			}

			// Tile Interactions
			for (int r = 0; r < GRID_ROWS; r++) {
				for (int c = 0; c < GRID_COLS; c++) {
					Tile *t = &farmGrid[r][c];

					if (mx >= t->x && mx <= t->x + 80 && my >= t->y && my <= t->y + 80) {

						// 1. PLOW
						if (selectedTool == 1) {
							if (t->state == CROP_EMPTY || t->state == CROP_ROTTEN) {
								t->state = CROP_PLOWED;
								t->growTimer = 0;

								// Check if ALL 9 tiles are now plowed
								int allPlowed = 1;
								for (int r2 = 0; r2 < GRID_ROWS; r2++) {
									for (int c2 = 0; c2 < GRID_COLS; c2++) {
										if (farmGrid[r2][c2].state != CROP_PLOWED) {
											allPlowed = 0;
											break;
										}
									}
								}

								// Timer triggers ONLY when all tiles are plowed
								if (allPlowed && !batchActive) {
									batchActive = 1;
									batchTimer = 20;
								}
							}
						}
						// 2. PLANT
						else if (selectedTool == 2 && t->state == CROP_PLOWED) {
							if (playerGold >= 5) {
								playerGold -= 5;

								if (berryUnlocked) {
									t->state = BERRY_TREE;
								}
								else {
									t->state = CROP_SEEDED;
								}
							}
						}
						// 3. WATER
						else if (selectedTool == 3) {
							if (t->state == CROP_SEEDED || t->state == BERRY_TREE) {
								t->state = CROP_WATERED;
								t->growTimer = 0;
							}
						}
						// 4. HARVEST
						else if (selectedTool == 4) {
							if (t->state == CROP_READY) {
								t->state = CROP_EMPTY;
								playerGold += 15;
								riceGold += 15;
							}
							else if (t->state == TOMATO_READY) {
								t->state = CROP_EMPTY;
								playerGold += 45;
								tomatoGold += 45;
							}
							else if (t->state == BERRY_READY) {
								t->state = CROP_EMPTY;
								playerGold += 60;
								berryGold += 60;
							}

							// Check remaining crops
							int activeCrops = 0;
							for (int r2 = 0; r2 < GRID_ROWS; r2++) {
								for (int c2 = 0; c2 < GRID_COLS; c2++) {
									int s = farmGrid[r2][c2].state;
									if (s != CROP_EMPTY) {
										activeCrops++;
									}
								}
							}

							// Progression trigger on field clear
							if (activeCrops == 0) {
								batchActive = 0;

								if (!hasRottenCrop) {
									if (tomatoUnlocked && !berryUnlocked) {
										berryUnlocked = 1;
									}
									else if (!tomatoUnlocked) {
										tomatoUnlocked = 1;
									}
								}
								hasRottenCrop = 0;
							}
						}
					}
				}
			}
		}
	}
}

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}
void iKeyboard(unsigned char key) {}
void iSpecialKeyboard(unsigned char key) {}
void fixedUpdate() {}

int main() {
	initFarmGrid();
	initAudio();

	iSetTimer(1000, updateCropGrowth);

	iInitialize(800, 600, "FarmScapes - 2D Farming Simulator");
	iStart();
	return 0;
}
