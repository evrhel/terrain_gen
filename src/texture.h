#pragma once

#include <cstddef>

#include <glad/glad.h>

enum ColorSpace
{
    COLOR_SPACE_SRGB,
    COLOR_SPACE_LINEAR
};

class Texture2D final
{
public:
    void load(GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
    void load(const void *image, size_t size, ColorSpace colorSpace = COLOR_SPACE_SRGB);
    void load(const char *filename, ColorSpace colorSpace = COLOR_SPACE_SRGB);

    constexpr GLuint get() const { return _texture; }

    constexpr int width() const { return _width; }
    constexpr int height() const { return _height; }

    Texture2D();
    ~Texture2D();

private:
    GLuint _texture;
    int _width, _height;
};
