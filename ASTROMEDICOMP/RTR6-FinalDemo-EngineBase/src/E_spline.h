#pragma once
#include "vmath.h"

#define E_SPLINE_MAX_POINTS 64
#define E_SPLINE_MAX_COUNT  128

typedef struct
{
    char        name[32];
    vmath::vec3 points[E_SPLINE_MAX_POINTS];
    int         numPoints;
} E_spline;

vmath::vec3 E_spline_evaluate(const E_spline* s, float t);          // raw parametric t, non-uniform speed
vmath::vec3 E_spline_evaluateTangent(const E_spline* s, float t);   // raw parametric t
vmath::vec3 E_spline_getPositionAtT(const E_spline* s, float u);    // arc-length uniform u in [0,1]
vmath::vec3 E_spline_getRotationAtT(const E_spline* s, float u);    // arc-length uniform u in [0,1]
// Returns spline position mapped to [startTime, endTime].
// speed > 1 completes early and holds; speed < 1 completes late (clamps at t=1 when reached).
// Before startTime returns t=0; after completion returns t=1.
vmath::vec3 E_spline_getPositionBetweenTime(const E_spline* s, float startTime, float endTime, float speed);
vmath::vec3 E_spline_getRotationBetweenTime(const E_spline* s, float startTime, float endTime, float speed);

// Moves the camera along the spline mapped to [startTime, endTime]. No-op outside the window.
void E_spline_followBetweenTime(const E_spline* s, float startTime, float endTime);

// Returns current looping position/rotation on the spline. speed = traversals per second.
vmath::vec3 E_spline_getPositionLooped(const E_spline* s, float speed);
vmath::vec3 E_spline_getRotationLooped(const E_spline* s, float speed);

void E_spline_renderer_init(void);
void E_spline_renderAll(const E_spline* splines, int count);
