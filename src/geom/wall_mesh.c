#include "wall_mesh.h"

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

static void push_quad(Vtx *dst, int *at, Vtx a, Vtx b, Vtx c, Vtx d) {
  push_tri(dst, at, a, b, c);
  push_tri(dst, at, a, c, d);
}

static void add_wall_segment(Vtx *verts, int *at, float x0, float z0, float x1,
                             float z1, float y0, float y1, float u0, float u1,
                             float r, float g, float b, float light) {
  Vtx a = {x0, y0, z0, r, g, b, u0, y0, light};
  Vtx b0 = {x1, y0, z1, r, g, b, u1, y0, light};
  Vtx c = {x1, y1, z1, r, g, b, u1, y1, light};
  Vtx d = {x0, y1, z0, r, g, b, u0, y1, light};
  push_quad(verts, at, a, b0, c, d);
}

bool wall_mesh_build(WallMesh *out, const Map *map) {
  memset(out, 0, sizeof(*out));
  if (!map || map->line_count <= 0)
    return false;

  int max_quads = map->line_count * 2;
  int max_vtx = max_quads * 6;

  Vtx *verts = (Vtx *)malloc((size_t)max_vtx * sizeof(Vtx));
  if (!verts)
    return false;

  int at = 0;

  for (int i = 0; i < map->line_count; i++) {
    const Linedef *l = &map->lines[i];

    const Vec2 p0 = map->verts[l->v0];
    const Vec2 p1 = map->verts[l->v1];

    float dx = p1.x - p0.x;
    float dy = p1.y - p0.y;
    float wall_len = sqrtf(dx * dx + dy * dy);
    float u0 = 0.0f;
    float u1 = wall_len;

    const Sector *sf = &map->sectors[l->front_sector];

    if (l->back_sector < 0) {
      add_wall_segment(verts, &at, p0.x, p0.y, p1.x, p1.y, sf->floor_h,
                       sf->ceil_h, u0, u1, 0.8f, 0.8f, 0.8f, sf->light_level);
    } else {
      const Sector *sb = &map->sectors[l->back_sector];

      float f0 = sf->floor_h;
      float c0 = sf->ceil_h;
      float f1 = sb->floor_h;
      float c1 = sb->ceil_h;

      float low_top = (f0 > f1) ? f0 : f1;
      float low_bot = (f0 < f1) ? f0 : f1;
      if (low_top - low_bot > 0.0001f) {
        add_wall_segment(verts, &at, p0.x, p0.y, p1.x, p1.y, low_bot, low_top,
                         u0, u1, 0.7f, 0.5f, 0.2f, sf->light_level);
      }

      float up_top = (c0 > c1) ? c0 : c1;
      float up_bot = (c0 < c1) ? c0 : c1;
      if (up_top - up_bot > 0.0001f) {
        add_wall_segment(verts, &at, p0.x, p0.y, p1.x, p1.y, up_bot, up_top, u0,
                         u1, 0.2f, 0.6f, 0.8f, sf->light_level);
      }
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

  out->vertex_count = at;

  free(verts);
  return true;
}

void wall_mesh_destroy(WallMesh *m) {
  if (!m)
    return;
  if (m->vbo)
    glDeleteBuffers(1, &m->vbo);
  if (m->vao)
    glDeleteVertexArrays(1, &m->vao);
  memset(m, 0, sizeof(*m));
}
