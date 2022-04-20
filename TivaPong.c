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
#include "pong/objects/ball.h"
#include "physics.h"

#define THREADFREQ 1000   // frequency in Hz of round robin scheduler

int main(void){
    BSP_LCD_Init();
    BSP_LCD_FillScreen(LCD_BLACK);

    // Controls
    JoyPos *jp = JoyInit();

    // Playfield (Vertical)
    int WallLX = 2, WallRX = 125, WallY = 64;
    Physics_Body pbWallL;
    Physics_Body pbWallR;
    Physics_Body_Init(&pbWallL, &WallLX, &WallY, 1, 128, PHYSICS_BODY_FLAG_GENERIC);
    Physics_Body_Init(&pbWallR, &WallRX, &WallY, 1, 128, PHYSICS_BODY_FLAG_GENERIC);

    // Playfield (Horizontal/Scoring)
    int WallP1Y = 1, WallP2Y = 127, WallPX = 64;
    Physics_Body pbWallP1;
    Physics_Body pbWallP2;
    Physics_Body_Init(&pbWallP1, &WallPX, &WallP1Y, 128, 1, PHYSICS_BODY_FLAG_SCORE);
    Physics_Body_Init(&pbWallP2, &WallPX, &WallP2Y, 128, 1, PHYSICS_BODY_FLAG_SCORE);

    // Paddles
    Paddle player1;
    Paddle player2;
    Paddle_Init(&player1, 7, 4, LCD_WHITE);
    Paddle_Init(&player2, 121, 2, LCD_GREEN);

    // Ball
    Ball gameBall;
    Ball_Init(&gameBall, 64, 64, 4);
    Ball_AddCollisionCheck(&gameBall, &player1.pbPaddle);
    Ball_AddCollisionCheck(&gameBall, &player2.pbPaddle);
    Ball_AddCollisionCheck(&gameBall, &pbWallL);
    Ball_AddCollisionCheck(&gameBall, &pbWallR);

    while (true) {
        BSP_LCD_DrawFastVLine(WallLX, 0, 128, LCD_CYAN);
        BSP_LCD_DrawFastVLine(WallRX, 0, 128, LCD_CYAN);
        BSP_LCD_DrawFastHLine(0, WallP1Y, 128, LCD_CYAN); 
        BSP_LCD_DrawFastHLine(0, WallP2Y, 128, LCD_CYAN); 

        // Update vars
        ptJoyUpdate();

        // Condition where paddle hits right side
        if (jp->x > 750) Paddle_MoveRight(&player1);

        // Condition where paddle hits left side
        if (jp->x < 250) Paddle_MoveLeft(&player1);

        // move player2 based on ball position
        if (gameBall.x > player2.center + player2.width/2 - 1) Paddle_MoveRight(&player2);
        if (gameBall.x < player2.center - player2.width/2 - 1) Paddle_MoveLeft(&player2);

        // draw the paddle
        Paddle_Draw(&player1);
        Paddle_Draw(&player2);

        // move and draw the ball
        Ball_Move(&gameBall);

        if (Physics_Body_CheckCollision(&pbWallP1, &gameBall.pbBall)) {
            // P2 Score
            Paddle_Score(&player2);
            Ball_SetLocation(&gameBall, 64, 64);
        } else if (Physics_Body_CheckCollision(&pbWallP2, &gameBall.pbBall)) {
            // P1 Score
            Paddle_Score(&player1);
            Ball_SetLocation(&gameBall, 64, 64);
        }
        
        // display score
        BSP_LCD_SetCursor(1, 6);
        BSP_LCD_OutUDec(player1.score, LCD_WHITE);
        BSP_LCD_SetCursor(19, 6);
        BSP_LCD_OutUDec(player2.score, LCD_WHITE);

        BSP_Delay1ms(8);
    }

    // OS_Launch(BSP_Clock_GetFreq() / THREADFREQ); // doesn't return, interrupts enabled in here
}
