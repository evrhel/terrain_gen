#version 430 core

@include "lib/gbuffer.glsl"

in VS_OUT
{
    vec3 FragPos;   // World-space position
    vec3 Normal;    // View-space normal
    vec2 TexCoords; // Texture coordinates
} fs_in;

uniform Texture uAlbedo;
uniform Texture uEmissive;
uniform Texture uNormal;
uniform Texture uMaterial;

void main()
{
    vec4 albedo = sampleTexture(uAlbedo, fs_in.TexCoords);
    if (albedo.a < 0.5)
        discard;

    vec3 emissive = sampleTexture(uEmissive, fs_in.TexCoords).rgb;
    vec3 normal = sampleTexture(uNormal, fs_in.TexCoords).xyz;
    vec3 material = sampleTexture(uMaterial, fs_in.TexCoords).xyz;

    Albedo = vec4(albedo.rgb, 1.0);
    Emissive = vec4(0.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    NormalOut = vec4(fs_in.Normal, 1.0);
    MaterialOut = vec4(material, 1.0);
}
