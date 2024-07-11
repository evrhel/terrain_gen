/* Common structure definitions */

struct DirLight
{
    vec3 direction;
    vec3 color;
    float intensity;
};

struct Atmosphere
{
    float sunSize;
    float sunFalloff;

    vec3 horizonColor;
    vec3 zenithColor;
    float atmosphereHeight;
};

struct Gbuffer
{
    sampler2D albedo;
    sampler2D emissive;
};

struct Texture
{
    sampler2D tex;
    bool useTexture;
    vec4 color;
};

struct Camera
{
    vec3 position;
    vec3 direction;

    float near;
    float far;

    mat4 view;
    mat4 invView;

    mat4 projection;
    mat4 invProjection;

    mat4 viewProjection;
    mat4 invViewProjection;
};

/* Common functions */

vec3 sampleAtmosphere(vec3 direction)
{
    // TODO: Implement
    return vec3(0.0);
}

vec4 sampleTexture(Texture texture, vec2 texCoords)
{
    if (texture.useTexture)
    {
        return texture2D(texture.tex, texCoords) * texture.color;
    }

    return texture.color;
}

