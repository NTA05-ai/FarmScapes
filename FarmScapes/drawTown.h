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
	// Adjusted bounds so the starting position (400, 300) is valid
	if (newY >= 280 && newY <= 420 && newX >= 100 && newX <= 750) return 1;
	if (newX >= 360 && newX <= 420 && newY >= 50 && newY <= 420) return 1;
	if (newY >= 230 && newY <= 300 && newX >= 350 && newX <= 720) return 1;
	if (newY >= 130 && newY <= 190 && newX >= 350 && newX <= 720) return 1;
	if (newX >= 100 && newX <= 160 && newY >= 350 && newY <= 460) return 1;

	return 0;
}

// 2. TOWN RENDERING WITH DIALOGUE PORTRAITS
void drawTown() {
	// Render seasonal town map background
	if (currentSeason == 0) iShowBMP(0, 0, "assets/town_summer_bg.bmp");
	else if (currentSeason == 1) iShowBMP(0, 0, "assets/town_rainy_bg.bmp");
	else if (currentSeason == 2) iShowBMP(0, 0, "assets/town_winter_bg.bmp");

	// Render player character using standard transparent loader
	// (0 ignore Color Key treats pure black background as transparent)
	iShowBMP2(playerX, playerY, "assets/zubayer_player.bmp", 0);

	// Render Dialogue Box Overlay
	if (showDialogue) {
		// Outer box background
		iSetColor(30, 30, 30);
		iFilledRectangle(80, 30, 640, 110);
		iSetColor(255, 255, 255);
		iRectangle(82, 32, 636, 106);

		// Render NPC Portrait inside the box on the left side
		if (strcmp(npcName, "Nadira") == 0) {
			iShowBMP2(95, 45, "assets/portrait_nadira.bmp", 0);
		}
		else if (strcmp(npcName, "Ragib") == 0) {
			iShowBMP2(95, 45, "assets/portrait_ragib.bmp", 0);
		}
		else if (strcmp(npcName, "Anika") == 0) {
			iShowBMP2(95, 45, "assets/portrait_anika.bmp", 0);
		}

		// Text display offset to the right of the portrait
		iText(190, 110, npcName, GLUT_BITMAP_HELVETICA_18);
		iText(190, 75, dialogueText, GLUT_BITMAP_HELVETICA_12);
		iText(560, 42, "[Press E to Close]", GLUT_BITMAP_HELVETICA_10);
	}
}

#endif // DRAW_TOWN_H