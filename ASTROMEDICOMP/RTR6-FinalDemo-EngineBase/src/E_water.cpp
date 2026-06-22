#include "E_water.h"
#include "E_uniformCache.h"
#include <GL/glew.h>

static GLuint s_prog           = 0;
static float  s_scale          = 3.198f;
static float  s_amplitudeScale = 0.083f;
static float  s_frequencyScale = 1.075f;
static float  s_speedScale     = 1.968f;
static float  s_steepnessScale = 0.951f;

static void applyParams(void)
{
    glProgramUniform1f(s_prog, E_getUniformLocation(s_prog, "uWaveScale"),          s_scale);
    glProgramUniform1f(s_prog, E_getUniformLocation(s_prog, "uWaveAmplitudeScale"), s_amplitudeScale);
    glProgramUniform1f(s_prog, E_getUniformLocation(s_prog, "uWaveFrequencyScale"), s_frequencyScale);
    glProgramUniform1f(s_prog, E_getUniformLocation(s_prog, "uWaveSpeedScale"),     s_speedScale);
    glProgramUniform1f(s_prog, E_getUniformLocation(s_prog, "uWaveSteepnessScale"), s_steepnessScale);
}

void E_water_setProgram(GLuint prog)
{
    s_prog = prog;
    applyParams();
}

