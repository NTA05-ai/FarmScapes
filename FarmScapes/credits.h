#ifndef CREDITS_H
#define CREDITS_H

void drawCredits() {
    // 1. Background
    iSetColor(255, 255, 255);
    iShowBMPAlternative(0, 0, "assets/menu_bg.bmp");

    // 2. Credits Title
    iShowBMPAlternative2(300, 520, "assets/credits.bmp", 0xFFFFFF);

    // 3. Large Wooden Board Frame (X: 120, Y: 240, Width: 560, Height: 200)
    iSetColor(255, 255, 255);
    iShowBMPAlternative2(120, 230, "assets/board.bmp", 0xFFFFFF);

    // 4. Names Inside the Board
    iSetColor(30, 20, 10);
    iText(160, 380, "Md. Azizur Rahman Ragib, ID:00725105101090", GLUT_BITMAP_HELVETICA_18);
    iText(160, 330, "Nabiha Tahsin Anika, ID:00725105101099", GLUT_BITMAP_HELVETICA_18);
    iText(160, 280, "Nadira Fairuza, ID:00725105101118", GLUT_BITMAP_HELVETICA_18);

    // 5. Exit Button
    iSetColor(255, 255, 255);
    iShowBMPAlternative2(300, 140, "assets/exit.bmp", 0xFFFFFF);
}

#endif // CREDITS_H
