#pragma once

#include <cstddef>

#include <glad/glad.h>
#include <mutil/mutil.h>

#include "material.h"

using namespace mutil;

class Shader;

struct Vertex
{
    Vector3 position;
    Vector2 texCoords;
    Vector3 normal;
};

class Mesh
{
public:
    void load(const Vertex *vertex, GLsizei nVertices, const GLuint *index, GLsizei nIndices);

    void render(Shader *shader) const;

    void retain();
    void release();

    Mesh();
    ~Mesh();

private:
    GLuint _vao, _vbo, _ebo;
    GLsizei _nIndices;

    size_t _refs;
};

class RenderableMesh
{
public:
    void render(Shader *shader) const;

    void update();

    constexpr bool enabled() const { return _enabled; }
    constexpr void setEnabled(bool enabled) { _enabled = enabled; }

    constexpr Material *getMaterial() { return &_material; }

    constexpr const Vector3 &position() const { return _position; }

    constexpr void setPosition(const Vector3 &position)
    {
        _position = position;
        _dirty = true;
    }

    constexpr const Quaternion &rotation() const { return _rotation; }

    constexpr void setRotation(const Quaternion &rotation)
    {
        _rotation = rotation;
        _dirty = true;
    }

    constexpr const Vector3 &scale() const { return _scale; }

    constexpr void setScale(const Vector3 &scale)
    {
        _scale = scale;
        _dirty = true;
    }

    constexpr const Matrix4 &model() const { return _model; }
    constexpr const Matrix4 &invModel() const { return _invModel; }

    void retain();
    void release();

    RenderableMesh(Mesh *mesh);
    ~RenderableMesh();

private:
    Mesh *_mesh;

    bool _enabled;

    Material _material;

    Vector3 _position;
    Quaternion _rotation;
    Vector3 _scale;
    bool _dirty;

    Matrix4 _model, _invModel;

    size_t _refs;
};
