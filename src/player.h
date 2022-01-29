#ifndef PLAYER_H
# define PLAYER_H

#include "raylib.h"

typedef struct
{
    Rectangle rec;
    Texture2D texture;
    int lives;
    int score;
} Player;

void CreatePlayer(Player *player, Texture2D texture);
void ResetPlayer(Player *player);
void DrawPlayer(Player *player);



#endif
