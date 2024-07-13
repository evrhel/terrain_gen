#pragma once

#include <cstdint>

#include <glad/glad.h>

#include <mutil/mutil.h>

#include "material.h"

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

    void load(const char *heightmap, uint32_t resolution, float scale);

    void retain();
    void release();

    constexpr bool enabled() const { return _enabled; }
    constexpr void setEnabled(bool enabled) { _enabled = enabled; }

    constexpr Material *getMaterial() { return &_material; }

    Terrain();
    ~Terrain();

private:
    GLuint _vao, _vbo;
    GLsizei _nVertices;

    GLuint _heightmap;
    GLuint _normalMap;

    float _scale;

    size_t _refs;

    bool _enabled;

    Material _material;
};
