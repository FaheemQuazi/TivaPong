#include "button.h"

Button b1, b2;
Button * Button1Init(void) {
    BSP_Button1_Init();
    return &b1;
}

void Button1Update(void) {
    uint8_t pressed = BSP_Button1_Input();
    b1.pressed = pressed;
}

Button * Button2Init(void) {
    BSP_Button2_Init();
    return &b2;
}

void Button2Update(void) {
    uint8_t pressed = BSP_Button2_Input();
    b2.pressed = pressed;
}
