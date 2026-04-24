#include <GL/glut.h>
#include <cstdlib>
#include <ctime>
#include <string>

enum GameState {
    MENU,
    PLAYING,
    WIN,
    GAME_OVER
};

GameState gameState = MENU;

int startTime = 0;

//  اللاعب
float playerX = 380;
float playerY = 50;

//  العربيات
float car1Y = 600;
float car2Y = 800;
float car3Y = 1000;

float car1X = 230;
float car2X = 380;
float car3X = 530;

float lanes[3] = { 230, 380, 530 };

bool gameOver = false;
int gameDuration = 5; // default


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

// اختبار التصادم
bool checkCollision(float carX, float carY) {
    return (playerX < carX + 40 &&
        playerX + 40 > carX &&
        playerY < carY + 60 &&
        playerY + 60 > carY);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);


    if (gameState == MENU) {
        glColor3f(0, 0, 0);
        glRasterPos2f(200, 350);

        const char* text = "Select Time: 1) 5 sec  2) 10 sec  3) 30 sec";
        for (int i = 0; text[i]; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
        }
    }

    // PLAYING
    else if (gameState == PLAYING) {

        drawLane(200);
        drawLane(350);
        drawLane(500);

        drawCar(car1X, car1Y, 1, 0, 0);
        drawCar(car2X, car2Y, 1, 1, 0);
        drawCar(car3X, car3Y, 0.5, 0, 0.5);

        drawCar(playerX, playerY, 0, 0, 1);
    }

    // WIN
    else if (gameState == WIN) {
        glColor3f(0, 1, 0);
        glRasterPos2f(350, 300);

        const char* text = "YOU WIN!";
        for (int i = 0; text[i]; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
        }
    }

    // GAME OVER
    else if (gameState == GAME_OVER) {
        glColor3f(1, 0, 0);
        glRasterPos2f(330, 300);

        const char* text = "GAME OVER";
        for (int i = 0; text[i]; i++) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
        }
    }

    glFlush();
}

void update(int value) {

    if (gameState == PLAYING) {

        int currentTime = glutGet(GLUT_ELAPSED_TIME);
        int elapsed = (currentTime - startTime) / 1000;

        // WIN
        if (elapsed >= gameDuration) {
            gameState = WIN;
        }
        car1Y -= 8.0;
        car2Y -= 6.5;
        car3Y -= 10.0;



        if (car1Y < -60) {
            car1Y = 600; // 600 دا اقصي ارتفاع الشاشه عشان تنزل فوق
            car1X = lanes[rand() % 3];
        }
        //-60 عشان تبقي خرجت كلها من الشاشه

        if (car2Y < -60) {
            car2Y = 800;
            car2X = lanes[rand() % 3];
        }

        if (car3Y < -60) {
            car3Y = 1000;
            car3X = lanes[rand() % 3];
        }
        // بنغير احيانا 600  او 800 او 1000 عشان ميلزقوش ف بعض مره واحده و يخبطوا ف بعض زي المتخاذلين



        //  خسارة عشان متخاذل زي ناس
        if (checkCollision(car1X, car1Y) ||
            checkCollision(car2X, car2Y) ||
            checkCollision(car3X, car3Y)) {

            gameState = GAME_OVER;
        }
    }

    glutPostRedisplay();

    // هنا بنحدد الـ Frame Rate (مثلاً 16ms تعادل تقريباً 60 فريم في الثانية)
    glutTimerFunc(16, update, 0);
}


void keyboard(unsigned char key, int, int) {

    // MENU
    if (gameState == MENU) {

        if (key == '1') {
            gameDuration = 5;
            gameState = PLAYING;
            startTime = glutGet(GLUT_ELAPSED_TIME);
        }

        if (key == '2') {
            gameDuration = 10;
            gameState = PLAYING;
            startTime = glutGet(GLUT_ELAPSED_TIME);
        }

        if (key == '3') {
            gameDuration = 30;
            gameState = PLAYING;
            startTime = glutGet(GLUT_ELAPSED_TIME);
        }
    }

    // movement
    if (gameState == PLAYING) {

        if (key == 'a' && playerX > 200)
            playerX -= 150;

        if (key == 'd' && playerX < 500)
            playerX += 150;
    }

    // restart
    if (gameState == WIN || gameState == GAME_OVER) {

        if (key == 'y') {

            gameState = MENU;

            playerX = 380;

            car1Y = 600;
            car2Y = 800;
            car3Y = 1000;
        }

        if (key == 'n') {
            exit(0);
        }
    }

    glutPostRedisplay();
}


int main(int argc, char** argv) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Car Game - Enum Version");

    init();

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(0,update, 0);
    glutMainLoop();
}
