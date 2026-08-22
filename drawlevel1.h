#ifndef DRAWLEVEL1_H
#define DRAWLEVEL1_H

#include <stdio.h>

extern int playerGold;
extern int selectedTool;
extern int batchTimer;
extern int batchActive;
extern int tomatoUnlocked;
extern int berryUnlocked;
extern Tile farmGrid[3][3];

inline void drawLevel1() {
	iSetColor(255, 255, 255);
	iShowBMPAlternative(0, 0, "assets/mainland_bg.bmp");

	// --------------------------------------
	// TOP HUD
	// --------------------------------------
	iSetColor(50, 50, 50);
	iFilledRectangle(0, 540, 800, 60);

	// Total Gold Display
	iSetColor(255, 215, 0);
	char goldStr[32];
	sprintf_s(goldStr, sizeof(goldStr), "Total: $%d", playerGold);
	iText(15, 562, goldStr, GLUT_BITMAP_HELVETICA_18);

	// Return to Menu Button
	iSetColor(180, 50, 50);
	iFilledRectangle(680, 552, 100, 34);
	iSetColor(255, 255, 255);
	iText(710, 564, "MENU", GLUT_BITMAP_HELVETICA_12);

	// --------------------------------------
	// TIMER DISPLAY (TEXT ONLY)
	// --------------------------------------
	if (batchActive) {
		char timeText[16];
		sprintf_s(timeText, sizeof(timeText), "Time Left: %ds", batchTimer);
		iSetColor(255, 255, 0);
		iText(480, 562, timeText, GLUT_BITMAP_HELVETICA_18);
	}

	// --------------------------------------
	// 3x3 FARM TILES
	// --------------------------------------
	for (int r = 0; r < GRID_ROWS; r++) {
		for (int c = 0; c < GRID_COLS; c++) {
			Tile t = farmGrid[r][c];

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
			case TOMATO_READY:
				iShowBMPAlternative(t.x, t.y, "assets/tile_tomato_ripe.bmp");
				break;
			case BERRY_TREE:
				iShowBMPAlternative(t.x, t.y, "assets/tile_berrytree.bmp");
				break;
			case BERRY_READY:
				iShowBMPAlternative(t.x, t.y, "assets/tile_berry.bmp");
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

	if (selectedTool == 1) iSetColor(0, 255, 0); else iSetColor(100, 100, 100);
	iRectangle(170, 28, 90, 44);
	iText(190, 44, "PLOW", GLUT_BITMAP_HELVETICA_12);

	if (selectedTool == 2) iSetColor(0, 255, 0); else iSetColor(100, 100, 100);
	iRectangle(290, 28, 90, 44);
	iText(305, 44, "PLANT($5)", GLUT_BITMAP_HELVETICA_10);

	if (selectedTool == 3) iSetColor(0, 255, 0); else iSetColor(100, 100, 100);
	iRectangle(410, 28, 90, 44);
	iText(425, 44, "WATER", GLUT_BITMAP_HELVETICA_12);

	if (selectedTool == 4) iSetColor(0, 255, 0); else iSetColor(100, 100, 100);
	iRectangle(530, 28, 100, 44);

	if (berryUnlocked) {
		iText(538, 44, "HARVEST($60)", GLUT_BITMAP_HELVETICA_10);
	}
	else if (tomatoUnlocked) {
		iText(538, 44, "HARVEST($45)", GLUT_BITMAP_HELVETICA_10);
	}
	else {
		iText(538, 44, "HARVEST($15)", GLUT_BITMAP_HELVETICA_10);
	}
}

#endif // DRAWLEVEL1_H
