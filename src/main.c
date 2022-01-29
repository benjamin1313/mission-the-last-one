#include <stdlib.h>
#include <time.h>
#include <math.h>

#include "raylib.h"
#include "player.h"
#include "enemies.h"

//#define PLATFORM_WEB
#define BACKGROUNDCOLOR CLITERAL(Color){ 181, 181, 181, 255 }

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
int screenWidth = 360;
int screenHeight = 720;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void);     // Update and Draw one frame
void DrawBtn(Rectangle rect , const char * text, Color color);
void StartGame();

#define NIGHTBLUE CLITERAL(Color){ 0, 13, 40, 255 }
#define LIGHTRED CLITERAL(Color){242, 65, 79, 255}

typedef enum GameScreen { LOGO = 0, TITLE, GAMEPLAY, ENDING, WINNING } GameScreen;
GameScreen currentScreen = LOGO;
int framesCounter = 0; 

//--------------------------------------------------------------------------------------
// General variables
Vector2 mousePos;
Player player;
size_t maxNumEnemies = 20;
Enemy enemies[20];
int speed = 8;
int spawnCounter = 120;
int score = 0;
int backgroundOffset1 = 0;
int backgroundOffset2 = 0;
int rocketcount = 200;

// Variables Start Screen
float startBtnWidth;
float startBtnHeight = 100;
Rectangle startBtnRec;
const char *startBtnText = "Start";

// game variables
int rocketsSpawned = 0;
bool spawnRockets = true;
bool endGame = false;

// Variables End Screen
Rectangle retryBtnRec;
Rectangle retryBtnRec2;
const char *retryBtnText = "Try again!";
int endingState = 0; // -1 = dead, 0 = default, 1 = won
int explosionState = 0;
Rectangle explosionSourceRec;
Rectangle explosionDistRec;
Vector2 explosionOrigin;
float explosionScale = 4.0f;

// Variables for WINNING sceen
Rectangle spyFallingRec;
Rectangle spyJumpingRec;
Rectangle spyStandingRec;
Rectangle spyEatingRec;
Rectangle spyMainRec;
Rectangle manStandingRec;
Rectangle manDownRec;
Rectangle manMainRec;
int scaler = 4;

int winStage = 0;

// textures
Texture2D backgroundTexture;
Texture2D playerTexture;
Texture2D rocketTexture;
Texture2D explosionTexture;
Texture2D spyTexture;
Texture2D manTexture;
Texture2D logoTexture;

// sounds
Sound explosionSound;
Music backgroundMusic;


//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    InitWindow(screenWidth, screenHeight, "Web Game");
    InitAudioDevice();

    // loading textures
    logoTexture = LoadTexture("resources/logo.png");

    backgroundTexture = LoadTexture("resources/background.png");
    backgroundOffset2 = backgroundTexture.height;

    spyTexture = LoadTexture("resources/tuxedo.png");
    manTexture = LoadTexture("resources/old_hero.png");

    Image playerImage = LoadImage("resources/falling_player.png");
    ImageResizeNN(&playerImage, playerImage.width*4, playerImage.height*4);
    playerTexture = LoadTextureFromImage(playerImage);
    UnloadImage(playerImage);

    Image rocketImage = LoadImage("resources/rocket.png");
    ImageResizeNN(&rocketImage, rocketImage.width*4, rocketImage.height*4);
    rocketTexture = LoadTextureFromImage(rocketImage);
    UnloadImage(rocketImage);

    Image explosionImage = LoadImage("resources/explosion.png");
    //ImageResizeNN(&explosionImage, explosionImage.width*4, explosionImage.height*4);
    explosionTexture = LoadTextureFromImage(explosionImage);
    UnloadImage(explosionImage);

    // loading sounds
    explosionSound = LoadSound("resources/explosion.wav");
    SetSoundVolume(explosionSound, 1.0f);

    backgroundMusic = LoadMusicStream("resources/spy_music.mp3");
     SetMusicVolume(backgroundMusic, 0.5f);

    // Variables Start Screen
    startBtnWidth = 300;
    startBtnHeight = 100;
    startBtnRec = (Rectangle){(screenWidth/2)-(startBtnWidth/2), (screenHeight/2)-(startBtnHeight/2), startBtnWidth, startBtnHeight};

    // Variables End Screen
    retryBtnRec = (Rectangle){(screenWidth/2)-(startBtnWidth/2), screenHeight-(screenHeight/3), startBtnWidth, startBtnHeight};
    retryBtnRec2 = (Rectangle){(screenWidth/2)-(startBtnWidth/2), 300, startBtnWidth, startBtnHeight};

    currentScreen = TITLE;

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
#else
    SetTargetFPS(60);   // Set our game to run at 60 frames-per-second
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        UpdateDrawFrame();
    }
#endif


    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(backgroundTexture);
    UnloadTexture(playerTexture);
    UnloadTexture(rocketTexture);
    UnloadTexture(explosionTexture);
    UnloadTexture(spyTexture);
    UnloadTexture(manTexture);
    UnloadTexture(logoTexture);

    UnloadSound(explosionSound);
    UnloadMusicStream(backgroundMusic);

    CloseAudioDevice();
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void)
{
    mousePos = GetMousePosition();

    switch(currentScreen)
    {
        case LOGO:
        {
            // TODO: Update LOGO screen variables here!
        } break;
        case TITLE:
        {
             // TODO: Update TITLE screen variables here!

             // Press enter to change to GAMEPLAY screen
            if (CheckCollisionPointRec(mousePos, startBtnRec) && IsGestureDetected(GESTURE_TAP))
            {   
                currentScreen = GAMEPLAY;
                StartGame();
            }
        } break;
        case GAMEPLAY:
        {
            // TODO: Update GAMEPLAY screen variables here!
            if(!IsMusicStreamPlaying(backgroundMusic)) PlayMusicStream(backgroundMusic);
            UpdateMusicStream(backgroundMusic);

            // Press enter to change to ENDING screen
            if (IsKeyPressed(KEY_Q))
            {
                currentScreen = TITLE;
                StopMusicStream(backgroundMusic);
            }

            if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)){
                if(!(player.rec.x < 0)) player.rec.x -= (speed+2);
            }

            if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)){
                if(!(player.rec.x+player.rec.width > screenWidth)) player.rec.x += speed+2;
            }

            UpdateEnemies(&enemies, maxNumEnemies, speed);

            if(framesCounter != 0 && framesCounter%30==0 && spawnRockets){
                CreateNewEnemy(&enemies, maxNumEnemies);
                rocketsSpawned++;
            }

            if(framesCounter==spawnCounter) framesCounter=0;
            framesCounter++;

            for(int i = 0; i < maxNumEnemies; i++){
                if(CheckCollisionRecs(enemies[i].rec,player.rec) && enemies[i].alive){
                    enemies[i].alive = false;
                    endingState = -1;
                    currentScreen = ENDING;
                    StopMusicStream(backgroundMusic);
                }

                // Calculating score based on distace between rocket and player
                float dX = (enemies[i].rec.x+(enemies[i].rec.width/2))-(player.rec.x+(player.rec.width/2));
                float dY = (player.rec.y+(player.rec.height/2))-(enemies[i].rec.y+(enemies[i].rec.height/2));
                float distace = sqrt(pow(dX,2)+pow(dY,2));
                if(distace < 80 && enemies[i].alive){
                    score++;
                }
            }
            
            backgroundOffset1 -= speed/2;
            backgroundOffset2 -= speed/2;
            if(backgroundOffset1 < -1*(backgroundTexture.height)){
                backgroundOffset1 = backgroundTexture.height;
            }

            if(backgroundOffset2 < -1*backgroundTexture.height){
                backgroundOffset2 = backgroundTexture.height;
            }

            // Getting ready to end game (player victory)
            if(rocketsSpawned > rocketcount){ // 100 rockets takes around 50 secons to pass
                spawnRockets = false;
            }

            if(!spawnRockets){
                endGame = true;
                for(int i = 0; i < maxNumEnemies; i++){
                    if(enemies[i].alive) endGame = false;
                }
            }

            if(endGame == true){
                player.rec.y += speed;
            }

            if(player.rec.y > screenHeight){
                StopMusicStream(backgroundMusic);
                 currentScreen = WINNING;
            }



        } break;
        case ENDING:
        {   

            // TODO: Update ENDING screen variables here!
            
            // play explosion when dead.
            if(endingState < 0){
                endingState = 0;
                PlaySound(explosionSound);
                explosionState = 3;
            }

            if(explosionState == 3){
                explosionSourceRec = (Rectangle){68, 4, 25, 25};
                explosionDistRec = (Rectangle){player.rec.x+(player.rec.width/2), player.rec.y+(player.rec.height/2), 25*explosionScale, 25*explosionScale};
                explosionOrigin = (Vector2){explosionTexture.width/2, explosionTexture.height/2};
            }

            // if no explotion don't render it
            if(explosionState == 0){
                explosionSourceRec = (Rectangle){0, 0, 1, 1};
                explosionDistRec = (Rectangle){0, 0, 1, 1};
                explosionOrigin = (Vector2){0,0};
            }

            if (IsKeyPressed(KEY_Q))
            {
                currentScreen = TITLE;
                StopMusicStream(backgroundMusic);
            }

            if (CheckCollisionPointRec(mousePos, retryBtnRec) && IsGestureDetected(GESTURE_TAP))
            {
                StartGame();
                explosionState = 0;
            }

        } break;
        case WINNING:
        {
            if(winStage==0){
                // These are used to find the right sprite in the texture. 
                spyFallingRec = (Rectangle){215, 40, 13, 14};
                spyJumpingRec = (Rectangle){92, 10, 14, 15};
                spyStandingRec = (Rectangle){12, 11, 10, 15};
                spyEatingRec = (Rectangle){213, 71, 15, 15};
                manStandingRec = (Rectangle){95, 49, 15, 15};
                manDownRec = (Rectangle){97, 65, 15, 11};

                spyMainRec = (Rectangle){screenWidth/2, 0, (spyFallingRec.width*scaler), spyFallingRec.height*scaler};
                manMainRec = (Rectangle){screenWidth/2, screenHeight-(manStandingRec.height*scaler), manStandingRec.width*scaler, manStandingRec.height*scaler};
                winStage=1;
            }

            if(winStage==1){
                
            }


            switch(winStage)
            {
                case 1:
                {
                    if(spyMainRec.y < screenHeight-(spyMainRec.height/2)){
                        spyMainRec.y += speed;
                    }
                    if(CheckCollisionRecs(spyMainRec, manMainRec)){
                        winStage = 2;
                        spyMainRec = (Rectangle){spyMainRec.x, spyMainRec.y, (spyJumpingRec.width*scaler), spyJumpingRec.height*scaler};
                        manMainRec = (Rectangle){screenWidth/2, screenHeight-(manDownRec.height*scaler), manDownRec.width*scaler, manDownRec.height*scaler};
                    }
                } break;
                case 2:
                {
                    if(spyMainRec.y > screenHeight-(screenHeight/3)){
                        spyMainRec.y -= speed;
                    }else{
                        winStage = 3;
                    }
                } break;
                case 3:
                {
                    if(spyMainRec.y < screenHeight-(spyMainRec.height)){
                        spyMainRec.y += speed;
                    }else{
                        winStage = 4;
                        spyMainRec = (Rectangle){spyMainRec.x, spyMainRec.y, (spyEatingRec.width*scaler), spyEatingRec.height*scaler};
                    }
                } break;
            }

            if (IsKeyPressed(KEY_Q))
            {
                currentScreen = TITLE;
            }


            if (CheckCollisionPointRec(mousePos, retryBtnRec2) && IsGestureDetected(GESTURE_TAP))
            {
                StartGame();
                explosionState = 0;
            }
        }
        default: break;
    }
    //----------------------------------------------------------------------------------

    // Draw
    //----------------------------------------------------------------------------------
    BeginDrawing();

        ClearBackground(RAYWHITE);

        switch(currentScreen)
        {
            case LOGO:
            {
                // TODO: Draw LOGO screen here!
                DrawRectangle(0, 0, screenWidth, screenHeight, BACKGROUNDCOLOR);
                DrawText("LOADING...", (screenWidth/2)-(MeasureText("LOADING...",30)/2), screenHeight/2, 30, DARKGRAY);

            } break;
            case TITLE:
            {
                // TODO: Draw TITLE screen here!
                DrawRectangle(0, 0, screenWidth, screenHeight, BACKGROUNDCOLOR);
                //DrawText("Welcome to the web!!!", (screenWidth/2)-(MeasureText("Welcome to the web!!!",30)/2), 50, 30, RED);
                DrawTexture(logoTexture, 0, 200, WHITE);
                if(CheckCollisionPointRec(mousePos, startBtnRec)){
                    DrawBtn(startBtnRec, startBtnText, LIGHTRED);
                }else{
                    DrawBtn(startBtnRec, startBtnText, RED);
                }

            } break;
            case GAMEPLAY:
            {
                // TODO: Draw GAMEPLAY screen here!
                DrawRectangle(0, 0, screenWidth, screenHeight, BACKGROUNDCOLOR);
                DrawTexture(backgroundTexture, 0, backgroundOffset1, WHITE);
                DrawTexture(backgroundTexture, 0, backgroundOffset2, WHITE);
                DrawEnemies(&enemies, maxNumEnemies);
                DrawPlayer(&player);

                //DrawText(TextFormat("FPS: %02i", GetFPS()), 10, 5, 20, RED);
                const char* scoreString = TextFormat("Score: %03i", score);
                DrawText(scoreString, (screenWidth/2)-(MeasureText(scoreString,30)/2), 5, 20, RED);
                DrawRectangle(0, 0, 8, ((float)screenHeight/(float)rocketcount)*rocketsSpawned, RED);

            } break;
            case ENDING:
            {
                // TODO: Draw ENDING screen here!
                DrawRectangle(0, 0, screenWidth, screenHeight, BACKGROUNDCOLOR);
                DrawTexture(backgroundTexture, 0, backgroundOffset1, WHITE);
                DrawTexture(backgroundTexture, 0, backgroundOffset2, WHITE);
                DrawEnemies(&enemies, maxNumEnemies);

                DrawRectangle(0, 0, 8, ((float)screenHeight/(float)rocketcount)*rocketsSpawned, RED);
                
                if(explosionState > 0){
                    DrawTextureTiled(explosionTexture, explosionSourceRec, explosionDistRec, explosionOrigin, 0.0f, explosionScale, WHITE);
                }
                
                const char* scoreString = TextFormat("Score: %03i", score);
                DrawText(scoreString, (screenWidth/2)-(MeasureText(scoreString,30)/2), screenHeight/2, 30, RED);
                DrawText("Thanks for playing!", (screenWidth/2)-(MeasureText("Thanks for playing!",30)/2), 300, 30, RED);

                if(CheckCollisionPointRec(mousePos, retryBtnRec)){
                    DrawBtn(retryBtnRec, retryBtnText, LIGHTRED);
                }else{
                    DrawBtn(retryBtnRec, retryBtnText, RED);
                }

            } break;
            case WINNING:
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, BACKGROUNDCOLOR);

                const char* scoreString = TextFormat("Score: %03i", score);
                DrawText(scoreString, (screenWidth/2)-(MeasureText(scoreString,30)/2), 200, 30, RED);
                DrawText("Thanks for playing!", (screenWidth/2)-(MeasureText("Thanks for playing!",30)/2), 100, 30, RED);

                if(CheckCollisionPointRec(mousePos, retryBtnRec)){
                    DrawBtn(retryBtnRec2, retryBtnText, LIGHTRED);
                }else{
                    DrawBtn(retryBtnRec2, retryBtnText, RED);
                }

                // Drawing based on wincase should be change to sitch case as well
                if(winStage==1){
                    DrawTexturePro(spyTexture, spyFallingRec, spyMainRec, (Vector2){spyMainRec.width/2, spyMainRec.height/2}, 0.0f, WHITE);
                    DrawTexturePro(manTexture, manStandingRec, manMainRec, (Vector2){manMainRec.width/2, 0}, 0.0f, WHITE);
                }
                if(winStage==2 || winStage==3){
                    DrawTexturePro(manTexture, manDownRec, manMainRec, (Vector2){manMainRec.width/2, 0}, 0.0f, WHITE);
                    DrawTexturePro(spyTexture, spyJumpingRec, spyMainRec, (Vector2){spyMainRec.width/2, spyMainRec.height/2}, 0.0f, WHITE);
                }
                if(winStage==4){
                    DrawTexturePro(manTexture, manDownRec, manMainRec, (Vector2){manMainRec.width/2, 0}, 0.0f, WHITE);
                    DrawTexturePro(spyTexture, spyEatingRec, spyMainRec, (Vector2){spyMainRec.width/2, spyMainRec.height/2}, 0.0f, WHITE);
                    const char* donut1 = "Pew. That was close.";
                    const char* donut2 = "Almost didn't get the last donut.";
                    DrawText(donut1, (screenWidth/2)-(MeasureText(donut1,20)/2), screenHeight - 150, 20, BLACK);
                    DrawText(donut2, (screenWidth/2)-(MeasureText(donut2,20)/2), screenHeight - 120, 20, BLACK);
                }

            }
            default: break;
        }

    EndDrawing();
    //----------------------------------------------------------------------------------
}

void DrawBtn(Rectangle rect , const char * text, Color color){
    int fontSize = 40; // might want to change this pased on button hight.
    DrawRectangleRounded(rect, 0.1, 7, color);
    int stringWidth = MeasureText(text, fontSize);
    DrawText(text, (rect.x + rect.width/2)-(stringWidth/2), (rect.y + rect.height/2)-(fontSize/2), fontSize, WHITE);
}

void StartGame(){
    currentScreen = GAMEPLAY;
    CreatePlayer(&player, playerTexture);
    LoadEnemiesWithTexture(&enemies, maxNumEnemies, rocketTexture);
    endingState = 0;
    rocketsSpawned = 0;
    spawnRockets = true;
    endGame = false;
    winStage=0;
    score=0;
    PlayMusicStream(backgroundMusic);
}