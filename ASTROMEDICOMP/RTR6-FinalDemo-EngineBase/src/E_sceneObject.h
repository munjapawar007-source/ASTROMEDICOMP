#pragma once
#include "vmath.h"
#include "E_mesh.h"
#include "E_material.h"
#include "E_anim.h"

typedef struct E_sceneObject_s
{
    char        name[64];
    E_mesh      mesh;
    E_material  material;
    int         hasMesh;

    // Local transform (relative to parent)
    vmath::vec3 position;
    vmath::vec3 rotation;      // degrees
    vmath::vec3 scale;
    vmath::mat4 modelMatrix;   // local: computed from TRS, or set directly when customMatrix=1
    vmath::mat4 worldMatrix;   // computed each frame: parent_world * modelMatrix

    int         active;
    int         customMatrix;  // 1 = modelMatrix is set externally, skip TRS recompute
    int         uniformScale;  // 1 = scale is uniform (single float); 0 = per-axis
    int         matrixDirty;  // 1 = modelMatrix needs recompute before next render

    int         transparent;   // 1 = rendered in transparent back-to-front pass with alpha blending
    int         castShadow;   // 0 = skip this node's mesh during shadow depth pass
    int         receiveShadow; // 0 = shadow map is ignored when shading this node
    int         writeColor;   // 0 = color buffer writes disabled (shadow-only rendering)
    int         godRayEmitter; // 1 = writes emissive to the god ray texture (attachment 2)
    float       cullRadius;   // >0 = use this world-space sphere for frustum cull (skinned meshes)

    // Skinned animation (set on root and on each skinned primitive)
    E_animator* animator;
    int         isSkinned;

    // Hierarchy
    struct E_sceneObject_s*  parent;
    struct E_sceneObject_s** children;
    int                      childCount;
    int                      childCapacity;
} E_sceneObject;

// Creates a node with a default triangle mesh (hasMesh=1). Backward-compatible.
E_sceneObject E_createSceneObject(const char* name);

// Creates a transform-only node (hasMesh=0, no mesh/material).
E_sceneObject E_createSceneNode(const char* name);

void E_sceneObject_setMaterial    (E_sceneObject* obj, E_material mat);
void E_sceneObject_setMaterialAll (E_sceneObject* obj, E_material mat); // applies to all mesh descendants
void E_sceneObject_setAlbedo    (E_sceneObject* obj, GLuint texture);
void E_sceneObject_setAlbedoAll (E_sceneObject* obj, GLuint texture); // applies to all mesh descendants
void E_sceneObject_setBaseColor    (E_sceneObject* obj, float r, float g, float b, float a);
void E_sceneObject_setBaseColorAll (E_sceneObject* obj, float r, float g, float b, float a);
void E_sceneObject_setTransparentAll(E_sceneObject* obj, int transparent);
void E_sceneObject_setCullRadius      (E_sceneObject* obj, float radius);
void E_sceneObject_setCullRadiusAll   (E_sceneObject* obj, float radius);
void E_sceneObject_setCastShadow      (E_sceneObject* obj, int castShadow);
void E_sceneObject_setCastShadowAll   (E_sceneObject* obj, int castShadow);
void E_sceneObject_setReceiveShadow   (E_sceneObject* obj, int receiveShadow);
void E_sceneObject_setReceiveShadowAll(E_sceneObject* obj, int receiveShadow);
void E_sceneObject_setWriteColorAll    (E_sceneObject* obj, int writeColor);
void E_sceneObject_setGodRayEmitterAll (E_sceneObject* obj, int v);
void E_sceneObject_setShader    (E_sceneObject* obj, GLuint shaderProgram);
void E_sceneObject_setShaderAll (E_sceneObject* obj, GLuint shaderProgram);
void E_sceneObject_setPosition  (E_sceneObject* obj, vmath::vec3 pos);
void E_sceneObject_setRotation  (E_sceneObject* obj, vmath::vec3 rot);
void E_sceneObject_setScale     (E_sceneObject* obj, float s);
void E_sceneObject_setScaleXYZ  (E_sceneObject* obj, vmath::vec3 scale);
void E_sceneObject_setActive    (E_sceneObject* obj, int active);
void E_sceneObject_updateMatrix (E_sceneObject* obj);
void E_sceneObject_addChild              (E_sceneObject* parent, E_sceneObject* child);
void E_sceneObject_updateWorldMatrices   (E_sceneObject* obj, const float* parentWorldMatrix);

// Renders this node and all descendants. Pass NULL as parentWorldMatrix for root nodes.
void E_renderSceneObject(E_sceneObject* obj, void* scene, const float* parentWorldMatrix);

// Controls whether transparent nodes are skipped during E_renderSceneObject (1 = skip, 0 = render all).
void E_sceneObject_setSkipTransparent(int skip);

// Extract frustum planes from the current camera. Call once per frame before rendering.
void E_frustum_update(void);
void E_frustum_setCullEnabled(int enabled);

// Draws only this node's mesh (no children). Assumes worldMatrix is already current.
// Used by the scene's sorted transparent pass.
void E_renderSceneObjectSingle(E_sceneObject* obj, void* scene);

void E_sceneObject_destroyResources(E_sceneObject* obj);
void E_destroySceneObject();
