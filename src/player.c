#include "raylib.h"
#include "player.h"

void CreatePlayer(Player *player, Texture2D texture){
    player->texture = texture;
    player->rec = (Rectangle){(GetScreenWidth()/2)-(50/2), 75, texture.width, texture.height};
    player->lives = 3;
    player->score = 0;
}

void ResetPlayer(Player *player){
    player->rec = (Rectangle){(GetScreenWidth()/2)-(50/2), 75, player->texture.width, player->texture.height};
    player->lives = 3;
    player->score = 0;
}

void DrawPlayer(Player *player){
    DrawTexture(player->texture, player->rec.x, player->rec.y, WHITE);
}