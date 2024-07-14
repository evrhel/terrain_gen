#version 410 core

@include "lib/composite.glsl"

uniform float uFilterRadius;

void main()
{
    float x = uFilterRadius;
    float y = uFilterRadius;
    vec2 texCoords = fs_in.TexCoords;

    vec3 a = texture(uTexture0, vec2(texCoords.x - x, texCoords.y + y)).rgb;
    vec3 b = texture(uTexture0, vec2(texCoords.x, texCoords.y + y)).rgb;
    vec3 c = texture(uTexture0, vec2(texCoords.x + x, texCoords.y + y)).rgb;

    vec3 d = texture(uTexture0, vec2(texCoords.x - x, texCoords.y)).rgb;
    vec3 e = texture(uTexture0, texCoords).rgb;
    vec3 f = texture(uTexture0, vec2(texCoords.x + x, texCoords.y)).rgb;

    vec3 g = texture(uTexture0, vec2(texCoords.x - x, texCoords.y - y)).rgb;
    vec3 h = texture(uTexture0, vec2(texCoords.x, texCoords.y - y)).rgb;
    vec3 i = texture(uTexture0, vec2(texCoords.x + x, texCoords.y - y)).rgb;

    vec3 color = e * 4.0;
    color += (b + d + f + h) * 2.0;
    color += (a + c + g + i);
    color *= 1.0 / 16.0;

    Color0 = vec4(color, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}