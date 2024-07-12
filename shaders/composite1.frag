#version 410 core

@include "lib/composite.glsl"

void main()
{
    /* Base color */
    vec3 albedo = texture(uGbuffer.albedo, fs_in.TexCoords).rgb;
    vec3 emissive = texture(uGbuffer.emissive, fs_in.TexCoords).rgb;

    vec3 color = albedo + emissive;

    // TODO: lighting

    Color0 = vec4(color, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
