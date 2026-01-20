#include "shader.h"

#include <stdio.h>
#include <stdlib.h>

static bool compile_stage(GLuint *out_shader, GLenum type, const char *src) {
  GLuint sh = glCreateShader(type);
  glShaderSource(sh, 1, &src, NULL);
  glCompileShader(sh);

  GLint ok = 0;
  glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
  if (!ok) {
    GLint len = 0;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);
    char *log = (char *)malloc((size_t)len + 1);
    if (log) {
      glGetShaderInfoLog(sh, len, NULL, log);
      log[len] = 0;
      fprintf(stderr, "Shader compile error:\n%s\n", log);
      free(log);
    }
    glDeleteShader(sh);
    return false;
  }

  *out_shader = sh;
  return true;
}

bool shader_build(ShaderProgram *out, const char *vs_src, const char *fs_src) {
  GLuint vs = 0;
  GLuint fs = 0;

  if (!compile_stage(&vs, GL_VERTEX_SHADER, vs_src))
    return false;
  if (!compile_stage(&fs, GL_FRAGMENT_SHADER, fs_src)) {
    glDeleteShader(vs);
    return false;
  }

  GLuint prog = glCreateProgram();
  glAttachShader(prog, vs);
  glAttachShader(prog, fs);
  glLinkProgram(prog);

  glDeleteShader(vs);
  glDeleteShader(fs);

  GLint ok = 0;
  glGetProgramiv(prog, GL_LINK_STATUS, &ok);
  if (!ok) {
    GLint len = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    char *log = (char *)malloc((size_t)len + 1);
    if (log) {
      glGetProgramInfoLog(prog, len, NULL, log);
      log[len] = 0;
      fprintf(stderr, "Program link error:\n%s\n", log);
      free(log);
    }
    glDeleteProgram(prog);
    return false;
  }

  out->program = prog;
  return true;
}

void shader_destroy(ShaderProgram *s) {
  if (s && s->program) {
    glDeleteProgram(s->program);
    s->program = 0;
  }
}
