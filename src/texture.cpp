#include "texture.h"

#include <cstdio>
#include <cstdint>

#include <stb_image.h>

void Texture2D::load(GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const void *pixels)
{
    glGenTextures(1, &_texture);

    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::load(const void *image, size_t size)
{
    int channels;

    stbi_uc *pixels = stbi_load_from_memory((const stbi_uc *)image, size, &_width, &_height, &channels, 0);
    if (!pixels)
    {
        printf("Texture2D::Load: Failed to load image\n");
        return;
    }

    GLenum internalformat, format;
    switch (channels)
    {
    default:
        printf("Texture2D::Load: Unsupported number of channels: %d\n", channels);
        stbi_image_free(pixels);
        return;
    case 1:
        internalformat = GL_R8;
        format = GL_RED;
        break;
    case 2:
        internalformat = GL_RG8;
        format = GL_RG;
        break;
    case 3:
        internalformat = GL_RGB8;
        format = GL_RGB;
        break;
    case 4:
        internalformat = GL_RGBA8;
        format = GL_RGBA;
        break;
    }

    load(internalformat, _width, _height, format, GL_UNSIGNED_BYTE, pixels);

    stbi_image_free(pixels);
}

void Texture2D::load(const char *filename)
{
    printf("Texture2D::load: %s\n", filename);

    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        printf("Texture2D::Load: Failed to open file: %s\n", filename);
        return;
    }

    /* Get file size */
    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    /* Read file */
    uint8_t *buf = new uint8_t[size];
    if (fread(buf, 1, size, f) != size)
    {
        printf("Texture2D::Load: Failed to read file: %s\n", filename);
        delete[] buf;
        fclose(f);
        return;
    }

    load(buf, size);

    delete[] buf;
}

Texture2D::Texture2D() : _texture(0), _width(0), _height(0)
{
}

Texture2D::~Texture2D()
{
    if (_texture)
        glDeleteTextures(1, &_texture);
}
