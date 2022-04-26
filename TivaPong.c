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
#include "gameOS.h"

// Pong
#include "pong/controls/joy.h"
#include "pong/controls/button.h"
#include "pong/objects/paddle.h"
#include "pong/objects/ball.h"
#include "physics.h"

#define THREADFREQ 1000   // frequency in Hz of round robin scheduler

// Global Variables
Paddle player1; // P1 Paddle
Paddle player2; // P2 Paddle
Ball gameBall;  // Game Ball

JoyPos *stick;
Button *B1, *B2;

int WallLX = 2, WallRX = 125, WallY = 64;
Physics_Body pbWallL;
Physics_Body pbWallR;

int WallP1Y = 1, WallP2Y = 127, WallPX = 64;
Physics_Body pbWallP1;
Physics_Body pbWallP2;

uint32_t p1Score = 0;
uint32_t p2Score = 0;

int32_t semaBuzzer;

void Playfield_Init(Ball* gb) {
    // Playfield (Vertical)
    Physics_Body_Init(&pbWallL, &WallLX, &WallY, 1, 128, PHYSICS_BODY_FLAG_GENERIC);
    Physics_Body_Init(&pbWallR, &WallRX, &WallY, 1, 128, PHYSICS_BODY_FLAG_GENERIC);

    // Playfield (Horizontal/Scoring)
    Physics_Body_Init(&pbWallP1, &WallPX, &WallP1Y, 128, 3, PHYSICS_BODY_FLAG_SCORE);
    Physics_Body_Init(&pbWallP2, &WallPX, &WallP2Y, 128, 3, PHYSICS_BODY_FLAG_SCORE);
    
    // Ball Collision
    Ball_AddCollisionCheck(&gameBall, &pbWallL);
    Ball_AddCollisionCheck(&gameBall, &pbWallR);
}

void Playfield_ScoreCheck() {
    while (true) { 
        if (Physics_Body_CheckCollision(&pbWallP1, &gameBall.pbBall)) {
            // P2 Score
            p2Score += 1;
            Ball_SetLocation(&gameBall, 64, 64);
            Ball_SetDirection(&gameBall, 0, 1);
            Paddle_Recenter(&player1);
            Paddle_Recenter(&player2);
        } else if (Physics_Body_CheckCollision(&pbWallP2, &gameBall.pbBall)) {
            // P1 Score 
            p1Score += 1;
            Ball_SetLocation(&gameBall, 64, 64);
            Ball_SetDirection(&gameBall, 0, -1);
            Paddle_Recenter(&player1);
            Paddle_Recenter(&player2);
        }
    }
}

void Playfield_ResetScore() {
    p1Score = 0;
    p2Score = 0;
    Ball_SetLocation(&gameBall, 64, 64);
    Ball_SetDirection(&gameBall, 0, -1);
    Paddle_Recenter(&player1);
    Paddle_Recenter(&player2);
}

void Draw(void) {
    while (true) {
        // Draw player paddles
        Paddle_Draw(&player1);
        Paddle_Draw(&player2);

        // Draw ball
        Ball_Draw(&gameBall);

        // Draw Score
        BSP_LCD_SetCursor(1, 6);
        BSP_LCD_OutUDec(p1Score, LCD_WHITE);
        BSP_LCD_SetCursor(19, 6);
        BSP_LCD_OutUDec(p2Score, LCD_WHITE);

        // Draw Field
        BSP_LCD_DrawFastVLine(WallLX, 0, 128, LCD_CYAN);
        BSP_LCD_DrawFastVLine(WallRX, 0, 128, LCD_CYAN);
        BSP_LCD_DrawFastHLine(0, WallP1Y, 128, LCD_CYAN); 
        BSP_LCD_DrawFastHLine(0, WallP2Y, 128, LCD_CYAN); 
        gOS_Sleep(16);
    }
}

uint32_t randM = 1231451;
void Physics(void) {
    while(true) {
        // randomly delay the Player 2 AI Control
        randM = 1664525*randM+1013904223;
        if (randM % 100 > 47) {
            // Move player2 paddle
            if (gameBall.x > player2.center + player2.width/2 - 1) Paddle_MoveRight(&player2);
            if (gameBall.x < player2.center - player2.width/2 - 1) Paddle_MoveLeft(&player2);
        }

        // Condition where paddle hits right side
        if (stick->x > 750) Paddle_MoveRight(&player1);

        // Condition where paddle hits left side
        if (stick->x < 250) Paddle_MoveLeft(&player1);

        // Move Ball
        bool hasHit = Ball_Move(&gameBall);
        
        // if ball hits something, buzzer
        if (hasHit) {
            gOS_Signal(&semaBuzzer);
        }
        
        gOS_Sleep(32);
    }
}

void Control(void) {
    
    while (true) {
        ptJoyUpdate();
        gOS_Sleep(8);
    }
}

int previousState = 1;
void State(void) {

    while (true) {

        Button1Update();
        Button2Update();

        if (B1->pressed == 0 && previousState == 1) {
            previousState = 0;
            gOS_Lock(&Control);
            gOS_Lock(&Physics);
            Ball_ChangeColor(&gameBall, LCD_RED);
            gOS_Sleep(200);
        } else if (B1->pressed == 0 && previousState == 0) {
            previousState = 1;
            gOS_Unlock(&Control);
            gOS_Unlock(&Physics);
            Ball_ChangeColor(&gameBall, LCD_WHITE);
            gOS_Sleep(200);
        }

        if (B2->pressed == 0) {
            Playfield_ResetScore();
            gOS_Sleep(200);
        }

        gOS_Sleep(20);
    }

}

void Sound(void) {
    while (true) {
        gOS_Wait(&semaBuzzer);
        BSP_Buzzer_Set(512);
        gOS_Sleep(20);
        BSP_Buzzer_Set(0);
        gOS_Sleep(20);
    }
}

int main(void){
    
    // OS Stuff
    gOS_Init();

    // Clear the screen
    BSP_LCD_Init();
    BSP_LCD_FillScreen(LCD_BLACK);

    // Initialize Joystick
    stick = JoyInit();

    // Initialize Buzzer Stuff
    BSP_Buzzer_Init(0);
    gOS_InitSemaphore(&semaBuzzer, 0);

    // Initialize Button1 and Button2
    B1 = Button1Init();
    B2 = Button2Init();
    
    // Initialize Paddles
    Paddle_Init(&player1, 7, 2, LCD_WHITE);
    Paddle_Init(&player2, 121, 2, LCD_GREEN);

    // Initialize Ball
    Ball_Init(&gameBall, 64, 64, 4);
    Ball_AddCollisionCheck(&gameBall, &player1.pbPaddle);
    Ball_AddCollisionCheck(&gameBall, &player2.pbPaddle);

    // Initialize Playfield
    Playfield_Init(&gameBall);

    void (*ths[NUMTHREADS])(void) = {&Control, &Physics, &Draw, &Playfield_ScoreCheck, &State, &Sound};

    gOS_AddThreads(ths);
    gOS_Launch(BSP_Clock_GetFreq() / THREADFREQ);

    return 0;

}
