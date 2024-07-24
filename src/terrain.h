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
    void update();

    void load(float width, float height, uint32_t resolution);
    void load(const char *folder, uint32_t resolution);

    void retain();
    void release();

    constexpr float width() const { return _width; }
    constexpr float height() const { return _height; }

    constexpr bool enabled() const { return _enabled; }
    constexpr void setEnabled(bool enabled) { _enabled = enabled; }

    constexpr Material *getMaterials() { return _materials; }
    constexpr Material *getMaterial() { return _materials; }

    constexpr bool usesMaterials() const { return _useMaterials; }
    constexpr void setUseMaterials(bool use) { _useMaterials = use; }

    constexpr void setPosition(const Vector3 &position)
    {
        _position = position;
        _dirty = true;
    }

    constexpr const Vector3 &position() const { return _position; }

    constexpr void setRotation(const Quaternion &rotation)
    {
		_rotation = rotation;
		_dirty = true;
	}

    constexpr const Quaternion &rotation() const { return _rotation; }

    constexpr void setScale(const Vector3 &scale)
    {
		_scale = scale;
		_dirty = true;
	}

    constexpr const Vector3 &scale() const { return _scale; }

    constexpr const Matrix4 &model() const { return _model; }
    constexpr const Matrix4 &invModel() const { return _invModel; }

    Terrain();
    ~Terrain();

private:
    GLuint _vao, _vbo;
    GLsizei _nVertices;

    float _width, _height;

    size_t _refs;

    bool _enabled;

    bool _hasHeightMap;
    GLuint _heightMap, _normalMap;

    Material _materials[NUM_TERRAIN_MATERIALS];
    bool _useMaterials;

    Vector3 _position;
    Quaternion _rotation;
    Vector3 _scale;
    bool _dirty;

    Matrix4 _model, _invModel;
    Matrix3 _normalMatrix;
};
