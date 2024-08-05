#version 410 core

@include "lib/composite.glsl"
@include "lib/atmosphere.glsl"
@include "lib/camera.glsl"

uniform float uGamma;
uniform float uExposure;
uniform float uBloomStrength;
uniform int uTonemapFunction;

#define TONEMAP_FUNCTION_LINEAR 0
#define TONEMAP_FUNCTION_REINHARD 2
#define TONEMAP_FUNCTION_ACES 1
#define TONEMAP_FUNCTION_UNCHARTED2_RGB 3
#define TONEMAP_FUNCTION_UNCHARTED2_LUM 4

vec3 reinhard(vec3 x)
{
    x *= uExposure;
    return x / (x + 1.0);
}

vec3 aces(vec3 x)
{
    const float a = 2.51;
    const float b = 0.03;
    const float c = 2.43;
    const float d = 0.59;
    const float e = 0.14;

    x *= uExposure;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

const float A = 0.15; // Shoulder strength
const float B = 0.50; // Linear strength
const float C = 0.10; // Linear angle
const float D = 0.20; // Toe strength
const float E = 0.02; // Toe numerator
const float F = 0.30; // Toe denominator
const float W = 11.2; // White point

vec3 uncharted2(vec3 x)
{
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

float uncharted2(float x)
{
    return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 uncharted2_rgb(vec3 x)
{
    const float kExposureBias = 2.0;

    x *= uExposure;
    x = uncharted2(x * kExposureBias);

    float whiteScale = (1.0 / uncharted2(vec3(W))).r;
    return x * whiteScale;
}

vec3 uncharted2_lum(vec3 x)
{
    const float kExposureBias = 2.0;

    x *= uExposure;

    float lum = dot(x, vec3(0.2126, 0.7152, 0.0722));
    float newLum = uncharted2(kExposureBias * lum);
    x *= newLum / lum;

    float whiteScale = 1.0 / uncharted2(W);
    return x * whiteScale;
}

vec3 tonemap(vec3 x)
{
    if (uTonemapFunction == TONEMAP_FUNCTION_REINHARD)
        return reinhard(x);
    else if (uTonemapFunction == TONEMAP_FUNCTION_ACES)
        return aces(x);
    else if (uTonemapFunction == TONEMAP_FUNCTION_UNCHARTED2_RGB)
        return uncharted2_rgb(x);
    else if (uTonemapFunction == TONEMAP_FUNCTION_UNCHARTED2_LUM)
        return uncharted2_lum(x);
    
    return clamp(x * uExposure, 0.0, 1.0); // Linear
}

void main()
{
    /* Linear color */
    vec3 colorLinear = texture(uTexture0, fs_in.TexCoords).rgb;
    if (uWireframe)
    {
        Color0 = vec4(colorLinear, 1.0);
        Color1 = vec4(0.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }

    /* Bloom */
    vec3 bloom = texture(uTexture1, fs_in.TexCoords).rgb;
    colorLinear = mix(colorLinear, bloom, uBloomStrength);//bloom * uBloomStrength;

    /* Tonemapping */
    colorLinear = tonemap(colorLinear);
    //colorLinear = 1.0 - exp(-colorLinear * uExposure);
    //colorLinear = aces(colorLinear * uExposure);

    /* Gamma correction */
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / uGamma));

    Color0 = vec4(colorGammaCorrected, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
