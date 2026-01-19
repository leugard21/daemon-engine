#ifndef VEC2_H
#define VEC2_H

#include <math.h>

typedef struct Vec2 {
  float x;
  float y;
} Vec2;

static inline Vec2 v2(float x, float y) {
  Vec2 r = {x, y};
  return r;
}

static inline Vec2 v2_add(Vec2 a, Vec2 b) { return v2(a.x + b.x, a.y + b.y); }
static inline Vec2 v2_sub(Vec2 a, Vec2 b) { return v2(a.x - b.x, a.y - b.y); }
static inline Vec2 v2_mul(Vec2 a, float s) { return v2(a.x * s, a.y * s); }

static inline float v2_dot(Vec2 a, Vec2 b) { return a.x * b.x + a.y * b.y; }
static inline float v2_len2(Vec2 a) { return v2_dot(a, a); }
static inline float v2_len(Vec2 a) { return sqrtf(v2_len2(a)); }

static inline Vec2 v2_norm(Vec2 a) {
  float l = v2_len(a);
  if (l <= 0.000001f)
    return v2(0.0f, 0.0f);
  return v2_mul(a, 1.0f / l);
}

static inline Vec2 v2_perp_left(Vec2 a) { return v2(-a.y, a.x); }

#endif // !VEC2_H
