#version 460 core

in vec3 out_Color;
in vec2 out_TexCoord;
in vec3 out_Normal;
in vec3 out_worldPos;
in vec3 out_vertexInCameraSpace;
in float out_worldHeight;

uniform float uTime;
uniform vec4 uBaseColor;
uniform vec3 uCameraPos;
uniform sampler2D uAlbedo;
uniform int uHasTexture;

uniform sampler2DShadow uShadowMap;
uniform mat4 uLightSpaceMatrix;
uniform int uShadowEnabled;

uniform sampler2DShadow uSpotShadowMap;
uniform mat4 uSpotLightSpaceMatrix;
uniform int uSpotShadowEnabled;
uniform int uSpotShadowCasterIdx;

uniform vec3 uDirectionalLightDirection;
uniform vec3 uDirectionalLightColor;
uniform float uDirectionalLightIntensity;

uniform vec3 uAmbientLightColor;
uniform float uAmbientLightIntensity;

struct SpotLight {
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

struct PointLight {
    vec3 position;
    vec3 color;
    float intensity;
    float range;
};
#define MAX_POINT_LIGHTS 48
uniform int uNumPointLights;
uniform PointLight uPointLights[MAX_POINT_LIGHTS];

uniform int ufogEnabled;
uniform vec3 ufogColor;
uniform float ufogDensity;
uniform int uFogType;
uniform float uFogHeight;
uniform float uFogFalloff;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 EmissiveOut;
layout(location = 2) out vec4 GodRayOut;

float depthFog()  { return clamp(1.0 - exp(-ufogDensity * length(out_vertexInCameraSpace)), 0.0, 1.0); }
float heightFog() { return clamp(1.0 - exp(-max(uFogHeight - out_worldHeight, 0.0) * uFogFalloff), 0.0, 1.0); }

float calcShadow(vec3 worldPos, vec3 normal)
{
    vec4 posLS = uLightSpaceMatrix * vec4(worldPos, 1.0);
    vec3 proj = posLS.xyz / posLS.w * 0.5 + 0.5;
    if (proj.z > 1.0) return 1.0;
    float cosTheta = max(dot(normal, normalize(-uDirectionalLightDirection)), 0.0);
    float bias = max(0.0005 * (1.0 - cosTheta), 0.00005);
    float shadow = 0.0;
    vec2 sz = 1.0 / vec2(textureSize(uShadowMap, 0));
    for (int x = -1; x <= 1; x++)
        for (int y = -1; y <= 1; y++)
            shadow += texture(uShadowMap, vec3(proj.xy + vec2(x, y) * sz, proj.z - bias));
    return shadow / 9.0;
}

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

void main(void)
{
    vec3 N = normalize(out_Normal);
    vec3 V = normalize(uCameraPos - out_worldPos);
    vec3 L = normalize(-uDirectionalLightDirection);
    vec3 H = normalize(L + V);

    vec3 shallowColor = vec3(0.05, 0.38, 0.52);
    vec3 deepColor = vec3(0.01, 0.09, 0.22);
    float tilt = 1.0 - max(dot(N, vec3(0.0, 1.0, 0.0)), 0.0);
    vec3 waterColor = mix(shallowColor, deepColor, tilt * 0.8) * uBaseColor.rgb;
    if (uHasTexture == 1)
        waterColor *= texture(uAlbedo, out_TexCoord).rgb;

    float shadow = (uShadowEnabled == 1) ? calcShadow(out_worldPos, N) : 1.0;
    float NdotL = max(dot(N, L), 0.0);
    vec3 ambient = uAmbientLightColor * uAmbientLightIntensity;
    vec3 diffuse = uDirectionalLightColor * uDirectionalLightIntensity * NdotL * shadow;

    float spec = pow(max(dot(N, H), 0.0), 256.0);
    vec3 specular = uDirectionalLightColor * uDirectionalLightIntensity * spec * 3.0 * shadow;

    for (int i = 0; i < uNumPointLights; i++) {
        vec3 toL = uPointLights[i].position - out_worldPos;
        float dist = length(toL);
        vec3 Lp = toL / dist;
        float atten = smoothstep(uPointLights[i].range, 0.0, dist);
        diffuse += uPointLights[i].color * uPointLights[i].intensity * max(dot(N, Lp), 0.0) * atten;
        float sp = pow(max(dot(N, normalize(Lp + V)), 0.0), 256.0);
        specular += uPointLights[i].color * uPointLights[i].intensity * sp * atten;
    }

    for (int i = 0; i < uNumSpotLights; i++) {
        vec3 toL = uSpotLights[i].position - out_worldPos;
        float dist = length(toL);
        vec3 Ls = toL / dist;
        float atten = smoothstep(uSpotLights[i].range, 0.0, dist);
        float theta = dot(Ls, normalize(-uSpotLights[i].direction));
        float eps = uSpotLights[i].cutOff - uSpotLights[i].outerCutOff;
        float cone = clamp((theta - uSpotLights[i].outerCutOff) / eps, 0.0, 1.0);
        float spotShadow = (uSpotShadowEnabled == 1 && i == uSpotShadowCasterIdx)
                           ? calcSpotShadow(out_worldPos, N, uSpotLights[i].direction) : 1.0;
        diffuse += uSpotLights[i].color * uSpotLights[i].intensity * max(dot(N, Ls), 0.0) * cone * atten * spotShadow;
    }

    float fresnel = pow(1.0 - max(dot(N, V), 0.0), 4.0);
    vec3 skyTint = vec3(0.25, 0.45, 0.65);

    vec3 litColor = waterColor * (ambient + diffuse)
                  + specular
                  + fresnel * skyTint * 0.6;

    if (ufogEnabled == 1) {
        float fogAmount = (uFogType == 0) ? depthFog() : heightFog();
        litColor = mix(litColor, ufogColor, fogAmount);
    }

    FragColor = vec4(litColor, uBaseColor.a);
    EmissiveOut = vec4(0.0);
    GodRayOut = vec4(0.0);
}
