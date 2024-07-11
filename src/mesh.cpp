#include "mesh.h"

#include <utility>

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

void Mesh::update()
{
    if (!_dirty)
        return;

    _model = Matrix4(1.0f);
    _model = mutil::translate(_model, _position);
    _model = _model * mutil::torotation(_rotation);
    _model = mutil::scale(_model, _scale);

    _invModel = mutil::inverse(_model);

    _dirty = false;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept
{
    if (this == &other)
        return *this;

    if (_ebo)
        glDeleteBuffers(1, &_ebo);

    if (_vbo)
        glDeleteBuffers(1, &_vbo);

    if (_vao)
        glDeleteVertexArrays(1, &_vao);

    _vao = other._vao;
    _vbo = other._vbo;
    _ebo = other._ebo;
    _nIndices = other._nIndices;

    _material = std::move(other._material);

    other._vbo = 0;
    other._ebo = 0;
    other._vao = 0;

    return *this;
}

Mesh::Mesh() : _vao(0), _vbo(0), _ebo(0),
               _nIndices(0),
               _scale(1.0f),
               _dirty(true)
{
}

Mesh::Mesh(Mesh &&other) noexcept : _vao(other._vao), _vbo(other._vbo), _ebo(other._ebo),
                                    _nIndices(other._nIndices),
                                    _material(std::move(other._material)),
                                    _position(other._position),
                                    _rotation(other._rotation),
                                    _scale(other._scale),
                                    _dirty(other._dirty),
                                    _model(other._model), _invModel(other._invModel)
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
