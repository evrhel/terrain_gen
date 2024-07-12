#include "camera.h"

#include "util.h"

struct CameraGPU
{
    Vector3 position;
    float _pad0;

    Vector3 direction;

    float near;
    float far;
    float _pad2[3];

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

    Matrix4 projView;
    Matrix4 invProjView;
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

    Quaternion q1 = mutil::rotateaxis(kWorldRight, mutil::radians(_pitch));
    Quaternion q2 = mutil::rotateaxis(kWorldUp, mutil::radians(_yaw));

    Quaternion q = q2 * q1;

    _front = rotatevector(q, kWorldBack);
    _right = cross(_front, kWorldUp);
    _up = cross(_right, _front);

    _view = lookAt(_position, _position + _front, _up);
    _invView = inverse(_view);

    _proj = perspective(mutil::radians(_fov), _aspect, _near, _far);
    _invProj = inverse(_proj);

    _projView = _proj * _view;
    _invProjView = inverse(_projView);

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

    cam->projView = _projView;
    cam->invProjView = _invProjView;

    glBindBuffer(GL_UNIFORM_BUFFER, _ubo);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(CameraGPU), buf);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
