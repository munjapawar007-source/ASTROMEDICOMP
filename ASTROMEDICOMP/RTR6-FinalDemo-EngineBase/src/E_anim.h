#pragma once
#include <GL/glew.h>
#include <gl/GL.h>

#define E_MAX_JOINTS 256

typedef enum {
    E_ANIM_PATH_TRANSLATION = 0,
    E_ANIM_PATH_ROTATION,
    E_ANIM_PATH_SCALE
} E_animPath;

typedef struct {
    float* times;       // [count]
    float* values;      // [count * components]
    int    count;
    int    components;  // 3 for T/S, 4 for R
} E_animSampler;

typedef struct {
    int        nodeIdx;    // index into animator->nodes[]
    E_animPath path;
    int        samplerIdx;
} E_animChannel;

typedef struct {
    char           name[64];
    E_animSampler* samplers;
    int            samplerCount;
    E_animChannel* channels;
    int            channelCount;
    float          duration;
} E_animClip;

typedef struct E_animator_s {
    E_animClip* clips;
    int         clipCount;
    int         activeClip;
    float       time;
    float       speed;

    // All scene nodes parallel to cgltf nodes at load time (not owned)
    struct E_sceneObject_s** nodes;
    int                      nodeCount;

    // Per-node rest-pose TRS (initialized from GLTF, updated by animation channels)
    float* nodeTrans;   // [nodeCount * 3]
    float* nodeRots;    // [nodeCount * 4]  quaternions xyzw
    float* nodeScales;  // [nodeCount * 3]

    // Skin
    int   jointCount;
    int   jointNodeIndices[E_MAX_JOINTS];
    float inverseBindMats[E_MAX_JOINTS * 16];   // column-major

    // Computed per-frame, uploaded to skinned shader via UBO
    float  skinningMatrices[E_MAX_JOINTS * 16];
    GLuint boneUBO;
} E_animator;

struct E_sceneObject_s;

E_animator* E_animator_create(int nodeCount, int clipCount, int jointCount);
void        E_animator_destroy(E_animator* anim);
void        E_animator_setNodeRest(E_animator* anim, int nodeIdx,
                                   const float* t, const float* r, const float* s);
void        E_animator_update(E_animator* anim, float dt);
void        E_animator_computeSkinningMatrices(E_animator* anim);
