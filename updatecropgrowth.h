#ifndef UPDATECROPGROWTH_H
#define UPDATECROPGROWTH_H

#include <stdio.h>

#define STATE_LEVEL_1 1

#define CROP_EMPTY 0
#define CROP_PLOWED 1
#define CROP_SEEDED 2
#define CROP_WATERED 3
#define CROP_READY 4
#define CROP_ROTTEN 5
#define TOMATO_READY 6
#define BERRY_TREE 7
#define BERRY_READY 8

#define GRID_ROWS 3
#define GRID_COLS 3

struct Tile {
	int x, y;
	int state;
	int growTimer;
	int spoilTimer;
};

extern int gameState;
extern int playerGold;
extern int riceGold;
extern int tomatoGold;
extern int berryGold;
extern int batchTimer;
extern int batchActive;
extern int tomatoUnlocked;
extern int berryUnlocked;
extern int hasRottenCrop;
extern Tile farmGrid[GRID_ROWS][GRID_COLS];

inline void updateCropGrowth() {
	if (gameState != STATE_LEVEL_1) return;

	for (int r = 0; r < GRID_ROWS; r++) {
		for (int c = 0; c < GRID_COLS; c++) {
			Tile *t = &farmGrid[r][c];

			// --- BERRY MODE ---
			if (berryUnlocked) {
				if (t->state == CROP_WATERED) {
					t->state = BERRY_READY;
				}
			}
			// --- TOMATO MODE ---
			else if (tomatoUnlocked) {
				if (t->state == CROP_WATERED) {
					t->growTimer++;
					if (t->growTimer >= 3) {
						t->state = TOMATO_READY;
					}
				}
			}
			// --- STANDARD PADDY / CROP MODE ---
			else {
				if (t->state == CROP_WATERED) {
					t->growTimer++;
					if (t->growTimer >= 5) {
						t->state = CROP_READY;
					}
				}
			}
		}
	}

	// 20-Second Countdown Batch Timer
	if (batchActive) {
		batchTimer--;

		if (batchTimer <= 0) {
			batchActive = 0;
			hasRottenCrop = 1;
			playerGold -= 20;
			if (playerGold < 0) playerGold = 0;

			for (int r = 0; r < GRID_ROWS; r++) {
				for (int c = 0; c < GRID_COLS; c++) {
					int s = farmGrid[r][c].state;
					if (s == CROP_PLOWED || s == CROP_SEEDED || s == CROP_WATERED ||
						s == CROP_READY || s == TOMATO_READY || s == BERRY_TREE || s == BERRY_READY) {
						farmGrid[r][c].state = CROP_ROTTEN;
					}
				}
			}
		}
	}
}

inline void initFarmGrid() {
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

#endif // UPDATECROPGROWTH_H
