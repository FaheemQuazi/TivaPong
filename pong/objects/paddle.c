#include "paddle.h"


void Paddle_Init(Paddle *p, uint16_t color) {
    p->axis = 7;
    p->increment = 4;
    p->center = 64;
    p->width = 16;
    p->thickness = 3;
    p->color = color;
};


bool Paddle_CollisionLeft(Paddle *p) {
    return (p->center - (p->width / 2)) > 0;
};

bool Paddle_CollisionRight(Paddle *p) { 
    return (p->center + (p->width / 2)) < 128;
};

bool Paddle_MoveLeft(Paddle *p) {
    if (Paddle_CollisionLeft(p))
      p->center -= p->increment;

    return Paddle_CollisionLeft(p);
}

bool Paddle_MoveRight(Paddle *p) {
    if (Paddle_CollisionRight(p))
      p->center += p->increment;
      
    return Paddle_CollisionRight(p);
}
