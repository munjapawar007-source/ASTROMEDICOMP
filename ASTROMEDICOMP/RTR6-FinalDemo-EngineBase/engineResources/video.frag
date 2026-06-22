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

uniform int uShadowPass;

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
    int applyAttenuation;
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

uniform int uGodRayEmitter;

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
    if (uShadowPass == 1) return;

    vec2 videoUV = vec2(out_TexCoord.x, 1.0 - out_TexCoord.y);
    vec4 baseColor;
    if (uHasTexture == 1)
        baseColor = texture(uAlbedo, videoUV) * uBaseColor;
    else
        baseColor = vec4(out_Color, 1.0) * uBaseColor;

    vec3 videoColor = texture(uAlbedo, videoUV).rgb;

    FragColor = vec4(videoColor, baseColor.a);
    EmissiveOut = vec4(videoColor * 0.5, 1.0);
    GodRayOut = vec4(0.0);
}
