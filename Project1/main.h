#ifndef MAIN_H
#define MAIN_H

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


#define WIDTH 1200
#define HEIGHT 740

// 크기는 홀수여야함
#define MAP_WIDTH 31
#define MAP_HEIGHT 21

typedef enum _Direction {
    DIRECTION_LEFT,
    DIRECTION_UP,
    DIRECTION_RIGHT,
    DIRECTION_DOWN
} Direction;

typedef enum _MapFlag {
    MAP_FLAG_WALL,
    MAP_FLAG_EMPTY,
    MAP_FLAG_VISITED,
    MAP_FLAG_DESTINATION,
    MAP_FLAG_STARTPOINT,
    MAP_FLAG_ITEM1,
    MAP_FLAG_ITEM2
} MapFlag;

extern float cameraX;
extern float cameraY;
extern float cameraZ;

extern float lookAtX;
extern float lookAtY;
extern float lookAtZ;

extern int lastMouseX;
extern int lastMouseY;

extern float yaw;
extern float a;
extern float b;

extern float playerX;
extern float playerY;
extern float playerZ;

extern int isGKeyPressed;

extern GLfloat sunRadius;
extern GLfloat sunColor[];

extern int maze[MAP_HEIGHT][MAP_WIDTH];

extern const int DIR[4][2];

void drawPlayer();

void shuffleArray(int array[], int size);
int inRange(int y, int x);
void generateMap(int y, int x, int maze[MAP_HEIGHT][MAP_WIDTH]);

void updateCameraDirection();
void drawMaze();
void drawGhost();
void drawPlayer();

int isCollision(float x, float z);

void movePlayer(float speed);
void movePlayerSide(float speed);

void keyboardHandler(unsigned char key, int x, int y);

#endif // MAIN_H
