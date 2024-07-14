#version 410 core

@include "lib/composite.glsl"

const float kGamma = 2.2;
const float kExposure = 1.0;

void main()
{
    /* Linear color */
    vec3 colorLinear = texture(uTexture0, fs_in.TexCoords).rgb;

    /* Bloom */
    vec3 bloom = texture(uTexture1, fs_in.TexCoords).rgb;
    colorLinear += bloom;

    /* Tonemapping */
    colorLinear = 1.0 - exp(-colorLinear * kExposure);

    /* Gamma correction */
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / kGamma));

    Color0 = vec4(colorGammaCorrected, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
