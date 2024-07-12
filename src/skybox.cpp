#include "skybox.h"

#include <cstdio>

#include "util.h"

struct SkyboxGPU
{
    Vector3 sunDirection;
    float _pad0;

    Vector3 sunColor;
    float _pad1;

    float sunIntensity;

    float sunTightness;
    float _pad2[2];

    Vector3 horizonColor;
    float _pad3;

    Vector3 zenithColor;
    float _pad4;

    float atmosphereHeight;
    float _pad5[3];
};

static const Vector3 kSkyboxVertices[] = {
    {-1.0f, -1.0f, -1.0f},
    {1.0f, -1.0f, -1.0f},
    {1.0f, 1.0f, -1.0f},
    {-1.0f, 1.0f, -1.0f},
    {-1.0f, -1.0f, 1.0f},
    {1.0f, -1.0f, 1.0f},
    {1.0f, 1.0f, 1.0f},
    {-1.0f, 1.0f, 1.0f},
};

static const unsigned int kSkyboxIndices[] = {
    0, 1, 2, 2, 3, 0,
    1, 5, 6, 6, 2, 1,
    7, 6, 5, 5, 4, 7,
    4, 0, 3, 3, 7, 4,
    4, 5, 1, 1, 0, 4,
    3, 2, 6, 6, 7, 3,
};

#define SKYBOX_INDEX_COUNT 36

void Skybox::render() const
{
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, SKYBOX_INDEX_COUNT, GL_UNSIGNED_INT, 0);
}

void Skybox::load()
{
    /* Skybox mesh */

    glGenVertexArrays(1, &_vao);
    glGenBuffers(1, &_vbo);
    glGenBuffers(1, &_ebo);
    
    glBindVertexArray(_vao);

    /* Upload vertices */

    glBindBuffer(GL_ARRAY_BUFFER, _vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kSkyboxVertices), kSkyboxVertices, GL_STATIC_DRAW);

    /* Upload indices */

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(kSkyboxIndices), kSkyboxIndices, GL_STATIC_DRAW);

    /* Bind vertex attributes */

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    /* Atmosphere buffer */
    glGenBuffers(1, &_ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(SkyboxGPU), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, ATMOSPHERE_UNIFORM_BINDING, _ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Skybox::update()
{
    if (!_dirty)
        return;

    /* Update sun direction */
    Quaternion q1 = mutil::rotateaxis(kWorldRight, mutil::radians(_altitude));
    Quaternion q2 = mutil::rotateaxis(kWorldUp, mutil::radians(_azimuth));
    Quaternion q = q1 * q2;
    _sunDirection = mutil::rotatevector(q, kWorldFront);

    upload();

    _dirty = false;
}

Skybox::Skybox() : _altitude(0.0f), _azimuth(0.0f),
                   _sunColor(1.0f),
                   _sunIntensity(1.0f),
                   _sunTightness(500.0f),
                   _horizonColor(0.1f), _zenithColor(0.0f),
                   _atmosphereHeight(0.0f),
                   _dirty(true),
                   _ubo(0)
{
}

Skybox::~Skybox()
{
    if (_ubo)
        glDeleteBuffers(1, &_ubo);
}

void Skybox::upload() const
{
    /* Upload data to GPU */

    uint8_t buf[sizeof(SkyboxGPU)];
    SkyboxGPU *sb = (SkyboxGPU *)buf;

    sb->sunDirection = _sunDirection;
    sb->sunColor = _sunColor;
    sb->sunIntensity = _sunIntensity;
    sb->sunTightness = _sunTightness;
    sb->horizonColor = _horizonColor;
    sb->zenithColor = _zenithColor;
    sb->atmosphereHeight = _atmosphereHeight;

    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SkyboxGPU), buf);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
