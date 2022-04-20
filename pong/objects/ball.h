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

typedef struct b {
    int x;
    int y;
    int thickness;
    int verticalDirection;
    int horizontalDirection;
    int speed;
    Physics_Body pbBall;
    Physics_Body *pbOthers[PHYSICS_MAX_COLLIDERS];
    int numRegColliders;
} Ball;

void Ball_Init(Ball *b, int x, int y, int thickness);
void Ball_AddCollisionCheck(Ball *b, Physics_Body *pb);
void Ball_SetLocation(Ball *b, int x, int y);
void Ball_Move(Ball *b);

#endif
