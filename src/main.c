#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>

#include "camera.h"
#include "input.h"
#include "map/map.h"
#include "renderer.h"
#include "time.h"

static void log_sdl_error(const char *msg) {
  fprintf(stderr, "%s: %s\n", msg, SDL_GetError());
}

static Camera g_cam;
static Mat4 g_vp;

static Vec3 yaw_forward(float yaw) {
  float cy = cosf(yaw);
  float sy = sinf(yaw);
  return v3(sy, 0.0f, -cy);
}

static Vec3 yaw_right(float yaw) {
  float cy = cosf(yaw);
  float sy = sinf(yaw);
  return v3(cy, 0.0f, sy);
}

static void game_update(double fixed_dt, const InputState *in) {
  float dt = (float)fixed_dt;

  if (in->key_down[SDL_SCANCODE_LEFT])
    g_cam.yaw += dt * 1.5f;
  if (in->key_down[SDL_SCANCODE_RIGHT])
    g_cam.yaw -= dt * 1.5f;

  Vec3 f = yaw_forward(g_cam.yaw);
  Vec3 r = yaw_right(g_cam.yaw);

  float speed = 3.0f;
  if (in->key_down[SDL_SCANCODE_W])
    g_cam.pos = v3_add(g_cam.pos, v3_mul(f, speed * dt));
  if (in->key_down[SDL_SCANCODE_S])
    g_cam.pos = v3_sub(g_cam.pos, v3_mul(f, speed * dt));
  if (in->key_down[SDL_SCANCODE_A])
    g_cam.pos = v3_sub(g_cam.pos, v3_mul(r, speed * dt));
  if (in->key_down[SDL_SCANCODE_D])
    g_cam.pos = v3_add(g_cam.pos, v3_mul(r, speed * dt));
}

static void game_render(double frame_dt) {
  (void)frame_dt;
  renderer_begin_frame();
  renderer_draw_world(&g_vp);
}

int main(int argc, char **argv) {
  (void)argc;
  (void)argv;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER) != 0) {
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
  SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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

  SDL_GL_MakeCurrent(window, gl);
  SDL_GL_SetSwapInterval(1);

  if (!renderer_init()) {
    SDL_GL_DeleteContext(gl);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  camera_init(&g_cam);
  g_cam.pos = v3(2.0f, 1.0f, 10.0f);
  g_cam.yaw = 0.0f;

  Map map;
  if (!map_build_test(&map)) {
    printf("Failed to build test map\n");
    return 1;
  }
  map_debug_print(&map);

  if (!renderer_build_world_meshes(&map)) {
    printf("Failed to build world mesh\n");
    map_destroy(&map);
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
    input_begin_frame(&in);

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      input_process_event(&in, &e);
    }

    if (in.quit_requested || in.key_pressed[SDL_SCANCODE_ESCAPE]) {
      running = false;
      continue;
    }

    double now = time_now_seconds();
    double frame_dt = now - prev;
    prev = now;
    if (frame_dt > max_frame_dt)
      frame_dt = max_frame_dt;

    renderer_set_viewport(in.window_w, in.window_h);

    float aspect = (float)in.window_w / (float)in.window_h;
    g_vp = camera_view_proj(&g_cam, aspect);

    acc += frame_dt;
    while (acc >= fixed_dt) {
      game_update(fixed_dt, &in);
      acc -= fixed_dt;
    }

    renderer_begin_frame();
    renderer_draw_world(&g_vp);

    game_render(frame_dt);
    SDL_GL_SwapWindow(window);
  }

  renderer_shutdown();
  map_destroy(&map);
  SDL_GL_DeleteContext(gl);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}
