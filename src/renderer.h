#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>

bool renderer_init(void);
void renderer_begin_frame(void);
void renderer_shutdown(void);

#endif // !RENDERER_H
