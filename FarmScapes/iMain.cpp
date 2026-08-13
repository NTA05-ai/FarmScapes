#define _CRT_SECURE_NO_WARNINGS

#include "iGraphics.h"
#include "bitmap_loader.h" // Integrates custom BMP direct loader
#include <stdio.h>
#include <stdlib.h>
#include <mmsystem.h>       // MCI - used to play background.mp3
#pragma comment(lib, "winmm.lib")
#include "updatecropgrowth.h"
#include "drawlevel1.h"

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

void initAudio() {
	mciSendString("open \"Audios/background.mp3\" type mpegvideo alias bgmusic", NULL, 0, NULL);
	mciSendString("play bgmusic repeat", NULL, 0, NULL);
}

void toggleMusic() {
	musicOn = !musicOn;
	if (musicOn) {
		mciSendString("resume bgmusic", NULL, 0, NULL);
	}
	else {
		mciSendString("pause bgmusic", NULL, 0, NULL);
	}
}

// ==========================================
// PLAYER & GAME VARIABLES
// ==========================================
int playerGold = 100;

// Selected Tool: 0 = None, 1 = Plow, 2 = Seed, 3 = Water, 4 = Harvest
int selectedTool = 0;
int batchTimer = 0;       // Counts down from 20 seconds
int batchActive = 0;      // 0 = no batch running, 1 = timer active
#define BATCH_TIME_LIMIT 20

Tile farmGrid[GRID_ROWS][GRID_COLS];

// ==========================================
// DRAW FUNCTIONS
// ==========================================

void drawMenu() {
	// Reset color to white before displaying menu background BMP
	iSetColor(255, 255, 255);
	iShowBMPAlternative(0, 0, "assets/menu_bg.bmp");

	// Title
	iSetColor(0, 100, 0);
	iText(310, 540, "FARMSCAPES", GLUT_BITMAP_TIMES_ROMAN_24);

	// Re-aligned Buttons (4 items evenly spaced on 800x600 screen)
	// Button 1: Play Game
	iSetColor(139, 69, 19);
	iFilledRectangle(300, 420, 200, 50);
	iSetColor(255, 255, 255);
	iText(360, 438, "PLAY GAME", GLUT_BITMAP_HELVETICA_18);

	// Button 2: Settings
	iSetColor(139, 69, 19);
	iFilledRectangle(300, 330, 200, 50);
	iSetColor(255, 255, 255);
	iText(355, 348, "SETTINGS", GLUT_BITMAP_HELVETICA_18);

	// Button 3: Credits
	iSetColor(139, 69, 19);
	iFilledRectangle(300, 240, 200, 50);
	iSetColor(255, 255, 255);
	iText(360, 258, "CREDITS", GLUT_BITMAP_HELVETICA_18);

	// Button 4: Exit
	iSetColor(178, 34, 34);
	iFilledRectangle(300, 150, 200, 50);
	iSetColor(255, 255, 255);
	iText(380, 168, "EXIT", GLUT_BITMAP_HELVETICA_18);
}

void drawSettings() {
	iSetColor(240, 240, 240);
	iFilledRectangle(0, 0, 800, 600);

	iSetColor(0, 0, 0);
	iText(320, 500, "SETTINGS", GLUT_BITMAP_TIMES_ROMAN_24);

	if (musicOn) iSetColor(0, 150, 0);
	else iSetColor(150, 0, 0);
	iFilledRectangle(300, 380, 200, 50);

	iSetColor(255, 255, 255);
	if (musicOn) iText(345, 398, "MUSIC: ON", GLUT_BITMAP_HELVETICA_18);
	else iText(340, 398, "MUSIC: OFF", GLUT_BITMAP_HELVETICA_18);

	// Back button
	iSetColor(180, 50, 50);
	iFilledRectangle(330, 150, 140, 40);
	iSetColor(255, 255, 255);
	iText(370, 164, "BACK", GLUT_BITMAP_HELVETICA_12);
}

void drawCredits() {
	iSetColor(240, 240, 240);
	iFilledRectangle(0, 0, 800, 600);

	iSetColor(0, 0, 0);
	iText(320, 500, "CREDITS", GLUT_BITMAP_TIMES_ROMAN_24);

	iText(250, 420, "Md. Azizur Rahman Ragib, ID:00725105101090", GLUT_BITMAP_HELVETICA_18);
	iText(250, 380, "Nabiha Tahsin Anika, ID:00725105101099", GLUT_BITMAP_HELVETICA_18);
	iText(250, 340, "Nadira Fairuza, ID:00725105101118", GLUT_BITMAP_HELVETICA_18);

	// Back button
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

		// --- 1. MENU NAVIGATION (4 Centered Buttons) ---
		if (gameState == STATE_MENU) {
			// PLAY GAME Button (X: 300-500, Y: 420-470)
			if (mx >= 300 && mx <= 500 && my >= 420 && my <= 470) {
				gameState = STATE_LEVEL_1;
			}
			// SETTINGS Button (X: 300-500, Y: 330-380)
			else if (mx >= 300 && mx <= 500 && my >= 330 && my <= 380) {
				gameState = STATE_SETTINGS;
			}
			// CREDITS Button (X: 300-500, Y: 240-290)
			else if (mx >= 300 && mx <= 500 && my >= 240 && my <= 290) {
				gameState = STATE_CREDITS;
			}
			// EXIT Button (X: 300-500, Y: 150-200)
			else if (mx >= 300 && mx <= 500 && my >= 150 && my <= 200) {
				mciSendString("close bgmusic", NULL, 0, NULL);
				exit(0);
			}
		}

		// --- SETTINGS MENU ---
		else if (gameState == STATE_SETTINGS) {
			if (mx >= 300 && mx <= 500 && my >= 380 && my <= 430) {
				toggleMusic();
			}
			else if (mx >= 330 && mx <= 470 && my >= 150 && my <= 190) {
				gameState = STATE_MENU;
			}
		}

		// --- CREDITS SCREEN ---
		else if (gameState == STATE_CREDITS) {
			if (mx >= 330 && mx <= 470 && my >= 150 && my <= 190) {
				gameState = STATE_MENU;
			}
		}

		// --- 2. LEVEL 1 GAMEPLAY ---
		else if (gameState == STATE_LEVEL_1) {
			// Return to Menu Button
			if (mx >= 680 && mx <= 780 && my >= 558 && my <= 592) {
				gameState = STATE_MENU;
				return;
			}

			// Toolbar Selections
			if (my >= 28 && my <= 72) {
				if (mx >= 170 && mx <= 260) selectedTool = 1; // PLOW
				if (mx >= 290 && mx <= 380) selectedTool = 2; // PLANT
				if (mx >= 410 && mx <= 500) selectedTool = 3; // WATER
				if (mx >= 530 && mx <= 630) selectedTool = 4; // HARVEST
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


int main() {
	initFarmGrid();
	initAudio();

	iSetTimer(1000, updateCropGrowth);

	iInitialize(800, 600, "FarmScapes - 2D Farming Simulator");
	iStart();
	return 0;
}

		
