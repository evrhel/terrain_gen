#version 410 core

@include "lib/gbuffer.glsl"
@include "lib/atmosphere.glsl"
@include "lib/camera.glsl"
@include "lib/types.glsl"

in VS_OUT
{
    vec3 FragPos;
} fs_in;

void main()
{
    vec3 V = normalize(vec3(uCamera.invView * vec4(normalize(fs_in.FragPos), 0.0)));
    
    /* Sky color */
    vec3 color = sampleAtmosphere(V);

    /* Draw sun */
    float intensity = max(dot(V, -uAtmosphere.sunDirection), 0.0);
    intensity = pow(intensity, uAtmosphere.sunTightness);
    color += uAtmosphere.sunColor * intensity * uAtmosphere.sunIntensity;

    Albedo = vec4(0.0, 0.0, 0.0, 1.0);
    Emissive = vec4(color, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(0.0, 0.0, 0.0, 1.0);
    MaterialOut = vec4(0.0, 0.0, 0.0, 1.0);
}
