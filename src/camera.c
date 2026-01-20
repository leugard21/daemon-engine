#include "camera.h"

#include <math.h>

void camera_init(Camera *c) {
  c->pos = v3(0.0f, 1.6f, 2.0f);
  c->yaw = 0.0f;
  c->fov_y = (float)(60.0 * (M_PI / 180.0));
  c->znear = 0.05f;
  c->zfar = 2000.0f;
}

Mat4 camera_view(Camera *c) {
  float cy = cosf(c->yaw);
  float sy = sinf(c->yaw);

  Vec3 forward = v3(0.0f, 0.0f, -1.0f);
  forward = v3(forward.x * cy + forward.z * sy, 0.0f,
               -forward.x * sy + forward.z * cy);
  forward = v3_norm(forward);

  Vec3 up = v3(0.0f, 1.0f, 0.0f);
  Vec3 right = v3_norm(v3_cross(forward, up));

  return m4_from_basis(right, up, forward, c->pos);
}

Mat4 camera_proj(Camera *c, float aspect) {
  return m4_perspective(c->fov_y, aspect, c->znear, c->zfar);
}

Mat4 camera_view_proj(Camera *c, float aspect) {
  Mat4 v = camera_view(c);
  Mat4 p = camera_proj(c, aspect);
  return m4_mul(p, v);
}
