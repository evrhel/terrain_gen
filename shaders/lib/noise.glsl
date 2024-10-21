
/* Noise texture */
uniform sampler2D uNoiseTex;

float getNoise(vec2 coord)
{
    return texture(uNoiseTex, coord).x;
}

float getNoise(vec3 coord)
{
    float zFrac = fract(coord.z);

    vec2 coordLayer1 = coord.xy;
    vec2 coordLayer2 = coord.xy + vec2(0.5, 0.5);

    float noiseLayer1 = getNoise(coordLayer1);
    float noiseLayer2 = getNoise(coordLayer2);

    return mix(noiseLayer1, noiseLayer2, zFrac);
}
