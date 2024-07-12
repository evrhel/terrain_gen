#include "camera.h"

#include "util.h"

struct CameraGPU
{
    Vector3 position;
    float _pad0;

    Vector3 direction;
    float _pad1;

    float near;
    float far;
    float _pad2[2];

    Vector3 front;
    float _pad3;

    Vector3 right;
    float _pad4;

    Vector3 up;
    float _pad5;

    Matrix4 view;
    Matrix4 invView;

    Matrix4 projection;
    Matrix4 invProjection;

    Matrix4 viewProjection;
    Matrix4 invViewProjection;
};

void Camera::load()
{
    glGenBuffers(1, &_ubo);

    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(CameraGPU), NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, CAMERA_UNIFORM_BINDING, _ubo);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Camera::update()
{
    if (!_dirty)
        return;

    Quaternion q1 = rotateaxis(kWorldUp, mutil::radians(_yaw));
    Quaternion q2 = rotateaxis(kWorldRight, mutil::radians(_pitch));
    Quaternion q = q1 * q2;

    _front = rotatevector(q, kWorldFront);
    _right = cross(_front, kWorldUp);
    _up = cross(_right, _front);

    _view = lookAt(_position, _position + _front, _up);
    _invView = inverse(_view);

    _proj = perspective(mutil::radians(_fov), _aspect, _near, _far);
    _invProj = inverse(_proj);

    _viewProj = _proj * _view;
    _invViewProj = _invView * _invProj;

    upload();

    _dirty = false;
}

Camera::Camera() : _pitch(0.0f), _yaw(0.0f),
                   _fov(45.0f), _aspect(16.0f / 9.0f),
                   _near(0.1f), _far(100.0f),
                   _dirty(true), _ubo(0)
{
}

Camera::~Camera()
{
    if (_ubo)
        glDeleteBuffers(1, &_ubo);
}

void Camera::upload() const
{
    /* Upload camera data to GPU */

    uint8_t buf[sizeof(CameraGPU)];
    CameraGPU *cam = (CameraGPU *)buf;

    cam->position = _position;
    cam->direction = _front;
    cam->near = _near;
    cam->far = _far;

    cam->front = _front;
    cam->right = _right;
    cam->up = _up;

    cam->view = _view;
    cam->invView = _invView;

    cam->projection = _proj;
    cam->invProjection = _invProj;

    cam->viewProjection = _viewProj;
    cam->invViewProjection = _invViewProj;

    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), buf);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
