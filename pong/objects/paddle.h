#ifndef _PONG_OBJECTS_PADDLE_H_
#define _PONG_OBJECTS_PADDLE_H_

#include <stdbool.h>
#include <stdint.h>
#include "BSP.h"
#include "physics.h"

typedef struct pdl {
    int axis;
    int increment;
    int center;
    int thickness;
    int width;
    Physics_Body pbPaddle;
    uint16_t color;
} Paddle;

void Paddle_Init(Paddle *p, int axis, int increment, uint16_t color);
void Paddle_Score(Paddle *p);

bool Paddle_MoveRight(Paddle *p);
bool Paddle_MoveLeft(Paddle *p);

void Paddle_Draw(Paddle *p);

#endif
