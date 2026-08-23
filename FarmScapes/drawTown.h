#ifndef DRAW_TOWN_H
#define DRAW_TOWN_H

#include "iGraphics.h"
#include "bitmap_loader.h"

extern int currentSeason;
extern int playerX;
extern int playerY;
extern int showDialogue;
extern char dialogueText[200];
extern char npcName[50];

// 1. COLLISION LOGIC
int canWalk(int newX, int newY) {
	// Bounds adjusted so player starting at (400, 300) can move
	if (newY >= 250 && newY <= 450 && newX >= 100 && newX <= 750) return 1;
	if (newX >= 350 && newX <= 450 && newY >= 50 && newY <= 500) return 1;
	return 0;
}

// 2. TOWN RENDERING
void drawTown() {
	// Render 800x600 scaled background
	if (currentSeason == 0) iShowBMP(0, 0, "assets/town_summer_bg.bmp");
	else if (currentSeason == 1) iShowBMP(0, 0, "assets/town_rainy_bg.bmp");
	else if (currentSeason == 2) iShowBMP(0, 0, "assets/town_winter_bg.bmp");

	// Render 48x48 player with pure black background ignored (0 key)
	iShowBMP2(playerX, playerY, "assets/zubayer_player.bmp", 0);

	// --- BOTTOM RIGHT: RETURN TO MENU BUTTON ---
	iSetColor(180, 50, 50);
	iFilledRectangle(670, 20, 110, 40); // Dark red background

	iSetColor(255, 255, 255);
	iRectangle(670, 20, 110, 40);       // White border
	iText(705, 33, "MENU", GLUT_BITMAP_HELVETICA_12); // Button label

	// Render Dialogue Overlay
	if (showDialogue) {
		iSetColor(30, 30, 30);
		iFilledRectangle(80, 30, 640, 110);
		iSetColor(255, 255, 255);
		iRectangle(82, 32, 636, 106);

		if (strcmp(npcName, "Nadira") == 0) iShowBMP2(95, 45, "assets/portrait_nadira.bmp", 0);
		else if (strcmp(npcName, "Ragib") == 0) iShowBMP2(95, 45, "assets/portrait_ragib.bmp", 0);
		else if (strcmp(npcName, "Anika") == 0) iShowBMP2(95, 45, "assets/portrait_anika.bmp", 0);

		iText(190, 110, npcName, GLUT_BITMAP_HELVETICA_18);
		iText(190, 75, dialogueText, GLUT_BITMAP_HELVETICA_12);
		iText(560, 42, "[Press E to Close]", GLUT_BITMAP_HELVETICA_10);
	}
}

#endif // DRAW_TOWN_H