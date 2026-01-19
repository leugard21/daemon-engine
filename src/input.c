#include "input.h"

#include <string.h>

void input_init(InputState *in, int w, int h) {
  memset(in, 0, sizeof(*in));
  in->window_w = w;
  in->window_h = h;
}

void input_begin_frame(InputState *in) {
  memset(in->key_pressed, 0, sizeof(in->key_pressed));
  memset(in->key_released, 0, sizeof(in->key_released));
  in->resized = false;
}

void input_process_event(InputState *in, const SDL_Event *e) {
  switch (e->type) {
  case SDL_QUIT:
    in->quit_requested = true;
    break;

  case SDL_KEYDOWN: {
    SDL_Scancode sc = e->key.keysym.scancode;
    if (!e->key.repeat) {
      in->key_down[sc] = true;
      in->key_pressed[sc] = true;
    }
  } break;

  case SDL_KEYUP: {
    SDL_Scancode sc = e->key.keysym.scancode;
    in->key_down[sc] = false;
    in->key_released[sc] = true;
  } break;

  case SDL_WINDOWEVENT:
    if (e->window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
      in->window_w = e->window.data1;
      in->window_h = e->window.data2;
      in->resized = true;
    }
    break;

  default:
    break;
  }
}
