#ifndef MENU_H
#define MENU_H

inline void drawMenu() {
	iSetColor(255, 255, 255);
	iShowBMPAlternative(0, 0, "assets/menu_bg.bmp");

	// Game Title Image (FARMSCAPES logo)
	iShowBMPAlternative2(200, 500, "assets/fs.bmp", 0xFFFFFF);

	// Menu Buttons
	iSetColor(255, 255, 255);
	iShowBMPAlternative2(300, 420, "assets/play.bmp", 0xFFFFFF);
	iShowBMPAlternative2(300, 330, "assets/settings.bmp", 0xFFFFFF);
	iShowBMPAlternative2(300, 240, "assets/credits.bmp", 0xFFFFFF);
	iShowBMPAlternative2(300, 150, "assets/exit.bmp", 0xFFFFFF);
}

#endif // MENU_H
