#ifndef GEOM_2D_H
#define GEOM_2D_H

#include <stdbool.h>

#include "../math/vec2.h"

typedef struct Segment2 {
  Vec2 a;
  Vec2 b;
} Segment2;

float seg2_signed_distance(Vec2 p, Vec2 a, Vec2 b);
Vec2 seg2_closest_point(Vec2 p, Vec2 a, Vec2 b);

bool seg2_intersect(Vec2 p0, Vec2 p1, Vec2 q0, Vec2 q1);

#endif // !GEOM_2D_H
