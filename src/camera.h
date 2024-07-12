#pragma once

#include <glad/glad.h>
#include <mutil/mutil.h>

using namespace mutil;

class Camera
{
public:
    constexpr const Vector3 &position() const { return _position; }

    constexpr void setPosition(const Vector3 &position)
    {
        _position = position;
        _dirty = true;
    }

    constexpr float pitch() const { return _pitch; }

    constexpr void setPitch(float pitch)
    {
        _pitch = pitch;
        _dirty = true;
    }

    constexpr float yaw() const { return _yaw; }

    constexpr void setYaw(float yaw)
    {
        _yaw = yaw;
        _dirty = true;
    }

    constexpr float fov() const { return _fov; }

    constexpr void setFov(float fov)
    {
        _fov = fov;
        _dirty = true;
    }

    constexpr float aspect() const { return _aspect; }

    constexpr void setAspect(float aspect)
    {
        _aspect = aspect;
        _dirty = true;
    }

    constexpr float near() const { return _near; }

    constexpr void setNear(float near)
    {
        _near = near;
        _dirty = true;
    }

    constexpr float far() const { return _far; }

    constexpr void setFar(float far)
    {
        _far = far;
        _dirty = true;
    }

    constexpr const Vector3 &front() const { return _front; }
    constexpr const Vector3 &right() const { return _right; }
    constexpr const Vector3 &up() const { return _up; }

    constexpr const Matrix4 &view() const { return _view; }
    constexpr const Matrix4 &invView() const { return _invView; }
    constexpr const Matrix4 &proj() const { return _proj; }
    constexpr const Matrix4 &invProj() const { return _invProj; }
    constexpr const Matrix4 &viewProj() const { return _viewProj; }
    constexpr const Matrix4 &invViewProj() const { return _invViewProj; }

    void load();

    void update();

    Camera();
    ~Camera();

private:
    Vector3 _position;
    float _pitch, _yaw;

    float _fov;
    float _aspect;
    float _near, _far;

    bool _dirty;
    GLuint _ubo;

    Vector3 _front, _right, _up;

    Matrix4 _view, _invView;
    Matrix4 _proj, _invProj;
    Matrix4 _viewProj, _invViewProj;

    void upload() const;
};
