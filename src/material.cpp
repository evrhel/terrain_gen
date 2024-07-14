#include "material.h"

#include <utility>
#include <cstdio>

void Material::load(const char *name)
{
    printf("Material::load: %s\n", name);

    char path[256];

    snprintf(path, sizeof(path), "%s/albedo.png", name);
    albedo.load(path);

    snprintf(path, sizeof(path), "%s/normal.png", name);
    normal.load(path);

    snprintf(path, sizeof(path), "%s/roughness.png", name);
    roughness.load(path);

    snprintf(path, sizeof(path), "%s/metallic.png", name);
    metallic.load(path);

    snprintf(path, sizeof(path), "%s/ao.png", name);
    ao.load(path);
}

Material &Material::operator=(Material &&other) noexcept
{
    if (this == &other)
        return *this;

    albedo = std::move(other.albedo);
    emissive = std::move(other.emissive);
    normal = std::move(other.normal);
    roughness = std::move(other.roughness);
    metallic = std::move(other.metallic);
    ao = std::move(other.ao);

    albedoColor = other.albedoColor;
    emissiveColor = other.emissiveColor;
    roughnessValue = other.roughnessValue;
    metallicValue = other.metallicValue;
    aoValue = other.aoValue;

    return *this;
}

Material::Material() : albedoColor(1.0f),
                       emissiveColor(0.0f),
                       roughnessValue(0.5f),
                       metallicValue(0.0f),
                       aoValue(1.0f)
{
}

Material::Material(Material &&other) noexcept : albedo(std::move(other.albedo)),
                                                emissive(std::move(other.emissive)),
                                                normal(std::move(other.normal)),
                                                roughness(std::move(other.roughness)),
                                                metallic(std::move(other.metallic)),
                                                ao(std::move(other.ao)),
                                                albedoColor(other.albedoColor),
                                                emissiveColor(other.emissiveColor),
                                                roughnessValue(other.roughnessValue),
                                                metallicValue(other.metallicValue),
                                                aoValue(other.aoValue)
{
}
