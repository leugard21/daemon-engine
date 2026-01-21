#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

#include "map/map.h"
#include "math/mat4.h"

bool renderer_init(void);
void renderer_set_viewport(int w, int h);
void renderer_begin_frame(void);
bool renderer_build_sector_mesh(const Map *map);
bool renderer_build_world_meshes(const Map *map);
void renderer_draw_world(const Mat4 *view_proj);
void renderer_shutdown(void);

#endif // !RENDERER_H
