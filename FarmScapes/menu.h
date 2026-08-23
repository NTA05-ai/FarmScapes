#ifndef MENU_H
#define MENU_H

inline void drawMenu() {
	iSetColor(255, 255, 255);
	iShowBMPAlternative(0, 0, "assets/menu_bg.bmp");

	iSetColor(20, 20, 20);
	iText(312, 538, "FARMSCAPES", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(255, 215, 0);
	iText(310, 540, "FARMSCAPES", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(255, 255, 255);
	iShowBMPAlternative2(300, 420, "assets/play.bmp", 0xFFFFFF);
	iShowBMPAlternative2(300, 330, "assets/settings.bmp", 0xFFFFFF);
	iShowBMPAlternative2(300, 240, "assets/credits.bmp", 0xFFFFFF);
	iShowBMPAlternative2(300, 150, "assets/exit.bmp", 0xFFFFFF);
}

#endif // MENU_H
