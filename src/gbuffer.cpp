#include "gbuffer.h"

#include <cstring>
#include <cstdint>

#include "engine.h"
#include "util.h"

void Gbuffer::load()
{
}

static void setupGbufferTexture(GLuint texture, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type)
{
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Gbuffer::resize(GLsizei width, GLsizei height)
{
    unload();
    
    _width = width;
    _height = height;

    glGenFramebuffers(1, &_fbo);
    glGenRenderbuffers(1, &_rbo);
    glGenTextures(6, _textures);

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    /* Albedo */
    setupGbufferTexture(
        _textures[GBUFFER_ALBEDO],
        GL_R11F_G11F_B10F,
        width, height,
        GL_RGB,
        GL_FLOAT);

    /* Emissive */
    setupGbufferTexture(
        _textures[GBUFFER_EMISSIVE],
        GL_R11F_G11F_B10F,
        width, height,
        GL_RGB,
        GL_FLOAT);

    /* Position */
    setupGbufferTexture(
        _textures[GBUFFER_POSITION],
        GL_RGB32F,
        width, height,
        GL_RGB,
        GL_FLOAT);

    /* Depth */
    setupGbufferTexture(
        _textures[GBUFFER_DEPTH],
        GL_R32F,
        width, height,
        GL_RED,
        GL_FLOAT);

    /* Normal */
    setupGbufferTexture(
        _textures[GBUFFER_NORMAL],
        GL_RGB16F,
        width, height,
        GL_RGB,
        GL_FLOAT);

    /* Material */
    setupGbufferTexture(
        _textures[GBUFFER_MATERIAL],
        GL_RGB8,
        width, height,
        GL_RGB,
        GL_UNSIGNED_BYTE);

    /* Attach to framebuffer */
    for (int32_t i = 0; i < GBUFFER_NUM_TEXTURES; i++)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, _textures[i], 0);

    glBindRenderbuffer(GL_RENDERBUFFER, _rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _rbo);

    GLenum drawBuffers[GBUFFER_NUM_TEXTURES];
    for (int32_t i = 0; i < GBUFFER_NUM_TEXTURES; i++)
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

    glDrawBuffers(GBUFFER_NUM_TEXTURES, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fatal("Gbuffer::resize: Framebuffer is not complete");

    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Gbuffer::Gbuffer() : _fbo(0), _rbo(0)
{
    memset(_textures, 0, sizeof(_textures));
}

Gbuffer::~Gbuffer()
{
    unload();
}

void Gbuffer::unload()
{
    if (_fbo)
    {
        glDeleteTextures(6, _textures);
        memset(_textures, 0, sizeof(_textures));

        glDeleteRenderbuffers(1, &_rbo);
        _rbo = 0;

        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }
}
