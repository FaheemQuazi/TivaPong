#ifndef _PONG_CONTROLS_JOY_H_
#define _PONG_CONTROLS_JOY_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "BSP.h"

typedef struct jp {
    uint16_t x;
    uint16_t y;
    uint8_t sel;
} JoyPos;

JoyPos* JoyInit(void);
JoyPos JoyGetPosition(void);
void ptJoyUpdate(void);

#endif
