#ifndef SECTOR_MESH_H
#define SECTOR_MESH_H

#include <glad/glad.h>
#include <stdbool.h>

#include "../map/map.h"
#include "../math/mat4.h"

typedef struct SectorMesh {
  GLuint vao;
  GLuint vbo;
  int vertex_count;
} SectorMesh;

bool sector_mesh_build(SectorMesh *out, const Map *map);
void sector_mesh_destroy(SectorMesh *m);

void sector_mesh_draw(const SectorMesh*m, GLuint program, GLint u_viewProj);

#endif // !SECTOR_MESH_H
