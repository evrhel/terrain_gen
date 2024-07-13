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

void main()
{
    if (uWireframe)
    {
        Albedo = vec4(0.0, 0.0, 0.0, 1.0);
        Emissive = vec4(1.0, 0.0, 0.0, 1.0);
        PositionOut = vec4(0.0, 0.0, 0.0, 1.0);
        DepthOut = vec4(gl_FragCoord.zzz, 1.0);
        NormalOut = vec4(0.0, 0.0, 0.0, 1.0);
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

    float h = (fs_in.Height + 16) / uScale;

    Albedo = vec4(albedo.rgb, 1.0);
    Emissive = vec4(emissive, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(fs_in.Normal, 1.0);
    MaterialOut = vec4(material, 1.0);
}
