/*
 _________  ___  ___      ___ ________  ________  ________  ________   ________     
|\___   ___\\  \|\  \    /  /|\   __  \|\   __  \|\   __  \|\   ___  \|\   ____\    
\|___ \  \_\ \  \ \  \  /  / | \  \|\  \ \  \|\  \ \  \|\  \ \  \\ \  \ \  \___|    
     \ \  \ \ \  \ \  \/  / / \ \   __  \ \   ____\ \  \\\  \ \  \\ \  \ \  \  ___  
      \ \  \ \ \  \ \    / /   \ \  \ \  \ \  \___|\ \  \\\  \ \  \\ \  \ \  \|\  \ 
       \ \__\ \ \__\ \__/ /     \ \__\ \__\ \__\    \ \_______\ \__\\ \__\ \_______\
        \|__|  \|__|\|__|/       \|__|\|__|\|__|     \|_______|\|__| \|__|\|_______|
                                                                                    
                                                                                    
                                                                                    

  Movement:
  - Player can only move the pad left-right

  Orientation:
  - Game will be displayed in vertical instead of horizontal


  ╔════════════════════════════════╗
  ║                ========        ║
  ║                                ║
  ║                                ║
  ║                                ║
  ║                                ║
  ║                                ║
  ║ 1                           1  ║
  ║                                ║
  ║             o                  ║
  ║                                ║
  ║                                ║
  ║                                ║
  ║                                ║
  ║    ========                    ║
  ╚════════════════════════════════╝

*/


#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "BSP.h"
#include "CortexM.h"
#include "os.h"

#define THREADFREQ 1000   // frequency in Hz of round robin scheduler

int main(void){
  // OS_Init();
  BSP_Button1_Init();
  BSP_Button2_Init();
  BSP_Joystick_Init();
  BSP_Buzzer_Init(1024);
  BSP_LCD_Init();
  BSP_LCD_FillScreen(BSP_LCD_Color565(0, 0, 0));

  const int paddleAxis = 6;
  const int paddleIncrement = 3;
  int paddleStart = 6;
  int paddleEnd = 24;

  uint16_t joyX, joyY;
  uint8_t joySel;

  BSP_LCD_DrawFastHLine(5, paddleAxis-2, 123, LCD_WHITE);

  while (true) {

    BSP_LCD_DrawFastHLine(0, paddleAxis, 128, LCD_BLACK);
    BSP_LCD_DrawFastHLine(0, paddleAxis+1, 128, LCD_BLACK);
    BSP_LCD_DrawFastHLine(0, paddleAxis+2, 128, LCD_BLACK);

    BSP_LCD_DrawFastHLine(paddleStart, paddleAxis, 16, LCD_WHITE);
    BSP_LCD_DrawFastHLine(paddleStart, paddleAxis+1, 16, LCD_WHITE);
    BSP_LCD_DrawFastHLine(paddleStart, paddleAxis+2, 16, LCD_WHITE);

    BSP_Joystick_Input(&joyX, &joyY, &joySel);
    if (joyX > 750 && paddleEnd <= 123) {
      paddleStart = paddleStart + paddleIncrement;
      paddleEnd = paddleEnd + paddleIncrement;
    } else if (joyX < 250 && paddleStart >= 5) {
      paddleStart = paddleStart - paddleIncrement;
      paddleEnd = paddleEnd - paddleIncrement;
    }

    // BSP_LCD_SetCursor(0, 6);
    // BSP_LCD_OutUDec(joyX, LCD_WHITE);
    // BSP_LCD_SetCursor(0, 8);
    // BSP_LCD_OutUDec(joyY, LCD_WHITE);
    // BSP_LCD_SetCursor(0, 10);
    // BSP_LCD_OutUDec(joySel, LCD_WHITE);    


    BSP_Delay1ms(10);
  }
  
  // when grading change 1000 to 4-digit number from edX
  // OS_Launch(BSP_Clock_GetFreq() / THREADFREQ); // doesn't return, interrupts enabled in here
}
