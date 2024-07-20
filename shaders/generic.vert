#version 410 core

@include "lib/camera.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

out VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform mat4 uModel;

void main()
{
    mat4 modelView = uCamera.view * uModel;
    vec4 viewPos = modelView * vec4(aPos, 1.0);

    vs_out.FragPos = viewPos.xyz;
    vs_out.Normal = normalize(transpose(inverse(mat3(modelView))) * aNormal);
    vs_out.TexCoords = aTexCoords;
    gl_Position = uCamera.proj * viewPos;
}
