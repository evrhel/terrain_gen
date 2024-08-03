#version 410 core

@include "lib/atmosphere.glsl"
@include "lib/camera.glsl"

layout (location = 0) out vec4 Color0;

in GS_OUT
{
    vec3 Position;
} fs_in;

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

// R: ray direction (world space)
// eye: ray origin (world space)
vec3 calcAtmosphere(vec3 R, vec3 eye)
{
    const int iSteps = 16;
    const int jSteps = 8;
    const float kPI = 3.14159265359;
    const vec3 kRlh = vec3(5.5e-6, 13.0e-6, 22.4e-6);
    const float kMie = 21e-6;
    const float shRlh = 8e3;
    const float shMie = 1.2e3;
    const float g = 0.758;

    vec3 sunDir = -normalize(uAtmosphere.sunDirection);

    R = normalize(R);

    vec2 p = rsi(eye, R, uAtmosphere.atmosphereRadius);
    if (p.x > p.y)
        return vec3(0, 0, 0);
    p.y = min(p.y, rsi(eye, R, uAtmosphere.planetRadius).x);
    float iStepSize = (p.y - p.x) / float(iSteps);

    float iTime = 0.0;

    vec3 totalRlh = vec3(0.0);
    vec3 totalMie = vec3(0.0);

    float iOdRlh = 0.0;
    float iOdMie = 0.0;

    float mu = dot(R, sunDir);
    float mumu = mu * mu;
    float gg = g * g;
    float pRlh = 3.0 / (16.0 * kPI) * (1.0 + mumu);
    float pMie = 3.0 / (8.0 * kPI) * ((1.0 - gg) * (mumu + 1.0)) / (pow(1.0 + gg - 2.0 * mu * g, 1.5) * (2.0 + gg));

    for (int i = 0; i < iSteps; i++)
    {
        vec3 iPos = eye + R * (iTime + iStepSize * 0.5);

        float iHeight = length(iPos) - uAtmosphere.planetRadius;

        float odStepRlh = exp(-iHeight / shRlh) * iStepSize;
        float odStepMie = exp(-iHeight / shMie) * iStepSize;

        float jStepSize = rsi(iPos, sunDir, uAtmosphere.atmosphereRadius).y / float(jSteps);

        float jTime = 0.0;

        float jOdRlh = 0.0;
        float jOdMie = 0.0;

        for (int j = 0; j < jSteps; j++)
        {
            vec3 jPos = iPos + sunDir * (jTime + jStepSize * 0.5);

            float jHeight = length(jPos) - uAtmosphere.planetRadius;

            jOdRlh += exp(-jHeight / shRlh) * jStepSize;
            jOdMie += exp(-jHeight / shMie) * jStepSize;

            jTime += jStepSize;
        }

        vec3 attn = exp(-(kMie * (iOdMie + jOdMie) + kRlh * (iOdRlh + jOdRlh)));

        totalRlh += odStepRlh * attn;
        totalMie += odStepMie * attn;

        iTime += iStepSize;
    }

    return uAtmosphere.sunColor * uAtmosphere.sunIntensity * (pRlh * kRlh * totalRlh + pMie * kMie * totalMie);
}

void main()
{
    vec3 eye = vec3(0.0, uCamera.position.y + uAtmosphere.planetRadius, 0.0);
    vec3 V = normalize(fs_in.Position);

    vec3 color = calcAtmosphere(V, eye);

    Color0 = vec4(color, 1.0);
}
