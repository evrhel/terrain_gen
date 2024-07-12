#pragma once

#include <glad/glad.h>
#include <mutil/mutil.h>

using namespace mutil;

class Skybox final
{
public:
    void render() const;

    void load();

    void update();

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

    constexpr float atmosphereHeight() const { return _atmosphereHeight; }

    constexpr void setAtmosphereHeight(float height)
    {
        _atmosphereHeight = height;
        _dirty = true;
    }

    constexpr const Vector3 &sunDirection() const { return _sunDirection; }

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
    float _atmosphereHeight;

    bool _dirty;
    GLuint _ubo;

    Vector3 _sunDirection;

    void upload() const;
};
