#version 410 core

@include "lib/composite.glsl"

uniform float uGamma;
uniform float uExposure;
uniform float uBloomStrength;

void main()
{
    /* Linear color */
    vec3 colorLinear = texture(uTexture0, fs_in.TexCoords).rgb;
    if (uWireframe)
    {
        Color0 = vec4(colorLinear, 1.0);
        Color1 = vec4(0.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }

    /* Bloom */
    vec3 bloom = texture(uTexture1, fs_in.TexCoords).rgb;
    colorLinear = mix(colorLinear, bloom, uBloomStrength);//bloom * uBloomStrength;

    /* Tonemapping */
    colorLinear = 1.0 - exp(-colorLinear * uExposure);

    /* Gamma correction */
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / uGamma));

    Color0 = vec4(colorGammaCorrected, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
