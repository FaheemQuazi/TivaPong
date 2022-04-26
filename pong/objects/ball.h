#ifndef _PONG_OBJECTS_BALL_H_
#define _PONG_OBJECTS_BALL_H_

#include <stdbool.h>
#include <stdint.h>
#include "BSP.h"
#include "physics.h"
#include "paddle.h"

// Ball
//   int ballX = 64;
//   int ballY = 60;
//   int ballThickness = 4;
//   int bounce = 1;
//   int bounceSideLeft = 0;
//   int bounceSideRight = 0;
//   bool goal = false;
//   int leftWallBounceDown = 0;
//   int rightWallBounceDown = 0;

typedef struct bl {
    int x;
    int y;
    int prevX;
    int prevY;
    int thickness;
    int verticalDirection;
    int horizontalDirection;
    int speed;
    int color;
    Physics_Body pbBall;
    Physics_Body *pbOthers[PHYSICS_MAX_COLLIDERS];
    int numRegColliders;
} Ball;

void Ball_Init(Ball *b, int x, int y, int thickness);
void Ball_AddCollisionCheck(Ball *b, Physics_Body *pb);
void Ball_SetLocation(Ball *b, int x, int y);
void Ball_SetDirection(Ball *b, int xDirection, int yDirection);
void Ball_ChangeColor(Ball *b, int c);
bool Ball_Move(Ball *b);
void Ball_Draw(Ball *b);

#endif
