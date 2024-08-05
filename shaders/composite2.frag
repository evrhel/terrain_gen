#version 410 core

@include "lib/composite.glsl"
@include "lib/camera.glsl"
@include "lib/atmosphere.glsl"
@include "lib/material.glsl"
@include "lib/utils.glsl"

vec3 sampleColor(vec2 uv)
{
    /*vec2 texelSize = 1.0 / textureSize(uTexture0, 0);
    
    vec3 a = texture(uTexture0, uv + vec2(-texelSize.x, -texelSize.y)).rgb;
    vec3 b = texture(uTexture0, uv + vec2(0.0, -texelSize.y)).rgb * 2;
    vec3 c = texture(uTexture0, uv + vec2(texelSize.x, -texelSize.y)).rgb;

    vec3 d = texture(uTexture0, uv + vec2(-texelSize.x, 0.0)).rgb * 2;
    vec3 e = texture(uTexture0, uv).rgb * 4;
    vec3 f = texture(uTexture0, uv + vec2(texelSize.x, 0.0)).rgb * 2;

    vec3 g = texture(uTexture0, uv + vec2(-texelSize.x, texelSize.y)).rgb;
    vec3 h = texture(uTexture0, uv + vec2(0.0, texelSize.y)).rgb * 2;
    vec3 i = texture(uTexture0, uv + vec2(texelSize.x, texelSize.y)).rgb;

    return (a + b + c + d + e + f + g + h + i) / 16.0;*/
    return texture(uTexture0, uv).rgb;
}

const float maxf = 3;
const float ref = 0.11;
const float inc = 3.0;

vec4 raytrace(vec3 fragpos, vec3 rvector)
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
                    /* Do not reflect reflective materials */
                    MaterialInfo material;
                    decodeMaterial(texture(uGbuffer.material, pos.xy), material);
                    if (!material.reflective)
                    {
                        color.rgb = sampleColor(pos.xy);
                        color.a = 1.0;
                    }
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

const float kCloudHeight = 300;
const float kMaxHeight = kCloudHeight + 200;
const vec3 kCloudColor2 = vec3(0.5);
const vec3 kCloudColor = vec3(1);

float densityAtPos(vec3 pos)
{
    pos /= 18.0;
    pos.xz *= 0.5;

    vec3 p = floor(pos);
    vec3 f = fract(pos);

    f = (f * f) * (3.0 - 2.0 * f);
    f = sqrt(f);
    vec2 uv = p.xz + f.xz + p.y * 17.0;

    vec2 coord = uv / 64.0;
    vec2 coord2 = uv / 64.0 + 17.0 / 64.0;
    float xy1 = texture(uNoiseTex, coord).x;
    float xy2 = texture(uNoiseTex, coord2).x;
    return mix(xy1, xy2, f.y);
}

float cloudPlane(vec3 pos)
{
    float center = kCloudHeight * 0.5 + kMaxHeight * 0.5;
    float difcenter = kMaxHeight - center;
    float mult = (pos.y - center) / difcenter;

    vec3 samplePos = pos * vec3(0.5) * 0.35 + uTime * vec3(0.5, 0, 0.5);
    float noise = 0.0;
    float tot = 0.0;
    for (int i = 0; i < 4; i++)
    {
        noise += densityAtPos(samplePos * exp(i * 1.05) * 0.6 + uTime * i * vec3(0.5, 0, 0.5) * 0.6) * exp(-i * 0.8);
        tot += exp(-i * 0.8);
    }

    return 1.0 - pow(0.4, max(noise / tot - 0.56 - mult * mult * 0.3, 0.0) * 2.2);
}

vec3 renderClouds(vec3 viewPos, vec3 color, const int cloudIT, inout float occlusion)
{
    float dither = fract(0.75487765 * gl_FragCoord.x + 0.56984026 * gl_FragCoord.y);

    vec3 pos = nvec3(uCamera.invView * nvec4(viewPos));
    vec3 V = normalize(vec3(uCamera.invView * vec4(normalize(viewPos), 0.0)));

    vec3 dV_view = V;
    vec3 progress_view = vec3(0.0);

    if (uCamera.position.y <= kCloudHeight)
    {
        float maxHeight2 = min(kMaxHeight, pos.y);

        dV_view *= -(maxHeight2 - kCloudHeight) / dV_view.y / cloudIT;
        progress_view = dV_view * dither + uCamera.position + dV_view * (maxHeight2 - uCamera.position.y) / dV_view.y;
        if (pos.y < kCloudHeight)
            return color;
    }
    else if (uCamera.position.y < kMaxHeight)
    {
        if (dV_view.y <= 0.0)
        {
            float maxHeight2 = max(kCloudHeight, pos.y);

            dV_view *= abs(maxHeight2 - uCamera.position.y) / abs(dV_view.y) / cloudIT;
            progress_view = dV_view * dither + uCamera.position + dV_view * cloudIT;
            dV_view *= -1.0;
        }
        else
        {
            float maxHeight2 = min(kMaxHeight, pos.y);

            dV_view *= -abs(maxHeight2 - uCamera.position.y) / abs(dV_view.y) / cloudIT;
            progress_view = dV_view * dither + uCamera.position - dV_view * cloudIT;
        }
    }
    else
    {
        float maxHeight2 = min(kMaxHeight, pos.y);

        dV_view *= -abs(maxHeight2 - kMaxHeight) / abs(dV_view.y) / cloudIT;
        progress_view = dV_view * dither + uCamera.position + dV_view * (maxHeight2 - uCamera.position.y) / dV_view.y;
        if (pos.y > kMaxHeight)
            return color;
    }

    float mult = length(dV_view) / 256.0;

    vec3 cloudBaseColor = sampleSky(vec3(0, 1, 0));
    vec3 cloudIllumColor = sampleSky(V) + uAtmosphere.sunColor * uAtmosphere.sunIntensity;
    
    /* Subsurface scattering */
    float intensity = max(dot(V, -uAtmosphere.sunDirection), 0.0);
    intensity = pow(intensity, sqrt(uAtmosphere.sunTightness));
    cloudIllumColor += uAtmosphere.sunColor * uAtmosphere.sunIntensity * intensity;

    for (int i = 0; i < cloudIT; i++)
    {
        float cloud = cloudPlane(progress_view) * 40.0;
        float lightsourceVis = pow(clamp(progress_view.y - kCloudHeight, 0, 200) / 200, 2.3);

        vec3 cloudColor = mix(cloudBaseColor * 0.05, cloudIllumColor * 0.15, lightsourceVis);
        
        float amount = 1.0 - exp(-cloud * mult);
        color = mix(color, cloudColor, amount);
        occlusion -= amount;

        progress_view += dV_view;
    }

    occlusion = clamp(occlusion, 0.0, 1.0);
    return color;
}

void main()
{
    float occlusion = texture(uGbuffer.depth, fs_in.TexCoords).r;
    occlusion = occlusion < 1.0 ? 0.0 : 1.0;

    /* Get base color */
    vec3 baseColor = texture(uTexture0, fs_in.TexCoords).rgb;
    if (uWireframe)
    {
        Color0 = vec4(baseColor, 1.0);
        Color1 = vec4(baseColor * occlusion, 1.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }

    vec3 fragpos = texture(uGbuffer.position, fs_in.TexCoords).rgb;

    /* Apply fog */
    //float fogF = calcFog(uCamera.position, nvec3(uCamera.invView * nvec4(fragpos)));
   // baseColor = mix(baseColor, vec3(1,0,0), fogF);
    
    /* Volumetric clouds */
    baseColor = renderClouds(fragpos, baseColor, 8, occlusion);
    
    /* Get material */
    MaterialInfo material;
    decodeMaterial(texture(uGbuffer.material, fs_in.TexCoords), material);

    if (!material.reflective)
    {
        Color0 = vec4(baseColor, 1.0);
        Color1 = vec4(baseColor * occlusion, 1.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }

    /* Get normal */
    vec3 N = texture(uGbuffer.normal, fs_in.TexCoords).rgb;
    if (N == vec3(0.0))
    {
        Color0 = vec4(baseColor, 1.0);
        Color1 = vec4(baseColor * occlusion, 1.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }

    N = normalize(N);
    
    vec3 V = normalize(fragpos);

    vec3 R = normalize(reflect(V, N));

    float NdotV = dot(N, V);
    float fresnel = pow(clamp(1.0 + NdotV, 0.0, 1.0), 4.0);
    fresnel = mix(0.09, 1.0, fresnel); // F0
    
    vec3 worldPos = nvec3(uCamera.invView * nvec4(fragpos));

    vec4 viewDir = normalize(uCamera.invView * vec4(R, 0.0));
    vec3 sunColor;
    vec3 skyColor = sampleSky(viewDir.xyz) + sampleSun(viewDir.xyz);
    //skyColor += sunColor;

    vec4 reflection = raytrace(fragpos.xyz, R);

    reflection.rgb = mix(skyColor, reflection.rgb, reflection.a);
    vec3 color = mix(baseColor, reflection.rgb, fresnel);

    Color0 = vec4(color, 1.0);
    Color1 = vec4(color * occlusion, 1.0);

    /* No other layers */
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
