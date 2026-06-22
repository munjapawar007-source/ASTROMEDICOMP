#version 460 core

in  vec2 out_TexCoord;
in  vec3 out_worldPos;
in  vec3 out_vertexInCameraSpace;
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 EmissiveOut;
layout(location = 2) out vec4 GodRayOut;

uniform sampler2D uAlbedo;    // sky texture
uniform sampler2D uTexture_1; // cloud noise layer 1
uniform sampler2D uTexture_2; // cloud noise layer 2
uniform float     uTime;

uniform int   ufogEnabled;
uniform vec3  ufogColor;
uniform float ufogDensity;
uniform int   uFogType;
uniform float uFogHeight;
uniform float uFogFalloff;

float calculateDepthFog()
{
    float d = length(out_vertexInCameraSpace);
    return clamp(1.0 - exp(-ufogDensity * d), 0.0, 1.0);
}

float calculateHeightFog()
{
    float h = max(uFogHeight - out_worldPos.y, 0.0);
    return clamp(1.0 - exp(-h * uFogFalloff), 0.0, 1.0);
}

void main(void)
{
    vec2 uv = out_TexCoord;
    uv.x = fract(uv.x);

    vec3 skyColor = texture(uAlbedo, uv).rgb * 0.55;

    // Horizon tint
    float horizonMask = smoothstep(0.0, 0.35, 1.0 - uv.y);
    vec3  horizonTint = vec3(1.0, 0.45, 0.15);
    vec3 topTint     = vec3(0.10, 0.18, 0.34);
    skyColor = mix(horizonTint, skyColor, horizonMask);
    skyColor = mix(skyColor, topTint, smoothstep(0.35, 1.0, 1.0 - uv.y));

    // Wind-scrolled cloud noise from two textures
    vec2 windA = vec2(0.0075, 0.004) * uTime;
    vec2 windB = vec2(-0.005, 0.0025) * uTime;

    float n1 = texture(uTexture_1,  uv + windA        ).r;
    float n2 = texture(uTexture_1, (uv + windB) * 2.5 ).r * 0.5;
    float n3 = texture(uTexture_2,  uv + windB        ).r;
    float n4 = texture(uTexture_2, (uv + windA) * 3.0 ).r * 0.5;

    float cloudNoise = n1 + n2 + (n3 + n4) * 0.35;
    float cloud      = smoothstep(0.55, 0.8, cloudNoise);

    // Fade clouds near horizon
    cloud *= smoothstep(0.05, 0.35, uv.y);

    vec3 cloudShadow = vec3(0.025, 0.045, 0.085);
    vec3 cloudLight  = vec3(0.09, 0.13, 0.19);

    float sun        = smoothstep(0.0, 0.45, uv.y);
    vec3  cloudColor = mix(cloudShadow, cloudLight, cloud * sun);

    // Warm glow near horizon
    cloudColor += vec3(0.8, 0.35, 0.15) * cloud * (1.0 - uv.y) * 0.2;

    vec3 finalColor = mix(skyColor, cloudColor, cloud * 0.6);

    if (ufogEnabled == 1)
    {
        float fogAmount = (uFogType == 0) ? calculateDepthFog() : calculateHeightFog();
        finalColor = mix(finalColor, ufogColor, fogAmount);
    }

    FragColor   = vec4(finalColor, 1.0);
    EmissiveOut = vec4(0.0);
    GodRayOut   = vec4(0.0);
}