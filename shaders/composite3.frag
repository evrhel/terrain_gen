#version 410 core

@include "lib/composite.glsl"
@include "lib/atmosphere.glsl"
@include "lib/utils.glsl"
@include "lib/camera.glsl"

vec3 godrays(vec3 baseColor)
{
    const float kStrength = 0.2;
    const int kSamples = 100;
    const float kDensity = 0.98;
    //const float kWeight = 0.01;
    const float kDecay = 0.99;

    vec3 sunPosition = uAtmosphere.sunPosition;
    if (sunPosition.x < 0.0 || sunPosition.x > 1.0 || sunPosition.y < 0.0 || sunPosition.y > 1.0 || sunPosition.z <= 0.0)
        return baseColor * (1.0 - kStrength);
        
    vec2 texCoord = fs_in.TexCoords;

    vec2 deltaTexCoord = texCoord - sunPosition.xy;
    deltaTexCoord *= 1.0 / float(kSamples) * kDensity;

    float decay = 1.0;

    vec3 color = vec3(0.0);
    for (int i = 0; i < kSamples; i++)
    {
        texCoord -= deltaTexCoord;
        vec3 occ = texture(uTexture1, texCoord).rgb; // Occlusion buffer
        
        color += occ * decay / float(kSamples); // * kWeight;

        decay *= kDecay;
    }

    color = color * edgefade(sunPosition.xy, 0.1) * kStrength;

    return mix(baseColor, color, kStrength);
}

void main()
{
    /* Base color */
    vec3 color = texture(uTexture0, fs_in.TexCoords).rgb;

    /* Godrays */
    color = godrays(color);

    Color0 = vec4(color, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
