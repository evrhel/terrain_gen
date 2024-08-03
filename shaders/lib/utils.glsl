/* Various utility functions */

const vec3 kWorldUp = vec3(0.0, 1.0, 0.0);
const vec3 kWorldRight = vec3(1.0, 0.0, 0.0);
const vec3 kWorldFront = vec3(0.0, 0.0, 1.0);

vec3 nvec3(vec4 pos)
{
    return pos.xyz / pos.w;
}

vec4 nvec4(vec3 pos)
{
    return vec4(pos.xyz, 1.0);
}

float cdist(vec2 coord)
{
    return max(abs(coord.x - 0.5), abs(coord.y - 0.5)) * 2.0;
}

float edgefade(vec2 coord, float edge)
{
    float dT = 1.0 - coord.y;
    float dB = coord.y;
    float dR = 1.0 - coord.x;
    float dL = coord.x;

    float mindist = min(min(dT, dB), min(dR, dL));

    return clamp(mindist / edge, 0.0, 1.0);
}
