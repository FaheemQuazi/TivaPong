#ifndef _PONG_OBJECTS_PADDLE_H_
#define _PONG_OBJECTS_PADDLE_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct pdl {
    int axis;
    int increment;
    int center;
    int thickness;
    int width;
    uint16_t color;
} Paddle;

void Paddle_Init(Paddle *p, uint16_t color); 

bool Paddle_MoveRight(Paddle *p);
bool Paddle_MoveLeft(Paddle *p);

#endif
