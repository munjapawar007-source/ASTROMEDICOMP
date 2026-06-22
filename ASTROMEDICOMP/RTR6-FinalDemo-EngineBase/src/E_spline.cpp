#include "E_spline.h"
#include "E_engineFlags.h"
#include "E_shader.h"
#include "E_camera.h"
#include "E_utils.h"
#include "E_uniformCache.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

static vmath::vec3 catmullRomInterp(
    vmath::vec3 p0, vmath::vec3 p1, vmath::vec3 p2, vmath::vec3 p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;
    vmath::vec3 r;
    for (int i = 0; i < 3; i++)
        r[i] = 0.5f * ((2.0f * p1[i])
             + (-p0[i] + p2[i]) * t
             + (2.0f*p0[i] - 5.0f*p1[i] + 4.0f*p2[i] - p3[i]) * t2
             + (-p0[i] + 3.0f*p1[i] - 3.0f*p2[i] + p3[i]) * t3);
    return r;
}

static void segmentFromT(const E_spline* s, float t, int* outSeg, float* outLocalT)
{
    int segments = s->numPoints - 3;
    float ft = t * (float)segments;
    int seg = (int)ft;
    if (seg >= segments) { seg = segments - 1; ft = (float)segments; }
    *outSeg    = seg;
    *outLocalT = ft - (float)seg;
}

vmath::vec3 E_spline_evaluate(const E_spline* s, float t)
{
    if (s->numPoints < 4) return vmath::vec3(0.0f, 0.0f, 0.0f);
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    int seg; float lt;
    segmentFromT(s, t, &seg, &lt);

    return catmullRomInterp(
        s->points[seg], s->points[seg+1], s->points[seg+2], s->points[seg+3], lt);
}

vmath::vec3 E_spline_evaluateTangent(const E_spline* s, float t)
{
    const float eps = 0.001f;
    float t0 = t - eps, t1 = t + eps;
    if (t0 < 0.0f) t0 = 0.0f;
    if (t1 > 1.0f) t1 = 1.0f;
    vmath::vec3 a = E_spline_evaluate(s, t0);
    vmath::vec3 b = E_spline_evaluate(s, t1);
    return b - a;
}

static vmath::vec3 evaluateUniform(const E_spline* s, float u)
{
    const int STEPS = 128;
    float arcLen[STEPS + 1];
    arcLen[0] = 0.0f;
    vmath::vec3 prev = E_spline_evaluate(s, 0.0f);
    for (int i = 1; i <= STEPS; i++)
    {
        float t = (float)i / (float)STEPS;
        vmath::vec3 p = E_spline_evaluate(s, t);
        vmath::vec3 d = p - prev;
        arcLen[i] = arcLen[i-1] + sqrtf(d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
        prev = p;
    }
    float total = arcLen[STEPS];
    if (total < 0.00001f) return E_spline_evaluate(s, u);
    float target = u * total;
    for (int i = 1; i <= STEPS; i++)
    {
        if (arcLen[i] >= target)
        {
            float frac = (target - arcLen[i-1]) / (arcLen[i] - arcLen[i-1]);
            float t    = ((float)(i - 1) + frac) / (float)STEPS;
            return E_spline_evaluate(s, t);
        }
    }
    return E_spline_evaluate(s, 1.0f);
}

static float computeT_betweenTime(float startTime, float endTime, float speed)
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();
    if (now <= startTime) return 0.0f;
    float t = (now - startTime) / (endTime - startTime) * speed;
    if (t >= 1.0f) return 1.0f;
    return t * t * (3.0f - 2.0f * t); // smoothstep ease in-out
}

static float computeT_looped(float speed)
{
    static float s_frozenAt = -1.0f;
    float now = (float)E_Utils_getElapsedTimeInSeconds();
    if (!E_FLAG_ANIMATIONS)
    {
        if (s_frozenAt < 0.0f) s_frozenAt = now;
        now = s_frozenAt;
    }
    else s_frozenAt = -1.0f;
    return fmodf(now * speed, 1.0f);
}

static vmath::vec3 evaluateUniformTangent(const E_spline* s, float u)
{
    const float eps = 0.005f;
    vmath::vec3 a = evaluateUniform(s, u - eps < 0.0f ? 0.0f : u - eps);
    vmath::vec3 b = evaluateUniform(s, u + eps > 1.0f ? 1.0f : u + eps);
    return b - a;
}

static vmath::vec3 tangentToRotation(vmath::vec3 fwd)
{
    float yaw   = atan2f( fwd[0], fwd[2]) * (180.0f / 3.14159265f);
    float pitch = atan2f(-fwd[1], sqrtf(fwd[0]*fwd[0] + fwd[2]*fwd[2])) * (180.0f / 3.14159265f);
    return vmath::vec3(pitch, yaw, 0.0f);
}

vmath::vec3 E_spline_getPositionAtT(const E_spline* s, float u)
{
    return evaluateUniform(s, u);
}

vmath::vec3 E_spline_getRotationAtT(const E_spline* s, float u)
{
    return tangentToRotation(evaluateUniformTangent(s, u));
}

vmath::vec3 E_spline_getPositionBetweenTime(const E_spline* s, float startTime, float endTime, float speed)
{
    float t = computeT_betweenTime(startTime, endTime, speed);
    return evaluateUniform(s, t);
}

vmath::vec3 E_spline_getRotationBetweenTime(const E_spline* s, float startTime, float endTime, float speed)
{
    float t = computeT_betweenTime(startTime, endTime, speed);
    return tangentToRotation(evaluateUniformTangent(s, t));
}

void E_spline_followBetweenTime(const E_spline* s, float startTime, float endTime)
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();
    if (now < startTime || now > endTime) return;
    float t   = (now - startTime) / (endTime - startTime);
    vmath::vec3 pos = E_spline_evaluate(s, t);
    E_camera_setPosition(pos[0], pos[1], pos[2]);
}

vmath::vec3 E_spline_getPositionLooped(const E_spline* s, float speed)
{
    return evaluateUniform(s, computeT_looped(speed));
}

vmath::vec3 E_spline_getRotationLooped(const E_spline* s, float speed)
{
    float u = computeT_looped(speed);
    return tangentToRotation(evaluateUniformTangent(s, u));
}

// spline renderer

#define SPLINE_STEPS     64
#define SPLINE_MAX_VERTS (E_SPLINE_MAX_COUNT * SPLINE_STEPS * 2)

static GLuint s_prog = 0;
static GLuint s_vao  = 0;
static GLuint s_vbo  = 0;
static float  s_verts[SPLINE_MAX_VERTS * 3];

static const char* s_vs =
    "#version 460 core\n"
    "layout(location = 0) in vec3 aPosition;\n"
    "uniform mat4 uView;\n"
    "uniform mat4 uProjection;\n"
    "void main(void) {\n"
    "    gl_Position = uProjection * uView * vec4(aPosition, 1.0);\n"
    "}\n";

static const char* s_fs =
    "#version 460 core\n"
    "uniform vec4 uColor;\n"
    "out vec4 fragColor;\n"
    "void main(void) {\n"
    "    fragColor = uColor;\n"
    "}\n";

void E_spline_renderer_init(void)
{
    GLuint vs = E_compileShader(s_vs, GL_VERTEX_SHADER);
    GLuint fs = E_compileShader(s_fs, GL_FRAGMENT_SHADER);
    s_prog = E_createProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenVertexArrays(1, &s_vao);
    glGenBuffers(1, &s_vbo);

    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_verts), NULL, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void E_spline_renderAll(const E_spline* splines, int count)
{
    if (!s_prog || count == 0) return;

    glUseProgram(s_prog);
    glUniformMatrix4fv(E_getUniformLocation(s_prog, "uView"),       1, GL_FALSE, engineCamera.viewMatrix);
    glUniformMatrix4fv(E_getUniformLocation(s_prog, "uProjection"), 1, GL_FALSE, engineCamera.projectionMatrix);
    glBindVertexArray(s_vao);
    glBindBuffer(GL_ARRAY_BUFFER, s_vbo);

    int lineVtx = 0;
    for (int i = 0; i < count; i++)
    {
        const E_spline* s = &splines[i];
        if (s->numPoints < 4) continue;
        for (int step = 0; step < SPLINE_STEPS; step++)
        {
            float t0 = (float)step       / (float)SPLINE_STEPS;
            float t1 = (float)(step + 1) / (float)SPLINE_STEPS;
            vmath::vec3 p0 = E_spline_evaluate(s, t0);
            vmath::vec3 p1 = E_spline_evaluate(s, t1);
            s_verts[lineVtx*3+0] = p0[0]; s_verts[lineVtx*3+1] = p0[1]; s_verts[lineVtx*3+2] = p0[2]; lineVtx++;
            s_verts[lineVtx*3+0] = p1[0]; s_verts[lineVtx*3+1] = p1[1]; s_verts[lineVtx*3+2] = p1[2]; lineVtx++;
        }
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, lineVtx * 3 * sizeof(float), s_verts);
    glUniform4f(E_getUniformLocation(s_prog, "uColor"), 1.0f, 0.8f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 0, lineVtx);

    int ptVtx = 0;
    for (int i = 0; i < count; i++)
    {
        const E_spline* s = &splines[i];
        for (int pi = 0; pi < s->numPoints; pi++)
        {
            s_verts[ptVtx*3+0] = s->points[pi][0];
            s_verts[ptVtx*3+1] = s->points[pi][1];
            s_verts[ptVtx*3+2] = s->points[pi][2];
            ptVtx++;
        }
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, ptVtx * 3 * sizeof(float), s_verts);
    glUniform4f(E_getUniformLocation(s_prog, "uColor"), 1.0f, 0.3f, 0.1f, 1.0f);
    glPointSize(8.0f);
    glDrawArrays(GL_POINTS, 0, ptVtx);
    glPointSize(1.0f);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}
