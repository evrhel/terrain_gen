#version 410 core

@include "lib/types.glsl"
@include "lib/gbuffer.glsl"
@include "lib/atmosphere.glsl"

in TES_OUT
{
    float Height;
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
} fs_in;

uniform float uScale;
uniform Material uMaterial;

const vec3 kDirtAlbedo = vec3(0.436, 0.301, 0.252);
const float kDirtRoughness = 0.9;
const float kDirtMetallic = 0.0;

const vec3 kGrassAlbedo = vec3(0.267, 0.755, 0.163);
const float kGrassRoughness = 1.0;
const float kGrassMetallic = 0.0;

const vec3 kSnowAlbedo = vec3(0.89);
const float kSnowRoughness = 0.25;
const float kSnowMetallic = 0.0;

const vec3 kRockAlbedo = vec3(0.29);
const float kRockRoughness = 0.85;
const float kRockMetallic = 0.0;

const vec3 kWaterAlbedo = vec3(1.0, 1.0, 1.0);
const float kWaterRoughness = 0.1;
const float kWaterMetallic = 1.0;

const vec3 kSandAlbedo = vec3(1.0, 0.841, 0.510);
const float kSandRoughness = 0.6;
const float kSandMetallic = 0.0;

const float kRockStart = 0.45;
const float kRockEnd = 0.5;

const float kDirtStart = 40.0;
const float kDirtEnd = 45.0;

const float kSnowStart = 65.0;
const float kSnowEnd = 70.0;

const float kSandStart = 0.0;
const float kSandEnd = 3.0;

void computeBiome(out vec3 albedo, out float roughness, out float metallic)
{
    vec3 N = fs_in.Normal;
    float h = fs_in.FragPos.y;
    float cosTheta = 1.0 - max(dot(N, vec3(0.0, 1.0, 0.0)), 0.0);
    float factor;

    factor = clamp((h - kSandStart) / (kSandEnd - kSandStart), 0.0, 1.0);
    albedo = mix(kSandAlbedo, kGrassAlbedo, factor);
    roughness = mix(kSandRoughness, kGrassRoughness, factor);
    metallic = mix(kSandMetallic, kGrassMetallic, factor);

    factor = clamp((h - kDirtStart) / (kDirtEnd - kDirtStart), 0.0, 1.0);
    albedo = mix(albedo, kDirtAlbedo, factor);
    roughness = mix(roughness, kDirtRoughness, factor);
    metallic = mix(metallic, kDirtMetallic, factor);

    factor = clamp((h - kSnowStart) / (kSnowEnd - kSnowStart), 0.0, 1.0);
    albedo = mix(albedo, kSnowAlbedo, factor);
    roughness = mix(roughness, kSnowRoughness, factor);
    metallic = mix(metallic, kSnowMetallic, factor);

    factor = clamp((cosTheta - kRockStart) / (kRockEnd - kRockStart), 0.0, 1.0);
    albedo = mix(albedo, kRockAlbedo, factor);
    roughness = mix(roughness, kRockRoughness, factor);
    metallic = mix(metallic, kRockMetallic, factor);
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
        MaterialOut = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 albedo;
    float roughness;
    float metallic;
    computeBiome(albedo, roughness, metallic);

    vec3 material = vec3(roughness, metallic, 1.0);

    Albedo = vec4(albedo, 1.0);
    Emissive = vec4(0.0, 0.0, 0.0, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(fs_in.Normal, 1.0);
    MaterialOut = vec4(material, 1.0);
    
    /*vec4 albedo = sampleTexture(uMaterial.albedo, fs_in.TexCoords);
    if (albedo.a < 0.5)
        discard;

    vec3 emissive = sampleTexture(uMaterial.emissive, fs_in.TexCoords).rgb;
    vec3 normal = sampleTexture(uMaterial.normal, fs_in.TexCoords).xyz;

    float roughness = sampleTexture(uMaterial.roughness, fs_in.TexCoords).r;
    float metallic = sampleTexture(uMaterial.metallic, fs_in.TexCoords).r;
    float ao = sampleTexture(uMaterial.ao, fs_in.TexCoords).r;
    vec3 material = vec3(roughness, metallic, ao);

    float h = (fs_in.Height + 16) / uScale;

    Albedo = vec4(albedo.rgb, 1.0);
    Emissive = vec4(emissive, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(fs_in.Normal, 1.0);
    MaterialOut = vec4(material, 1.0);*/
}
