#include "main.h"
#include <glut.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <Windows.h>
#include <glaux.h>

#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")


#define _USE_MATH_DEFINES
#include <math.h>

#define WIDTH 1200
#define HEIGHT 740

float cameraX = 1.0f;
float cameraY = 1.5f;
float cameraZ = 1.0;

float lookAtX = 0.0f;
float lookAtY = -0.5f;
float lookAtZ = 11.0f;

int centerX = WIDTH / 2;
int centerY = HEIGHT / 2;

float yaw = -90.0f;
float a = 1;
float b = 0;

float playerX = 1.0f;
float playerY = 0.0f;
float playerZ = 1.0f;

float prevCameraX, prevCameraY, prevCameraZ;
float prevLookAtX, prevLookAtY, prevLookAtZ;

float playerSpeed = 0.1f;

int isGKeyPressed = 0;
int item1Eaten = 0;
int item2Eaten = 0;
int clearGame = 0;

time_t startTime;
double clearTime = 0;
double getItem1Time = 0;
double getItem2Time = 0;
double elapsedTime;

GLfloat lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
GLfloat lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat lightPosition[] = { MAP_WIDTH, 5.0f, MAP_HEIGHT, 1.0f };

int maze[MAP_HEIGHT][MAP_WIDTH];

const int DIR[4][2] = { {0, -2}, {0, 2}, {-2, 0}, {2, 0} };

unsigned int ids[1];
AUX_RGBImageRec* tex[1];

double getElapsedTime() {
    clock_t currentTime = clock();
    return ((double)currentTime) / CLOCKS_PER_SEC;
}

void drawSnowman() {
    // ´«»ç¶÷ ¸öÅë
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(playerX, playerY + 0.5, playerZ);
    glutSolidSphere(0.3, 20, 20);
    glPopMatrix();

    // ´«»ç¶÷ ¸Ó¸®
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(playerX, playerY + 0.9, playerZ);
    glutSolidSphere(0.2, 20, 20);
    glPopMatrix();

    // ´«»ç¶÷ ´«
    glColor3f(0.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(playerX - 0.1, playerY + 0.95, playerZ + 0.15);
    glutSolidSphere(0.05, 10, 10);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(playerX + 0.1, playerY + 0.95, playerZ + 0.15);
    glutSolidSphere(0.05, 10, 10);
    glPopMatrix();

    // ´«»ç¶÷ ÀÔ
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
    glTranslatef(playerX, playerY + 0.85, playerZ + 0.2);
    glutSolidSphere(0.05, 10, 10);
    glPopMatrix();
}

void setupLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
}


void shuffleArray(int array[], int size) {
    int i, r, temp;

    for (i = 0; i < (size - 1); ++i) {
        r = i + (rand() % (size - i));
        temp = array[i];
        array[i] = array[r];
        array[r] = temp;
    }
}

int inRange(int y, int x) {
    return (y < MAP_HEIGHT - 1 && y > 0) && (x < MAP_WIDTH - 1 && x > 0);
}

void generateMap(int y, int x, int maze[MAP_HEIGHT][MAP_WIDTH]) {
    int i, nx, ny;
    int directions[4] = { DIRECTION_UP, DIRECTION_RIGHT, DIRECTION_DOWN, DIRECTION_LEFT };

    maze[y][x] = MAP_FLAG_VISITED;

    shuffleArray(directions, 4);

    for (i = 0; i < 4; i++) {
        nx = x + DIR[directions[i]][0];
        ny = y + DIR[directions[i]][1];

        if (inRange(ny, nx) && maze[ny][nx] == MAP_FLAG_WALL) {
            generateMap(ny, nx, maze);

            if (ny != y)
                maze[(ny + y) / 2][x] = MAP_FLAG_EMPTY;
            else
                maze[y][(x + nx) / 2] = MAP_FLAG_EMPTY;
            if (rand() % 20 == 0) { 
                maze[ny][nx] = MAP_FLAG_ITEM1;
            }
            else if (rand() % 15 == 0) { 
                maze[ny][nx] = MAP_FLAG_ITEM2;
            }
            else {
                maze[ny][nx] = MAP_FLAG_EMPTY;
            }
        }
    }

    maze[0][0] = MAP_FLAG_STARTPOINT;
    maze[1][0] = MAP_FLAG_STARTPOINT;
    maze[0][1] = MAP_FLAG_STARTPOINT;
    maze[MAP_HEIGHT - 1][MAP_WIDTH - 1] = MAP_FLAG_DESTINATION;
    maze[MAP_HEIGHT - 2][MAP_WIDTH - 1] = MAP_FLAG_DESTINATION;
    maze[MAP_HEIGHT - 1][MAP_WIDTH - 2] = MAP_FLAG_DESTINATION;
}

void updateCameraDirection() {
    float yawRad = yaw * 0.0174533f;

    lookAtX = cos(yawRad);
    lookAtZ = sin(yawRad);

    cameraX = playerX - lookAtX * 0.5;
    cameraZ = playerZ - lookAtZ * 0.5;

    if (item1Eaten) {
        cameraY = playerY + 6;
    }
    else {
        cameraY = playerY + 1.5;
    }

    if (isGKeyPressed) {
        cameraX = MAP_WIDTH / 2;
        cameraY = 34;
        cameraZ = MAP_HEIGHT / 2;
        lookAtX = 0;
        lookAtY = -15;
        lookAtZ = 0;
        a = 0;
        b = 1;
    }
}

void drawText(char* text, float x, float y) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);

    glRasterPos2f(x, y);

    int length = strlen(text);
    for (int i = 0; i < length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, text[i]);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void drawTime(double elapsedTime) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    glColor3f(1.0f, 1.0f, 1.0f);

    glRasterPos2f(WIDTH * 0.45, HEIGHT * 0.1);

    char timeString[20];
    sprintf_s(timeString, sizeof(timeString), "Time: %.2fs", elapsedTime);

    int length = strlen(timeString);
    for (int i = 0; i < length; i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, timeString[i]);
    }
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void createCylinder(GLfloat radius, GLfloat h) {
    GLfloat x, z, angle;

    glBegin(GL_TRIANGLE_FAN);           //¿ø±âµÕÀÇ À­¸é
    glNormal3f(0.0f, 0.0f, -1.0f);
    glColor3ub(139, 69, 19);

    for (angle = 0.0f; angle < (2.0f * M_PI); angle += (M_PI / 8.0f))
    {
        x = radius * sin(angle);
        z = radius * cos(angle);
        glNormal3f(0.0f, 0.0f, -1.0f);
        glVertex3f(x, 0, z);
    }
    glEnd();

    glBegin(GL_QUAD_STRIP);
    for (angle = 0.0f; angle < (2.0f * M_PI); angle += (M_PI / 8.0f))
    {
        x = radius * sin(angle);
        z = radius * cos(angle);
        glNormal3f(sin(angle), cos(angle), 0.0f);
        glVertex3f(x, 0, z);
        glVertex3f(x, h, z);
    }
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(0.0f, 0.0f, 1.0f);
    for (angle = (2.0f * M_PI); angle > 0.0f; angle -= (M_PI / 8.0f))
    {
        x = radius * sin(angle);
        z = radius * cos(angle);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(x, h, z);
    }
    glEnd();
}

void drawMaze() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    GLfloat plane_coef_s[] = { 1.0, 0.0, 0.0, 1.0 };
    GLfloat plane_coef_t[] = { 0.0, 1.0, 0.0, 1.0 };
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_GEN_S);
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_S, GL_OBJECT_PLANE, plane_coef_s);

    glEnable(GL_TEXTURE_GEN_T);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGenfv(GL_T, GL_OBJECT_PLANE, plane_coef_t);


    if (!isGKeyPressed) {
        prevCameraX = cameraX;
        prevCameraY = cameraY;
        prevCameraZ = cameraZ;
        prevLookAtX = lookAtX;
        prevLookAtY = lookAtY;
        prevLookAtZ = lookAtZ;
        a = 1;
        b = 0;
    }

    gluLookAt(
        cameraX, cameraY, cameraZ,
        cameraX + lookAtX, cameraY + lookAtY, cameraZ + lookAtZ,
        0.0, a, b
    );

    setupLighting();

    glPushMatrix();
    glTranslatef(0, 35, 0);
    glDisable(GL_LIGHTING); 
    glBindTexture(GL_TEXTURE_2D, ids[0]); 
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0); glVertex3f(-100, 0, -100);
    glTexCoord2f(1.0, 0.0); glVertex3f(100, 0, -100);
    glTexCoord2f(1.0, 1.0); glVertex3f(100, 0, 100);
    glTexCoord2f(0.0, 1.0); glVertex3f(-100, 0, 100);
    glEnd();
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopMatrix();

    glColor3f(0.8f, 0.8f, 0.8f); 
    glBegin(GL_QUADS);
    glVertex3f(0.0f, 0.0f, 0.0f);
    glVertex3f(MAP_WIDTH, 0.0f, 0.0f);
    glVertex3f(MAP_WIDTH, 0.0f, MAP_HEIGHT);
    glVertex3f(0.0f, 0.0f, MAP_HEIGHT);
    glEnd();

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            if (maze[i][j] == MAP_FLAG_WALL) {
                glColor3f(0.2f, 0.2f, 0.2f);

                for (int k = 0; k < 2; k++) {
                    glPushMatrix();
                    glTranslatef(j, 0.5 + k * 1.1, i);
                    glutSolidCube(1.0);
                    glPopMatrix();
                }
            }
            else if (maze[i][j] == MAP_FLAG_DESTINATION) {
                glColor3f(0.0f, 1.0f, 0.0f);

                for (int k = 0; k < 2; k++) {
                    glPushMatrix();
                    glTranslatef(j, 0.5 + k * 1.1, i);
                    glutSolidCube(1.0);
                    glPopMatrix();
                }
            }
            else if (maze[i][j] == MAP_FLAG_STARTPOINT) {
                glColor3f(1.0f, 0.0f, 0.0f);

                for (int k = 0; k < 2; k++) {
                    glPushMatrix();
                    glTranslatef(j, 0.5 + k * 1.1, i);
                    glutSolidCube(1.0);
                    glPopMatrix();
                }
            }
            else if (maze[i][j] == MAP_FLAG_ITEM1) {
                glColor3f(1.0f, 1.0f, 1.0f); 
                glPushMatrix();
                glTranslatef(j, 0.5, i);
                glScalef(1.0, 2.0, 1.0);
                glutSolidCube(0.5);
                glPopMatrix();
                glDisable(GL_TEXTURE_2D);
                if (item1Eaten == 0 && abs(playerX - j) < 1 && abs(playerZ - i) < 1) {
                    item1Eaten = 1;
                    getItem1Time = getElapsedTime() - startTime;
                }
            }
            else if (maze[i][j] == MAP_FLAG_ITEM2) {
                glColor3f(0.5f, 0.25f, 0.1f);
                glPushMatrix();
                glTranslatef(j, 0, i);
                createCylinder(0.25, 1);
                glPopMatrix();

                if (item2Eaten == 0 && abs(playerX - j) < 1 && abs(playerZ - i) < 1) {
                    item2Eaten = 1; 
                    playerSpeed *= 3; 
                    getItem2Time = getElapsedTime() - startTime;
                }
            }
            else {
                glColor3f(1.0f, 1.0f, 1.0f); 
            }
        }
    }
    if (clearTime == 0 && (abs(playerX - (MAP_WIDTH - 1.1)) < 1) && (abs(playerZ - (MAP_HEIGHT - 1.1)) < 1)) {
        clearGame = 1;
        clearTime = getElapsedTime();
        printf("%f\n", clearTime);
    }

    updateCameraDirection();
    drawSnowman();

    if (clearTime != 0) {
        char result1[100];
        sprintf_s(result1, sizeof(result1), "%s %.2fs", "Congratulations! You cleared the game! Your record : ", clearTime);
        drawText(result1, WIDTH * 0.25f, HEIGHT * 0.4f);
        char result2[100];
        sprintf_s(result2, sizeof(result2), "Press ESC to exit the game");
        drawText(result2, WIDTH * 0.375f, HEIGHT * 0.5f);
    }


    if (!clearGame) {
        elapsedTime = getElapsedTime() - startTime;
    }
    drawTime(elapsedTime);

    printf("%f %f\n", elapsedTime - getItem2Time, playerSpeed);
    if (item1Eaten == 1 && elapsedTime - getItem1Time > 5) {
        item1Eaten = 0;
    }
    if (item2Eaten == 1 && elapsedTime - getItem2Time > 5) {
        item2Eaten = 0;
        playerSpeed /= 3;
    }

    glutSwapBuffers();
}   

int isCollision(float x, float z) {
    int mazeX = (int)floor(x + 0.5);
    int mazeZ = (int)floor(z + 0.5);

    int max_snowmanX = (int)floor(x + 0.2 + 0.5);
    int min_snowmanX = (int)floor(x - 0.2 + 0.5);
    int max_snowmanZ = (int)floor(z + 0.2 + 0.5);
    int min_snowmanZ = (int)floor(z - 0.2 + 0.5);

    if (maze[max_snowmanZ][mazeX] == MAP_FLAG_WALL || maze[min_snowmanZ][mazeX] == MAP_FLAG_WALL || maze[mazeZ][max_snowmanX] == MAP_FLAG_WALL || maze[mazeZ][min_snowmanX] == MAP_FLAG_WALL) {
        return 1;
    }
    if (maze[max_snowmanZ][mazeX] == MAP_FLAG_STARTPOINT || maze[min_snowmanZ][mazeX] == MAP_FLAG_STARTPOINT || maze[mazeZ][max_snowmanX] == MAP_FLAG_STARTPOINT || maze[mazeZ][min_snowmanX] == MAP_FLAG_STARTPOINT) {
        return 1;
    }
    if (maze[max_snowmanZ][mazeX] == MAP_FLAG_DESTINATION || maze[min_snowmanZ][mazeX] == MAP_FLAG_DESTINATION || maze[mazeZ][max_snowmanX] == MAP_FLAG_DESTINATION || maze[mazeZ][min_snowmanX] == MAP_FLAG_DESTINATION) {
        return 1;
    }

    return 0;
}


void movePlayer(float speed) {
    float newPlayerX = playerX + speed * lookAtX;
    float newPlayerZ = playerZ + speed * lookAtZ;

    if (!isCollision(newPlayerX, newPlayerZ)) {
        playerX = newPlayerX;
        playerZ = newPlayerZ;
    }
}

void movePlayerSide(float speed) {
    float newPlayerX = playerX + speed * lookAtZ;
    float newPlayerZ = playerZ - speed * lookAtX;

    if (!isCollision(newPlayerX, newPlayerZ)) {
        playerX = newPlayerX;
        playerZ = newPlayerZ;
    }
}

void keyboardHandler(unsigned char key, int x, int y) {
    switch (key) {
    case 'w':
        movePlayer(playerSpeed);
        break;
    case 's':
        movePlayer(-playerSpeed);
        break;
    case 'a':
        movePlayerSide(playerSpeed);
        break;
    case 'd':
        movePlayerSide(-playerSpeed);
        break;
    case 'g':
        isGKeyPressed = !isGKeyPressed;
        break;
    case 27:
        exit(0);
        break;
    }
    glutPostRedisplay();
}

void handleMouseMotion(int x, int y) {
    int deltaX = x - centerX + 8;
    int deltaY = y - centerY + 31;

    if (deltaX != 0) {
        yaw += deltaX * 0.1f;
    }

    if (deltaY != 0) {
        lookAtY -= deltaY * 0.001f;
        if (lookAtY > 2.0f) {
            lookAtY = 1.0f;
        }
        else if (lookAtY < -2.0f) {
            lookAtY = -1.0f;
        }
    }

    updateCameraDirection();

    SetCursorPos(centerX, centerY);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("3D Maze Game");

    glEnable(GL_DEPTH_TEST);

    tex[0] = auxDIBImageLoad(L"background.bmp");

    for (int i = 0; i < 1; i++) {
        glGenTextures(2, &ids[i]);
        glBindTexture(GL_TEXTURE_2D, ids[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, 3, tex[i]->sizeX, tex[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, tex[i]->data);
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    }

    srand((unsigned int)time(NULL));
    memset(maze, MAP_FLAG_WALL, sizeof(maze));
    generateMap(1, 1, maze);

    glutIdleFunc(drawMaze);
    glutDisplayFunc(drawMaze);
    glutKeyboardFunc(keyboardHandler);
    glutPassiveMotionFunc(handleMouseMotion);

    glMatrixMode(GL_PROJECTION);
    gluPerspective(60.0, 1.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);

    startTime = getElapsedTime();
    glutMainLoop();
    return 0;
}
