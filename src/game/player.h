#ifndef PLAYER_H
#define PLAYER_H

#include "../input.h"
#include "../map/map.h"
#include "../math/vec2.h"

typedef struct Player {
  Vec2 pos;
  float yaw;
  float radius;
  float height;
  int sector;
} Player;

void player_init(Player *p);
void player_update(Player *p, const Map *map, const InputState *in, float dt);

#endif // !PLAYER_H
