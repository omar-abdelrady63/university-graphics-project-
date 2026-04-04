#include <GL/glut.h>

void init() {


    gluOrtho2D(0.0, 800.0, 0.0, 600.0);
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT);


    drawLane(200);
    drawLane(350);
    drawLane(500);


    drawCar(380, 50, 0.0, 0.0, 1.0);


    drawCar(230, 450, 1.0, 0.0, 0.0);
    drawCar(530, 300, 1.0, 1.0, 0.0);
    drawCar(380, 500, 0.5, 0.0, 0.5);

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Advanced Road Prototype");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
