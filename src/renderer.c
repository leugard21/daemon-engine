#include "renderer.h"

#include <SDL_video.h>
#include <glad/glad.h>
#include <stdio.h>

bool renderer_init(void) {
  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    fprintf(stderr, "Failed to initialize glad\n");
    return false;
  }

  printf("OpenGL Vendor  : %s\n", glGetString(GL_VENDOR));
  printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
  printf("OpenGL Version : %s\n", glGetString(GL_VERSION));

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}

void renderer_set_viewport(int w, int h) {
  if (w < 1)
    w = 1;
  if (h < 1)
    h = 1;
  glViewport(0, 0, w, h);
}

void renderer_begin_frame(void) {
  glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void renderer_shutdown(void) {}
