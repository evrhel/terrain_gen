#pragma once

#include <mutil/mutil.h>

#include "texture.h"

using namespace mutil;

class Material final : public Object
{
public:
    AutoRelease<Texture2D> albedo;
    Vector3 albedoColor;

    AutoRelease<Texture2D> emissive;
    Vector3 emissiveColor;

    AutoRelease<Texture2D> normal;

    AutoRelease<Texture2D> roughness;
    float roughnessValue;

    AutoRelease<Texture2D> metallic;
    float metallicValue;

    AutoRelease<Texture2D> ao;
    float aoValue;

    void load(const char *name);

    Material();
    virtual ~Material();
};

template <size_t N>
class MaterialArray final
{
public:
    constexpr size_t size() const { return N; }
    constexpr AutoRelease<Material> &operator[](size_t i) { return _materials[i]; }
    constexpr const AutoRelease<Material> &operator[](size_t i) const { return _materials[i]; }

    constexpr AutoRelease<Material> *begin() { return _materials; }
    constexpr AutoRelease<Material> *end() { return _materials + N; }

    constexpr MaterialArray() {}
    inline ~MaterialArray() {}
private:
    AutoRelease<Material> _materials[N];
};

AutoRelease<Material> &loadMaterial(const char *name);
void unloadMaterials();
