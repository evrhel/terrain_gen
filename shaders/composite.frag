#version 430 core

@include "lib/composite.glsl"

void main()
{
    vec3 albedo = sampleTexture(uGbuffer.albedo, fs_in.TexCoords).rgb;
    FragColor = vec4(albedo, 1.0);
}
