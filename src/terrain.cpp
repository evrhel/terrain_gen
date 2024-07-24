#include "terrain.h"

#include <cstdlib>
#include <cstdio>
#include <string>

#include <stb_image.h>
#include <half.hpp>

#include "engine.h"
#include "shader.h"

#define NUM_PATCH_PTS 4

void Terrain::render(Shader *shader) const
{
    shader->setMatrix4("uModel", _model);
    shader->setMatrix3("uNormalMatrix", _normalMatrix);

    if (_hasHeightMap)
    {
        shader->setTexture("uHeightmap", _heightMap, 30);
        shader->setTexture("uNormalmap", _normalMap, 31);
    }

    if (_useMaterials)
    {
        for (int i = 0; i < NUM_TERRAIN_MATERIALS; i++)
            shader->setMaterial(i, _materials[i]);
    }
    else
        shader->setMaterial(_materials[0]);

    shader->setFloat("uTime", getTime());

    glBindVertexArray(_vao);
    glDrawArrays(GL_PATCHES, 0, _nVertices);
}

void Terrain::update()
{
    if (!_dirty)
        return;

    _model = Matrix4(1.0f);
    _model = mutil::translate(_model, _position);
    _model = _model * mutil::torotation(_rotation);
    _model = mutil::scale(_model, _scale);

    _invModel = mutil::inverse(_model);

    _normalMatrix = Matrix3(mutil::transpose(_invModel));

    _dirty = false;
}

void Terrain::load(float width, float height, uint32_t resolution)
{
    _hasHeightMap = false;

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

            v1.position = Vector3{-width / 2.0f + width * i / fRes,
                                  0.0f,
                                  -height / 2.0f + height * j / fRes};
            v1.texCoords = Vector2{i / fRes, j / fRes};

            v0.position = Vector3{-width / 2.0f + width * (i + 1) / fRes,
                                  0.0f,
                                  -height / 2.0f + height * j / fRes};
            v0.texCoords = Vector2{(i + 1) / fRes, j / fRes};

            v3.position = Vector3{-width / 2.0f + width * i / fRes,
                                  0.0f,
                                  -height / 2.0f + height * (j + 1) / fRes};
            v3.texCoords = Vector2{i / fRes, (j + 1) / fRes};

            v2.position = Vector3{-width / 2.0f + width * (i + 1) / fRes,
                                  0.0f,
                                  -height / 2.0f + height * (j + 1) / fRes};
            v2.texCoords = Vector2{(i + 1) / fRes, (j + 1) / fRes};
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

    _width = width;
    _height = height;
}

void Terrain::load(const char *folder, uint32_t resolution)
{
    std::string dir(folder);
    std::string infoFile = dir + "/heightmap.txt";
    std::string heightmapFile = dir + "/heightmap.raw";
    std::string normalFile = dir + "/normal.raw";

    /* Read heightmap dimensions */

    FILE *file = fopen(infoFile.c_str(), "r");
    if (!file)
		fatal("Terrain::load: failed to open heightmap.txt");

    int hmWidth, hmHeight;
    int n = fscanf(file, "%d %d", &hmWidth, &hmHeight);
    if (n != 2)
        fatal("Terrain::load: failed to read heightmap dimensions");

    fclose(file);

    /* Setup main terrain */
    load(hmWidth, hmHeight, resolution);

    /* Load heightmap */
    file = fopen(heightmapFile.c_str(), "rb");
    if (!file)
		fatal("Terrain::load: failed to open heightmap.raw");

    size_t size = hmWidth * hmHeight * sizeof(half_float::half);
    half_float::half *data = (half_float::half *)malloc(size);
    if (!data)
		fatal("Terrain::load: failed to allocate heightmap data");

    size_t nRead = fread(data, 1, size, file);
    if (nRead != size)
		fatal("Terrain::load: failed to read heightmap data");

    fclose(file);

    /* Create heightmap texture */
    glGenTextures(1, &_heightMap);
    glBindTexture(GL_TEXTURE_2D, _heightMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R16F, hmWidth, hmHeight, 0, GL_RED, GL_HALF_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(data);

    /* Load normalmap */
    file = fopen(normalFile.c_str(), "rb");
    if (!file)
        fatal("Terrain::load: failed to open normal.raw");

    size = hmWidth * hmHeight * 3 * sizeof(half_float::half);
    data = (half_float::half *)malloc(size);
    if (!data)
		fatal("Terrain::load: failed to allocate normalmap data");

    nRead = fread(data, 1, size, file);
    if (nRead != size)
        fatal("Terrain::load: failed to read normalmap data");

    fclose(file);

    /* Create normalmap texture */
    glGenTextures(1, &_normalMap);
    glBindTexture(GL_TEXTURE_2D, _normalMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, hmWidth, hmHeight, 0, GL_RGB, GL_HALF_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    free(data);

    _hasHeightMap = true;
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
                     _nVertices(0),
                     _width(0.0f), _height(0.0f),
                     _refs(1),
                     _enabled(true),
                     _hasHeightMap(false),
                     _heightMap(0), _normalMap(0),
                     _useMaterials(true),
                     _dirty(true)
{
}

Terrain::~Terrain()
{
    if (_normalMap)
        glDeleteTextures(1, &_normalMap);

    if (_heightMap)
        glDeleteTextures(1, &_heightMap);

    if (_vbo)
        glDeleteBuffers(1, &_vbo);

    if (_vao)
        glDeleteVertexArrays(1, &_vao);
}
