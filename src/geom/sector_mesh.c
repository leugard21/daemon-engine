#include "sector_mesh.h"

#include <stdlib.h>
#include <string.h>

typedef struct Vtx {
  float px, py, pz;
  float cr, cg, cb;
  float u, v;
  float light;
} Vtx;

static void push_tri(Vtx *dst, int *at, Vtx a, Vtx b, Vtx c) {
  dst[(*at)++] = a;
  dst[(*at)++] = b;
  dst[(*at)++] = c;
}

bool sector_mesh_build(SectorMesh *out, const Map *map) {
  memset(out, 0, sizeof(*out));
  if (!map || map->sector_count <= 0)
    return false;

  int total_tris = 0;
  for (int s = 0; s < map->sector_count; s++) {
    int n = map->sectors[s].loop.count;
    if (n < 3)
      continue;
    total_tris += (n - 2) * 2;
  }
  int total_vtx = total_tris * 3;

  Vtx *verts = (Vtx *)malloc((size_t)total_vtx * sizeof(Vtx));
  if (!verts)
    return false;

  int at = 0;

  for (int s = 0; s < map->sector_count; s++) {
    const Sector *sec = &map->sectors[s];
    const int n = sec->loop.count;
    if (n < 3)
      continue;

    int i0 = sec->loop.indices[0];
    Vec2 p0_2 = map->verts[i0];

    for (int i = 1; i < n - 1; i++) {
      int i1 = sec->loop.indices[i];
      int i2 = sec->loop.indices[i + 1];

      Vec2 p1_2 = map->verts[i1];
      Vec2 p2_2 = map->verts[i2];

      Vtx f0 = {p0_2.x, sec->floor_h, p0_2.y,          0.2f, 0.8f, 0.2f,
                p0_2.x, p0_2.y,       sec->light_level};
      Vtx f1 = {p1_2.x, sec->floor_h, p1_2.y,          0.2f, 0.8f, 0.2f,
                p1_2.x, p1_2.y,       sec->light_level};
      Vtx f2 = {p2_2.x, sec->floor_h, p2_2.y,          0.2f, 0.8f, 0.2f,
                p2_2.x, p2_2.y,       sec->light_level};

      Vtx c0 = {p0_2.x, sec->ceil_h, p0_2.y,          0.2f, 0.2f, 0.8f,
                p0_2.x, p0_2.y,      sec->light_level};
      Vtx c1 = {p1_2.x, sec->ceil_h, p1_2.y,          0.2f, 0.2f, 0.8f,
                p1_2.x, p1_2.y,      sec->light_level};
      Vtx c2 = {p2_2.x, sec->ceil_h, p2_2.y,          0.2f, 0.2f, 0.8f,
                p2_2.x, p2_2.y,      sec->light_level};

      push_tri(verts, &at, f0, f1, f2);
      push_tri(verts, &at, c2, c1, c0);
    }
  }

  glGenVertexArrays(1, &out->vao);
  glGenBuffers(1, &out->vbo);

  glBindVertexArray(out->vao);
  glBindBuffer(GL_ARRAY_BUFFER, out->vbo);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)(at * (int)sizeof(Vtx)), verts,
               GL_STATIC_DRAW);

  GLsizei stride = (GLsizei)sizeof(Vtx);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)(3 * sizeof(float)));

  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                        (void *)(6 * sizeof(float)));

  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride,
                        (void *)(8 * sizeof(float)));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  free(verts);

  out->vertex_count = at;
  return true;
}

void sector_mesh_destroy(SectorMesh *m) {
  if (!m)
    return;

  if (m->vbo)
    glDeleteBuffers(1, &m->vbo);
  if (m->vao)
    glDeleteVertexArrays(1, &m->vao);

  memset(m, 0, sizeof(*m));
}

void sector_mesh_draw(const SectorMesh *m, GLuint program, GLint u_viewProj) {
  if (!m || m->vao == 0 || m->vertex_count <= 0)
    return;

  glUseProgram(program);
  glUniformMatrix4fv(u_viewProj, 1, GL_FALSE, m4_identity().m);
  (void)u_viewProj;

  glBindVertexArray(m->vao);
  glDrawArrays(GL_TRIANGLES, 0, m->vertex_count);
  glBindVertexArray(0);

  glUseProgram(0);
}
