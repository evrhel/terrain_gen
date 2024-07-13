/* Common fragment shader for G-buffer rendering */

layout (location = 0) out vec4 Albedo;         // Fragment color
layout (location = 1) out vec4 Emissive;       // Emissive color
layout (location = 2) out vec4 PositionOut;    // Fragment world-space position
layout (location = 3) out vec4 DepthOut;       // Fragment depth
layout (location = 4) out vec4 NormalOut;      // Fragment view-space normal
layout (location = 5) out vec4 MaterialOut;    // Fragment material [roughness, metallic, ao, 1]

uniform bool uWireframe; // Whether wireframe mode is enabled
