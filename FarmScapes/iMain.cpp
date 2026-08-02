#define _CRT_SECURE_NO_WARNINGS

#include "iGraphics.h"
#include "bitmap_loader.h" // Integrates custom BMP direct loader
#include <stdio.h>
#include <stdlib.h>

// ==========================================
// GAME STATES
// ==========================================
#define STATE_MENU 0
#define STATE_LEVEL_1 1
#define STATE_INSTRUCTIONS 2

int gameState = STATE_MENU;

// ==========================================
// PLAYER & GAME VARIABLES
// ==========================================
int playerGold = 100;

// Selected Tool: 0 = None, 1 = Plow, 2 = Seed, 3 = Water, 4 = Harvest
int selectedTool = 0;

#define CROP_EMPTY 0
#define CROP_PLOWED 1
#define CROP_SEEDED 2
#define CROP_WATERED 3
#define CROP_READY 4

struct Tile {
	int x, y;
	int state;
	int growTimer;
};

#define GRID_ROWS 3
#define GRID_COLS 3
Tile farmGrid[GRID_ROWS][GRID_COLS];

// ==========================================
// DRAW FUNCTIONS
// ==========================================

void drawMenu() {
	// Menu Background (using bitmap_loader.h)
	iShowBMPAlternative(0, 0, "assets/menu_bg.bmp");

	// Title
	iSetColor(0, 100, 0);
	iText(310, 520, "FARMSCAPES", GLUT_BITMAP_TIMES_ROMAN_24);

	// Buttons
	iSetColor(139, 69, 19);
	iFilledRectangle(300, 380, 200, 50); // Play Button
	iFilledRectangle(300, 300, 200, 50); // Instructions Button

	iSetColor(178, 34, 34);
	iFilledRectangle(300, 220, 200, 50); // Exit Button

	// Button Labels
	iSetColor(255, 255, 255);
	iText(360, 398, "PLAY GAME", GLUT_BITMAP_HELVETICA_18);
	iText(335, 318, "INSTRUCTIONS", GLUT_BITMAP_HELVETICA_18);
	iText(380, 238, "EXIT", GLUT_BITMAP_HELVETICA_18);
}

void drawLevel1() {
	// Level 1 Background
	iShowBMPAlternative(0, 0, "assets/mainland_bg.bmp");

	// Top HUD
	iSetColor(50, 50, 50);
	iFilledRectangle(0, 550, 800, 50);

	// Gold Counter (Safe sprintf_s)
	iSetColor(255, 215, 0);
	char goldStr[32];
	sprintf_s(goldStr, sizeof(goldStr), "Gold: $%d", playerGold);
	iText(20, 568, goldStr, GLUT_BITMAP_HELVETICA_18);

	iSetColor(255, 255, 255);
	iText(200, 568, "Level 1: Mainland Farmland", GLUT_BITMAP_HELVETICA_18);

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

void drawInstructions() {
	iSetColor(240, 240, 240);
	iFilledRectangle(0, 0, 800, 600);

	iSetColor(0, 0, 0);
	iText(300, 500, "HOW TO PLAY", GLUT_BITMAP_TIMES_ROMAN_24);

	iText(150, 400, "1. Select 'PLOW' from toolbar and click a grass tile.", GLUT_BITMAP_HELVETICA_12);
	iText(150, 360, "2. Select 'PLANT' ($5 per seed) and click the plowed soil.", GLUT_BITMAP_HELVETICA_12);
	iText(150, 320, "3. Select 'WATER' and click the seeded soil to start growth.", GLUT_BITMAP_HELVETICA_12);
	iText(150, 280, "4. Wait 5 sec for crop to mature, then click 'HARVEST' to earn $15!", GLUT_BITMAP_HELVETICA_12);

	iSetColor(180, 50, 50);
	iFilledRectangle(330, 150, 140, 40);
	iSetColor(255, 255, 255);
	iText(370, 164, "BACK", GLUT_BITMAP_HELVETICA_12);
}

void iDraw() {
	iClear();

	if (gameState == STATE_MENU) {
		drawMenu();
	}
	else if (gameState == STATE_LEVEL_1) {
		drawLevel1();
	}
	else if (gameState == STATE_INSTRUCTIONS) {
		drawInstructions();
	}
}

// ==========================================
// MOUSE & INPUT CONTROLS
// ==========================================

void iMouse(int button, int state, int mx, int my) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {

		// --- MENU CLICK NAVIGATION ---
		if (gameState == STATE_MENU) {
			if (mx >= 300 && mx <= 500 && my >= 380 && my <= 430) {
				gameState = STATE_LEVEL_1;
			}
			else if (mx >= 300 && mx <= 500 && my >= 300 && my <= 350) {
				gameState = STATE_INSTRUCTIONS;
			}
			else if (mx >= 300 && mx <= 500 && my >= 220 && my <= 270) {
				exit(0);
			}
		}

		// --- INSTRUCTIONS MENU ---
		else if (gameState == STATE_INSTRUCTIONS) {
			if (mx >= 330 && mx <= 470 && my >= 150 && my <= 190) {
				gameState = STATE_MENU;
			}
		}

		// --- LEVEL 1 GAMEPLAY ---
		else if (gameState == STATE_LEVEL_1) {
			// Return to Menu
			if (mx >= 680 && mx <= 780 && my >= 558 && my <= 592) {
				gameState = STATE_MENU;
				return;
			}

			// Toolbar selections
			if (my >= 28 && my <= 72) {
				if (mx >= 170 && mx <= 260) selectedTool = 1; // Plow
				if (mx >= 290 && mx <= 380) selectedTool = 2; // Seed
				if (mx >= 410 && mx <= 500) selectedTool = 3; // Water
				if (mx >= 530 && mx <= 630) selectedTool = 4; // Harvest
			}

			// Farm Plot Grid interactions
			for (int r = 0; r < GRID_ROWS; r++) {
				for (int c = 0; c < GRID_COLS; c++) {
					Tile *t = &farmGrid[r][c];

					if (mx >= t->x && mx <= t->x + 80 && my >= t->y && my <= t->y + 80) {

						// Step 1: PLOW
						if (selectedTool == 1 && t->state == CROP_EMPTY) {
							t->state = CROP_PLOWED;
						}
						// Step 2: PLANT ($5)
						else if (selectedTool == 2 && t->state == CROP_PLOWED) {
							if (playerGold >= 5) {
								playerGold -= 5;
								t->state = CROP_SEEDED;
							}
						}
						// Step 3: WATER
						else if (selectedTool == 3 && t->state == CROP_SEEDED) {
							t->state = CROP_WATERED;
							t->growTimer = 0;
						}
						// Step 4: HARVEST (+$15)
						else if (selectedTool == 4 && t->state == CROP_READY) {
							t->state = CROP_EMPTY;
							playerGold += 15;
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

void iPassiveMouseMove(int mx, int my) {
	// Keeps linker satisfied for LNK2019
}

void iKeyboard(unsigned char key) {}

void iSpecialKeyboard(unsigned char key) {}

void fixedUpdate() {
	// Keeps linker satisfied for LNK2019
}

// ==========================================
// CROP GROWTH TIMER & INIT
// ==========================================

void updateCropGrowth() {
	if (gameState != STATE_LEVEL_1) return;

	for (int r = 0; r < GRID_ROWS; r++) {
		for (int c = 0; c < GRID_COLS; c++) {
			if (farmGrid[r][c].state == CROP_WATERED) {
				farmGrid[r][c].growTimer++;
				// Crop takes 5 seconds to grow
				if (farmGrid[r][c].growTimer >= 5) {
					farmGrid[r][c].state = CROP_READY;
				}
			}
		}
	}
}

void initFarmGrid() {
	int startX = 280, startY = 200;
	int tileSize = 80, spacing = 10;

	for (int r = 0; r < GRID_ROWS; r++) {
		for (int c = 0; c < GRID_COLS; c++) {
			farmGrid[r][c].x = startX + c * (tileSize + spacing);
			farmGrid[r][c].y = startY + r * (tileSize + spacing);
			farmGrid[r][c].state = CROP_EMPTY;
			farmGrid[r][c].growTimer = 0;
		}
	}
}

int main() {
	initFarmGrid();

	// Growth timer ticks every 1000ms (1 sec)
	iSetTimer(1000, updateCropGrowth);

	iInitialize(800, 600, "FarmScapes - 2D Farming Simulator");
	iStart();
	return 0;
}