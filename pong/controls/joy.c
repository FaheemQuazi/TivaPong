#include "joy.h"


JoyPos j;
JoyPos* JoyInit(void) {
    j.x = 512;
    j.y = 512;
    j.sel = 0;

    BSP_Joystick_Init();
    return &j;
}

void ptJoyUpdate(void) {
    BSP_Joystick_Input(&j.x, &j.y, &j.sel);
}


