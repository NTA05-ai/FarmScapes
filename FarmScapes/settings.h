#ifndef SETTINGS_H
#define SETTINGS_H

// Access musicOn defined in iMain.cpp
extern int musicOn;

void drawSettings() {
	iSetColor(255, 255, 255);
	iShowBMPAlternative(0, 0, "assets/menu_bg.bmp");

	// Title
	iShowBMPAlternative2(300, 520, "assets/settings.bmp", 0xFFFFFF);

	// Music Button (Rendered at Y = 350)
	iSetColor(255, 255, 255);
	if (musicOn) {
		iShowBMPAlternative2(300, 350, "assets/mon.bmp", 0xFFFFFF);
	} else {
		iShowBMPAlternative2(300, 350, "assets/moff.bmp", 0xFFFFFF);
	}

	// Back Button (Rendered at Y = 230)
	iShowBMPAlternative2(300, 230, "assets/exit.bmp", 0xFFFFFF);
}

#endif // SETTINGS_H
