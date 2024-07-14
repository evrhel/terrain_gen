#pragma once

#include <glad/glad.h>
#include <mutil/mutil.h>

#define BLOOM_DOWNSAMPLES 6

using namespace mutil;

class Shader;

struct BloomStage
{
    IntVector2 size;
    Vector2 fsize;
    GLuint texture;
};

class Bloom
{
public:
    void render(GLuint srcTexture) const;

    void load();

    void resize(int width, int height);

    constexpr GLuint texture() const
    {
        return _stages[0].texture;
    }

    constexpr float filterRadius() const { return _filterRadius; }
    constexpr void setFilterRadius(float radius) { _filterRadius = radius; }

    Bloom();
    ~Bloom();

private:
    GLuint _fbo;
    BloomStage _stages[BLOOM_DOWNSAMPLES];

    int _width, _height;

    float _filterRadius;

    void unload();

    void renderDownsamples(GLuint srcTexture) const;
    void renderUpsamples() const;
};
