#version 410 core

@include "lib/atmosphere.glsl"
@include "lib/camera.glsl"

layout (location = 0) out vec4 Color0;

in GS_OUT
{
    vec3 Position;
} fs_in;

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

// R: ray direction (world space)
// eye: ray origin (world space)
vec3 calcAtmosphere(vec3 R, vec3 eye)
{
    const vec3 kBetaR = vec3(3.8e-6, 12.5e-6, 33.1e-6); // Rayleigh scattering coefficient
    const vec3 kBetaM = vec3(21e-6); // Mie scattering coefficient
    const int kNumSamples = 16;
    const int kLightSamples = 8;
    const float kPi = 3.14159265358979323846;

    vec2 p_atmo = rsi(eye, R, uAtmosphere.atmosphereRadius);
    if (p_atmo.x > p_atmo.y || p_atmo.y < 0.0)
        return vec3(0.0); // No intersection

    vec3 sunDirection = -normalize(uAtmosphere.sunDirection);

    float tmin = max(p_atmo.x, 0.0);
    float tmax = p_atmo.y;

    float segLength = (tmax - tmin) / float(kNumSamples);
    float t = tmin;

    vec3 sumR = vec3(0.0);
    vec3 sumM = vec3(0.0);
    float opticalDepthR = 0.0;
    float opticalDepthM = 0.0;

    float mu = dot(R, sunDirection);
    float phaseR = 3.0 / (16.0 * kPi) * (1.0 + mu * mu);
    float g = uAtmosphere.g;// 0.76;
    float phaseM = 3.0 / (8.0 * kPi) * ((1.0 - g * g) * (1.0 + mu * mu)) / ((2.0 * g * g) * pow(1.0 + g * g - 2.0 * g * mu, 1.5));

    for (int i = 0; i < kNumSamples; i++)
    {
        vec3 pos = eye + (t + segLength * 0.5) * R;

        float height = length(pos) - uAtmosphere.planetRadius;

        // Optical depth
        opticalDepthR += exp(-height / uAtmosphere.Hr) * segLength;
        opticalDepthM += exp(-height / uAtmosphere.Hm) * segLength;

        vec2 p = rsi(pos, sunDirection, uAtmosphere.atmosphereRadius);
        float segLengthLight = p.y / float(kLightSamples);
        float tLight = 0.0;

        float opticalDepthLightR = 0.0;
        float opticalDepthLightM = 0.0;
        
        int j;
        for (j = 0; j < kLightSamples; j++)
        {
            vec3 posLight = pos + (tLight + segLengthLight * 0.5) * sunDirection;
            float heightLight = length(posLight) - uAtmosphere.planetRadius;

            if (heightLight < 0.0)
                break;

            opticalDepthLightR += exp(-heightLight / uAtmosphere.Hr) * segLengthLight;
            opticalDepthLightM += exp(-heightLight / uAtmosphere.Hm) * segLengthLight;

            tLight += segLengthLight;
        }

        if (j == kLightSamples)
        {
            vec3 tau = kBetaR * (opticalDepthR + opticalDepthLightR) + kBetaM * 1.1 * (opticalDepthM + opticalDepthLightM);
            vec3 attn = exp(-tau);

            sumR += attn * uAtmosphere.Hr;
            sumM += attn * uAtmosphere.Hm;
        }

        t += segLength;
    }

    return (/*sumR * kBetaR * phaseR + */sumM * kBetaM * phaseM) * uAtmosphere.sunIntensity * uAtmosphere.sunColor;
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
