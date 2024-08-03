#version 410 core

@include "lib/composite.glsl"
@include "lib/atmosphere.glsl"
@include "lib/utils.glsl"
@include "lib/camera.glsl"

const float kFXAAReduceMin = 1.0 / 64.0;
const float kFXAAReduceMul = 1.0 / 32.0;
const float kFXAASpanMax = 8.0;

uniform bool uEnableFXAA;

vec3 fxaa(vec2 uv)
{
    const vec3 kLuma = vec3(0.2126, 0.7152, 0.0722);

    vec2 texelSize = 1.0 / textureSize(uTexture0, 0);

    vec3 rgbNW = texture(uTexture0, uv + vec2(-1, -1) * texelSize).rgb;
    vec3 rgbNE = texture(uTexture0, uv + vec2(1, -1) * texelSize).rgb;
    vec3 rgbSW = texture(uTexture0, uv + vec2(-1, 1) * texelSize).rgb;
    vec3 rgbSE = texture(uTexture0, uv + vec2(1, 1) * texelSize).rgb;
    vec3 rgbM = texture(uTexture0, uv).rgb;

    float lumaNW = dot(rgbNW, kLuma);
    float lumaNE = dot(rgbNE, kLuma);
    float lumaSW = dot(rgbSW, kLuma);
    float lumaSE = dot(rgbSE, kLuma);
    float lumaM = dot(rgbM, kLuma);
    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSE, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y = (lumaNW + lumaSW) - (lumaNE + lumaSE);

    float dirReduce = max((lumaNW + lumaNE + lumaSE + lumaSE) * (0.25 * kFXAAReduceMul), kFXAAReduceMin);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = min(vec2(kFXAASpanMax), max(vec2(-kFXAASpanMax), dir * rcpDirMin)) * texelSize;

    vec3 rgbA = 0.5 * (texture(uTexture0, uv + dir * (1.0 / 3.0 - 0.5)).rgb + texture(uTexture0, uv + dir * (2.0 / 3.0 - 0.5)).rgb);
    vec3 rgbB = rgbA * 0.5 + 0.25 * (texture(uTexture0, uv + dir * -0.5).rgb + texture(uTexture0, uv + dir * 0.5).rgb);
    float lumaB = dot(rgbB, kLuma);

    if (lumaB < lumaMin || lumaB > lumaMax)
        return rgbA;
    
    return rgbB;
}

void main()
{
    /* FXAA */
    vec3 color = uEnableFXAA ? fxaa(fs_in.TexCoords) : texture(uTexture0, fs_in.TexCoords).rgb;

    Color0 = vec4(color, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
