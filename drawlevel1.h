#ifndef DRAWLEVEL1_H
#define DRAWLEVEL1_H


#include <stdio.h>

// -------------------------------------------------------------
// 1. TELL THIS HEADER WHICH GLOBAL VARIABLES EXIST IN iMain.cpp
// -------------------------------------------------------------
extern int playerGold;
extern int selectedTool;
extern int batchTimer;
extern int batchActive;

// Note: Tile struct and GRID_ROWS/GRID_COLS are already available
// if updatecropgrowth.h is included before this file in iMain.cpp.
extern Tile farmGrid[3][3];

// -------------------------------------------------------------
// 2. LEVEL 1 RENDER FUNCTION
// -------------------------------------------------------------
inline void drawLevel1() {
	// Reset color tint to pure white so OpenGL doesn't distort/tint background BMP textures
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

#endif // DRAWLEVEL1_H