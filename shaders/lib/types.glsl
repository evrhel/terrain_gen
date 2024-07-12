/* Common structure definitions */

struct Gbuffer
{
    sampler2D albedo;
    sampler2D emissive;
    sampler2D normal;
    sampler2D material; // roughness, metallic, ao
};

struct Texture
{
    sampler2D tex;
    bool hasTex;
    vec3 color;
};

struct Material
{
    Texture albedo;
    Texture emissive;
    Texture normal;
    Texture roughness;
    Texture metallic;
    Texture ao;
};

vec4 sampleTexture(Texture tex, vec2 texCoords)
{
    if (tex.hasTex)
    {
        vec4 color = texture(tex.tex, texCoords);
        color.rgb *= tex.color;
        return color;
    }

    return vec4(tex.color, 1.0);
}

const vec3 kWorldUp = vec3(0.0, 1.0, 0.0);
const vec3 kWorldRight = vec3(1.0, 0.0, 0.0);
const vec3 kWorldFront = vec3(0.0, 0.0, 1.0);
