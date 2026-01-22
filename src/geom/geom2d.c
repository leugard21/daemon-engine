#include "geom2d.h"

#include <math.h>

float seg2_signed_distance(Vec2 p, Vec2 a, Vec2 b) {
  Vec2 ab = v2_sub(b, a);
  Vec2 ap = v2_sub(p, a);
  float area2 = ab.x * ap.y - ab.y * ap.x;
  float len = v2_len(ab);
  if (len <= 0.000001f)
    return 0.0f;

  return area2 / len;
}

Vec2 seg2_closest_point(Vec2 p, Vec2 a, Vec2 b) {
  Vec2 ab = v2_sub(b, a);
  float ab2 = v2_dot(ab, ab);
  if (ab2 <= 0.000001f)
    return a;

  float t = v2_dot(v2_sub(p, a), ab) / ab2;
  if (t < 0.0f)
    t = 0.0f;
  if (t > 1.0f)
    t = 1.0f;

  return v2_add(a, v2_mul(ab, t));
}

static float cross2(Vec2 a, Vec2 b) { return a.x * b.y - a.y * b.x; }

bool seg2_intersect(Vec2 p0, Vec2 p1, Vec2 q0, Vec2 q1) {
  Vec2 r = v2_sub(p1, p0);
  Vec2 s = v2_sub(q1, q0);
  float rxs = cross2(r, s);
  float qpxr = cross2(v2_sub(q0, p0), r);
  if (fabsf(rxs) <= 1e-6f && fabsf(qpxr) <= 1e-6f) {
    return false;
  }
  if (fabs(rxs) <= 1e-6f && fabs(qpxr) > 1e-6f)
    return false;

  float t = cross2(v2_sub(q0, p0), s) / rxs;
  float u = cross2(v2_sub(q0, p0), r) / rxs;

  return (t >= 0.0f && t <= 1.0f && u >= 0.0f && u <= 1.0f);
}
