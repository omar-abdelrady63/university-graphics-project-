#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <cmath>
using namespace std;
enum GameState { MENU, PLAYING, WIN, GAME_OVER, CONTINUE_SCREEN };
GameState gameState = MENU;
float lanes[3] = { 230, 380, 530 };
int   currentLane = 1;
float playerX     = 380;
float playerY     = 50;
float car1X = 230, car1Y = 600;
float car2X = 380, car2Y = 800;
float car3X = 530, car3Y = 1000;
int   score       = 0;
int   totalCoins  = 0;
int   startTime   = 0;
int   gameDuration = 5;
bool  infiniteMode = false;
float roadOffset = 0;
struct Coin { float x, y; bool collected; };
vector<Coin> coins;

void spawnCoins() {
    coins.clear();
    for (int i = 0; i < 5; i++) {
        Coin c;
        c.x = lanes[rand() % 3] + 20;
        c.y = 150 + i * 120;
        c.collected = false;
        coins.push_back(c);
    }
}

void resetGamePositions() {
    currentLane = 1;
    playerX = lanes[currentLane];
    playerY = 50;
    car1Y = 600;  car1X = lanes[rand() % 3];
    car2Y = 800;  car2X = lanes[rand() % 3];
    car3Y = 1000; car3X = lanes[rand() % 3];
    spawnCoins();
}

void init() {
    glClearColor(0.08f, 0.08f, 0.12f, 1);
    gluOrtho2D(0.0, 800.0, 0.0, 600.0);
    startTime = glutGet(GLUT_ELAPSED_TIME);
    srand((unsigned)time(0));
}
void drawRoundedRect(float x, float y, float w, float h, float r) {
    const int segs = 8;
    glBegin(GL_QUADS);
        glVertex2f(x+r,   y);       glVertex2f(x+w-r, y);
        glVertex2f(x+w-r, y+h);     glVertex2f(x+r,   y+h);
        glVertex2f(x,     y+r);     glVertex2f(x+r,   y+r);
        glVertex2f(x+r,   y+h-r);   glVertex2f(x,     y+h-r);
        glVertex2f(x+w-r, y+r);     glVertex2f(x+w,   y+r);
        glVertex2f(x+w,   y+h-r);   glVertex2f(x+w-r, y+h-r);
    glEnd();
    float cx[4] = {x+r,     x+w-r, x+w-r, x+r    };
    float cy[4] = {y+r,     y+r,   y+h-r, y+h-r  };
    float sa[4] = {180,     270,   0,     90     };
    for (int c = 0; c < 4; c++) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx[c], cy[c]);
        for (int i = 0; i <= segs; i++) {
            float a = (sa[c] + i * 90.0f / segs) * 3.14159f / 180.0f;
            glVertex2f(cx[c] + cosf(a)*r, cy[c] + sinf(a)*r);
        }
        glEnd();
    }
}

void drawPanel(float x, float y, float w, float h, float alpha) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.05f, 0.05f, 0.1f, alpha);
    glBegin(GL_QUADS);
        glVertex2f(x,   y);   glVertex2f(x+w, y);
        glVertex2f(x+w, y+h); glVertex2f(x,   y+h);
    glEnd();
    glDisable(GL_BLEND);
}

void drawSeparator(float x, float y, float w) {
    glColor3f(0.35f, 0.35f, 0.55f);
    glBegin(GL_QUADS);
        glVertex2f(x,   y);   glVertex2f(x+w, y);
        glVertex2f(x+w, y+2); glVertex2f(x,   y+2);
    glEnd();
}

void drawText(float x, float y, const string& text) {
    glRasterPos2f(x, y);
    for (char ch : text) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, ch);
}

void drawMiniCoin(float x, float y) {
    glColor3f(1.0f, 0.82f, 0.1f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 16; i++) {
        float a = 2.0f * 3.14159f * i / 16;
        glVertex2f(x + cosf(a)*7, y + sinf(a)*7);
    }
    glEnd();
}

void drawCar(float x, float y, float r, float g, float b) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.25f);
    drawRoundedRect(x+4, y-4, 40, 60, 6);
    glDisable(GL_BLEND);
    glColor3f(r, g, b);
    drawRoundedRect(x, y, 40, 60, 6);
    glColor3f(fminf(r+0.35f,1.f), fminf(g+0.35f,1.f), fminf(b+0.35f,1.f));
    drawRoundedRect(x+6, y+32, 28, 18, 4);
    glColor3f(1.f, 1.f, 0.7f);
    drawRoundedRect(x+5,  y+4, 8, 5, 2);
    drawRoundedRect(x+27, y+4, 8, 5, 2);
}

void drawPlayerCar(float x, float y) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.25f);
    drawRoundedRect(x+4, y-4, 40, 60, 6);
    glDisable(GL_BLEND);
    glColor3f(0.15f, 0.45f, 1.0f);
    drawRoundedRect(x, y, 40, 60, 6);
    glColor3f(0.5f, 0.75f, 1.0f);
    drawRoundedRect(x+6, y+32, 28, 18, 4);
    glColor3f(0.9f, 0.95f, 1.0f);
    drawRoundedRect(x+5,  y+4, 8, 5, 2);
    drawRoundedRect(x+27, y+4, 8, 5, 2);
    glColor3f(0.0f, 0.85f, 1.0f);
    drawRoundedRect(x+10, y+50, 20, 5, 2);
}

void drawLane(float x_start) {
    glColor3f(0.18f, 0.18f, 0.22f);
    glBegin(GL_QUADS);
        glVertex2f(x_start,     0); glVertex2f(x_start+100, 0);
        glVertex2f(x_start+100, 600); glVertex2f(x_start, 600);
    glEnd();
    glColor3f(0.28f, 0.28f, 0.33f);
    glBegin(GL_QUADS);
        glVertex2f(x_start,   0); glVertex2f(x_start+3, 0);
        glVertex2f(x_start+3, 600); glVertex2f(x_start, 600);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(x_start+97, 0); glVertex2f(x_start+100, 0);
        glVertex2f(x_start+100, 600); glVertex2f(x_start+97, 600);
    glEnd();
}

void drawDashedLines() {
    glColor3f(0.9f, 0.85f, 0.3f);
    const float dashH = 30, gapH = 20, period = dashH + gapH;
    float divX[2] = { 348, 498 };
    for (int d = 0; d < 2; d++) {
        float startY = fmodf(roadOffset, period) - period;
        for (float yy = startY; yy < 600; yy += period) {
            glBegin(GL_QUADS);
                glVertex2f(divX[d],   yy);        glVertex2f(divX[d]+4, yy);
                glVertex2f(divX[d]+4, yy+dashH);  glVertex2f(divX[d],   yy+dashH);
            glEnd();
        }
    }
}

void drawCoin(float x, float y) {
    glColor3f(1.0f, 0.82f, 0.1f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 24; i++) {
        float a = 2.0f * 3.14159f * i / 24;
        glVertex2f(x + cosf(a)*12, y + sinf(a)*12);
    }
    glEnd();
    glColor3f(0.85f, 0.6f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 24; i++) {
        float a = 2.0f * 3.14159f * i / 24;
        glVertex2f(x + cosf(a)*8, y + sinf(a)*8);
    }
    glEnd();
    glColor3f(1.0f, 0.95f, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x-2, y+2);
    for (int i = 0; i <= 12; i++) {
        float a = 2.0f * 3.14159f * i / 12;
        glVertex2f(x-2 + cosf(a)*4, y+2 + sinf(a)*4);
    }
    glEnd();
}

bool checkCarCollision(float carX, float carY) {
    return (playerX     < carX + 40 &&
            playerX + 40 > carX     &&
            playerY     < carY + 60 &&
            playerY + 60 > carY);
}

bool checkCoinCollision(float coinX, float coinY) {
    float dx = coinX - (playerX + 20);
    float dy = coinY - (playerY + 30);
    return sqrtf(dx*dx + dy*dy) < 35;
}

void drawMenuBackground() {
    for (int i = 0; i < 10; i++) {
        float t = i / 10.0f;
        glColor3f(0.05f + t*0.08f, 0.05f + t*0.05f, 0.15f + t*0.1f);
        glBegin(GL_QUADS);
            glVertex2f(0,   i*60);     glVertex2f(800, i*60);
            glVertex2f(800, (i+1)*60); glVertex2f(0,   (i+1)*60);
        glEnd();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    if (gameState == MENU) {
        drawMenuBackground();
        drawPanel(150, 430, 500, 130, 0.7f);
        glColor3f(0.3f, 0.8f, 1.0f);
        drawText(240, 520, "=   CAR  DODGER   =");
        glColor3f(0.6f, 0.6f, 0.8f);
        drawText(205, 480, "Dodge cars. Collect coins. Survive.");
        drawSeparator(160, 460, 480);
        drawPanel(150, 240, 500, 185, 0.6f);
        glColor3f(0.9f, 0.9f, 0.9f);
        drawText(200, 400, "Select Game Duration:");
        glColor3f(0.4f, 0.9f, 0.5f);
        drawText(200, 368, "[ 1 ]  5 seconds    [ 2 ]  10 seconds");
        drawText(200, 338, "[ 3 ]  30 seconds   [ 4 ]  Infinite");
        drawSeparator(160, 320, 480);
        drawMiniCoin(207, 294);
        glColor3f(1.0f, 0.82f, 0.1f);
        drawText(220, 288, "Total Coins: " + to_string(totalCoins));
        drawPanel(150, 180, 500, 50, 0.5f);
        glColor3f(0.5f, 0.5f, 0.7f);
        drawText(195, 198, "Controls:  [ A ] = Left      [ D ] = Right");
    }

    else if (gameState == PLAYING) {
        glColor3f(0.12f, 0.12f, 0.16f);
        glBegin(GL_QUADS);
            glVertex2f(0,0); glVertex2f(800,0);
            glVertex2f(800,600); glVertex2f(0,600);
        glEnd();
        glColor3f(0.1f, 0.22f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(0,0);   glVertex2f(200,0);
            glVertex2f(200,600); glVertex2f(0,600);
        glEnd();
        glBegin(GL_QUADS);
            glVertex2f(600,0); glVertex2f(800,0);
            glVertex2f(800,600); glVertex2f(600,600);
        glEnd();

        for (int i = 0; i < 7; i++) {
            glColor3f((i%2)?0.85f:0.15f, 0.1f, 0.1f);
            glBegin(GL_QUADS);
                glVertex2f(195, i*90); glVertex2f(205, i*90);
                glVertex2f(205, i*90+45); glVertex2f(195, i*90+45);
            glEnd();
            glBegin(GL_QUADS);
                glVertex2f(595, i*90); glVertex2f(605, i*90);
                glVertex2f(605, i*90+45); glVertex2f(595, i*90+45);
            glEnd();
        }
        drawLane(200); drawLane(350); drawLane(500);
        drawDashedLines();
        drawCar(car1X, car1Y, 0.95f, 0.2f,  0.2f);
        drawCar(car2X, car2Y, 0.9f,  0.7f,  0.1f);
        drawCar(car3X, car3Y, 0.55f, 0.1f,  0.85f);
        for (auto& c : coins)
            if (!c.collected) drawCoin(c.x, c.y);
        drawPlayerCar(playerX, playerY);
        drawPanel(0, 565, 800, 35, 0.82f);
        glColor3f(0.3f, 0.8f, 1.0f);  drawText(12,  578, "SCORE");
        glColor3f(1.f,  1.f,  1.f);   drawText(75,  578, to_string(score));
        drawMiniCoin(150, 584);
        glColor3f(1.f, 0.82f, 0.1f);  drawText(163, 578, to_string(totalCoins));
        if (infiniteMode) {
            glColor3f(0.5f, 1.0f, 0.5f);
            drawText(610, 578, "INFINITE MODE");
        } else {
            int elapsed    = (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000;
            int remaining  = gameDuration - elapsed;
            if (remaining < 0) remaining = 0;
            bool urgent = remaining <= 3;
            glColor3f(urgent ? 1.0f : 0.8f, urgent ? 0.3f : 0.9f, 0.3f);
            drawText(650, 578, "TIME: " + to_string(remaining) + "s");
        }
    }
    else if (gameState == WIN) {
        drawMenuBackground();
        drawPanel(200, 200, 400, 240, 0.85f);
        glColor3f(0.3f, 1.0f, 0.5f);
        drawText(305, 405, "YOU  WIN!");
        drawSeparator(210, 385, 380);
        glColor3f(0.9f, 0.9f, 0.9f);
        drawText(245, 355, "Final Score:  " + to_string(score));
        drawMiniCoin(252, 320);
        glColor3f(1.f, 0.82f, 0.1f);
        drawText(265, 314, "Coins:  " + to_string(totalCoins));
        drawSeparator(210, 294, 380);
        glColor3f(0.5f, 0.5f, 0.7f);
        drawText(220, 264, "[ Y ] Back to Menu     [ N ] Exit");
    }else if (gameState == GAME_OVER) {
        drawMenuBackground();
        drawPanel(180, 160, 440, 280, 0.88f);
        glColor3f(1.0f, 0.3f, 0.3f);
        drawText(295, 410, "GAME  OVER");
        drawSeparator(190, 390, 420);
        glColor3f(0.9f, 0.9f, 0.9f);
        drawText(225, 360, "Score:  " + to_string(score));
        drawMiniCoin(232, 328);
        glColor3f(1.f, 0.82f, 0.1f);
        drawText(245, 322, "Coins:  " + to_string(totalCoins));
        drawSeparator(190, 305, 420);
        if (totalCoins >= 5) {
            glColor3f(0.3f, 0.7f, 1.0f);
            drawText(210, 274, "[ C ] Continue  (-5 coins)");
        }
        glColor3f(0.6f, 0.9f, 0.6f);
        drawText(210, 242, "[ Y ] Back to Menu");
        glColor3f(0.8f, 0.4f, 0.4f);
        drawText(210, 210, "[ N ] Exit Game");
    }else if (gameState == CONTINUE_SCREEN) {
        drawMenuBackground();
        drawPanel(180, 210, 440, 200, 0.88f);
        glColor3f(0.3f, 1.0f, 0.5f);
        drawText(275, 385, "CONTINUE!");
        drawSeparator(190, 365, 420);
        glColor3f(0.9f, 0.9f, 0.9f);
        drawText(210, 335, "5 coins spent to continue.");
        drawMiniCoin(217, 303);
        glColor3f(1.f, 0.82f, 0.1f);
        drawText(230, 297, "Coins remaining:  " + to_string(totalCoins));
        drawSeparator(190, 277, 420);
        glColor3f(0.4f, 0.8f, 1.0f);
        drawText(240, 247, "[ S ] Start Again");
    }
    glFlush();
}

void update(int value) {
    if (gameState == PLAYING) {
        int elapsed = (glutGet(GLUT_ELAPSED_TIME) - startTime) / 1000;
        if (!infiniteMode && elapsed >= gameDuration) {
            gameState = WIN;
        }
        roadOffset += 4.5f;
        car1Y -= 8.0f;
        car2Y -= 6.5f;
        car3Y -= 10.0f;
        if (car1Y < -60) { car1Y = 600;  car1X = lanes[rand() % 3]; }
        if (car2Y < -60) { car2Y = 800;  car2X = lanes[rand() % 3]; }
        if (car3Y < -60) { car3Y = 1000; car3X = lanes[rand() % 3]; }
        for (auto& c : coins) {
            if (!c.collected && checkCoinCollision(c.x, c.y)) {
                c.collected = true;
                score      += 10;
                totalCoins++;
            }
            c.y -= 3.0f;
            if (c.y < -50) {
                c.y = 650;
                c.x = lanes[rand() % 3] + 20;
                c.collected = false;
            }
        }
        if (checkCarCollision(car1X, car1Y) ||
            checkCarCollision(car2X, car2Y) ||
            checkCarCollision(car3X, car3Y)) {
            gameState = GAME_OVER;
        }
    }
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void startGame() {
    gameState = PLAYING;
    startTime = glutGet(GLUT_ELAPSED_TIME);
    score = 0;
    resetGamePositions();
}

void keyboard(unsigned char key, int, int) {
    if (gameState == MENU) {
        if (key == '1') { gameDuration = 5;  infiniteMode = false; startGame(); }
        if (key == '2') { gameDuration = 10; infiniteMode = false; startGame(); }
        if (key == '3') { gameDuration = 30; infiniteMode = false; startGame(); }
        if (key == '4') { infiniteMode = true;                     startGame(); }
    }
    if (gameState == PLAYING) {
        if ((key == 'a' || key == 'A') && currentLane > 0) {
            currentLane--;
            playerX = lanes[currentLane];
        }
        if ((key == 'd' || key == 'D') && currentLane < 2) {
            currentLane++;
            playerX = lanes[currentLane];
        }
    }
    if (gameState == GAME_OVER) {
        if ((key == 'c' || key == 'C') && totalCoins >= 5) {
            totalCoins -= 5;
            gameState = CONTINUE_SCREEN;
        }
        if (key == 'y' || key == 'Y') { gameState = MENU; score = 0; resetGamePositions(); }
        if (key == 'n' || key == 'N') exit(0);
    }
    if (gameState == CONTINUE_SCREEN) {
        if (key == 's' || key == 'S') {
            gameState = PLAYING;
            startTime = glutGet(GLUT_ELAPSED_TIME);
            resetGamePositions();
        }
    }
    if (gameState == WIN) {
        if (key == 'y' || key == 'Y') { gameState = MENU; score = 0; resetGamePositions(); }
        if (key == 'n' || key == 'N') exit(0);
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Car Dodger");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
}
