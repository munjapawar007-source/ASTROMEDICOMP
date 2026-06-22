#include "../public/03-TajDinningToTVBlast.h"
#include "../public/DEMO.hpp"
#include "engineAPIs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "E_utils.h"
#include "E_spline.h"
#include "E_material.h"
#include "E_video.h"
#include <GL/glew.h>

static E_sceneObject* s_tajInterior = NULL;
static E_sceneObject* s_tv = NULL;
static E_sceneObject* s_skySphere = NULL;
static E_sceneObject* s_taj = NULL;
static E_sceneObject* s_terroristWalkingOnStairs = NULL;
static E_sceneObject* s_terrorist2IdleWithRifle   = NULL;
static E_sceneObject* s_terrorist2WalkingWithRifle = NULL;
static E_sceneObject* s_terroristStairsAK47 = NULL;
static E_sceneObject* s_terroristIdleAK47   = NULL;
static vmath::vec3    s_terroristStairsAK47PosOffset = vmath::vec3(0.0f, -0.40f, -0.10f);
static vmath::vec3    s_terroristIdleAK47PosOffset = vmath::vec3(0.05f, -0.25f, -0.05f);
#define DEAD_WAITRESS_COUNT 5
static E_sceneObject* s_deadBodies[DEAD_WAITRESS_COUNT] = {};
static E_video*       s_tvVideo           = NULL;
static E_emitter*     s_emitterFire1  = NULL;
static E_emitter*     s_emitterSmoke1 = NULL;
static E_emitter*     s_emitterSmoke2     = NULL;
static E_emitter*     s_emitterBlackSmoke = NULL;
static E_emitter*     s_emitterSmoke      = NULL;
static E_emitter*     s_emitterFire       = NULL;
static E_pointLight*  s_plTajFire = NULL;


static E_spline*      s_splineToTV        = NULL;
static E_spline*      s_splineToTVLookAt  = NULL;
static E_spline*      s_splineDeadBodyCamMovement          = NULL;
static E_spline*      s_splineDeadBodyCamFollowToTerrorist = NULL;
static E_spline*      s_splineTerroristToEvacuate          = NULL;
static E_spline*      s_splineCctvStyle                    = NULL;
static E_spline*      s_splineCctvStyleLookAt              = NULL;

static E_pointLight* s_pl_taj_1 = NULL;
static E_pointLight* s_pl_taj_2 = NULL;
static E_pointLight* s_pl_taj_3 = NULL;
static E_pointLight* s_pl_taj_4 = NULL;
static E_pointLight* s_pl_taj_5 = NULL;
static E_pointLight* s_pl_taj_6 = NULL;
static E_pointLight* s_pl_taj_7 = NULL;
static E_pointLight* s_pl_taj_8 = NULL;
static E_pointLight* s_pl_taj_9  = NULL;
static E_pointLight* s_pl_taj_10 = NULL;
static E_pointLight* s_pl_taj_11 = NULL;
static E_pointLight* s_pl_taj_12 = NULL;
static E_pointLight* s_pl_taj_tv = NULL;
static E_pointLight* s_pl_chabutara = NULL;
static E_spotLight*  s_sl_tvSpotlight = NULL;
static E_spotLight*  s_sl_tajLeft     = NULL;
static E_spotLight*  s_sl_tajRight    = NULL;
static E_spotLight*  s_sl_pillar      = NULL;
static E_spotLight*  s_sl_pillar2     = NULL;

static E_spline*      s_spline_2ndFloorWalk              = NULL;
#define TAJ_WALKER_COUNT 3
static E_sceneObject* s_tajWalkers[TAJ_WALKER_COUNT]     = {NULL, NULL, NULL};
static const float    s_tajWalkerOffset[TAJ_WALKER_COUNT] = {0.0f, 0.333f, 0.667f};

static void TajDinning_attachGunToRightHand(E_sceneObject* terrorist, E_sceneObject* gun, vmath::vec3 posOffset, vmath::vec3 rotOffset)
{
    if (!gun) return;
    if (!terrorist || !terrorist->active || !terrorist->animator)
    {
        E_sceneObject_setActive(gun, 0);
        return;
    }

    E_animator_update(terrorist->animator, 0.0f);
    E_sceneObject_updateWorldMatrices(terrorist, NULL);

    vmath::vec3 handPos = E_animator_getBonePosition(terrorist->animator, "mixamorig:RightHand");
    E_sceneObject_setPosition(gun, handPos + posOffset);
    E_sceneObject_setRotation(gun, terrorist->rotation + rotOffset);
    E_sceneObject_setActive(gun, 1);
}

void TajDinning_init()
{
    s_tajInterior = E_loadModel("DEMO_ASSETS/03-TajDinningToTVBlast/TajInteriorDinningScene_V2.glb", &engineScene, "s_tajInterior");
    s_tv    = E_loadModel("DEMO_ASSETS/03-TajDinningToTVBlast/TajTV.glb", &engineScene, "s_tv");
    s_tvVideo = E_video_load("DEMO_ASSETS/03-TajDinningToTVBlast/2611NewsV3.mov", 1);
    {
        E_material videoMat = E_createMaterialFromFiles("engineResources/video.vert", "engineResources/video.frag");
        videoMat.baseColor[0] = videoMat.baseColor[1] = videoMat.baseColor[2] = videoMat.baseColor[3] = 1.0f;
        E_sceneObject_setMaterialAll(s_tv, videoMat);
    }
    E_sceneObject_setCastShadowAll  (s_tv, 0);
    E_sceneObject_setReceiveShadowAll(s_tv, 0);

    s_skySphere = E_loadModel("DEMO_ASSETS/SkySphere/SkyDome.glb", &engineScene, "s_skySphere");
    {
        E_material skyMat = E_createMaterialFromFiles("DEMO_ASSETS/SkySphere/skysphere.vert", "DEMO_ASSETS/SkySphere/skysphere.frag");
        skyMat.texture  = E_loadTexture("DEMO_ASSETS/SkySphere/textures/qwantani_dawn_puresky.jpg");
        skyMat.texture1 = E_loadTexture("DEMO_ASSETS/SkySphere/textures/seamlessCloud.png");
        skyMat.texture2 = E_loadTexture("DEMO_ASSETS/SkySphere/textures/perlinNoise2.jpg");
        glProgramUniform1i(skyMat.shaderProgram, glGetUniformLocation(skyMat.shaderProgram, "uTexture_1"), 1);
        glProgramUniform1i(skyMat.shaderProgram, glGetUniformLocation(skyMat.shaderProgram, "uTexture_2"), 2);
        E_sceneObject_setMaterialAll    (s_skySphere, skyMat);
        E_sceneObject_setCastShadowAll  (s_skySphere, 0);
        E_sceneObject_setReceiveShadowAll(s_skySphere, 0);
    }
    E_sceneObject_setScale(s_skySphere, 100.0f);
    E_sceneObject_setRotation(s_skySphere, {0.0f, 86.0f, 0.0f});

    s_taj           = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/TAJ_v1.glb",                    &engineScene, "s_taj");

    s_terroristWalkingOnStairs = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist_WalkingOnStairs.glb", &engineScene, "Terrorist_WalkingOnStairs");
    E_sceneObject_setPosition  (s_terroristWalkingOnStairs, {3.400f, 0.54f, -12.96f});
    E_sceneObject_setRotation  (s_terroristWalkingOnStairs, {-3.00f, 89.00f,   0.00f});
    E_sceneObject_setScale     (s_terroristWalkingOnStairs, 3.21f);
    E_sceneObject_setCastShadow(s_terroristWalkingOnStairs, 1);
    if (s_terroristWalkingOnStairs->animator)
        E_animator_setClipByName(s_terroristWalkingOnStairs->animator, "Animation");

    s_terrorist2IdleWithRifle = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist2_Idle_With_Rifle.glb", &engineScene, "Terrorist2_Idle_With_Rifle");
    E_sceneObject_setPosition  (s_terrorist2IdleWithRifle, { -2.37f,    4.92f, -8.41f});
    E_sceneObject_setRotation  (s_terrorist2IdleWithRifle, { 31.00f, 0.0f,  0.00f});
    E_sceneObject_setScale     (s_terrorist2IdleWithRifle, 2.70f);
    E_sceneObject_setCastShadow(s_terrorist2IdleWithRifle, 1);
    if (s_terrorist2IdleWithRifle->animator)
        E_animator_setClipByName(s_terrorist2IdleWithRifle->animator, "Animation");

    s_terrorist2WalkingWithRifle = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist1_Walking_With_Gun.glb", &engineScene, "Terrorist2_Walking_With_Rifle");
    E_sceneObject_setPosition  (s_terrorist2WalkingWithRifle, { -2.37f,    4.92f, -8.41f});
    E_sceneObject_setRotation  (s_terrorist2WalkingWithRifle, { 31.00f, 0.0f,  0.00f});
    E_sceneObject_setScale     (s_terrorist2WalkingWithRifle, 2.70f);
    E_sceneObject_setActive    (s_terrorist2WalkingWithRifle, 0);
    E_sceneObject_setCastShadow(s_terrorist2WalkingWithRifle, 1);
    if (s_terrorist2WalkingWithRifle->animator)
    {
        E_animator_setClipByName(s_terrorist2WalkingWithRifle->animator, "Animation");
    }

    s_terroristStairsAK47 = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "AK47scaled_Stairs");
    E_sceneObject_setCastShadowAll(s_terroristStairsAK47, 1);
    E_sceneObject_setScale   (s_terroristStairsAK47, 2.62f);
    TajDinning_attachGunToRightHand(s_terroristWalkingOnStairs, s_terroristStairsAK47, s_terroristStairsAK47PosOffset, {0.0f, 172.0f, 0.0f});
    E_sceneObject_setActive(s_terroristStairsAK47, 0);

    s_terroristIdleAK47 = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "AK47scaled_Idle");
    E_sceneObject_setCastShadowAll(s_terroristIdleAK47, 1);
    E_sceneObject_setScale   (s_terroristIdleAK47, 2.31f);
    TajDinning_attachGunToRightHand(s_terrorist2IdleWithRifle, s_terroristIdleAK47, s_terroristIdleAK47PosOffset, {-63.0f, -100.0f, 0.0f});
    E_sceneObject_setActive(s_terroristIdleAK47, 0);

    static const float s_deadBodyPos[DEAD_WAITRESS_COUNT][3] = {
        { 2.30f,  0.00f,  2.30f },
        {-1.20f,  0.00f, -1.50f },
        { 0.90f,  0.00f, -6.20f },
        { 6.00f,  0.00f, -3.90f },
        { 6.30f,  0.00f,  1.90f },
    };
    static const float s_deadBodyRotY[DEAD_WAITRESS_COUNT] = { -37.0f, 31.0f, -249.0f, -330.0f, -555.0f };

    for (int i = 0; i < DEAD_WAITRESS_COUNT; i++)
    {
        char name[32];
        snprintf(name, sizeof(name), "deadbody%d", i + 1);
        s_deadBodies[i] = E_loadModel("DEMO_ASSETS/03-TajDinningToTVBlast/DeadWaitress.glb", &engineScene, name);
        E_sceneObject_setPosition(s_deadBodies[i], {s_deadBodyPos[i][0], s_deadBodyPos[i][1], s_deadBodyPos[i][2]});
        E_sceneObject_setRotation(s_deadBodies[i], {0.0f, s_deadBodyRotY[i], 0.0f});
        E_sceneObject_setScale   (s_deadBodies[i], 1.0f);
        E_sceneObject_setCastShadowAll(s_deadBodies[i], 1);
        if (s_deadBodies[i]->animator)
        {
            E_animator_setClipByName(s_deadBodies[i]->animator, "Armature|mixamo.com|Layer0");
            E_animator_setSpeed(s_deadBodies[i]->animator, 0.0f);
        }
    }

    s_emitterFire1 = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName     (s_emitterFire1, "TajFire1");
    E_emitter_setPosition (s_emitterFire1, {-50.87f, 4.55f, 2.91f});
    E_emitter_setDirection(s_emitterFire1, {-0.19f, 1.00f, 1.00f});
    E_emitter_setSpread   (s_emitterFire1, 44.20f);
    E_emitter_setSpawnRate(s_emitterFire1, 100.34f);
    E_emitter_setLifespan (s_emitterFire1, 1.02f);
    E_emitter_setSpeed    (s_emitterFire1, 1.02f);
    E_emitter_setSize     (s_emitterFire1, 1.03f, 4.96f);
    E_emitter_setColor    (s_emitterFire1, 1.00f, 1.00f, 0.00f, 1.00f);
    E_emitter_setOpacity  (s_emitterFire1, 0.13f);
    E_emitter_setPan      (s_emitterFire1, 0.00f, 0.02f, 0.50f);
    E_emitter_setTextureType(s_emitterFire1, (E_ParticleTextureType)1);

    s_emitterSmoke1 = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName     (s_emitterSmoke1, "TajSmoke1");
    E_emitter_setPosition (s_emitterSmoke1, {-49.62f, 3.64f, 2.08f});
    E_emitter_setDirection(s_emitterSmoke1, {0.00f, 1.00f, 0.50f});
    E_emitter_setSpread   (s_emitterSmoke1, 55.40f);
    E_emitter_setSpawnRate(s_emitterSmoke1, 14.86f);
    E_emitter_setLifespan (s_emitterSmoke1, 8.30f);
    E_emitter_setSpeed    (s_emitterSmoke1, 1.02f);
    E_emitter_setSize     (s_emitterSmoke1, 0.82f, 7.29f);
    E_emitter_setColor    (s_emitterSmoke1, 0.10f, 0.10f, 0.10f, 1.00f);
    E_emitter_setOpacity  (s_emitterSmoke1, 0.55f);
    E_emitter_setPan      (s_emitterSmoke1, 0.00f, -0.06f, 0.50f);
    E_emitter_setTextureType(s_emitterSmoke1, (E_ParticleTextureType)0);

    s_emitterSmoke2 = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName        (s_emitterSmoke2, "Smoke2_TajInside");
    E_emitter_setPosition    (s_emitterSmoke2, {5.98f, 0.45f, -2.21f});
    E_emitter_setDirection   (s_emitterSmoke2, {0.24f, 0.42f, 0.87f});
    E_emitter_setSpread      (s_emitterSmoke2, 17.19f);
    E_emitter_setSpawnRate   (s_emitterSmoke2, 10.00f);
    E_emitter_setLifespan    (s_emitterSmoke2, 2.00f);
    E_emitter_setSpeed       (s_emitterSmoke2, 1.46f);
    E_emitter_setSize        (s_emitterSmoke2, 0.64f, 4.62f);
    E_emitter_setColor       (s_emitterSmoke2, 1.00f, 1.00f, 1.00f, 1.00f);
    E_emitter_setOpacity     (s_emitterSmoke2, 0.35f);
    E_emitter_setPan         (s_emitterSmoke2, 0.00f, 1.00f, 0.50f);
    E_emitter_setTextureType (s_emitterSmoke2, (E_ParticleTextureType)1);

    s_emitterBlackSmoke = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName        (s_emitterBlackSmoke, "BlackSmoke_TajInside");
    E_emitter_setPosition    (s_emitterBlackSmoke, {-3.63f, 8.78f, 5.72f});
    E_emitter_setDirection   (s_emitterBlackSmoke, {0.57f, 0.16f, -0.81f});
    E_emitter_setSpread      (s_emitterBlackSmoke, 17.19f);
    E_emitter_setSpawnRate   (s_emitterBlackSmoke, 10.00f);
    E_emitter_setLifespan    (s_emitterBlackSmoke, 3.12f);
    E_emitter_setSpeed       (s_emitterBlackSmoke, 0.76f);
    E_emitter_setSize        (s_emitterBlackSmoke, 1.29f, 7.12f);
    E_emitter_setColor       (s_emitterBlackSmoke, 0.00f, 0.00f, 0.00f, 1.00f);
    E_emitter_setOpacity     (s_emitterBlackSmoke, 0.67f);
    E_emitter_setPan         (s_emitterBlackSmoke, 0.00f, -0.03f, 0.50f);
    E_emitter_setTextureType (s_emitterBlackSmoke, (E_ParticleTextureType)0);

    s_emitterSmoke = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName        (s_emitterSmoke, "Smoke_TajInside");
    E_emitter_setPosition    (s_emitterSmoke, {-4.94f, 1.77f, -0.22f});
    E_emitter_setDirection   (s_emitterSmoke, {0.79f, 0.00f, -0.62f});
    E_emitter_setSpread      (s_emitterSmoke, 17.19f);
    E_emitter_setSpawnRate   (s_emitterSmoke, 34.16f);
    E_emitter_setLifespan    (s_emitterSmoke, 6.40f);
    E_emitter_setSpeed       (s_emitterSmoke, 1.52f);
    E_emitter_setSize        (s_emitterSmoke, 0.41f, 10.00f);
    E_emitter_setColor       (s_emitterSmoke, 0.12f, 0.12f, 0.12f, 1.00f);
    E_emitter_setOpacity     (s_emitterSmoke, 0.78f);
    E_emitter_setPan         (s_emitterSmoke, 0.00f, 1.00f, 0.50f);
    E_emitter_setTextureType (s_emitterSmoke, (E_ParticleTextureType)0);

    s_emitterFire = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName        (s_emitterFire, "Fire_TajInside");
    E_emitter_setPosition    (s_emitterFire, {-5.03f, 7.99f, -0.68f});
    E_emitter_setDirection   (s_emitterFire, {0.92f, 1.00f, -0.31f});
    E_emitter_setSpread      (s_emitterFire, 28.80f);
    E_emitter_setSpawnRate   (s_emitterFire, 12.05f);
    E_emitter_setLifespan    (s_emitterFire, 2.00f);
    E_emitter_setSpeed       (s_emitterFire, 2.66f);
    E_emitter_setSize        (s_emitterFire, 1.37f, 9.06f);
    E_emitter_setColor       (s_emitterFire, 1.00f, 1.00f, 1.00f, 1.00f);
    E_emitter_setOpacity     (s_emitterFire, 0.48f);
    E_emitter_setPan         (s_emitterFire, 0.00f, -0.22f, 0.50f);
    E_emitter_setTextureType (s_emitterFire, (E_ParticleTextureType)1);

    s_plTajFire = E_pointLight_addToScene(&engineScene,
        E_pointLight_create({-50.33f, 3.88f, 5.33f}, {1.00f, 1.00f, 0.24f}, 0.86f, 4.00f));
    E_pointLight_setName(s_plTajFire, "TajFirePointLight");

    {
        E_pointLight pl = E_pointLight_create({0.33f, 7.63f, -0.46f}, {1.00f, 1.00f, 0.61f}, 0.15f, 4.00f);
        s_pl_taj_1 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_1, "pointLight_tajDinning_1");
        s_pl_taj_1->flicker = 0; s_pl_taj_1->flickerSpeed = 3.000f; s_pl_taj_1->flickerAmount = 0.500f;
        s_pl_taj_1->applyAttenuation = 0;
    }
    {
        E_pointLight pl = E_pointLight_create({-6.31f, 1.76f, -10.46f}, {1.00f, 0.89f, 0.81f}, 1.06f, 2.95f);
        s_pl_taj_2 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_2, "pointLight_tajDinning_2");
        s_pl_taj_2->flicker = 0; s_pl_taj_2->flickerSpeed = 3.000f; s_pl_taj_2->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({-6.31f, 1.76f, 6.84f}, {1.00f, 0.89f, 0.81f}, 1.06f, 2.79f);
        s_pl_taj_3 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_3, "pointLight_tajDinning_3");
        s_pl_taj_3->flicker = 0; s_pl_taj_3->flickerSpeed = 3.000f; s_pl_taj_3->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({-5.43f, 2.12f, -0.34f}, {1.00f, 0.89f, 0.81f}, 0.25f, 2.53f);
        s_pl_taj_4 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_4, "pointLight_tajDinning_4");
        s_pl_taj_4->flicker = 0; s_pl_taj_4->flickerSpeed = 3.000f; s_pl_taj_4->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({-6.31f, 1.76f, -4.46f}, {1.00f, 0.89f, 0.81f}, 1.06f, 2.95f);
        s_pl_taj_5 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_5, "pointLight_tajDinning_5");
        s_pl_taj_5->flicker = 0; s_pl_taj_5->flickerSpeed = 3.000f; s_pl_taj_5->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({6.35f, 1.60f, 6.67f}, {1.00f, 0.89f, 0.81f}, 0.67f, 3.57f);
        s_pl_taj_6 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_6, "pointLight_tajDinning_6");
        s_pl_taj_6->flicker = 0; s_pl_taj_6->flickerSpeed = 3.000f; s_pl_taj_6->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({6.35f, 1.60f, 0.67f}, {1.00f, 0.89f, 0.81f}, 0.67f, 3.57f);
        s_pl_taj_7 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_7, "pointLight_tajDinning_7");
        s_pl_taj_7->flicker = 0; s_pl_taj_7->flickerSpeed = 3.000f; s_pl_taj_7->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({6.35f, 1.60f, -8.23f}, {1.00f, 0.89f, 0.81f}, 0.67f, 3.57f);
        s_pl_taj_8 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_8, "pointLight_tajDinning_8");
        s_pl_taj_8->flicker = 0; s_pl_taj_8->flickerSpeed = 3.000f; s_pl_taj_8->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({-0.05f, 1.13f, -4.75f}, {1.00f, 0.89f, 0.81f}, 0.67f, 4.46f);
        s_pl_taj_9 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_9, "pointLight_tajDinning_9");
        s_pl_taj_9->flicker = 0; s_pl_taj_9->flickerSpeed = 3.000f; s_pl_taj_9->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({7.11f, 8.50f, -7.61f}, {1.00f, 1.00f, 0.53f}, 0.49f, 10.00f);
        s_pl_taj_10 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_10, "pointLight_tajDinning_10");
        s_pl_taj_10->flicker = 1; s_pl_taj_10->flickerSpeed = 1.264f; s_pl_taj_10->flickerAmount = 0.500f;
    }
    // {
    //     E_pointLight pl = E_pointLight_create({7.11f, 8.50f, -7.61f}, {1.00f, 1.00f, 1.00f}, 1.06f, 10.00f);
    //     s_pl_taj_11 = E_pointLight_addToScene(&engineScene, pl);
    //     E_pointLight_setName(s_pl_taj_11, "pointLight_tajDinning_11");
    //     s_pl_taj_11->flicker = 1; s_pl_taj_11->flickerSpeed = 1.264f; s_pl_taj_11->flickerAmount = 0.500f;
    // }
    {
        E_pointLight pl = E_pointLight_create({0.11f, 2.77f, -8.33f}, {1.00f, 1.00f, 1.00f}, 0.11f, 10.00f);
        s_pl_taj_12 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_12, "pointLight_tajDinning_12");
        s_pl_taj_12->flicker = 0; s_pl_taj_12->flickerSpeed = 1.264f; s_pl_taj_12->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({-7.02f, 1.22f, 9.68f}, {0.79f, 0.90f, 1.00f}, 2.34f, 0.70f);
        s_pl_taj_tv = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_taj_tv, "pointLight_tv");
        s_pl_taj_tv->flicker = 1; s_pl_taj_tv->flickerSpeed = 3.000f; s_pl_taj_tv->flickerAmount = 0.500f;
    }
    {
        E_pointLight pointLight_chabutara = E_pointLight_create(
            {-1.74f, 2.11f, 2.66f},
            {1.00f, 0.67f, 0.38f},
            1.16f, 3.41f);
        s_pl_chabutara = E_pointLight_addToScene(&engineScene, pointLight_chabutara);
        E_pointLight_setName(s_pl_chabutara, "pointLight_chabutara");
        s_pl_chabutara->flicker       = 0;
        s_pl_chabutara->flickerSpeed  = 3.000f;
        s_pl_chabutara->flickerAmount = 0.500f;
    }

    {
        s_splineToTV = E_scene_addSpline(&engineScene, "splineToTV");
        E_spline* splineToTV = s_splineToTV;
        splineToTV->numPoints    = 15;
        splineToTV->points[0]  = vmath::vec3(-6.502f,  0.747f,  -1.042f);
        splineToTV->points[1]  = vmath::vec3(-6.502f,  0.747f,   0.992f);
        splineToTV->points[2]  = vmath::vec3(-6.502f,  0.748f,   1.595f);
        splineToTV->points[3]  = vmath::vec3(-6.502f,  0.750f,   2.885f);
        splineToTV->points[4]  = vmath::vec3(-6.502f,  0.751f,   3.323f);
        splineToTV->points[5]  = vmath::vec3(-6.502f,  0.751f,   3.584f);
        splineToTV->points[6]  = vmath::vec3(-6.502f,  0.752f,   3.935f);
        splineToTV->points[7]  = vmath::vec3(-6.502f,  0.752f,   4.340f);
        splineToTV->points[8]  = vmath::vec3(-6.502f,  0.753f,   4.886f);
        splineToTV->points[9]  = vmath::vec3(-6.502f,  0.754f,   5.264f);
        splineToTV->points[10] = vmath::vec3(-6.502f,  0.755f,   5.894f);
        splineToTV->points[11] = vmath::vec3(-6.528f,  0.894f,   7.081f);
        splineToTV->points[12] = vmath::vec3(-6.615f,  0.991f,   7.886f);
        splineToTV->points[13] = vmath::vec3(-7.118f,  1.186f,  10.469f);
        splineToTV->points[14] = vmath::vec3(-7.355f,  1.549f,  12.578f);
    }

    {
        s_splineToTVLookAt = E_scene_addSpline(&engineScene, "splineToTVLookAt");
        E_spline* splineToTVLookAt = s_splineToTVLookAt;
        splineToTVLookAt->numPoints   = 9;
        splineToTVLookAt->points[0]  = vmath::vec3( 0.118f,  6.649f,  -4.150f);
        splineToTVLookAt->points[1]  = vmath::vec3(-0.014f,  6.492f,  -3.944f);
        splineToTVLookAt->points[2]  = vmath::vec3(-3.951f,  2.559f,  -1.240f);
        splineToTVLookAt->points[3]  = vmath::vec3(-5.119f,  1.239f,   0.102f);
        splineToTVLookAt->points[4]  = vmath::vec3(-5.520f,  1.105f,   3.159f);
        splineToTVLookAt->points[5]  = vmath::vec3(-5.708f,  1.069f,   3.993f);
        splineToTVLookAt->points[6]  = vmath::vec3(-6.208f,  1.048f,   6.075f);
        splineToTVLookAt->points[7]  = vmath::vec3(-6.858f,  1.148f,   9.094f);
        splineToTVLookAt->points[8]  = vmath::vec3(-7.165f,  1.133f,   9.538f);
    }

    {
        s_splineDeadBodyCamMovement = E_scene_addSpline(&engineScene, "SplineDeadBodyCamMovement");
        s_splineDeadBodyCamMovement->numPoints    = 5;
        s_splineDeadBodyCamMovement->points[0] = vmath::vec3(-6.769f, 7.028f,  6.998f);
        s_splineDeadBodyCamMovement->points[1] = vmath::vec3(-6.630f, 6.228f,  7.884f);
        s_splineDeadBodyCamMovement->points[2] = vmath::vec3(-5.822f, 5.752f, 10.997f);
        s_splineDeadBodyCamMovement->points[3] = vmath::vec3(-4.306f, 5.952f, 12.226f);
        s_splineDeadBodyCamMovement->points[4] = vmath::vec3(-1.651f, 6.652f, 12.678f);
    }

    {
        s_splineDeadBodyCamFollowToTerrorist = E_scene_addSpline(&engineScene, "SplineDeadBodyCamFollowToTerrorist");
        s_splineDeadBodyCamFollowToTerrorist->numPoints    = 6;
        s_splineDeadBodyCamFollowToTerrorist->points[0] = vmath::vec3(-6.504f,  6.232f,   7.765f);
        s_splineDeadBodyCamFollowToTerrorist->points[1] = vmath::vec3( 6.959f,  0.464f,   2.816f);
        s_splineDeadBodyCamFollowToTerrorist->points[2] = vmath::vec3( 2.373f,  0.397f,   1.655f);
        s_splineDeadBodyCamFollowToTerrorist->points[3] = vmath::vec3( 0.360f,  0.349f,  -5.367f);
        s_splineDeadBodyCamFollowToTerrorist->points[4] = vmath::vec3( 2.443f,  3.131f, -11.664f);
        s_splineDeadBodyCamFollowToTerrorist->points[5] = vmath::vec3( 5.871f,  4.595f, -13.016f);
    }

    {
        s_splineTerroristToEvacuate = E_scene_addSpline(&engineScene, "splineTerroristToEvacuate");
        s_splineTerroristToEvacuate->numPoints    = 4;
        s_splineTerroristToEvacuate->points[0] = vmath::vec3(-6.000f,  2.000f,   0.000f);
        s_splineTerroristToEvacuate->points[1] = vmath::vec3( 2.443f,  3.131f, -11.664f);
        s_splineTerroristToEvacuate->points[2] = vmath::vec3(-3.083f,  7.787f, -12.441f);
        s_splineTerroristToEvacuate->points[3] = vmath::vec3(-7.022f,  7.773f, -15.129f);
    }

    {
        s_splineCctvStyle = E_scene_addSpline(&engineScene, "cctvStyleSpline");
        s_splineCctvStyle->numPoints    = 7;
        s_splineCctvStyle->points[0] = vmath::vec3( 5.197f,  5.891f, -2.346f);
        s_splineCctvStyle->points[1] = vmath::vec3( 4.163f,  6.168f, -1.598f);
        s_splineCctvStyle->points[2] = vmath::vec3(-3.222f,  9.568f,  3.393f);
        s_splineCctvStyle->points[3] = vmath::vec3(-3.095f, 11.181f,  2.686f);
        s_splineCctvStyle->points[4] = vmath::vec3(-1.670f, 12.978f,  0.024f);
        s_splineCctvStyle->points[5] = vmath::vec3( 4.918f, 12.978f,  0.997f);
        s_splineCctvStyle->points[6] = vmath::vec3( 6.846f, 12.978f,  0.521f);
    }

    {
        s_splineCctvStyleLookAt = E_scene_addSpline(&engineScene, "cctvStyleSpline_camLookAt");
        s_splineCctvStyleLookAt->numPoints    = 6;
        s_splineCctvStyleLookAt->points[0] = vmath::vec3( 7.712f,  4.971f, -11.216f);
        s_splineCctvStyleLookAt->points[1] = vmath::vec3( 7.599f,  5.034f, -10.158f);
        s_splineCctvStyleLookAt->points[2] = vmath::vec3( 7.751f,  5.272f,  -4.778f);
        s_splineCctvStyleLookAt->points[3] = vmath::vec3( 7.667f,  8.343f,  -5.329f);
        s_splineCctvStyleLookAt->points[4] = vmath::vec3(-1.142f, 11.858f, -14.733f);
        s_splineCctvStyleLookAt->points[5] = vmath::vec3(-3.363f, 11.824f, -15.476f);
    }

    {
        E_spotLight sl = E_spotLight_create(
            {-5.50f, 3.07f, 8.35f},
            {-0.54f, -0.80f, 0.28f},
            {1.00f, 1.00f, 1.00f},
            5.00f,
            15.00f, 20.00f,
            4.00f);
        sl.flicker = 0; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f;
        sl.castShadow = 1;
        s_sl_tvSpotlight = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_tvSpotlight, "tvSpotlight");
    }

    {
        E_spotLight spotLightTajLeft = E_spotLight_create(
            {-55.22f, -2.46f, 7.75f},
            {-0.16f, 0.96f, -0.24f},
            {0.99f, 0.62f, 0.21f},
            0.5f,
            26.50f, 50.40f,
            9.70f);
        spotLightTajLeft.flicker       = 0;
        spotLightTajLeft.flickerSpeed  = 3.000f;
        spotLightTajLeft.flickerAmount = 0.500f;
        spotLightTajLeft.castShadow    = 0;
        s_sl_tajLeft = E_spotLight_addToScene(&engineScene, spotLightTajLeft);
        E_spotLight_setName(s_sl_tajLeft, "spotLightTajLeft");
    }

    {
        E_spotLight spotLightTajRight = E_spotLight_create(
            {-46.62f, -2.46f, 1.77f},
            {-0.27f, 0.87f, -0.41f},
            {0.99f, 0.62f, 0.21f},
            0.5f,
            26.50f, 50.40f,
            9.70f);
        spotLightTajRight.flicker       = 0;
        spotLightTajRight.flickerSpeed  = 3.000f;
        spotLightTajRight.flickerAmount = 0.500f;
        spotLightTajRight.castShadow    = 0;
        s_sl_tajRight = E_spotLight_addToScene(&engineScene, spotLightTajRight);
        E_spotLight_setName(s_sl_tajRight, "spotLightTajRight");
    }

    {
        E_spotLight spotLightPillar = E_spotLight_create(
            {-4.64f, 4.09f, 3.12f},
            {0.06f, -0.80f, -0.40f},
            {1.00f, 1.00f, 1.00f},
            8.52f,
            24.50f, 36.60f,
            4.32f);
        spotLightPillar.flicker       = 0;
        spotLightPillar.flickerSpeed  = 3.000f;
        spotLightPillar.flickerAmount = 0.500f;
        spotLightPillar.castShadow    = 0;
        s_sl_pillar = E_spotLight_addToScene(&engineScene, spotLightPillar);
        E_spotLight_setName(s_sl_pillar, "spotLightPillar");
    }

    {
        E_spotLight spotLightPillar2 = E_spotLight_create(
            {-4.34f, 4.09f, -3.18f},
            {0.06f, -0.80f, -0.40f},
            {1.00f, 1.00f, 1.00f},
            8.52f,
            18.90f, 25.50f,
            4.32f);
        spotLightPillar2.flicker       = 0;
        spotLightPillar2.flickerSpeed  = 3.000f;
        spotLightPillar2.flickerAmount = 0.500f;
        spotLightPillar2.castShadow    = 0;
        s_sl_pillar2 = E_spotLight_addToScene(&engineScene, spotLightPillar2);
        E_spotLight_setName(s_sl_pillar2, "spotLightPillar2");
    }

    {
        s_spline_2ndFloorWalk = E_scene_addSpline(&engineScene, "spline_2ndFloorWalk");
        s_spline_2ndFloorWalk->numPoints = 5;
        s_spline_2ndFloorWalk->points[0] = vmath::vec3( 5.127f,  7.342f,   6.334f);
        s_spline_2ndFloorWalk->points[1] = vmath::vec3( 5.146f,  7.342f,   5.523f);
        s_spline_2ndFloorWalk->points[2] = vmath::vec3( 5.960f,  7.342f, -10.892f);
        s_spline_2ndFloorWalk->points[3] = vmath::vec3( 2.184f,  7.342f, -15.080f);
        s_spline_2ndFloorWalk->points[4] = vmath::vec3(-7.800f,  7.342f, -13.195f);
    }

    {
        int i;
        char walkerName[32];
        for (i = 0; i < TAJ_WALKER_COUNT; i++)
        {
            vmath::vec3 startPos;
            sprintf(walkerName, "TAJ_WALKER_%d", i);
            s_tajWalkers[i] = E_loadModel("DEMO_ASSETS/Common/Generic human model/Walking/WalkingMan1.glb", &engineScene, walkerName);
            E_sceneObject_setScale(s_tajWalkers[i], 0.023f);
            startPos = E_spline_evaluate(s_spline_2ndFloorWalk, s_tajWalkerOffset[i]);
            E_sceneObject_setPosition(s_tajWalkers[i], startPos);
        }
    }

}

void TajDinning_begin()
{
    static int s_begun = 0;
    if (s_begun) return;
    s_begun = 1;

    E_postProcess_setFXAA(1);
    E_postProcess_setSaturation(1.12f);
    E_postProcess_setBloom(1, 1.23f, 10.62f);
    E_postProcess_setGodRays(0, 0.5f, 0.5f, 64, 0.3f, 0.97f, 0.9f, 0.5f);
    E_postProcess_setOverlay(0, 0);
    E_postProcess_setDissolve(0.0f);
    E_postProcess_setFadeColor(0.0f, 0.0f, 0.0f);
    E_fog_setEnabled(&engineScene.fog, 0);

    TajDinning_activate();
}

void TajDinning_activate()
{
    int i;
    if (s_tajInterior)    E_sceneObject_setActive(s_tajInterior,  1);
    if (s_tv)               E_sceneObject_setActive(s_tv,         1);
    if (s_skySphere)        E_sceneObject_setActive(s_skySphere,  1);
    if (s_taj)              E_sceneObject_setActive(s_taj,        1);
    if (s_terroristWalkingOnStairs) E_sceneObject_setActive(s_terroristWalkingOnStairs, 0);
    if (s_terrorist2IdleWithRifle)   E_sceneObject_setActive(s_terrorist2IdleWithRifle, 0);
    if (s_terrorist2WalkingWithRifle) E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 0);
    if (s_terroristStairsAK47) E_sceneObject_setActive(s_terroristStairsAK47, 0);
    if (s_terroristIdleAK47)   E_sceneObject_setActive(s_terroristIdleAK47,   0);
    for (i = 0; i < DEAD_WAITRESS_COUNT; i++)
        if (s_deadBodies[i]) E_sceneObject_setActive(s_deadBodies[i], 0);
    if (s_emitterFire1)             s_emitterFire1->active             = 0;
    if (s_emitterSmoke1)            s_emitterSmoke1->active            = 0;
    if (s_emitterSmoke2)     s_emitterSmoke2->active     = 0;
    if (s_emitterBlackSmoke) s_emitterBlackSmoke->active = 0;
    if (s_emitterSmoke)      s_emitterSmoke->active      = 0;
    if (s_emitterFire)       s_emitterFire->active       = 0;
    if (s_plTajFire) E_pointLight_deactivate(s_plTajFire);
    if (s_pl_taj_1)      E_pointLight_activate(s_pl_taj_1);
    if (s_pl_taj_2)      E_pointLight_activate(s_pl_taj_2);
    if (s_pl_taj_3)      E_pointLight_activate(s_pl_taj_3);
    if (s_pl_taj_4)      E_pointLight_activate(s_pl_taj_4);
    if (s_pl_taj_5)      E_pointLight_activate(s_pl_taj_5);
    if (s_pl_taj_6)      E_pointLight_activate(s_pl_taj_6);
    if (s_pl_taj_7)      E_pointLight_activate(s_pl_taj_7);
    if (s_pl_taj_8)      E_pointLight_activate(s_pl_taj_8);
    if (s_pl_taj_9)      E_pointLight_activate(s_pl_taj_9);
    if (s_pl_taj_10)     E_pointLight_activate(s_pl_taj_10);
    // if (s_pl_taj_11)     E_pointLight_activate(s_pl_taj_11);
    if (s_pl_taj_12)     E_pointLight_activate(s_pl_taj_12);
    if (s_pl_taj_tv)     E_pointLight_activate(s_pl_taj_tv);
    if (s_pl_chabutara)  E_pointLight_activate(s_pl_chabutara);
    if (s_sl_tvSpotlight) E_spotLight_activate(s_sl_tvSpotlight);
    if (s_sl_tajLeft)     E_spotLight_activate(s_sl_tajLeft);
    if (s_sl_tajRight)    E_spotLight_activate(s_sl_tajRight);
    if (s_sl_pillar)      E_spotLight_activate(s_sl_pillar);
    if (s_sl_pillar2)     E_spotLight_activate(s_sl_pillar2);
    for (i = 0; i < TAJ_WALKER_COUNT; i++)
        if (s_tajWalkers[i]) E_sceneObject_setActive(s_tajWalkers[i], 1);
}

void TajDinning_deactivate()
{
    int i;
    if (s_tajInterior)    E_sceneObject_setActive(s_tajInterior,  0);
    if (s_tv)               E_sceneObject_setActive(s_tv,         0);
    if (s_skySphere)        E_sceneObject_setActive(s_skySphere,  0);
    if (s_taj)              E_sceneObject_setActive(s_taj,        0);
    if (s_terroristWalkingOnStairs) E_sceneObject_setActive(s_terroristWalkingOnStairs, 0);
    if (s_terrorist2IdleWithRifle)   E_sceneObject_setActive(s_terrorist2IdleWithRifle, 0);
    if (s_terrorist2WalkingWithRifle) E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 0);
    if (s_terroristStairsAK47) E_sceneObject_setActive(s_terroristStairsAK47, 0);
    if (s_terroristIdleAK47)   E_sceneObject_setActive(s_terroristIdleAK47,   0);
    for (i = 0; i < DEAD_WAITRESS_COUNT; i++)
        if (s_deadBodies[i]) E_sceneObject_setActive(s_deadBodies[i], 0);
    if (s_emitterFire1)             s_emitterFire1->active             = 0;
    if (s_emitterSmoke1)            s_emitterSmoke1->active            = 0;
    if (s_emitterSmoke2)     s_emitterSmoke2->active     = 0;
    if (s_emitterBlackSmoke) s_emitterBlackSmoke->active = 0;
    if (s_emitterSmoke)      s_emitterSmoke->active      = 0;
    if (s_emitterFire)       s_emitterFire->active       = 0;
    if (s_plTajFire) E_pointLight_deactivate(s_plTajFire);
    if (s_pl_taj_1)      E_pointLight_deactivate(s_pl_taj_1);
    if (s_pl_taj_2)      E_pointLight_deactivate(s_pl_taj_2);
    if (s_pl_taj_3)      E_pointLight_deactivate(s_pl_taj_3);
    if (s_pl_taj_4)      E_pointLight_deactivate(s_pl_taj_4);
    if (s_pl_taj_5)      E_pointLight_deactivate(s_pl_taj_5);
    if (s_pl_taj_6)      E_pointLight_deactivate(s_pl_taj_6);
    if (s_pl_taj_7)      E_pointLight_deactivate(s_pl_taj_7);
    if (s_pl_taj_8)      E_pointLight_deactivate(s_pl_taj_8);
    if (s_pl_taj_9)      E_pointLight_deactivate(s_pl_taj_9);
    if (s_pl_taj_10)     E_pointLight_deactivate(s_pl_taj_10);
    // if (s_pl_taj_11)     E_pointLight_deactivate(s_pl_taj_11);
    if (s_pl_taj_12)     E_pointLight_deactivate(s_pl_taj_12);
    if (s_pl_taj_tv)     E_pointLight_deactivate(s_pl_taj_tv);
    if (s_pl_chabutara)  E_pointLight_deactivate(s_pl_chabutara);
    if (s_sl_tvSpotlight) E_spotLight_deactivate(s_sl_tvSpotlight);
    if (s_sl_tajLeft)     E_spotLight_deactivate(s_sl_tajLeft);
    if (s_sl_tajRight)    E_spotLight_deactivate(s_sl_tajRight);
    if (s_sl_pillar)      E_spotLight_deactivate(s_sl_pillar);
    if (s_sl_pillar2)     E_spotLight_deactivate(s_sl_pillar2);
    for (i = 0; i < TAJ_WALKER_COUNT; i++)
        if (s_tajWalkers[i]) E_sceneObject_setActive(s_tajWalkers[i], 0);
}

void TajDinning_updateWorld()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();
    int i;

    if (s_spline_2ndFloorWalk)
    {
        float walkSpeed = 0.025f;
        for (i = 0; i < TAJ_WALKER_COUNT; i++)
        {
            float t;
            if (!s_tajWalkers[i]) continue;
            t = fmodf(now * walkSpeed + s_tajWalkerOffset[i], 1.0f);
            E_sceneObject_setPosition(s_tajWalkers[i], E_spline_evaluate(s_spline_2ndFloorWalk, t));
            E_sceneObject_setRotation(s_tajWalkers[i], E_spline_getRotationAtT(s_spline_2ndFloorWalk, t));
        }
    }

    if (now > 203.2f)
    {
        for (i = 0; i < DEAD_WAITRESS_COUNT; i++)
        {
            if (s_deadBodies[i]) E_sceneObject_setActive(s_deadBodies[i], 1);
        }
    }
    else
    {
        for (i = 0; i < DEAD_WAITRESS_COUNT; i++)
        {
            if (s_deadBodies[i]) E_sceneObject_setActive(s_deadBodies[i], 0);
        }
    }

    if (now > 213.2f && now <= 222.2f)
    {
        if (s_terroristWalkingOnStairs)
        {
            E_sceneObject_setActive(s_terroristWalkingOnStairs, 1);
            if (s_terroristWalkingOnStairs->animator)
            {
                E_animator_setTime (s_terroristWalkingOnStairs->animator, 0.0f);
                E_animator_setSpeed(s_terroristWalkingOnStairs->animator, 0.0f);
            }
            TajDinning_attachGunToRightHand(s_terroristWalkingOnStairs, s_terroristStairsAK47, s_terroristStairsAK47PosOffset, {0.0f, 172.0f, 0.0f});
        }
    }
    else if (now > 224.0f && now <= 230.2f && s_terroristWalkingOnStairs)
    {
        if (s_terroristWalkingOnStairs->animator)
            E_animator_setSpeed(s_terroristWalkingOnStairs->animator, 0.25f);
        E_sceneObject_setActive(s_terrorist2IdleWithRifle, 1);
        TajDinning_attachGunToRightHand(s_terroristWalkingOnStairs, s_terroristStairsAK47, s_terroristStairsAK47PosOffset, {0.0f, 172.0f, 0.0f});
        TajDinning_attachGunToRightHand(s_terrorist2IdleWithRifle, s_terroristIdleAK47, s_terroristIdleAK47PosOffset, {-63.0f, -103.0f, 0.0f});
    }
    else if (s_terroristWalkingOnStairs)
    {
        if (s_terroristWalkingOnStairs->animator)
                E_animator_setSpeed(s_terroristWalkingOnStairs->animator, 0.0f);
        TajDinning_attachGunToRightHand(s_terroristWalkingOnStairs, s_terroristStairsAK47, s_terroristStairsAK47PosOffset, {0.0f, 172.0f, 0.0f});
        TajDinning_attachGunToRightHand(s_terrorist2IdleWithRifle, s_terroristIdleAK47, s_terroristIdleAK47PosOffset, {-63.0f, -103.0f, 0.0f});
    }

    if (now >= 232.0f && now <= 242.0f)
    {
        vmath::vec3 startPos = vmath::vec3(-0.96f, 0.90f, -3.29f);
        vmath::vec3 endPos   = vmath::vec3(-0.96f, 0.90f,  0.61f);
        vmath::vec3 startRot = vmath::vec3( 0.00f, 0.00f,  0.00f);
        vmath::vec3 endRot   = vmath::vec3(44.01f, -4.00f, 0.00f);
        float t = E_Utils_lerpBetweenTime(now, 232.0f, 242.0f, 0.0f, 1.0f);

        E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 1);
        E_sceneObject_setPosition(s_terrorist2WalkingWithRifle, vmath::vec3(
            startPos[0] + t * (endPos[0] - startPos[0]),
            startPos[1] + t * (endPos[1] - startPos[1]),
            startPos[2] + t * (endPos[2] - startPos[2])));
        E_sceneObject_setRotation(s_terrorist2WalkingWithRifle, vmath::vec3(
            startRot[0] + t * (endRot[0] - startRot[0]),
            startRot[1] + t * (endRot[1] - startRot[1]),
            startRot[2] + t * (endRot[2] - startRot[2])));
    }

    if (s_tvVideo && s_tv && now >= 170.0f)
    {
        E_video_update(s_tvVideo);
        GLuint frame = E_video_getTexture(s_tvVideo);
        E_sceneObject_setAlbedoAll(s_tv, frame);
    }

    // Fire and smoke appear at 2:57.5
    if (now >= 205.7f)
    {
        if (s_emitterFire1)      s_emitterFire1->active  = 1;
        if (s_emitterSmoke1)     s_emitterSmoke1->active = 1;
        if (s_plTajFire) E_pointLight_activate(s_plTajFire);
        if (s_sl_tajLeft)          E_spotLight_deactivate(s_sl_tajLeft);
        if (s_sl_tajRight)         E_spotLight_deactivate(s_sl_tajRight);

        static int s_fireShakeFired = 0;
        if (!s_fireShakeFired)
        {
            E_camera_addTrauma(0.8f);
            s_fireShakeFired = 1;
        }
    }

    // Inside emitters appear after the blast at 213.2s
    if (now >= 213.2f)
    {
        if (s_emitterSmoke2)     s_emitterSmoke2->active     = 1;
        if (s_emitterBlackSmoke) s_emitterBlackSmoke->active = 1;
        if (s_emitterSmoke)      s_emitterSmoke->active      = 1;
        if (s_emitterFire)       s_emitterFire->active       = 1;

        E_fog_setEnabled(&engineScene.fog, 1);
        E_fog_setColor(&engineScene.fog, {0.21f, 0.21f, 0.21f});
        E_fog_setDensity(&engineScene.fog, 0.02f);
        E_fog_setType(&engineScene.fog, 1);
        E_fog_setHeight(&engineScene.fog, 1.96f, 0.68f);
    }

    // Smoke_TajInside opacity fades from 0.78 to 0.25 during 223.2-226.2s
    if (now > 223.2f && now <= 226.2f)
    {
        if (s_emitterSmoke)
            E_emitter_setOpacity(s_emitterSmoke, E_Utils_lerpBetweenTime(now, 223.2f, 226.2f, 0.78f, 0.25f));
    }
}

void TajDinning_updateCamera()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    if (now > 159.2f && now <= 184.2f)
    {
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineToTVLookAt, 159.2f, 184.2f, 1.0f);
        E_camera_setPosition(E_spline_getPositionBetweenTime(s_splineToTV, 159.2f, 184.2f, 1.0f));
        E_camera_lookAtPoint(lookAt);
        E_postProcess_setDOF(1, 0.10f, 2.48f, 20.0f);
    }

    // eyelids close
    if (now > 208.0f && now <= 209.5f)
        E_postProcess_setBlinkOpen(E_Utils_lerpBetweenTime(now, 208.0f, 209.5f, 1.0f, 0.0f));
    if (now > 209.5f && now <= 213.0f)
        E_postProcess_setBlinkOpen(0.0f);

    // eyelids open then blink once
    if (now > 213.0f && now <= 213.5f)
        E_postProcess_setBlinkOpen(E_Utils_lerpBetweenTime(now, 213.0f, 213.5f, 0.0f, 1.0f));
    if (now > 213.5f && now <= 214.0f)
        E_postProcess_setBlinkOpen(E_Utils_lerpBetweenTime(now, 213.5f, 214.0f, 1.0f, 0.0f));
    if (now > 214.0f && now <= 214.5f)
        E_postProcess_setBlinkOpen(E_Utils_lerpBetweenTime(now, 214.0f, 214.5f, 0.0f, 1.0f));
    if (now > 214.5f)
        E_postProcess_disableBlink();

    // taj blast
    if (now > 203.2f && now <= 213.2f)
    {
        E_directionalLight_setDirection(&engineScene.directionalLight, {0.50f, -0.48f, -1.00f});
        E_directionalLight_setColor(&engineScene.directionalLight, {0.00f, 0.24f, 1.00f});
        E_directionalLight_setIntensity(&engineScene.directionalLight, 0.10f);

        E_ambientLight_setColor(&engineScene.ambientLight, {0.00f, 0.15f, 0.42f});
        E_ambientLight_setIntensity(&engineScene.ambientLight, 0.12f);

        E_fog_setEnabled(&engineScene.fog, 1);
        E_fog_setColor(&engineScene.fog, {0.00f, 0.00f, 0.00f});
        E_fog_setDensity(&engineScene.fog, 0.02f);
        E_fog_setType(&engineScene.fog, 0);

        E_setFOV(12.87f);
        E_camera_setPosition(-10.59f, -1.08f, 8.30f);
        vmath::vec3 lookAt = vmath::vec3(-45.303f, 4.108f, 3.939f);
        E_camera_lookAtPoint(lookAt);
        E_postProcess_applyDOF(lookAt, 5.0f, 5.0f);
    }

    // dead body cam
    if (now > 213.2f && now <= 223.2f)
    {
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineDeadBodyCamFollowToTerrorist, 213.2f, 223.2f, 1.0f);
        E_setFOV(30.0f);
        vmath::vec3 camPos = vmath::vec3(-1.712, 0.183, 3.548);
        E_camera_setPosition(camPos);
        E_camera_lookAtPoint(lookAt);
        E_postProcess_applyDOF(lookAt, 5.0f, 5.0f);
    }

    // fov lerp 30 -> 10
    if (now > 223.2f && now <= 226.2f)
        E_setFOV(E_Utils_lerpBetweenTime(now, 223.2f, 226.2f, 30.0f, 10.0f));

    // look at terrorist evacuation
    if (now > 226.2f && now <= 229.2f)
    {
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineTerroristToEvacuate, 226.2f, 229.2f, 1.0f);
        E_camera_lookAtPoint(lookAt);
        E_postProcess_applyDOF(lookAt, 5.0f, 5.0f);
    }

    // fixed shot
    if (now > 232.0f)
    {
        static int s_chabutaraOff_done = 0;
        if (!s_chabutaraOff_done) { s_chabutaraOff_done = 1; if (s_pl_chabutara) E_pointLight_deactivate(s_pl_chabutara); }
        E_setFOV(40.00f);
        E_camera_setPosition(-0.962f, 0.205f, 2.309f);
        E_camera_lookAtPoint(vmath::vec3(0.224f, 7.332f, 1.178f));
    }


    // 3:34 to 3:44  cctv style shot
    // if (now > 214.0f && now <= 224.0f)
    // {
    //     E_setFOV(40.00f);
    //     vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineCctvStyleLookAt, 214.0f, 224.0f, 1.0f);
    //     E_postProcess_applyDOF(lookAt, 5.0f, 10.00f);
    //     E_camera_setPosition(E_spline_getPositionBetweenTime(s_splineCctvStyle, 214.0f, 224.0f, 1.0f));
    //     E_camera_lookAtPoint(lookAt);
    //     E_postProcess_applyDOF(lookAt, 5.0f, 28.74f);
    //     E_fog_setEnabled(&engineScene.fog, 1);
    //     E_fog_setColor(&engineScene.fog, {0.50f, 0.50f, 0.50f});
    //     E_fog_setDensity(&engineScene.fog, 0.04f);
    //     E_fog_setType(&engineScene.fog, 1);
    //     E_fog_setHeight(&engineScene.fog, 7.72f, 1.00f);
    // }

}

void TajDinning_update()
{
    TajDinning_updateWorld();
    TajDinning_updateCamera();
}

void TajDinning_uninit()
{
    int i;
    if (s_tvVideo)    { E_video_destroy(s_tvVideo);                   s_tvVideo    = NULL; }
    if (s_tv)       { E_sceneObject_destroyResources(s_tv);       s_tv       = NULL; }
    if (s_tajInterior) { E_sceneObject_destroyResources(s_tajInterior); s_tajInterior = NULL; }
    if (s_terroristWalkingOnStairs)   { E_sceneObject_destroyResources(s_terroristWalkingOnStairs);   s_terroristWalkingOnStairs   = NULL; }
    if (s_terrorist2IdleWithRifle)    { E_sceneObject_destroyResources(s_terrorist2IdleWithRifle);    s_terrorist2IdleWithRifle    = NULL; }
    if (s_terrorist2WalkingWithRifle) { E_sceneObject_destroyResources(s_terrorist2WalkingWithRifle); s_terrorist2WalkingWithRifle = NULL; }
    if (s_terroristStairsAK47)        { E_sceneObject_destroyResources(s_terroristStairsAK47);        s_terroristStairsAK47        = NULL; }
    if (s_terroristIdleAK47)          { E_sceneObject_destroyResources(s_terroristIdleAK47);          s_terroristIdleAK47          = NULL; }
    for (i = 0; i < TAJ_WALKER_COUNT; i++)
    {
        if (s_tajWalkers[i]) { E_sceneObject_destroyResources(s_tajWalkers[i]); s_tajWalkers[i] = NULL; }
    }

    if (s_plTajFire) { E_pointLight_deactivate(s_plTajFire); s_plTajFire = NULL; }
    if (s_pl_taj_1)    { E_pointLight_deactivate(s_pl_taj_1);    s_pl_taj_1    = NULL; }
    if (s_pl_taj_2)    { E_pointLight_deactivate(s_pl_taj_2);    s_pl_taj_2    = NULL; }
    if (s_pl_taj_3)    { E_pointLight_deactivate(s_pl_taj_3);    s_pl_taj_3    = NULL; }
    if (s_pl_taj_4)    { E_pointLight_deactivate(s_pl_taj_4);    s_pl_taj_4    = NULL; }
    if (s_pl_taj_5)    { E_pointLight_deactivate(s_pl_taj_5);    s_pl_taj_5    = NULL; }
    if (s_pl_taj_6)    { E_pointLight_deactivate(s_pl_taj_6);    s_pl_taj_6    = NULL; }
    if (s_pl_taj_7)    { E_pointLight_deactivate(s_pl_taj_7);    s_pl_taj_7    = NULL; }
    if (s_pl_taj_8)    { E_pointLight_deactivate(s_pl_taj_8);    s_pl_taj_8    = NULL; }
    if (s_pl_taj_9)    { E_pointLight_deactivate(s_pl_taj_9);    s_pl_taj_9    = NULL; }
    if (s_pl_taj_10)   { E_pointLight_deactivate(s_pl_taj_10);   s_pl_taj_10   = NULL; }
    if (s_pl_taj_12)   { E_pointLight_deactivate(s_pl_taj_12);   s_pl_taj_12   = NULL; }
    if (s_pl_taj_tv)   { E_pointLight_deactivate(s_pl_taj_tv);   s_pl_taj_tv   = NULL; }
    if (s_pl_chabutara){ E_pointLight_deactivate(s_pl_chabutara);s_pl_chabutara= NULL; }

    if (s_sl_tvSpotlight){ E_spotLight_deactivate(s_sl_tvSpotlight); s_sl_tvSpotlight = NULL; }
    if (s_sl_tajLeft)    { E_spotLight_deactivate(s_sl_tajLeft);     s_sl_tajLeft     = NULL; }
    if (s_sl_tajRight)   { E_spotLight_deactivate(s_sl_tajRight);    s_sl_tajRight    = NULL; }
    if (s_sl_pillar)     { E_spotLight_deactivate(s_sl_pillar);      s_sl_pillar      = NULL; }
    if (s_sl_pillar2)    { E_spotLight_deactivate(s_sl_pillar2);     s_sl_pillar2     = NULL; }
}
