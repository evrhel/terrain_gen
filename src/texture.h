#pragma once

#include <cstddef>

#include <glad/glad.h>

class Texture2D final
{
public:
    void load(GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels);
    void load(const void *image, size_t size);
    void load(const char *filename);

    constexpr GLuint get() const { return _texture; }

    constexpr int width() const { return _width; }
    constexpr int height() const { return _height; }

    Texture2D &operator=(const Texture2D &) = delete;
    Texture2D &operator=(Texture2D &&other) noexcept;

    Texture2D();
    Texture2D(const Texture2D &) = delete;
    Texture2D(Texture2D &&other) noexcept;
    ~Texture2D();

private:
    GLuint _texture;
    int _width, _height;
};
