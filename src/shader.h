#pragma once

#include <string>
#include <cstdarg>

#include <glad/glad.h>
#include <mutil/mutil.h>

using namespace mutil;

struct Material;
class Gbuffer;

class Shader
{
public:
    void use() const;

    void load(const char *name, const char *vertexSource, const char *fragmentSource);
    void loadTess(const char *name, const char *vertexSource, const char *fragmentSource, const char *tessControlSource, const char *tessEvalSource);

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
    void setMaterial(int index, const Material &material);
    void setGbuffer(const Gbuffer *gbuffer);

    void setBoolf(const char *format, bool value, ...);
    void setFloatf(const char *format, float value, ...);
    void setIntf(const char *format, int value, ...);
    void setVector2f(const char *format, Vector2 value, ...);
    void setVector3f(const char *format, Vector3 value, ...);
    void setVector4f(const char *format, Vector4 value, ...);
    void setMatrix3f(const char *format, Matrix3 value, ...);
    void setMatrix4f(const char *format, Matrix4 value, ...);

    void setTexturef(const char *format, GLuint texture, int unit, ...);

    void bindUniformBlock(const char *name, GLuint bindingPoint);

    constexpr const std::string &name() const { return _name; }

    Shader();
    ~Shader();

private:
    GLuint _program;
    std::string _name;

    int getUniformLocationf(const char *format, ...);
    int getUniformLocationv(const char *format, va_list args);
};
