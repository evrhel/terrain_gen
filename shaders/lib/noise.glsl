
/* Noise texture */
uniform sampler2D uNoiseTex;

float getNoise(vec2 coord)
{
    //vec2 pixel = coord * textureSize(uTexture0, 0);
    //coord = fract(pixel / textureSize(uNoiseTex, 0));
    return texture(uNoiseTex, coord).x;
}
