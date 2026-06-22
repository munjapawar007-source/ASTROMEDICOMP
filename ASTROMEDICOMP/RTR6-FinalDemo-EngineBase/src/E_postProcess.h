#pragma once

void E_postProcess_setFXAA      (int enabled);
void E_postProcess_setSaturation(float saturation);
void E_postProcess_setBlur      (float strength);
void E_postProcess_setVignette  (float strength, float radius);
void E_postProcess_setDOF       (int enabled, float focalDistance, float focalRange, float maxBlur);
void E_postProcess_setGodRays   (int enabled, float lightX, float lightY, int numSamples,
                                  float exposure, float decay, float density, float weight);
void E_postProcess_setBloom     (int enabled, float strength, float radius);
void E_postProcess_setBarrel    (int enabled, float k1, float k2);
void E_postProcess_setGamma     (float gamma);
void E_postProcess_setCCTV      (int enabled, float wobble, float scanline, float grain, float desaturate);
void E_postProcess_setCCTVGlitch    (int enabled, float amount);
void E_postProcess_triggerCCTVGlitch();

typedef enum {
    E_TRANSITION_START, // black -> normal  (fade in)
    E_TRANSITION_MID,   // normal -> black -> normal  (scene cut)
    E_TRANSITION_END,   // normal -> black  (fade out)
} E_TransitionMode;

void E_postProcess_setBlink       (float b1, float b2, float k, float h, float a);
void E_postProcess_setBlinkOpen   (float open);   // 0 = closed, 1 = open
void E_postProcess_disableBlink   ();             // turns the shader off entirely

void E_postProcess_setFade               (float fade);
void E_postProcess_setFadeColor          (float r, float g, float b);
void E_postProcess_transitionBetweenTime (float t0, float t1, E_TransitionMode mode);

#include <GL/glew.h>
void E_postProcess_setOverlay      (int enabled, GLuint texture);
void E_postProcess_setDissolve     (float amount);
void E_postProcess_setDissolveNoise(GLuint noiseTexture);
void E_postProcess_bindOverlay     ();


#include "vmath.h"
void E_postProcess_applyDOF(vmath::vec3 focusWorld, float focalRange, float maxBlur);
