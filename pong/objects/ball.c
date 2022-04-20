#include "ball.h"
#include "paddle.h"

void Ball_Init(Ball *b, int x, int y, int thickness) {
    // Ball Setup
    b->x = x;
    b->y = y;
    b->thickness = thickness;
    b->verticalDirection = -1;
    b->horizontalDirection = 0;
    b->speed = 1;

    // Physics Setup
    Physics_Body_Init(&(b->pbBall), &(b->x), &(b->y), thickness, thickness, PHYSICS_BODY_FLAG_GENERIC);
}

void Ball_AddCollisionCheck(Ball *b, Physics_Body *pb) {
    if (b->numRegColliders < PHYSICS_MAX_COLLIDERS) {
        b->pbOthers[b->numRegColliders++] = pb;
    }
}

void Ball_SetLocation(Ball *b, int x, int y) {
    BSP_LCD_FillRect(b->x - b->thickness / 2, b->y - b->thickness / 2, b->thickness, b->thickness, LCD_BLACK);
    b->x = x;
    b->y = y;
    BSP_LCD_FillRect(b->x - b->thickness / 2, b->y - b->thickness / 2, b->thickness, b->thickness, LCD_WHITE);
}

void Ball_Move(Ball *b) {
    
    BSP_LCD_FillRect(b->x - b->thickness / 2, b->y - b->thickness / 2, b->thickness, b->thickness, LCD_BLACK);

    // start by checking colliders
    for (int i = 0; i < b->numRegColliders; i++) {
        if (Physics_Body_CheckCollision(&b->pbBall, b->pbOthers[i])) {
            // we've hit something, figure out if it's horizontal or vertical
            // (*b->pbOthers[i]) - the thing you hit
            // square colliders won't work here
            if ((*b->pbOthers[i]).width > (*b->pbOthers[i]).height) {
                // this is a horizontal surface
                b->verticalDirection = b->verticalDirection * -1;
                // did we hit a paddle
                if ((*b->pbOthers[i]).flag == PHYSICS_BODY_FLAG_PADDLE) {
                    // paddle-specific bounce handling
                    // checks if ball hits left side of the paddle
                    if (((*(*b->pbOthers[i]).centerX) - ((*b->pbOthers[i]).width / 2)) < (b->x + (b->thickness / 2)) && b->x < ((*(*b->pbOthers[i]).centerX) - 1)){
                        b->horizontalDirection = -1;
                    } // checks if ball hits right side of the paddle
                    else if (((*(*b->pbOthers[i]).centerX) + ((*b->pbOthers[i]).width / 2)) > (b->x - (b->thickness / 2)) && b->x > ((*(*b->pbOthers[i]).centerX) + 1)){
                        b->horizontalDirection = 1;
                    } else {
                        b->horizontalDirection = 0;
                    }
                } else {
                    b->horizontalDirection = b->horizontalDirection * -1;
                }
                
            } else {
                // this is a vertical surface
                b->horizontalDirection = b->horizontalDirection * -1;
            }

            break;
        }
    }
    // move the ball
    b->x += (b->horizontalDirection * b->speed);
    b->y += (b->verticalDirection * b->speed);
    
    BSP_LCD_FillRect(b->x - b->thickness / 2, b->y - b->thickness / 2, b->thickness, b->thickness, LCD_WHITE);
}
