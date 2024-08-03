#include "material.h"

#include <utility>
#include <cstdio>
#include <unordered_map>
#include <string>

static std::unordered_map<std::string, AutoRelease<Material>> _materials;

void Material::load(const char *name)
{
    printf("Material::load: %s\n", name);

    char path[256];

    snprintf(path, sizeof(path), "%s/albedo.png", name);
    albedo = loadTexture2D(path, COLOR_SPACE_SRGB);

    snprintf(path, sizeof(path), "%s/normal.png", name);
    normal = loadTexture2D(path, COLOR_SPACE_LINEAR);

    snprintf(path, sizeof(path), "%s/roughness.png", name);
    roughness = loadTexture2D(path, COLOR_SPACE_LINEAR);

    snprintf(path, sizeof(path), "%s/metallic.png", name);
    metallic = loadTexture2D(path, COLOR_SPACE_LINEAR);

    snprintf(path, sizeof(path), "%s/ao.png", name);
    ao = loadTexture2D(path, COLOR_SPACE_LINEAR);
}

Material::Material() : albedoColor(1.0f),
                       emissiveColor(0.0f),
                       roughnessValue(0.5f),
                       metallicValue(0.0f),
                       aoValue(1.0f)
{
}

Material::~Material() {}

AutoRelease<Material> &loadMaterial(const char *name)
{
    auto it = _materials.find(name);
    if (it != _materials.end())
		return it->second;

    AutoRelease<Material> &material = _materials[name];
    material = new Material();

    material->load(name);

    return material;
}

void unloadMaterials()
{
	_materials.clear();
}
