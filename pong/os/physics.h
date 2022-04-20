#ifndef _PONG_OS_PHYSICS_H_
#define _PONG_OS_PHYSICS_H_

#include <stdint.h>
#include <stdbool.h>
#include "BSP.h"

#define PHYSICS_MAX_COLLIDERS 8

#define PHYSICS_BODY_FLAG_GENERIC 0
#define PHYSICS_BODY_FLAG_PADDLE 1
#define PHYSICS_BODY_FLAG_SCORE 2

typedef struct phys_body {
    int *centerX;
    int *centerY;
    uint32_t width;
    uint32_t height;
    int flag;
} Physics_Body;

void Physics_Body_Init(Physics_Body *bod, int *centerX, int *centerY, uint32_t width, uint32_t height, int flag);
bool Physics_Body_CheckCollision(Physics_Body *b1, Physics_Body *b2);

#endif
