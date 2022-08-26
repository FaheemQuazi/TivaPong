#ifndef _PONG_CONTROLS_BUTTON_H_
#define _PONG_CONTROLS_BUTTON_H_

#include <stdlib.h>
#include <stdint.h>
#include "BSP.h"

typedef struct b {
    uint8_t pressed;
} Button;

Button * Button1Init(void);
Button * Button2Init(void);
void Button1Update(void);
void Button2Update(void); 

#endif
