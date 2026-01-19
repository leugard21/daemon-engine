#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

bool renderer_init(void);
void renderer_set_viewport(int w, int h);
void renderer_begin_frame(void);
void renderer_shutdown(void);

#endif // !RENDERER_H
