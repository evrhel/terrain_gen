#include "camera.h"

void Camera::update()
{
    if (!_dirty)
        return;

    Quaternion q1 = rotateaxis(kWorldUp, _yaw);
    Quaternion q2 = rotateaxis(kWorldRight, _pitch);
    Quaternion q = q1 * q2;

    _front = rotatevector(q, kWorldForward);
    _right = cross(_front, kWorldUp);
    _up = cross(_right, _front);

    _view = lookAt(_position, _position + _front, _up);
    _invView = inverse(_view);

    _proj = perspective(_fov, _aspect, _near, _far);
    _invProj = inverse(_proj);

    _viewProj = _proj * _view;
    _invViewProj = _invView * _invProj;

    _dirty = false;
}

Camera::Camera() : _pitch(0.0f), _yaw(0.0f), _dirty(true)
{
}

Camera::~Camera()
{
}
