#pragma once
#include "E_sceneObject.h"
#include "E_fog.h"
#include "E_light.h"
#include "E_spline.h"
#include "E_particles.h"

typedef struct
{
    E_sceneObject**    objects;
    int                count;
    int                capacity;
    E_fogProperties    fog;
    E_directionalLight directionalLight;
    E_ambientLight     ambientLight;
    E_spotLight*       spotLights;
    int                spotLightCount;
    int                spotLightCapacity;
    E_pointLight*      pointLights;
    int                pointLightCount;
    int                pointLightCapacity;
    // --- Splines ---
    E_spline splines[E_SPLINE_MAX_COUNT];
    int      splineCount;

    // --- Particle emitters ---
    E_emitter emitters[E_EMITTER_MAX_COUNT];
    int       emitterCount;

    // --- Spline follow state (camera-related, but scene-driven) ---
    int   activeSpline;
    float splineT;
    float splineDir;
    float splineSpeed;
    int   splineFollow; // bool

    // --- Directional shadow map ---
    GLuint      shadowFBO;
    GLuint      shadowDepthTex;
    vmath::mat4 lightSpaceMatrix;
    int         shadowMapSize;
    float       shadowCenter[3];
    float       shadowRadius;
    int         shadowEnabled;

    // --- Spot light shadow map ---
    GLuint      spotShadowFBO;
    GLuint      spotShadowDepthTex;
    vmath::mat4 spotLightSpaceMatrix;
    int         spotShadowEnabled;
    int         spotShadowCasterIdx;  // index into spotLights[] of the chosen caster

} E_scene;

void E_scene_init(E_scene* scene);
void E_scene_add(E_scene* scene, E_sceneObject* obj);
void E_scene_remove(E_scene* scene, int index);
void E_scene_render(E_scene* scene);
void E_scene_destroy(E_scene* scene);

void E_scene_addOwned(E_scene* scene, E_sceneObject* obj);

// Per-draw-call scene uniform helpers (called from E_renderSceneObject)
void E_scene_applyFog     (const E_scene* scene, GLuint prog);
void E_scene_applyLighting(const E_scene* scene, GLuint prog, const float worldPos[3]);

E_spline* E_scene_addSpline(E_scene* scene, const char* name);
void E_scene_removeSpline(E_scene* scene, int idx);

void E_scene_setShadowEnabled(E_scene* scene, int enabled);
void E_scene_setShadowArea   (E_scene* scene, float cx, float cy, float cz, float radius);



