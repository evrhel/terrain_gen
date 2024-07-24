/* Texture utility functions. */

struct Texture
{
    sampler2D tex;
    bool hasTex;
    vec3 color;
};

vec4 sampleTexture(Texture tex, vec2 texCoords)
{
    if (tex.hasTex)
    {
        return texture(tex.tex, texCoords);
    }

    return vec4(tex.color, 1.0);
}
