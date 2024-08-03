#pragma once

#include <glad/glad.h>
#include <mutil/mutil.h>

using namespace mutil;

class Shader;
class Camera;

#define SKYBOX_TEXTURE_UNIT 25
#define IRRADIANCE_TEXTURE_UNIT 26

class Skybox final
{
public:
    void renderSkybox() const;
    void renderIrradiance() const;
    void render(Shader *shader) const;

    void load();

    bool update(const Camera *camera);

    constexpr float sunAltitude() const { return _altitude; }

    constexpr void setSunAltitude(float altitude)
    {
        _altitude = altitude;
        _dirty = true;
    }

    constexpr float sunAzimuth() const { return _azimuth; }

    constexpr void setSunAzimuth(float azimuth)
    {
        _azimuth = azimuth;
        _dirty = true;
    }

    constexpr const Vector3 &sunColor() const { return _sunColor; }

    constexpr void setSunColor(const Vector3 &color)
    {
        _sunColor = color;
        _dirty = true;
    }

    constexpr float sunIntensity() const { return _sunIntensity; }

    constexpr void setSunIntensity(float intensity)
    {
        _sunIntensity = intensity;
        _dirty = true;
    }

    constexpr float sunTightness() const { return _sunTightness; }

    constexpr void setSunTightness(float tightness)
    {
        _sunTightness = tightness;
        _dirty = true;
    }

    constexpr const Vector3 &horizonColor() const { return _horizonColor; }

    constexpr void setHorizonColor(const Vector3 &color)
    {
        _horizonColor = color;
        _dirty = true;
    }

    constexpr const Vector3 &zenithColor() const { return _zenithColor; }

    constexpr void setZenithColor(const Vector3 &color)
    {
        _zenithColor = color;
        _dirty = true;
    }

    constexpr float fogDensity() const { return _fogDensity; }

    constexpr void setFogDensity(float density)
    {
        _fogDensity = density;
        _dirty = true;
    }

    constexpr float planetRadius() const { return _planetRadius; }

    constexpr void setPlanetRadius(float radius)
    {
		_planetRadius = radius;
		_dirty = true;
	}

    constexpr float atmosphereRadius() const { return _atmosphereRadius; }

    constexpr void setAtmosphereRadius(float radius)
    {
        _atmosphereRadius = radius;
        _dirty = true;
    }

    constexpr const Vector3 &sunDirection() const { return _sunDirection; }
    constexpr const Vector3 &sunPosition() const { return _sunPosition; }

    constexpr GLuint skybox() const { return _cubemap; }
    constexpr GLuint irradiance() const { return _irradiance; }

    Skybox();
    ~Skybox();

private:
    GLuint _vao, _vbo, _ebo;

    /* Sun */
    float _altitude, _azimuth;
    Vector3 _sunColor;
    float _sunIntensity;

    /* Atmosphere */
    float _sunTightness;
    Vector3 _horizonColor;
    Vector3 _zenithColor;

    /* Fog */
    float _fogDensity;

    /* Atmosphere */
    float _planetRadius;
    float _atmosphereRadius;

    bool _dirty;
    GLuint _ubo;

    Vector3 _sunDirection;

    Vector3 _sunPosition;
    Vector3 _sunPositionWorld;

    GLuint _skyQuadVAO, _skyQuadVBO;

    GLuint _cubemapFBO;
    GLuint _cubemap; // Skybox cubemap

    GLuint _irradianceFBO;
    GLuint _irradiance; // Irradiance cubemap

    void upload() const;
};
