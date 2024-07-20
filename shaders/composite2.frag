#version 410 core

@include "lib/composite.glsl"
@include "lib/camera.glsl"
@include "lib/atmosphere.glsl"

vec3 sampleColor(vec2 uv)
{
    vec2 texelSize = 1.0 / textureSize(uTexture0, 0);
    
    vec3 a = texture(uTexture0, uv + vec2(-texelSize.x, -texelSize.y)).rgb;
    vec3 b = texture(uTexture0, uv + vec2(0.0, -texelSize.y)).rgb * 2;
    vec3 c = texture(uTexture0, uv + vec2(texelSize.x, -texelSize.y)).rgb;

    vec3 d = texture(uTexture0, uv + vec2(-texelSize.x, 0.0)).rgb * 2;
    vec3 e = texture(uTexture0, uv).rgb * 4;
    vec3 f = texture(uTexture0, uv + vec2(texelSize.x, 0.0)).rgb * 2;

    vec3 g = texture(uTexture0, uv + vec2(-texelSize.x, texelSize.y)).rgb;
    vec3 h = texture(uTexture0, uv + vec2(0.0, texelSize.y)).rgb * 2;
    vec3 i = texture(uTexture0, uv + vec2(texelSize.x, texelSize.y)).rgb;

    return (a + b + c + d + e + f + g + h + i) / 16.0;
}

const float maxf = 3;
const float ref = 0.11;
const float inc = 3.0;

vec4 raytrace(vec3 fragpos, vec3 skycolor, vec3 rvector)
{
    float comp = 1.0 - uCamera.near / uCamera.far / uCamera.far;

    vec4 color = vec4(0.0);
    vec3 start = fragpos;
    rvector *= 1.2;
    fragpos += rvector;
    vec3 tvector = rvector;
    int sr = 0;

    for (int i = 0; i < 25; i++)
    {
        vec3 pos = nvec3(uCamera.proj * nvec4(fragpos)) * 0.5 + 0.5;
        if (pos.x < 0.0 || pos.x > 1.0 || pos.y < 0.0 || pos.y > 1.0 || pos.z < 0.0 || pos.z > 1.0)
            break;

        vec3 fragpos0 = texture(uGbuffer.position, pos.xy).xyz;
        float err = distance(fragpos, fragpos0);
        if (err < pow(length(rvector), 1.175))
        {
            sr++;
            if (sr >= maxf)
            {
                bool land = texture(uGbuffer.depth, pos.xy).r < comp;
                if (land)
                {
                    color.rgb = sampleColor(pos.xy);
                    color.a = 1.0;
                }
                
                break;
            }

            tvector -= rvector;
            rvector *= ref;
        }

        rvector *= inc;
        tvector += rvector;
        fragpos = start + tvector;
    }

    return color;
}

void main()
{
    /* Get base color */
    vec3 baseColor = texture(uTexture0, fs_in.TexCoords).rgb;
    if (uWireframe)
    {
        Color0 = vec4(baseColor, 1.0);
        Color1 = vec4(0.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }

    /* Check if fully metallic */
    vec3 material = texture(uGbuffer.material, fs_in.TexCoords).rgb;
    if (material.b != 1.0)
    {
        Color0 = vec4(baseColor, 1.0);
        Color1 = vec4(0.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }

    /* Get normal */
    vec3 N = texture(uGbuffer.normal, fs_in.TexCoords).rgb;
    if (N == vec3(0.0))
    {
        Color0 = vec4(baseColor, 1.0);
        Color1 = vec4(0.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }

    N = normalize(N);
    
    vec3 fragpos = texture(uGbuffer.position, fs_in.TexCoords).rgb;
    vec3 V = normalize(fragpos);

    vec3 R = normalize(reflect(V, N));

    float NdotV = dot(N, V);
    float fresnel = pow(clamp(1.0 + NdotV, 0.0, 1.0), 4.0);
    fresnel = mix(0.09, 1.0, fresnel); // F0

    vec4 viewDir = uCamera.invView * vec4(R, 0.0);
    vec3 skyColor = sampleAtmosphere(viewDir.xyz);

    vec4 reflection = raytrace(fragpos.xyz, skyColor, R);

    reflection.rgb = mix(skyColor, reflection.rgb, reflection.a);
    vec3 color = mix(baseColor, reflection.rgb, fresnel);

    Color0 = vec4(color, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
