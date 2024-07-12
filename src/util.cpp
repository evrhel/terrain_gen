#include "util.h"

#include <cstdio>

#include <glad/glad.h>

void checkGLErrors(const char *where)
{
    GLenum error;
    while ((error = glGetError()) != GL_NO_ERROR)
        printf("%s: OpenGL error: 0x%X\n", where, error);
}
