#version 410 core

layout (location = 0) in vec4 aVertex;  // <vec2 position, vec2 texCoord>

out VS_OUT
{
    vec2 TexCoords;
} vs_out;

void main()
{
    vs_out.TexCoords = aVertex.zw;
    gl_Position = vec4(aVertex.xy, 0.0, 1.0);
}
