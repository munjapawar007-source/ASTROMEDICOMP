#version 460 core

in vec3 out_Color;
in vec2 out_TexCoord;
in vec3 out_Normal;
in vec3 out_worldPos;
in vec3 out_vertexInCameraSpace;
in float out_worldHeight;

uniform float uTime;

uniform sampler2D uAlbedo;
uniform int uHasTexture;
uniform vec4 uBaseColor;

uniform sampler2D uMetallicRoughness;
uniform sampler2D uEmissiveTex;
uniform int uHasMetallicRoughness;
uniform int uHasEmissiveTex;
uniform float uRoughness;
uniform float uMetallic;
uniform vec3 uEmissive;
uniform vec3 uCameraPos;

uniform vec3 uDirectionalLightDirection;
uniform vec3 uDirectionalLightColor;
uniform float uDirectionalLightIntensity;

uniform vec3 uAmbientLightColor;
uniform float uAmbientLightIntensity;

struct SpotLight
{
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
    float cutOff;
    float outerCutOff;
    float range;
};

#define MAX_SPOT_LIGHTS 48
uniform int uNumSpotLights;
uniform SpotLight uSpotLights[MAX_SPOT_LIGHTS];

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
    float range;
};

#define MAX_POINT_LIGHTS 48
uniform int uNumPointLights;
uniform PointLight uPointLights[MAX_POINT_LIGHTS];

uniform sampler2DShadow uSpotShadowMap;
uniform mat4 uSpotLightSpaceMatrix;
uniform int uSpotShadowEnabled;
uniform int uSpotShadowCasterIdx;

uniform int ufogEnabled;
uniform vec3 ufogColor;
uniform float ufogDensity;
uniform int uFogType;
uniform float uFogHeight;
uniform float uFogFalloff;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 EmissiveOut;

float calcSpotShadow(vec3 worldPos, vec3 normal, vec3 lightDir)
{
    vec4 posLS = uSpotLightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 proj = posLS.xyz / posLS.w * 0.5 + 0.5;
    if (proj.x < 0.0 || proj.x > 1.0 || proj.y < 0.0 || proj.y > 1.0 || proj.z > 1.0)
        return 1.0;

    float cosTheta = max(dot(normal, normalize(-lightDir)), 0.0);
    float bias = max(0.0005 * (1.0 - cosTheta), 0.00005);

    float shadow = 0.0;
    vec2 sz = 1.0 / vec2(textureSize(uSpotShadowMap, 0));
    for (int x = -1; x <= 1; x++)
        for (int y = -1; y <= 1; y++)
            shadow += texture(uSpotShadowMap, vec3(proj.xy + vec2(x, y) * sz, proj.z - bias));
    return shadow / 9.0;
}

float calculateDepthFog()
{
    float d = length(out_vertexInCameraSpace);
    return clamp(1.0 - exp(-ufogDensity * d), 0.0, 1.0);
}

float calculateHeightFog()
{
    float h = max(uFogHeight - out_worldHeight, 0.0);
    return clamp(1.0 - exp(-h * uFogFalloff), 0.0, 1.0);
}

void main(void)
{
    vec4 baseColor;
    if (uHasTexture == 1)
        baseColor = texture(uAlbedo, out_TexCoord) * uBaseColor;
    else
        baseColor = vec4(out_Color, 1.0) * uBaseColor;

    vec3 albedo = baseColor.rgb;
    vec3 N = normalize(out_Normal);

    float roughness = uRoughness;
    float metallic = uMetallic;
    if (uHasMetallicRoughness == 1)
    {
        vec4 mr = texture(uMetallicRoughness, out_TexCoord);
        roughness *= mr.g;
        metallic *= mr.b;
    }

    vec3 emissive;
    if (uHasEmissiveTex == 1)
        emissive = texture(uEmissiveTex, out_TexCoord).rgb * uEmissive;
    else
        emissive = albedo * uEmissive;

    float r2 = (1.0 - roughness) * (1.0 - roughness);
    float shininess = mix(1.0, 256.0, r2);
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 diffAlbedo = albedo * (1.0 - 0.9 * metallic);
    vec3 V = normalize(uCameraPos - out_worldPos);

    vec3 ambient = uAmbientLightColor * uAmbientLightIntensity;

    vec3 Ld = normalize(-uDirectionalLightDirection);
    float NdotLd = max(dot(N, Ld), 0.0);
    vec3 Hd = normalize(Ld + V);
    vec3 dirDiff = diffAlbedo * uDirectionalLightColor * uDirectionalLightIntensity * NdotLd;
    vec3 dirSpec = F0 * pow(max(dot(N, Hd), 0.0), shininess) * uDirectionalLightColor * uDirectionalLightIntensity * NdotLd;

    vec3 litColor = diffAlbedo * ambient + dirDiff + dirSpec;

    for (int i = 0; i < uNumPointLights; i++)
    {
        vec3 toLight = uPointLights[i].position - out_worldPos;
        float dist = length(toLight);
        vec3 L = toLight / dist;
        float rangeAtten = smoothstep(uPointLights[i].range, 0.0, dist);
        float NdotL = max(dot(N, L), 0.0);
        vec3 Hp = normalize(L + V);
        vec3 pDiff = diffAlbedo * uPointLights[i].color * uPointLights[i].intensity * NdotL * rangeAtten;
        vec3 pSpec = F0 * pow(max(dot(N, Hp), 0.0), shininess) * uPointLights[i].color * uPointLights[i].intensity * NdotL * rangeAtten;
        float fogAtten = 1.0;
        if (ufogEnabled == 1)
            fogAtten = exp(-ufogDensity * dist);
        litColor += 3.0 * (pDiff + pSpec) * fogAtten;
    }

    for (int i = 0; i < uNumSpotLights; i++)
    {
        vec3 toLight = uSpotLights[i].position - out_worldPos;
        float dist = length(toLight);
        vec3 L = toLight / dist;
        float rangeAtten = smoothstep(uSpotLights[i].range, 0.0, dist);
        float theta = dot(L, normalize(-uSpotLights[i].direction));
        float epsilon = uSpotLights[i].cutOff - uSpotLights[i].outerCutOff;
        float coneInt = clamp((theta - uSpotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
        float NdotL = max(dot(N, L), 0.0);
        vec3 Hs = normalize(L + V);
        vec3 sDiff = diffAlbedo * uSpotLights[i].color * uSpotLights[i].intensity * NdotL * coneInt * rangeAtten;
        vec3 sSpec = F0 * pow(max(dot(N, Hs), 0.0), shininess) * uSpotLights[i].color * uSpotLights[i].intensity * NdotL * coneInt * rangeAtten;
        float fogAtten = 1.0;
        if (ufogEnabled == 1)
            fogAtten = exp(-ufogDensity * dist);
        float sShadow = (uSpotShadowEnabled == 1 && i == uSpotShadowCasterIdx)
                            ? calcSpotShadow(out_worldPos, N, uSpotLights[i].direction)
                            : 1.0;
        litColor += 3.0 * (sDiff + sSpec) * fogAtten * sShadow;
    }

    if (ufogEnabled == 1)
    {
        float fogAmount = (uFogType == 0) ? calculateDepthFog() : calculateHeightFog();
        litColor = mix(litColor, ufogColor, fogAmount);
    }

    litColor += emissive;

    FragColor = vec4(litColor, baseColor.a);
    EmissiveOut = vec4(emissive, 1.0);
}
