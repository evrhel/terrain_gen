@include "types.glsl"

/* Output color buffers */
layout (location = 0) out vec4 Color0;
layout (location = 1) out vec4 Color1;
layout (location = 2) out vec4 Color2;
layout (location = 3) out vec4 Color3;

in VS_OUT
{
    vec2 TexCoords;
} fs_in;

/* Output from gbuffer, value is never overwritten */
uniform Gbuffer uGbuffer;

/* Arbitrary textures, usage depends on the shader */
uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform sampler2D uTexture2;
uniform sampler2D uTexture3;

/* Whether wireframe mode is enabled */
uniform bool uWireframe;
