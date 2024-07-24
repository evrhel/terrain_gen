#version 410 core

@include "lib/gbuffer_base.glsl"
@include "lib/atmosphere.glsl"
@include "lib/material.glsl"

in VS_OUT
{
    vec3 FragPos;   // World-space position
    vec3 Normal;    // View-space normal
    vec2 TexCoords; // Texture coordinates
} fs_in;

uniform MaterialSpec uMaterial;

void main()
{
    if (uWireframe)
    {
        Albedo = vec4(1.0, 1.0, 1.0, 1.0);
        Emissive = vec4(0.0, 0.0, 0.0, 1.0);
        PositionOut = vec4(fs_in.FragPos, 1.0);
        DepthOut = vec4(gl_FragCoord.zzz, 1.0);
        NormalOut = vec4(fs_in.Normal, 1.0);
        MaterialOut = uvec4(0, 0, 0, 1);
        return;
    }

    vec4 albedo = sampleTexture(uMaterial.albedo, fs_in.TexCoords);
    if (albedo.a < 0.5)
        discard;

    vec3 emissive = sampleTexture(uMaterial.emissive, fs_in.TexCoords).rgb;
    vec3 normal = sampleTexture(uMaterial.normal, fs_in.TexCoords).xyz;
    
    MaterialInfo material;
    material.roughness = sampleTexture(uMaterial.roughness, fs_in.TexCoords).r;
    material.metallic = sampleTexture(uMaterial.metallic, fs_in.TexCoords).r;
    material.ao = sampleTexture(uMaterial.ao, fs_in.TexCoords).r;
    material.lit = true;
    material.reflective = false;

    Albedo = vec4(albedo.rgb, 1.0);
    Emissive = vec4(0.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(fs_in.Normal, 1.0);
    MaterialOut = encodeMaterial(material);
}
