#include "physics.h"

void Physics_Body_Init(Physics_Body *bod, int *centerX, int *centerY, uint32_t width, uint32_t height, int flag){
    bod->centerX = centerX;
    bod->centerY = centerY;
    bod->width = width;
    bod->height = height;
    bod->flag = flag;
}

bool Physics_Body_CheckCollision(Physics_Body *b1, Physics_Body *b2){
    return ((*b1->centerX) - b1->width/2 < (*b2->centerX) + b2->width/2 &&   //  leftmost X1 < rightmost X2
        (*b1->centerX) + b1->width/2 > (*b2->centerX) - b2->width/2 &&       // rightmost X1 > leftmost X2
        (*b1->centerY) - b1->height/2 < (*b2->centerY) + b2->height/2 &&     //  leftmost Y1 < rightmost Y2
        b1->height/2 + (*b1->centerY) > (*b2->centerY) - b2->height/2);      // rightmost Y1 > leftmost Y2
}
