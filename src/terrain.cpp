#include "terrain.h"

#include <cstdlib>
#include <cstdio>

#include <stb_image.h>
#include <half.hpp>

#include "engine.h"
#include "shader.h"

#define NUM_PATCH_PTS 4

#define HEIGHTMAP_UNIT 10
#define NORMALMAP_UNIT 11

struct Heightmap
{
    stbi_uc *data;
    int width, height;
};

struct half3
{
    half_float::half x, y, z;
};

struct NormalMap
{
    half3 *data;
    int width, height;
};

static void loadHeightmap(Heightmap &hm, const char *name)
{
    int nChannels;
    hm.data = stbi_load(name, &hm.width, &hm.height, &nChannels, 1);
    if (!hm.data)
        fatal("loadHeightmap: failed to load heightmap %s", name);
}

static void freeHeightmap(Heightmap &hm)
{
    stbi_image_free(hm.data);
}

static constexpr float fetchHeightmap(const Heightmap &hm, int32_t x, int32_t y)
{
    x = mutil::clamp(x, 0, hm.width - 1);
    y = mutil::clamp(y, 0, hm.height - 1);
    return hm.data[y * hm.width + x] / 255.0f;
}

static constexpr float sampleHeightmap(const Heightmap &hm, int32_t x, int32_t y)
{
    float a = fetchHeightmap(hm, x - 1, y - 1);
    float b = fetchHeightmap(hm, x, y - 1);
    float c = fetchHeightmap(hm, x + 1, y - 1);

    float d = fetchHeightmap(hm, x - 1, y);
    float e = fetchHeightmap(hm, x, y);
    float f = fetchHeightmap(hm, x + 1, y);

    float g = fetchHeightmap(hm, x - 1, y + 1);
    float h = fetchHeightmap(hm, x, y + 1);
    float i = fetchHeightmap(hm, x + 1, y + 1);

    return (4.0f * e + 2.0f * (b + d + f + h) + (a + c + g + i)) / 16.0f;
}

static void generateNormalMap(NormalMap &nm, const Heightmap &hm, float scale)
{
    nm.data = (half3 *)malloc(sizeof(half3) * hm.width * hm.height);
    if (!nm.data)
        fatal("generateNormalMap: failed to allocate data");

    nm.width = hm.width;
    nm.height = hm.height;

    for (int32_t i = 0; i < nm.width; i++)
    {
        for (int32_t j = 0; j < nm.height; j++)
        {
            Vector3 a = Vector3(
                i,
                sampleHeightmap(hm, i, j) * scale,
                j);

            Vector3 b = Vector3(
                i + 1,
                sampleHeightmap(hm, i + 1, j) * scale,
                j);

            Vector3 c = Vector3(
                i,
                sampleHeightmap(hm, i, j + 1) * scale,
                j + 1);

            Vector3 normal = -normalize(cross(b - a, c - a));

            half3 &pixel = nm.data[j * hm.width + i];
            pixel.x = normal.x;
            pixel.y = normal.y;
            pixel.z = normal.z;
        }
    }
}

static void freeNormalMap(NormalMap &nm)
{
    free(nm.data);
}

void Terrain::render(Shader *shader) const
{
    shader->setMatrix4("uModel", Matrix4(1.0f));

    shader->setTexture("uHeightmap", _heightmap, HEIGHTMAP_UNIT);
    shader->setTexture("uNormalmap", _normalMap, NORMALMAP_UNIT);
    shader->setFloat("uScale", _scale);

    shader->setMaterial(_material);

    glBindVertexArray(_vao);
    glDrawArrays(GL_PATCHES, 0, _nVertices);
}

void Terrain::load(const char *heightmap, uint32_t resolution, float scale)
{
    _scale = scale;

    /* Load heightmap */
    Heightmap hm;
    loadHeightmap(hm, heightmap);

    glGenTextures(1, &_heightmap);
    glBindTexture(GL_TEXTURE_2D, _heightmap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, hm.width, hm.height, 0, GL_RED, GL_UNSIGNED_BYTE, hm.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Generate normal map */
    NormalMap nm;
    generateNormalMap(nm, hm, scale);

    glGenTextures(1, &_normalMap);
    glBindTexture(GL_TEXTURE_2D, _normalMap);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, nm.width, nm.height, 0, GL_RGB, GL_HALF_FLOAT, nm.data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    freeNormalMap(nm);
    freeHeightmap(hm);

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

            v0.position = Vector3{-hm.width / 2.0f + hm.width * i / fRes,
                                  0.0f,
                                  -hm.height / 2.0f + hm.height * j / fRes};
            v0.texCoords = Vector2{i / fRes, j / fRes};

            v1.position = Vector3{-hm.width / 2.0f + hm.width * (i + 1) / fRes,
                                  0.0f,
                                  -hm.height / 2.0f + hm.height * j / fRes};
            v1.texCoords = Vector2{(i + 1) / fRes, j / fRes};

            v2.position = Vector3{-hm.width / 2.0f + hm.width * i / fRes,
                                  0.0f,
                                  -hm.height / 2.0f + hm.height * (j + 1) / fRes};
            v2.texCoords = Vector2{i / fRes, (j + 1) / fRes};

            v3.position = Vector3{-hm.width / 2.0f + hm.width * (i + 1) / fRes,
                                  0.0f,
                                  -hm.height / 2.0f + hm.height * (j + 1) / fRes};
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
                     _nVertices(0),
                     _heightmap(0), _normalMap(0),
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

    if (_normalMap)
        glDeleteTextures(1, &_normalMap);

    if (_heightmap)
        glDeleteTextures(1, &_heightmap);
}
