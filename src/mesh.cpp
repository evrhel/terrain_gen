#include "mesh.h"

void Mesh::load(const Vertex *vertex, GLsizei nVertices, const GLuint *index, GLsizei nIndices)
{
    glCreateVertexArrays(1, &_vao);

    glCreateBuffers(1, &_vbo);
    glCreateBuffers(1, &_ebo);

    /* Upload vertices and indices */

    glNamedBufferData(_vbo, nVertices * sizeof(Vertex), vertex, GL_STATIC_DRAW);
    glNamedBufferData(_ebo, nIndices * sizeof(GLuint), index, GL_STATIC_DRAW);

    /* Bind vertex attributes */

    glVertexArrayVertexBuffer(_vao, 0, _vbo, 0, sizeof(Vertex));

    glEnableVertexArrayAttrib(_vao, 0);
    glVertexArrayAttribFormat(_vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position));
    glVertexArrayAttribBinding(_vao, 0, 0);

    glEnableVertexArrayAttrib(_vao, 1);
    glVertexArrayAttribFormat(_vao, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoords));
    glVertexArrayAttribBinding(_vao, 1, 0);

    glEnableVertexArrayAttrib(_vao, 2);
    glVertexArrayAttribFormat(_vao, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
    glVertexArrayAttribBinding(_vao, 2, 0);

    /* Bind indices */

    glVertexArrayElementBuffer(_vao, _ebo);

    _nIndices = nIndices;
}

void Mesh::render() const
{
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _nIndices, GL_UNSIGNED_INT, 0);
}

Mesh::Mesh() : _vao(0), _vbo(0), _ebo(0),
               _nIndices(0)
{
}

Mesh::~Mesh()
{
    if (_ebo)
        glDeleteBuffers(1, &_ebo);

    if (_vbo)
        glDeleteBuffers(1, &_vbo);

    if (_vao)
        glDeleteVertexArrays(1, &_vao);
}
