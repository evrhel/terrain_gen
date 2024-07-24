layout (std140) uniform Atmosphere
{
    vec3 sunDirection;
    vec3 sunColor;
    float sunIntensity;

    float sunTightness;

    vec3 horizonColor;
    vec3 zenithColor;
    float atmosphereHeight;
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
