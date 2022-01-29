#include "raylib.h"
#include "enemies.h"

void LoadEnemies(Enemy (*enemies)[], int size, Rectangle rec, Color color){
    for(int i = 0; i < size; i++){
        (*enemies)[i].alive = false;
        (*enemies)[i].rec = rec;
        (*enemies)[i].color = color;
    }
}

void LoadEnemiesWithTexture(Enemy (*enemies)[], int size, Texture2D Texture){
    for(int i = 0; i < size; i++){
        (*enemies)[i].alive = false;
        (*enemies)[i].rec = (Rectangle){0,0,Texture.width,Texture.height};
        (*enemies)[i].texture = Texture;
    }
}

int CreateNewEnemy(Enemy (*enemies)[], int size){
    for(int i = 0; i < size; i++){
        if(!(*enemies)[i].alive){
            (*enemies)[i].alive = true;
            (*enemies)[i].rec = (Rectangle){(float)GetRandomValue(0, GetScreenWidth()-(int)(*enemies)[i].rec.width), GetScreenHeight(), (*enemies)[i].rec.width, (*enemies)[i].rec.height};
            return true;
        }
    }
    return false;
}

void UpdateEnemies(Enemy (*enemies)[], int size, float speed){
    for(int i = 0; i < size; i++){
        if((*enemies)[i].alive) (*enemies)[i].rec.y -= speed;
        if((*enemies)[i].rec.y < 0 - (*enemies)[i].rec.height){
             (*enemies)[i].alive = false;;
        }
    }
}

void ResetEnemies(Enemy (*enemies)[], int size){
    for(int i = 0; i < size; i++){
        (*enemies)[i].alive = false;
        (*enemies)[i].rec = (Rectangle){0, 0, 50, 50};
    }
}

void DrawEnemies(Enemy (*enemies)[], int size){
    for(int i = 0; i < size; i++){
        if((*enemies)[i].alive){
            DrawRectangleRec((*enemies)[i].rec, (*enemies)[i].color);
            DrawTexture((*enemies)[i].texture, (*enemies)[i].rec.x, (*enemies)[i].rec.y, WHITE);
        }
    }
}