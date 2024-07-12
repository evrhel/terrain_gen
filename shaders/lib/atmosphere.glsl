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
