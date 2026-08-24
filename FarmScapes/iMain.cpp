#define _CRT_SECURE_NO_WARNINGS

#include "iGraphics.h"
#include "bitmap_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>

#pragma comment(lib, "winmm.lib")

// --- SCREEN & RENDER DIMENSIONS ---
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// Fixed Player Sprite Size
#define PLAYER_WIDTH 48
#define PLAYER_HEIGHT 48

// Game States
#define STATE_MENU 0
#define STATE_LEVEL_1 1
#define STATE_SETTINGS 2
#define STATE_CREDITS 3
#define STATE_LOADING 4
#define STATE_TOWN 5

int gameState = STATE_MENU;
int loadingTimer = 0;
int musicOn = 1;
int eKeyPressedLastFrame = 0;

// --- TOWN & SEASON VARIABLES ---
int playerX = 400, playerY = 300, playerSpeed = 8;
int currentSeason = 0, seasonTimer = 120; // 0=Summer, 1=Rainy, 2=Winter
int showDialogue = 0;
char dialogueText[200] = "";
char npcName[50] = "";
int level2Unlocked = 0, level3Unlocked = 0;

#include "toggleMusic.h"
#include "menu.h"
#include "settings.h"
#include "credits.h"
#include "loading.h"
#include "updatecropgrowth.h"
#include "drawlevel1.h"
#include "drawTown.h"

int playerGold = 0;

// CROP PRICES (MAX PROFIT = $5 EACH)
int riceBuyPrice = 5, riceSellPrice = 10;
int tomatoBuyPrice = 15, tomatoSellPrice = 20;
int berryBuyPrice = 25, berrySellPrice = 30;

// STARTING SEEDS
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

// --- BOUNDARY CHECK HELPER ---
int isWithinBounds(int x, int y) {
	if (x < 0 || x > SCREEN_WIDTH - PLAYER_WIDTH) return 0;
	if (y < 0 || y > SCREEN_HEIGHT - PLAYER_HEIGHT) return 0;
	return 1;
}

void updatePlayer() {
	// Forces iGraphics to continuously repaint player coordinates
}

void iDraw() {
	iClear();

	if (gameState == STATE_MENU) drawMenu();
	else if (gameState == STATE_LOADING) drawLoading();
	else if (gameState == STATE_TOWN) drawTown();
	else if (gameState == STATE_LEVEL_1) drawLevel1();
	else if (gameState == STATE_SETTINGS) drawSettings();
	else if (gameState == STATE_CREDITS) drawCredits();
}

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		// 1. MENU: Play button transitions to LOADING
		if (gameState == STATE_MENU) {
			if (mx >= 290 && mx <= 510 && my >= 410 && my <= 480) {
				gameState = STATE_LOADING; // Set state to loading
				loadingTimer = 0;          // Reset timer
			}
			else if (mx >= 290 && mx <= 510 && my >= 320 && my <= 390) gameState = STATE_SETTINGS;
			else if (mx >= 290 && mx <= 510 && my >= 230 && my <= 300) gameState = STATE_CREDITS;
			else if (mx >= 290 && mx <= 510 && my >= 140 && my <= 210) {
				mciSendString("close bgmusic", NULL, 0, NULL);
				exit(0);
			}
		}

		// 2. SETTINGS / CREDITS
		else if (gameState == STATE_SETTINGS) {
			if (mx >= 290 && mx <= 510 && my >= 340 && my <= 410) {
				toggleMusic();
			}
			else if (mx >= 290 && mx <= 510 && my >= 220 && my <= 290) gameState = STATE_MENU;
		}
		else if (gameState == STATE_CREDITS) {
			if (mx >= 290 && mx <= 510 && my >= 140 && my <= 210) gameState = STATE_MENU;
		}
		// 3. TOWN STATE
		else if (gameState == STATE_TOWN) {
			if (mx >= 670 && mx <= 780 && my >= 20 && my <= 60) {
				gameState = STATE_MENU;
				return;
			}
		}
		// 4. LEVEL 1 STATE
		else if (gameState == STATE_LEVEL_1) {
			if (showCapWarning) {
				showCapWarning = 0;
			}

			// Market Button
			if (mx >= 420 && mx <= 520 && my >= 552 && my <= 586) {
				isMarketOpen = !isMarketOpen;
				return;
			}

			// Explore Town Button
			if (mx >= 535 && mx <= 655 && my >= 552 && my <= 586) {
				gameState = STATE_TOWN;
				return;
			}

			// Menu Button
			if (mx >= 670 && mx <= 780 && my >= 552 && my <= 586) {
				gameState = STATE_MENU;
				return;
			}

			// Marketplace Overlay Interactions
			if (isMarketOpen) {
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

				// Mass-Plow Upgrade
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
								}
								else { showCapWarning = 1; }
							}
							else if (t->state == TOMATO_READY) {
								if (cropTomatoCount < MAX_INVENTORY_CAP) {
									t->state = CROP_EMPTY;
									cropTomatoCount++;
								}
								else { showCapWarning = 1; }
							}
							else if (t->state == BERRY_READY) {
								if (cropBerryCount < MAX_INVENTORY_CAP) {
									t->state = CROP_EMPTY;
									cropBerryCount++;
								}
								else { showCapWarning = 1; }
							}

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

// --- CONTINUOUS GAME LOOP (Runs every frame via SetTimer) ---
void fixedUpdate() {
	// 1. WASD & Arrow Keys Movement
	if ((gameState == STATE_TOWN && !showDialogue) || gameState == STATE_LEVEL_1) {

		// UP (W or Up Arrow)
		if (isKeyPressed('w') || isKeyPressed('W') || isSpecialKeyPressed(GLUT_KEY_UP)) {
			if (gameState == STATE_LEVEL_1 || (canWalk(playerX, playerY + playerSpeed) && isWithinBounds(playerX, playerY + playerSpeed))) {
				playerY += playerSpeed;
			}
		}
		// DOWN (S or Down Arrow)
		if (isKeyPressed('s') || isKeyPressed('S') || isSpecialKeyPressed(GLUT_KEY_DOWN)) {
			if (gameState == STATE_LEVEL_1 || (canWalk(playerX, playerY - playerSpeed) && isWithinBounds(playerX, playerY - playerSpeed))) {
				playerY -= playerSpeed;
			}
		}
		// LEFT (A or Left Arrow)
		if (isKeyPressed('a') || isKeyPressed('A') || isSpecialKeyPressed(GLUT_KEY_LEFT)) {
			if (gameState == STATE_LEVEL_1 || (canWalk(playerX - playerSpeed, playerY) && isWithinBounds(playerX - playerSpeed, playerY))) {
				playerX -= playerSpeed;
			}
		}
		// RIGHT (D or Right Arrow)
		if (isKeyPressed('d') || isKeyPressed('D') || isSpecialKeyPressed(GLUT_KEY_RIGHT)) {
			if (gameState == STATE_LEVEL_1 || (canWalk(playerX + playerSpeed, playerY) && isWithinBounds(playerX + playerSpeed, playerY))) {
				playerX += playerSpeed;
			}
		}
	}
	// --- DIALOGUE TRIGGER (Bypasses GLUT iKeyboard completely) ---
	if (gameState == STATE_TOWN) {
		int eIsDown = isKeyPressed('e') || isKeyPressed('E');

		// Edge trigger: Only run ONCE when 'E' is first pressed down
		if (eIsDown && !eKeyPressedLastFrame) {

			// 1. Close dialogue if already open
			if (showDialogue) {
				showDialogue = 0;
				if (strcmp(npcName, "Nadira") == 0) {
					gameState = STATE_LEVEL_1; // Transition to Level 1
				}
			}
			// 2. Open Nadira Dialogue
			else if (playerX >= 480 && playerX <= 700 && playerY >= 320 && playerY <= 460) {
				strcpy(npcName, "Nadira");
				strcpy(dialogueText, "Welcome to the Farm! Press E again to enter Level 1.");
				showDialogue = 1;
			}
			// 3. Open Ragib Dialogue
			else if (playerX >= 450 && playerX <= 680 && playerY >= 210 && playerY <= 310) {
				strcpy(npcName, "Ragib");
				strcpy(dialogueText, level2Unlocked ? "Entering Ranch..." : "Welcome to the Ranch! Clear Level 1 first.");
				showDialogue = 1;
			}
			// 4. Open Anika Dialogue
			else if (playerX >= 450 && playerX <= 680 && playerY >= 100 && playerY <= 200) {
				strcpy(npcName, "Anika");
				strcpy(dialogueText, level3Unlocked ? "Entering Fishery..." : "Welcome to the Fishery! Clear Level 2 first.");
				showDialogue = 1;
			}
		}

		// Save state for next frame debounce
		eKeyPressedLastFrame = eIsDown;
	}
}

// Keep these blank or handle single-press toggles here (like 'E' to talk)
void iKeyboard(unsigned char key) {
	if (gameState == STATE_TOWN) {
		if (key == 'e' || key == 'E') {

			// 1. Close dialogue if already open
			if (showDialogue) {
				showDialogue = 0;
				if (strcmp(npcName, "Nadira") == 0) {
					gameState = STATE_LEVEL_1; // Transition to Level 1
				}
				return;
			}

			// 2. Open Nadira Dialogue
			if (playerX >= 480 && playerX <= 700 && playerY >= 320 && playerY <= 460) {
				sprintf(npcName, "Nadira");
				sprintf(dialogueText, "Welcome to the Cropland! Press E again to start farming.");
				showDialogue = 1;
			}
			// 3. Open Ragib Dialogue
			else if (playerX >= 450 && playerX <= 680 && playerY >= 210 && playerY <= 310) {
				sprintf(npcName, "Ragib");
				sprintf(dialogueText, level2Unlocked ? "Entering Ranch..." : "Welcome to the Ranch! Clear Level 1 first.");
				showDialogue = 1;
			}
			// 4. Open Anika Dialogue
			else if (playerX >= 450 && playerX <= 680 && playerY >= 100 && playerY <= 200) {
				sprintf(npcName, "Anika");
				sprintf(dialogueText, level3Unlocked ? "Entering Fishery..." : "Welcome to the Fishery! Clear Level 2 first.");
				showDialogue = 1;
			}
		}
	}
}

void iSpecialKeyboard(unsigned char key) {}

void updateSeasonTimer() {
	if (seasonTimer > 0) {
		seasonTimer--;
	}
	else {
		currentSeason = (currentSeason + 1) % 3; // Rotates between 0, 1, and 2
		seasonTimer = 120;                       // Reset back to 2 minutes
	}
}
// --- NEW LOADING SCREEN ANIMATION CALLBACK ---
void updateLoading() {
	if (gameState == STATE_LOADING) {
		loadingTimer += 2; // Increments smoothly every 50ms

		if (loadingTimer >= 100) {
			gameState = STATE_LEVEL_1; // Transition to Level 1
			loadingTimer = 0;
		}
	}
}
void iAnim() {
	// Empty function used strictly to force GLUT to process keyboard input frames
}
int main() {
	initFarmGrid();
	initAudio();

	iSetTimer(1000, updateCropGrowth);
	iSetTimer(1000, updateSeasonTimer);

	iSetTimer(50, updateLoading);
	iSetTimer(20, iAnim);

	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "FarmScapes - 2D Farming Simulator");
	iStart();
	return 0;
}