#include "mesh.h"

#include <utility>
#include <cassert>

#include "shader.h"

void Mesh::load(const Vertex *vertex, GLsizei nVertices, const GLuint *index, GLsizei nIndices)
{
    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    /* Upload vertices */

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, nVertices * sizeof(Vertex), vertex, GL_STATIC_DRAW);

    /* Upload indices */

    glGenBuffers(1, &_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, nIndices * sizeof(GLuint), index, GL_STATIC_DRAW);

    /* Bind vertex attributes */

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoords));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    _nIndices = nIndices;
}

void Mesh::render(Shader *shader) const
{
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _nIndices, GL_UNSIGNED_INT, 0);
}

void Mesh::retain()
{
    _refs++;
}

void Mesh::release()
{
    if (--_refs == 0)
        delete this;
}

Mesh::Mesh() : _vao(0), _vbo(0), _ebo(0),
               _nIndices(0),
               _refs(1)
{
}

Mesh::~Mesh()
{
    assert(_refs == 0);

    if (_ebo)
        glDeleteBuffers(1, &_ebo);

    if (_vbo)
        glDeleteBuffers(1, &_vbo);

    if (_vao)
        glDeleteVertexArrays(1, &_vao);
}

void RenderableMesh::render(Shader *shader) const
{
    shader->setMatrix4("uModel", _model);
    shader->setMatrix4("uInvModel", _invModel);
    shader->setMaterial(_material);

    _mesh->render(shader);
}

void RenderableMesh::update()
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

RenderableMesh::RenderableMesh(Mesh *mesh) : _mesh(mesh),
                                             _enabled(true),
                                             _position(0.0f), _rotation(), _scale(1.0f),
                                             _dirty(true),
                                             _refs(1)
{
    assert(mesh != nullptr);
    mesh->retain();
}

RenderableMesh::~RenderableMesh()
{
    assert(_refs == 0);
    _mesh->release();
}

void RenderableMesh::retain()
{
    _refs++;
}

void RenderableMesh::release()
{
    if (--_refs == 0)
        delete this;
}
