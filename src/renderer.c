#include "renderer.h"
#include "gfx/shader.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

typedef struct RendererState {
  ShaderProgram prog;
  GLuint vao;
  GLuint vbo;
  GLint u_viewProj;
  GLint u_model;
} RendererState;

static RendererState g;

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

bool renderer_init(void) {
  memset(&g, 0, sizeof(g));

  if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    fprintf(stderr, "Failed to initialize glad\n");
    return false;
  }

  printf("OpenGL Vendor  : %s\n", glGetString(GL_VENDOR));
  printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
  printf("OpenGL Version : %s\n", glGetString(GL_VERSION));

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  if (!shader_build(&g.prog, k_vs, k_fs))
    return false;

  g.u_viewProj = glGetUniformLocation(g.prog.program, "u_viewProj");
  g.u_model = glGetUniformLocation(g.prog.program, "u_model");

  float verts[] = {
      0.0f, 0.5f, 0.0f, -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f,
  };

  glGenVertexArrays(1, &g.vao);
  glGenBuffers(1, &g.vbo);

  glBindVertexArray(g.vao);
  glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(verts), verts,
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

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
  Mat4 model = m4_identity();

  glUseProgram(g.prog.program);
  glUniformMatrix4fv(g.u_viewProj, 1, GL_FALSE, view_proj->m);
  glUniformMatrix4fv(g.u_model, 1, GL_FALSE, model.m);

  glBindVertexArray(g.vao);
  glDrawArrays(GL_TRIANGLES, 0, 3);
  glBindVertexArray(0);

  glUseProgram(0);
}

void renderer_shutdown(void) {
  if (g.vbo)
    glDeleteBuffers(1, &g.vbo);
  if (g.vao)
    glDeleteVertexArrays(1, &g.vao);
  shader_destroy(&g.prog);
  memset(&g, 0, sizeof(g));
}
