#version 410 core

@include "lib/composite.glsl"

uniform vec2 uSize;

void main()
{
    vec2 texelSize = 1.0 / uSize;
    float x = texelSize.x;
    float y = texelSize.y;
    vec2 texCoords = fs_in.TexCoords;

    vec3 a = texture(uTexture0, vec2(texCoords.x - 2 * x, texCoords.y + 2 * y)).rgb;
    vec3 b = texture(uTexture0, vec2(texCoords.x, texCoords.y + 2 * y)).rgb;
    vec3 c = texture(uTexture0, vec2(texCoords.x + 2 * x, texCoords.y + 2 * y)).rgb;

    vec3 d = texture(uTexture0, vec2(texCoords.x - 2 * x, texCoords.y)).rgb;
    vec3 e = texture(uTexture0, texCoords).rgb;
    vec3 f = texture(uTexture0, vec2(texCoords.x + 2 * x, texCoords.y)).rgb;

    vec3 g = texture(uTexture0, vec2(texCoords.x - 2 * x, texCoords.y - 2 * y)).rgb;
    vec3 h = texture(uTexture0, vec2(texCoords.x, texCoords.y - 2 * y)).rgb;
    vec3 i = texture(uTexture0, vec2(texCoords.x + 2 * x, texCoords.y - 2 * y)).rgb;

    vec3 j = texture(uTexture0, vec2(texCoords.x - x, texCoords.y + y)).rgb;
    vec3 k = texture(uTexture0, vec2(texCoords.x + x, texCoords.y + y)).rgb;
    vec3 l = texture(uTexture0, vec2(texCoords.x - x, texCoords.y - y)).rgb;
    vec3 m = texture(uTexture0, vec2(texCoords.x + x, texCoords.y - y)).rgb;

    vec3 color = e * 0.125;
    color += (a + c + g + i) * 0.03125;
    color += (b + d + f + h) * 0.0625;
    color += (j + k + l + m) * 0.0625;

    Color0 = vec4(max(color, 0.0001), 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}