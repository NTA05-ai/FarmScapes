#ifndef AUDIO_H
#define AUDIO_H

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

// Access the global musicOn variable declared in iMain.cpp
extern int musicOn;

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

#endif // AUDIO_H
