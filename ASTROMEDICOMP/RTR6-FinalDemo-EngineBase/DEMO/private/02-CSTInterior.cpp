#include "../public/02-CSTInterior.h"
#include "../public/DEMO.hpp"
#include "engineAPIs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "E_utils.h"
#include "E_spline.h"
#include "E_animation.h"
#include "E_material.h"
#include "E_water.h"
#include <GL/glew.h>

static E_sceneObject* s_cstInterior = NULL;
static E_sceneObject* s_child = NULL;
static E_sceneObject* s_papa = NULL;
static E_sceneObject* s_papaDead = NULL;
static E_emitter* s_emitterTrackSmoke = NULL;
static E_emitter* s_emitterTrackSmoke2 = NULL;
static E_emitter* s_emitterTrackSmoke3 = NULL;
static E_emitter* s_emitterBlackSmoke = NULL;
static E_emitter* s_emitterFire = NULL;
static E_emitter* s_emitterBlackSmoke2 = NULL;
static E_emitter* s_emitterBlackSmoke3 = NULL;
static E_emitter* s_emitterBlackSmoke4 = NULL;
static E_pointLight*  s_pointLight_train          = NULL;
static E_pointLight*  s_pointLight_train2         = NULL;
static E_pointLight*  s_pointLight_train3         = NULL;
static E_pointLight*  s_pointLight_train4         = NULL;
static E_spline*      s_splineToTrain             = NULL;
static E_spline*      s_splineToChild             = NULL;
static E_spline*      s_cameraSplineToCSTBoard     = NULL;
static E_spline*      s_cameraLookAtSplineToKid    = NULL;
static int            s_afterAttack                = 0;
static GLuint         s_overlayTex                 = 0;

static E_pointLight*  s_pl_0         = NULL;
static E_pointLight*  s_pl_1         = NULL;
static E_pointLight*  s_pl_2         = NULL;
static E_pointLight*  s_pl_3         = NULL;
static E_pointLight*  s_pl_4         = NULL;
static E_pointLight*  s_pl_fire      = NULL;
static E_spotLight*   s_sl_0         = NULL;
static E_spotLight*   s_sl_1         = NULL;
static E_spotLight*   s_sl_2         = NULL;
static E_spotLight*   s_sl_3         = NULL;
static E_spotLight*   s_sl_leftside  = NULL;
static E_spotLight*   s_sl_leftside2 = NULL;
static E_spotLight*   s_sl_leftside3 = NULL;
static E_spotLight*   s_sl_leftside4 = NULL;
static E_spotLight*   s_sl_leftside5 = NULL;
static E_spotLight*   s_sl_kidSeat       = NULL;
static E_spotLight*   s_sl_child         = NULL;
static E_spotLight*   s_sl_papawadwas    = NULL;
static E_spline*      s_splineRunningMen  = NULL;
static const int      RUNNER_COUNT        = 7;
static E_sceneObject* s_runners[RUNNER_COUNT];
static float          s_runnersT[RUNNER_COUNT];

static const float RUNNER_X_OFFSET[RUNNER_COUNT] = {  0.0f,  0.3f, -0.3f,  0.15f, -0.15f,  0.0f,  0.3f };
static const float RUNNER_T_START [RUNNER_COUNT] = {  0.0f,  0.1f,   0.2f,  0.3f,   0.4f,  0.5f,  0.6f };

static void CSTInterior_applyAfterAttackState(int afterAttack);

void CSTInterior_init()
{
    s_cstInterior = E_loadModel("DEMO_ASSETS/02-CSTInterior/CSMTPlaform_v3.glb",            &engineScene, "s_cstInterior");
    s_child     = E_loadModel("DEMO_ASSETS/02-CSTInterior/ChildCST.glb",                       &engineScene, "s_child");
    s_papa      = E_loadModel("DEMO_ASSETS/02-CSTInterior/CST_FatherTalking.glb",             &engineScene, "s_papa");
    s_papaDead = E_loadModel("DEMO_ASSETS/02-CSTInterior/CST_FatherDeathFromTheRight.glb",   &engineScene, "s_papaDead");
    E_sceneObject_setPosition    (s_child, { 42.30f,  1.47f, -24.02f});
    E_sceneObject_setRotation    (s_child, {  0.00f, 160.00f,  0.00f});
    E_sceneObject_setScale       (s_child, 0.80f);
    E_sceneObject_setCastShadowAll(s_child, 1);

    E_sceneObject_setPosition    (s_papa, { 43.53f, 0.45f, -25.46f});
    E_sceneObject_setRotation    (s_papa, {  0.00f, -110.00f, 0.00f});
    E_sceneObject_setScale       (s_papa, 4.40f);
    E_sceneObject_setCastShadowAll(s_papa, 1);
    if (s_papa->animator) E_animator_setClipByName(s_papa->animator, "Animation");

    E_sceneObject_setPosition    (s_papaDead, { 43.83f, 0.45f, -25.46f});
    E_sceneObject_setRotation    (s_papaDead, {  0.00f, -110.00f, 0.00f});
    E_sceneObject_setScale       (s_papaDead, 4.40f);
    E_sceneObject_setCastShadowAll(s_papaDead, 1);
    if (s_papaDead->animator)
    {
        E_animator_setClipByName(s_papaDead->animator, "Animation");
        E_animator_setTime      (s_papaDead->animator, 0.0f);
        E_animator_setSpeed     (s_papaDead->animator, 0.0f);
    }

    {
        E_spotLight sl = E_spotLight_create(
            { 42.71f, 1.01f, -25.16f},
            { -0.25f, 0.67f,   0.70f},
            {  1.00f, 1.00f,   1.00f},
            1.38f,
            15.00f, 20.00f,
            4.00f);
        sl.flicker = 1; sl.flickerSpeed = 5.389f; sl.flickerAmount = 0.519f; sl.castShadow = 0;
        s_sl_child = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_child, "spotLight_child");
    }

    {
        E_spotLight sl = E_spotLight_create(
            { 43.86f,  4.33f, -27.49f},
            { -0.20f, -0.51f,   0.84f},
            {  1.00f,  1.00f,   1.00f},
            0.53f,
            7.00f, 11.80f,
            9.49f);
        sl.flicker = 1; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_papawadwas = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_papawadwas, "spotLight_papawadwas");
    }

    s_overlayTex = E_loadTexture("DEMO_ASSETS/02-CSTInterior/2611_Text.png");

    s_emitterTrackSmoke = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName     (s_emitterTrackSmoke, "s_emitterTrackSmoke");
    E_emitter_setPosition (s_emitterTrackSmoke, {47.75f, 3.82f, -21.52f});
    E_emitter_setDirection(s_emitterTrackSmoke, {-1.00f, -0.04f,   0.06f});
    E_emitter_setSpread   (s_emitterTrackSmoke, 17.19f);
    E_emitter_setSpawnRate(s_emitterTrackSmoke, 13.29f);
    E_emitter_setLifespan (s_emitterTrackSmoke, 5.49f);
    E_emitter_setSpeed    (s_emitterTrackSmoke, 3.00f);
    E_emitter_setSize     (s_emitterTrackSmoke, 1.11f, 10.00f);
    E_emitter_setColor    (s_emitterTrackSmoke, 0.92f, 1.00f, 0.78f, 1.00f);
    E_emitter_setOpacity  (s_emitterTrackSmoke, 0.14f);
    E_emitter_setPan      (s_emitterTrackSmoke, 0.00f, 1.00f, 0.50f);
    E_emitter_setTextureType(s_emitterTrackSmoke, (E_ParticleTextureType)0);
    s_emitterTrackSmoke->active = 0;

    s_emitterTrackSmoke2 = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName     (s_emitterTrackSmoke2, "s_emitterTrackSmoke2");
    E_emitter_setPosition (s_emitterTrackSmoke2, {42.71f, -0.96f, 1.58f});
    E_emitter_setDirection(s_emitterTrackSmoke2, {0.50f, 0.45f, 0.72f});
    E_emitter_setSpread   (s_emitterTrackSmoke2, 17.19f);
    E_emitter_setSpawnRate(s_emitterTrackSmoke2, 14.18f);
    E_emitter_setLifespan (s_emitterTrackSmoke2, 5.49f);
    E_emitter_setSpeed    (s_emitterTrackSmoke2, 3.00f);
    E_emitter_setSize     (s_emitterTrackSmoke2, 1.11f, 5.25f);
    E_emitter_setColor    (s_emitterTrackSmoke2, 1.00f, 1.00f, 1.00f, 1.00f);
    E_emitter_setOpacity  (s_emitterTrackSmoke2, 0.05f);
    E_emitter_setPan      (s_emitterTrackSmoke2, 0.00f, 1.00f, 0.50f);
    E_emitter_setTextureType(s_emitterTrackSmoke2, (E_ParticleTextureType)0);

    s_emitterTrackSmoke3 = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName     (s_emitterTrackSmoke3, "s_emitterTrackSmoke3");
    E_emitter_setPosition (s_emitterTrackSmoke3, {31.29f, 1.11f, -0.41f});
    E_emitter_setDirection(s_emitterTrackSmoke3, {0.84f, 0.22f, 0.49f});
    E_emitter_setSpread   (s_emitterTrackSmoke3, 8.00f);
    E_emitter_setSpawnRate(s_emitterTrackSmoke3, 5.94f);
    E_emitter_setLifespan (s_emitterTrackSmoke3, 5.49f);
    E_emitter_setSpeed    (s_emitterTrackSmoke3, 3.00f);
    E_emitter_setSize     (s_emitterTrackSmoke3, 0.85f, 5.87f);
    E_emitter_setColor    (s_emitterTrackSmoke3, 1.00f, 1.00f, 1.00f, 1.00f);
    E_emitter_setOpacity  (s_emitterTrackSmoke3, 0.05f);
    E_emitter_setPan      (s_emitterTrackSmoke3, -0.44f, -0.80f, 1.58f);
    E_emitter_setTextureType(s_emitterTrackSmoke3, (E_ParticleTextureType)0);

    E_spotLight spotLight_0 = E_spotLight_create(
        {38.22f, 7.90f, -29.73f},
        {0.00f, -1.00f, 0.00f},
        {1.00f, 0.96f, 0.62f},
        5.00f,
        0.00f, 27.90f,
        10.00f);
    spotLight_0.flicker       = 0;
    spotLight_0.flickerSpeed  = 0.898f;
    spotLight_0.flickerAmount = 0.500f;
    s_sl_0 = E_spotLight_addToScene(&engineScene, spotLight_0);
    E_spotLight_setName(s_sl_0, "spotLight_0");

    E_spotLight spotLight_1 = E_spotLight_create(
        {38.22f, 7.90f, -0.63f},
        {0.00f, -1.00f, 0.00f},
        {1.00f, 0.96f, 0.62f},
        4.45f,
        0.00f, 57.90f,
        10.00f);
    spotLight_1.flicker       = 1;
    spotLight_1.flickerSpeed  = 0.898f;
    spotLight_1.flickerAmount = 0.500f;
    s_sl_1 = E_spotLight_addToScene(&engineScene, spotLight_1);
    E_spotLight_setName(s_sl_1, "spotLight_1");

    E_spotLight spotLight_2 = E_spotLight_create(
        {28.42f, 7.90f, -10.53f},
        {0.00f, -1.00f, 0.00f},
        {1.00f, 0.96f, 0.62f},
        3.77f,
        0.00f, 57.90f,
        10.00f);
    spotLight_2.flicker       = 1;
    spotLight_2.flickerSpeed  = 0.898f;
    spotLight_2.flickerAmount = 0.500f;
    s_sl_2 = E_spotLight_addToScene(&engineScene, spotLight_2);
    E_spotLight_setName(s_sl_2, "spotLight_2");

    E_spotLight spotLight_3 = E_spotLight_create(
        {44.92f, 7.10f, -10.53f},
        {0.00f, -1.00f, 0.00f},
        {1.00f, 0.96f, 0.62f},
        4.48f,
        0.00f, 57.90f,
        10.00f);
    spotLight_3.flicker       = 1;
    spotLight_3.flickerSpeed  = 0.898f;
    spotLight_3.flickerAmount = 0.500f;
    s_sl_3 = E_spotLight_addToScene(&engineScene, spotLight_3);
    E_spotLight_setName(s_sl_3, "spotLight_3");

    E_ambientLight_setColor(&engineScene.ambientLight, {1.00f, 1.00f, 0.61f});
    E_ambientLight_setIntensity(&engineScene.ambientLight, 0.34f);

    E_pointLight pointLight_0 = E_pointLight_create(
        {38.42f, 3.64f, 11.07f},
        {1.00f, 0.00f, 0.00f},
        0.51f, 6.14f);
    s_pl_0 = E_pointLight_addToScene(&engineScene, pointLight_0);
    E_pointLight_setName(s_pl_0, "pointLight_0");
    s_pl_0->flicker       = 0;
    s_pl_0->flickerSpeed  = 3.000f;
    s_pl_0->flickerAmount = 0.500f;

    E_pointLight pointLight_1 = E_pointLight_create(
        {38.87f, 5.57f, -13.42f},
        {1.00f, 0.00f, 0.00f},
        2.57f, 6.14f);
    s_pl_1 = E_pointLight_addToScene(&engineScene, pointLight_1);
    E_pointLight_setName(s_pl_1, "pointLight_1");
    s_pl_1->flicker       = 0;
    s_pl_1->flickerSpeed  = 3.000f;
    s_pl_1->flickerAmount = 0.500f;

    E_pointLight pointLight_2 = E_pointLight_create(
        {38.87f, 5.57f, -26.23f},
        {1.00f, 0.00f, 0.00f},
        2.57f, 6.14f);
    s_pl_2 = E_pointLight_addToScene(&engineScene, pointLight_2);
    E_pointLight_setName(s_pl_2, "pointLight_2");
    s_pl_2->flicker       = 0;
    s_pl_2->flickerSpeed  = 3.000f;
    s_pl_2->flickerAmount = 0.500f;

    E_pointLight pointLight_3 = E_pointLight_create(
        {38.87f, 5.57f, 12.07f},
        {1.00f, 0.00f, 0.00f},
        2.57f, 6.14f);
    s_pl_3 = E_pointLight_addToScene(&engineScene, pointLight_3);
    E_pointLight_setName(s_pl_3, "pointLight_3");
    s_pl_3->flicker       = 0;
    s_pl_3->flickerSpeed  = 3.000f;
    s_pl_3->flickerAmount = 0.500f;

    E_pointLight pointLight_4 = E_pointLight_create(
        {34.74f, 5.57f, -33.42f},
        {1.00f, 0.96f, 0.59f},
        0.46f, 10.00f);
    s_pl_4 = E_pointLight_addToScene(&engineScene, pointLight_4);
    E_pointLight_setName(s_pl_4, "pointLight_4");
    s_pl_4->flicker       = 0;
    s_pl_4->flickerSpeed  = 3.000f;
    s_pl_4->flickerAmount = 0.500f;

    E_pointLight pointLight_fire = E_pointLight_create(
        {30.67f, 1.56f, 38.01f},
        {1.00f, 0.92f, 0.11f},
        0.46f, 10.00f);
    s_pl_fire = E_pointLight_addToScene(&engineScene, pointLight_fire);
    E_pointLight_setName(s_pl_fire, "pointLight_fire");
    s_pl_fire->flicker       = 0;
    s_pl_fire->flickerSpeed  = 3.000f;
    s_pl_fire->flickerAmount = 0.500f;

    {
        E_pointLight pl = E_pointLight_create(
            {44.11f, 3.50f, -33.85f},
            {0.65f, 0.82f, 1.00f},
            0.97f, 4.00f);
        s_pointLight_train = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pointLight_train, "pointLight_train");
        s_pointLight_train->flicker       = 0;
        s_pointLight_train->flickerSpeed  = 26.200f;
        s_pointLight_train->flickerAmount = 0.600f;
    }

    {
        E_pointLight pl = E_pointLight_create(
            {43.81f, 4.48f, -29.38f},
            {0.65f, 0.82f, 1.00f},
            0.97f, 4.00f);
        s_pointLight_train2 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pointLight_train2, "pointLight_train_copy");
        s_pointLight_train2->flicker       = 0;
        s_pointLight_train2->flickerSpeed  = 32.000f;
        s_pointLight_train2->flickerAmount = 0.750f;
    }

    {
        E_pointLight pl = E_pointLight_create(
            {44.26f, 3.07f, -19.24f},
            {0.65f, 0.82f, 1.00f},
            0.97f, 4.00f);
        s_pointLight_train3 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pointLight_train3, "pointLight_train_copy_copy");
        s_pointLight_train3->flicker       = 0;
        s_pointLight_train3->flickerSpeed  = 37.800f;
        s_pointLight_train3->flickerAmount = 0.300f;
    }

    {
        E_pointLight pl = E_pointLight_create(
            {43.87f, 3.85f, -23.76f},
            {0.65f, 0.82f, 1.00f},
            0.97f, 4.00f);
        s_pointLight_train4 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pointLight_train4, "pointLight_train_copy_copy_copy");
        s_pointLight_train4->flicker       = 0;
        s_pointLight_train4->flickerSpeed  = 37.300f;
        s_pointLight_train4->flickerAmount = 0.450f;
    }

    E_spotLight spotLight_leftside = E_spotLight_create(
        {50.25f, 7.81f, -29.29f},
        {-0.05f, -0.83f, 0.55f},
        {1.00f, 0.97f, 0.80f},
        5.00f,
        25.00f, 51.10f,
        10.00f);
    spotLight_leftside.flicker       = 0;
    spotLight_leftside.flickerSpeed  = 3.000f;
    spotLight_leftside.flickerAmount = 0.500f;
    s_sl_leftside = E_spotLight_addToScene(&engineScene, spotLight_leftside);
    E_spotLight_setName(s_sl_leftside, "spotLight_leftside");

    E_spotLight spotLight_leftside2 = E_spotLight_create(
        {56.55f, 8.11f, -11.29f},
        {0.00f, -1.00f, 0.00f},
        {1.00f, 0.96f, 0.70f},
        5.00f,
        25.00f, 56.70f,
        10.00f);
    spotLight_leftside2.flicker       = 0;
    spotLight_leftside2.flickerSpeed  = 3.000f;
    spotLight_leftside2.flickerAmount = 0.500f;
    s_sl_leftside2 = E_spotLight_addToScene(&engineScene, spotLight_leftside2);
    E_spotLight_setName(s_sl_leftside2, "spotLight_leftside2");

    E_spotLight spotLight_leftside3 = E_spotLight_create(
        {52.55f, 8.11f, 8.91f},
        {0.00f, -1.00f, 0.00f},
        {1.00f, 0.96f, 0.70f},
        5.00f,
        25.00f, 56.70f,
        10.00f);
    spotLight_leftside3.flicker       = 0;
    spotLight_leftside3.flickerSpeed  = 3.000f;
    spotLight_leftside3.flickerAmount = 0.500f;
    s_sl_leftside3 = E_spotLight_addToScene(&engineScene, spotLight_leftside3);
    E_spotLight_setName(s_sl_leftside3, "spotLight_leftside3");

    E_spotLight spotLight_leftside4 = E_spotLight_create(
        {54.85f, 14.21f, 37.61f},
        {0.00f, -1.00f, 0.00f},
        {1.00f, 0.96f, 0.70f},
        5.00f,
        25.00f, 34.70f,
        10.00f);
    spotLight_leftside4.flicker       = 0;
    spotLight_leftside4.flickerSpeed  = 3.000f;
    spotLight_leftside4.flickerAmount = 0.500f;
    s_sl_leftside4 = E_spotLight_addToScene(&engineScene, spotLight_leftside4);
    E_spotLight_setName(s_sl_leftside4, "spotLight_leftside4");

    E_spotLight spotLight_leftside5 = E_spotLight_create(
        {59.55f, 8.51f, -27.79f},
        {0.00f, -1.00f, 0.06f},
        {1.00f, 0.82f, 0.38f},
        5.00f,
        25.00f, 49.60f,
        10.00f);
    spotLight_leftside5.flicker       = 0;
    spotLight_leftside5.flickerSpeed  = 3.000f;
    spotLight_leftside5.flickerAmount = 0.500f;
    s_sl_leftside5 = E_spotLight_addToScene(&engineScene, spotLight_leftside5);
    E_spotLight_setName(s_sl_leftside5, "spotLight_leftside5");

    s_emitterBlackSmoke = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName    (s_emitterBlackSmoke, "s_emitterBlackSmoke");
    E_emitter_setPosition (s_emitterBlackSmoke, {23.16f, 0.22f, -7.83f});
    E_emitter_setDirection(s_emitterBlackSmoke, {0.37f,  0.05f, 0.93f});
    E_emitter_setSpread  (s_emitterBlackSmoke, 17.19f);
    E_emitter_setSpawnRate(s_emitterBlackSmoke, 22.41f);
    E_emitter_setLifespan(s_emitterBlackSmoke, 5.29f);
    E_emitter_setSpeed   (s_emitterBlackSmoke, 3.00f);
    E_emitter_setSize    (s_emitterBlackSmoke, 3.02f, 3.32f);
    E_emitter_setColor   (s_emitterBlackSmoke, 0.00f, 0.00f, 0.00f, 1.00f);
    E_emitter_setOpacity (s_emitterBlackSmoke, 0.21f);
    E_emitter_setPan     (s_emitterBlackSmoke, 1.00f, -0.13f, 0.50f);
    E_emitter_setTextureType(s_emitterBlackSmoke, (E_ParticleTextureType)0);
    s_emitterBlackSmoke->active = 0;

    s_emitterFire = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName    (s_emitterFire, "s_emitterFire");
    E_emitter_setPosition (s_emitterFire, {23.16f, 0.22f, -7.83f});
    E_emitter_setDirection(s_emitterFire, {0.37f,  0.05f, 0.93f});
    E_emitter_setSpread  (s_emitterFire, 17.19f);
    E_emitter_setSpawnRate(s_emitterFire, 5.94f);
    E_emitter_setLifespan(s_emitterFire, 2.60f);
    E_emitter_setSpeed   (s_emitterFire, 3.10f);
    E_emitter_setSize    (s_emitterFire, 2.22f, 9.17f);
    E_emitter_setColor   (s_emitterFire, 1.00f, 0.97f, 0.54f, 1.00f);
    E_emitter_setOpacity (s_emitterFire, 1.00f);
    E_emitter_setPan     (s_emitterFire, 1.00f, -0.03f, 0.50f);
    E_emitter_setTextureType(s_emitterFire, (E_ParticleTextureType)1);
    s_emitterFire->active = 0;

    s_emitterBlackSmoke2 = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName    (s_emitterBlackSmoke2, "s_emitterBlackSmoke2");
    E_emitter_setPosition(s_emitterBlackSmoke2, {44.43f, -0.47f, 17.95f});
    E_emitter_setDirection(s_emitterBlackSmoke2, {-0.26f, 0.74f, 0.62f});
    E_emitter_setSpread  (s_emitterBlackSmoke2, 72.80f);
    E_emitter_setSpawnRate(s_emitterBlackSmoke2, 22.41f);
    E_emitter_setLifespan(s_emitterBlackSmoke2, 5.29f);
    E_emitter_setSpeed   (s_emitterBlackSmoke2, 3.00f);
    E_emitter_setSize    (s_emitterBlackSmoke2, 3.02f, 3.32f);
    E_emitter_setColor   (s_emitterBlackSmoke2, 0.00f, 0.00f, 0.00f, 1.00f);
    E_emitter_setOpacity (s_emitterBlackSmoke2, 0.21f);
    E_emitter_setPan     (s_emitterBlackSmoke2, 1.00f, -0.13f, 0.50f);
    E_emitter_setTextureType(s_emitterBlackSmoke2, (E_ParticleTextureType)0);
    s_emitterBlackSmoke2->active = 0;

    s_emitterBlackSmoke3 = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName    (s_emitterBlackSmoke3, "s_emitterBlackSmoke3");
    E_emitter_setPosition(s_emitterBlackSmoke3, {27.84f, 8.72f, -1.43f});
    E_emitter_setDirection(s_emitterBlackSmoke3, {0.09f, -0.56f, 0.82f});
    E_emitter_setSpread  (s_emitterBlackSmoke3, 44.60f);
    E_emitter_setSpawnRate(s_emitterBlackSmoke3, 22.41f);
    E_emitter_setLifespan(s_emitterBlackSmoke3, 5.29f);
    E_emitter_setSpeed   (s_emitterBlackSmoke3, 3.00f);
    E_emitter_setSize    (s_emitterBlackSmoke3, 3.02f, 3.32f);
    E_emitter_setColor   (s_emitterBlackSmoke3, 0.12f, 0.12f, 0.12f, 1.00f);
    E_emitter_setOpacity (s_emitterBlackSmoke3, 1.00f);
    E_emitter_setPan     (s_emitterBlackSmoke3, 1.00f, -0.13f, 0.50f);
    E_emitter_setTextureType(s_emitterBlackSmoke3, (E_ParticleTextureType)0);
    s_emitterBlackSmoke3->active = 0;

    s_emitterBlackSmoke4 = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setName    (s_emitterBlackSmoke4, "s_emitterBlackSmoke4");
    E_emitter_setPosition(s_emitterBlackSmoke4, {53.07f, 2.53f, -2.65f});
    E_emitter_setDirection(s_emitterBlackSmoke4, {-1.00f, -0.05f, 0.01f});
    E_emitter_setSpread  (s_emitterBlackSmoke4, 44.60f);
    E_emitter_setSpawnRate(s_emitterBlackSmoke4, 10.88f);
    E_emitter_setLifespan(s_emitterBlackSmoke4, 6.86f);
    E_emitter_setSpeed   (s_emitterBlackSmoke4, 3.00f);
    E_emitter_setSize    (s_emitterBlackSmoke4, 3.02f, 3.32f);
    E_emitter_setColor   (s_emitterBlackSmoke4, 0.12f, 0.12f, 0.12f, 1.00f);
    E_emitter_setOpacity (s_emitterBlackSmoke4, 1.00f);
    E_emitter_setPan     (s_emitterBlackSmoke4, 1.00f, -0.13f, 0.50f);
    E_emitter_setTextureType(s_emitterBlackSmoke4, (E_ParticleTextureType)0);
    s_emitterBlackSmoke4->active = 0;

    s_splineToTrain = E_scene_addSpline(&engineScene, "SplineToTrain");
    s_splineToTrain->numPoints  = 4;
    s_splineToTrain->points[0] = vmath::vec3(24.493f, 2.288f, -44.335f);
    s_splineToTrain->points[1] = vmath::vec3(35.327f, 2.288f, -43.900f);
    s_splineToTrain->points[2] = vmath::vec3(43.459f, 2.288f, -42.888f);
    s_splineToTrain->points[3] = vmath::vec3(43.989f, 2.288f, -42.856f);

    s_splineToChild = E_scene_addSpline(&engineScene, "SplineToChild");
    s_splineToChild->numPoints  = 4;
    s_splineToChild->points[0] = vmath::vec3(44.081f, 2.747f, -45.203f);
    s_splineToChild->points[1] = vmath::vec3(43.459f, 2.288f, -42.888f);
    s_splineToChild->points[2] = vmath::vec3(43.509f, 2.382f, -27.486f);
    s_splineToChild->points[3] = vmath::vec3(43.507f, 2.363f, -26.562f);

    s_cameraSplineToCSTBoard = E_scene_addSpline(&engineScene, "cameraSpline_toCSTBoard");
    s_cameraSplineToCSTBoard->numPoints  = 4;
    s_cameraSplineToCSTBoard->points[0] = vmath::vec3(28.971f, 2.408f, -35.370f);
    s_cameraSplineToCSTBoard->points[1] = vmath::vec3(30.819f, 2.408f, -35.322f);
    s_cameraSplineToCSTBoard->points[2] = vmath::vec3(38.234f, 2.630f, -28.265f);
    s_cameraSplineToCSTBoard->points[3] = vmath::vec3(38.439f, 2.867f, -28.066f);

    s_cameraLookAtSplineToKid = E_scene_addSpline(&engineScene, "cameraLookAtSpline_toKid");
    s_cameraLookAtSplineToKid->numPoints  = 4;
    s_cameraLookAtSplineToKid->points[0] = vmath::vec3(37.020f, 2.758f, -28.570f);
    s_cameraLookAtSplineToKid->points[1] = vmath::vec3(38.234f, 2.630f, -28.265f);
    s_cameraLookAtSplineToKid->points[2] = vmath::vec3(42.297f, 2.020f, -24.040f);
    s_cameraLookAtSplineToKid->points[3] = vmath::vec3(44.310f, 2.085f, -21.457f);

    {
        E_spotLight spotLight_kidSeat = E_spotLight_create(
            {33.84f, 4.86f, -32.08f},
            {0.77f, -0.36f, 0.52f},
            {1.00f, 0.69f, 0.51f},
            2.51f,
            47.60f, 56.80f,
            40.67f);
        spotLight_kidSeat.flicker       = 1;
        spotLight_kidSeat.flickerSpeed  = 3.703f;
        spotLight_kidSeat.flickerAmount = 0.500f;
        spotLight_kidSeat.castShadow    = 1;
        spotLight_kidSeat.active        = 0;
        s_sl_kidSeat = E_spotLight_addToScene(&engineScene, spotLight_kidSeat);
        E_spotLight_setName(s_sl_kidSeat, "spotLight_kidSeat");
    }

    s_splineRunningMen = E_scene_addSpline(&engineScene, "Spline_runningMen");
    s_splineRunningMen->numPoints  = 4;
    s_splineRunningMen->points[0] = vmath::vec3(40.243f, 0.000f, -45.710f);
    s_splineRunningMen->points[1] = vmath::vec3(40.049f, 0.000f, -41.669f);
    s_splineRunningMen->points[2] = vmath::vec3(40.163f, 0.000f, -16.815f);
    s_splineRunningMen->points[3] = vmath::vec3(39.955f, 0.000f, -12.814f);

    static const char* RUNNER_MODEL_PATHS[] = {
        // "DEMO_ASSETS/Common/Generic human model/Running/Shannon_Run.glb",
        "DEMO_ASSETS/Common/Generic human model/Running/Tony_Running_Low_Poly.glb",
        // "DEMO_ASSETS/Common/Generic human model/Running/Bryce_run.glb",
        // "DEMO_ASSETS/Common/Generic human model/Running/Jody_Running.glb",
        // "DEMO_ASSETS/Common/Generic human model/Running/Louise_Fastrun.glb",
        "DEMO_ASSETS/Common/Generic human model/Running/SprintingMan1.glb",
        "DEMO_ASSETS/Common/Generic human model/Running/RunningWoman.glb",
        "DEMO_ASSETS/Common/Generic human model/RunningNew/RunningGreenMan.glb",
        "DEMO_ASSETS/Common/Generic human model/RunningNew/RunningPinkMan.glb",
        "DEMO_ASSETS/Common/Generic human model/RunningNew/RunningRedMan.glb",
        "DEMO_ASSETS/Common/Generic human model/RunningNew/RunningYellowMan.glb",
    };

    static const int RUNNER_MODEL_PATHS_COUNT = sizeof(RUNNER_MODEL_PATHS) / sizeof(RUNNER_MODEL_PATHS[0]);
    for (int i = 0; i < RUNNER_COUNT; i++)
    {
        char name[32];
        snprintf(name, sizeof(name), "SprintingMan_%d", i);
        s_runnersT[i]  = RUNNER_T_START[i];
        int modelIdx = i % RUNNER_MODEL_PATHS_COUNT;
        s_runners[i]   = E_loadModel(RUNNER_MODEL_PATHS[modelIdx], &engineScene, name);
        E_sceneObject_setScale(s_runners[i], modelIdx == 0 ? 4.5f : 0.042f);
        s_runners[i]->active = 0;
        vmath::vec3 pos = E_spline_getPositionAtT(s_splineRunningMen, s_runnersT[i]);
        pos[0] += RUNNER_X_OFFSET[i];
        E_sceneObject_setPosition(s_runners[i], pos);
        E_sceneObject_setRotation(s_runners[i], E_spline_getRotationAtT(s_splineRunningMen, s_runnersT[i]));
        if (s_runners[i]->animator)
            E_animator_setTime(s_runners[i]->animator,
                E_animator_getDuration(s_runners[i]->animator) * ((float)i / RUNNER_COUNT));
    }
}

void CSTInterior_begin()
{
    static int s_begun = 0;
    if (s_begun) return;
    s_begun = 1;

    E_postProcess_setFXAA(1);
    E_postProcess_setSaturation(1.00f);
    E_postProcess_setBlur(0.00f);
    E_postProcess_setVignette(0.00f, 0.50f);
    E_postProcess_setDOF(1, 7.27f, 5.00f, 28.74f);
    E_postProcess_setGodRays(1, 0.500f, 0.500f, 256, 0.469f, 0.884f, 0.422f, 0.470f);
    E_postProcess_setBloom(1, 2.07f, 7.53f);
    E_postProcess_setFadeColor(0.0f, 0.0f, 0.0f);
    E_postProcess_setOverlay(0, 0);
    E_postProcess_setDissolve(0.0f);
    E_postProcess_setDissolveNoise(E_loadTexture("DEMO_ASSETS/SkySphere/textures/perlinNoise.png"));
    E_setFOV(30.0f);
    CSTInterior_activate();
}

void CSTInterior_activate()
{
    if (s_cstInterior)               E_sceneObject_setActive(s_cstInterior,  1);
    if (s_child)                  E_sceneObject_setActive(s_child,     1);
    if (s_papa)                   E_sceneObject_setActive(s_papa,      1);
    if (s_papaDead)              E_sceneObject_setActive(s_papaDead, 0);
    if (s_sl_child)                 E_spotLight_activate(s_sl_child);
    if (s_sl_papawadwas)            E_spotLight_activate(s_sl_papawadwas);
    if (s_emitterTrackSmoke)  s_emitterTrackSmoke->active  = 0;
    if (s_emitterTrackSmoke2) s_emitterTrackSmoke2->active = 1;
    if (s_emitterTrackSmoke3) s_emitterTrackSmoke3->active = 1;
    if (s_pl_0)          E_pointLight_activate(s_pl_0);
    if (s_pl_1)          E_pointLight_activate(s_pl_1);
    if (s_pl_2)          E_pointLight_activate(s_pl_2);
    if (s_pl_3)          E_pointLight_activate(s_pl_3);
    if (s_pl_4)          E_pointLight_activate(s_pl_4);
    if (s_pl_fire)       E_pointLight_activate(s_pl_fire);
    if (s_pointLight_train)  E_pointLight_activate(s_pointLight_train);
    if (s_pointLight_train2) E_pointLight_activate(s_pointLight_train2);
    if (s_pointLight_train3) E_pointLight_activate(s_pointLight_train3);
    if (s_pointLight_train4) E_pointLight_activate(s_pointLight_train4);
    if (s_sl_0)          E_spotLight_activate(s_sl_0);
    if (s_sl_1)          E_spotLight_activate(s_sl_1);
    if (s_sl_2)          E_spotLight_activate(s_sl_2);
    if (s_sl_3)          E_spotLight_activate(s_sl_3);
    if (s_sl_leftside)   E_spotLight_activate(s_sl_leftside);
    if (s_sl_leftside2)  E_spotLight_activate(s_sl_leftside2);
    if (s_sl_leftside3)  E_spotLight_activate(s_sl_leftside3);
    if (s_sl_leftside4)  E_spotLight_activate(s_sl_leftside4);
    if (s_sl_leftside5)  E_spotLight_activate(s_sl_leftside5);
    if (s_sl_kidSeat)    E_spotLight_deactivate(s_sl_kidSeat); // managed by applyAfterAttackState
}

void CSTInterior_deactivate()
{
    if (s_cstInterior)               E_sceneObject_setActive(s_cstInterior,  0);
    if (s_child)                  E_sceneObject_setActive(s_child,     0);
    if (s_papa)                   E_sceneObject_setActive(s_papa,      0);
    if (s_papaDead)              E_sceneObject_setActive(s_papaDead, 0);
    if (s_sl_child)                 E_spotLight_deactivate(s_sl_child);
    if (s_sl_papawadwas)            E_spotLight_deactivate(s_sl_papawadwas);
    if (s_emitterTrackSmoke)  s_emitterTrackSmoke->active  = 0;
    if (s_emitterTrackSmoke2) s_emitterTrackSmoke2->active = 0;
    if (s_emitterTrackSmoke3) s_emitterTrackSmoke3->active = 0;
    if (s_emitterBlackSmoke)  s_emitterBlackSmoke->active  = 0;
    if (s_emitterFire)        s_emitterFire->active         = 0;
    if (s_emitterBlackSmoke2) s_emitterBlackSmoke2->active = 0;
    if (s_emitterBlackSmoke3) s_emitterBlackSmoke3->active = 0;
    if (s_emitterBlackSmoke4) s_emitterBlackSmoke4->active = 0;
    if (s_pl_0)          E_pointLight_deactivate(s_pl_0);
    if (s_pl_1)          E_pointLight_deactivate(s_pl_1);
    if (s_pl_2)          E_pointLight_deactivate(s_pl_2);
    if (s_pl_3)          E_pointLight_deactivate(s_pl_3);
    if (s_pl_4)          E_pointLight_deactivate(s_pl_4);
    if (s_pl_fire)       E_pointLight_deactivate(s_pl_fire);
    if (s_pointLight_train)  E_pointLight_deactivate(s_pointLight_train);
    if (s_pointLight_train2) E_pointLight_deactivate(s_pointLight_train2);
    if (s_pointLight_train3) E_pointLight_deactivate(s_pointLight_train3);
    if (s_pointLight_train4) E_pointLight_deactivate(s_pointLight_train4);
    if (s_sl_0)          E_spotLight_deactivate(s_sl_0);
    if (s_sl_1)          E_spotLight_deactivate(s_sl_1);
    if (s_sl_2)          E_spotLight_deactivate(s_sl_2);
    if (s_sl_3)          E_spotLight_deactivate(s_sl_3);
    if (s_sl_leftside)   E_spotLight_deactivate(s_sl_leftside);
    if (s_sl_leftside2)  E_spotLight_deactivate(s_sl_leftside2);
    if (s_sl_leftside3)  E_spotLight_deactivate(s_sl_leftside3);
    if (s_sl_leftside4)  E_spotLight_deactivate(s_sl_leftside4);
    if (s_sl_leftside5)  E_spotLight_deactivate(s_sl_leftside5);
    if (s_sl_kidSeat)    E_spotLight_deactivate(s_sl_kidSeat);
}

void CSTInterior_updateWorld()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();
    if (!s_afterAttack && now >= 120.2f)
    {
        s_afterAttack = 1;
        CSTInterior_applyAfterAttackState(1);
    }

    static int s_papaDead_stopped = 0;
    if (!s_papaDead_stopped && s_papaDead && s_papaDead->animator && now >= 120.2f)
    {
        float dur = E_animator_getDuration(s_papaDead->animator);
        if (now >= 120.2f + dur)
        {
            s_papaDead_stopped = 1;
            E_animator_setTime (s_papaDead->animator, dur - 0.05f);
            E_animator_setSpeed(s_papaDead->animator, 0.0f);
        }
    }

    if (s_splineRunningMen)
    {
        for (int i = 0; i < RUNNER_COUNT; i++)
        {
            if (!s_runners[i]) continue;
            s_runnersT[i] += s_afterAttack ? 0.003750f : 0.001875f;
            if (s_runnersT[i] > 1.0f) s_runnersT[i] -= 1.0f;
            vmath::vec3 pos = E_spline_getPositionAtT(s_splineRunningMen, s_runnersT[i]);
            pos[0] += RUNNER_X_OFFSET[i];
            E_sceneObject_setPosition(s_runners[i], pos);
            E_sceneObject_setRotation(s_runners[i], E_spline_getRotationAtT(s_splineRunningMen, s_runnersT[i]));
        }
    }
}

void CSTInterior_updateCamera()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    // At 93s: instant white flash, fades back to scene over 2s
    E_postProcess_transitionBetweenTime(120.2f, 122.2f, E_TRANSITION_START);

    if (now > 109.2f && now <= 116.2f)
    {
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_cameraSplineToCSTBoard, 109.2f, 116.2f, 1.0f);
        E_camera_setPosition(E_spline_getPositionBetweenTime(s_splineToTrain, 109.2f, 116.2f, 1.0f));
        E_camera_lookAtPoint(lookAt);
        E_postProcess_applyDOF(lookAt, 5.0f, 28.74f);
    }
    else if (now > 116.2f && now <= 120.2f)
    {
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_cameraLookAtSplineToKid, 116.2f, 120.2f, 1.0f);
        E_camera_setPosition(E_spline_getPositionBetweenTime(s_splineToChild, 116.2f, 145.2f, 1.0f));
        E_camera_lookAtPoint(lookAt);
        E_postProcess_applyDOF(lookAt, 5.0f, 28.74f);
    }
    else if (now > 120.2f && now <= 145.2f)
    {
        vmath::vec3 lookAt = vmath::vec3(42.297f, 2.020f, -24.040f);
        E_camera_setPosition(E_spline_getPositionBetweenTime(s_splineToChild, 116.2f, 145.2f, 1.0f));
        E_camera_lookAtPoint(lookAt);
        E_postProcess_applyDOF(lookAt, 5.0f, 28.74f);
    }

    if (now >= 146.5f)
    {
        E_postProcess_setOverlay(1, s_overlayTex);

        float dissolve = 0.0f;
        if (now >= 155.2f)
            dissolve = 1.0f;
        else if (now >= 151.2f)
        {
            float t  = (now - 151.2f) / (155.2f - 151.2f);
            dissolve = t * t * (3.0f - 2.0f * t);
        }
        E_postProcess_setDissolve(dissolve);
    }
}

void CSTInterior_update()
{
    CSTInterior_updateWorld();
    CSTInterior_updateCamera();
}

void CSTInterior_uninit()
{
    if (s_cstInterior) { E_sceneObject_destroyResources(s_cstInterior); s_cstInterior = NULL; }
    if (s_emitterTrackSmoke)  { E_emitter_deactivate(s_emitterTrackSmoke);  s_emitterTrackSmoke  = NULL; }
    if (s_emitterTrackSmoke2) { E_emitter_deactivate(s_emitterTrackSmoke2); s_emitterTrackSmoke2 = NULL; }
    if (s_emitterTrackSmoke3) { E_emitter_deactivate(s_emitterTrackSmoke3); s_emitterTrackSmoke3 = NULL; }
    if (s_emitterBlackSmoke)  { E_emitter_deactivate(s_emitterBlackSmoke);  s_emitterBlackSmoke  = NULL; }
    if (s_emitterFire)        { E_emitter_deactivate(s_emitterFire);         s_emitterFire        = NULL; }
    if (s_emitterBlackSmoke2) { E_emitter_deactivate(s_emitterBlackSmoke2); s_emitterBlackSmoke2 = NULL; }
    if (s_emitterBlackSmoke3) { E_emitter_deactivate(s_emitterBlackSmoke3); s_emitterBlackSmoke3 = NULL; }
    if (s_emitterBlackSmoke4) { E_emitter_deactivate(s_emitterBlackSmoke4); s_emitterBlackSmoke4 = NULL; }

    if (s_pl_0)              { E_pointLight_deactivate(s_pl_0);              s_pl_0              = NULL; }
    if (s_pl_1)              { E_pointLight_deactivate(s_pl_1);              s_pl_1              = NULL; }
    if (s_pl_2)              { E_pointLight_deactivate(s_pl_2);              s_pl_2              = NULL; }
    if (s_pl_3)              { E_pointLight_deactivate(s_pl_3);              s_pl_3              = NULL; }
    if (s_pl_4)              { E_pointLight_deactivate(s_pl_4);              s_pl_4              = NULL; }
    if (s_pl_fire)           { E_pointLight_deactivate(s_pl_fire);           s_pl_fire           = NULL; }
    if (s_pointLight_train)  { E_pointLight_deactivate(s_pointLight_train);  s_pointLight_train  = NULL; }
    if (s_pointLight_train2) { E_pointLight_deactivate(s_pointLight_train2); s_pointLight_train2 = NULL; }
    if (s_pointLight_train3) { E_pointLight_deactivate(s_pointLight_train3); s_pointLight_train3 = NULL; }
    if (s_pointLight_train4) { E_pointLight_deactivate(s_pointLight_train4); s_pointLight_train4 = NULL; }

    if (s_sl_0)          { E_spotLight_deactivate(s_sl_0);          s_sl_0          = NULL; }
    if (s_sl_1)          { E_spotLight_deactivate(s_sl_1);          s_sl_1          = NULL; }
    if (s_sl_2)          { E_spotLight_deactivate(s_sl_2);          s_sl_2          = NULL; }
    if (s_sl_3)          { E_spotLight_deactivate(s_sl_3);          s_sl_3          = NULL; }
    if (s_sl_leftside)   { E_spotLight_deactivate(s_sl_leftside);   s_sl_leftside   = NULL; }
    if (s_sl_leftside2)  { E_spotLight_deactivate(s_sl_leftside2);  s_sl_leftside2  = NULL; }
    if (s_sl_leftside3)  { E_spotLight_deactivate(s_sl_leftside3);  s_sl_leftside3  = NULL; }
    if (s_sl_leftside4)  { E_spotLight_deactivate(s_sl_leftside4);  s_sl_leftside4  = NULL; }
    if (s_sl_leftside5)  { E_spotLight_deactivate(s_sl_leftside5);  s_sl_leftside5  = NULL; }
    if (s_sl_kidSeat)    { E_spotLight_deactivate(s_sl_kidSeat);    s_sl_kidSeat    = NULL; }
    if (s_sl_child)      { E_spotLight_deactivate(s_sl_child);      s_sl_child      = NULL; }
    if (s_sl_papawadwas) { E_spotLight_deactivate(s_sl_papawadwas); s_sl_papawadwas = NULL; }
}

static void CSTInterior_applyAfterAttackState(int afterAttack)
{
    int i;
    E_pointLight* cstPointLights[] = {
        s_pl_0, s_pl_1, s_pl_2, s_pl_3, s_pl_4, s_pl_fire
    };
    E_spotLight* cstSpotLights[] = {
        s_sl_0, s_sl_1, s_sl_2, s_sl_3,
        s_sl_leftside, s_sl_leftside2, s_sl_leftside3, s_sl_leftside4, s_sl_leftside5
    };
    for (i = 0; i < 6; i++)
    {
        if (!cstPointLights[i]) continue;
        cstPointLights[i]->flicker       = afterAttack;
        cstPointLights[i]->flickerSpeed  = 4.0f + (i * 3.7f + (i % 3) * 2.1f);
        cstPointLights[i]->flickerAmount = 0.3f + (i % 4) * 0.15f;
    }
    /* train lights always flicker regardless of attack state */
    if (s_pointLight_train)  { s_pointLight_train->flicker = 1;  s_pointLight_train->flickerSpeed  = 26.200f; s_pointLight_train->flickerAmount  = 0.600f; }
    if (s_pointLight_train2) { s_pointLight_train2->flicker = 1; s_pointLight_train2->flickerSpeed = 32.000f; s_pointLight_train2->flickerAmount = 0.750f; }
    if (s_pointLight_train3) { s_pointLight_train3->flicker = 1; s_pointLight_train3->flickerSpeed = 37.800f; s_pointLight_train3->flickerAmount = 0.300f; }
    if (s_pointLight_train4) { s_pointLight_train4->flicker = 1; s_pointLight_train4->flickerSpeed = 37.300f; s_pointLight_train4->flickerAmount = 0.450f; }

    for (i = 0; i < 9; i++)
    {
        if (!cstSpotLights[i]) continue;
        cstSpotLights[i]->flicker       = afterAttack;
        cstSpotLights[i]->flickerSpeed  = 5.0f + (i * 2.9f + (i % 5) * 1.7f);
        cstSpotLights[i]->flickerAmount = 0.25f + (i % 3) * 0.2f;
    }
    E_ambientLight_setIntensity(&engineScene.ambientLight, afterAttack ? 0.13f : 0.34f);

    E_fog_setEnabled(&engineScene.fog, afterAttack);
    if (afterAttack)
    {
        E_fog_setColor  (&engineScene.fog, {0.45f, 0.00f, 0.00f});
        E_fog_setDensity(&engineScene.fog, 0.03f);
        E_fog_setType   (&engineScene.fog, 0);
    }

    if (s_emitterBlackSmoke)        s_emitterBlackSmoke->active        = afterAttack;
    if (s_emitterFire)              s_emitterFire->active               = afterAttack;
    if (s_emitterBlackSmoke2)       s_emitterBlackSmoke2->active       = afterAttack;
    if (s_emitterBlackSmoke3)       s_emitterBlackSmoke3->active       = afterAttack;
    if (s_emitterBlackSmoke4)       s_emitterBlackSmoke4->active       = afterAttack;
    if (s_emitterTrackSmoke) s_emitterTrackSmoke->active = afterAttack;
    if (s_sl_kidSeat) { if (afterAttack) E_spotLight_activate(s_sl_kidSeat); else E_spotLight_deactivate(s_sl_kidSeat); }

    if (afterAttack)
    {
        if (s_papa)      E_sceneObject_setActive(s_papa,      0);
        if (s_papaDead) E_sceneObject_setActive(s_papaDead, 1);
        if (s_papaDead && s_papaDead->animator)
            E_animator_setSpeed(s_papaDead->animator, 1.0f);
    }
    else
    {
        if (s_papa)      E_sceneObject_setActive(s_papa,      1);
        if (s_papaDead) E_sceneObject_setActive(s_papaDead, 0);
    }
    for (int i = 0; i < RUNNER_COUNT; i++)
        if (s_runners[i]) s_runners[i]->active = afterAttack;

    E_postProcess_setFadeColor(afterAttack ? 1.0f : 0.0f, 0.0f, 0.0f);
}
