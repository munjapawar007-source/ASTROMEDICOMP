#version 460 core

in  vec2 out_TexCoord;
out vec4 FragColor;

uniform sampler2D uTexture;
uniform sampler2D uDepthTexture;
uniform sampler2D uEmissiveTex;
uniform sampler2D uGodRayTex;
uniform int uBloomEnabled;
uniform float uBloomStrength;
uniform float uBloomRadius;
uniform int uOverlayEnabled;
uniform sampler2D uOverlayTex;
uniform float uDissolveAmount;
uniform sampler2D uNoiseTex;
uniform float uSaturation;
uniform float uVignetteStrength;
uniform float uVignetteRadius;
uniform float uBlurStrength;
uniform float uFade;
uniform vec3 uFadeColor;

uniform float uTime;

uniform float uGamma;

uniform int uBarrelEnabled;
uniform float uBarrelK1;
uniform float uBarrelK2;

uniform int uCCTVEnabled;
uniform float uCCTVWobble;
uniform float uCCTVScanline;
uniform float uCCTVGrain;
uniform float uCCTVDesaturate;

uniform int uCCTVGlitchEnabled;
uniform float uCCTVGlitchAmount;

uniform int uPassthrough;
uniform int uBlinkEnabled;
uniform float uBlinkB1;
uniform float uBlinkB2;
uniform float uBlinkK;
uniform float uBlinkH;
uniform float uBlinkA;

uniform int uDofEnabled;
uniform float uDofFocalDistance;
uniform float uDofFocalRange;
uniform float uDofMaxBlur;
uniform float uZNear;
uniform float uZFar;

vec3 bloomBlur(sampler2D tex, vec2 uv, float radius)
{
    vec2 ts = radius / vec2(textureSize(tex, 0));
    vec3 acc = vec3(0.0);
    float total = 0.0;
    const int N = 32;
    const float goldenAngle = 2.39996323;
    for (int i = 0; i < N; i++)
    {
        float t = (float(i) + 0.5) / float(N);
        float r = sqrt(t);
        float angle = float(i) * goldenAngle;
        float w = 1.0 - t;
        acc += texture(tex, uv + vec2(cos(angle), sin(angle)) * r * ts).rgb * w;
        total += w;
    }
    return acc / total;
}

uniform int uFXAAEnabled;

vec3 fxaa(sampler2D tex, vec2 uv)
{
    vec2 texelSize = 1.0 / vec2(textureSize(tex, 0));
    const vec3 luma = vec3(0.299, 0.587, 0.114);

    vec3 rgbNW = texture(tex, uv + vec2(-1.0, -1.0) * texelSize).rgb;
    vec3 rgbNE = texture(tex, uv + vec2( 1.0, -1.0) * texelSize).rgb;
    vec3 rgbSW = texture(tex, uv + vec2(-1.0,  1.0) * texelSize).rgb;
    vec3 rgbSE = texture(tex, uv + vec2( 1.0,  1.0) * texelSize).rgb;
    vec3 rgbM  = texture(tex, uv).rgb;

    float lumaNW = dot(rgbNW, luma);
    float lumaNE = dot(rgbNE, luma);
    float lumaSW = dot(rgbSW, luma);
    float lumaSE = dot(rgbSE, luma);
    float lumaM  = dot(rgbM,  luma);

    float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
    float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

    vec2 dir;
    dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
    dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

    float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) * 0.03125, 0.0078125);
    float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
    dir = clamp(dir * rcpDirMin, vec2(-8.0), vec2(8.0)) * texelSize;

    vec3 rgbA = 0.5 * (
        texture(tex, uv + dir * (1.0/3.0 - 0.5)).rgb +
        texture(tex, uv + dir * (2.0/3.0 - 0.5)).rgb
    );
    vec3 rgbB = rgbA * 0.5 + 0.25 * (
        texture(tex, uv + dir * -0.5).rgb +
        texture(tex, uv + dir *  0.5).rgb
    );

    float lumaB = dot(rgbB, luma);
    return (lumaB < lumaMin || lumaB > lumaMax) ? rgbA : rgbB;
}

uniform int uGodRaysEnabled;
uniform vec2 uGR_LightPos;
uniform int uGR_numSamples;
uniform float uGR_exposure;
uniform float uGR_decay;
uniform float uGR_density;
uniform float uGR_weight;

vec3 computeGodRays(vec2 uv)
{
    vec2 stepUV = vec2(0.0, -(uGR_density / float(uGR_numSamples)));

    vec3 godColor = vec3(0.0);
    float illumination = 1.0;

    for (int i = 0; i < uGR_numSamples; i++)
    {
        uv -= stepUV;
        vec3 sampleColor = texture(uGodRayTex, uv).rgb;
        godColor += sampleColor * illumination * uGR_weight;
        illumination *= uGR_decay;
    }

    return godColor * uGR_exposure;
}

float cctvNoise(vec2 p) { return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453); }

vec3 applyGlitch(vec3 color, vec2 uv, float amount)
{
    float noise = cctvNoise(uv * vec2(800.0, 600.0) + uTime * 60.0);
    return clamp(mix(color, vec3(noise), amount), 0.0, 1.0);
}

void main(void)
{
    if (uPassthrough != 0) { FragColor = texture(uTexture, out_TexCoord); return; }

    vec2 sampleUV = out_TexCoord;
    if (uBarrelEnabled != 0)
    {
        vec2 p = sampleUV - 0.5;
        float r2 = dot(p, p);
        p = p * (1.0 + uBarrelK1 * r2 + uBarrelK2 * r2 * r2);
        float zoom = 1.0 / (1.0 + uBarrelK1 * 0.5 + uBarrelK2 * 0.25);
        p = p * zoom;
        sampleUV = p + 0.5;
    }
    if (uCCTVEnabled != 0)
        sampleUV.x += sin(uTime * 1.5 + sampleUV.y * 30.0) * uCCTVWobble;

    vec3 color;

    if (uDofEnabled == 1)
    {
        float rawDepth = texture(uDepthTexture, sampleUV).r;
        float depth = (2.0 * uZNear * uZFar) /
                      (uZFar + uZNear - (rawDepth * 2.0 - 1.0) * (uZFar - uZNear));
        float coc = clamp(abs(depth - uDofFocalDistance) / uDofFocalRange, 0.0, 1.0);
        float radius = coc * uDofMaxBlur / float(textureSize(uTexture, 0).x);

        float pixelNoise = fract(sin(dot(sampleUV, vec2(12.9898, 78.233))) * 43758.5453);
        float rotOffset = pixelNoise * 6.28318;

        vec3 acc = vec3(0.0);
        const int N = 64;
        const float goldenAngle = 2.39996323;
        for (int i = 0; i < N; i++)
        {
            float t = (float(i) + 0.5) / float(N);
            float r = sqrt(t);
            float a = float(i) * goldenAngle + rotOffset;
            acc += texture(uTexture, sampleUV + vec2(cos(a), sin(a)) * r * radius).rgb;
        }
        color = mix(texture(uTexture, sampleUV).rgb, acc / float(N), coc);
    }
    else if (uFXAAEnabled != 0)
    {
        color = fxaa(uTexture, sampleUV);
    }
    else
    {
        vec2 offset = (1.0 / textureSize(uTexture, 0)) * uBlurStrength;
        vec3 blurred = vec3(0.0);
        blurred += texture(uTexture, sampleUV + offset * vec2(-1.0,  1.0)).rgb * 0.0625;
        blurred += texture(uTexture, sampleUV + offset * vec2( 0.0,  1.0)).rgb * 0.125;
        blurred += texture(uTexture, sampleUV + offset * vec2( 1.0,  1.0)).rgb * 0.0625;
        blurred += texture(uTexture, sampleUV + offset * vec2(-1.0,  0.0)).rgb * 0.125;
        blurred += texture(uTexture, sampleUV).rgb * 0.25;
        blurred += texture(uTexture, sampleUV + offset * vec2( 1.0,  0.0)).rgb * 0.125;
        blurred += texture(uTexture, sampleUV + offset * vec2(-1.0, -1.0)).rgb * 0.0625;
        blurred += texture(uTexture, sampleUV + offset * vec2( 0.0, -1.0)).rgb * 0.125;
        blurred += texture(uTexture, sampleUV + offset * vec2( 1.0, -1.0)).rgb * 0.0625;
        color = blurred;
    }

    if (uCCTVEnabled != 0)
    {
        float scan = sin(sampleUV.y * 800.0);
        color *= (1.0 - uCCTVScanline) + uCCTVScanline * (0.9 + 0.1 * scan);
        float flicker = cctvNoise(vec2(uTime * 2.0, 0.0));
        color *= 0.96 + flicker * 0.04;
        float grain = cctvNoise(sampleUV * vec2(800.0, 600.0) + uTime * 10.0);
        color += (grain - 0.5) * uCCTVGrain;
        float gray = dot(color, vec3(0.299, 0.587, 0.114));
        color = mix(color, vec3(gray), uCCTVDesaturate);
    }

    if (uCCTVGlitchEnabled != 0)
        color = applyGlitch(color, sampleUV, uCCTVGlitchAmount);

    float luma = dot(color, vec3(0.299, 0.587, 0.114));
    color = mix(vec3(luma), color, uSaturation);

    vec2 uv = out_TexCoord - 0.5;
    float dist = length(uv) / 0.7071;
    float vignette = mix(1.0, dist, uVignetteRadius) * uVignetteStrength;
    color *= 1.0 - clamp(vignette, 0.0, 1.0);

    if (uGodRaysEnabled != 0)
        color += computeGodRays(out_TexCoord);

    if (uBloomEnabled != 0)
        color += bloomBlur(uEmissiveTex, out_TexCoord, uBloomRadius) * uBloomStrength;

    if (uBlinkEnabled != 0)
    {
        float ex = out_TexCoord.x;
        float ey = out_TexCoord.y;
        float dx = ex - uBlinkH;
        float y_upper = uBlinkK + uBlinkB1 - (dx * dx) / uBlinkA;
        float y_lower = uBlinkK + uBlinkB2 + (dx * dx) / uBlinkA;
        float e = 0.003;
        float belowUpper = 1.0 - smoothstep(y_upper - e, y_upper + e, ey);
        float aboveLower = smoothstep(y_lower - e, y_lower + e, ey);
        float eyeMask = belowUpper * aboveLower;
        color = mix(vec3(0.0), color, eyeMask);
    }

    if (uOverlayEnabled == 1)
    {
        vec2 overlayUV = vec2(out_TexCoord.x, 1.0 - out_TexCoord.y);
        vec4 ov = texture(uOverlayTex, overlayUV);

        if (uDissolveAmount > 0.0)
        {
            float n1 = texture(uNoiseTex, out_TexCoord * 2.5).r;
            float n2 = texture(uNoiseTex, out_TexCoord * 5.5 + vec2(0.37, 0.61)).r * 0.4;
            float noise = (n1 + n2) / 1.4;
            float covered = step(noise, uDissolveAmount);
            float edgeT = smoothstep(uDissolveAmount - 0.12, uDissolveAmount, noise);
            vec3 bloodFill = vec3(0.30, 0.00, 0.00);
            vec3 bloodEdge = vec3(0.92, 0.04, 0.00);
            vec3 blood = mix(bloodFill, bloodEdge, edgeT);
            ov.rgb = mix(ov.rgb, blood, covered);
            ov.a = max(ov.a, covered);
        }

        color = mix(color, ov.rgb, ov.a);
    }

    color = mix(uFadeColor, color, uFade);

    color = (color * (2.51 * color + 0.03)) / (color * (2.43 * color + 0.59) + 0.14);
    color = clamp(color, 0.0, 1.0);

    color = pow(color, vec3(1.0 / uGamma));

    FragColor = vec4(color, 1.0);
}
