#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uTime;
uniform float uWaveScale = 1.0;
uniform float uWaveAmplitudeScale = 1.0;
uniform float uWaveFrequencyScale = 1.0;
uniform float uWaveSpeedScale = 1.0;
uniform float uWaveSteepnessScale = 1.0;

out vec3 out_Color;
out vec2 out_TexCoord;
out vec3 out_Normal;
out vec3 out_worldPos;
out vec3 out_vertexInCameraSpace;
out float out_worldHeight;

const float PI = 3.14159265359;

vec3 gerstner(vec2 xz, vec2 dir, float A, float wavelength, float Q, float speed, inout vec3 normal)
{
    float k = 2.0 * PI / wavelength;
    float w = speed * k;
    float f = k * dot(dir, xz) + w * uTime;
    float sinF = sin(f), cosF = cos(f);

    normal.x += -dir.x * k * A * cosF;
    normal.y += -Q * k * A * sinF;
    normal.z += -dir.y * k * A * cosF;

    return vec3(Q * A * dir.x * cosF, A * sinF, Q * A * dir.y * cosF);
}

void main(void)
{
    vec3 worldPos = (uModel * vec4(aPosition, 1.0)).xyz;
    vec2 xz = worldPos.xz * uWaveScale;

    vec3 disp = vec3(0.0);
    vec3 normal = vec3(0.0, 1.0, 0.0);

    disp += gerstner(xz, normalize(vec2(1.0, 0.6)),   0.35 * uWaveAmplitudeScale, 10.0 / uWaveFrequencyScale, 0.60 * uWaveSteepnessScale, 1.2 * uWaveSpeedScale, normal);
    disp += gerstner(xz, normalize(vec2(-0.7, 1.0)),  0.20 * uWaveAmplitudeScale,  6.0 / uWaveFrequencyScale, 0.50 * uWaveSteepnessScale, 1.5 * uWaveSpeedScale, normal);
    disp += gerstner(xz, normalize(vec2(0.4, -0.9)),  0.12 * uWaveAmplitudeScale,  4.0 / uWaveFrequencyScale, 0.40 * uWaveSteepnessScale, 2.0 * uWaveSpeedScale, normal);
    disp += gerstner(xz, normalize(vec2(-0.5, -0.5)), 0.18 * uWaveAmplitudeScale,  7.0 / uWaveFrequencyScale, 0.45 * uWaveSteepnessScale, 1.3 * uWaveSpeedScale, normal);

    vec3 displaced = worldPos + disp;

    out_worldPos = displaced;
    out_worldHeight = displaced.y;
    out_vertexInCameraSpace = (uView * vec4(displaced, 1.0)).xyz;
    out_Normal = normalize(normal);
    out_TexCoord = aTexCoord + vec2(0.02, 0.01) * uTime;
    out_Color = aColor;

    gl_Position = uProjection * uView * vec4(displaced, 1.0);
}
