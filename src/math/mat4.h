#ifndef MAT4_H
#define MAT4_H

#include "vec3.h"
#include <math.h>
#include <string.h>

typedef struct Mat4 {
  float m[16];
} Mat4;

static inline Mat4 m4_identity(void) {
  Mat4 r;
  memset(&r, 0, sizeof(r));
  r.m[0] = 1.0f;
  r.m[5] = 1.0f;
  r.m[10] = 1.0f;
  r.m[15] = 1.0f;
  return r;
}

static inline Mat4 m4_mul(Mat4 a, Mat4 b) {
  Mat4 r;
  for (int c = 0; c < 4; c++) {
    for (int r0 = 0; r0 < 4; r0++) {
      r.m[c * 4 + r0] =
          a.m[0 * 4 + r0] * b.m[c * 4 + 0] + a.m[1 * 4 + r0] * b.m[c * 4 + 1] +
          a.m[2 * 4 + r0] * b.m[c * 4 + 2] + a.m[3 * 4 + r0] * b.m[c * 4 + 3];
    }
  }
  return r;
}

static inline Mat4 m4_translate(Vec3 t) {
  Mat4 r = m4_identity();
  r.m[12] = t.x;
  r.m[13] = t.y;
  r.m[14] = t.z;
  return r;
}

static inline Mat4 m4_rotate_y(float rad) {
  Mat4 r = m4_identity();
  float c = cosf(rad);
  float s = sinf(rad);
  r.m[0] = c;
  r.m[2] = -s;
  r.m[8] = s;
  r.m[10] = c;
  return r;
}

static inline Mat4 m4_rotate_x(float rad) {
  Mat4 r = m4_identity();
  float c = cosf(rad);
  float s = sinf(rad);
  r.m[5] = c;
  r.m[6] = s;
  r.m[9] = -s;
  r.m[10] = c;
  return r;
}

static inline Mat4 m4_perspective(float fovy_rad, float aspect, float znear,
                                  float zfar) {
  Mat4 r;
  memset(&r, 0, sizeof(r));
  float f = 1.0f / tanf(fovy_rad * 0.5f);
  r.m[0] = f / aspect;
  r.m[5] = f;
  r.m[10] = (zfar + znear) / (znear - zfar);
  r.m[11] = -1.0f;
  r.m[14] = (2.0f * zfar * znear) / (znear - zfar);
  return r;
}

static inline Mat4 m4_lookat(Vec3 eye, Vec3 center, Vec3 up) {
  Vec3 f = v3_norm(v3_sub(center, eye));
  Vec3 s = v3_norm(v3_cross(f, up));
  Vec3 u = v3_cross(s, f);

  Mat4 r = m4_identity();
  r.m[0] = s.x;
  r.m[4] = s.y;
  r.m[8] = s.z;
  r.m[1] = u.x;
  r.m[5] = u.y;
  r.m[9] = u.z;
  r.m[2] = -f.x;
  r.m[6] = -f.y;
  r.m[10] = -f.z;

  r.m[12] = -v3_dot(s, eye);
  r.m[13] = -v3_dot(u, eye);
  r.m[14] = v3_dot(f, eye);
  return r;
}

static inline Mat4 m4_from_basis(Vec3 right, Vec3 up, Vec3 forward, Vec3 pos) {
  Mat4 r = m4_identity();

  r.m[0] = right.x;
  r.m[4] = right.y;
  r.m[8] = right.z;
  r.m[1] = up.x;
  r.m[5] = up.y;
  r.m[9] = up.z;
  r.m[2] = -forward.x;
  r.m[6] = -forward.y;
  r.m[10] = -forward.z;

  r.m[12] = -v3_dot(right, pos);
  r.m[13] = -v3_dot(up, pos);
  r.m[14] = v3_dot(forward, pos);

  return r;
}

#endif // !MAT4_H
