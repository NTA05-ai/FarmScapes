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
extern int batchTimer;
extern int batchActive;
extern int tomatoUnlocked;
extern Tile farmGrid[GRID_ROWS][GRID_COLS];

inline void updateCropGrowth() {
	if (gameState != STATE_LEVEL_1) return;

	for (int r = 0; r < GRID_ROWS; r++) {
		for (int c = 0; c < GRID_COLS; c++) {
			Tile *t = &farmGrid[r][c];

			if (tomatoUnlocked) {
				// --- TOMATO MODE (25s Total from Plowing) ---
				if (t->state != CROP_EMPTY && t->state != CROP_ROTTEN) {
					t->growTimer++;

					// At 17s (after 16s small plant phase), turns into Big Ripe Tomato Tree
					if (t->growTimer >= 17 && t->state == CROP_WATERED) {
						t->state = TOMATO_READY;
					}

					// Hard limit: Exactly 25s total timer -> Rots
					if (t->growTimer >= 25) {
						t->state = CROP_ROTTEN;
					}
				}
			}
			else {
				// --- ORIGINAL CROP PIPELINE ---
				if (t->state == CROP_WATERED) {
					t->growTimer++;
					if (t->growTimer >= 5) {
						t->state = CROP_READY;
					}
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
					if (s == CROP_SEEDED || s == CROP_WATERED || s == CROP_READY || s == TOMATO_READY) {
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
