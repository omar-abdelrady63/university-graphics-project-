#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <cmath>
using namespace std;

enum GameState {
    MENU,
    PLAYING,
    WIN,
    GAME_OVER,
    CONTINUE_SCREEN
};

GameState gameState = MENU;

int startTime = 0;
int score = 0;
int totalCoins = 0;

float playerX = 380;
float playerY = 50;

float car1Y = 600;
float car2Y = 800;
float car3Y = 1000;

float car1X = 230;
float car2X = 380;
float car3X = 530;

float lanes[3] = { 230, 380, 530 };

int gameDuration = 5;
bool infiniteMode = false;

struct Coin {
    float x;
    float y;
    bool collected;
};

vector<Coin> coins;

void spawnCoins() {
    coins.clear();
    for (int i = 0; i < 5; i++) {
        Coin c;
        // هنا بنرسبن الكوينات

        c.x = lanes[rand() % 3] + 20;
        c.y = 150 + i * 120;
        c.collected = false;
        coins.push_back(c);
    }
}

void init() {
    glClearColor(1, 1, 1, 1);
    gluOrtho2D(0.0, 800.0, 0.0, 600.0);
    startTime = glutGet(GLUT_ELAPSED_TIME);
    srand(time(0));
}

void drawCar(float x, float y, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + 40, y);
    glVertex2f(x + 40, y + 60);
    glVertex2f(x, y + 60);
    glEnd();
}

void drawLane(float x_start) {
    glColor3f(0.3, 0.3, 0.3);
    glBegin(GL_QUADS);
    glVertex2f(x_start, 0);
    glVertex2f(x_start + 100, 0);
    glVertex2f(x_start + 100, 600);
    glVertex2f(x_start, 600);
    glEnd();
}

void drawCoin(float x, float y) {
    glColor3f(1, 0.84, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 20; i++) {
        float angle = 2.0f * 3.14159f * i / 20;
        glVertex2f(x + cos(angle) * 12, y + sin(angle) * 12);
    }
    glEnd();
    glColor3f(1, 0.65, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= 20; i++) {
        float angle = 2.0f * 3.14159f * i / 20;
        glVertex2f(x + cos(angle) * 8, y + sin(angle) * 8);
    }
    glEnd();
}

bool checkCarCollision(float carX, float carY) {
    return (playerX < carX + 40 &&
        playerX + 40 > carX &&
        playerY < carY + 60 &&
        playerY + 60 > carY);
}

bool checkCoinCollision(float coinX, float coinY) {
    float cx = coinX + 20;
    float cy = coinY + 30;
    float px = playerX + 20;
    float py = playerY + 30;
    float dist = sqrt((cx - px) * (cx - px) + (cy - py) * (cy - py));
    return dist < 35;
}

void drawText(float x, float y, string text) {
    glRasterPos2f(x, y);
    for (int i = 0; i < text.length(); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
    }
}

void resetGamePositions() {
    playerX = 380;
    playerY = 50;
    car1Y = 600;
    car2Y = 800;
    car3Y = 1000;
    car1X = 230;
    car2X = 380;
    car3X = 530;
    spawnCoins();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (gameState == MENU) {
        glColor3f(0, 0, 0);
        drawText(200, 400, "Select Time:");
        drawText(200, 360, "1) 5 sec  2) 10 sec  3) 30 sec");
        drawText(200, 320, "4) Infinite Mode");
        glColor3f(0.8, 0.6, 0);
        drawText(200, 280, "Coins: " + to_string(totalCoins));
    }
    else if (gameState == PLAYING) {
        drawLane(200);
        drawLane(350);
        drawLane(500);

        drawCar(car1X, car1Y, 1, 0, 0);
        drawCar(car2X, car2Y, 1, 1, 0);
        drawCar(car3X, car3Y, 0.5, 0, 0.5);

        for (auto& c : coins) {
            if (!c.collected) {
                drawCoin(c.x, c.y);
            }
        }

        drawCar(playerX, playerY, 0, 0, 1);

        glColor3f(0, 0, 0);
        drawText(10, 570, "Score: " + to_string(score));
        glColor3f(0.8, 0.6, 0);
        drawText(10, 540, "Coins: " + to_string(totalCoins));
        if (infiniteMode) {
            drawText(10, 510, "Mode: Infinite");
        }
    }
    else if (gameState == WIN) {
        glColor3f(0, 1, 0);
        drawText(350, 300, "YOU WIN!");
        drawText(350, 250, "Score: " + to_string(score));
        glColor3f(0.8, 0.6, 0);
        drawText(350, 200, "Coins: " + to_string(totalCoins));
    }
    else if (gameState == GAME_OVER) {
        glColor3f(1, 0, 0);
        drawText(330, 320, "GAME OVER");
        drawText(330, 270, "Score: " + to_string(score));
        glColor3f(0.8, 0.6, 0);
        drawText(330, 220, "Coins: " + to_string(totalCoins));
        // هنا عملت فكره طرش محصلش شكلها حلو انك لو جمعت 5 كوين تقدر تكمل وبسكورك عادي عشان نلاقي لازمه للحاجات اليوزليس دي
        if (totalCoins >= 5) {
            glColor3f(0, 0, 1);
            drawText(250, 170, "Press C to Continue (5 coins)");
        }
        glColor3f(0, 0, 0);
        drawText(300, 120, "Press Y for Menu");
        drawText(300, 90, "Press N to Exit");
    }
    else if (gameState == CONTINUE_SCREEN) {
        glColor3f(0, 1, 0);
        drawText(300, 350, "CONTINUE!");
        glColor3f(0, 0, 0);
        drawText(250, 300, "You used 5 coins to continue");
        glColor3f(0.8, 0.6, 0);
        drawText(300, 250, "Coins left: " + to_string(totalCoins));
        glColor3f(0, 0, 0);
        drawText(250, 200, "Press S to start again");
        // مخليتهوش يبدا علطول عشان وانا بجرب مع نفسي كنت ممكن اسرح لحظه والاقي نفسي بادئ وعربيه جايه في وشي
    }

    glFlush();
}

void update(int value) {
    if (gameState == PLAYING) {
        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        int elapsed = (currentTime - startTime) / 1000;

        if (!infiniteMode && elapsed >= gameDuration) {
            gameState = WIN;
        }

        car1Y -= 8.0;
        car2Y -= 6.5;
        car3Y -= 10.0;

        if (car1Y < -60) {
            car1Y = 600;
            car1X = lanes[rand() % 3];
        }

        if (car2Y < -60) {
            car2Y = 800;
            car2X = lanes[rand() % 3];
        }

        if (car3Y < -60) {
            car3Y = 1000;
            car3X = lanes[rand() % 3];
        }

        for (auto& c : coins) {
            if (!c.collected && checkCoinCollision(c.x, c.y)) {
                c.collected = true;
                score += 10;
                totalCoins++;
            }
            c.y -= 3.0;
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

void keyboard(unsigned char key, int, int) {
    if (gameState == MENU) {
        if (key == '1') {
            gameDuration = 5;
            infiniteMode = false;
            gameState = PLAYING;
            startTime = glutGet(GLUT_ELAPSED_TIME);
            score = 0;
            resetGamePositions();
        }
        if (key == '2') {
            gameDuration = 10;
            infiniteMode = false;
            gameState = PLAYING;
            startTime = glutGet(GLUT_ELAPSED_TIME);
            score = 0;
            resetGamePositions();
        }
        if (key == '3') {
            gameDuration = 30;
            infiniteMode = false;
            gameState = PLAYING;
            startTime = glutGet(GLUT_ELAPSED_TIME);
            score = 0;
            resetGamePositions();
        }
        if (key == '4') {
            infiniteMode = true;
            gameState = PLAYING;
            startTime = glutGet(GLUT_ELAPSED_TIME);
            score = 0;
            resetGamePositions();
        }
    }

    if (gameState == PLAYING) {
        if (key == 'a' || key == 'A' && playerX > 200)
            playerX -= 150;
        if (key == 'd' || key == 'D' && playerX < 500)
            playerX += 150;
    }

    if (gameState == GAME_OVER) {
        if (key == 'c' || key == 'C') {
            if (totalCoins >= 5) {
                totalCoins -= 5;
                gameState = CONTINUE_SCREEN;
            }
        }
        if (key == 'y' || key == 'Y') {
            gameState = MENU;
            score = 0;
            resetGamePositions();
        }
        if (key == 'n' || key == 'N') {
            exit(0);
        }
    }

    if (gameState == CONTINUE_SCREEN) {
        if (key == 's' || key == 'S') {
            gameState = PLAYING;
            startTime = glutGet(GLUT_ELAPSED_TIME);
            resetGamePositions();
        }
    }

    if (gameState == WIN) {
        if (key == 'y' || key == 'Y') {
            gameState = MENU;
            score = 0;
            resetGamePositions();
        }
        if (key == 'n' || key == 'N') {
            exit(0);
        }
    }

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Car Game");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0, update, 0);
    glutMainLoop();
}
