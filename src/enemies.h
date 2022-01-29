#ifndef ENEMIES_H
#define ENEMIES_H

#include "raylib.h"

typedef struct
{
    Rectangle rec;
    Texture2D texture;
    Color color; // for drawing object without texture.
    bool alive;
} Enemy;

void LoadEnemies(Enemy (*enemies)[], int size, Rectangle rec, Color color);
void LoadEnemiesWithTexture(Enemy (*enemies)[], int size, Texture2D Texture);
int CreateNewEnemy(Enemy (*enemies)[], int size);
void UpdateEnemies(Enemy (*enemies)[], int size, float speed);
void ResetEnemies(Enemy (*enemies)[], int size);

void DrawEnemies(Enemy (*enemies)[], int size);


#endif