#pragma once

// Set to 0 to disable entire engine systems at runtime.
// Defined in DEMO.cpp; checked in core engine code.
extern int E_FLAG_LIGHTING;     // 0 = ambient only, no spot/point/directional
extern int E_FLAG_SHADOWS;      // 0 = skip all shadow passes
extern int E_FLAG_POSTPROCESS;  // 0 = passthrough (raw scene texture, no effects)
extern int E_FLAG_ANIMATIONS;   // 0 = freeze all animators + object spline followers
extern int E_FLAG_WIREFRAME;    // 1 = GL_LINE polygon mode, gizmos/decals suppressed
