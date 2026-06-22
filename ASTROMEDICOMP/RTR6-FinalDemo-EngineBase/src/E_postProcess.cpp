#include "E_postProcess.h"
#include "E_fbo.h"
#include "E_material.h"
#include "E_camera.h"
#include "E_utils.h"
#include "E_uniformCache.h"
#include <GL/glew.h>
#include <stdio.h>
#include <math.h>

static int   s_fxaaEnabled      = 1;
static float s_saturation       = 1.0f;
static float s_blurStrength     = 0.0f;
static float s_vignetteStrength = 0.0f;
static float s_vignetteRadius   = 0.5f;
static int   s_dofEnabled       = 0;
static float s_dofFocalDistance = 10.0f;
static float s_dofFocalRange    = 5.0f;
static float s_dofMaxBlur       = 20.0f;

static int   s_bloomEnabled  = 0;
static float s_bloomStrength = 1.0f;
static float s_bloomRadius   = 3.0f;

static float s_fade         = 1.0f;
static float s_fadeColor[3] = { 0.0f, 0.0f, 0.0f };

static int   s_blinkEnabled = 0;
static float s_blinkB1 = 1.0f;
static float s_blinkB2 = 0.0f;
static float s_blinkK  = 0.0f;
static float s_blinkH  = 0.5f;
static float s_blinkA  = 0.5f;

static int    s_overlayEnabled  = 0;
static GLuint s_overlayTexture  = 0;
static float  s_dissolveAmount  = 0.0f;
static GLuint s_noiseTexture    = 0;

static int   s_barrelEnabled = 0;
static float s_barrelK1      = 0.2f;
static float s_barrelK2      = 0.05f;

static int    s_cctvEnabled      = 0;
static float  s_cctvWobble       = 0.002f;
static float  s_cctvScanline     = 0.10f;
static float  s_cctvGrain        = 0.04f;
static float  s_cctvDesaturate   = 0.25f;
static GLuint s_cctvOverlayTex   = 0;

static float s_gamma             = 1.0f;

static int   s_cctvGlitchEnabled     = 0;
static float s_cctvGlitchAmount      = 0.0f;
static float s_cctvGlitchTriggerTime = -999.0f;
static float s_cctvGlitchDuration    = 0.25f;

static int   s_grEnabled    = 0;
static float s_grLightPos[2] = { 0.5f, 0.5f };
static int   s_grNumSamples = 100;
static float s_grExposure   = 0.3f;
static float s_grDecay      = 0.97f;
static float s_grDensity    = 0.9f;
static float s_grWeight     = 0.5f;

void E_postProcess_setFXAA(int enabled)
{
    s_fxaaEnabled = enabled;
    E_material_setInt(&enginePostProcessingQuad.material, "uFXAAEnabled", s_fxaaEnabled);
}

void E_postProcess_setSaturation(float saturation)
{
    s_saturation = saturation;
    E_material_setFloat(&enginePostProcessingQuad.material, "uSaturation", s_saturation);
}

void E_postProcess_setBlur(float strength)
{
    s_blurStrength = strength;
    E_material_setFloat(&enginePostProcessingQuad.material, "uBlurStrength", s_blurStrength);
}

void E_postProcess_setVignette(float strength, float radius)
{
    s_vignetteStrength = strength;
    s_vignetteRadius   = radius;
    E_material_setFloat(&enginePostProcessingQuad.material, "uVignetteStrength", s_vignetteStrength);
    E_material_setFloat(&enginePostProcessingQuad.material, "uVignetteRadius",   s_vignetteRadius);
}

void E_postProcess_setBlink(float b1, float b2, float k, float h, float a)
{
    s_blinkB1 = b1; s_blinkB2 = b2;
    s_blinkK  = k;  s_blinkH  = h;  s_blinkA = a;
    E_material_setFloat(&enginePostProcessingQuad.material, "uBlinkB1", s_blinkB1);
    E_material_setFloat(&enginePostProcessingQuad.material, "uBlinkB2", s_blinkB2);
    E_material_setFloat(&enginePostProcessingQuad.material, "uBlinkK",  s_blinkK);
    E_material_setFloat(&enginePostProcessingQuad.material, "uBlinkH",  s_blinkH);
    E_material_setFloat(&enginePostProcessingQuad.material, "uBlinkA",  s_blinkA);
}

void E_postProcess_disableBlink()
{
    s_blinkEnabled = 0;
    E_material_setInt(&enginePostProcessingQuad.material, "uBlinkEnabled", 0);
}

void E_postProcess_setBlinkOpen(float open)
{
    if (!s_blinkEnabled)
    {
        s_blinkEnabled = 1;
        E_material_setInt(&enginePostProcessingQuad.material, "uBlinkEnabled", 1);
    }
    E_postProcess_setBlink(
        0.5f + 0.5f * open,   // b1: 0.5 -> 1.0
        0.5f - 0.5f * open,   // b2: 0.5 -> 0.0
        0.0f, 0.5f, 0.5f);
}

void E_postProcess_setDOF(int enabled, float focalDistance, float focalRange, float maxBlur)
{
    s_dofEnabled       = enabled;
    s_dofFocalDistance = focalDistance;
    s_dofFocalRange    = focalRange;
    s_dofMaxBlur       = maxBlur;
    E_material_setInt  (&enginePostProcessingQuad.material, "uDofEnabled",       s_dofEnabled);
    E_material_setFloat(&enginePostProcessingQuad.material, "uDofFocalDistance", s_dofFocalDistance);
    E_material_setFloat(&enginePostProcessingQuad.material, "uDofFocalRange",    s_dofFocalRange);
    E_material_setFloat(&enginePostProcessingQuad.material, "uDofMaxBlur",       s_dofMaxBlur);
}

void E_postProcess_applyDOF(vmath::vec3 focusWorld, float focalRange, float maxBlur)
{
    float dx   = focusWorld[0] - engineCamera.position[0];
    float dy   = focusWorld[1] - engineCamera.position[1];
    float dz   = focusWorld[2] - engineCamera.position[2];
    float dist = sqrtf(dx*dx + dy*dy + dz*dz);
    E_postProcess_setDOF(1, dist, focalRange, maxBlur);
}

void E_postProcess_setGodRays(int enabled, float lightX, float lightY, int numSamples,
                               float exposure, float decay, float density, float weight)
{
    s_grEnabled     = enabled;
    s_grLightPos[0] = lightX;
    s_grLightPos[1] = lightY;
    s_grNumSamples  = numSamples;
    s_grExposure    = exposure;
    s_grDecay       = decay;
    s_grDensity     = density;
    s_grWeight      = weight;

    GLuint prog = enginePostProcessingQuad.material.shaderProgram;
    glProgramUniform1i (prog, E_getUniformLocation(prog, "uGodRaysEnabled"), s_grEnabled);
    glProgramUniform2fv(prog, E_getUniformLocation(prog, "uGR_LightPos"),    1, s_grLightPos);
    glProgramUniform1i (prog, E_getUniformLocation(prog, "uGR_numSamples"),  s_grNumSamples);
    glProgramUniform1f (prog, E_getUniformLocation(prog, "uGR_exposure"),    s_grExposure);
    glProgramUniform1f (prog, E_getUniformLocation(prog, "uGR_decay"),       s_grDecay);
    glProgramUniform1f (prog, E_getUniformLocation(prog, "uGR_density"),     s_grDensity);
    glProgramUniform1f (prog, E_getUniformLocation(prog, "uGR_weight"),      s_grWeight);
}

void E_postProcess_setBloom(int enabled, float strength, float radius)
{
    s_bloomEnabled  = enabled;
    s_bloomStrength = strength;
    s_bloomRadius   = radius;
    E_material_setInt  (&enginePostProcessingQuad.material, "uBloomEnabled",  s_bloomEnabled);
    E_material_setFloat(&enginePostProcessingQuad.material, "uBloomStrength", s_bloomStrength);
    E_material_setFloat(&enginePostProcessingQuad.material, "uBloomRadius",   s_bloomRadius);
}

void E_postProcess_setFade(float fade)
{
    s_fade = fade;
    E_material_setFloat(&enginePostProcessingQuad.material, "uFade", s_fade);
}

void E_postProcess_setFadeColor(float r, float g, float b)
{
    s_fadeColor[0] = r;
    s_fadeColor[1] = g;
    s_fadeColor[2] = b;
    E_material_setVec3(&enginePostProcessingQuad.material, "uFadeColor", r, g, b);
}

void E_postProcess_setOverlay(int enabled, GLuint texture)
{
    s_overlayEnabled = enabled;
    s_overlayTexture = texture;
    E_material_setInt(&enginePostProcessingQuad.material, "uOverlayEnabled", s_overlayEnabled);
    E_material_setInt(&enginePostProcessingQuad.material, "uOverlayTex",     5);
}

void E_postProcess_setDissolve(float amount)
{
    s_dissolveAmount = amount;
    E_material_setFloat(&enginePostProcessingQuad.material, "uDissolveAmount", s_dissolveAmount);
}

void E_postProcess_setDissolveNoise(GLuint noiseTexture)
{
    s_noiseTexture = noiseTexture;
    E_material_setInt(&enginePostProcessingQuad.material, "uNoiseTex", 6);
}

void E_postProcess_bindOverlay()
{
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, s_overlayTexture);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, s_noiseTexture);
    glActiveTexture(GL_TEXTURE0);
}

void E_postProcess_transitionBetweenTime(float t0, float t1, E_TransitionMode mode)
{
    float now  = (float)E_Utils_getElapsedTimeInSeconds();
    float fade = 1.0f;

    if (now < t0 || now > t1) return;

    float tt = (now - t0) / (t1 - t0);
    tt = tt * tt * (3.0f - 2.0f * tt); // smoothstep

    if (mode == E_TRANSITION_START)
    {
        fade = tt; // 0 -> 1
    }
    else if (mode == E_TRANSITION_END)
    {
        fade = 1.0f - tt; // 1 -> 0
    }
    else // E_TRANSITION_MID
    {
        float mid = (t0 + t1) * 0.5f;
        if (now <= mid)
        {
            float t2 = (now - t0) / (mid - t0);
            t2   = t2 * t2 * (3.0f - 2.0f * t2);
            fade = 1.0f - t2; // 1 -> 0
        }
        else
        {
            float t2 = (now - mid) / (t1 - mid);
            t2   = t2 * t2 * (3.0f - 2.0f * t2);
            fade = t2; // 0 -> 1
        }
    }

    E_postProcess_setFade(fade);
}

void E_postProcess_setBarrel(int enabled, float k1, float k2)
{
    s_barrelEnabled = enabled;
    s_barrelK1      = k1;
    s_barrelK2      = k2;
    E_material_setInt  (&enginePostProcessingQuad.material, "uBarrelEnabled", s_barrelEnabled);
    E_material_setFloat(&enginePostProcessingQuad.material, "uBarrelK1",      s_barrelK1);
    E_material_setFloat(&enginePostProcessingQuad.material, "uBarrelK2",      s_barrelK2);
}

void E_postProcess_setGamma(float gamma)
{
    s_gamma = gamma;
    E_material_setFloat(&enginePostProcessingQuad.material, "uGamma", s_gamma);
}

void E_postProcess_setCCTV(int enabled, float wobble, float scanline, float grain, float desaturate)
{
    s_cctvEnabled    = enabled;
    s_cctvWobble     = wobble;
    s_cctvScanline   = scanline;
    s_cctvGrain      = grain;
    s_cctvDesaturate = desaturate;
    E_material_setInt  (&enginePostProcessingQuad.material, "uCCTVEnabled",    s_cctvEnabled);
    E_material_setFloat(&enginePostProcessingQuad.material, "uCCTVWobble",     s_cctvWobble);
    E_material_setFloat(&enginePostProcessingQuad.material, "uCCTVScanline",   s_cctvScanline);
    E_material_setFloat(&enginePostProcessingQuad.material, "uCCTVGrain",      s_cctvGrain);
    E_material_setFloat(&enginePostProcessingQuad.material, "uCCTVDesaturate", s_cctvDesaturate);
    E_material_setInt  (&enginePostProcessingQuad.material, "uCCTVGlitchEnabled", s_cctvGlitchEnabled);
    E_material_setFloat(&enginePostProcessingQuad.material, "uCCTVGlitchAmount",  s_cctvGlitchAmount);

    if (s_cctvEnabled)
    {
        if (!s_cctvOverlayTex)
            s_cctvOverlayTex = E_loadTexture("DEMO_ASSETS/04-BedroomScene/cctvoverlay.png");
        E_postProcess_setOverlay(1, s_cctvOverlayTex);
    }
    else
    {
        E_postProcess_setOverlay(0, 0);
    }
}

void E_postProcess_setCCTVGlitch(int enabled, float amount)
{
    s_cctvGlitchEnabled = enabled;
    s_cctvGlitchAmount  = amount;
    E_material_setInt  (&enginePostProcessingQuad.material, "uCCTVGlitchEnabled", s_cctvGlitchEnabled);
    E_material_setFloat(&enginePostProcessingQuad.material, "uCCTVGlitchAmount",  s_cctvGlitchAmount);
}

void E_postProcess_triggerCCTVGlitch()
{
    s_cctvGlitchTriggerTime = (float)E_Utils_getElapsedTimeInSeconds();
    E_postProcess_setCCTVGlitch(1, 1.0f);
}

