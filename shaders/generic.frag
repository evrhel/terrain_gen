#version 410 core

@include "lib/types.glsl"
@include "lib/gbuffer.glsl"
@include "lib/atmosphere.glsl"

in VS_OUT
{
    vec3 FragPos;   // World-space position
    vec3 Normal;    // View-space normal
    vec2 TexCoords; // Texture coordinates
} fs_in;

uniform Material uMaterial;

void main()
{
    if (uWireframe)
    {
        Albedo = vec4(1.0, 1.0, 1.0, 1.0);
        Emissive = vec4(0.0, 0.0, 0.0, 1.0);
        PositionOut = vec4(fs_in.FragPos, 1.0);
        DepthOut = vec4(gl_FragCoord.zzz, 1.0);
        NormalOut = vec4(fs_in.Normal, 1.0);
        MaterialOut = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec4 albedo = sampleTexture(uMaterial.albedo, fs_in.TexCoords);
    if (albedo.a < 0.5)
        discard;

    vec3 emissive = sampleTexture(uMaterial.emissive, fs_in.TexCoords).rgb;
    vec3 normal = sampleTexture(uMaterial.normal, fs_in.TexCoords).xyz;
    
    float roughness = sampleTexture(uMaterial.roughness, fs_in.TexCoords).r;
    float metallic = sampleTexture(uMaterial.metallic, fs_in.TexCoords).r;
    float ao = sampleTexture(uMaterial.ao, fs_in.TexCoords).r;
    vec3 material = vec3(roughness, metallic, ao);

    Albedo = vec4(albedo.rgb, 1.0);
    Emissive = vec4(0.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(fs_in.Normal, 1.0);
    MaterialOut = vec4(material, 1.0);
}
