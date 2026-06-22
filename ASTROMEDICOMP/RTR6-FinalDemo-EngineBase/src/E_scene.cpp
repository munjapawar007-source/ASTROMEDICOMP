#include "E_scene.h"
#include "E_engineFlags.h"
#include "E_camera.h"
#include "E_shader.h"
#include "E_spline.h"
#include "E_anim.h"
#include "E_utils.h"
#include "E_uniformCache.h"
#include <GL/glew.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#define E_SCENE_INITIAL_CAPACITY      256
#define E_LIGHT_INITIAL_CAPACITY       256
#define E_SHADOW_MAP_SIZE              1024
#define E_SPOT_SHADOW_MAP_SIZE         2048

// ---- shadow matrix helpers ----

static void shadow_mat4_mul(const float a[16], const float b[16], float out[16])
{
    float tmp[16];
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++) {
            float s = 0;
            for (int k = 0; k < 4; k++) s += a[k*4+row] * b[col*4+k];
            tmp[col*4+row] = s;
        }
    memcpy(out, tmp, 64);
}

static void shadow_lookAt(const float eye[3], const float center[3], float out[16])
{
    float f[3] = { center[0]-eye[0], center[1]-eye[1], center[2]-eye[2] };
    float flen = sqrtf(f[0]*f[0]+f[1]*f[1]+f[2]*f[2]);
    if (flen < 0.0001f) flen = 1.0f;
    f[0]/=flen; f[1]/=flen; f[2]/=flen;

    // world up  avoid parallel with f
    float up[3] = { 0.0f, 1.0f, 0.0f };
    if (fabsf(f[1]) > 0.99f) { up[0]=1.0f; up[1]=0.0f; }

    float r[3] = { f[1]*up[2]-f[2]*up[1], f[2]*up[0]-f[0]*up[2], f[0]*up[1]-f[1]*up[0] };
    float rlen = sqrtf(r[0]*r[0]+r[1]*r[1]+r[2]*r[2]);
    r[0]/=rlen; r[1]/=rlen; r[2]/=rlen;

    float u[3] = { r[1]*f[2]-r[2]*f[1], r[2]*f[0]-r[0]*f[2], r[0]*f[1]-r[1]*f[0] };

    out[0]=r[0]; out[1]=u[0]; out[2]=-f[0]; out[3]=0;
    out[4]=r[1]; out[5]=u[1]; out[6]=-f[1]; out[7]=0;
    out[8]=r[2]; out[9]=u[2]; out[10]=-f[2]; out[11]=0;
    out[12]=-(r[0]*eye[0]+r[1]*eye[1]+r[2]*eye[2]);
    out[13]=-(u[0]*eye[0]+u[1]*eye[1]+u[2]*eye[2]);
    out[14]= (f[0]*eye[0]+f[1]*eye[1]+f[2]*eye[2]);
    out[15]=1;
}

static void shadow_ortho(float l, float r, float b, float t, float n, float f, float out[16])
{
    memset(out, 0, 64);
    out[0]  =  2.0f/(r-l);
    out[5]  =  2.0f/(t-b);
    out[10] = -2.0f/(f-n);
    out[12] = -(r+l)/(r-l);
    out[13] = -(t+b)/(t-b);
    out[14] = -(f+n)/(f-n);
    out[15] =  1.0f;
}

static void computeLightSpaceMatrix(const E_scene* scene, float out[16])
{
    const float* dir = scene->directionalLight.direction;
    float len = sqrtf(dir[0]*dir[0]+dir[1]*dir[1]+dir[2]*dir[2]);
    if (len < 0.0001f) len = 1.0f;

    // Center frustum on camera position so resolution stays near what the camera sees
    float center[3] = {
        engineCamera.position[0] + scene->shadowCenter[0],
        engineCamera.position[1] + scene->shadowCenter[1],
        engineCamera.position[2] + scene->shadowCenter[2],
    };

    float dist = scene->shadowRadius * 2.0f;
    float eye[3] = {
        center[0] - dir[0]/len * dist,
        center[1] - dir[1]/len * dist,
        center[2] - dir[2]/len * dist,
    };

    float view[16], proj[16];
    shadow_lookAt(eye, center, view);

    float r = scene->shadowRadius;
    shadow_ortho(-r, r, -r, r, 0.1f, dist * 2.0f, proj);

    shadow_mat4_mul(proj, view, out);
}

static vmath::mat4 computeSpotLightSpaceMatrix(const E_spotLight* sl)
{
    vmath::vec3 pos(sl->position[0], sl->position[1], sl->position[2]);
    vmath::vec3 dir(sl->direction[0], sl->direction[1], sl->direction[2]);
    dir = vmath::normalize(dir);
    vmath::vec3 target = pos + dir * sl->range;

    vmath::vec3 up(0.0f, 1.0f, 0.0f);
    if (fabsf(dir[1]) > 0.99f) up = vmath::vec3(1.0f, 0.0f, 0.0f);

    float outerCos = sl->outerCutOff < -1.0f ? -1.0f : sl->outerCutOff > 1.0f ? 1.0f : sl->outerCutOff;
    float fovDeg   = 2.0f * acosf(outerCos) * 180.0f / 3.14159265f;

    vmath::mat4 view = vmath::lookat(pos, target, up);
    vmath::mat4 proj = vmath::perspective(fovDeg, 1.0f, 0.1f, sl->range);
    return proj * view;
}

// ---- shadow render helpers ----

static void shadowRenderObject(E_sceneObject* obj, GLuint prog)
{
    if (!obj->active) return;
    if (obj->hasMesh && obj->castShadow && !obj->isSkinned && obj->material.shaderProgram != 0)
    {
        glUniformMatrix4fv(E_getUniformLocation(prog, "uModel"), 1, GL_FALSE, (const float*)obj->worldMatrix);
        glBindVertexArray(obj->mesh.vao);
        if (obj->mesh.indexCount > 0)
            glDrawElements(GL_TRIANGLES, obj->mesh.indexCount, GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, obj->mesh.vertexCount);
        glBindVertexArray(0);
    }
    else if (obj->hasMesh && obj->castShadow && obj->isSkinned && obj->animator && obj->animator->jointCount > 0)
    {
        glUseProgram(skinnedShaderProgramObject);
        GLuint boneBlockIdx = glGetUniformBlockIndex(skinnedShaderProgramObject, "BoneBlock");
        if (boneBlockIdx != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(skinnedShaderProgramObject, boneBlockIdx, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, obj->animator->boneUBO);
        }
        glBindVertexArray(obj->mesh.vao);
        if (obj->mesh.indexCount > 0)
            glDrawElements(GL_TRIANGLES, obj->mesh.indexCount, GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, obj->mesh.vertexCount);
        glBindVertexArray(0);
        glUseProgram(prog);
    }
    for (int i = 0; i < obj->childCount; i++)
        shadowRenderObject(obj->children[i], prog);
}

void E_scene_init(E_scene* scene)
{
    scene->objects  = (E_sceneObject**)malloc(E_SCENE_INITIAL_CAPACITY * sizeof(E_sceneObject*));
    scene->count    = 0;
    scene->capacity = E_SCENE_INITIAL_CAPACITY;

    scene->fog.enabled  = 0;
    scene->fog.color[0] = 0.5f;
    scene->fog.color[1] = 0.5f;
    scene->fog.color[2] = 0.5f;
    scene->fog.density  = 0.01f;
    scene->fog.type     = 0;
    scene->fog.height   = 0.0f;
    scene->fog.falloff  = 0.1f;

    scene->directionalLight.direction[0] = 0.0f;
    scene->directionalLight.direction[1] = -1.0f;
    scene->directionalLight.direction[2] = 0.0f;
    scene->directionalLight.color[0]     = 1.0f;
    scene->directionalLight.color[1]     = 1.0f;
    scene->directionalLight.color[2]     = 1.0f;
    scene->directionalLight.intensity    = 1.0f;

    scene->ambientLight.color[0]  = 1.0f;
    scene->ambientLight.color[1]  = 1.0f;
    scene->ambientLight.color[2]  = 1.0f;
    scene->ambientLight.intensity = 0.2f;

    scene->spotLights         = (E_spotLight*)malloc(E_LIGHT_INITIAL_CAPACITY * sizeof(E_spotLight));
    scene->spotLightCount     = 0;
    scene->spotLightCapacity  = E_LIGHT_INITIAL_CAPACITY;

    scene->pointLights        = (E_pointLight*)malloc(E_LIGHT_INITIAL_CAPACITY * sizeof(E_pointLight));
    scene->pointLightCount    = 0;
    scene->pointLightCapacity = E_LIGHT_INITIAL_CAPACITY;

    scene->splineCount   = 0;
    scene->emitterCount  = 0;
    scene->activeSpline  = 0;
    scene->splineT       = 0.0f;
    scene->splineDir     = 1.0f;
    scene->splineSpeed   = 0.2f;
    scene->splineFollow  = 0;

    // Directional shadow map
    scene->shadowMapSize    = E_SHADOW_MAP_SIZE;
    scene->shadowCenter[0]  = 0.0f;
    scene->shadowCenter[1]  = 0.0f;
    scene->shadowCenter[2]  = 0.0f;
    scene->shadowRadius     = 5.0f;
    scene->shadowEnabled    = 0;

    glGenFramebuffers(1, &scene->shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, scene->shadowFBO);

    glGenTextures(1, &scene->shadowDepthTex);
    glBindTexture(GL_TEXTURE_2D, scene->shadowDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
                 E_SHADOW_MAP_SIZE, E_SHADOW_MAP_SIZE, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,     GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,     GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,         GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,         GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,   GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,   GL_LEQUAL);
    {
        float bc[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bc);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, scene->shadowDepthTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Spot light shadow map
    scene->spotShadowEnabled = 0;

    glGenFramebuffers(1, &scene->spotShadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, scene->spotShadowFBO);

    glGenTextures(1, &scene->spotShadowDepthTex);
    glBindTexture(GL_TEXTURE_2D, scene->spotShadowDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F,
                 E_SPOT_SHADOW_MAP_SIZE, E_SPOT_SHADOW_MAP_SIZE, 0,
                 GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,     GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,     GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,         GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,         GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,   GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,   GL_LEQUAL);
    {
        float bc[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bc);
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, scene->spotShadowDepthTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void E_scene_add(E_scene* scene, E_sceneObject* obj)
{
    if (scene->count == scene->capacity)
    {
        scene->capacity *= 2;
        scene->objects   = (E_sceneObject**)realloc(scene->objects, scene->capacity * sizeof(E_sceneObject*));
    }
    scene->objects[scene->count++] = obj;
}

void E_scene_remove(E_scene* scene, int index)
{
    if (index < 0 || index >= scene->count)
    {
        printf("[Scene] Remove index %d out of range\n", index);
        return;
    }
    for (int i = index; i < scene->count - 1; i++)
        scene->objects[i] = scene->objects[i + 1];
    scene->count--;
}

void E_scene_addOwned(E_scene* scene, E_sceneObject* obj)
{
    E_scene_add(scene, obj);
}



// ---- transparent node collection for sorted pass ----

typedef struct { E_sceneObject* obj; float distSq; } TransNode;

static int cmpTranspNodes(const void* a, const void* b)
{
    float da = ((const TransNode*)a)->distSq;
    float db = ((const TransNode*)b)->distSq;
    return (da < db) - (da > db); // descending: furthest first
}

static void collectTransparent(E_sceneObject* obj, const float camPos[3],
                                TransNode** list, int* count, int* cap)
{
    if (!obj->active) return;
    if (obj->hasMesh && obj->transparent && obj->material.shaderProgram != 0)
    {
        if (*count < *cap)
        {
            const float* wm = (const float*)obj->worldMatrix;
            float dx = wm[12] - camPos[0], dy = wm[13] - camPos[1], dz = wm[14] - camPos[2];
            (*list)[*count].obj    = obj;
            (*list)[*count].distSq = dx*dx + dy*dy + dz*dz;
            (*count)++;
        }
    }
    for (int i = 0; i < obj->childCount; i++)
        collectTransparent(obj->children[i], camPos, list, count, cap);
}

static unsigned int s_renderFrame = 0;

// ---- helpers for distance-based light selection ----

typedef struct { int idx; float distSq; } LightSort;

static int cmpLightSort(const void* a, const void* b)
{
    float da = ((const LightSort*)a)->distSq;
    float db = ((const LightSort*)b)->distSq;
    return (da > db) - (da < db);
}

static float distSq3(const float* a, const float* b)
{
    float dx = a[0]-b[0], dy = a[1]-b[1], dz = a[2]-b[2];
    return dx*dx + dy*dy + dz*dz;
}

void E_scene_applyFog(const E_scene* scene, GLuint prog)
{
    static struct { GLuint prog; unsigned int frame; } s_uploaded[4];
    int i;
    for (i = 0; i < 8; i++)
        if (s_uploaded[i].prog == prog && s_uploaded[i].frame == s_renderFrame)
            return;
    for (i = 0; i < 8; i++)
        if (s_uploaded[i].frame != s_renderFrame) { s_uploaded[i].prog = prog; s_uploaded[i].frame = s_renderFrame; break; }

    const E_fogProperties* fog = &scene->fog;
    glProgramUniform1i(prog, E_getUniformLocation(prog, "ufogEnabled"), fog->enabled);
    glProgramUniform3fv(prog, E_getUniformLocation(prog, "ufogColor"),   1, fog->color);
    glProgramUniform1f (prog, E_getUniformLocation(prog, "ufogDensity"), fog->density);
    glProgramUniform1i (prog, E_getUniformLocation(prog, "uFogType"),    fog->type);
    glProgramUniform1f (prog, E_getUniformLocation(prog, "uFogHeight"),  fog->height);
    glProgramUniform1f (prog, E_getUniformLocation(prog, "uFogFalloff"), fog->falloff);
}

void E_scene_applyLighting(const E_scene* scene, GLuint prog, const float worldPos[3])
{
    // Each shader program only needs lighting uploaded once per frame.
    // All light updates (flicker, position) happen before rendering starts,
    // so every mesh in the same frame sees identical light data.
    static struct { GLuint prog; unsigned int frame; } s_uploaded[4];
    int i;
    for (i = 0; i < 8; i++)
        if (s_uploaded[i].prog == prog && s_uploaded[i].frame == s_renderFrame)
            return;
    for (i = 0; i < 8; i++)
        if (s_uploaded[i].frame != s_renderFrame) { s_uploaded[i].prog = prog; s_uploaded[i].frame = s_renderFrame; break; }

    glProgramUniform1i       (prog, E_getUniformLocation(prog, "uShadowEnabled"),      scene->shadowEnabled && E_FLAG_SHADOWS);
    glProgramUniformMatrix4fv(prog, E_getUniformLocation(prog, "uLightSpaceMatrix"), 1, GL_FALSE, scene->lightSpaceMatrix);
    glProgramUniform1i       (prog, E_getUniformLocation(prog, "uShadowMap"),          3);

    glProgramUniform3fv(prog, E_getUniformLocation(prog, "uAmbientLightColor"),     1, scene->ambientLight.color);
    glProgramUniform1f (prog, E_getUniformLocation(prog, "uAmbientLightIntensity"),    scene->ambientLight.intensity);

    if (!E_FLAG_LIGHTING)
    {
        glProgramUniform1f (prog, E_getUniformLocation(prog, "uAmbientLightIntensity"), scene->ambientLight.intensity * 3.0f);
        static const float kZero3[3] = {0.0f, 0.0f, 0.0f};
        glProgramUniform3fv(prog, E_getUniformLocation(prog, "uDirectionalLightColor"), 1, kZero3);
        glProgramUniform1f (prog, E_getUniformLocation(prog, "uDirectionalLightIntensity"), 0.0f);
        glProgramUniform1i (prog, E_getUniformLocation(prog, "uNumSpotLights"),  0);
        glProgramUniform1i (prog, E_getUniformLocation(prog, "uNumPointLights"), 0);
        glProgramUniform1i (prog, E_getUniformLocation(prog, "uSpotShadowEnabled"), 0);
        glProgramUniform1i (prog, E_getUniformLocation(prog, "uEmissiveEnabled"), 0);
        return;
    }
    glProgramUniform1i(prog, E_getUniformLocation(prog, "uEmissiveEnabled"), 1);

    glProgramUniform3fv(prog, E_getUniformLocation(prog, "uDirectionalLightDirection"), 1, scene->directionalLight.direction);
    glProgramUniform3fv(prog, E_getUniformLocation(prog, "uDirectionalLightColor"),     1, scene->directionalLight.color);
    glProgramUniform1f (prog, E_getUniformLocation(prog, "uDirectionalLightIntensity"),    scene->directionalLight.intensity);

    char buf[64];

    // --- spot lights ---
    {
        LightSort sorted[E_SHADER_MAX_LIGHTS];
        int activeSpots = 0;
        for (int j = 0; j < scene->spotLightCount && activeSpots < E_SHADER_MAX_LIGHTS; j++)
        {
            if (!scene->spotLights[j].active) continue;
            sorted[activeSpots].idx = j;
            // The chosen shadow caster sorts first so it lands at a known shader slot
            sorted[activeSpots].distSq = (j == scene->spotShadowCasterIdx) ? -1.0f
                                           : distSq3(scene->spotLights[j].position, worldPos);
            activeSpots++;
        }
        qsort(sorted, activeSpots, sizeof(LightSort), cmpLightSort);

        int numSpots = activeSpots;
        glProgramUniform1i(prog, E_getUniformLocation(prog, "uNumSpotLights"), numSpots);

        // Find which shader slot the chosen shadow caster landed in
        int spotShadowIdx = -1;
        if (scene->spotShadowEnabled)
            for (int j = 0; j < numSpots; j++)
                if (sorted[j].idx == scene->spotShadowCasterIdx)
                    { spotShadowIdx = j; break; }

        glProgramUniform1i       (prog, E_getUniformLocation(prog, "uSpotShadowEnabled"),      scene->spotShadowEnabled);
        glProgramUniform1i       (prog, E_getUniformLocation(prog, "uSpotShadowCasterIdx"),    spotShadowIdx);
        glProgramUniformMatrix4fv(prog, E_getUniformLocation(prog, "uSpotLightSpaceMatrix"), 1, GL_FALSE, (const float*)scene->spotLightSpaceMatrix);
        glProgramUniform1i       (prog, E_getUniformLocation(prog, "uSpotShadowMap"),          4);

        for (int j = 0; j < numSpots; j++)
        {
            const E_spotLight* sl = &scene->spotLights[sorted[j].idx];
            snprintf(buf, sizeof(buf), "uSpotLights[%d].position",    j); glProgramUniform3fv(prog, E_getUniformLocation(prog, buf), 1, sl->position);
            snprintf(buf, sizeof(buf), "uSpotLights[%d].direction",   j); glProgramUniform3fv(prog, E_getUniformLocation(prog, buf), 1, sl->direction);
            snprintf(buf, sizeof(buf), "uSpotLights[%d].color",       j); glProgramUniform3fv(prog, E_getUniformLocation(prog, buf), 1, sl->color);
            snprintf(buf, sizeof(buf), "uSpotLights[%d].intensity",   j); glProgramUniform1f (prog, E_getUniformLocation(prog, buf),    sl->intensity);
            snprintf(buf, sizeof(buf), "uSpotLights[%d].cutOff",      j); glProgramUniform1f (prog, E_getUniformLocation(prog, buf),    sl->cutOff);
            snprintf(buf, sizeof(buf), "uSpotLights[%d].outerCutOff", j); glProgramUniform1f (prog, E_getUniformLocation(prog, buf),    sl->outerCutOff);
            snprintf(buf, sizeof(buf), "uSpotLights[%d].range",       j); glProgramUniform1f (prog, E_getUniformLocation(prog, buf),    sl->range);
        }
    }

    // --- point lights ---
    {
        LightSort sorted[E_SHADER_MAX_LIGHTS];
        int activePoints = 0;
        for (int j = 0; j < scene->pointLightCount && activePoints < E_SHADER_MAX_LIGHTS; j++)
        {
            if (!scene->pointLights[j].active) continue;
            sorted[activePoints].idx    = j;
            sorted[activePoints].distSq = distSq3(scene->pointLights[j].position, worldPos);
            activePoints++;
        }
        qsort(sorted, activePoints, sizeof(LightSort), cmpLightSort);

        int numPoints = activePoints;
        glProgramUniform1i(prog, E_getUniformLocation(prog, "uNumPointLights"), numPoints);

        for (int j = 0; j < numPoints; j++)
        {
            const E_pointLight* pl = &scene->pointLights[sorted[j].idx];
            snprintf(buf, sizeof(buf), "uPointLights[%d].position",         j); glProgramUniform3fv(prog, E_getUniformLocation(prog, buf), 1, pl->position);
            snprintf(buf, sizeof(buf), "uPointLights[%d].color",            j); glProgramUniform3fv(prog, E_getUniformLocation(prog, buf), 1, pl->color);
            snprintf(buf, sizeof(buf), "uPointLights[%d].intensity",        j); glProgramUniform1f (prog, E_getUniformLocation(prog, buf),    pl->intensity);
            snprintf(buf, sizeof(buf), "uPointLights[%d].range",            j); glProgramUniform1f (prog, E_getUniformLocation(prog, buf),    pl->range);
            snprintf(buf, sizeof(buf), "uPointLights[%d].applyAttenuation", j); glProgramUniform1i (prog, E_getUniformLocation(prog, buf),    pl->applyAttenuation);
        }
    }
}

void E_scene_render(E_scene* scene)
{
    s_renderFrame++;

    // Save FBO/viewport once  avoids GPU readback stalls in each shadow pass
    GLint prevFBO, viewport[4];
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFBO);
    glGetIntegerv(GL_VIEWPORT, viewport);

    // Compute dt for animation update
    static double s_lastAnimTime = -1.0;
    double now = E_Utils_getElapsedTimeInSeconds();
    float dt = (s_lastAnimTime < 0.0) ? 0.0f : (float)(now - s_lastAnimTime);
    s_lastAnimTime = now;

    // Point light flicker update
    for (int i = 0; i < scene->pointLightCount; i++)
    {
        E_pointLight* pl = &scene->pointLights[i];
        if (!pl->active || !pl->flicker) { pl->intensity = pl->baseIntensity; continue; }
        float t = (float)now * pl->flickerSpeed;
        float f = sinf(t * 3.7f) * 0.30f + sinf(t * 7.1f) * 0.20f + sinf(t * 13.3f) * 0.10f;
        f = (f + 0.6f) / 1.2f; // map [-0.6, 0.6] → [0, 1]
        pl->intensity = pl->baseIntensity * (1.0f - pl->flickerAmount + pl->flickerAmount * f);
        if (pl->intensity < 0.0f) pl->intensity = 0.0f;
    }

    // Spot light flicker update
    for (int i = 0; i < scene->spotLightCount; i++)
    {
        E_spotLight* sl = &scene->spotLights[i];
        if (!sl->active || !sl->flicker) { sl->intensity = sl->baseIntensity; continue; }
        float t = (float)now * sl->flickerSpeed;
        float f = sinf(t * 3.7f) * 0.30f + sinf(t * 7.1f) * 0.20f + sinf(t * 13.3f) * 0.10f;
        f = (f + 0.6f) / 1.2f;
        sl->intensity = sl->baseIntensity * (1.0f - sl->flickerAmount + sl->flickerAmount * f);
        if (sl->intensity < 0.0f) sl->intensity = 0.0f;
    }

    // Animation pre-pass: update poses, world matrices, then skinning matrices
    for (int i = 0; i < scene->count; i++)
    {
        E_sceneObject* obj = scene->objects[i];
        if (!obj->active || !obj->animator) continue;
        E_animator_update(obj->animator, dt);
        E_sceneObject_updateWorldMatrices(obj, NULL);
        E_animator_computeSkinningMatrices(obj->animator);
    }

    // World matrix pre-pass for non-animated objects (animated ones already done above)
    for (int i = 0; i < scene->count; i++)
    {
        E_sceneObject* obj = scene->objects[i];
        if (!obj->active || obj->animator) continue;
        E_sceneObject_updateWorldMatrices(obj, NULL);
    }

    // Shadow pass
    if (scene->shadowEnabled && E_FLAG_SHADOWS)
    {
        computeLightSpaceMatrix(scene, (float*)scene->lightSpaceMatrix);

        glBindFramebuffer(GL_FRAMEBUFFER, scene->shadowFBO);
        glViewport(0, 0, scene->shadowMapSize, scene->shadowMapSize);
        glClear(GL_DEPTH_BUFFER_BIT);
        glEnable(GL_POLYGON_OFFSET_FILL);
        glPolygonOffset(2.0f, 4.0f);

        static const float kIdentity[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
        GLuint shadowProgs[2] = { uberShaderProgramObject, skinnedShaderProgramObject };
        for (int p = 0; p < 2; p++)
        {
            glProgramUniformMatrix4fv(shadowProgs[p], E_getUniformLocation(shadowProgs[p], "uView"),       1, GL_FALSE, scene->lightSpaceMatrix);
            glProgramUniformMatrix4fv(shadowProgs[p], E_getUniformLocation(shadowProgs[p], "uProjection"), 1, GL_FALSE, kIdentity);
            glProgramUniform1i       (shadowProgs[p], E_getUniformLocation(shadowProgs[p], "uShadowPass"), 1);
        }

        glUseProgram(uberShaderProgramObject);
        for (int i = 0; i < scene->count; i++)
        {
            E_sceneObject* obj = scene->objects[i];
            if (obj->active)
                shadowRenderObject(obj, uberShaderProgramObject);
        }

        for (int p = 0; p < 2; p++)
            glProgramUniform1i(shadowProgs[p], E_getUniformLocation(shadowProgs[p], "uShadowPass"), 0);
        glUseProgram(0);
        glDisable(GL_POLYGON_OFFSET_FILL);
        glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
        glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

        // Bind shadow map to unit 3 for the rest of the frame
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, scene->shadowDepthTex);
        glActiveTexture(GL_TEXTURE0);
    }

    // Spot light shadow pass
    if (!E_FLAG_SHADOWS) { scene->spotShadowEnabled = 0; }
    if (E_FLAG_SHADOWS)
    {
        E_spotLight* caster  = NULL;
        float        bestDsq = 1e38f;
        for (int i = 0; i < scene->spotLightCount; i++)
        {
            E_spotLight* sl = &scene->spotLights[i];
            if (!sl->active || !sl->castShadow) continue;
            float dx = sl->position[0] - engineCamera.position[0];
            float dy = sl->position[1] - engineCamera.position[1];
            float dz = sl->position[2] - engineCamera.position[2];
            float dsq = dx*dx + dy*dy + dz*dz;
            if (dsq < bestDsq) { bestDsq = dsq; caster = sl; }
        }

        scene->spotShadowEnabled  = (caster != NULL) ? 1 : 0;
        scene->spotShadowCasterIdx = (caster != NULL) ? (int)(caster - scene->spotLights) : -1;

        if (scene->spotShadowEnabled)
        {
            scene->spotLightSpaceMatrix = computeSpotLightSpaceMatrix(caster);

            glBindFramebuffer(GL_FRAMEBUFFER, scene->spotShadowFBO);
            glViewport(0, 0, E_SPOT_SHADOW_MAP_SIZE, E_SPOT_SHADOW_MAP_SIZE);
            glClear(GL_DEPTH_BUFFER_BIT);
            glEnable(GL_POLYGON_OFFSET_FILL);
            glPolygonOffset(2.0f, 4.0f);

            static const float kIdentity[16] = {1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
            GLuint shadowProgs[2] = { uberShaderProgramObject, skinnedShaderProgramObject };
            for (int p = 0; p < 2; p++)
            {
                glProgramUniformMatrix4fv(shadowProgs[p], E_getUniformLocation(shadowProgs[p], "uView"),       1, GL_FALSE, (const float*)scene->spotLightSpaceMatrix);
                glProgramUniformMatrix4fv(shadowProgs[p], E_getUniformLocation(shadowProgs[p], "uProjection"), 1, GL_FALSE, kIdentity);
                glProgramUniform1i       (shadowProgs[p], E_getUniformLocation(shadowProgs[p], "uShadowPass"), 1);
            }

            glUseProgram(uberShaderProgramObject);
            for (int i = 0; i < scene->count; i++)
                if (scene->objects[i]->active)
                    shadowRenderObject(scene->objects[i], uberShaderProgramObject);

            for (int p = 0; p < 2; p++)
                glProgramUniform1i(shadowProgs[p], E_getUniformLocation(shadowProgs[p], "uShadowPass"), 0);
            glUseProgram(0);
            glDisable(GL_POLYGON_OFFSET_FILL);
            glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prevFBO);
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

            glActiveTexture(GL_TEXTURE4);
            glBindTexture(GL_TEXTURE_2D, scene->spotShadowDepthTex);
            glActiveTexture(GL_TEXTURE0);
        }
    }

    // Opaque pass: render all non-transparent nodes (also updates world matrices for the whole hierarchy)
    E_sceneObject_setSkipTransparent(1);
    for (int i = 0; i < scene->count; i++)
    {
        E_sceneObject* obj = scene->objects[i];
        if (!obj->active) continue;
        E_renderSceneObject(obj, scene, NULL);
    }
    E_sceneObject_setSkipTransparent(0);

    // Collect transparent nodes (world matrices now correct from opaque pass)
    static TransNode s_transNodesBuf[512];
    int tCap = 512, tCount = 0;
    TransNode* tNodes = s_transNodesBuf;
    float camPos[3] = { engineCamera.position[0], engineCamera.position[1], engineCamera.position[2] };
    for (int i = 0; i < scene->count; i++)
        if (scene->objects[i]->active)
            collectTransparent(scene->objects[i], camPos, &tNodes, &tCount, &tCap);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    for (int i = 0; i < tCount; i++)
        E_renderSceneObjectSingle(tNodes[i].obj, scene);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    // Update then render all particle emitters
    for (int i = 0; i < scene->emitterCount; i++)
        if (scene->emitters[i].active)
            E_emitter_update(&scene->emitters[i]);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);
    for (int i = 0; i < scene->emitterCount; i++)
        if (scene->emitters[i].active)
            E_emitter_render(&scene->emitters[i]);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}

void E_scene_destroy(E_scene* scene)
{
    if (scene->shadowDepthTex)     { glDeleteTextures(1, &scene->shadowDepthTex);         scene->shadowDepthTex     = 0; }
    if (scene->shadowFBO)          { glDeleteFramebuffers(1, &scene->shadowFBO);          scene->shadowFBO          = 0; }
    if (scene->spotShadowDepthTex) { glDeleteTextures(1, &scene->spotShadowDepthTex);     scene->spotShadowDepthTex = 0; }
    if (scene->spotShadowFBO)      { glDeleteFramebuffers(1, &scene->spotShadowFBO);      scene->spotShadowFBO      = 0; }

    free(scene->objects);
    scene->objects  = NULL;
    scene->count    = 0;
    scene->capacity = 0;

    free(scene->spotLights);
    scene->spotLights        = NULL;
    scene->spotLightCount    = 0;
    scene->spotLightCapacity = 0;

    free(scene->pointLights);
    scene->pointLights        = NULL;
    scene->pointLightCount    = 0;
    scene->pointLightCapacity = 0;

    for (int i = 0; i < scene->emitterCount; i++)
        E_emitter_destroy(&scene->emitters[i]);
    scene->emitterCount = 0;
}


void E_scene_setShadowEnabled(E_scene* scene, int enabled)
{
    scene->shadowEnabled = enabled;
}

void E_scene_setShadowArea(E_scene* scene, float cx, float cy, float cz, float radius)
{
    scene->shadowCenter[0] = cx;
    scene->shadowCenter[1] = cy;
    scene->shadowCenter[2] = cz;
    scene->shadowRadius    = radius;
}

E_spline* E_scene_addSpline(E_scene* scene, const char* name)
{
    if (scene->splineCount >= E_SPLINE_MAX_COUNT) return NULL;

    E_spline* s = &scene->splines[scene->splineCount];

    if (name && name[0])
        snprintf(s->name, sizeof(s->name), "%s", name);
    else
        snprintf(s->name, sizeof(s->name), "spline_%d", scene->splineCount);

    s->numPoints = 4;
    s->points[0] = vmath::vec3(-6,2,0);
    s->points[1] = vmath::vec3(-2,4,4);
    s->points[2] = vmath::vec3( 2,4,-4);
    s->points[3] = vmath::vec3( 6,2,0);

    scene->splineCount++;
    return s;
}

void E_scene_removeSpline(E_scene* scene, int idx)
{
    if (idx < 0 || idx >= scene->splineCount) return;

    for (int i = idx; i < scene->splineCount - 1; i++)
        scene->splines[i] = scene->splines[i + 1];

    scene->splineCount--;

    if (scene->activeSpline >= scene->splineCount)
        scene->activeSpline = scene->splineCount - 1;

    if (scene->activeSpline < 0)
    {
        scene->activeSpline = 0;
        scene->splineFollow = 0;
    }
}

