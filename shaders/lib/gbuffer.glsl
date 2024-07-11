/* Common fragment shader for G-buffer rendering */

@include "base.glsl"

out (location = 0) vec4 Albedo;         // Fragment color
out (location = 1) vec4 Emissive;       // Emissive color
out (location = 2) vec4 PositionOut;    // Fragment world-space position
out (location = 3) vec4 NormalOut;      // Fragment view-space normal
out (location = 4) vec4 MaterialOut;    // Fragment material [roughness, metallic, ao, 1]

uniform Atmosphere uAtmosphere;

uniform DirLight uSun;

uniform Camera uCamera;
