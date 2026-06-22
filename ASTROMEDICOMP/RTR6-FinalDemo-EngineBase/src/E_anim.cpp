#include "E_anim.h"
#include "E_engineFlags.h"
#include "E_animation.h"
#include "E_sceneObject.h"
#include "vmath.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static vmath::mat4 trs_mat4(vmath::vec3 t, vmath::vec4 r, vmath::vec3 s)
{
    float qx=r[0], qy=r[1], qz=r[2], qw=r[3];
    vmath::mat4 m;
    m[0] = vmath::vec4((1-2*(qy*qy+qz*qz))*s[0], (2*(qx*qy+qw*qz))*s[0],   (2*(qx*qz-qw*qy))*s[0],   0.0f);
    m[1] = vmath::vec4((2*(qx*qy-qw*qz))*s[1],   (1-2*(qx*qx+qz*qz))*s[1], (2*(qy*qz+qw*qx))*s[1],   0.0f);
    m[2] = vmath::vec4((2*(qx*qz+qw*qy))*s[2],   (2*(qy*qz-qw*qx))*s[2],   (1-2*(qx*qx+qy*qy))*s[2], 0.0f);
    m[3] = vmath::vec4(t[0], t[1], t[2], 1.0f);
    return m;
}

static vmath::vec3 lerp3(vmath::vec3 a, vmath::vec3 b, float t)
{
    return a + (b - a) * t;
}

static vmath::vec4 nlerp4(vmath::vec4 qa, vmath::vec4 qb, float t)
{
    if (vmath::dot(qa, qb) < 0.0f) qb = qb * -1.0f;
    vmath::vec4 result = qa + (qb - qa) * t;
    float len = vmath::length(result);
    return (len > 0.0001f) ? result / len : result;
}

static void findKeyframes(const E_animSampler* s, float t, int* lo, int* hi, float* alpha)
{
    if (s->count == 1) { *lo=*hi=0; *alpha=0; return; }
    if (t <= s->times[0]) { *lo=*hi=0; *alpha=0; return; }
    if (t >= s->times[s->count-1]) { *lo=*hi=s->count-1; *alpha=0; return; }
    for (int i = 0; i < s->count-1; i++) {
        if (t >= s->times[i] && t < s->times[i+1]) {
            *lo = i; *hi = i+1;
            float range = s->times[i+1] - s->times[i];
            *alpha = (range > 0.0001f) ? (t - s->times[i]) / range : 0;
            return;
        }
    }
    *lo = *hi = s->count-1; *alpha = 0;
}


E_animator* E_animator_create(int nodeCount, int clipCount, int jointCount)
{
    E_animator* a = (E_animator*)calloc(1, sizeof(E_animator));
    a->clips      = (E_animClip*)calloc(clipCount > 0 ? clipCount : 1, sizeof(E_animClip));
    a->clipCount  = clipCount;
    a->activeClip = 0;
    a->time       = 0;
    a->speed      = 1.0f;
    a->nodes      = (E_sceneObject**)calloc(nodeCount > 0 ? nodeCount : 1, sizeof(E_sceneObject*));
    a->nodeCount  = nodeCount;
    a->nodeTrans  = (float*)calloc(nodeCount * 3, sizeof(float));
    a->nodeRots   = (float*)calloc(nodeCount * 4, sizeof(float));
    a->nodeScales = (float*)malloc(nodeCount * 3 * sizeof(float));
    for (int i = 0; i < nodeCount; i++) {
        a->nodeRots[i*4+3]    = 1.0f;
        a->nodeScales[i*3+0]  = 1.0f;
        a->nodeScales[i*3+1]  = 1.0f;
        a->nodeScales[i*3+2]  = 1.0f;
    }
    a->jointCount = (jointCount < E_MAX_JOINTS) ? jointCount : E_MAX_JOINTS;
    for (int j = 0; j < E_MAX_JOINTS; j++)
        *(vmath::mat4*)(a->skinningMatrices + j*16) = vmath::mat4::identity();

    glGenBuffers(1, &a->boneUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, a->boneUBO);
    glBufferData(GL_UNIFORM_BUFFER, E_MAX_JOINTS * 64, a->skinningMatrices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return a;
}

void E_animator_setNodeRest(E_animator* anim, int ni, const float* t, const float* r, const float* s)
{
    if (!anim || ni < 0 || ni >= anim->nodeCount) return;
    if (t) { anim->nodeTrans[ni*3+0]=t[0]; anim->nodeTrans[ni*3+1]=t[1]; anim->nodeTrans[ni*3+2]=t[2]; }
    if (r) { anim->nodeRots[ni*4+0]=r[0];  anim->nodeRots[ni*4+1]=r[1];  anim->nodeRots[ni*4+2]=r[2];  anim->nodeRots[ni*4+3]=r[3]; }
    if (s) { anim->nodeScales[ni*3+0]=s[0]; anim->nodeScales[ni*3+1]=s[1]; anim->nodeScales[ni*3+2]=s[2]; }
}

void E_animator_destroy(E_animator* anim)
{
    if (!anim) return;
    for (int i = 0; i < anim->clipCount; i++) {
        E_animClip* c = &anim->clips[i];
        for (int s = 0; s < c->samplerCount; s++) {
            free(c->samplers[s].times);
            free(c->samplers[s].values);
        }
        free(c->samplers);
        free(c->channels);
    }
    if (anim->boneUBO) glDeleteBuffers(1, &anim->boneUBO);
    free(anim->clips);
    free(anim->nodes);
    free(anim->nodeTrans);
    free(anim->nodeRots);
    free(anim->nodeScales);
    free(anim);
}

void E_animator_update(E_animator* anim, float dt)
{
    if (!anim || anim->clipCount == 0) return;
    int ci = anim->activeClip;
    if (ci < 0 || ci >= anim->clipCount) return;
    E_animClip* clip = &anim->clips[ci];
    if (clip->duration <= 0) return;

    if (E_FLAG_ANIMATIONS)
    {
        anim->time += dt * anim->speed;
        if (anim->time > clip->duration)
            anim->time = fmodf(anim->time, clip->duration);
    }

    for (int ch = 0; ch < clip->channelCount; ch++) {
        E_animChannel* chan = &clip->channels[ch];
        int ni = chan->nodeIdx;
        if (ni < 0 || ni >= anim->nodeCount) continue;
        E_sceneObject* node = anim->nodes[ni];
        if (!node) continue;

        E_animSampler* samp = &clip->samplers[chan->samplerIdx];
        if (samp->count == 0) continue;

        int lo, hi; float alpha;
        findKeyframes(samp, anim->time, &lo, &hi, &alpha);

        const float* valA = samp->values + lo * samp->components;
        const float* valB = samp->values + hi * samp->components;

        if (chan->path == E_ANIM_PATH_ROTATION) {
            vmath::vec4 r = nlerp4(vmath::vec4(valA[0],valA[1],valA[2],valA[3]), vmath::vec4(valB[0],valB[1],valB[2],valB[3]), alpha);
            anim->nodeRots[ni*4+0]=r[0]; anim->nodeRots[ni*4+1]=r[1];
            anim->nodeRots[ni*4+2]=r[2]; anim->nodeRots[ni*4+3]=r[3];
        } else if (chan->path == E_ANIM_PATH_TRANSLATION) {
            vmath::vec3 r = lerp3(vmath::vec3(valA[0],valA[1],valA[2]), vmath::vec3(valB[0],valB[1],valB[2]), alpha);
            anim->nodeTrans[ni*3+0]=r[0]; anim->nodeTrans[ni*3+1]=r[1]; anim->nodeTrans[ni*3+2]=r[2];
        } else {
            vmath::vec3 r = lerp3(vmath::vec3(valA[0],valA[1],valA[2]), vmath::vec3(valB[0],valB[1],valB[2]), alpha);
            anim->nodeScales[ni*3+0]=r[0]; anim->nodeScales[ni*3+1]=r[1]; anim->nodeScales[ni*3+2]=r[2];
        }

        const float* nt = anim->nodeTrans  + ni*3;
        const float* nr = anim->nodeRots   + ni*4;
        const float* ns = anim->nodeScales + ni*3;
        node->modelMatrix  = trs_mat4(
            vmath::vec3(nt[0], nt[1], nt[2]),
            vmath::vec4(nr[0], nr[1], nr[2], nr[3]),
            vmath::vec3(ns[0], ns[1], ns[2]));
        node->customMatrix = 1;
    }
}

// Requires E_sceneObject_updateWorldMatrices to have been called first
void E_animator_computeSkinningMatrices(E_animator* anim)
{
    if (!anim || anim->jointCount == 0) return;
    for (int j = 0; j < anim->jointCount; j++) {
        int ni = anim->jointNodeIndices[j];
        if (ni < 0 || ni >= anim->nodeCount || !anim->nodes[ni]) continue;
        *(vmath::mat4*)(anim->skinningMatrices + j*16) =
            anim->nodes[ni]->worldMatrix *
            *(const vmath::mat4*)(anim->inverseBindMats + j*16);
    }

    glBindBuffer(GL_UNIFORM_BUFFER, anim->boneUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, anim->jointCount * 64, anim->skinningMatrices);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// animation API

void E_animator_setClip(E_animator* anim, int index)
{
    if (!anim || index < 0 || index >= anim->clipCount) return;
    anim->activeClip = index;
    anim->time       = 0.0f;
}

void E_animator_setClipByName(E_animator* anim, const char* name)
{
    if (!anim || !name) return;
    for (int i = 0; i < anim->clipCount; i++) {
        if (strcmp(anim->clips[i].name, name) == 0) {
            anim->activeClip = i;
            anim->time       = 0.0f;
            return;
        }
    }
}

void E_animator_setSpeed(E_animator* anim, float speed)
{
    if (!anim) return;
    anim->speed = speed;
}

void E_animator_setTime(E_animator* anim, float time)
{
    if (!anim || anim->activeClip < 0 || anim->activeClip >= anim->clipCount) return;
    float dur = anim->clips[anim->activeClip].duration;
    anim->time = (dur > 0.0f) ? fmodf(time, dur) : 0.0f;
}

void E_animator_pause(E_animator* anim)
{
    if (!anim) return;
    anim->speed = 0.0f;
}

void E_animator_resume(E_animator* anim)
{
    if (!anim) return;
    anim->speed = 1.0f;
}

int E_animator_getClipCount(E_animator* anim)
{
    return anim ? anim->clipCount : 0;
}

const char* E_animator_getClipName(E_animator* anim, int index)
{
    if (!anim || index < 0 || index >= anim->clipCount) return NULL;
    return anim->clips[index].name;
}

float E_animator_getDuration(E_animator* anim)
{
    if (!anim || anim->activeClip < 0 || anim->activeClip >= anim->clipCount) return 0.0f;
    return anim->clips[anim->activeClip].duration;
}

E_sceneObject* E_animator_getBoneNode(E_animator* anim, const char* boneName)
{
    if (!anim || !boneName) return NULL;
    for (int i = 0; i < anim->nodeCount; i++) {
        E_sceneObject* node = anim->nodes[i];
        if (node && strcmp(node->name, boneName) == 0)
            return node;
    }
    return NULL;
}

vmath::vec3 E_animator_getBonePosition(E_animator* anim, const char* boneName)
{
    E_sceneObject* node = E_animator_getBoneNode(anim, boneName);
    if (!node) return vmath::vec3(0.0f, 0.0f, 0.0f);
    const float* m = (const float*)node->worldMatrix;
    return vmath::vec3(m[12], m[13], m[14]);
}

vmath::vec3 E_animator_getBoneRotation(E_animator* anim, const char* boneName)
{
    E_sceneObject* node = E_animator_getBoneNode(anim, boneName);
    if (!node) return vmath::vec3(0.0f, 0.0f, 1.0f);
    const float* m = (const float*)node->worldMatrix;
    return vmath::vec3(m[8], m[9], m[10]);
}
