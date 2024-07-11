#pragma once

#include <mutil/mutil.h>

#include "texture.h"

using namespace mutil;

struct Material
{
    Texture2D albedo;
    Vector3 albedoColor;

    Texture2D emissive;
    Vector3 emissiveColor;

    Texture2D normal;

    Texture2D roughness;
    float roughnessValue;

    Texture2D metallic;
    float metallicValue;

    Texture2D ao;
    float aoValue;

    Material &operator=(const Material &) = delete;
    Material &operator=(Material &&other) noexcept;

    Material();
    Material(const Material &) = delete;
    Material(Material &&other) noexcept;
};
