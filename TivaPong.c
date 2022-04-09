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

// Pong
#include "pong/controls/joy.h"
#include "pong/objects/paddle.h"

#define THREADFREQ 1000   // frequency in Hz of round robin scheduler

int main(void){
  BSP_LCD_Init();
  BSP_LCD_FillScreen(LCD_BLACK);

  // Controls
  JoyPos *jp = JoyInit();
 
  // Paddles
  Paddle player1;
  Paddle_Init(&player1, LCD_WHITE);

  // Ball
  int ballX = 64;
  int ballY = 60;
  int ballThickness = 4;
  int bounce = 1;
  int bounceSideLeft = 0;
  int bounceSideRight = 0;
  bool goal = false;

  // Border around the playing field
  BSP_LCD_DrawFastHLine(0, 2, 128, LCD_CYAN);

  while (true) {
    // Update vars
    ptJoyUpdate();

    // Condition where paddle hits right side
    if (jp->x > 750) Paddle_MoveRight(&player1);
      
    // Condition where paddle hits left side
    if (jp->x < 250) Paddle_MoveLeft(&player1);

    // draw the paddle
    for (int i = 0; i < player1.thickness; i++) {
      BSP_LCD_DrawFastHLine(0, player1.axis+i, 128, LCD_BLACK);
      BSP_LCD_DrawFastHLine(player1.center-(player1.width/2), player1.axis+i, player1.width, player1.color);
    }

    if (goal == false){
      BSP_LCD_FillRect(ballX-ballThickness/2, ballY-ballThickness/2, ballThickness, ballThickness, LCD_BLACK);
      if (bounce == 1){
        ballY--;
        // if ball hits player1 paddle, bounce off
        if (ballY == (player1.axis+player1.thickness)+1 && player1.center-2 < ballX && ballX < player1.center+2){
          bounce--;
        }
        // if player1 misses ball, other player scores
        else if(ballY < (player1.axis+player1.thickness)+1) {
          goal = true;
        }
        // if ball bounces off the right side of player1 paddle, bounce to the right at 45 degrees
        else if (ballY == (player1.axis+player1.thickness)+1 && player1.center-(player1.width/2) < (ballX + (ballThickness/2)) && ballX < player1.center-2){
          bounceSideLeft++;
          bounce--;
        }
        // if ball bounces off the left side of player1 paddle, bounce to the left at 45 degrees
        else if (ballY == (player1.axis+player1.thickness)+1 && player1.center+2 < ballX && (ballX - (ballThickness/2)) < player1.center+(player1.width/2)){
          bounceSideRight++;
          bounce--;
        }
      }
      else if (bounce == 0 && bounceSideRight != 1 && bounceSideLeft != 1){
        ballY++;
        if (ballY == 64){
          bounce++;
        }
      }
      else if (bounceSideLeft == 1){
        ballY++;
        ballX--;
      }
      else if (bounceSideRight == 1){
        ballY++;
        ballX++;
      }
      BSP_LCD_FillRect(ballX-ballThickness/2, ballY-ballThickness/2, ballThickness, ballThickness, LCD_WHITE);
    }

    BSP_Delay1ms(10);
  }
  
  // when grading change 1000 to 4-digit number from edX
  // OS_Launch(BSP_Clock_GetFreq() / THREADFREQ); // doesn't return, interrupts enabled in here
}
