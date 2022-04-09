#include "joy.h"

JoyPos jp;
JoyPos* JoyInit(void) {
    jp.x = 512;
    jp.y = 512;
    jp.sel = 0;

    BSP_Joystick_Init();

    return &jp;
}

JoyPos JoyGetPosition() {
    return jp;
}

void ptJoyUpdate(void) {
    BSP_Joystick_Input(&jp.x, &jp.y, &jp.sel);
}

