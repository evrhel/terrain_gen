#include "composite.h"

#include <cstring>
#include <cassert>

#include "engine.h"
#include "gbuffer.h"
#include "shader.h"
#include "util.h"

static void setupAttachment(const OutputSpec &spec, GLsizei width, GLsizei height)
{
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        spec.internalFormat,
        width, height,
        0,
        spec.format,
        spec.type,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    checkGLErrors("setupAttachment");
}

void Compositor::render(Shader *shader, const Gbuffer *gbuffer, const Compositor *last) const
{
    /* Set gbuffer uniforms */
    shader->setGbuffer(gbuffer);

    /* Set outputs from last compositor stage */
    if (last)
    {
        switch (last->_nOutputs)
        {
        default:
            fatal("Compositor::render: Too many outputs!");
        case 4:
            shader->setTexture("uTexture3", last->_textures[3], 3);
        case 3:
            shader->setTexture("uTexture2", last->_textures[2], 2);
        case 2:
            shader->setTexture("uTexture1", last->_textures[1], 1);
        case 1:
            shader->setTexture("uTexture0", last->_textures[0], 0);
        case 0:
            break;
        }
    }

    drawQuad();
}

void Compositor::load(const OutputSpec *outputs, GLsizei nOutputs)
{
    if (outputs)
    {
        _nOutputs = nOutputs;
        _outputs = new OutputSpec[_nOutputs];
        memcpy(_outputs, outputs, sizeof(OutputSpec) * _nOutputs);
    }
    else
    {
        _nOutputs = 0;
        _outputs = nullptr;
    }
}

void Compositor::resize(GLsizei width, GLsizei height)
{
    unload();

    _width = width;
    _height = height;

    if (_nOutputs == 0)
        return; // Output to default framebuffer
    
    glGenFramebuffers(1, &_fbo);
    glGenTextures(_nOutputs, _textures);

    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    for (GLsizei i = 0; i < _nOutputs; i++)
    {
        GLuint tex = _textures[i];
        const OutputSpec &spec = _outputs[i];

        glBindTexture(GL_TEXTURE_2D, tex);
        setupAttachment(spec, width, height);

        glFramebufferTexture2D(
            GL_FRAMEBUFFER,
            GL_COLOR_ATTACHMENT0 + i,
            GL_TEXTURE_2D,
            tex,
            0);
    }

    GLenum drawBuffers[MAX_COMPOSITOR_OUTPUTS];
    for (GLsizei i = 0; i < _nOutputs; i++)
        drawBuffers[i] = GL_COLOR_ATTACHMENT0 + i;

    glDrawBuffers(_nOutputs, drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fatal("Compositor::resize: Framebuffer is not complete!");
}

Compositor::Compositor() : _fbo(0),
                           _outputs(nullptr), _nOutputs(0),
                           _width(0), _height(0)
{
    memset(_textures, 0, sizeof(_textures));
}

Compositor::~Compositor()
{
    unload();

    if (_outputs)
        delete[] _outputs;
}

void Compositor::unload()
{
    if (!_fbo)
        return;

    glDeleteTextures(_nOutputs, _textures);
    memset(_textures, 0, sizeof(_textures));

    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
}