#version 410 core

@include "lib/camera.glsl"

layout (location = 0) in vec3 aPos;

out VS_OUT
{
    vec3 FragPos;
} vs_out;

void main()
{
    vs_out.FragPos = aPos;
    gl_Position = uCamera.viewProj * vec4(aPos, 1.0);
}
