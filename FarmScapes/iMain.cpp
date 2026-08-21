#define _CRT_SECURE_NO_WARNINGS

#include "iGraphics.h"
#include "bitmap_loader.h"
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")
// ==========================================
// GAME STATES
// ==========================================
#define STATE_MENU 0
#define STATE_LEVEL_1 1
#define STATE_SETTINGS 2
#define STATE_CREDITS 3

int gameState = STATE_MENU;

// ==========================================
// AUDIO
// ==========================================
int musicOn = 1; // 1 = playing, 0 = muted

#include "toggleMusic.h"
#include "settings.h"
#include "credits.h"





// ==========================================
// PLAYER & GAME VARIABLES
// ==========================================
int playerGold = 100;

// Selected Tool: 0 = None, 1 = Plow, 2 = Seed, 3 = Water, 4 = Harvest
int selectedTool = 0;
int batchTimer = 0;       // Counts down from 20 seconds
int batchActive = 0;      // 0 = no batch running, 1 = timer active
#define BATCH_TIME_LIMIT 20

#define CROP_EMPTY 0
#define CROP_PLOWED 1
#define CROP_SEEDED 2
#define CROP_WATERED 3
#define CROP_READY 4
#define CROP_ROTTEN 5

struct Tile {
	int x, y;
	int state;
	int growTimer;
	int spoilTimer;
};

#define GRID_ROWS 3
#define GRID_COLS 3
Tile farmGrid[GRID_ROWS][GRID_COLS];

// ==========================================
// DRAW FUNCTIONS
// ==========================================

void drawMenu() {
    iSetColor(255, 255, 255);
    iShowBMPAlternative(0, 0, "assets/menu_bg.bmp");

    // Title Text
    iSetColor(20, 20, 20);
    iText(312, 538, "FARMSCAPES", GLUT_BITMAP_TIMES_ROMAN_24);
    iSetColor(255, 215, 0);
    iText(310, 540, "FARMSCAPES", GLUT_BITMAP_TIMES_ROMAN_24);

    // Render Buttons with White (0xFFFFFF) ignored
    iSetColor(255, 255, 255);
    iShowBMPAlternative2(300, 420, "assets/play.bmp", 0xFFFFFF);
    iShowBMPAlternative2(300, 330, "assets/settings.bmp", 0xFFFFFF);
    iShowBMPAlternative2(300, 240, "assets/credits.bmp", 0xFFFFFF);
    iShowBMPAlternative2(300, 150, "assets/exit.bmp", 0xFFFFFF);
}




void drawLevel1() {
	// FIX: Reset color tint to pure white so OpenGL doesn't distort/tint background BMP textures
	iSetColor(255, 255, 255);
	iShowBMPAlternative(0, 0, "assets/mainland_bg.bmp");

	// --------------------------------------
	// TOP HUD
	// --------------------------------------
	iSetColor(50, 50, 50);
	iFilledRectangle(0, 550, 800, 50);

	// Gold Counter
	iSetColor(255, 215, 0);
	char goldStr[32];
	sprintf_s(goldStr, sizeof(goldStr), "Gold: $%d", playerGold);
	iText(20, 568, goldStr, GLUT_BITMAP_HELVETICA_18);

	// Timer / Header Text
	if (batchActive) {
		char timerStr[32];
		sprintf_s(timerStr, sizeof(timerStr), "Time Left: %ds", batchTimer);
		iSetColor(255, 50, 50);
		iText(200, 568, timerStr, GLUT_BITMAP_HELVETICA_18);
	}
	else {
		iSetColor(255, 255, 255);
		iText(200, 568, "Level 1: Mainland Farmland", GLUT_BITMAP_HELVETICA_18);
	}

	// Return to Menu Button
	iSetColor(180, 50, 50);
	iFilledRectangle(680, 558, 100, 34);
	iSetColor(255, 255, 255);
	iText(710, 570, "MENU", GLUT_BITMAP_HELVETICA_12);

	// --------------------------------------
	// 3x3 FARM TILES
	// --------------------------------------
	for (int r = 0; r < GRID_ROWS; r++) {
		for (int c = 0; c < GRID_COLS; c++) {
			Tile t = farmGrid[r][c];

			// Reset color to white before rendering tile BMPs to prevent tint distortion
			iSetColor(255, 255, 255);

			switch (t.state) {
			case CROP_EMPTY:
				iShowBMPAlternative(t.x, t.y, "assets/tile_grass.bmp");
				break;
			case CROP_PLOWED:
				iShowBMPAlternative(t.x, t.y, "assets/tile_plowed.bmp");
				break;
			case CROP_SEEDED:
				iShowBMPAlternative(t.x, t.y, "assets/tile_seeded.bmp");
				break;
			case CROP_WATERED:
				iShowBMPAlternative(t.x, t.y, "assets/tile_watered.bmp");
				break;
			case CROP_READY:
				iShowBMPAlternative(t.x, t.y, "assets/tile_ready.bmp");
				break;
			case CROP_ROTTEN:
				iSetColor(80, 50, 20);
				iFilledRectangle(t.x, t.y, 80, 80);
				iSetColor(255, 0, 0);
				iText(t.x + 15, t.y + 35, "ROTTEN!", GLUT_BITMAP_HELVETICA_12);
				break;
			}
		}
	}

	// --------------------------------------
	// BOTTOM TOOLBAR
	// --------------------------------------
	iSetColor(210, 180, 140);
	iFilledRectangle(150, 20, 500, 60);

	// Tool 1: PLOW
	if (selectedTool == 1) iSetColor(0, 255, 0); else iSetColor(100, 100, 100);
	iRectangle(170, 28, 90, 44);
	iText(190, 44, "PLOW", GLUT_BITMAP_HELVETICA_12);

	// Tool 2: PLANT
	if (selectedTool == 2) iSetColor(0, 255, 0); else iSetColor(100, 100, 100);
	iRectangle(290, 28, 90, 44);
	iText(305, 44, "PLANT($5)", GLUT_BITMAP_HELVETICA_10);

	// Tool 3: WATER
	if (selectedTool == 3) iSetColor(0, 255, 0); else iSetColor(100, 100, 100);
	iRectangle(410, 28, 90, 44);
	iText(425, 44, "WATER", GLUT_BITMAP_HELVETICA_12);

	// Tool 4: HARVEST
	if (selectedTool == 4) iSetColor(0, 255, 0); else iSetColor(100, 100, 100);
	iRectangle(530, 28, 100, 44);
	iText(538, 44, "HARVEST($15)", GLUT_BITMAP_HELVETICA_10);
}

void iDraw() {
	iClear();

	if (gameState == STATE_MENU) {
		drawMenu();
	}
	else if (gameState == STATE_LEVEL_1) {
		drawLevel1();
	}
	else if (gameState == STATE_SETTINGS) {
		drawSettings();
	}
	else if (gameState == STATE_CREDITS) {
		drawCredits();
	}
}

// ==========================================
// MOUSE & INPUT CONTROLS
// ==========================================

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		// ==========================================
		// 1. MAIN MENU NAVIGATION
		// ==========================================
		if (gameState == STATE_MENU) {
			// PLAY GAME Button (Y: 420-470 -> Padded 410-480)
			if (mx >= 290 && mx <= 510 && my >= 410 && my <= 480) {
				gameState = STATE_LEVEL_1;
			}
			// SETTINGS Button (Y: 330-380 -> Padded 320-390)
			else if (mx >= 290 && mx <= 510 && my >= 320 && my <= 390) {
				gameState = STATE_SETTINGS;
			}
			// CREDITS Button (Y: 240-290 -> Padded 230-300)
			else if (mx >= 290 && mx <= 510 && my >= 230 && my <= 300) {
				gameState = STATE_CREDITS;
			}
			// EXIT Button (Y: 150-200 -> Padded 140-210)
			else if (mx >= 290 && mx <= 510 && my >= 140 && my <= 210) {
				mciSendString("close bgmusic", NULL, 0, NULL);
				exit(0);
			}
		}

		// ==========================================
		// 2. SETTINGS MENU
		// ==========================================
		else if (gameState == STATE_SETTINGS) {
			// Music Toggle (Padded hit box for easy clicking)
			if (mx >= 290 && mx <= 510 && my >= 340 && my <= 410) {
				musicOn = !musicOn;

				if (musicOn) {
					mciSendString("play bgmusic repeat", NULL, 0, NULL);
				}
				else {
					mciSendString("pause bgmusic", NULL, 0, NULL);
				}
			}
			// BACK TO MENU Button (Y: 230-280 -> Padded 220-290)
			else if (mx >= 290 && mx <= 510 && my >= 220 && my <= 290) {
				gameState = STATE_MENU;
			}
		}

		// --- CREDITS SCREEN ---
		else if (gameState == STATE_CREDITS) {
			// BACK TO MENU Button (X: 300 to 500, Y: 150 to 200 -> Padded 290-510, 140-210)
			if (mx >= 290 && mx <= 510 && my >= 140 && my <= 210) {
				gameState = STATE_MENU;
			}
		}

		// ==========================================
		// 4. LEVEL 1 GAMEPLAY
		// ==========================================
		else if (gameState == STATE_LEVEL_1) {
			// Return to Menu Button
			if (mx >= 670 && mx <= 790 && my >= 550 && my <= 600) {
				gameState = STATE_MENU;
				return;
			}

			// Toolbar Selections
			if (my >= 20 && my <= 80) {
				if (mx >= 160 && mx <= 270) selectedTool = 1; // PLOW
				if (mx >= 280 && mx <= 390) selectedTool = 2; // PLANT
				if (mx >= 400 && mx <= 510) selectedTool = 3; // WATER
				if (mx >= 520 && mx <= 640) selectedTool = 4; // HARVEST
			}

			// Farm Grid Interactions
			for (int r = 0; r < GRID_ROWS; r++) {
				for (int c = 0; c < GRID_COLS; c++) {
					Tile *t = &farmGrid[r][c];

					if (mx >= t->x && mx <= t->x + 80 && my >= t->y && my <= t->y + 80) {

						if (selectedTool == 1) {
							if (t->state == CROP_EMPTY || t->state == CROP_ROTTEN) {
								t->state = CROP_PLOWED;
							}
						}
						else if (selectedTool == 2 && t->state == CROP_PLOWED) {
							if (playerGold >= 5) {
								playerGold -= 5;
								t->state = CROP_SEEDED;

								if (!batchActive) {
									batchActive = 1;
									batchTimer = BATCH_TIME_LIMIT;
								}
							}
						}
						else if (selectedTool == 3 && t->state == CROP_SEEDED) {
							t->state = CROP_WATERED;
							t->growTimer = 0;
						}
						else if (selectedTool == 4 && t->state == CROP_READY) {
							t->state = CROP_EMPTY;
							playerGold += 15;

							int remainingCrops = 0;
							for (int r2 = 0; r2 < GRID_ROWS; r2++) {
								for (int c2 = 0; c2 < GRID_COLS; c2++) {
									int s = farmGrid[r2][c2].state;
									if (s == CROP_SEEDED || s == CROP_WATERED || s == CROP_READY) {
										remainingCrops++;
									}
								}
							}
							if (remainingCrops == 0) {
								batchActive = 0;
							}
						}
					}
				}
			}
		}
	}
}

// ==========================================
// REQUIRED IGRAPHICS CALLBACKS
// ==========================================

void iMouseMove(int mx, int my) {}
void iPassiveMouseMove(int mx, int my) {}
void iKeyboard(unsigned char key) {}
void iSpecialKeyboard(unsigned char key) {}
void fixedUpdate() {}

// ==========================================
// CROP GROWTH TIMER & INIT
// ==========================================

void updateCropGrowth() {
	if (gameState != STATE_LEVEL_1) return;

	for (int r = 0; r < GRID_ROWS; r++) {
		for (int c = 0; c < GRID_COLS; c++) {
			if (farmGrid[r][c].state == CROP_WATERED) {
				farmGrid[r][c].growTimer++;
				if (farmGrid[r][c].growTimer >= 5) {
					farmGrid[r][c].state = CROP_READY;
				}
			}
		}
	}

	if (batchActive) {
		batchTimer--;

		if (batchTimer <= 0) {
			batchActive = 0;
			playerGold -= 20;
			if (playerGold < 0) playerGold = 0;

			for (int r = 0; r < GRID_ROWS; r++) {
				for (int c = 0; c < GRID_COLS; c++) {
					int s = farmGrid[r][c].state;
					if (s == CROP_SEEDED || s == CROP_WATERED || s == CROP_READY) {
						farmGrid[r][c].state = CROP_ROTTEN;
					}
				}
			}
		}
	}
}

void initFarmGrid() {
	int startX = 200, startY = 200;
	int tileSize = 80, spacing = 10;

	for (int r = 0; r < GRID_ROWS; r++) {
		for (int c = 0; c < GRID_COLS; c++) {
			farmGrid[r][c].x = startX + c * (tileSize + spacing);
			farmGrid[r][c].y = startY + r * (tileSize + spacing);
			farmGrid[r][c].state = CROP_EMPTY;
			farmGrid[r][c].growTimer = 0;
			farmGrid[r][c].spoilTimer = 0;
		}
	}
}

int main() {
	initFarmGrid();
	initAudio();

	iSetTimer(1000, updateCropGrowth);

	iInitialize(800, 600, "FarmScapes - 2D Farming Simulator");
	iStart();
	return 0;
}
