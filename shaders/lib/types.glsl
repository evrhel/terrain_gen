/* Common structure definitions */

struct Gbuffer
{
    sampler2D albedo;
    sampler2D emissive;
    sampler2D position;
    sampler2D depth;
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
        return texture(tex.tex, texCoords);
    }

    return vec4(tex.color, 1.0);
}

const vec3 kWorldUp = vec3(0.0, 1.0, 0.0);
const vec3 kWorldRight = vec3(1.0, 0.0, 0.0);
const vec3 kWorldFront = vec3(0.0, 0.0, 1.0);

vec3 nvec3(vec4 pos)
{
    return pos.xyz / pos.w;
}

vec4 nvec4(vec3 pos)
{
    return vec4(pos.xyz, 1.0);
}

float cdist(vec2 coord)
{
    return max(abs(coord.x - 0.5), abs(coord.y - 0.5)) * 2.0;
}
