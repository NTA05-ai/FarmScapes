#ifndef LOADING_H
#define LOADING_H

void drawLoading() {
    // 1. Green Background
	iSetColor(34, 139, 34);
    iFilledRectangle(0, 0, 800, 600);

    // 2. Loading Title Banner Image (480 x 80 px)
    iSetColor(255, 255, 255);
    iShowBMPAlternative2(160, 480, "assets/lc_title.bmp", 0xFFFFFF);

    // 3. How to Play Instruction Box Image (600 x 360 px)
    iShowBMPAlternative2(100, 90, "assets/instrc.bmp", 0xFFFFFF);

    // 4. Click Anywhere to Start Prompt Image (400 x 50 px)
    iShowBMPAlternative2(200, 25, "assets/start.bmp", 0xFFFFFF);
}

#endif // LOADING_H
