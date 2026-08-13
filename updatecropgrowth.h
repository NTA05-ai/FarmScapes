#ifndef UPDATECROPGROWTH_H
#define UPDATECROPGROWTH_H

#include <stdio.h>

// 1. Game State & Crop Macros
#define STATE_LEVEL_1 1

#define CROP_EMPTY 0
#define CROP_PLOWED 1
#define CROP_SEEDED 2
#define CROP_WATERED 3
#define CROP_READY 4
#define CROP_ROTTEN 5

#define GRID_ROWS 3
#define GRID_COLS 3

// 2. Tile Structure
struct Tile {
	int x, y;
	int state;
	int growTimer;
	int spoilTimer;
};

// 3. Tell header these variables exist in iMain.cpp
extern int gameState;
extern int playerGold;
extern int batchTimer;
extern int batchActive;
extern Tile farmGrid[GRID_ROWS][GRID_COLS];

// 4. Functions (marked inline to avoid linker errors)
inline void updateCropGrowth() {
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