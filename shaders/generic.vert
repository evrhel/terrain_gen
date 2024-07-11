#version 430 core

@include "lib/types.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

in VS_OUT
{
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} vs_out;

uniform Camera uCamera;

uniform mat4 uModel;

void main()
{
    vs_out.FragPos = aPos;
    vs_out.Normal = normalize(transpose(inverse(mat3(uModel))) * aNormal);
    vs_out.TexCoords = aTexCoords;
    gl_Position = uCamera.viewProjection * uModel * vec4(aPos, 1.0);
}
