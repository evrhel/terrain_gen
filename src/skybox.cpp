#include "skybox.h"

#include <cstdio>

#include "util.h"
#include "camera.h"
#include "shader.h"
#include "engine.h"

struct SkyboxGPU
{
    Vector3 sunDirection;
    float _pad0;

    Vector3 sunColor;
    float _pad1;

    Vector3 sunPosition;
    float _pad2;

    Vector3 sunPositionWorld;

    float sunIntensity;
    //float _pad2;

    float sunTightness;
    float _pad3[3];

    Vector3 horizonColor;
    float _pad4;

    Vector3 zenithColor;

    float fogDensity;

    float planetRadius;
    float atmosphereRadius;
    float Hr;
    float Hm;
    float g;
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

static const Vector3 kQuadVertices[] = {
	{-1.0f, -1.0f, 0.0f},
	{1.0f, -1.0f, 0.0f},
	{1.0f, 1.0f, 0.0f},
    {1.0f, 1.0f, 0.0f},
    {-1.0f, 1.0f, 0.0f},
    {-1.0f, -1.0f, 0.0f}
};

/* Wavelength of sun */
constexpr Vector3 kWavelength = Vector3(0.65f, 0.57f, 0.475f);
const Vector3 kInvWavelength = Vector3(
    1.0f / powf(kWavelength.x, 4.0),
    1.0f / powf(kWavelength.y, 4.0),
    1.0f / powf(kWavelength.z, 4.0)
);

#define SKYBOX_INDEX_COUNT 36

#define SKYBOX_RESOLUTION 512
#define IRRADIANCE_RESOLUTION 32

static void createCubemapRenderer(GLsizei res, GLuint *fbo, GLuint *tex)
{
    /* Initialize framebuffer */
    glGenFramebuffers(1, fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, *fbo);

    /* Initialize cubemap */
    glGenTextures(1, tex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *tex);

    for (int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, res, res, 0, GL_RGB, GL_HALF_FLOAT, NULL);
    checkGLErrors("createCubemapRenderer: glTexImage2D");

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, *tex, 0);
    checkGLErrors("createCubemapRenderer: glFramebufferTexture");

    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        fatal("Skybox::setupSkybox: Framebuffer is not complete!\n");

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

static inline Matrix3 getSkyProjViewMatrix(int side)
{
    float pitch, yaw;

    switch (side)
    {
    case 0: // Positive X
        pitch = 0.0f;
        yaw = 90.0f;
        break;
    case 1: // Negative X
        pitch = 0.0f;
        yaw = -90.0f;
        break;
    case 2: // Positive Y
        pitch = -90.0f;
        yaw = 0.0f;
        break;
    case 3: // Negative Y
        pitch = 90.0f;
        yaw = 0.0f;
        break;
    case 4: // Positive Z
        pitch = 0.0f;
        yaw = 0.0f;
        break;
    case 5: // Negative Z
        pitch = 0.0f;
        yaw = 180.0f;
        break;
    default:
        fatal("getSkyProjViewMatrix: Invalid side %d\n", side);

    }

    Matrix4 view;
    view = rotate(view, radians(pitch), kWorldRight);
    view = rotate(view, radians(yaw), kWorldUp);
    view = scale(view, Vector3(1.0f, -1.0f, 1.0f));

    Matrix4 projView = view;// proj * view;

    return Matrix3(projView);
}

static const Matrix3 kSkyMatrices[6] = {
	getSkyProjViewMatrix(0),
	getSkyProjViewMatrix(1),
	getSkyProjViewMatrix(2),
	getSkyProjViewMatrix(3),
	getSkyProjViewMatrix(4),
	getSkyProjViewMatrix(5),
};

void Skybox::renderSkybox() const
{
    Shader *shader = getShader(SHADER_SKYDOME);
    shader->use();

    glDepthFunc(GL_LEQUAL);

    glBindFramebuffer(GL_FRAMEBUFFER, _cubemapFBO);
    glViewport(0, 0, SKYBOX_RESOLUTION, SKYBOX_RESOLUTION);

    shader->setMatrix3("uViews[0]", kSkyMatrices[0]);
    shader->setMatrix3("uViews[1]", kSkyMatrices[1]);
    shader->setMatrix3("uViews[2]", kSkyMatrices[2]);
    shader->setMatrix3("uViews[3]", kSkyMatrices[3]);
    shader->setMatrix3("uViews[4]", kSkyMatrices[4]);
    shader->setMatrix3("uViews[5]", kSkyMatrices[5]);

    glBindVertexArray(_skyQuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    checkGLErrors("Skybox::renderSkybox");
}

void Skybox::renderIrradiance() const
{
    // TODO: Implement
}

void Skybox::render(Shader *shader) const
{
    shader->setVector2("uResolution", Vector2(getWindowSize()));

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

    /* Setup quad */
    glGenVertexArrays(1, &_skyQuadVAO);
    glGenBuffers(1, &_skyQuadVBO);

    glBindVertexArray(_skyQuadVAO);

    glBindBuffer(GL_ARRAY_BUFFER, _skyQuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(kQuadVertices), kQuadVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void *)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    createCubemapRenderer(SKYBOX_RESOLUTION, &_cubemapFBO, &_cubemap);
    createCubemapRenderer(IRRADIANCE_RESOLUTION, &_irradianceFBO, &_irradiance);
}

bool Skybox::update(const Camera *camera)
{
    if (!_dirty)
        return false;

    /* Update sun direction */
    Quaternion q1 = mutil::rotateaxis(kWorldUp, mutil::radians(_azimuth));
    Quaternion q2 = mutil::rotateaxis(kWorldRight, mutil::radians(_altitude));
    Quaternion q = q1 * q2;
    _sunDirection = mutil::rotatevector(q, kWorldFront);

    /* Compute sun position */
    _sunPositionWorld = -_sunDirection * _atmosphereRadius * 1.1f; // Sun position in world space

    /* Compute screen-space sun position */
    //Vector3 sunViewDir = Matrix3(camera->view()) * _sunDirection; // Sun direction in view space
    //Vector3 sunPosView = sunViewDir * (camera->far() - camera->near()); // Sun position in view space

    //Vector4 sunPosNDC = camera->proj() * Vector4(sunPosView, 1.0f);

    //_sunPosition = Vector3(sunPosNDC.x / sunPosNDC.w, sunPosNDC.y / sunPosNDC.w, 1.0f);
    //_sunPosition = (_sunPosition + Vector3(1.0f)) * 0.5f;

    Vector4 sunPosView = camera->view() * Vector4(_sunPositionWorld, 1.0f);
    Vector4 sunPosNDC = camera->proj() * sunPosView;

    _sunPosition = Vector3(sunPosNDC.x / sunPosNDC.w, sunPosNDC.y / sunPosNDC.w, 1.0f);
    _sunPosition = (_sunPosition + Vector3(1.0f)) * 0.5f;

    upload();

    _dirty = false;

    return true;
}

Skybox::Skybox() :
    _vao(0), _vbo(0), _ebo(0),
    _altitude(0.0f), _azimuth(0.0f),
    _sunColor(1.0f),
    _sunIntensity(1.0f),
    _sunTightness(500.0f),
    _horizonColor(0.1f), _zenithColor(0.0f),
    _fogDensity(0.1f),
    _planetRadius(6371e3f), _atmosphereRadius(6471e3f),
    _Hr(7994.0f), _Hm(1200.0f), _miePhase(0.76f),
    _dirty(true),
    _ubo(0),
    _cubemapFBO(0), _cubemap(0),
    _irradianceFBO(0), _irradiance(0)
{
}

Skybox::~Skybox()
{
    if (_ubo)
        glDeleteBuffers(1, &_ubo);

    if (_irradiance)
        glDeleteTextures(1, &_irradiance);

    if (_irradianceFBO)
        glDeleteFramebuffers(1, &_irradianceFBO);

    if (_cubemap)
		glDeleteTextures(1, &_cubemap);

    if (_cubemapFBO)
        glDeleteFramebuffers(1, &_cubemapFBO);

    if (_vao)
		glDeleteVertexArrays(1, &_vao);

    if (_vbo)
        glDeleteBuffers(1, &_vbo);
}

void Skybox::upload() const
{
    /* Upload data to GPU */

    uint8_t buf[sizeof(SkyboxGPU)];
    SkyboxGPU *sb = (SkyboxGPU *)buf;

    sb->sunDirection = _sunDirection;
    sb->sunColor = _sunColor;
    sb->sunPosition = _sunPosition;
    sb->sunPositionWorld = _sunPositionWorld;
    sb->sunIntensity = _sunIntensity;
    sb->sunTightness = _sunTightness;
    sb->horizonColor = _horizonColor;
    sb->zenithColor = _zenithColor;
    sb->fogDensity = _fogDensity;
    sb->planetRadius = _planetRadius;
    sb->atmosphereRadius = _atmosphereRadius;
    sb->Hr = _Hr;
    sb->Hm = _Hm;
    sb->g = _miePhase;

    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(SkyboxGPU), buf);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
