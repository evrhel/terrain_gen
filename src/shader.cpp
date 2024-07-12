#include "shader.h"

#include <cstdio>

#include "material.h"
#include "util.h"

void Shader::use() const
{
    glUseProgram(_program);
}

void Shader::load(const char *name, const char *vertexSource, const char *fragmentSource)
{
    GLuint vert, frag;
    int success;
    char infoLog[512];
    bool hasError = false;

    printf("Shader::load: Loading %s\n", name);

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

    _name = name;

    /* Bind standard buffers */
    bindUniformBlock("Camera", CAMERA_UNIFORM_BINDING);
    bindUniformBlock("Atmosphere", ATMOSPHERE_UNIFORM_BINDING);
}

void Shader::setBool(const char *name, bool value)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
        glUniform1i(loc, value);
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

void Shader::setTexture(const char *name, GLuint texture, int unit)
{
    int loc = glGetUniformLocation(_program, name);
    if (loc != -1)
    {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
        glUniform1i(loc, unit);
    }
}

void Shader::setMaterial(const Material &material)
{
    /* Albedo */
    setTexture("uMaterial.albedo.tex", material.albedo.get(), 0);
    setBool("uMaterial.albedo.hasTex", material.albedo.get() != 0);
    setVector3("uMaterial.albedo.color", material.albedoColor);

    /* Emissive */
    setTexture("uMaterial.emissive.tex", material.emissive.get(), 1);
    setBool("uMaterial.emissive.hasTex", material.emissive.get() != 0);
    setVector3("uMaterial.emissive.color", material.emissiveColor);

    /* Normal */
    setTexture("uMaterial.normal.tex", material.normal.get(), 2);
    setBool("uMaterial.normal.hasTex", material.normal.get() != 0);
    setVector3("uMaterial.normal.color", material.normal.get() ? Vector3(1.0f) : kDefaultNormal);

    /* Roughness */
    setTexture("uMaterial.roughness.tex", material.roughness.get(), 3);
    setBool("uMaterial.roughness.hasTex", material.roughness.get() != 0);
    setFloat("uMaterial.roughness.value", material.roughnessValue);

    /* Metallic */
    setTexture("uMaterial.metallic.tex", material.metallic.get(), 4);
    setBool("uMaterial.metallic.hasTex", material.metallic.get() != 0);
    setFloat("uMaterial.metallic.value", material.metallicValue);

    /* AO */
    setTexture("uMaterial.ao.tex", material.ao.get(), 5);
    setBool("uMaterial.ao.hasTex", material.ao.get() != 0);
    setFloat("uMaterial.ao.value", material.aoValue);
}

void Shader::bindUniformBlock(const char *name, GLuint bindingPoint)
{
    GLuint index = glGetUniformBlockIndex(_program, name);
    if (index != GL_INVALID_INDEX)
        glUniformBlockBinding(_program, index, bindingPoint);
}

Shader::Shader() : _program(0) {}

Shader::~Shader()
{
    if (_program)
        glDeleteProgram(_program);
}