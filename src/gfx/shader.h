#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <stdbool.h>

typedef struct ShaderProgram {
  GLuint program;
} ShaderProgram;

bool shader_build(ShaderProgram *out, const char *vs_src, const char *fs_src);
void shader_destroy(ShaderProgram *s);

#endif // !SHADER_H
