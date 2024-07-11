#pragma once

#include <glad/glad.h>
#include <mutil/mutil.h>

#include "material.h"

using namespace mutil;

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

    void render() const;

    constexpr Material *getMaterial() { return &_material; }

    Mesh();
    ~Mesh();

private:
    GLuint _vao, _vbo, _ebo;
    GLsizei _nIndices;

    Material _material;
};
