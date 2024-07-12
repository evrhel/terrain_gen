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
    gl_Position = uCamera.proj * mat4(mat3(uCamera.view)) * vec4(aPos, 1.0);
    gl_Position.z = gl_Position.w;
}
