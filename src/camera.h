#ifndef CAMERA_H
#define CAMERA_H

#include "math/mat4.h"
#include "math/vec3.h"

typedef struct Camera {
  Vec3 pos;
  float yaw;
  float pitch;
  float fov_y;
  float znear;
  float zfar;
} Camera;

void camera_init(Camera *c);
Mat4 camera_view(Camera *c);
Mat4 camera_proj(Camera *c, float aspect);
Mat4 camera_view_proj(Camera *c, float aspect);

#endif // !CAMERA_H
