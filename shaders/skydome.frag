#version 410 core

@include "lib/atmosphere.glsl"
@include "lib/camera.glsl"

#define PI 3.141592
#define iSteps 16
#define jSteps 9

layout (location = 0) out vec4 Color0;

in GS_OUT
{
    vec3 Position;
} fs_in;

uniform vec2 uResolution;

// Ray-sphere intersection
//
// r0: ray origin
// rd: ray direction
// sr: sphere radius
vec2 rsi(vec3 r0, vec3 rd, float sr)
{
    float a = dot(rd, rd);
    float b = 2.0 * dot(rd, r0);
    float c = dot(r0, r0) - (sr * sr);
    float d = (b * b) - 4.0 * a * c;

    if (d < 0.0)
        return vec2(1e5, -1e5);

    float sqrtd = sqrt(d);
    return vec2((-b - sqrtd) / (2.0 * a), (-b + sqrtd) / (2.0 * a));
}

vec2 earthShadow(float y0, vec3 sunDir)
{
    return vec2(0.0);
}

vec3 calcAtmosphere(vec3 V, vec3 eye)
{
    //const vec3 kRlh = vec3(5.8e-6, 13.5e-6, 33.1e-6); // For lambda = (680, 550, 440 nm)
    //const vec3 kMie = //vec3(0.00002);
    
    const vec3 kRlh = vec3(5.8e-6, 13.0e-6, 22.4e-6);
    const float kMie = 21e-6;
    const float shRlh = 8e3;
    const float shMie = 1.2e3;
    const float g = 0.76;
    const float gg = g * g;

    vec3 pSun = -normalize(uAtmosphere.sunDirection);
    vec3 r = normalize(V);

    vec3 r0 = eye;

    vec2 p = rsi(r0, r, uAtmosphere.atmosphereRadius);

    if (p.x > p.y || p.y < 0)
        return vec3(0.0);

    p.y = min(p.y, rsi(r0, r, uAtmosphere.planetRadius).x);

    float iStepSize = (p.y - p.x) / float(iSteps);

    float iTime = 0.0;

    vec3 totalRlh = vec3(0.0);
    vec3 totalMie = vec3(0.0);

    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    float mu = dot(r, pSun);
    float mumu = mu * mu;
    float pRlh = 3.0 / (16.0 * PI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * PI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    for (int i = 0; i < iSteps; i++)
    {
        vec3 iPos = r0 + r * (iTime + iStepSize * 0.5);

        float iHeight = length(iPos) - uAtmosphere.planetRadius;

        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;

        iOdRlh += odStepRlh;
        iOdMie += odStepMie;

        float jStepSize = rsi(iPos, pSun, uAtmosphere.atmosphereRadius).y / float(jSteps);

        float jTime = 0.0;

        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        for (int j = 0; j < jSteps; j++)
        {
            vec3 jPos = iPos + pSun * (jTime + jStepSize * 0.5);

            float jHeight = length(jPos) - uAtmosphere.planetRadius;

            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;

            jTime += jStepSize;
        }

        vec3 attn = exp(-(kMie * (iOdMie + iOdMie) + kRlh * (iOdRlh + jOdRlh)));

        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;

        iTime + iStepSize;
    }

    return uAtmosphere.sunColor * uAtmosphere.sunIntensity * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}

void main()
{
    vec3 eye = vec3(0.0, uCamera.position.y + uAtmosphere.planetRadius, 0.0);
    vec3 V = normalize(fs_in.Position);

    vec3 color = calcAtmosphere(V, eye);
    if (isnan(color.x) || isnan(color.y) || isnan(color.z))
        color = vec3(1.0, 0.0, 0.0);

    Color0 = vec4(color, 1.0);
}
