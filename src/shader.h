#pragma once

#include <string>

#include <glad/glad.h>
#include <mutil/mutil.h>

using namespace mutil;

struct Material;

class Shader
{
public:
    void use() const;

    void load(const char *name, const char *vertexSource, const char *fragmentSource);

    void setBool(const char *name, bool value);
    void setFloat(const char *name, float value);
    void setInt(const char *name, int value);
    void setVector2(const char *name, const Vector2 &value);
    void setVector3(const char *name, const Vector3 &value);
    void setVector4(const char *name, const Vector4 &value);
    void setMatrix3(const char *name, const Matrix3 &value);
    void setMatrix4(const char *name, const Matrix4 &value);

    void setTexture(const char *name, GLuint texture, int unit);
    void setMaterial(const Material &material);

    void bindUniformBlock(const char *name, GLuint bindingPoint);

    constexpr const std::string &name() const { return _name; }

    Shader();
    ~Shader();

private:
    GLuint _program;
    std::string _name;
};
