#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "input.h"
#include "renderer.h"
#include "time.h"

static void log_sdl_error(const char *msg) {
  fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
}

static void game_update(double fixed_dt, const InputState *in) {
  (void)fixed_dt;
  (void)in;
}

static void game_render(double frame_dt) {
  (void)frame_dt;
  renderer_begin_frame();
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0) {
    log_sdl_error("SDL_Init failed");
    return 1;
  }

  const int start_w = 1280;
  const int start_h = 720;

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

  SDL_Window *window = SDL_CreateWindow(
      "Daemon Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, start_w,
      start_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  if (!window) {
    log_sdl_error("SDL_CreateWindow failed");
    SDL_Quit();
    return 1;
  }

  SDL_GLContext gl = SDL_GL_CreateContext(window);
  if (!gl) {
    log_sdl_error("SDL_GL_CreateContext failed");
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  SDL_GL_SetSwapInterval(1);

  if (!renderer_init()) {
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  InputState in;
  input_init(&in, start_w, start_h);
  renderer_set_viewport(start_w, start_h);

  time_init();

  const double fixed_dt = 1.0 / 60.0;
  const double max_frame_dt = 0.25;

  double prev = time_now_seconds();
  double acc = 0.0;

  bool running = true;
  while (running) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      input_process_event(&in, &e);
    }

    if (in.quit_requested || in.key_pressed[SDL_SCANCODE_ESCAPE])
      running = false;

    double now = time_now_seconds();
    double frame_dt = now - prev;
    prev = now;
    if (frame_dt > max_frame_dt)
      frame_dt = max_frame_dt;

    if (in.resized)
      renderer_set_viewport(in.window_w, in.window_h);

    acc += frame_dt;
    while (acc >= fixed_dt) {
      game_update(fixed_dt, &in);
      acc -= fixed_dt;
    }

    game_render(frame_dt);
    SDL_GL_SwapWindow(window);
  }

  renderer_shutdown();
  SDL_GL_DeleteContext(gl);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
