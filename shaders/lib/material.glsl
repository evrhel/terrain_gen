/* Material encoding and decoding functions. */

@include "texture.glsl"

struct MaterialSpec
{
    Texture albedo;
    Texture emissive;
    Texture normal;
    Texture roughness;
    Texture metallic;
    Texture ao;
};

struct MaterialInfo
{
    float roughness;
    float metallic;
    float ao;

    bool lit;
    bool reflective;
};

const uint kByteMask = 255;

const uint kMaterialLit = 16777216;
const uint kMaterialReflective = 33554432;

void decodeMaterial(uvec4 p, out MaterialInfo material)
{   
    uint packedR = p.r;
    material.roughness = float(packedR & kByteMask) / 255.0;
    material.metallic = float((packedR >> 8) & kByteMask) / 255.0;
    material.ao = float((packedR >> 16) & kByteMask) / 255.0;
    material.lit = (packedR & kMaterialLit) != 0;
    material.reflective = (packedR & kMaterialReflective) != 0;
}

uvec4 encodeMaterial(in MaterialInfo material)
{    
    uint packedR = 0;
    packedR |= uint(material.roughness * 255.0);
    packedR |= uint(material.metallic * 255.0) << 8;
    packedR |= uint(material.ao * 255.0) << 16;
    packedR |= material.lit ? kMaterialLit : 0;
    packedR |= material.reflective ? kMaterialReflective : 0;

    return uvec4(packedR, 0, 0, 1);
}
