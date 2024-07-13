#include "terrain.h"

#include <cstdlib>
#include <cstdio>

#include <stb_image.h>

#include "engine.h"
#include "shader.h"

#define NUM_PATCH_PTS 4

#define HEIGHTMAP_UNIT 10

void Terrain::render(Shader *shader) const
{
    shader->setMatrix4("uModel", Matrix4(1.0f));

    shader->setTexture("uHeightmap", _heightmap, HEIGHTMAP_UNIT);

    glBindVertexArray(_vao);
    glDrawArrays(GL_PATCHES, 0, _nVertices);
}

void Terrain::load(const char *heightmap, uint32_t resolution)
{
    /* Load heightmap */

    int width, height, nChannels;
    stbi_uc *data = stbi_load(heightmap, &width, &height, &nChannels, 1);
    if (!data)
        fatal("Terrain::load: failed to load heightmap %s", heightmap);

    glGenTextures(1, &_heightmap);
    glBindTexture(GL_TEXTURE_2D, _heightmap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glGenerateMipmap(GL_TEXTURE_2D);

    /* Generate vertices */

    _nVertices = resolution * resolution * NUM_PATCH_PTS;
    TerrainVertex *vertices = (TerrainVertex *)malloc(sizeof(TerrainVertex) * _nVertices);
    if (!vertices)
        fatal("Terrain::load: failed to allocate vertices");

    float fRes = (float)resolution;
    for (uint32_t i = 0; i < resolution; i++)
    {
        uint32_t off0 = i * resolution * 4;
        for (uint32_t j = 0; j < resolution; j++)
        {
            uint32_t offset = off0 + j * 4;
            TerrainVertex &v0 = vertices[offset + 0];
            TerrainVertex &v1 = vertices[offset + 1];
            TerrainVertex &v2 = vertices[offset + 2];
            TerrainVertex &v3 = vertices[offset + 3];

            v0.position = Vector3{-width / 2.0f + width * i / fRes,
                                  0.0f,
                                  -height / 2.0f + height * j / fRes};
            v0.texCoords = Vector2{i / fRes, j / fRes};

            v1.position = Vector3{-width / 2.0f + width * (i + 1) / fRes,
                                  0.0f,
                                  -height / 2.0f + height * j / fRes};
            v1.texCoords = Vector2{(i + 1) / fRes, j / fRes};

            v2.position = Vector3{-width / 2.0f + width * i / fRes,
                                  0.0f,
                                  -height / 2.0f + height * (j + 1) / fRes};
            v2.texCoords = Vector2{i / fRes, (j + 1) / fRes};

            v3.position = Vector3{-width / 2.0f + width * (i + 1) / fRes,
                                  0.0f,
                                  -height / 2.0f + height * (j + 1) / fRes};
            v3.texCoords = Vector2{(i + 1) / fRes, (j + 1) / fRes};
        }
    }

    /* Create vertex array */

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(TerrainVertex) * _nVertices, vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void *)offsetof(TerrainVertex, position));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TerrainVertex), (void *)offsetof(TerrainVertex, texCoords));
    glEnableVertexAttribArray(1);

    glPatchParameteri(GL_PATCH_VERTICES, NUM_PATCH_PTS);

    free(vertices);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Terrain::retain()
{
    _refs++;
}

void Terrain::release()
{
    if (--_refs == 0)
        delete this;
}

Terrain::Terrain() : _vao(0), _vbo(0),
                     _heightmap(0), _nVertices(0),
                     _refs(1),
                     _enabled(true)
{
}

Terrain::~Terrain()
{
    if (_vbo)
        glDeleteBuffers(1, &_vbo);

    if (_vao)
        glDeleteVertexArrays(1, &_vao);

    if (_heightmap)
        glDeleteTextures(1, &_heightmap);
}
