#version 410 core

@include "lib/composite.glsl"
@include "lib/material.glsl"
@include "lib/camera.glsl"

const int kVisualizeAlbedo = 1;
const int kVisualizeEmissive = 2;
const int kVisualizePosition = 3;
const int kVisualizeDepth = 4;
const int kVisualizeNormal = 5;
const int kVisualizeMaterial = 6;
const int kVisualizeCompositor = 7;

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
        /* Linearize depth */
        float depth = texture(uGbuffer.depth, fs_in.TexCoords).r;  
        depth = (2.0 * uCamera.near) / (uCamera.far + uCamera.near - depth * (uCamera.far - uCamera.near));

        color = vec3(depth);
    }
    else if (uMode == kVisualizeNormal)
    {
        /* World-space normal */
        vec3 N = texture(uGbuffer.normal, fs_in.TexCoords).rgb;
        N = vec3(uCamera.invView * vec4(N, 0.0));

        color = N * 0.5 + 0.5;
    }
    else if (uMode == kVisualizeMaterial)
    {
        MaterialInfo material;
        decodeMaterial(texture(uGbuffer.material, fs_in.TexCoords), material);
        color = vec3(material.roughness, material.metallic, material.ao);
    }
    else if (uMode == kVisualizeCompositor)
    {
        color = texture(uTexture0, fs_in.TexCoords).rgb;
    }
    
    Color0 = vec4(color, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
