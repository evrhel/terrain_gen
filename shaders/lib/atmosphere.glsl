layout (std140) uniform Atmosphere
{
    vec3 sunDirection;
    vec3 sunColor; // True sun color
    vec3 sunColorMask; // Sun mask color
    vec3 sunPosition; // Sun position in screen space, [0, 1]
    vec3 sunPositionWorld; // Sun position in world space
    float sunIntensity;

    float sunTightness;

    vec3 horizonColor;
    vec3 zenithColor;

    float fogDensity;

    float planetRadius;
    float atmosphereRadius;
    float Hr; // Thickness of the atmosphere (Rayleigh scattering)
    float Hm; // Thickness of the atmosphere (Mie scattering)

    vec3 fogColor;

    float g; // Mie scattering phase function
} uAtmosphere;

uniform samplerCube uSkybox;
uniform samplerCube uIrradiance;
uniform samplerCube uStarbox;

/*vec3 sampleAtmosphere(vec3 direction)
{
    float cosTheta = max(dot(direction, vec3(0.0, 1.0, 0.0)), 0.0);
    return mix(uAtmosphere.horizonColor, uAtmosphere.zenithColor, cosTheta);
}*/

vec3 sampleSun(vec3 direction)
{
    if (direction.y < 0.0)
        return vec3(0.0); // below horizon

    float angle = max(dot(direction, -uAtmosphere.sunDirection), 0.0);
    float intensity = step(0.5, pow(angle, uAtmosphere.sunTightness));
    return uAtmosphere.sunColor * uAtmosphere.sunIntensity * intensity * 25.0;
}

float sampleEclipse(vec3 direction)
{
    float angle = 1.0 - max(dot(direction, -uAtmosphere.sunDirection), 0.0);
    angle *= uAtmosphere.sunTightness;

    float sun = exp(-angle * angle * angle);
    float eclipse = pow(sun, 1.0001);

    return step(0.75, 1.0 - eclipse);
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

vec3 sampleSky(vec3 R)
{
    return texture(uSkybox, R).rgb;
}

vec3 sampleIrradiance(vec3 R)
{
    return texture(uIrradiance, R).rgb;
}
