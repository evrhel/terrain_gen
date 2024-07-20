#version 410 core

@include "lib/camera.glsl"

layout (location = 0) in vec3 aPos;

out VS_OUT
{
    vec3 FragPos;
} vs_out;

void main()
{
    vec4 viewPos = mat4(mat3(uCamera.view)) * vec4(aPos, 1.0);
    vs_out.FragPos = viewPos.xyz;
    gl_Position = uCamera.proj * viewPos;
    gl_Position.z = gl_Position.w;
}
