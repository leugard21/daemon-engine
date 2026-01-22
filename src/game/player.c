#include "player.h"
#include "../geom/geom2d.h"
#include <math.h>

static Vec2 yaw_forward(float yaw) {
  float cy = cosf(yaw);
  float sy = sinf(yaw);
  return v2(sy, -cy);
}

static Vec2 yaw_right(float yaw) {
  float cy = cosf(yaw);
  float sy = sinf(yaw);
  return v2(cy, sy);
}

static bool portal_passable(const Sector *a, const Sector *b, float player_h) {
  float floor = (a->floor_h > b->floor_h) ? a->floor_h : b->floor_h;
  float ceil = (a->ceil_h < b->ceil_h) ? a->ceil_h : b->ceil_h;
  return (ceil - floor) >= player_h;
}

static void collide_and_slide(Player *p, const Map *map, Vec2 old_pos,
                              Vec2 new_pos) {
  Vec2 pos = new_pos;

  for (int i = 0; i < map->line_count; i++) {
    const Linedef *l = &map->lines[i];

    if (l->front_sector != p->sector && l->back_sector != p->sector)
      continue;

    const Sector *sf = &map->sectors[l->front_sector];
    const Sector *sb =
        (l->back_sector >= 0) ? &map->sectors[l->back_sector] : NULL;

    bool solid = (l->back_sector < 0);
    if (!solid && sb) {
      int other =
          (l->front_sector == p->sector) ? l->back_sector : l->front_sector;
      const Sector *so = &map->sectors[other];
      if (!portal_passable(sf, so, p->height))
        solid = true;
    }

    if (!solid)
      continue;

    Vec2 a = map->verts[l->v0];
    Vec2 b = map->verts[l->v1];

    Vec2 cp = seg2_closest_point(pos, a, b);
    Vec2 d = v2_sub(pos, cp);
    float dist2 = v2_len2(d);
    float r = p->radius;

    if (dist2 < r * r) {
      float dist = sqrtf(dist2);
      Vec2 n =
          (dist > 1e-6f) ? v2_mul(d, 1.0f / dist) : v2_norm(v2_sub(pos, a));
      pos = v2_add(cp, v2_mul(n, r));
    }
  }

  p->pos = pos;
}

static void update_sector(Player *p, const Map *map, Vec2 old_pos,
                          Vec2 new_pos) {
  for (int i = 0; i < map->line_count; i++) {
    const Linedef *l = &map->lines[i];
    if (l->back_sector < 0)
      continue;

    int a = l->front_sector;
    int b = l->back_sector;

    if (p->sector != a && p->sector != b)
      continue;

    Vec2 s0 = map->verts[l->v0];
    Vec2 s1 = map->verts[l->v1];

    if (!seg2_intersect(old_pos, new_pos, s0, s1))
      continue;

    int other = (p->sector == a) ? b : a;

    if (portal_passable(&map->sectors[p->sector], &map->sectors[other],
                        p->height)) {
      p->sector = other;
      return;
    }
  }
}

void player_init(Player *p) {
  p->pos = v2(1.0f, 1.0f);
  p->yaw = 0.0f;
  p->radius = 0.25f;
  p->height = 1.6f;
  p->sector = 0;
}

void player_update(Player *p, const Map *map, const InputState *in, float dt) {
  if (in->key_down[SDL_SCANCODE_LEFT])
    p->yaw += dt * 1.6f;
  if (in->key_down[SDL_SCANCODE_RIGHT])
    p->yaw -= dt * 1.6f;

  Vec2 f = yaw_forward(p->yaw);
  Vec2 r = yaw_right(p->yaw);

  Vec2 wish = v2(0.0f, 0.0f);
  if (in->key_down[SDL_SCANCODE_W])
    wish = v2_add(wish, f);
  if (in->key_down[SDL_SCANCODE_S])
    wish = v2_sub(wish, f);
  if (in->key_down[SDL_SCANCODE_D])
    wish = v2_add(wish, r);
  if (in->key_down[SDL_SCANCODE_A])
    wish = v2_sub(wish, r);

  float len = v2_len(wish);
  if (len > 0.0001f)
    wish = v2_mul(wish, 1.0f / len);

  float speed = 3.0f;
  Vec2 old_pos = p->pos;
  Vec2 new_pos = v2_add(p->pos, v2_mul(wish, speed * dt));

  collide_and_slide(p, map, old_pos, new_pos);
  update_sector(p, map, old_pos, p->pos);
}
