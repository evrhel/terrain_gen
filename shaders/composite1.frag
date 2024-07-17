#version 410 core

@include "lib/composite.glsl"
@include "lib/atmosphere.glsl"
@include "lib/camera.glsl"

const float kPI = 3.14159265359;
const float kFogDensity = 0.0003;

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float num = a2;
    float denom = NdotH2 * (a2 - 1.0) + 1.0;
    denom = kPI * denom * denom;

    return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0;

    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = geometrySchlickGGX(NdotV, roughness);
    float ggx1 = geometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 calcSun(vec3 fragPos, vec3 N, float metallic, float roughness, vec3 albedo)
{
    if (uAtmosphere.sunIntensity <= 0.0)
        return vec3(0.0);

    vec3 V = normalize(uCamera.position - fragPos);
    vec3 L = normalize(-uAtmosphere.sunDirection);
    vec3 H = normalize(V + L);

    vec3 radiance = uAtmosphere.sunIntensity * uAtmosphere.sunColor;

    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    float NDF = distributionGGX(N, H, roughness);
    float G = geometrySmith(N, V, L, roughness);

    vec3 num = NDF * G * F;
    float denom = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
    vec3 specular = num / max(denom, 0.000001);

    vec3 kS = F;
    vec3 kD = 1.0 - kS;

    kD *= 1.0 - metallic;

    float NdotL = max(dot(N, L), 0.0);

    return (kD * albedo / kPI + specular) * radiance * NdotL;
}

void main()
{
    /* Sample albedo */
    vec3 albedo = texture(uGbuffer.albedo, fs_in.TexCoords).rgb;

    if (uWireframe)
    {
        Color0 = vec4(albedo, 1.0);
        Color1 = vec4(0.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }

    /* Sample gbuffer */
    vec3 emissive = texture(uGbuffer.emissive, fs_in.TexCoords).rgb;
    vec3 normal = texture(uGbuffer.normal, fs_in.TexCoords).rgb;

    if (normal == vec3(0.0))
    {
        /* No lighting */
        Color0 = vec4(albedo + emissive, 1.0);
        Color1 = vec4(0.0);
        Color2 = vec4(0.0);
        Color3 = vec4(0.0);
        return;
    }
    
    /* Sample gbuffer */
    vec3 position = texture(uGbuffer.position, fs_in.TexCoords).rgb;
    vec3 material = texture(uGbuffer.material, fs_in.TexCoords).rgb;

    float roughness = material.r;
    float metallic = material.g;
    float ao = material.b;

    /* Lighting */
    vec3 lighting = calcSun(position, normal, metallic, roughness, albedo);

    /* Ambient */
    float kD = 1.0 - metallic;
    vec3 irradiance = sampleAtmosphere(normal);
    vec3 diffuse = irradiance * albedo;
    
    vec3 V = normalize(position - uCamera.position);
    vec3 reflection = sampleAtmosphere(reflect(V, normal));

    vec3 ambient = mix(reflection, diffuse, kD) * ao; //kD * diffuse;// * ao;

    /* Final color */
    vec3 color = lighting + ambient;

    /* Fog */
    float dist = length(position - uCamera.position);
    float fog = 1.0 - exp(-dist * kFogDensity);
    color = mix(color, sampleAtmosphere(V), fog);

    Color0 = vec4(color, 1.0);

    /* No other layers */
    Color1 = vec4(0.0);
    Color2 = vec4(0.0);
    Color3 = vec4(0.0);
}
