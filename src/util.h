#pragma once

#include <mutil/mutil.h>

using namespace mutil;

#define CAMERA_UNIFORM_BINDING 0
#define ATMOSPHERE_UNIFORM_BINDING 1

constexpr Vector3 kWorldUp = {0.0f, 1.0f, 0.0f};
constexpr Vector3 kWorldDown = {0.0f, -1.0f, 0.0f};
constexpr Vector3 kWorldRight = {1.0f, 0.0f, 0.0f};
constexpr Vector3 kWorldLeft = {-1.0f, 0.0f, 0.0f};
constexpr Vector3 kWorldFront = {0.0f, 0.0f, 1.0f};
constexpr Vector3 kWorldBack = {0.0f, 0.0f, -1.0f};

constexpr Vector3 kDefaultNormal = {0.5f, 1.0f, 0.5f};

#define colorRGB(r, g, b) (Vector3((r) / 255.0f, (g) / 255.0f, (b) / 255.0f))

void checkGLErrors(const char *where);
