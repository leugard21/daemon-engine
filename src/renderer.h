#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

#include "math/mat4.h"

bool renderer_init(void);
void renderer_set_viewport(int w, int h);
void renderer_begin_frame(void);
void renderer_draw_test_triangle(const Mat4 *view_proj);
void renderer_shutdown(void);

#endif // !RENDERER_H
