#version 410 core

@include "lib/gbuffer.glsl"
@include "lib/atmosphere.glsl"

in VS_OUT
{
    vec3 FragPos;
} fs_in;

void main()
{
    /* View direction */
    vec3 direction = normalize(fs_in.FragPos);
    
    /* Sky color */
    vec3 color = sampleAtmosphere(direction);

    /* Draw sun */
    float intensity = max(dot(direction, -uAtmosphere.sunDirection), 0.0);
    intensity = pow(intensity, uAtmosphere.sunTightness);
    color += vec3(1.0) * intensity * 1.0 * uAtmosphere.sunIntensity;

    Albedo = vec4(color, 1.0);
    Emissive = vec4(0.0, 0.0, 0.0, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    NormalOut = vec4(0.0, 0.0, 0.0, 1.0);
    MaterialOut = vec4(0.0, 0.0, 0.0, 1.0);
}
