#version 410 core


// @include "lib\gbuffer_base.glsl"
#line 1
/* Common fragment shader for G-buffer rendering */

layout (location = 0) out vec4 Albedo;         // Fragment color
layout (location = 1) out vec4 Emissive;       // Emissive color
layout (location = 2) out vec4 PositionOut;    // Fragment view-space position
layout (location = 3) out vec4 DepthOut;       // Fragment depth (gl_FragCoord.z)
layout (location = 4) out vec4 NormalOut;      // Fragment view-space normal
layout (location = 5) out uvec4 MaterialOut;   // Fragment material, see material.glsl

uniform bool uWireframe; // Whether wireframe mode is enabled

#line 4

// @include "lib\atmosphere.glsl"
#line 1
layout (std140) uniform Atmosphere
{
    vec3 sunDirection;
    vec3 sunColor;
    vec3 sunPosition; // Sun position in screen space, [0, 1]
    float sunIntensity;

    float sunTightness;

    vec3 horizonColor;
    vec3 zenithColor;
    float atmosphereHeight;

    float fogDensity;
} uAtmosphere;

vec3 sampleAtmosphere(vec3 direction)
{
    float cosTheta = max(dot(direction, vec3(0.0, 1.0, 0.0)), 0.0);
    return mix(uAtmosphere.horizonColor, uAtmosphere.zenithColor, cosTheta);
}

vec3 sampleSun(vec3 direction)
{
    float intensity = max(dot(direction, -uAtmosphere.sunDirection), 0.0);
    intensity = pow(intensity, uAtmosphere.sunTightness);
    return uAtmosphere.sunColor * uAtmosphere.sunIntensity * intensity;
}

float calcAirDensity(float height)
{
    return max(height / 100, 6);
}

float calcFog(vec3 eye, vec3 fragPos)
{
    float height = calcAirDensity(fragPos.y);
    float d = distance(eye, fragPos);

    float eyeDensity = calcAirDensity(eye.y);

    return clamp(2.625 / exp(-6 / uAtmosphere.fogDensity) * exp(-eyeDensity / uAtmosphere.fogDensity) * (1 - exp(-pow(d, 2.712) * height / uAtmosphere.fogDensity / 6000.0 / 13.0)) / height, 0.0, 1.0);
}

#line 5

// @include "lib\camera.glsl"
#line 1
layout (std140) uniform Camera
{
    vec3 position;
    vec3 direction;

    float near;
    float far;
    float aspect;

    vec3 front;
    vec3 right;
    vec3 up;

    mat4 view;
    mat4 invView;

    mat4 proj;
    mat4 invProj;

    mat4 projView;
    mat4 invProjView;
} uCamera;

#line 6

// @include "lib\material.glsl"
#line 1
/* Material encoding and decoding functions. */


// @include "texture.glsl"
#line 1
/* Texture utility functions. */

struct Texture
{
    sampler2D tex;
    bool hasTex;
    vec3 color;
};

vec4 sampleTexture(Texture tex, vec2 texCoords)
{
    if (tex.hasTex)
    {
        return texture(tex.tex, texCoords);
    }

    return vec4(tex.color, 1.0);
}

#line 4

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

#line 7

// @include "lib\noise.glsl"
#line 1

/* Noise texture */
uniform sampler2D uNoiseTex;

float getNoise(vec2 coord)
{
    //vec2 pixel = coord * textureSize(uTexture0, 0);
    //coord = fract(pixel / textureSize(uNoiseTex, 0));
    return texture(uNoiseTex, coord).x;
}

#line 8

in VS_OUT
{
    vec3 FragPos;
} fs_in;

uniform float uTime;

float subSurfaceScattering(vec3 v, vec3 pos, float N)
{
    return pow(max(dot(v, normalize(pos)), 0.0), N) * (N + 1) / 6.28;
}

vec3 drawCloud(vec3 V, vec3 color)
{
    const float r = 3.2;
    const vec4 noiseC = vec4(1.0, r, r * r, r * r * r);
    const vec4 noiseWeights = 1.0 / noiseC / dot(1.0 / noiseC, vec4(1.0));

    const vec3 cloudColor = vec3(1,1,1);
    const vec3 cloudColor2 = vec3(0.9,0.9,0.9);

    vec3 tpos = vec3(uCamera.invView * vec4(V, 0.0));
    tpos = normalize(tpos);

    float cosT = max(dot(V, uCamera.up), 0.0);

    float wind = abs(uTime * 0.0005 - 0.5) + 0.5;
    float distortion = wind * 0.045;

    float iMult = -log(cosT) * 2.0 + 2.0;
    float heightA = (400.0 + 300.0 * sqrt(cosT)) / tpos.y;

    for (int i = 1; i < 22; i++)
    {
        vec3 intersection = tpos * (heightA - 4.0 * i * iMult);
        vec2 coord1 = intersection.xz / 200000.0 + wind * 0.05;
        vec2 coord = fract(coord1 / 0.25);

        vec4 noiseSample = vec4(
            getNoise(coord + distortion),
            getNoise(coord * noiseC.y + distortion),
            getNoise(coord * noiseC.z + distortion),
            getNoise(coord * noiseC.w + distortion));

        float j = i / 22.0;
        vec2 noiseTextureResolution = textureSize(uNoiseTex, 0);
        coord = vec2(j + 0.5, -j + 0.5) / noiseTextureResolution + coord.xy + sin(coord.xy * 3.14 * j) / 10.0 + wind * 0.02 * (j + 0.5);

        vec2 secondcoord = 1.0 - coord.yx;
        vec4 noiseSample2 = vec4(
            getNoise(secondcoord),
            getNoise(secondcoord * noiseC.y),
            getNoise(secondcoord * noiseC.z),
            getNoise(secondcoord * noiseC.w));
        
        float finalnoise = dot(noiseSample * noiseSample2, noiseWeights);
        float c1 = max((sqrt(finalnoise * max(1.0 - abs(i -11.0) / 11 * 0.15,0 )) - 0.55) / 0.65 * clamp(cosT * cosT * 2, 0, 1), 0);

        float cMult = max(pow(30.0 - i, 3.5) / pow(30, 3.5), 0.0) * 6.0;

        //float sunscattering = su

        color = color * (1.0 - c1) + c1 * cMult * mix(cloudColor2 * 4.75, cloudColor, min(cMult, 0.875)) * 0.05;
    }

    return color;
}

void main()
{
    vec3 V = normalize(vec3(uCamera.invView * vec4(normalize(fs_in.FragPos), 0.0)));
    
    /* Sky color */
    vec3 color = sampleAtmosphere(V);

    /* Draw sun */
    color += sampleSun(V);

    color = drawCloud(normalize(fs_in.FragPos), color);

    MaterialInfo material;
    material.roughness = 0.0;
    material.metallic = 0.0;
    material.ao = 0.0;
    material.lit = false;
    material.reflective = false;

    Albedo = vec4(0.0, 0.0, 0.0, 1.0);
    Emissive = vec4(color, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(0.0, 0.0, 0.0, 1.0);
    MaterialOut = encodeMaterial(material);
}

