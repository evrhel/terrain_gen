#version 410 core

@include "lib/gbuffer_base.glsl"
@include "lib/atmosphere.glsl"
@include "lib/camera.glsl"
@include "lib/material.glsl"
@include "lib/utils.glsl"

in TES_OUT
{
    float Height;
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    mat3 TBN;
} fs_in;

uniform MaterialSpec uMaterials[5];

#define DIRT_INDEX 0
#define GRASS_INDEX 1
#define SNOW_INDEX 2
#define ROCK_INDEX 3
#define SAND_INDEX 4

const float kRockStart = 0.3;
const float kRockEnd = 0.4;

const float kDirtStart = 50.0;
const float kDirtEnd = 100.0;

const float kSnowStart = 176.0;
const float kSnowEnd = 224.0;

const float kSandStart = 0.0;
const float kSandEnd = 16.0;

void computeWeights(vec3 fragpos, out float dirt, out float grass, out float snow, out float rock, out float sand)
{
    vec3 N = fs_in.Normal;
    float h = fragpos.y;
    float cosTheta = max(dot(N, vec3(0.0, 1.0, 0.0)), 0.0);

    if (h < kSandStart)
    {
        dirt = 0.0;
        grass = 0.0;
        snow = 0.0;
        rock = 0.0;
        sand = 1.0;
        return;
    }

    if (h < kSandEnd)
    {
        dirt = 0.0;
        snow = 0.0;
        rock = 0.0;
        sand = 1.0 - (h - kSandStart) / (kSandEnd - kSandStart);
        grass = 1.0 - sand;
        return;
    }
    
    if (h < kDirtStart)
    {
        dirt = 0.0;
        grass = 1.0;
        snow = 0.0;
        rock = 0.0;
        sand = 0.0;
        return;
    }

    if (h < kDirtEnd)
    {
        dirt = (h - kDirtStart) / (kDirtEnd - kDirtStart);
        grass = 1.0 - dirt;
        snow = 0.0;
        rock = 0.0;
        sand = 0.0;
        return;
    }

    snow = clamp((h - kSnowStart) / (kSnowEnd - kSnowStart), 0.0, 1.0);
    dirt = 1.0 - snow;
    grass = 0.0;
    rock = 0.0;
    sand = 0.0;
}

void sampleTerrain(vec3 fragpos, out vec3 albedo, out vec3 normal, out float roughness, out float metallic, out float ao)
{
    float dirt, grass, snow, rock, sand;

    computeWeights(fragpos, dirt, grass, snow, rock, sand);

    /* Albedo */
    albedo = vec3(0.0);
    albedo += dirt * sampleTexture(uMaterials[DIRT_INDEX].albedo, fs_in.TexCoords).rgb;
    albedo += grass * sampleTexture(uMaterials[GRASS_INDEX].albedo, fs_in.TexCoords).rgb;
    albedo += snow * sampleTexture(uMaterials[SNOW_INDEX].albedo, fs_in.TexCoords).rgb;
    albedo += rock * sampleTexture(uMaterials[ROCK_INDEX].albedo, fs_in.TexCoords).rgb;
    albedo += sand * sampleTexture(uMaterials[SAND_INDEX].albedo, fs_in.TexCoords).rgb;

    /* Normal */
    normal = vec3(0.0);
    normal += dirt * sampleTexture(uMaterials[DIRT_INDEX].normal, fs_in.TexCoords).rgb;
    normal += grass * sampleTexture(uMaterials[GRASS_INDEX].normal, fs_in.TexCoords).rgb;
    normal += snow * sampleTexture(uMaterials[SNOW_INDEX].normal, fs_in.TexCoords).rgb;
    normal += rock * sampleTexture(uMaterials[ROCK_INDEX].normal, fs_in.TexCoords).rgb;
    normal += sand * sampleTexture(uMaterials[SAND_INDEX].normal, fs_in.TexCoords).rgb;

    /* Roughness */
    roughness = 0.0;
    roughness += dirt * sampleTexture(uMaterials[DIRT_INDEX].roughness, fs_in.TexCoords).r;
    roughness += grass * sampleTexture(uMaterials[GRASS_INDEX].roughness, fs_in.TexCoords).r;
    roughness += snow * sampleTexture(uMaterials[SNOW_INDEX].roughness, fs_in.TexCoords).r;
    roughness += rock * sampleTexture(uMaterials[ROCK_INDEX].roughness, fs_in.TexCoords).r;
    roughness += sand * sampleTexture(uMaterials[SAND_INDEX].roughness, fs_in.TexCoords).r;

    /* Metallic */
    metallic = 0.0;
    metallic += dirt * sampleTexture(uMaterials[DIRT_INDEX].metallic, fs_in.TexCoords).r;
    metallic += grass * sampleTexture(uMaterials[GRASS_INDEX].metallic, fs_in.TexCoords).r;
    metallic += snow * sampleTexture(uMaterials[SNOW_INDEX].metallic, fs_in.TexCoords).r;
    metallic += rock * sampleTexture(uMaterials[ROCK_INDEX].metallic, fs_in.TexCoords).r;
    metallic += sand * sampleTexture(uMaterials[SAND_INDEX].metallic, fs_in.TexCoords).r;

    /* AO */
    ao = 0.0;
    ao += dirt * sampleTexture(uMaterials[DIRT_INDEX].ao, fs_in.TexCoords).r;
    ao += grass * sampleTexture(uMaterials[GRASS_INDEX].ao, fs_in.TexCoords).r;
    ao += snow * sampleTexture(uMaterials[SNOW_INDEX].ao, fs_in.TexCoords).r;
    ao += rock * sampleTexture(uMaterials[ROCK_INDEX].ao, fs_in.TexCoords).r;
    ao += sand * sampleTexture(uMaterials[SAND_INDEX].ao, fs_in.TexCoords).r;
}

void main()
{
    if (uWireframe)
    {
        Albedo = vec4(1.0, 0.0, 0.0, 1.0);
        Emissive = vec4(0.0, 0.0, 0.0, 1.0);
        PositionOut = vec4(0.0, 0.0, 0.0, 1.0);
        DepthOut = vec4(gl_FragCoord.zzz, 1.0);
        NormalOut = vec4(0.0, 0.0, 0.0, 1.0);
        MaterialOut = uvec4(0, 0, 0, 1);
        return;
    }

    vec3 fragpos = fs_in.FragPos;
    fragpos = nvec3(uCamera.invView * nvec4(fragpos));

    vec3 albedo;
    vec3 N;
    float roughness;
    float metallic;
    float ao;
    sampleTerrain(fragpos, albedo, N, roughness, metallic, ao);

    MaterialInfo material;
    material.roughness = roughness;
    material.metallic = metallic;
    material.ao = ao;
    material.lit = true;
    material.reflective = false;

    /* Compute normal */
    N = N * 2.0 - 1.0;
    N = normalize(fs_in.TBN * N);

    Albedo = vec4(albedo, 1.0);
    Emissive = vec4(0.0, 0.0, 0.0, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(N, 1.0);
    MaterialOut = encodeMaterial(material);
}
