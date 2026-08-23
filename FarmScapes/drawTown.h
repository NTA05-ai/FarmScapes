#ifndef DRAW_TOWN_H
#define DRAW_TOWN_H

#include "iGraphics.h"

// External global variables defined in main.cpp
extern int currentSeason;
extern int playerX;
extern int playerY;
extern int showDialogue;
extern char dialogueText[200];
extern char npcName[50];

// 1. COLLISION LOGIC: Restricts player movement exclusively to yellow dirt paths
int canWalk(int newX, int newY) {
	// Top Horizontal Path: Hut -> Market -> Nadira's Herbary & Cropland
	if (newY >= 380 && newY <= 420 && newX >= 120 && newX <= 750) return 1;

	// Main Vertical Trunk Line (Center Column)
	if (newX >= 360 && newX <= 400 && newY >= 60 && newY <= 400) return 1;

	// Middle Horizontal Branch: Center Road -> Ragib's Barn & Ranch
	if (newY >= 250 && newY <= 290 && newX >= 380 && newX <= 720) return 1;

	// Lower Horizontal Branch: Center Road -> The Fishery & Anika's Boathouse
	if (newY >= 140 && newY <= 180 && newX >= 380 && newX <= 720) return 1;

	// Short Entrance Path to Hut Door (Far Left)
	if (newX >= 120 && newX <= 150 && newY >= 380 && newY <= 450) return 1;

	// Block movement anywhere else (Trees, Water, Buildings)
	return 0;
}

// 2. TOWN RENDERING FUNCTION
void drawTown() {
	// Render seasonal town map background (800x600 canvas)
	if (currentSeason == 0) iShowBMP(0, 0, "assets/town_summer_bg.bmp");
	else if (currentSeason == 1) iShowBMP(0, 0, "assets/town_rainy_bg.bmp");
	else if (currentSeason == 2) iShowBMP(0, 0, "assets/town_winter_bg.bmp");

	// Render NPC sprites at their dedicated locations on the map
	iShowBMP(580, 390, "assets/npc_nadira.bmp"); // Nadira near Cropland
	iShowBMP(500, 260, "assets/npc_ragib.bmp");  // Ragib near Barn
	iShowBMP(500, 150, "assets/npc_anika.bmp");  // Anika near Fishery

	// Render player character (Zubayer)
	iShowBMP(playerX, playerY, "assets/zubayer_player.bmp");

	// Render dialogue overlay box if interacting
	if (showDialogue) {
		iSetColor(40, 40, 40);
		iFilledRectangle(100, 40, 600, 100);
		iSetColor(255, 255, 255);
		iRectangle(102, 42, 596, 96);

		iText(120, 110, npcName, GLUT_BITMAP_HELVETICA_18);
		iText(120, 70, dialogueText, GLUT_BITMAP_HELVETICA_12);
		iText(520, 50, "[Press E to Close]", GLUT_BITMAP_HELVETICA_10);
	}
}

#endif