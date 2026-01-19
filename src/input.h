#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef struct InputState {
  bool quit_requested;
  bool key_down[SDL_NUM_SCANCODES];
  bool key_pressed[SDL_NUM_SCANCODES];
  bool key_released[SDL_NUM_SCANCODES];
  int window_w;
  int window_h;
  bool resized;
} InputState;

void input_init(InputState *in, int w, int h);
void input_begin_frame(InputState *in);
void input_process_event(InputState *in, const SDL_Event *e);

#endif // !INPUT_H
