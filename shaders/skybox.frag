#version 410 core

@include "lib/gbuffer.glsl"

in VS_OUT
{
    vec3 FragPos;
} fs_in;

void main()
{
    Albedo = vec4(1.0, 0.0, 0.0, 1.0);
    Emissive = vec4(0.0, 0.0, 0.0, 1.0);
    PositionOut = vec4(fs_in.FragPos, 1.0);
    NormalOut = vec4(0.0, 0.0, 0.0, 1.0);
    MaterialOut = vec4(0.0, 0.0, 0.0, 1.0);
}
