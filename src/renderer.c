#include "renderer.h"
#include "geom/sector_mesh.h"
#include "geom/wall_mesh.h"
#include "gfx/shader.h"
#include "map/map.h"

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

typedef struct RendererState {
  ShaderProgram prog;
  GLuint vao;
  GLuint vbo;
  GLint u_viewProj;
  GLint u_model;
  SectorMesh sector_mesh;
  WallMesh wall_mesh;
} RendererState;

static RendererState g;

static const char *k_vs =
    "#version 330 core\n"
    "layout(location=0) in vec3 a_pos;\n"
    "layout(location=1) in vec3 a_col;\n"
    "out vec3 v_col;\n"
    "uniform mat4 u_viewProj;\n"
    "uniform mat4 u_model;\n"
    "void main(){\n"
    "  v_col = a_col;\n"
    "  gl_Position = u_viewProj * u_model * vec4(a_pos, 1.0);\n"
    "}\n";

static const char *k_fs = "#version 330 core\n"
                          "in vec3 v_col;\n"
                          "out vec4 o_color;\n"
                          "void main(){\n"
                          "  o_color = vec4(v_col, 1.0);\n"
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

  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  if (!shader_build(&g.prog, k_vs, k_fs))
    return false;

  g.u_viewProj = glGetUniformLocation(g.prog.program, "u_viewProj");
  g.u_model = glGetUniformLocation(g.prog.program, "u_model");

  const float s = 2.0f;
  float verts[] = {
      -s, 0.0f, -s, 0.2f, 0.8f, 0.2f, s,  0.0f, -s, 0.8f, 0.2f, 0.2f,
      s,  0.0f, s,  0.2f, 0.2f, 0.8f, -s, 0.0f, -s, 0.2f, 0.8f, 0.2f,
      s,  0.0f, s,  0.2f, 0.2f, 0.8f, -s, 0.0f, s,  0.8f, 0.8f, 0.2f,
  };

  glGenVertexArrays(1, &g.vao);
  glGenBuffers(1, &g.vbo);

  glBindVertexArray(g.vao);
  glBindBuffer(GL_ARRAY_BUFFER, g.vbo);
  glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)sizeof(verts), verts,
               GL_STATIC_DRAW);

  GLsizei stride = (GLsizei)(6 * sizeof(float));
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void *)0);

  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                        (void *)(3 * sizeof(float)));

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

bool renderer_build_sector_mesh(const Map *map) {
  sector_mesh_destroy(&g.sector_mesh);
  return sector_mesh_build(&g.sector_mesh, map);
}

bool renderer_build_world_meshes(const Map *map) {
  sector_mesh_destroy(&g.sector_mesh);
  wall_mesh_destroy(&g.wall_mesh);

  if (!sector_mesh_build(&g.sector_mesh, map))
    return false;
  if (!wall_mesh_build(&g.wall_mesh, map))
    return false;
  return true;
}

void renderer_draw_world(const Mat4 *view_proj) {
  glUseProgram(g.prog.program);
  if (g.u_viewProj >= 0)
    glUniformMatrix4fv(g.u_viewProj, 1, GL_FALSE, view_proj->m);
  if (g.u_model >= 0)
    glUniformMatrix4fv(g.u_model, 1, GL_FALSE, m4_identity().m);

  glBindVertexArray(g.sector_mesh.vao);
  glDrawArrays(GL_TRIANGLES, 0, g.sector_mesh.vertex_count);

  glBindVertexArray(g.wall_mesh.vao);
  glDrawArrays(GL_TRIANGLES, 0, g.wall_mesh.vertex_count);

  glBindVertexArray(0);
  glUseProgram(0);
}

void renderer_shutdown(void) {
  if (g.vbo)
    glDeleteBuffers(1, &g.vbo);
  if (g.vao)
    glDeleteVertexArrays(1, &g.vao);
  shader_destroy(&g.prog);
  sector_mesh_destroy(&g.sector_mesh);
  wall_mesh_destroy(&g.wall_mesh);
  memset(&g, 0, sizeof(g));
}
