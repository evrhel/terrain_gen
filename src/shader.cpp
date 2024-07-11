#include "shader.h"

#include <cstdio>

void Shader::use() const
{
    glUseProgram(_program);
}

void Shader::load(const char *vertexSource, const char *fragmentSource)
{
    GLuint vert, frag;
    int success;
    char infoLog[512];
    bool hasError = false;

    /* Compile vertex shader */

    vert = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vert, 1, &vertexSource, NULL);
    glCompileShader(vert);

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        printf("Vertex shader compilation failed: %s\n", infoLog);
        hasError = true;
        glDeleteShader(vert);
    }

    /* Compile fragment shader */

    frag = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(frag, 1, &fragmentSource, NULL);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        printf("Fragment shader compilation failed: %s\n", infoLog);
        hasError = true;
        glDeleteShader(frag);
    }

    if (hasError)
        exit(1);

    /* Link shaders */

    _program = glCreateProgram();

    glAttachShader(_program, vert);
    glAttachShader(_program, frag);
    glLinkProgram(_program);

    glGetProgramiv(_program, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(_program, 512, NULL, infoLog);
        printf("Shader program linking failed: %s\n", infoLog);
        exit(1);
    }

    glDeleteShader(vert);
    glDeleteShader(frag);
}

void Shader::setFloat(const char *name, float value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform1f(loc, value);
}

void Shader::setInt(const char *name, int value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform1i(loc, value);
}

void Shader::setVector2(const char *name, const Vector2 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform2fv(loc, 1, (float *)&value);
}

void Shader::setVector3(const char *name, const Vector3 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform3fv(loc, 1, (float *)&value);
}

void Shader::setVector4(const char *name, const Vector4 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform4fv(loc, 1, (float *)&value);
}

void Shader::setMatrix3(const char *name, const Matrix3 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniformMatrix3fv(loc, 1, GL_FALSE, (float *)&value);
}

void Shader::setMatrix4(const char *name, const Matrix4 &value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniformMatrix4fv(loc, 1, GL_FALSE, (float *)&value);
}

Shader::Shader() : _program(0) {}

Shader::~Shader()
{
    if (_program)
        glDeleteProgram(_program);
}