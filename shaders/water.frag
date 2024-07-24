#version 410 core

@include "lib/gbuffer_base.glsl"
@include "lib/atmosphere.glsl"
@include "lib/camera.glsl"
@include "lib/material.glsl"

in TES_OUT
{
    float Height;
    vec3 FragPos;
    vec2 TexCoords;
    vec3 Normal;
    mat3 TBN;
} fs_in;

uniform float uScale;
uniform MaterialSpec uMaterial;

const vec3 kAlbedo = vec3(0.0, 0.0, 0.0);

const float kNormalStrength = 0.15;

const float kThresholdMap[64] = float[](
	0, 32, 8, 40, 2, 34, 10, 42,
	48, 16, 56, 24, 50, 18, 58, 26,
	12, 44, 4, 36, 14, 46, 6, 38,
	60, 28, 52, 20, 62, 30, 54, 22,
	3, 35, 11, 43, 1, 33, 9, 41,
	51, 19, 59, 27, 49, 17, 57, 25,
	15, 47, 7, 39, 13, 45, 5, 37,
	63, 31, 55, 23, 61, 29, 53, 21
);

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

    /* Compute normal */
    vec3 N = sampleTexture(uMaterial.normal, fs_in.TexCoords).rgb;
    N = N * 2.0 - 1.0;
    N = normalize(fs_in.TBN * N);
    N = mix(fs_in.Normal, N, kNormalStrength);

    if (!gl_FrontFacing)
        N = -N;

    /* Transparency */
    vec3 V = normalize(fs_in.FragPos);
    float NdotV = dot(N, V);
    float fresnel = pow(clamp(1.0 + NdotV, 0.0, 1.0), 4.0);
    fresnel = 1.0;// mix(0.09, 1.0, fresnel); // F0

    /* Dithering */
    ivec2 pos = ivec2(gl_FragCoord.xy);
    pos.x = pos.x % 8;
    pos.y = pos.y % 8;

    float threshold = kThresholdMap[pos.y * 8 + pos.x] / 64.0;
    if (fresnel < threshold)
        discard;

    MaterialInfo material;
    material.roughness = 0.0;
    material.metallic = 0.0;
    material.ao = 0.0;
    material.lit = false;
    material.reflective = true;

    Albedo = vec4(0.0, 0.0, 0.0, 1.0);
    Emissive = vec4(0.0, 0.0, 0.0, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(N, 1.0);
    MaterialOut = encodeMaterial(material);
}
