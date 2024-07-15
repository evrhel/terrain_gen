#pragma once

#include <cstdint>

#include <glad/glad.h>

#include <mutil/mutil.h>

#include "material.h"

#define NUM_TERRAIN_MATERIALS 5
#define TERRAIN_DIRT_INDEX 0
#define TERRAIN_GRASS_INDEX 1
#define TERRAIN_SNOW_INDEX 2
#define TERRAIN_ROCK_INDEX 3
#define TERRAIN_SAND_INDEX 4

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

    void load(float width, float height, uint32_t resolution, float scale);

    void retain();
    void release();

    constexpr bool enabled() const { return _enabled; }
    constexpr void setEnabled(bool enabled) { _enabled = enabled; }

    constexpr Material *getMaterials() { return _materials; }
    constexpr Material *getMaterial() { return _materials; }

    constexpr bool usesMaterials() const { return _useMaterials; }
    constexpr void setUseMaterials(bool use) { _useMaterials = use; }

    Terrain();
    ~Terrain();

private:
    GLuint _vao, _vbo;
    GLsizei _nVertices;

    float _scale;

    size_t _refs;

    bool _enabled;

    Material _materials[NUM_TERRAIN_MATERIALS];
    bool _useMaterials;
};
