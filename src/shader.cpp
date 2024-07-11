#include "shader.h"

void Shader::use();

void Shader::load(const char *vertexSource, const char *fragmentSource)
{
    
}

void Shader::setFloat(const char *name, float value);

void Shader::setInt(const char *name, int value);

void Shader::setVector2(const char *name, const Vector2 &value);

void Shader::setVector3(const char *name, const Vector3 &value);

void Shader::setVector4(const char *name, const Vector4 &value);

void Shader::setMatrix3(const char *name, const Matrix3 &value);

void Shader::setMatrix4(const char *name, const Matrix4 &value);

Shader::Shader() : _shader(0) {}

Shader::~Shader()
{
    if (_shader)
        glDeleteProgram(_shader);
}