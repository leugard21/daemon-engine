#ifndef WALL_MESH_H
#define WALL_MESH_H

#include <glad/glad.h>
#include <stdbool.h>

#include "../map/map.h"

typedef struct WallMesh {
  GLuint vao;
  GLuint vbo;
  int vertex_count;
} WallMesh;

bool wall_mesh_build(WallMesh *out, const Map *map);
void wall_mesh_destroy(WallMesh *m);

#endif // !WALL_MESH_H
