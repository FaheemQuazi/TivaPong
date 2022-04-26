#include "paddle.h"

void Paddle_Init(Paddle *p, int axis, int increment, uint16_t color) {
    p->axis = axis;
    p->increment = increment;
    p->center = 64;
    p->width = 16;
    p->thickness = 4;
    p->color = color;

    // Physics Setup
    Physics_Body_Init(&(p->pbPaddle), &(p->center), &(p->axis), p->width, p->thickness, PHYSICS_BODY_FLAG_PADDLE);
};

void Paddle_Draw(Paddle *p) {
    for (int i = 0; i < p->thickness/2; i++) {
        BSP_LCD_DrawFastHLine(0, p->axis + i, 128, LCD_BLACK);
        BSP_LCD_DrawFastHLine(p->center - (p->width / 2), p->axis + i, p->width, p->color);
    }
    for (int i = 0; i < p->thickness/2; i++) {
        BSP_LCD_DrawFastHLine(0, p->axis - i, 128, LCD_BLACK);
        BSP_LCD_DrawFastHLine(p->center - (p->width / 2), p->axis - i, p->width, p->color);
    }
}

bool Paddle_CollisionLeft(Paddle *p) {
    return (p->center - (p->width / 2)) > 0;
};

bool Paddle_CollisionRight(Paddle *p) {
    return (p->center + (p->width / 2)) < 128;
};

bool Paddle_MoveLeft(Paddle *p) {
    // Checks to see if we can move
    if (Paddle_CollisionLeft(p))
        p->center -= p->increment;

    return Paddle_CollisionLeft(p);
}

bool Paddle_MoveRight(Paddle *p) {
    // Checks to see if we can move
    if (Paddle_CollisionRight(p))
        p->center += p->increment;

    return Paddle_CollisionRight(p);
}

void Paddle_Recenter(Paddle *p) {
    p->center = 64;
}
