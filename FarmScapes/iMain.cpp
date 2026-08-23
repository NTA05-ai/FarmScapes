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
#define STATE_LOADING 4

int gameState = STATE_MENU;
int musicOn = 1;

#include "toggleMusic.h"
#include "menu.h"
#include "settings.h"
#include "credits.h"
#include "loading.h"
#include "updatecropgrowth.h"
#include "drawlevel1.h"

int playerGold = 0;

// CROP PRICES (MAX PROFIT = $5 EACH)
int riceBuyPrice = 5, riceSellPrice = 10;
int tomatoBuyPrice = 15, tomatoSellPrice = 20;
int berryBuyPrice = 25, berrySellPrice = 30;

// STARTING SEEDS (Only Rice)
int seedRice = 9;
int seedTomato = 0;
int seedBerry = 0;

// INVENTORY
int cropRiceCount = 0;
int cropTomatoCount = 0;
int cropBerryCount = 0;

int isMarketOpen = 0;
int massPlowUnlocked = 0;
int showCapWarning = 0;

int selectedTool = 0;
int batchTimer = 0;
int batchActive = 0;
int hasRottenCrop = 0;

Tile farmGrid[GRID_ROWS][GRID_COLS];

void iDraw() {
	iClear();

	if (gameState == STATE_MENU) drawMenu();
	else if (gameState == STATE_LOADING) drawLoading();
	else if (gameState == STATE_LEVEL_1) drawLevel1();
	else if (gameState == STATE_SETTINGS) drawSettings();
	else if (gameState == STATE_CREDITS) drawCredits();
}

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		if (gameState == STATE_MENU) {
			if (mx >= 290 && mx <= 510 && my >= 410 && my <= 480) gameState = STATE_LOADING;
			else if (mx >= 290 && mx <= 510 && my >= 320 && my <= 390) gameState = STATE_SETTINGS;
			else if (mx >= 290 && mx <= 510 && my >= 230 && my <= 300) gameState = STATE_CREDITS;
			else if (mx >= 290 && mx <= 510 && my >= 140 && my <= 210) {
				mciSendString("close bgmusic", NULL, 0, NULL);
				exit(0);
			}
		}
		else if (gameState == STATE_LOADING) {
			gameState = STATE_LEVEL_1;
		}
		else if (gameState == STATE_SETTINGS) {
			if (mx >= 290 && mx <= 510 && my >= 340 && my <= 410) {
				toggleMusic();
			}
			else if (mx >= 290 && mx <= 510 && my >= 220 && my <= 290) gameState = STATE_MENU;
		}
		else if (gameState == STATE_CREDITS) {
			if (mx >= 290 && mx <= 510 && my >= 140 && my <= 210) gameState = STATE_MENU;
		}
		else if (gameState == STATE_LEVEL_1) {
			if (showCapWarning) {
				showCapWarning = 0;
			}

			// Menu button
			if (mx >= 670 && mx <= 790 && my >= 550 && my <= 600) {
				gameState = STATE_MENU;
				return;
			}

			// Toggle Market
			if (mx >= 540 && mx <= 650 && my >= 552 && my <= 586) {
				isMarketOpen = !isMarketOpen;
				return;
			}

			// Marketplace Overlay Interactions
			if (isMarketOpen) {
				// Close Market Button
				if (mx >= 600 && mx <= 680 && my >= 80 && my <= 110) {
					isMarketOpen = 0;
					return;
				}

				// Sell Crops
				if (mx >= 330 && mx <= 395 && my >= 370 && my <= 392 && cropRiceCount > 0) {
					cropRiceCount--; playerGold += riceSellPrice;
				}
				else if (mx >= 330 && mx <= 395 && my >= 330 && my <= 352 && cropTomatoCount > 0) {
					cropTomatoCount--; playerGold += tomatoSellPrice;
				}
				else if (mx >= 330 && mx <= 395 && my >= 290 && my <= 312 && cropBerryCount > 0) {
					cropBerryCount--; playerGold += berrySellPrice;
				}

				// Buy Seeds
				if (mx >= 600 && mx <= 665 && my >= 370 && my <= 392 && playerGold >= riceBuyPrice) {
					playerGold -= riceBuyPrice; seedRice++;
				}
				else if (mx >= 600 && mx <= 665 && my >= 330 && my <= 352 && playerGold >= tomatoBuyPrice) {
					playerGold -= tomatoBuyPrice; seedTomato++;
				}
				else if (mx >= 600 && mx <= 665 && my >= 290 && my <= 312 && playerGold >= berryBuyPrice) {
					playerGold -= berryBuyPrice; seedBerry++;
				}

				// Mass-Plow Upgrade ($1500 Gold Check)
				if (!massPlowUnlocked && mx >= 380 && mx <= 510 && my >= 188 && my <= 214) {
					if (playerGold >= 1500) {
						playerGold -= 1500;
						massPlowUnlocked = 1;
					}
				}
				return;
			}

			// Mass-Plow Button
			if (massPlowUnlocked && mx >= 90 && mx <= 160 && my >= 28 && my <= 72) {
				for (int r = 0; r < GRID_ROWS; r++) {
					for (int c = 0; c < GRID_COLS; c++) {
						if (farmGrid[r][c].state == CROP_EMPTY || farmGrid[r][c].state == CROP_ROTTEN) {
							farmGrid[r][c].state = CROP_PLOWED;
							farmGrid[r][c].growTimer = 0;
						}
					}
				}
				return;
			}

			// Toolbar Selection
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
							}
						}
						// 2. PLANT
						else if (selectedTool == 2 && t->state == CROP_PLOWED) {
							if (seedBerry > 0) {
								seedBerry--;
								t->cropType = 2;
								t->state = BERRY_TREE;
								t->growTimer = 0;
								if (!batchActive) {
									batchActive = 1;
									batchTimer = 20;
								}
							}
							else if (seedTomato > 0) {
								seedTomato--;
								t->cropType = 1;
								t->state = CROP_SEEDED;
								t->growTimer = 0;
								if (!batchActive) {
									batchActive = 1;
									batchTimer = 20;
								}
							}
							else if (seedRice > 0) {
								seedRice--;
								t->cropType = 0;
								t->state = CROP_SEEDED;
								t->growTimer = 0;
								if (!batchActive) {
									batchActive = 1;
									batchTimer = 20;
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
								if (cropRiceCount < MAX_INVENTORY_CAP) {
									t->state = CROP_EMPTY;
									cropRiceCount++;
								} else { showCapWarning = 1; }
							}
							else if (t->state == TOMATO_READY) {
								if (cropTomatoCount < MAX_INVENTORY_CAP) {
									t->state = CROP_EMPTY;
									cropTomatoCount++;
								} else { showCapWarning = 1; }
							}
							else if (t->state == BERRY_READY) {
								if (cropBerryCount < MAX_INVENTORY_CAP) {
									t->state = CROP_EMPTY;
									cropBerryCount++;
								} else { showCapWarning = 1; }
							}

							// Fix: Only count actual active crops (ignoring plowed/empty dirt)
							int activeCrops = 0;
							for (int r2 = 0; r2 < GRID_ROWS; r2++) {
								for (int c2 = 0; c2 < GRID_COLS; c2++) {
									int s = farmGrid[r2][c2].state;
									if (s == CROP_SEEDED || s == CROP_WATERED || s == CROP_READY ||
										s == TOMATO_READY || s == BERRY_TREE || s == BERRY_READY) {
										activeCrops++;
									}
								}
							}

							if (activeCrops == 0) {
								batchActive = 0;
								batchTimer = 0;
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
