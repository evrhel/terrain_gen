#version 410 core

@include "lib/types.glsl"
@include "lib/gbuffer.glsl"
@include "lib/atmosphere.glsl"

in float Height;

void main()
{
    float h = (Height + 16) / 64.0;

    Albedo = vec4(0.0, 0.0, 0.0, 1.0);
    Emissive = vec4(vec3(h), 1.0);
    PositionOut = vec4(0.0, 0.0, 0.0, 1.0);
    DepthOut = vec4(gl_FragCoord.zzz, 1.0);
    NormalOut = vec4(0.0, 0.0, 0.0, 1.0);
    MaterialOut = vec4(0.0, 0.0, 0.0, 1.0);
}
