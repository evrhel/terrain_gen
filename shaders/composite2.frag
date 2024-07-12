#version 410 core

@include "lib/composite.glsl"

const float kGamma = 2.2;

void main()
{
    /* Linear color */
    vec3 colorLinear = texture(uTexture0, fs_in.TexCoords).rgb;

    /* Gamma correction */
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / kGamma));

    Color0 = vec4(colorGammaCorrected, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
