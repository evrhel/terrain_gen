#include "bloom.h"

#include "engine.h"
#include "shader.h"

static constexpr float kFilterRadius = 0.005f;

void Bloom::render(GLuint srcTexture) const
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    renderDownsamples(srcTexture);
    renderUpsamples();
}

void Bloom::load()
{
}

void Bloom::resize(int width, int height)
{
    unload();

    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    IntVector2 size(width, height);
    for (int32_t i = 0; i < BLOOM_DOWNSAMPLES; i++)
    {
        BloomStage &stage = _stages[i];
        size = size / 2;
        stage.size = size;
        stage.fsize = Vector2(size);

        glGenTextures(1, &stage.texture);
        glBindTexture(GL_TEXTURE_2D, stage.texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F, size.x, size.y, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _stages[0].texture, 0);

    const unsigned int attachments[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, attachments);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fatal("Bloom::resize: Framebuffer is not complete!");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    _width = width;
    _height = height;
}

Bloom::Bloom() : _fbo(0),
                 _width(0), _height(0),
                 _filterRadius(kFilterRadius)
{
    memset(_stages, 0, sizeof(_stages));
}

Bloom::~Bloom()
{
    unload();
}

void Bloom::unload()
{
    for (int32_t i = 0; i < BLOOM_DOWNSAMPLES; i++)
    {
        if (_stages[i].texture)
        {
            glDeleteTextures(1, &_stages[i].texture);
            _stages[i].texture = 0;
        }
    }

    if (_fbo)
    {
        glDeleteFramebuffers(1, &_fbo);
        _fbo = 0;
    }
}

void Bloom::renderDownsamples(GLuint srcTexture) const
{
    Shader *shader = getShader(SHADER_DOWNSAMPLE);
    shader->use();

    shader->setVector2("uSize", Vector2(_width, _height));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, srcTexture);
    shader->setInt("uTexture0", 0);

    for (int32_t i = 0; i < BLOOM_DOWNSAMPLES; i++)
    {
        const BloomStage &stage = _stages[i];

        glViewport(0, 0, stage.size.x, stage.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stage.texture, 0);

        drawQuad();

        shader->setVector2("uSize", stage.fsize);
        glBindTexture(GL_TEXTURE_2D, stage.texture);
    }
}

void Bloom::renderUpsamples() const
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glBlendEquation(GL_FUNC_ADD);

    Shader *shader = getShader(SHADER_UPSAMPLE);
    shader->use();

    shader->setFloat("uFilterRadius", _filterRadius);

    glActiveTexture(GL_TEXTURE0);
    shader->setInt("uTexture0", 0);

    for (int32_t i = BLOOM_DOWNSAMPLES - 1; i > 0; i--)
    {
        const BloomStage &stage = _stages[i];
        const BloomStage &nextStage = _stages[i - 1];

        glBindTexture(GL_TEXTURE_2D, stage.texture);

        glViewport(0, 0, nextStage.size.x, nextStage.size.y);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, nextStage.texture, 0);

        drawQuad();
    }

    glDisable(GL_BLEND);
}
