#include "map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void map_zero(Map *m) { memset(m, 0, sizeof(*m)); }

void map_destroy(Map *m) {
  if (!m)
    return;

  free(m->verts);
  free(m->lines);
  free(m->sectors);

  map_zero(m);
}

static bool alloc_arrays(Map *m, int vcount, int lcount, int scount) {
  m->verts = (Vec2 *)calloc((size_t)vcount, sizeof(Vec2));
  m->lines = (Linedef *)calloc((size_t)lcount, sizeof(Linedef));
  m->sectors = (Sector *)calloc((size_t)scount, sizeof(Sector));
  if (!m->verts || !m->lines || !m->sectors)
    return false;

  m->vert_count = vcount;
  m->line_count = lcount;
  m->sector_count = scount;

  return true;
}

bool map_build_test(Map *out) {
  if (!out)
    return false;
  map_zero(out);

  if (!alloc_arrays(out, 6, 7, 2)) {
    map_destroy(out);
    return false;
  }

  out->verts[0] = v2(0.0f, 0.0f);
  out->verts[1] = v2(4.0f, 0.0f);
  out->verts[2] = v2(4.0f, 4.0f);
  out->verts[3] = v2(0.0f, 4.0f);
  out->verts[4] = v2(8.0f, 0.0f);
  out->verts[5] = v2(8.0f, 4.0f);

  out->sectors[0].floor_h = 0.0f;
  out->sectors[0].ceil_h = 3.0f;

  out->sectors[1].floor_h = 0.5f;
  out->sectors[1].ceil_h = 3.0f;

  out->lines[0] =
      (Linedef){.v0 = 0, .v1 = 1, .front_sector = 0, .back_sector = -1};
  out->lines[1] = (Linedef){
      .v0 = 1, .v1 = 2, .front_sector = 0, .back_sector = 1}; // portal
  out->lines[2] =
      (Linedef){.v0 = 2, .v1 = 3, .front_sector = 0, .back_sector = -1};
  out->lines[3] =
      (Linedef){.v0 = 3, .v1 = 0, .front_sector = 0, .back_sector = -1};

  out->lines[4] =
      (Linedef){.v0 = 1, .v1 = 4, .front_sector = 1, .back_sector = -1};
  out->lines[5] =
      (Linedef){.v0 = 4, .v1 = 5, .front_sector = 1, .back_sector = -1};
  out->lines[6] =
      (Linedef){.v0 = 5, .v1 = 2, .front_sector = 1, .back_sector = -1};

  return true;
}

void map_debug_print(const Map *m) {
  if (!m)
    return;

  printf("Map:\n");
  printf("  verts: %d\n", m->vert_count);
  for (int i = 0; i < m->vert_count; i++) {
    printf("    v%d = (%.2f, %.2f)\n", i, m->verts[i].x, m->verts[i].y);
  }

  printf("  sectors: %d\n", m->sector_count);
  for (int i = 0; i < m->sector_count; i++) {
    printf("    s%d floor=%.2f ceil=%.2f\n", i, m->sectors[i].floor_h,
           m->sectors[i].ceil_h);
  }

  printf("  lines: %d\n", m->line_count);
  for (int i = 0; i < m->line_count; i++) {
    const Linedef *l = &m->lines[i];
    printf("    l%d v%d->v%d front=%d back=%d\n", i, l->v0, l->v1,
           l->front_sector, l->back_sector);
  }
}
