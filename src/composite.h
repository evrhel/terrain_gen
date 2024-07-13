#pragma once

#include <glad/glad.h>

#define MAX_COMPOSITOR_OUTPUTS 4

class Shader;
class Gbuffer;

struct OutputSpec
{
    GLenum format;
    GLenum internalFormat;
    GLenum type;
};

class Compositor
{
public:
    void render(Shader *shader, const Gbuffer *gbuffer, const Compositor *last) const;

    void load(const OutputSpec *outputs, GLsizei nOutputs);

    void resize(GLsizei width, GLsizei height);

    inline void bind() const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glViewport(0, 0, _width, _height);
    }

    constexpr GLuint getTexture(GLsizei index) const
    {
        if (index < 0 || index >= _nOutputs)
            return 0;
        return _textures[index];
    }

    Compositor();
    ~Compositor();

private:
    GLuint _fbo;
    GLuint _textures[MAX_COMPOSITOR_OUTPUTS];

    OutputSpec *_outputs;
    GLsizei _nOutputs;

    GLsizei _width, _height;

    void unload();
};
