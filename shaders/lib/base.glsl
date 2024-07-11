@include "types.glsl"

layout (std140, binding = 0) uniform Common
{
    Camera uCamera;
    DirLight uSun;
    Atmosphere uAtmosphere;
};
