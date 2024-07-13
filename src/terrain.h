#pragma once

#include <cstdint>

#include <glad/glad.h>

#include <mutil/mutil.h>

using namespace mutil;

class Shader;

struct TerrainVertex
{
    Vector3 position;
    Vector2 texCoords;
};

class Terrain
{
public:
    void render(Shader *shader) const;

    void load(const char *heightmap, uint32_t resolution);

    void retain();
    void release();

    constexpr bool enabled() const { return _enabled; }
    constexpr void setEnabled(bool enabled) { _enabled = enabled; }

    Terrain();
    ~Terrain();

private:
    GLuint _vao, _vbo;
    GLuint _heightmap;
    GLsizei _nVertices;

    size_t _refs;

    bool _enabled;
};
