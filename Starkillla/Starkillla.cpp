//Coders:Arantxa Marquez
//Date: 4/10/2025
//Descrption: basic shooting game
//Log:
//14 of March Created the code from a tutorial found on youtube
//21 of March fixed an error with the {} that may cause the last two lines to be underlined in red
//22 of March implemented blocks falling properly
//23 of March made blocks fall slower so it would be easier for the player
//25 of March added main menu, only quit and start game functional
//27 of March, tried to make the high score button usable, still not complete
//29 of March, changed the blocks behaviors once a certain score is hit.
//30 of March, added texture to blocks and game background
//1 of April, added texture to spaceship and main menu
//2 of April, trying to make the game not killl the player inmediately 
//3 of April, Made progress in making the highscore section work properly

//Things to do: * marks as completed
// Find a way to make the game last longer (make the hardness of the game more gradual)*
//Make Bricks not fall so quickly*
//
//Make more comfortable controls
// 
// Make a Main menu*
// *Starts *High Scores *Quit
//
//Add background*
//
//Add textures -> watch tutorial on finding the Raylib folder to place the textures there*
//Add sound*



#include "raylib.h"
#include <queue>
#include <fstream>
#include <algorithm>
#include <string>
#include <iostream>
#include <vector>





#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define PLAYER_SPEED 5
#define BULLET_SPEED 7
#define BRICK_SPEED 1
#define BRICK_HORIZONTAL_SPEED 1
bool movingRight = true;
int levelThreshold = 100; //score at which blocks change behavior

typedef struct Bullet
{
    Rectangle rect;
    bool active;
} Bullet;

typedef struct Brick
{
    Rectangle rect;
    bool active;
} Brick;




typedef enum
{
    MENU, PLAY, GAMEOVER, HIGHSCORES
}GameState;

GameState currentState = MENU;

void ResetGame(Rectangle& player, std::queue<Brick>& bricks, std::queue<Bullet>& bullets, int& score, int& hard, bool& gameOver)
{
    player.x = SCREEN_WIDTH / 2 - 25;
    player.y = SCREEN_HEIGHT - 50;

    while (!bricks.empty()) bricks.pop();
    while (!bullets.empty()) bullets.pop();

    score = 0;
    hard = 500;
    gameOver = false;

}

void SaveHighScores(int newScore)
{
    int currentHighScore = 0;


    std::ifstream inputFile("highscores.txt");
    if (inputFile.is_open()) {
        inputFile >> currentHighScore;
        inputFile.close();

    }

    if (newScore > currentHighScore)
    {
        std::ofstream outputFile("highscores.txt");
        if (outputFile.is_open())
        {
            outputFile << newScore;
            outputFile.close();
        }
    }

}

int LoadHighScore()
{
    int highScore = 0;

    std::ifstream inputFile("highscores.txt");
    if (inputFile.is_open())
    {
        inputFile >> highScore;
        inputFile.close();
    }

    return highScore;

}








int main()
{
    // Initialization
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Starkilla");

    InitAudioDevice();

    //Sound goes here ;)

    Sound shoot = LoadSound("Resources/lasergun.wav");

    //Textures go here :D
    Texture2D galaxybg = LoadTexture("Resources/galaxy.png");
    Texture2D block = LoadTexture("Resources/block.png");
    Texture2D menu = LoadTexture("Resources/mainmenu.png");
    Texture2D spaceship = LoadTexture("Resources/spaceship.png");

    // Player properties
    Rectangle player = { SCREEN_WIDTH / 2 - 25, SCREEN_HEIGHT - 50, 50, 10 };
    std::queue<Brick> bricks;
    std::queue<Bullet> bullets;

    bool gameOver = false;
    int hard = 500;
    int score = 0;


    GameState currentState = MENU;



    SetTargetFPS(60);



    while (!WindowShouldClose()) {  // Main game loop



        if (currentState == MENU) {
            BeginDrawing();

            DrawTexture(menu, 0, 0, WHITE);

            DrawText("STAR KILLA", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 4, 40, WHITE);
            DrawText("1. Start Game", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 60, 30, WHITE);
            DrawText("3. Hight Scores", SCREEN_WIDTH / 2 - 100, SCREEN_WIDTH / 2 - 20, 30, WHITE);
            DrawText("2. Quit", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 20, 30, WHITE);

            if (IsKeyPressed(KEY_ONE))
            {
                ResetGame(player, bricks, bullets, score, hard, gameOver);
                currentState = PLAY;
            }
            if (IsKeyPressed(KEY_THREE))
            {
                currentState = HIGHSCORES;//add high score functionality here
            }
            if (IsKeyPressed(KEY_TWO))
            {
                CloseWindow();
                return 0;
            }

            EndDrawing();
            continue; //skip the rest of the game loop while in the main menu so game won't start till 1 is pressed

        }

        if (currentState == HIGHSCORES) {
            int highScore = LoadHighScore();

          
            DrawTexture(menu, 0, 0, WHITE);

            DrawText("HIGH SCORE", SCREEN_WIDTH / 2 - 100, 50, 40, WHITE);
            DrawText(TextFormat("Highest Score: %i", highScore), SCREEN_WIDTH / 2 - 150, 150, 30, WHITE);
            DrawText("Press ENTER to return to Menu", SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT - 100, 30, WHITE);

            if (IsKeyPressed(KEY_ENTER)) {
                currentState = MENU;
            }

            EndDrawing();
            continue;
        }


        BeginDrawing();
        ClearBackground(GetColor(0x052c46ff));

        if (!gameOver && currentState == PLAY) {



            // Randomly generate bricks
            if (GetRandomValue(0, hard) < 21) {
                Brick bb;
                bb.rect.x = GetRandomValue(0, SCREEN_WIDTH - 50);
                bb.rect.y = -20; // Start just above the screen
                bb.rect.width = 50;
                bb.rect.height = 20;
                bb.active = true;
                bricks.push(bb);
                if (hard > 100) hard--; // Make it progressively harder
            }



            // Player movement
            if (IsKeyDown(KEY_RIGHT) && player.x + player.width < SCREEN_WIDTH) player.x += PLAYER_SPEED;
            if (IsKeyDown(KEY_LEFT) && player.x > 0) player.x -= PLAYER_SPEED;


            //Space bar for shooting
            if (IsKeyPressed(KEY_SPACE)) {
                PlaySound(shoot);
                Bullet bu;
                bu.rect.x = player.x + player.width / 2 - 2;
                bu.rect.y = player.y;
                bu.rect.width = 4;
                bu.rect.height = 10;
                bu.active = true;
                bullets.push(bu);
            }

            // Update bullets
            int bulletCount = bullets.size();
            std::queue<Bullet> tempBullets;

            for (int i = 0; i < bulletCount; i++) {
                Bullet bu = bullets.front();
                bullets.pop();
                if (bu.active) {
                    bu.rect.y -= BULLET_SPEED;

                    if (bu.rect.y > -bu.rect.height) {
                        tempBullets.push(bu);
                    }
                }
            }
            bullets = tempBullets; // Restore bullets queue

            // Update bricks
            // Update bricks
            int brickCount = bricks.size();
            std::queue<Brick> tempBricks;
            bool hitEdge = false;

            // First pass: Check if any brick hits the edge
            for (int i = 0; i < brickCount; i++)
            {
                Brick bb = bricks.front();
                bricks.pop();

                if (score >= levelThreshold)
                {
                    if (bb.rect.x <= 0 || bb.rect.x + bb.rect.width >= SCREEN_WIDTH) {
                        hitEdge = true;
                    }
                }
                tempBricks.push(bb);
            }

            // Change direction if edge was hit
            if (hitEdge)
            {
                movingRight = !movingRight;
            }

            // Second pass: Move all bricks according to game state
            bricks = tempBricks;
            brickCount = bricks.size();
            tempBricks = std::queue<Brick>();  // Clear the temp queue

            for (int i = 0; i < brickCount; i++)
            {
                Brick bb = bricks.front();
                bricks.pop();

                // Level 1: Straight down movement
                if (score < levelThreshold)
                {
                    bb.rect.y += BRICK_SPEED;
                }
                // Level 2+: Space Invaders style movement
                else
                {
                    // Move horizontally based on current direction
                    if (movingRight)
                    {
                        bb.rect.x += BRICK_HORIZONTAL_SPEED;
                    }
                    else
                    {
                        bb.rect.x -= BRICK_HORIZONTAL_SPEED;
                    }

                    // If we hit an edge this frame, move all bricks down
                    if (hitEdge)
                    {
                        bb.rect.y += 10;  // Move down by 10 pixels when changing direction
                    }
                }

                // Check if game over - this was outside the loop in your version
                if (bb.rect.y + bb.rect.height > SCREEN_HEIGHT)
                {
                    gameOver = true;  // End game if a brick reaches the bottom
                }
                else
                {
                    tempBricks.push(bb); // Keep brick if still on screen
                }
            }

            bricks = tempBricks; // Restore bricks queue







            // Collision detection between bullets and bricks
            bulletCount = bullets.size();

            std::queue<Bullet> tempBullets2;


            for (int i = 0; i < bulletCount; i++)
            {
                Bullet bullet = bullets.front();
                bullets.pop();
                bool bulletHit = false;

                brickCount = bricks.size();
                std::queue<Brick> tempBricks2;

                for (int j = 0; j < brickCount; j++)
                {
                    Brick brick = bricks.front();
                    bricks.pop();

                    if (CheckCollisionRecs(bullet.rect, brick.rect) && !bulletHit)
                    {
                        bulletHit = true; // Mark bullet for removal
                        score++; // Increment score when block is hit


                    }
                    else
                    {
                        tempBricks2.push(brick); // Keep bricks that weren't hit
                    }
                }

                bricks = tempBricks2;

                if (!bulletHit) {
                    tempBullets2.push(bullet); // Keep bullets that didn't hit anything
                }
            }

            bullets = tempBullets2; // Restore bullets queue

        }

        // Draw everything
        BeginDrawing();
        ClearBackground(BLACK);

        DrawTexture(galaxybg, 0, 0, WHITE);

        if (!gameOver) {

            //Draw player
            DrawTexture(spaceship, player.x, player.y, WHITE);

            // Draw bullets
            int bulletCount = bullets.size();
            std::queue<Bullet> tempBullets3;
            for (int i = 0; i < bulletCount; i++) {
                Bullet bullet = bullets.front();
                bullets.pop();
                DrawRectangleRec(bullet.rect, RED);
                tempBullets3.push(bullet);
            }
            bullets = tempBullets3;

            // Draw bricks
            int brickCount = bricks.size();
            std::queue<Brick> tempBricks3;
            for (int i = 0; i < brickCount; i++)
            {
                Brick brick = bricks.front();
                bricks.pop();

                DrawTexture(block, (int)brick.rect.x, (int)brick.rect.y, WHITE);


                tempBricks3.push(brick);
            }
            bricks = tempBricks3;


            //Score on the corner of the screen
            DrawText(TextFormat("Score: %i", score), 10, 10, 20, GREEN);
        }
        if (gameOver)
        {
            // Game over message
            DrawText(TextFormat("GAME OVER!! Your score is: %i", score), SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 50, 40, RED);
            DrawText("Press ENTER to return to Menu", SCREEN_WIDTH / 2 - 200, SCREEN_HEIGHT / 2 + 50, 30, WHITE);

            SaveHighScores(score);

            //return to main menu
            if (IsKeyPressed(KEY_ENTER))
            {
                currentState = MENU;
                gameOver = false;
            }
        }

        EndDrawing();
    }

    UnloadTexture(galaxybg);
    UnloadTexture(block);

    CloseWindow();



    return 0;
}