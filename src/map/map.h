#ifndef MAP_H
#define MAP_H

#include <stdbool.h>
#include <stdint.h>

#include "../math/vec2.h"

typedef struct SectorLoop {
  int *indices;
  int count;
} SectorLoop;

typedef struct Sector {
  float floor_h;
  float ceil_h;
  SectorLoop loop;
} Sector;

typedef struct Linedef {
  int v0;
  int v1;
  int front_sector;
  int back_sector;
} Linedef;

typedef struct Map {
  Vec2 *verts;
  int vert_count;

  Linedef *lines;
  int line_count;

  Sector *sectors;
  int sector_count;
} Map;

bool map_build_test(Map *out);
void map_destroy(Map *m);

void map_debug_print(const Map *m);

#endif // !MAP_H
