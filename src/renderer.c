#include "renderer.h"
#include "gfx/shader.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define RENDERER_DEBUG_CLIPSPACE 1

typedef struct RendererState {
  ShaderProgram prog;
  GLuint vao;
  GLuint vbo;
  GLint u_viewProj;
  GLint u_model;
} RendererState;

static RendererState g;

static void gl_check(const char *where) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    printf("GL error at %s: 0x%x\n", where, (unsigned)err);
  }
}

#if RENDERER_DEBUG_CLIPSPACE
static const char *k_vs = "#version 330 core\n"
                          "layout(location=0) in vec3 a_pos;\n"
                          "void main(){\n"
                          "  gl_Position = vec4(a_pos, 1.0);\n"
                          "}\n";

static const char *k_fs = "#version 330 core\n"
                          "out vec4 o_color;\n"
                          "void main(){\n"
                          "  o_color = vec4(1.0, 0.0, 0.0, 1.0);\n"
                          "}\n";
#else
static const char *k_vs =
    "#version 330 core\n"
    "layout(location=0) in vec3 a_pos;\n"
    "uniform mat4 u_viewProj;\n"
    "uniform mat4 u_model;\n"
    "void main(){\n"
    "  gl_Position = u_viewProj * u_model * vec4(a_pos, 1.0);\n"
    "}\n";

static const char *k_fs = "#version 330 core\n"
                          "out vec4 o_color;\n"
                          "void main(){\n"
                          "  o_color = vec4(0.9, 0.2, 0.3, 1.0);\n"
                          "}\n";
#endif

bool renderer_init(void) {
  memset(&g, 0, sizeof(g));

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    fprintf(stderr, "Failed to initialize glad\n");
    return false;
  }

  printf("OpenGL Vendor  : %s\n", glGetString(GL_VENDOR));
  printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
  printf("OpenGL Version : %s\n", glGetString(GL_VERSION));

  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  if (!shader_build(&g.prog, k_vs, k_fs))
    return false;

  g.u_viewProj = glGetUniformLocation(g.prog.program, "u_viewProj");
  g.u_model = glGetUniformLocation(g.prog.program, "u_model");

#if RENDERER_DEBUG_CLIPSPACE
  float verts[] = {
      0.0f, 0.8f, 0.0f, -0.8f, -0.8f, 0.0f, 0.8f, -0.8f, 0.0f,
  };
#else
  float verts[] = {
      0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
  };
#endif

  glGenVertexArrays(1, &g.vao);
  glGenBuffers(1, &g.vbo);

  glBindVertexArray(g.vao);
  glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(verts), verts,
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * (GLsizei)sizeof(float),
                        (void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  gl_check("renderer_init");
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

void renderer_draw_test_triangle(const Mat4 *view_proj) {
  glUseProgram(g.prog.program);

#if RENDERER_DEBUG_CLIPSPACE
  (void)view_proj;
  glDisable(GL_DEPTH_TEST);
#else
  Mat4 model = m4_identity();
  glEnable(GL_DEPTH_TEST);

  if (g.u_viewProj >= 0)
    glUniformMatrix4fv(g.u_viewProj, 1, GL_FALSE, view_proj->m);
  if (g.u_model >= 0)
    glUniformMatrix4fv(g.u_model, 1, GL_FALSE, model.m);
#endif

  glBindVertexArray(g.vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);

  glUseProgram(0);
  gl_check("renderer_draw_test_triangle");
}

void renderer_shutdown(void) {
  if (g.vbo)
    glDeleteBuffers(1, &g.vbo);
  if (g.vao)
    glDeleteVertexArrays(1, &g.vao);
  shader_destroy(&g.prog);
  memset(&g, 0, sizeof(g));
}
