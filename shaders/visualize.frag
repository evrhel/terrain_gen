#version 410 core

@include "lib/composite.glsl"

const int kVisualizeAlbedo = 1;
const int kVisualizeEmissive = 2;
const int kVisualizePosition = 3;
const int kVisualizeDepth = 4;
const int kVisualizeNormal = 5;
const int kVisualizeMaterial = 6;

uniform int uMode;

void main()
{
    vec3 color = vec3(0.0);
    if (uMode == kVisualizeAlbedo)
    {
        color = texture(uGbuffer.albedo, fs_in.TexCoords).rgb;
    }
    else if (uMode == kVisualizeEmissive)
    {
        color = texture(uGbuffer.emissive, fs_in.TexCoords).rgb;
    }
    else if (uMode == kVisualizePosition)
    {
        color = texture(uGbuffer.position, fs_in.TexCoords).rgb;
    }
    else if (uMode == kVisualizeDepth)
    {
        color = texture(uGbuffer.depth, fs_in.TexCoords).rrr;
    }
    else if (uMode == kVisualizeNormal)
    {
        color = texture(uGbuffer.normal, fs_in.TexCoords).rgb * 0.5 + 0.5;
    }
    else if (uMode == kVisualizeMaterial)
    {
        color = texture(uGbuffer.material, fs_in.TexCoords).rgb;
    }
    
    Color0 = vec4(color, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
