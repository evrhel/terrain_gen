#version 410 core

layout (location = 0) in vec3 aPos;

out VS_OUT
{
    vec3 Position;
} vs_out;

void main()
{
    vs_out.Position = aPos;
    //gl_Position = vec4(aPos, 1.0);
}
