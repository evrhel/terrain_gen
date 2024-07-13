#pragma once

#include <glad/glad.h>

enum GbufferTexture
{
    GBUFFER_ALBEDO = 0,
    GBUFFER_EMISSIVE = 1,
    GBUFFER_POSITION = 2,
    GBUFFER_DEPTH = 3,
    GBUFFER_NORMAL = 4,
    GBUFFER_MATERIAL = 5,

    GBUFFER_NUM_TEXTURES
};

// Last GBUFFER_NUM_TEXTURES units are reserved for gbuffer textures
#define GBUFFER_BASE_UNIT (32 - GBUFFER_NUM_TEXTURES)
#define GBUFFER_TEXTURE_UNIT(texture) (GBUFFER_BASE_UNIT + (texture))

class Gbuffer
{
public:
    void load();

    void resize(GLsizei width, GLsizei height);

    constexpr GLuint getTexture(GbufferTexture texture) const
    {
        if (texture < 0 || texture >= GBUFFER_NUM_TEXTURES)
            return 0;
        return _textures[texture];
    }

    inline void bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glViewport(0, 0, _width, _height);
    }

    Gbuffer();
    ~Gbuffer();

private:
    GLuint _fbo, _rbo;
    GLuint _textures[6];

    GLsizei _width, _height;

    void unload();
};
