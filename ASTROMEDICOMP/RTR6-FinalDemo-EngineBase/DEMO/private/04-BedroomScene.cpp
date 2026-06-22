#include "../public/04-BedroomScene.h"
#include "../public/DEMO.hpp"
#include "engineAPIs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "E_utils.h"
#include "E_spline.h"
#include "E_animation.h"
#include "E_material.h"
#include <GL/glew.h>

static E_sceneObject* s_bedroomScene = NULL;
static E_sceneObject* s_deadIndianLadyPose   = NULL;
static E_sceneObject* s_waiterEvacuating     = NULL;
static E_sceneObject* s_waiterBending        = NULL;
static E_sceneObject* s_waiterStartWalking   = NULL;
static E_sceneObject* s_waiterWithBaby               = NULL;
static E_sceneObject* s_waiterHandOnBabyMouth        = NULL;
static E_sceneObject* s_waiterHandRemoveFromBaby     = NULL;
static E_sceneObject* s_babySmiling                  = NULL;
static E_sceneObject* s_babyCrying           = NULL;
static E_sceneObject* s_crawlingMan1         = NULL;
static E_sceneObject* s_crawlingGirl1        = NULL;

static E_sceneObject* s_terrorist1Kick1      = NULL;
static E_sceneObject* s_terrorist1Kick2      = NULL;
static E_sceneObject* s_terrorist1Gun1       = NULL;
static E_sceneObject* s_terrorist1Gun2       = NULL;
static E_sceneObject* s_dyingForwardMan1     = NULL;
static E_sceneObject* s_dyingLadyForward1    = NULL;
static E_sceneObject* s_terrorist2Walking    = NULL;
static E_sceneObject* s_terrorist1StartWalk  = NULL;
static E_sceneObject* s_almirahDoorClosed    = NULL;
static E_sceneObject* s_almirahOpen          = NULL;
static E_sceneObject* s_terrorist1Kick1AK47  = NULL;
static E_sceneObject* s_terrorist1Kick2AK47  = NULL;
static E_sceneObject* s_terrorist1Gun1AK47   = NULL;
static E_sceneObject* s_terrorist1Gun2AK47   = NULL;
static E_sceneObject* s_terrorist1StartWalkAK47 = NULL;
static E_sceneObject* s_terrorist2WalkingAK47= NULL;

static vmath::vec3    s_terrorist1Kick1AK47PosOffset   = vmath::vec3(-0.096f, -0.799f, 0.204f);
static vmath::vec3    s_terrorist1Kick1AK47RotOffset   = vmath::vec3(-48.00f, 8.00f, 5.00f);
static vmath::vec3    s_terrorist1Kick2AK47PosOffset   = vmath::vec3( 0.198f, -0.500f, 0.402f);
static vmath::vec3    s_terrorist1Kick2AK47RotOffset   = vmath::vec3(-6.00f, -1.00f, 0.00f);
static vmath::vec3    s_terrorist1Gun1AK47PosOffset    = vmath::vec3( 0.247f, -0.519f, 0.234f);
static vmath::vec3    s_terrorist1Gun1AK47RotOffset    = vmath::vec3( 0.00f, 189.00f, 21.00f);
static vmath::vec3    s_terrorist1Gun2AK47PosOffset    = vmath::vec3(-0.004f, -0.399f, -0.304f);
static vmath::vec3    s_terrorist1Gun2AK47RotOffset    = vmath::vec3(-12.00f, -177.00f, 0.00f);
static vmath::vec3    s_terrorist1StartWalkAK47PosOffset = vmath::vec3(0.245f, -0.550f, 0.056f);
static vmath::vec3    s_terrorist1StartWalkAK47RotOffset = vmath::vec3(-34.00f, -123.00f, 0.00f);
static vmath::vec3    s_terrorist2WalkingAK47PosOffset = vmath::vec3(0.046f, -0.247f, -0.048f);
static vmath::vec3    s_terrorist2WalkingAK47RotOffset = vmath::vec3(-48.00f, -138.00f, 0.00f);

static E_spline*      s_splineCrawlingPeople              = NULL;
static E_spline*      s_splineToWaiterToHeadUp             = NULL;
static E_spline*      s_splineToCryingDoorLookAt           = NULL;
static E_spline*      s_splineTerroristKickingToWaiterE    = NULL;
static E_spline*      s_splineWaiterEntersRoom             = NULL;
static E_spline*      s_splineWaiterSeesDeadBodyLookAt     = NULL;
static E_spline*      s_splineDeadBodyPauseToCradle        = NULL;
static E_spline*      s_splineBodyToCradleLookAt           = NULL;
static E_spline*      s_splineTerroristComing_1            = NULL;
static E_spline*      s_splineTerroristComing_2            = NULL;
static E_spline*      s_splineToAlmirahInterior            = NULL;
static E_spline*      s_splineToBabyMouthCoveredLookat     = NULL;
static E_spline*      s_splineToBabyMouthCoveredCamMove    = NULL;
static E_spline*      s_splineToTransitionLookat           = NULL;
static E_spline*      s_splineAfterDoorCloseLookAt         = NULL;
static E_spline*      s_splineTerrorInsideRoom1            = NULL;
static E_spline*      s_splineTerrorInsideRoom2            = NULL;

static E_pointLight*  s_pl_gun1 = NULL;
static E_pointLight*  s_pl_gun2 = NULL;
static E_pointLight*  s_pl_gun3 = NULL;
static E_pointLight*  s_pl_gun4 = NULL;
static E_pointLight*  s_pl_gun5 = NULL;
static E_pointLight*  s_pl_cradleGun    = NULL;
static E_pointLight*  s_pl_nearWaiter   = NULL;

static E_pointLight*  s_pl_ceiling_right  = NULL;
static E_pointLight*  s_pl_ceiling_left   = NULL;
static E_pointLight*  s_pl_ceiling_corner = NULL;
static E_pointLight*  s_pl_corner_near    = NULL;
static E_pointLight*  s_pl_corner_far     = NULL;
static E_pointLight*  s_pl_warm_fill      = NULL;
static E_pointLight*  s_pl_outsiderooms   = NULL;
static E_pointLight*  s_pl_waiterBaby     = NULL;

static E_spotLight*   s_sl_outside_1          = NULL;
static E_spotLight*   s_sl_outside_2          = NULL;
static E_spotLight*   s_sl_outside_3          = NULL;
static E_spotLight*   s_sl_main_bedroom       = NULL;
static E_spotLight*   s_sl_outside_5          = NULL;
static E_spotLight*   s_sl_outside_7          = NULL;
static E_spotLight*   s_sl_outside_8          = NULL;
static E_spotLight*   s_sl_cradle             = NULL;
static E_spotLight*   s_sl_almirah            = NULL;
static E_spotLight*   s_sl_waiter             = NULL;
static E_spotLight*   s_sl_ladyDeadbody       = NULL;
static E_spotLight*   s_sl_10                 = NULL;
static E_spotLight*   s_sl_10_copy_copy       = NULL;
static E_spotLight*   s_sl_10_copy            = NULL;

static void BedroomScene_syncAK47ToTerrorist(E_sceneObject* ak47, E_sceneObject* terrorist, vmath::vec3 posOffset, vmath::vec3 rotOffset)
{
    if (!ak47) return;
    if (!terrorist || !terrorist->active || !terrorist->animator)
    {
        E_sceneObject_setActive(ak47, 0);
        return;
    }

    E_animator_update(terrorist->animator, 0.0f);
    E_sceneObject_updateWorldMatrices(terrorist, NULL);

    vmath::vec3 handPos = E_animator_getBonePosition(terrorist->animator, "mixamorig:RightHand");
    //printf("%s handPos: %.3f %.3f %.3f\n", terrorist->name, handPos[0], handPos[1], handPos[2]);
    E_sceneObject_setPosition(ak47, handPos + posOffset);
    E_sceneObject_setRotation(ak47, terrorist->rotation + rotOffset);
    E_sceneObject_setActive  (ak47, 1);
}

void BedroomScene_init()
{
    s_bedroomScene        = E_loadModel("DEMO_ASSETS/04-BedroomScene/RoomScene_v9.glb",        &engineScene, "s_bedroomScene");
    s_deadIndianLadyPose = E_loadModel("DEMO_ASSETS/04-BedroomScene/DeadIndianLadyPose.glb",  &engineScene, "DeadIndianLadyPose");
    s_waiterEvacuating     = E_loadModel("DEMO_ASSETS/04-BedroomScene/Waiter_Evacuating.glb",   &engineScene, "WaiterEvacuating");
    s_waiterBending        = E_loadModel("DEMO_ASSETS/04-BedroomScene/Waiter_Bending.glb",      &engineScene, "Waiter_Bending");
    s_waiterStartWalking   = E_loadModel("DEMO_ASSETS/04-BedroomScene/Waiter_StartWalking.glb", &engineScene, "Waiter_StartWalking");
    s_waiterWithBaby           = E_loadModel("DEMO_ASSETS/04-BedroomScene/Waiter_WithBaby.glb",                  &engineScene, "Waiter_WithBaby");
    s_waiterHandOnBabyMouth    = E_loadModel("DEMO_ASSETS/04-BedroomScene/WaiterHandOnBabyMouth.glb",           &engineScene, "WaiterHandOnBabyMouth");
    E_sceneObject_setPosition    (s_waiterHandOnBabyMouth, {-8.28f, 0.50f, 26.90f});
    E_sceneObject_setRotation    (s_waiterHandOnBabyMouth, { 0.00f, 0.00f,  0.00f});
    E_sceneObject_setScale       (s_waiterHandOnBabyMouth, 10.00f);
    E_sceneObject_setCastShadow  (s_waiterHandOnBabyMouth, 1);
    E_sceneObject_setActive      (s_waiterHandOnBabyMouth, 0);
    s_waiterHandRemoveFromBaby = E_loadModel("DEMO_ASSETS/04-BedroomScene/WaiterHandRemoveFromBabyAnimated2.glb", &engineScene, "WaiterHandRemoveFromBaby");
    E_sceneObject_setPosition    (s_waiterHandRemoveFromBaby, {-8.28f, 0.50f, 26.90f});
    E_sceneObject_setRotation    (s_waiterHandRemoveFromBaby, { 0.00f, 0.00f,  0.00f});
    E_sceneObject_setScale       (s_waiterHandRemoveFromBaby, 10.00f);
    E_sceneObject_setCastShadow  (s_waiterHandRemoveFromBaby, 1);
    E_sceneObject_setActive      (s_waiterHandRemoveFromBaby, 0);
    s_babySmiling              = E_loadModel("DEMO_ASSETS/04-BedroomScene/BabySmiling.glb",                     &engineScene, "BabySmiling");
    s_babyCrying           = E_loadModel("DEMO_ASSETS/04-BedroomScene/BabyCrying.glb",          &engineScene, "BabyCrying");
    s_crawlingMan1       = E_loadModel("DEMO_ASSETS/04-BedroomScene/CrawlingMan1.glb",        &engineScene, "CrawlingMan1");
    s_crawlingGirl1      = E_loadModel("DEMO_ASSETS/04-BedroomScene/CrawlingGirl1.glb",       &engineScene, "CrawlingGirl1");
    s_terrorist1Kick1    = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist1_Kicking_Door.glb",  &engineScene, "Terrorist1_Kick1");
    s_terrorist1Kick2    = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist1_Kicking_Door.glb",  &engineScene, "Terrorist1_Kick2");
    s_terrorist1Gun1     = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist1_Shooting.glb",       &engineScene, "Terrorist1_Gun1");
    s_terrorist1Gun2     = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist1_Shooting.glb",       &engineScene, "Terrorist1_Gun2");    s_dyingForwardMan1   = E_loadModel("DEMO_ASSETS/04-BedroomScene/DyingForwardMan.glb",             &engineScene, "DyingForwardMan1");
    s_dyingLadyForward1  = E_loadModel("DEMO_ASSETS/04-BedroomScene/DyingLadyForward.glb",           &engineScene, "DyingLadyForward1");
    s_terrorist1StartWalk = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist1_StartWalking.glb",  &engineScene, "Terrorist1_StartWalk");
    E_sceneObject_setActive(s_terrorist1StartWalk, 0);

    s_terrorist1StartWalkAK47 = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist1_StartWalk_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist1StartWalkAK47, 1);
    E_sceneObject_setScale   (s_terrorist1StartWalkAK47, 7.55f);
    BedroomScene_syncAK47ToTerrorist(s_terrorist1StartWalkAK47, s_terrorist1StartWalk, s_terrorist1StartWalkAK47PosOffset, s_terrorist1StartWalkAK47RotOffset);

    s_almirahDoorClosed   = E_loadModel("DEMO_ASSETS/04-BedroomScene/AlmirahDoorClosed.glb", &engineScene, "AlmirahDoorClosed");
    s_almirahOpen         = E_loadModel("DEMO_ASSETS/04-BedroomScene/AlmirahOpen.glb",       &engineScene, "AlmirahOpen");
    E_sceneObject_setActive(s_almirahOpen, 0);
    s_terrorist2Walking  = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist2_Walking_With_Rifle.glb", &engineScene, "Terrorist2_Walking");
    E_sceneObject_setScale (s_terrorist2Walking, 10.0f);
    E_sceneObject_setActive(s_terrorist2Walking, 0);

    s_terrorist2WalkingAK47 = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist2_Walking_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist2WalkingAK47, 1);
    E_sceneObject_setScale   (s_terrorist2WalkingAK47, 8.00f);
    BedroomScene_syncAK47ToTerrorist(s_terrorist2WalkingAK47, s_terrorist2Walking, s_terrorist2WalkingAK47PosOffset, s_terrorist2WalkingAK47RotOffset);

    {
        s_splineToAlmirahInterior = E_scene_addSpline(&engineScene, "splineToAlmirahInterior");
        s_splineToAlmirahInterior->numPoints    = 7;
        s_splineToAlmirahInterior->points[0] = vmath::vec3(-7.134f, 7.173f, 28.414f);
        s_splineToAlmirahInterior->points[1] = vmath::vec3(-8.614f, 7.173f, 28.480f);
        s_splineToAlmirahInterior->points[2] = vmath::vec3(-9.514f, 7.173f, 28.280f);
        s_splineToAlmirahInterior->points[3] = vmath::vec3(-9.687f, 7.181f, 28.066f);
        s_splineToAlmirahInterior->points[4] = vmath::vec3(-9.624f, 7.181f, 27.562f);
        s_splineToAlmirahInterior->points[5] = vmath::vec3(-9.177f, 7.181f, 26.589f);
        s_splineToAlmirahInterior->points[6] = vmath::vec3(-8.920f, 7.181f, 26.405f);
    }
    {
        s_splineTerrorInsideRoom1 = E_scene_addSpline(&engineScene, "splineTerrorInsideRoom1");
        s_splineTerrorInsideRoom1->numPoints    = 6;
        s_splineTerrorInsideRoom1->points[0] = vmath::vec3(-17.635f, 0.020f, 33.367f);
        s_splineTerrorInsideRoom1->points[1] = vmath::vec3(-17.635f, 0.020f, 33.367f);
        s_splineTerrorInsideRoom1->points[2] = vmath::vec3(-11.798f, 0.006f, 30.543f);
        s_splineTerrorInsideRoom1->points[3] = vmath::vec3( -6.436f, 0.289f, 34.642f);
        s_splineTerrorInsideRoom1->points[4] = vmath::vec3(  0.427f, 0.384f, 35.752f);
        s_splineTerrorInsideRoom1->points[5] = vmath::vec3(  2.920f, 0.469f, 36.039f);
    }
    {
        s_splineTerrorInsideRoom2 = E_scene_addSpline(&engineScene, "splineTerrorInsideRoom2");
        s_splineTerrorInsideRoom2->numPoints    = 4;
        s_splineTerrorInsideRoom2->points[0] = vmath::vec3(  1.835f, 0.115f, 35.497f);
        s_splineTerrorInsideRoom2->points[1] = vmath::vec3(  1.835f, 0.115f, 35.497f);
        s_splineTerrorInsideRoom2->points[2] = vmath::vec3(-21.476f, 0.089f, 33.390f);
        s_splineTerrorInsideRoom2->points[3] = vmath::vec3(-21.476f, 0.089f, 33.390f);
    }
    {
        s_splineAfterDoorCloseLookAt = E_scene_addSpline(&engineScene, "splineAfterDoorCloseLookAt");
        s_splineAfterDoorCloseLookAt->numPoints    = 4;
        s_splineAfterDoorCloseLookAt->points[0] = vmath::vec3(-9.489f, 7.373f, 29.305f);
        s_splineAfterDoorCloseLookAt->points[1] = vmath::vec3(-9.489f, 7.373f, 29.305f);
        s_splineAfterDoorCloseLookAt->points[2] = vmath::vec3(-8.065f, 6.616f, 27.462f);
        s_splineAfterDoorCloseLookAt->points[3] = vmath::vec3(-8.065f, 6.616f, 27.462f);
    }
    {
        s_splineToTransitionLookat = E_scene_addSpline(&engineScene, "splineToToTransitionLookat");
        s_splineToTransitionLookat->numPoints    = 4;
        s_splineToTransitionLookat->points[0] = vmath::vec3(-7.658f, 6.159f, 27.507f);
        s_splineToTransitionLookat->points[1] = vmath::vec3(-7.658f, 6.159f, 27.507f);
        s_splineToTransitionLookat->points[2] = vmath::vec3(-9.489f, 7.373f, 29.305f);
        s_splineToTransitionLookat->points[3] = vmath::vec3(-9.489f, 7.373f, 29.305f);
    }
    {
        s_splineToBabyMouthCoveredCamMove = E_scene_addSpline(&engineScene, "splineToBabyMouthCoveredCamMove");
        s_splineToBabyMouthCoveredCamMove->numPoints    = 4;
        s_splineToBabyMouthCoveredCamMove->points[0] = vmath::vec3(-9.177f, 7.181f, 26.589f);
        s_splineToBabyMouthCoveredCamMove->points[1] = vmath::vec3(-9.177f, 7.181f, 26.589f);
        s_splineToBabyMouthCoveredCamMove->points[2] = vmath::vec3(-9.876f, 7.364f, 27.406f);
        s_splineToBabyMouthCoveredCamMove->points[3] = vmath::vec3(-9.876f, 7.364f, 27.406f);
    }
    {
        s_splineToBabyMouthCoveredLookat = E_scene_addSpline(&engineScene, "splineToBabyMouthCoveredLookat");
        s_splineToBabyMouthCoveredLookat->numPoints    = 4;
        s_splineToBabyMouthCoveredLookat->points[0] = vmath::vec3(-8.455f, 7.171f, 27.394f);
        s_splineToBabyMouthCoveredLookat->points[1] = vmath::vec3(-8.455f, 7.171f, 27.394f);
        s_splineToBabyMouthCoveredLookat->points[2] = vmath::vec3(-7.658f, 6.159f, 27.507f);
        s_splineToBabyMouthCoveredLookat->points[3] = vmath::vec3(-7.658f, 6.159f, 27.507f);
    }
    {
        s_splineTerroristComing_1 = E_scene_addSpline(&engineScene, "splineTerroristComing_1");
        s_splineTerroristComing_1->numPoints    = 4;
        s_splineTerroristComing_1->points[0] = vmath::vec3(-20.462f, 0.088f, -73.645f);
        s_splineTerroristComing_1->points[1] = vmath::vec3(-20.462f, 0.088f, -73.645f);
        s_splineTerroristComing_1->points[2] = vmath::vec3(-20.462f, 0.088f, -47.845f);
        s_splineTerroristComing_1->points[3] = vmath::vec3(-20.462f, 0.088f, -47.845f);
    }
    {
        s_splineTerroristComing_2 = E_scene_addSpline(&engineScene, "splineTerroristComing_2");
        s_splineTerroristComing_2->numPoints    = 4;
        s_splineTerroristComing_2->points[0] = vmath::vec3(-20.462f, 0.088f, -10.445f);
        s_splineTerroristComing_2->points[1] = vmath::vec3(-20.462f, 0.088f, -10.445f);
        s_splineTerroristComing_2->points[2] = vmath::vec3(-20.462f, 0.088f,   5.755f);
        s_splineTerroristComing_2->points[3] = vmath::vec3(-20.462f, 0.088f,   5.500f);
    }
    E_sceneObject_setPosition    (s_terrorist1Kick1,  { 43.59f, -0.64f, -87.60f});
    E_sceneObject_setRotation    (s_terrorist1Kick1,  {  0.00f,   0.00f,  0.00f});
    E_sceneObject_setScale       (s_terrorist1Kick1,  10.00f);
    E_sceneObject_setCastShadowAll(s_terrorist1Kick1, 1);
    if (s_terrorist1Kick1->animator)
    {
        E_animator_setClipByName(s_terrorist1Kick1->animator, "Animation");
        E_animator_setTime      (s_terrorist1Kick1->animator, 0.0f);
        E_animator_setSpeed     (s_terrorist1Kick1->animator, 0.0f);
    }

    s_terrorist1Kick1AK47   = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist1_Kick1_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist1Kick1AK47, 1);
    E_sceneObject_setScale   (s_terrorist1Kick1AK47, 8.00f);
    BedroomScene_syncAK47ToTerrorist(s_terrorist1Kick1AK47, s_terrorist1Kick1, s_terrorist1Kick1AK47PosOffset, s_terrorist1Kick1AK47RotOffset);

    E_sceneObject_setPosition    (s_terrorist1Kick2,  { -5.24f,  0.00f, -77.28f});
    E_sceneObject_setRotation    (s_terrorist1Kick2,  {  0.00f, 182.00f,  0.00f});
    E_sceneObject_setScale       (s_terrorist1Kick2,  10.00f);
    E_sceneObject_setCastShadowAll(s_terrorist1Kick2, 1);
    if (s_terrorist1Kick2->animator) E_animator_setClipByName(s_terrorist1Kick2->animator, "Animation");

    s_terrorist1Kick2AK47   = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist1_Kick2_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist1Kick2AK47, 1);
    E_sceneObject_setScale   (s_terrorist1Kick2AK47, 8.00f);
    BedroomScene_syncAK47ToTerrorist(s_terrorist1Kick2AK47, s_terrorist1Kick2, s_terrorist1Kick2AK47PosOffset, s_terrorist1Kick2AK47RotOffset);

    E_sceneObject_setPosition    (s_terrorist1Gun1,  { 64.41f, 0.00f, -82.79f});
    E_sceneObject_setRotation    (s_terrorist1Gun1,  {  0.00f, 0.00f,   0.00f});
    E_sceneObject_setScale       (s_terrorist1Gun1,  10.00f);
    E_sceneObject_setCastShadowAll(s_terrorist1Gun1, 1);
    if (s_terrorist1Gun1->animator) E_animator_setClipByName(s_terrorist1Gun1->animator, "Animation");

    s_terrorist1Gun1AK47    = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist1_Gun1_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist1Gun1AK47, 1);
    E_sceneObject_setScale   (s_terrorist1Gun1AK47, 8.00f);
    BedroomScene_syncAK47ToTerrorist(s_terrorist1Gun1AK47, s_terrorist1Gun1, s_terrorist1Gun1AK47PosOffset, s_terrorist1Gun1AK47RotOffset);

    E_sceneObject_setPosition    (s_terrorist1Gun2,  {  4.18f, -0.22f, -80.90f});
    E_sceneObject_setRotation    (s_terrorist1Gun2,  {  0.00f, 185.00f,  0.00f});
    E_sceneObject_setScale       (s_terrorist1Gun2,  10.00f);
    E_sceneObject_setCastShadowAll(s_terrorist1Gun2, 1);
    if (s_terrorist1Gun2->animator) E_animator_setClipByName(s_terrorist1Gun2->animator, "Animation");

    s_terrorist1Gun2AK47    = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist1_Gun2_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist1Gun2AK47, 1);
    E_sceneObject_setScale   (s_terrorist1Gun2AK47, 8.00f);
    BedroomScene_syncAK47ToTerrorist(s_terrorist1Gun2AK47, s_terrorist1Gun2, s_terrorist1Gun2AK47PosOffset, s_terrorist1Gun2AK47RotOffset);

    E_sceneObject_setPosition    (s_dyingForwardMan1,  { 64.92f, 0.11f, -78.13f});
    E_sceneObject_setRotation    (s_dyingForwardMan1,  {  0.00f, 180.00f,  0.00f});
    E_sceneObject_setScale       (s_dyingForwardMan1,  0.10f);
    E_sceneObject_setCastShadowAll(s_dyingForwardMan1, 1);
    if (s_dyingForwardMan1->animator) { E_animator_setClipByName(s_dyingForwardMan1->animator, "Animation"); E_animator_setTime(s_dyingForwardMan1->animator, 0.0f); E_animator_setSpeed(s_dyingForwardMan1->animator, 0.0f); }

    E_sceneObject_setPosition    (s_dyingLadyForward1,  {  5.80f, 0.06f, -89.52f});
    E_sceneObject_setRotation    (s_dyingLadyForward1,  {  0.00f, 16.00f,  0.00f});
    E_sceneObject_setScale       (s_dyingLadyForward1,  0.10f);
    E_sceneObject_setCastShadowAll(s_dyingLadyForward1, 1);
    if (s_dyingLadyForward1->animator) { E_animator_setClipByName(s_dyingLadyForward1->animator, "Animation"); E_animator_setTime(s_dyingLadyForward1->animator, 0.0f); E_animator_setSpeed(s_dyingLadyForward1->animator, 0.0f); }

    E_sceneObject_setScale (s_crawlingMan1,  0.120f);
    E_sceneObject_setScale (s_crawlingGirl1, 0.120f);
    E_sceneObject_setActive(s_crawlingMan1,  1);
    E_sceneObject_setActive(s_crawlingGirl1, 1);
    E_sceneObject_setCastShadowAll(s_crawlingMan1,  1);
    E_sceneObject_setCastShadowAll(s_crawlingGirl1, 1);
    if (s_crawlingMan1->animator)  { E_animator_setClipByName(s_crawlingMan1->animator,  "Animation"); E_animator_setSpeed(s_crawlingMan1->animator,  1.3f); }
    if (s_crawlingGirl1->animator) { E_animator_setClipByName(s_crawlingGirl1->animator, "Animation"); E_animator_setSpeed(s_crawlingGirl1->animator, 1.3f); }

    {
        s_splineCrawlingPeople = E_scene_addSpline(&engineScene, "crawlingPeople");
        s_splineCrawlingPeople->numPoints    = 4;
        s_splineCrawlingPeople->points[0] = vmath::vec3(-1.783f, 0.073f, 87.426f);
        s_splineCrawlingPeople->points[1] = vmath::vec3(-4.007f, 0.076f, 87.470f);
        s_splineCrawlingPeople->points[2] = vmath::vec3(-41.615f, 0.071f, 88.270f);
        s_splineCrawlingPeople->points[3] = vmath::vec3(-44.823f, 0.067f, 87.514f);
    }
    {
        s_splineToWaiterToHeadUp = E_scene_addSpline(&engineScene, "splineToWaiterToHeadUp");
        s_splineToWaiterToHeadUp->numPoints    = 5;
        s_splineToWaiterToHeadUp->points[0] = vmath::vec3(-14.929f, 2.808f, 84.711f);
        s_splineToWaiterToHeadUp->points[1] = vmath::vec3(-19.144f, 3.883f, 88.420f);
        s_splineToWaiterToHeadUp->points[2] = vmath::vec3(-23.755f, 3.372f, 89.522f);
        s_splineToWaiterToHeadUp->points[3] = vmath::vec3(-23.044f, 6.613f, 83.601f);
        s_splineToWaiterToHeadUp->points[4] = vmath::vec3(-22.255f, 7.007f, 76.675f);
    }
    {
        s_splineToCryingDoorLookAt = E_scene_addSpline(&engineScene, "splineToCryingDoorLookAt");
        s_splineToCryingDoorLookAt->numPoints    = 6;
        s_splineToCryingDoorLookAt->points[0] = vmath::vec3(-23.044f, 6.613f, 83.601f);
        s_splineToCryingDoorLookAt->points[1] = vmath::vec3(-23.044f, 6.613f, 83.601f);
        s_splineToCryingDoorLookAt->points[2] = vmath::vec3(-21.642f, 6.293f, 75.505f);
        s_splineToCryingDoorLookAt->points[3] = vmath::vec3(-23.879f, 6.441f, 46.611f);
        s_splineToCryingDoorLookAt->points[4] = vmath::vec3(-17.782f, 6.314f, 35.859f);
        s_splineToCryingDoorLookAt->points[5] = vmath::vec3( -5.688f, 7.316f, 36.027f);
    }
    {
        s_splineTerroristKickingToWaiterE = E_scene_addSpline(&engineScene, "splineTerroristKickingToWaiterE");
        s_splineTerroristKickingToWaiterE->numPoints  = 8;
        s_splineTerroristKickingToWaiterE->points[0] = vmath::vec3( 54.759f,  0.601f, -80.843f);
        s_splineTerroristKickingToWaiterE->points[1] = vmath::vec3( 14.623f,  2.582f, -83.817f);
        s_splineTerroristKickingToWaiterE->points[2] = vmath::vec3(-12.770f,  4.172f, -83.513f);
        s_splineTerroristKickingToWaiterE->points[3] = vmath::vec3(-23.252f,  4.313f, -65.689f);
        s_splineTerroristKickingToWaiterE->points[4] = vmath::vec3(-24.959f,  4.765f, -34.724f);
        s_splineTerroristKickingToWaiterE->points[5] = vmath::vec3(-24.713f,  6.751f,  20.857f);
        s_splineTerroristKickingToWaiterE->points[6] = vmath::vec3(-24.947f,  7.303f,  73.336f);
        s_splineTerroristKickingToWaiterE->points[7] = vmath::vec3(-25.168f,  7.503f,  92.381f);
    }
    {
        s_splineWaiterEntersRoom = E_scene_addSpline(&engineScene, "splineWaiterEntersRoom");
        s_splineWaiterEntersRoom->numPoints    = 5;
        s_splineWaiterEntersRoom->points[0] = vmath::vec3(-23.187f, 7.502f, 37.148f);
        s_splineWaiterEntersRoom->points[1] = vmath::vec3(-21.342f, 7.112f, 36.480f);
        s_splineWaiterEntersRoom->points[2] = vmath::vec3(-15.614f, 5.037f, 32.139f);
        s_splineWaiterEntersRoom->points[3] = vmath::vec3(-13.626f, 4.272f, 32.543f);
        s_splineWaiterEntersRoom->points[4] = vmath::vec3(-13.023f, 4.076f, 32.915f);
    }
    {
        s_splineWaiterSeesDeadBodyLookAt = E_scene_addSpline(&engineScene, "splineWaiterSeesDeadBodyLookAt");
        s_splineWaiterSeesDeadBodyLookAt->numPoints    = 4;
        s_splineWaiterSeesDeadBodyLookAt->points[0] = vmath::vec3(-14.331f, 0.697f, 30.628f);
        s_splineWaiterSeesDeadBodyLookAt->points[1] = vmath::vec3(-13.732f, 0.339f, 28.852f);
        s_splineWaiterSeesDeadBodyLookAt->points[2] = vmath::vec3(-14.217f, 0.779f, 36.210f);
        s_splineWaiterSeesDeadBodyLookAt->points[3] = vmath::vec3(-14.281f, 0.731f, 37.004f);
    }
    {
        s_splineDeadBodyPauseToCradle = E_scene_addSpline(&engineScene, "splineDeadBodyPauseToCradle");
        s_splineDeadBodyPauseToCradle->numPoints    = 4;
        s_splineDeadBodyPauseToCradle->points[0] = vmath::vec3(-15.614f,  5.037f, 32.139f);
        s_splineDeadBodyPauseToCradle->points[1] = vmath::vec3(-13.626f,  4.272f, 32.543f);
        s_splineDeadBodyPauseToCradle->points[2] = vmath::vec3(  1.670f,  3.071f, 35.188f);
        s_splineDeadBodyPauseToCradle->points[3] = vmath::vec3( 15.100f,  6.100f, 33.200f);
    }
    {
        s_splineBodyToCradleLookAt = E_scene_addSpline(&engineScene, "splineBodyToCradleLookAt");
        s_splineBodyToCradleLookAt->numPoints    = 4;
        s_splineBodyToCradleLookAt->points[0] = vmath::vec3( -6.000f, 2.000f,  0.000f);
        s_splineBodyToCradleLookAt->points[1] = vmath::vec3(-14.217f, 0.779f, 36.210f);
        s_splineBodyToCradleLookAt->points[2] = vmath::vec3(  2.620f, 1.482f, 34.863f);
        s_splineBodyToCradleLookAt->points[3] = vmath::vec3(  2.993f, 1.054f, 34.757f);
    }
    E_sceneObject_setPosition    (s_waiterEvacuating, {-27.93f, -1.30f, 97.30f});
    E_sceneObject_setRotation    (s_waiterEvacuating, {-17.00f, 180.00f, 0.00f});
    E_sceneObject_setScale       (s_waiterEvacuating, 11.00f);
    E_sceneObject_setCastShadowAll(s_waiterEvacuating, 1);
    if (s_waiterEvacuating->animator) E_animator_setClipByName(s_waiterEvacuating->animator, "Animation");

    E_sceneObject_setPosition    (s_babyCrying, {  2.11f,  1.37f, 35.42f});
    E_sceneObject_setRotation    (s_babyCrying, {-68.00f, -42.00f,  0.00f});
    E_sceneObject_setScale       (s_babyCrying, 1.00f);
    E_sceneObject_setActive      (s_babyCrying, 1);
    E_sceneObject_setCastShadow  (s_babyCrying, 1);

    E_sceneObject_setPosition    (s_waiterStartWalking, {-18.43f, -3.50f, 40.82f});
    E_sceneObject_setRotation    (s_waiterStartWalking, {  0.00f,  90.00f,  0.00f});
    E_sceneObject_setScale       (s_waiterStartWalking, 10.00f);
    E_sceneObject_setCastShadowAll(s_waiterStartWalking, 1);
    if (s_waiterStartWalking->animator) { E_animator_setClipByName(s_waiterStartWalking->animator, "Animation"); E_animator_setTime(s_waiterStartWalking->animator, 0.0f); E_animator_setSpeed(s_waiterStartWalking->animator, 0.0f); }

    E_sceneObject_setPosition    (s_waiterBending, { 0.17f, -0.05f, 34.67f});
    E_sceneObject_setRotation    (s_waiterBending, { 0.00f,  72.75f,  0.00f});
    E_sceneObject_setScale       (s_waiterBending, 10.00f);
    E_sceneObject_setCastShadowAll(s_waiterBending, 1);
    if (s_waiterBending->animator) { E_animator_setClipByName(s_waiterBending->animator, "Animation"); E_animator_setTime(s_waiterBending->animator, 0.0f); E_animator_setSpeed(s_waiterBending->animator, 0.0f); }

    E_sceneObject_setPosition    (s_waiterWithBaby, { -9.57f, -3.83f, 32.57f});
    E_sceneObject_setRotation    (s_waiterWithBaby, {  0.00f, 144.00f, 0.00f});
    E_sceneObject_setScale       (s_waiterWithBaby, 11.00f);
    E_sceneObject_setCastShadowAll(s_waiterWithBaby, 1);
    if (s_waiterWithBaby->animator) { E_animator_setClipByName(s_waiterWithBaby->animator, "Animation"); E_animator_setTime(s_waiterWithBaby->animator, 0.0f); E_animator_setSpeed(s_waiterWithBaby->animator, 0.0f); }
    E_sceneObject_setPosition(s_deadIndianLadyPose, {-9.55f, -0.15f, 38.76f});
    E_sceneObject_setRotation(s_deadIndianLadyPose, {0.00f, -201.00f, 0.00f});
    E_sceneObject_setScale   (s_deadIndianLadyPose, 4.00f);
    E_sceneObject_setActive  (s_deadIndianLadyPose, 1);
    E_sceneObject_setCastShadowAll(s_deadIndianLadyPose, 1);
    if (s_deadIndianLadyPose->animator)
    {
        E_animator_setClipByName(s_deadIndianLadyPose->animator, "Animation");
        E_animator_setTime      (s_deadIndianLadyPose->animator, 3.33f);
        E_animator_setSpeed     (s_deadIndianLadyPose->animator, 0.0f);
    }

    {
        E_pointLight pointLight_nearWaiter = E_pointLight_create(
            {-23.83f, 4.83f, 86.42f},
            {1.00f, 1.00f, 1.00f},
            0.51f, 10.00f);
        s_pl_nearWaiter = E_pointLight_addToScene(&engineScene, pointLight_nearWaiter);
        E_pointLight_setName(s_pl_nearWaiter, "pointLight_nearWaiter");
        s_pl_nearWaiter->flicker       = 1;
        s_pl_nearWaiter->flickerSpeed  = 0.985f;
        s_pl_nearWaiter->flickerAmount = 0.500f;
    }
    {
        E_pointLight pointLight_cradleGun = E_pointLight_create(
            {0.29f, 7.11f, 37.66f},
            {1.00f, 1.00f, 1.00f},
            5.00f, 10.00f);
        s_pl_cradleGun = E_pointLight_addToScene(&engineScene, pointLight_cradleGun);
        E_pointLight_setName(s_pl_cradleGun, "pointLight_cradleGun");
        s_pl_cradleGun->flicker       = 1;
        s_pl_cradleGun->flickerSpeed  = 18.363f;
        s_pl_cradleGun->flickerAmount = 0.709f;
    }
    {
        E_pointLight pl = E_pointLight_create({64.94f, 5.63f, -79.33f}, {0.85f, 1.00f, 0.44f}, 10.70f, 5.70f);
        pl.flicker = 1; pl.flickerSpeed = 10.680f; pl.flickerAmount = 1.000f;
        s_pl_gun1 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_gun1, "pointLight_gun1");
    }
    {
        E_pointLight pl = E_pointLight_create({5.57f, 5.43f, -83.56f}, {0.85f, 1.00f, 0.44f}, 10.70f, 5.70f);
        pl.flicker = 1; pl.flickerSpeed = 10.680f; pl.flickerAmount = 1.000f;
        s_pl_gun2 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_gun2, "pointLight_gun2");
    }
    {
        E_pointLight pointLight_gun3 = E_pointLight_create(
            {-9.91f, 5.70f, -72.52f},
            {0.85f, 1.00f, 0.44f},
            3.21f, 9.06f);
        s_pl_gun3 = E_pointLight_addToScene(&engineScene, pointLight_gun3);
        E_pointLight_setName(s_pl_gun3, "pointLight_gun3");
        s_pl_gun3->flicker       = 1;
        s_pl_gun3->flickerSpeed  = 10.680f;
        s_pl_gun3->flickerAmount = 1.000f;
    }
    {
        E_pointLight pointLight_gun4 = E_pointLight_create(
            {5.54f, 5.63f, -95.73f},
            {0.85f, 1.00f, 0.44f},
            9.63f, 10.00f);
        s_pl_gun4 = E_pointLight_addToScene(&engineScene, pointLight_gun4);
        E_pointLight_setName(s_pl_gun4, "pointLight_gun4");
        s_pl_gun4->flicker       = 1;
        s_pl_gun4->flickerSpeed  = 10.680f;
        s_pl_gun4->flickerAmount = 1.000f;
    }
    {
        E_pointLight pointLight_gun5 = E_pointLight_create(
            {47.34f, 5.63f, -86.33f},
            {0.85f, 1.00f, 0.44f},
            10.70f, 5.70f);
        s_pl_gun5 = E_pointLight_addToScene(&engineScene, pointLight_gun5);
        E_pointLight_setName(s_pl_gun5, "pointLight_gun5");
        s_pl_gun5->flicker       = 1;
        s_pl_gun5->flickerSpeed  = 10.680f;
        s_pl_gun5->flickerAmount = 1.000f;
    }

    {
        E_pointLight pl = E_pointLight_create({9.02f, 5.50f, 27.19f}, {1.00f, 1.00f, 0.24f}, 0.38f, 9.40f);
        pl.flicker = 0; pl.flickerSpeed = 3.0f; pl.flickerAmount = 0.5f;
        s_pl_ceiling_right = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_ceiling_right, "pl_ceiling_right");
    }
    {
        E_pointLight pl = E_pointLight_create({-5.36f, 5.28f, 26.48f}, {1.00f, 1.00f, 0.24f}, 0.38f, 4.27f);
        pl.flicker = 0; pl.flickerSpeed = 3.0f; pl.flickerAmount = 0.5f;
        s_pl_ceiling_left = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_ceiling_left, "pl_ceiling_left");
    }
    {
        E_pointLight pl_ceiling_corner = E_pointLight_create(
            {-2.36f, 8.28f, 39.88f},
            {1.00f, 1.00f, 0.24f},
            0.38f, 5.18f);
        s_pl_ceiling_corner = E_pointLight_addToScene(&engineScene, pl_ceiling_corner);
        E_pointLight_setName(s_pl_ceiling_corner, "pl_ceiling_corner");
        s_pl_ceiling_corner->flicker       = 0;
        s_pl_ceiling_corner->flickerSpeed  = 3.000f;
        s_pl_ceiling_corner->flickerAmount = 0.500f;
    }
    {
        E_pointLight pl = E_pointLight_create({-5.17f, 3.45f, 43.34f}, {1.00f, 1.00f, 0.24f}, 0.38f, 7.63f);
        pl.flicker = 0; pl.flickerSpeed = 3.0f; pl.flickerAmount = 0.5f;
        s_pl_corner_near = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_corner_near, "pl_corner_near");
    }
    {
        E_pointLight pl = E_pointLight_create({-5.15f, 3.40f, 55.69f}, {1.00f, 1.00f, 0.24f}, 0.38f, 8.26f);
        pl.flicker = 0; pl.flickerSpeed = 3.0f; pl.flickerAmount = 0.5f;
        s_pl_corner_far = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_corner_far, "pl_corner_far");
    }
    {
        E_pointLight pl = E_pointLight_create({7.70f, 6.30f, 48.74f}, {1.00f, 1.00f, 0.87f}, 0.38f, 10.00f);
        pl.flicker = 0; pl.flickerSpeed = 3.0f; pl.flickerAmount = 0.5f;
        s_pl_warm_fill = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_warm_fill, "pl_warm_fill");
    }
    {
        E_pointLight pointLight_outsiderooms = E_pointLight_create(
            {-19.79f, 5.29f, 33.24f},
            {1.00f, 0.59f, 0.59f},
            0.82f, 5.75f);
        s_pl_outsiderooms = E_pointLight_addToScene(&engineScene, pointLight_outsiderooms);
        E_pointLight_setName(s_pl_outsiderooms, "pointLight_outsiderooms");
        s_pl_outsiderooms->flicker       = 0;
        s_pl_outsiderooms->flickerSpeed  = 3.000f;
        s_pl_outsiderooms->flickerAmount = 0.500f;
    }
    {
        E_pointLight pointLight_waiterBaby = E_pointLight_create(
            {1.83f, 4.76f, 36.62f},
            {1.00f, 0.93f, 0.61f},
            0.43f, 3.55f);
        s_pl_waiterBaby = E_pointLight_addToScene(&engineScene, pointLight_waiterBaby);
        E_pointLight_setName(s_pl_waiterBaby, "pointLight_waiterBaby");
        s_pl_waiterBaby->flicker       = 0;
        s_pl_waiterBaby->flickerSpeed  = 3.000f;
        s_pl_waiterBaby->flickerAmount = 0.500f;
    }

    {
        E_spotLight spotLight_outsideRoom_1 = E_spotLight_create(
            {-21.07f, 12.44f, -57.49f},
            {0.50f, -0.86f, 0.01f},
            {0.87f, 0.95f, 1.00f},
            0.70f,
            15.00f, 57.90f,
            17.89f);
        spotLight_outsideRoom_1.flicker       = 0;
        spotLight_outsideRoom_1.flickerSpeed  = 1.741f;
        spotLight_outsideRoom_1.flickerAmount = 0.500f;
        spotLight_outsideRoom_1.castShadow    = 0;
        s_sl_outside_1 = E_spotLight_addToScene(&engineScene, spotLight_outsideRoom_1);
        E_spotLight_setName(s_sl_outside_1, "spotLight_outsideRoom_1");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-18.92f, 18.27f, 34.50f},
            {-0.10f, -1.00f, 0.01f},
            {0.60f, 0.67f, 1.00f},
            0.25f,
            1.40f, 16.90f,
            78.80f);
        sl.flicker = 1; sl.flickerSpeed = 1.674f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_outside_2 = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_outside_2, "spotLight_outsideRoom_main");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-25.72f, 14.49f, -31.50f},
            {-0.37f, -0.93f, 0.03f},
            {0.94f, 1.00f, 0.87f},
            0.70f,
            15.00f, 34.10f,
            78.80f);
        sl.flicker = 0; sl.flickerSpeed = 1.741f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_outside_3 = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_outside_3, "spotLight_outsideRoom_3");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-21.47f, 12.44f, -4.09f},
            {0.50f, -0.86f, 0.01f},
            {0.94f, 1.00f, 0.87f},
            0.65f,
            15.00f, 34.10f,
            78.80f);
        sl.flicker = 1; sl.flickerSpeed = 1.741f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_main_bedroom = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_main_bedroom, "spotLight_outsideRoom_2");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-25.72f, 14.49f, 15.40f},
            {-0.37f, -0.93f, 0.03f},
            {0.94f, 1.00f, 0.87f},
            0.70f,
            15.00f, 34.10f,
            78.80f);
        sl.flicker = 0; sl.flickerSpeed = 1.741f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_outside_5 = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_outside_5, "spotLight_outsideRoom_5");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-25.72f, 14.49f, 56.30f},
            {-0.37f, -0.93f, 0.03f},
            {0.94f, 1.00f, 0.87f},
            0.70f,
            15.00f, 34.10f,
            78.80f);
        sl.flicker = 0; sl.flickerSpeed = 1.741f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_outside_7 = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_outside_7, "spotLight_outsideRoom_7");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-3.83f, 5.60f, -78.85f},
            {-0.94f, 0.30f, -0.15f},
            {0.94f, 1.00f, 0.87f},
            0.32f,
            15.00f, 34.10f,
            78.80f);
        sl.flicker = 0; sl.flickerSpeed = 1.741f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_outside_8 = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_outside_8, "spotLight_outsideRoom_8");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {2.87f, 2.57f, 36.78f},
            {-0.47f, -0.53f, -0.71f},
            {1.00f, 1.00f, 1.00f},
            0.04f,
            15.00f, 28.70f,
            20.25f);
        sl.flicker = 1; sl.flickerSpeed = 1.552f; sl.flickerAmount = 0.244f; sl.castShadow = 1;
        s_sl_cradle = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_cradle, "spotLight_cradle");
    }
    {
        E_spotLight spotLight_almirah = E_spotLight_create(
            {-9.57f, 7.53f, 33.14f},
            {0.36f, -0.04f, -0.93f},
            {1.00f, 0.78f, 0.63f},
            0.07f,
            0.50f, 24.40f,
            61.39f);
        spotLight_almirah.flicker       = 1;
        spotLight_almirah.flickerSpeed  = 3.501f;
        spotLight_almirah.flickerAmount = 0.177f;
        spotLight_almirah.castShadow    = 1;
        s_sl_almirah = E_spotLight_addToScene(&engineScene, spotLight_almirah);
        E_spotLight_setName(s_sl_almirah, "spotLight_almirah");
    }
    {
        E_spotLight spotLight_waiter = E_spotLight_create(
            {-28.81f, 8.42f, 67.37f},
            {0.28f, -0.14f, 0.95f},
            {1.00f, 1.00f, 1.00f},
            0.12f,
            0.00f, 15.00f,
            91.14f);
        spotLight_waiter.flicker       = 1;
        spotLight_waiter.flickerSpeed  = 3.000f;
        spotLight_waiter.flickerAmount = 0.500f;
        spotLight_waiter.castShadow    = 1;
        s_sl_waiter = E_spotLight_addToScene(&engineScene, spotLight_waiter);
        E_spotLight_setName(s_sl_waiter, "spotLight_waiter");
    }
    {
        E_spotLight spotLight_ladyDeadbody = E_spotLight_create(
            {-15.10f, 2.76f, 33.27f},
            {0.31f, -0.47f, 0.83f},
            {0.60f, 0.67f, 1.00f},
            0.33f,
            15.00f, 32.90f,
            100.00f);
        spotLight_ladyDeadbody.flicker       = 1;
        spotLight_ladyDeadbody.flickerSpeed  = 1.674f;
        spotLight_ladyDeadbody.flickerAmount = 0.500f;
        spotLight_ladyDeadbody.castShadow    = 1;
        s_sl_ladyDeadbody = E_spotLight_addToScene(&engineScene, spotLight_ladyDeadbody);
        E_spotLight_setName(s_sl_ladyDeadbody, "spotLight_ladyDeadbody");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {27.66f, 28.09f, -82.86f},
            {0.55f, -0.83f, -0.02f},
            {1.00f, 0.00f, 0.00f},
            1.46f,
            11.90f, 38.00f,
            37.34f);
        sl.flicker = 0; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_10 = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_10, "spotLight_10");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-13.64f, 28.09f, -81.76f},
            {0.55f, -0.83f, -0.02f},
            {1.00f, 1.00f, 1.00f},
            0.82f,
            11.90f, 38.00f,
            37.34f);
        sl.flicker = 1; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_10_copy_copy = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_10_copy_copy, "spotLight_10_copy_copy");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {72.86f, 28.09f, -81.76f},
            {0.55f, -0.83f, -0.02f},
            {1.00f, 0.00f, 0.00f},
            1.81f,
            19.40f, 89.00f,
            37.34f);
        sl.flicker = 1; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_10_copy = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_10_copy, "spotLight_10_copy");
    }
}

void BedroomScene_begin()
{
    static int s_begun = 0;
    if (s_begun) return;
    s_begun = 1;

    E_directionalLight_setDirection(&engineScene.directionalLight, {0.00f, -1.00f, 0.00f});
    E_directionalLight_setColor(&engineScene.directionalLight, {1.00f, 1.00f, 1.00f});
    E_directionalLight_setIntensity(&engineScene.directionalLight, 0.00f);
    E_ambientLight_setIntensity(&engineScene.ambientLight, 0.13f);
    E_ambientLight_setColor(&engineScene.ambientLight, {1.00f, 1.00f, 0.61f});
    
    E_postProcess_setFXAA(1);
    E_postProcess_setSaturation(1.00f);
    E_postProcess_setVignette(0.87f, 1.00f);
    E_postProcess_setDOF(0, 0.0f, 0.0f, 0.0f);
    E_fog_setEnabled(&engineScene.fog, 0);

    BedroomScene_activate();
}

void BedroomScene_activate()
{
    if (s_bedroomScene)        E_sceneObject_setActive(s_bedroomScene,        1);
    if (s_deadIndianLadyPose) E_sceneObject_setActive(s_deadIndianLadyPose, 1);
    if (s_waiterEvacuating)    E_sceneObject_setActive(s_waiterEvacuating,    1);
    if (s_waiterBending)       E_sceneObject_setActive(s_waiterBending,       0);
    if (s_waiterStartWalking)  E_sceneObject_setActive(s_waiterStartWalking,  0);
    if (s_waiterWithBaby)          E_sceneObject_setActive(s_waiterWithBaby,          0);
    if (s_waiterHandOnBabyMouth)   E_sceneObject_setActive(s_waiterHandOnBabyMouth,   0);
    if (s_waiterHandRemoveFromBaby) E_sceneObject_setActive(s_waiterHandRemoveFromBaby, 0);
    if (s_babySmiling)             E_sceneObject_setActive(s_babySmiling,             1);
    if (s_babyCrying)          E_sceneObject_setActive(s_babyCrying,          1);
    if (s_crawlingMan1)       E_sceneObject_setActive(s_crawlingMan1,        1);
    if (s_crawlingGirl1)      E_sceneObject_setActive(s_crawlingGirl1,      1);
    if (s_terrorist1Kick1)    E_sceneObject_setActive(s_terrorist1Kick1,   1);
    if (s_terrorist1Kick2)    E_sceneObject_setActive(s_terrorist1Kick2,   1);
    if (s_terrorist1Gun1)     E_sceneObject_setActive(s_terrorist1Gun1,    1);
    if (s_terrorist1Gun2)     E_sceneObject_setActive(s_terrorist1Gun2,    1);
    if (s_terrorist1Kick1AK47) E_sceneObject_setActive(s_terrorist1Kick1AK47, 1);
    if (s_terrorist1Kick2AK47) E_sceneObject_setActive(s_terrorist1Kick2AK47, 1);
    if (s_terrorist1Gun1AK47)  E_sceneObject_setActive(s_terrorist1Gun1AK47,  1);
    if (s_terrorist1Gun2AK47)  E_sceneObject_setActive(s_terrorist1Gun2AK47,  1);
    if (s_dyingForwardMan1)   E_sceneObject_setActive(s_dyingForwardMan1,  1);
    if (s_dyingLadyForward1)  E_sceneObject_setActive(s_dyingLadyForward1, 1);
    if (s_terrorist1StartWalk) E_sceneObject_setActive(s_terrorist1StartWalk, 0);
    if (s_terrorist1StartWalkAK47) E_sceneObject_setActive(s_terrorist1StartWalkAK47, 0);
    if (s_terrorist2Walking)  E_sceneObject_setActive(s_terrorist2Walking,   0);
    if (s_terrorist2WalkingAK47) E_sceneObject_setActive(s_terrorist2WalkingAK47, 0);
    if (s_almirahDoorClosed)  E_sceneObject_setActive(s_almirahDoorClosed,   1);
    if (s_almirahOpen)        E_sceneObject_setActive(s_almirahOpen,         0);
    if (s_pl_gun1)            E_pointLight_activate(s_pl_gun1);
    if (s_pl_gun2)            E_pointLight_deactivate(s_pl_gun2);
    if (s_pl_gun3)            E_pointLight_deactivate(s_pl_gun3);
    if (s_pl_gun4)            E_pointLight_deactivate(s_pl_gun4);
    if (s_pl_gun5)            E_pointLight_activate(s_pl_gun5);
    if (s_pl_cradleGun)       E_pointLight_deactivate(s_pl_cradleGun);
    if (s_pl_nearWaiter)      E_pointLight_activate(s_pl_nearWaiter);
    if (s_pl_ceiling_right)   E_pointLight_activate(s_pl_ceiling_right);
    if (s_pl_ceiling_left)    E_pointLight_activate(s_pl_ceiling_left);
    if (s_pl_ceiling_corner)  E_pointLight_activate(s_pl_ceiling_corner);
    if (s_pl_corner_near)     E_pointLight_activate(s_pl_corner_near);
    if (s_pl_corner_far)      E_pointLight_activate(s_pl_corner_far);
    if (s_pl_warm_fill)       E_pointLight_activate(s_pl_warm_fill);
    if (s_pl_outsiderooms)    E_pointLight_activate(s_pl_outsiderooms);
    if (s_pl_waiterBaby)      E_pointLight_activate(s_pl_waiterBaby);
    if (s_sl_outside_1)       E_spotLight_activate(s_sl_outside_1);
    if (s_sl_outside_2)       E_spotLight_activate(s_sl_outside_2);
    if (s_sl_outside_3)       E_spotLight_activate(s_sl_outside_3);
    if (s_sl_main_bedroom)    E_spotLight_activate(s_sl_main_bedroom);
    if (s_sl_outside_5)       E_spotLight_activate(s_sl_outside_5);
    if (s_sl_outside_7)       E_spotLight_activate(s_sl_outside_7);
    if (s_sl_outside_8)       E_spotLight_activate(s_sl_outside_8);
    if (s_sl_cradle)          E_spotLight_activate(s_sl_cradle);
    if (s_sl_almirah)         E_spotLight_deactivate(s_sl_almirah);
    if (s_sl_waiter)          E_spotLight_activate(s_sl_waiter);
    if (s_sl_ladyDeadbody)    E_spotLight_activate(s_sl_ladyDeadbody);
    if (s_sl_10)              E_spotLight_activate(s_sl_10);
    if (s_sl_10_copy_copy)    E_spotLight_deactivate(s_sl_10_copy_copy);
    if (s_sl_10_copy)         E_spotLight_activate(s_sl_10_copy);
}

void BedroomScene_deactivate()
{
    if (s_bedroomScene)        E_sceneObject_setActive(s_bedroomScene,        0);
    if (s_deadIndianLadyPose) E_sceneObject_setActive(s_deadIndianLadyPose, 0);
    if (s_waiterEvacuating)    E_sceneObject_setActive(s_waiterEvacuating,    0);
    if (s_waiterBending)       E_sceneObject_setActive(s_waiterBending,       0);
    if (s_waiterStartWalking)  E_sceneObject_setActive(s_waiterStartWalking,  0);
    if (s_waiterWithBaby)          E_sceneObject_setActive(s_waiterWithBaby,          0);
    if (s_waiterHandOnBabyMouth)   E_sceneObject_setActive(s_waiterHandOnBabyMouth,   0);
    if (s_waiterHandRemoveFromBaby) E_sceneObject_setActive(s_waiterHandRemoveFromBaby, 0);
    if (s_babySmiling)             E_sceneObject_setActive(s_babySmiling,             0);
    if (s_babyCrying)          E_sceneObject_setActive(s_babyCrying,          0);
    if (s_crawlingMan1)       E_sceneObject_setActive(s_crawlingMan1,        0);
    if (s_crawlingGirl1)      E_sceneObject_setActive(s_crawlingGirl1,      0);
    if (s_terrorist1Kick1)    E_sceneObject_setActive(s_terrorist1Kick1,   0);
    if (s_terrorist1Kick2)    E_sceneObject_setActive(s_terrorist1Kick2,   0);
    if (s_terrorist1Gun1)     E_sceneObject_setActive(s_terrorist1Gun1,    0);
    if (s_terrorist1Gun2)     E_sceneObject_setActive(s_terrorist1Gun2,    0);
    if (s_dyingForwardMan1)   E_sceneObject_setActive(s_dyingForwardMan1,  0);
    if (s_dyingLadyForward1)  E_sceneObject_setActive(s_dyingLadyForward1, 0);
    if (s_terrorist1StartWalk) E_sceneObject_setActive(s_terrorist1StartWalk, 0);
    if (s_terrorist2Walking)  E_sceneObject_setActive(s_terrorist2Walking,   0);
    if (s_terrorist1Kick1AK47)   E_sceneObject_setActive(s_terrorist1Kick1AK47,   0);
    if (s_terrorist1Kick2AK47)   E_sceneObject_setActive(s_terrorist1Kick2AK47,   0);
    if (s_terrorist1Gun1AK47)    E_sceneObject_setActive(s_terrorist1Gun1AK47,    0);
    if (s_terrorist1Gun2AK47)    E_sceneObject_setActive(s_terrorist1Gun2AK47,    0);
    if (s_terrorist1StartWalkAK47) E_sceneObject_setActive(s_terrorist1StartWalkAK47, 0);
    if (s_terrorist2WalkingAK47) E_sceneObject_setActive(s_terrorist2WalkingAK47, 0);
    if (s_almirahDoorClosed)  E_sceneObject_setActive(s_almirahDoorClosed,   0);
    if (s_almirahOpen)        E_sceneObject_setActive(s_almirahOpen,         0);
    if (s_pl_gun1)            E_pointLight_deactivate(s_pl_gun1);
    if (s_pl_gun2)            E_pointLight_deactivate(s_pl_gun2);
    if (s_pl_gun3)            E_pointLight_deactivate(s_pl_gun3);
    if (s_pl_gun4)            E_pointLight_deactivate(s_pl_gun4);
    if (s_pl_gun5)            E_pointLight_deactivate(s_pl_gun5);
    if (s_pl_cradleGun)       E_pointLight_deactivate(s_pl_cradleGun);
    if (s_pl_nearWaiter)      E_pointLight_deactivate(s_pl_nearWaiter);
    if (s_pl_ceiling_right)   E_pointLight_deactivate(s_pl_ceiling_right);
    if (s_pl_ceiling_left)    E_pointLight_deactivate(s_pl_ceiling_left);
    if (s_pl_ceiling_corner)  E_pointLight_deactivate(s_pl_ceiling_corner);
    if (s_pl_corner_near)     E_pointLight_deactivate(s_pl_corner_near);
    if (s_pl_corner_far)      E_pointLight_deactivate(s_pl_corner_far);
    if (s_pl_warm_fill)       E_pointLight_deactivate(s_pl_warm_fill);
    if (s_pl_outsiderooms)    E_pointLight_deactivate(s_pl_outsiderooms);
    if (s_pl_waiterBaby)      E_pointLight_deactivate(s_pl_waiterBaby);
    if (s_sl_outside_1)       E_spotLight_deactivate(s_sl_outside_1);
    if (s_sl_outside_2)       E_spotLight_deactivate(s_sl_outside_2);
    if (s_sl_outside_3)       E_spotLight_deactivate(s_sl_outside_3);
    if (s_sl_main_bedroom)    E_spotLight_deactivate(s_sl_main_bedroom);
    if (s_sl_outside_5)       E_spotLight_deactivate(s_sl_outside_5);
    if (s_sl_outside_7)       E_spotLight_deactivate(s_sl_outside_7);
    if (s_sl_outside_8)       E_spotLight_deactivate(s_sl_outside_8);
    if (s_sl_cradle)          E_spotLight_deactivate(s_sl_cradle);
    if (s_sl_almirah)         E_spotLight_deactivate(s_sl_almirah);
    if (s_sl_waiter)          E_spotLight_deactivate(s_sl_waiter);
    if (s_sl_ladyDeadbody)    E_spotLight_deactivate(s_sl_ladyDeadbody);
    if (s_sl_10)              E_spotLight_deactivate(s_sl_10);
    if (s_sl_10_copy_copy)    E_spotLight_deactivate(s_sl_10_copy_copy);
    if (s_sl_10_copy)         E_spotLight_deactivate(s_sl_10_copy);
}

void BedroomScene_updateWorld()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    if (s_splineCrawlingPeople)
    {
        float crawlSpeed = 0.156f;
        float tMan  = fmodf(now * crawlSpeed,        1.0f);
        float tGirl = fmodf(now * crawlSpeed + 0.5f, 1.0f);
        if (s_crawlingMan1)
        {
            E_sceneObject_setPosition(s_crawlingMan1,  E_spline_getPositionAtT(s_splineCrawlingPeople, tMan));
            E_sceneObject_setRotation(s_crawlingMan1,  E_spline_getRotationAtT(s_splineCrawlingPeople, tMan));
        }
        if (s_crawlingGirl1)
        {
            E_sceneObject_setPosition(s_crawlingGirl1, E_spline_getPositionAtT(s_splineCrawlingPeople, tGirl));
            E_sceneObject_setRotation(s_crawlingGirl1, E_spline_getRotationAtT(s_splineCrawlingPeople, tGirl));
        }
    }

    if (s_sl_cradle)
    {
        float swingX = 3.324f + 0.45f * sinf(3.14159265f * 0.5f * now);
        E_spotLight_setPosition(s_sl_cradle, {swingX, 2.57f, 36.78f});
    }

    static int s_group1Applied     = 0;
    static int s_group2Applied     = 0;
    static int s_dyingMan1Stopped  = 0;
    static int s_dyingLady1Stopped = 0;
    static int s_kick1Stopped           = 0;
    static int s_kick2Stopped           = 0;
    static int s_waiterStartWalkApplied = 0;
    static int s_waiterStartWalkStopped = 0;
    static int s_waiterBendApplied       = 0;
    static int s_waiterBendStopped       = 0;
    static int s_waiterWithBabyApplied   = 0;
    static int s_waiterWithBabyStopped   = 0;

    // 4:01 - activate group 1, start dying man 1 animation
    if (!s_group1Applied && now >= 241.2f)
    {
        s_group1Applied = 1;
        if (s_dyingForwardMan1)  E_sceneObject_setActive(s_dyingForwardMan1,  1);
        if (s_terrorist1Gun1)    E_sceneObject_setActive(s_terrorist1Gun1,    1);
        if (s_terrorist1Kick1)   E_sceneObject_setActive(s_terrorist1Kick1,   1);
        BedroomScene_syncAK47ToTerrorist(s_terrorist1Gun1AK47,  s_terrorist1Gun1,  s_terrorist1Gun1AK47PosOffset,  s_terrorist1Gun1AK47RotOffset);
        BedroomScene_syncAK47ToTerrorist(s_terrorist1Kick1AK47, s_terrorist1Kick1, s_terrorist1Kick1AK47PosOffset, s_terrorist1Kick1AK47RotOffset);
        if (s_pl_gun1)           E_pointLight_activate  (s_pl_gun1);
        if (s_dyingLadyForward1) E_sceneObject_setActive(s_dyingLadyForward1, 0);
        if (s_terrorist1Gun2)    E_sceneObject_setActive(s_terrorist1Gun2,    0);
        if (s_terrorist1Kick2)   E_sceneObject_setActive(s_terrorist1Kick2,   0);
        if (s_terrorist1Gun2AK47)  E_sceneObject_setActive(s_terrorist1Gun2AK47,  0);
        if (s_terrorist1Kick2AK47) E_sceneObject_setActive(s_terrorist1Kick2AK47, 0);
        if (s_pl_gun2)           E_pointLight_deactivate(s_pl_gun2);
        if (s_dyingForwardMan1 && s_dyingForwardMan1->animator)
            E_animator_setSpeed(s_dyingForwardMan1->animator, 1.0f);
        if (s_terrorist1Kick1 && s_terrorist1Kick1->animator)
            E_animator_setSpeed(s_terrorist1Kick1->animator, 1.0f);
    }
    if (now >= 241.2f && now < 246.2f)
    {
        BedroomScene_syncAK47ToTerrorist(s_terrorist1Gun1AK47,  s_terrorist1Gun1,  s_terrorist1Gun1AK47PosOffset,  s_terrorist1Gun1AK47RotOffset);
        BedroomScene_syncAK47ToTerrorist(s_terrorist1Kick1AK47, s_terrorist1Kick1, s_terrorist1Kick1AK47PosOffset, s_terrorist1Kick1AK47RotOffset);
    }

    // freeze dying man 1 on last frame when done
    if (!s_dyingMan1Stopped && s_dyingForwardMan1 && s_dyingForwardMan1->animator && now >= 241.2f)
    {
        float dur = E_animator_getDuration(s_dyingForwardMan1->animator);
        if (now >= 241.2f + dur)
        {
            s_dyingMan1Stopped = 1;
            E_animator_setTime (s_dyingForwardMan1->animator, dur - 0.05f);
            E_animator_setSpeed(s_dyingForwardMan1->animator, 0.0f);
        }
    }

    // freeze kick1 on last frame when done
    if (!s_kick1Stopped && s_terrorist1Kick1 && s_terrorist1Kick1->animator && now >= 241.2f)
    {
        float dur = E_animator_getDuration(s_terrorist1Kick1->animator);
        if (now >= 241.2f + dur)
        {
            s_kick1Stopped = 1;
            E_animator_setTime (s_terrorist1Kick1->animator, dur - 0.05f);
            E_animator_setSpeed(s_terrorist1Kick1->animator, 0.0f);
        }
    }

    // 4:06 - 4:06.25  glitch
    if (now >= 246.2f && now < 246.45f)
        E_postProcess_setCCTVGlitch(1, 1.0f);
    else
        E_postProcess_setCCTVGlitch(0, 0.0f);

    // 4:06 - swap to group 2, start dying lady 1 animation
    if (!s_group2Applied && now >= 246.2f)
    {
        s_group2Applied = 1;
        if (s_dyingLadyForward1) E_sceneObject_setActive(s_dyingLadyForward1, 1);
        if (s_terrorist1Gun2)    E_sceneObject_setActive(s_terrorist1Gun2,    1);
        if (s_terrorist1Kick2)   E_sceneObject_setActive(s_terrorist1Kick2,   1);
        BedroomScene_syncAK47ToTerrorist(s_terrorist1Gun2AK47,  s_terrorist1Gun2,  s_terrorist1Gun2AK47PosOffset,  s_terrorist1Gun2AK47RotOffset);
        BedroomScene_syncAK47ToTerrorist(s_terrorist1Kick2AK47, s_terrorist1Kick2, s_terrorist1Kick2AK47PosOffset, s_terrorist1Kick2AK47RotOffset);
        if (s_pl_gun2)           E_pointLight_activate  (s_pl_gun2);
        if (s_pl_gun4)           E_pointLight_activate(s_pl_gun4);
        if (s_dyingForwardMan1)  E_sceneObject_setActive(s_dyingForwardMan1,  0);
        if (s_terrorist1Gun1)    E_sceneObject_setActive(s_terrorist1Gun1,    0);
        if (s_terrorist1Kick1)   E_sceneObject_setActive(s_terrorist1Kick1,   0);
        if (s_terrorist1Gun1AK47)  E_sceneObject_setActive(s_terrorist1Gun1AK47,  0);
        if (s_terrorist1Kick1AK47) E_sceneObject_setActive(s_terrorist1Kick1AK47, 0);
        if (s_pl_gun1)           E_pointLight_deactivate(s_pl_gun1);
        if (s_pl_gun5)           E_pointLight_deactivate(s_pl_gun5);
        if (s_dyingLadyForward1 && s_dyingLadyForward1->animator)
            E_animator_setSpeed(s_dyingLadyForward1->animator, 1.0f);
        if (s_terrorist1Kick2 && s_terrorist1Kick2->animator)
            E_animator_setSpeed(s_terrorist1Kick2->animator, 1.0f);
    }
    if (now >= 246.2f)
    {
        BedroomScene_syncAK47ToTerrorist(s_terrorist1Gun2AK47,  s_terrorist1Gun2,  s_terrorist1Gun2AK47PosOffset,  s_terrorist1Gun2AK47RotOffset);
        BedroomScene_syncAK47ToTerrorist(s_terrorist1Kick2AK47, s_terrorist1Kick2, s_terrorist1Kick2AK47PosOffset, s_terrorist1Kick2AK47RotOffset);
    }

    // freeze kick2 on last frame when done
    if (!s_kick2Stopped && s_terrorist1Kick2 && s_terrorist1Kick2->animator && now >= 246.2f)
    {
        float dur = E_animator_getDuration(s_terrorist1Kick2->animator);
        if (now >= 246.2f + dur)
        {
            s_kick2Stopped = 1;
            E_animator_setTime (s_terrorist1Kick2->animator, dur - 0.05f);
            E_animator_setSpeed(s_terrorist1Kick2->animator, 0.0f);
            if (s_pl_gun3) E_pointLight_activate(s_pl_gun3);
        }
    }

    // 4:38 - 4:40  waiter starts walking: show, play, freeze, hide
    if (!s_waiterStartWalkApplied && now >= 278.2f)
    {
        s_waiterStartWalkApplied = 1;
        if (s_waiterStartWalking) E_sceneObject_setActive(s_waiterStartWalking, 1);
        if (s_waiterStartWalking && s_waiterStartWalking->animator)
            E_animator_setSpeed(s_waiterStartWalking->animator, 1.0f);
    }
    if (!s_waiterStartWalkStopped && s_waiterStartWalking && s_waiterStartWalking->animator && now >= 278.2f)
    {
        float dur = E_animator_getDuration(s_waiterStartWalking->animator);
        if (now >= 278.2f + dur)
        {
            s_waiterStartWalkStopped = 1;
            E_animator_setTime (s_waiterStartWalking->animator, dur - 0.05f);
            E_animator_setSpeed(s_waiterStartWalking->animator, 0.0f);
        }
    }
    if (now >= 280.2f && s_waiterStartWalking)
        E_sceneObject_setActive(s_waiterStartWalking, 0);

    // 4:54 - 4:57  waiter bending: show, play at 0.4x, freeze, hide
    if (!s_waiterBendApplied && now >= 294.2f)
    {
        s_waiterBendApplied = 1;
        if (s_waiterBending) E_sceneObject_setActive(s_waiterBending, 1);
        if (s_waiterBending && s_waiterBending->animator)
            E_animator_setSpeed(s_waiterBending->animator, 0.4f);
    }
    if (!s_waiterBendStopped && s_waiterBending && s_waiterBending->animator && now >= 294.2f)
    {
        float dur = E_animator_getDuration(s_waiterBending->animator);
        if (now >= 294.2f + dur / 0.4f)
        {
            s_waiterBendStopped = 1;
            E_animator_setTime (s_waiterBending->animator, dur - 0.05f);
            E_animator_setSpeed(s_waiterBending->animator, 0.0f);
        }
    }
    if (now >= 297.2f && s_waiterBending)
        E_sceneObject_setActive(s_waiterBending, 0);

    // 4:59 - 5:00  waiter with baby: show, lerp pos, play at 0.5x, freeze, hide
    if (!s_waiterWithBabyApplied && now >= 299.2f)
    {
        s_waiterWithBabyApplied = 1;
        if (s_waiterWithBaby) E_sceneObject_setActive(s_waiterWithBaby, 1);
        if (s_waiterWithBaby && s_waiterWithBaby->animator)
            E_animator_setSpeed(s_waiterWithBaby->animator, 0.5f);
    }
    if (now >= 299.2f && now < 300.2f && s_waiterWithBaby)
    {
        float t  = E_Utils_lerpBetweenTime(now, 299.2f, 300.2f, 0.0f, 1.0f);
        float px = -9.57f + t * (-6.370f - (-9.57f));
        float py = -3.83f + t * (-3.830f - (-3.83f));
        float pz = 32.57f + t * (25.270f -  32.57f);
        float ry = 144.00f + t * (144.77f - 144.00f);
        E_sceneObject_setPosition(s_waiterWithBaby, {px, py, pz});
        E_sceneObject_setRotation(s_waiterWithBaby, {0.00f, ry, 0.00f});
    }
    if (!s_waiterWithBabyStopped && s_waiterWithBaby && s_waiterWithBaby->animator && now >= 299.2f)
    {
        float dur = E_animator_getDuration(s_waiterWithBaby->animator);
        if (now >= 299.2f + dur / 0.5f)
        {
            s_waiterWithBabyStopped = 1;
            E_animator_setTime (s_waiterWithBaby->animator, dur - 0.05f);
            E_animator_setSpeed(s_waiterWithBaby->animator, 0.0f);
        }
    }
    if (now >= 300.2f && s_waiterWithBaby)
        E_sceneObject_setActive(s_waiterWithBaby, 0);

    // 4:51 - 4:54  terrorist walks on spline 1
    if (now >= 291.2f && now < 294.2f)
    {
        if (s_terrorist2Walking && s_splineTerroristComing_1)
        {
            E_sceneObject_setActive(s_terrorist2Walking, 1);
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineTerroristComing_1, 291.2f, 294.2f, 1.0f);
            E_sceneObject_setPosition(s_terrorist2Walking, pos);
            E_sceneObject_setRotation(s_terrorist2Walking, E_spline_getRotationAtT(s_splineTerroristComing_1,
                E_Utils_lerpBetweenTime(now, 291.2f, 294.2f, 0.0f, 1.0f)));
            BedroomScene_syncAK47ToTerrorist(s_terrorist2WalkingAK47, s_terrorist2Walking, s_terrorist2WalkingAK47PosOffset, s_terrorist2WalkingAK47RotOffset);
        }
    }
    else if (now >= 294.2f && now < 297.2f)
    {
        if (s_terrorist2Walking) E_sceneObject_setActive(s_terrorist2Walking, 0);
        if (s_terrorist2WalkingAK47) E_sceneObject_setActive(s_terrorist2WalkingAK47, 0);
    }

    // 4:57 - 4:59  terrorist walks on spline 2
    if (now >= 297.2f && now < 299.2f)
    {
        if (s_terrorist2Walking && s_splineTerroristComing_2)
        {
            E_sceneObject_setActive(s_terrorist2Walking, 1);
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineTerroristComing_2, 297.2f, 299.2f, 1.0f);
            E_sceneObject_setPosition(s_terrorist2Walking, pos);
            E_sceneObject_setRotation(s_terrorist2Walking, E_spline_getRotationAtT(s_splineTerroristComing_2,
                E_Utils_lerpBetweenTime(now, 297.2f, 299.2f, 0.0f, 1.0f)));
            BedroomScene_syncAK47ToTerrorist(s_terrorist2WalkingAK47, s_terrorist2Walking, s_terrorist2WalkingAK47PosOffset, s_terrorist2WalkingAK47RotOffset);
        }
    }
    else if (now >= 299.2f && s_terrorist2Walking)
    {
        E_sceneObject_setActive(s_terrorist2Walking, 0);
        if (s_terrorist2WalkingAK47) E_sceneObject_setActive(s_terrorist2WalkingAK47, 0);
    }

    // 5:03 - 5:09  terrorist walks inside room on spline 1
    if (now >= 303.2f && now < 309.2f)
    {
        if (s_terrorist2Walking && s_splineTerrorInsideRoom1)
        {
            E_sceneObject_setActive(s_terrorist2Walking, 1);
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineTerrorInsideRoom1, 303.2f, 309.2f, 1.0f);
            E_sceneObject_setPosition(s_terrorist2Walking, pos);
            E_sceneObject_setRotation(s_terrorist2Walking, E_spline_getRotationAtT(s_splineTerrorInsideRoom1,
                E_Utils_lerpBetweenTime(now, 303.2f, 309.2f, 0.0f, 1.0f)));
            BedroomScene_syncAK47ToTerrorist(s_terrorist2WalkingAK47, s_terrorist2Walking, s_terrorist2WalkingAK47PosOffset, s_terrorist2WalkingAK47RotOffset);
        }
    }
    else if (now >= 309.2f && now < 321.2f && s_terrorist2Walking)
    {
        E_sceneObject_setActive(s_terrorist2Walking, 0);
        if (s_terrorist2WalkingAK47) E_sceneObject_setActive(s_terrorist2WalkingAK47, 0);
    }

    // 5:21 - 5:26  terrorist walks inside room on spline 2
    if (now >= 321.2f && now < 326.2f)
    {
        if (s_terrorist2Walking && s_splineTerrorInsideRoom2)
        {
            E_sceneObject_setActive(s_terrorist2Walking, 1);
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineTerrorInsideRoom2, 321.2f, 326.2f, 1.0f);
            E_sceneObject_setPosition(s_terrorist2Walking, pos);
            E_sceneObject_setRotation(s_terrorist2Walking, E_spline_getRotationAtT(s_splineTerrorInsideRoom2,
                E_Utils_lerpBetweenTime(now, 321.2f, 326.2f, 0.0f, 1.0f)));
            BedroomScene_syncAK47ToTerrorist(s_terrorist2WalkingAK47, s_terrorist2Walking, s_terrorist2WalkingAK47PosOffset, s_terrorist2WalkingAK47RotOffset);
        }
    }
    else if (now >= 326.2f && s_terrorist2Walking)
    {
        E_sceneObject_setActive(s_terrorist2Walking, 0);
        if (s_terrorist2WalkingAK47) E_sceneObject_setActive(s_terrorist2WalkingAK47, 0);
    }

    // 5:00  terrorist1 start walking appears
    static int s_terrorist1StartWalkApplied = 0;
    if (!s_terrorist1StartWalkApplied && now >= 300.2f)
    {
        s_terrorist1StartWalkApplied = 1;
        if (s_terrorist1StartWalk)
        {
            E_sceneObject_setPosition(s_terrorist1StartWalk, {-22.49f, 0.00f, 36.27f});
            E_sceneObject_setRotation(s_terrorist1StartWalk, {0.00f, 95.00f, 0.00f});
            E_sceneObject_setScale   (s_terrorist1StartWalk, 10.00f);
            E_sceneObject_setActive  (s_terrorist1StartWalk, 1);
            BedroomScene_syncAK47ToTerrorist(s_terrorist1StartWalkAK47, s_terrorist1StartWalk, s_terrorist1StartWalkAK47PosOffset, s_terrorist1StartWalkAK47RotOffset);
            if (s_terrorist1StartWalk->animator)
                E_animator_setSpeed(s_terrorist1StartWalk->animator, 1.0f);
        }
    }
    if (now >= 300.2f && now < 301.7f)
        BedroomScene_syncAK47ToTerrorist(s_terrorist1StartWalkAK47, s_terrorist1StartWalk, s_terrorist1StartWalkAK47PosOffset, s_terrorist1StartWalkAK47RotOffset);
    if (now >= 301.7f && s_terrorist1StartWalk)
    {
        E_sceneObject_setActive(s_terrorist1StartWalk, 0);
        if (s_terrorist1StartWalkAK47) E_sceneObject_setActive(s_terrorist1StartWalkAK47, 0);
    }

    // 4:59 - 5:00  almirah door swap
    if (now >= 299.2f && now < 300.2f)
    {
        if (s_almirahOpen)       E_sceneObject_setActive(s_almirahOpen,       1);
        if (s_almirahDoorClosed) E_sceneObject_setActive(s_almirahDoorClosed, 0);
    }
    else if (now >= 300.2f)
    {
        if (s_almirahOpen)       E_sceneObject_setActive(s_almirahOpen,       0);
        if (s_almirahDoorClosed) E_sceneObject_setActive(s_almirahDoorClosed, 1);
    }

    // 5:10 - 5:12  cradle gun light
    if (now >= 310.0f && now < 312.0f)
    {
        if (s_pl_cradleGun) E_pointLight_activate(s_pl_cradleGun);
    }
    else
    {
        if (s_pl_cradleGun) E_pointLight_deactivate(s_pl_cradleGun);
    }

    // freeze dying lady 1 on last frame when done
    if (!s_dyingLady1Stopped && s_dyingLadyForward1 && s_dyingLadyForward1->animator && now >= 246.2f)
    {
        float dur = E_animator_getDuration(s_dyingLadyForward1->animator);
        if (now >= 246.2f + dur)
        {
            s_dyingLady1Stopped = 1;
            E_animator_setTime (s_dyingLadyForward1->animator, dur - 0.05f);
            E_animator_setSpeed(s_dyingLadyForward1->animator, 0.0f);
        }
    }

    // 5:00  show waiter hand on baby mouth
    static int s_waiterHandOnBabyMouthApplied = 0;
    if (!s_waiterHandOnBabyMouthApplied && now >= 300.2f)
    {
        s_waiterHandOnBabyMouthApplied = 1;
        if (s_waiterHandOnBabyMouth) E_sceneObject_setActive(s_waiterHandOnBabyMouth, 1);
    }

    // 5:25  swap waiter hand models
    static int s_waiterHandSwapApplied = 0;
    if (!s_waiterHandSwapApplied && now >= 325.2f)
    {
        s_waiterHandSwapApplied = 1;
        if (s_waiterHandRemoveFromBaby) E_sceneObject_setActive(s_waiterHandRemoveFromBaby, 1);
        if (s_waiterHandOnBabyMouth)    E_sceneObject_setActive(s_waiterHandOnBabyMouth,    0);
    }
}

void BedroomScene_updateCamera()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    static int s_cctvApplied      = 0;
    static int s_normalApplied    = 0;
    static int s_roomCCTVApplied  = 0;
    static int s_waiterEvacuatingReset = 0;

    // 4:01 - 4:11  Terrorist Cam 2
    if (now >= 241.2f && now < 246.2f)
    {
        if (!s_cctvApplied)
        {
            s_cctvApplied   = 1;
            s_normalApplied = 0;
            E_postProcess_setFXAA(1);
            E_postProcess_setSaturation(1.00f);
            E_postProcess_setBlur(0.00f);
            E_postProcess_setVignette(0.51f, 2.36f);
            E_postProcess_setDOF(0, 0.00f, 0.00f, 0.00f);
            E_postProcess_setGodRays(0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
            E_postProcess_setBloom(0, 1.00f, 3.00f);
            E_postProcess_setCCTV(1, 0.0010f, 1.00f, 0.20f, 1.00f);
            E_postProcess_setBarrel(1, -0.3900f, 0.5000f);
        }
        E_setFOV(25.0f);
        E_camera_setPosition(-26.390f, 10.359f, -85.921f);
        E_camera_lookAtPoint(14.623f, 2.582f, -83.817f);
    }
    
    if (now >= 246.2f && now < 250.0f)
    {
        if (!s_normalApplied)
        {
            s_normalApplied = 1;
            s_cctvApplied   = 0;
            E_postProcess_setCCTV(0, 0.0010f, 1.00f, 0.20f, 1.00f);
            E_postProcess_setBarrel(0, -0.3900f, 0.5000f);
        }
        E_camera_setProjection(29.0f, 0.10f, 1000.00f);
        E_camera_setPosition(-5.156f, 0.577f, -79.900f);
        E_camera_lookAtPoint(3.725f, 0.640f, -81.604f);
    }

    if (now >= 250.0f && now < 251.2f)
    {
        if (!s_cctvApplied)
        {
            s_cctvApplied   = 1;
            s_normalApplied = 0;
            E_postProcess_setFXAA(1);
            E_postProcess_setSaturation(1.00f);
            E_postProcess_setBlur(0.00f);
            E_postProcess_setVignette(0.51f, 2.36f);
            E_postProcess_setDOF(0, 0.00f, 0.00f, 0.00f);
            E_postProcess_setGodRays(0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
            E_postProcess_setBloom(0, 1.00f, 3.00f);
            E_postProcess_setCCTV(1, 0.0010f, 1.00f, 0.20f, 1.00f);
            E_postProcess_setBarrel(1, -0.3900f, 0.5000f);
        }
        E_setFOV(60.0f);
        E_camera_setPosition(-26.390f, 10.359f, -85.921f);
        E_camera_lookAtPoint(14.623f, 2.582f, -83.817f);
    }

    // 4:11 - 4:21  Terrorist Cam 2 pans to waiter
    if (now >= 251.2f && now < 261.2f)
    {
        float fov = (now >= 258.2f) ? E_Utils_lerpBetweenTime(now, 258.2f, 261.2f, 60.0f, 12.0f) : 60.0f;
        E_setFOV(fov);
        E_camera_setPosition(-26.390f, 10.359f, -85.921f);
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineTerroristKickingToWaiterE, 251.2f, 261.2f, 1.0f);
        E_camera_lookAtPoint(lookAt);
    }

    // 4:21 - 4:25  Cam 3 Waiter Establish
    if (now >= 261.2f && now < 265.2f)
    {
        if (s_sl_waiter)
        {
            E_spotLight_setPosition  (s_sl_waiter, {-23.24f, 11.18f, 95.26f});
            E_spotLight_setDirection (s_sl_waiter, {-0.05f, -0.95f, -0.32f});
            E_spotLight_setColor     (s_sl_waiter, {1.00f, 1.00f, 1.00f});
            E_spotLight_setIntensity (s_sl_waiter, 0.22f);
            E_spotLight_setConeAngles(s_sl_waiter, 0.00f, 25.30f);
            E_spotLight_setRange     (s_sl_waiter, 91.14f);
            s_sl_waiter->flicker       = 1;
            s_sl_waiter->flickerSpeed  = 3.000f;
            s_sl_waiter->flickerAmount = 0.500f;
            s_sl_waiter->castShadow    = 1;
        }

        if (!s_waiterEvacuatingReset && s_waiterEvacuating && s_waiterEvacuating->animator)
        {
            s_waiterEvacuatingReset = 1;
            E_animator_setTime (s_waiterEvacuating->animator, 1.0f);
            E_animator_setSpeed(s_waiterEvacuating->animator, 1.0f);
        }

        E_camera_setProjection(65.48f, 0.10f, 1000.00f);
        E_camera_setPosition(-18.338f, 11.520f, 94.193f);
        E_camera_lookAtPoint(-20.501f, -0.422f, 86.558f);
    }

    // 4:25 - 4:30  Waiter's POV  (CCTV off)
    if (now >= 265.2f && now < 270.2f)
    {
        if (!s_normalApplied)
        {
            s_normalApplied = 1;
            s_cctvApplied   = 0;
            E_postProcess_setCCTV(0, 0.0010f, 1.00f, 0.20f, 1.00f);
            E_postProcess_setBarrel(0, -0.3900f, 0.5000f);
            E_postProcess_setVignette(0.0f, 1.0f);
        }
        if (s_sl_waiter)
        {
            s_sl_waiter->flicker       = 1;
            s_sl_waiter->flickerSpeed  = 3.000f;
            s_sl_waiter->flickerAmount = 0.500f;
            s_sl_waiter->castShadow    = 1;
        }
        E_setFOV(E_Utils_lerpBetweenTime(now,  265.2f, 270.2f, 27.0f, 65.0f));
        E_camera_setPosition(-24.130f, 8.816f, 91.600f);
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineToWaiterToHeadUp, 265.2f, 270.2f, 1.0f);
        E_camera_lookAtPoint(lookAt);
    }

    // 4:30 - 4:38  Pan to crying door
    if (now >= 270.2f && now < 278.2f)
    {
        E_setFOV(65.0f);
        float t  = E_Utils_lerpBetweenTime(now, 270.2f, 278.2f, 0.0f, 1.0f);
        float cx = -24.130f + t * (-22.950f - (-24.130f));
        float cy =   8.816f + t * (  6.120f -   8.816f);
        float cz =  91.600f + t * ( 34.636f -  91.600f);
        E_camera_setPosition(cx, cy, cz);
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineToCryingDoorLookAt, 270.2f, 278.2f, 1.0f);
        E_camera_lookAtPoint(lookAt);
    }

    // 4:38 - 4:40  CCTV room overview
    if (now >= 278.2f && now < 280.2f)
    {
        if (!s_roomCCTVApplied)
        {
            s_roomCCTVApplied = 1;
            E_postProcess_setFXAA(1);
            E_postProcess_setSaturation(0.75f);
            E_postProcess_setBlur(0.00f);
            E_postProcess_setVignette(0.51f, 2.36f);
            E_postProcess_setDOF(0, 0.00f, 0.00f, 0.00f);
            E_postProcess_setGodRays(0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
            E_postProcess_setBloom(0, 1.00f, 3.00f);
            E_postProcess_setCCTV(1, 0.0010f, 1.00f, 0.20f, 1.00f);
            E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
        }
        E_camera_setPosition(-20.639f, 12.925f, 48.622f);
        E_camera_lookAtPoint(-19.562f, 3.882f, 35.838f);
    }

    // 4:40 - 4:42  Waiter enters room
    if (now >= 280.2f && now < 282.2f)
    {
        E_postProcess_setCCTV(0, 0.0010f, 1.00f, 0.20f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
        E_postProcess_setVignette(0.51f, 2.36f);
        E_setFOV(65.0f);
        vmath::vec3 pos    = E_spline_getPositionBetweenTime(s_splineWaiterEntersRoom,         280.2f, 282.2f, 1.0f);
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineWaiterSeesDeadBodyLookAt, 280.2f, 282.2f, 1.0f);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(lookAt);
    }

    // // 4:42 - 4:44  Quick zoom in and out (1 sec each)
    // if (now >= 282.2f && now < 284.2f)
    // {
    //     E_postProcess_setCCTV(0, 0.0010f, 1.00f, 0.20f, 1.00f);
    //     E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
    //     E_postProcess_setVignette(3.40f, 1.91f);
    //     float fov = (now < 283.2f)
    //         ? E_Utils_lerpBetweenTime(now, 282.2f, 283.2f, 65.0f, 20.0f)
    //         : E_Utils_lerpBetweenTime(now, 283.2f, 284.2f, 20.0f, 65.0f);
    //     E_setFOV(fov);
    //     E_camera_setPosition(-13.023f, 4.076f, 32.915f);
    //     E_camera_lookAtPoint(-14.281f, 0.731f, 37.004f);
    // }

    // 4:44 - 4:49  Dead body pause to cradle
    if (now >= 284.2f && now < 289.2f)
    {
        E_postProcess_setCCTV(0, 0.0010f, 1.00f, 0.20f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
        E_postProcess_setVignette(0.51f, 2.36f);
        E_setFOV(65.0f);
        vmath::vec3 pos    = E_spline_getPositionBetweenTime(s_splineDeadBodyPauseToCradle, 284.2f, 289.2f, 1.0f);
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineBodyToCradleLookAt,    284.2f, 289.2f, 1.0f);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(lookAt);
    }

    // // 4:49 - 4:51  Quick zoom in
    // if (now >= 289.2f && now < 291.2f)
    // {
    //     E_postProcess_setCCTV(0, 0.0010f, 1.00f, 0.20f, 1.00f);
    //     E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
    //     E_postProcess_setVignette(3.40f, 1.91f);
    //     E_setFOV(E_Utils_lerpBetweenTime(now, 289.2f, 291.2f, 65.0f, 20.0f));
    //     vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineDeadBodyPauseToCradle, 284.2f, 289.2f, 1.0f);
    //     E_camera_setPosition(pos[0], pos[1], pos[2]);
    //     E_camera_lookAtPoint(2.993f, 1.054f, 34.757f);
    // }

    // 4:51 - 4:54  CCTV room overview
    if (now >= 291.2f && now < 294.2f)
    {
        E_postProcess_setCCTV(1, 0.0010f, 1.00f, 0.20f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
        E_postProcess_setVignette(0.51f, 2.36f);
        E_setFOV(30.0f);
        E_camera_setPosition(-20.639f, 12.925f, 48.622f);
        vmath::vec3 focusPt = s_terrorist2Walking->position;
        E_camera_lookAtPoint(focusPt[0], focusPt[1], focusPt[2]);
        if (s_terrorist2Walking)
            E_postProcess_applyDOF(s_terrorist2Walking->position, 5.0f, 28.74f);
    }

    // 4:54 - 4:57  Picks up baby
    if (now >= 294.2f && now < 297.2f)
    {
        E_postProcess_setCCTV(0, 0.0010f, 1.00f, 0.20f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
        E_postProcess_setVignette(0.51f, 2.36f);
        E_setFOV(65.0f);
        E_camera_setPosition(9.609f, 8.194f, 40.234f);
        E_camera_lookAtPoint(2.499f, 2.726f, 35.716f);
        E_postProcess_setDOF(0, 10.0f, 2.0f, 0);

        // if (s_waiterBending)
        //     E_postProcess_applyDOF(s_waiterBending->position, 5.0f, 28.74f);
    }

    // 4:57 - 4:59  CCTV room overview
    if (now >= 297.2f && now < 299.2f)
    {
        E_postProcess_setCCTV(1, 0.0010f, 1.00f, 0.20f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
        E_postProcess_setVignette(0.51f, 2.36f);
        E_camera_setPosition(-20.639f, 12.925f, 48.622f);
        E_setFOV(45.0f);
        vmath::vec3 focusPt = s_terrorist2Walking->position;
        E_camera_lookAtPoint(focusPt[0], focusPt[1], focusPt[2]);
        if (s_terrorist2Walking)
            E_postProcess_applyDOF(s_terrorist2Walking->position, 5.0f, 7.74f);
    }

    // 4:59 - 5:00  Entering almirah
    if (now >= 299.2f && now < 300.2f)
    {
        E_setFOV(65.0f);
        E_postProcess_setDOF(0, 10.0f, 2.0f, 0);
        E_postProcess_setCCTV(0, 0.0010f, 1.00f, 0.20f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
        E_postProcess_setVignette(0.51f, 2.36f);
        E_camera_setPosition(-16.435, 10.968, 39.532);
        E_camera_lookAtPoint(-6.202, 0.033, 26.813);
        E_camera_setProjection(92.26f, 0.10f, 1000.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.0500f);
    }

    // 5:00 - 5:01.5  CCTV room overview
    if (now >= 300.2f && now < 301.7f)
    {
        E_postProcess_setDOF(0, 10.0f, 2.0f, 0);
        E_postProcess_setCCTV(1, 0.0010f, 1.00f, 0.20f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
        E_postProcess_setVignette(0.51f, 2.36f);
        E_camera_setPosition(-20.639f, 12.925f, 48.622f);
        E_camera_lookAtPoint(-19.562f, 3.882f, 35.838f);
    }

    // 5:01.5 - 5:38  Inside almirah
    if (now >= 301.7f && now <= 338.2f)
    {
        E_postProcess_setDOF(0, 10.0f, 2.0f, 0);
        E_postProcess_setCCTV(0, 0.0010f, 1.00f, 0.20f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.2420f);
        E_postProcess_setVignette(0.51f, 2.36f);
        if (s_sl_almirah) E_spotLight_activate(s_sl_almirah);

        if (now >= 301.7f && now < 312.2f)
        {
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineToAlmirahInterior, 301.7f, 312.2f, 1.0f);
            E_camera_setPosition(pos[0], pos[1], pos[2]);
            E_camera_lookAtPoint(-8.455f, 7.171f, 27.394f);
        }
        else if (now >= 312.2f && now < 319.2f)
        {
            vmath::vec3 pos    = E_spline_getPositionBetweenTime(s_splineToBabyMouthCoveredCamMove, 312.2f, 319.2f, 1.0f);
            vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineToBabyMouthCoveredLookat,  312.2f, 319.2f, 1.0f);
            E_camera_setPosition(pos[0], pos[1], pos[2]);
            E_camera_lookAtPoint(lookAt);
        }
        else if (now >= 319.2f && now < 325.2f)
        {
            vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineToTransitionLookat, 319.2f, 325.2f, 1.0f);
            E_camera_setPosition(-9.876f, 7.364f, 27.406f);
            E_camera_lookAtPoint(lookAt);
        }
        else if (now >= 328.2f && now < 334.2f)
        {
            vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineAfterDoorCloseLookAt, 328.2f, 334.2f, 1.0f);
            E_camera_setPosition(-9.876f, 7.364f, 27.406f);
            E_camera_lookAtPoint(lookAt);
        }
    }

    if (now > 338.2f)
    {
        if (s_sl_almirah) E_spotLight_deactivate(s_sl_almirah);
    }
}

void BedroomScene_update()
{
    BedroomScene_updateWorld();
    BedroomScene_updateCamera();
}

void BedroomScene_uninit()
{
    if (s_bedroomScene)             { E_sceneObject_destroyResources(s_bedroomScene);             s_bedroomScene             = NULL; }
    if (s_deadIndianLadyPose)      { E_sceneObject_destroyResources(s_deadIndianLadyPose);      s_deadIndianLadyPose      = NULL; }
    if (s_waiterEvacuating)        { E_sceneObject_destroyResources(s_waiterEvacuating);        s_waiterEvacuating        = NULL; }
    if (s_waiterBending)           { E_sceneObject_destroyResources(s_waiterBending);           s_waiterBending           = NULL; }
    if (s_waiterStartWalking)      { E_sceneObject_destroyResources(s_waiterStartWalking);      s_waiterStartWalking      = NULL; }
    if (s_waiterWithBaby)          { E_sceneObject_destroyResources(s_waiterWithBaby);          s_waiterWithBaby          = NULL; }
    if (s_waiterHandOnBabyMouth)   { E_sceneObject_destroyResources(s_waiterHandOnBabyMouth);   s_waiterHandOnBabyMouth   = NULL; }
    if (s_waiterHandRemoveFromBaby){ E_sceneObject_destroyResources(s_waiterHandRemoveFromBaby);s_waiterHandRemoveFromBaby= NULL; }
    if (s_babySmiling)             { E_sceneObject_destroyResources(s_babySmiling);             s_babySmiling             = NULL; }
    if (s_babyCrying)              { E_sceneObject_destroyResources(s_babyCrying);              s_babyCrying              = NULL; }
    if (s_crawlingMan1)            { E_sceneObject_destroyResources(s_crawlingMan1);            s_crawlingMan1            = NULL; }
    if (s_crawlingGirl1)           { E_sceneObject_destroyResources(s_crawlingGirl1);           s_crawlingGirl1           = NULL; }
    if (s_terrorist1Kick1)         { E_sceneObject_destroyResources(s_terrorist1Kick1);         s_terrorist1Kick1         = NULL; }
    if (s_terrorist1Kick2)         { E_sceneObject_destroyResources(s_terrorist1Kick2);         s_terrorist1Kick2         = NULL; }
    if (s_terrorist1Gun1)          { E_sceneObject_destroyResources(s_terrorist1Gun1);          s_terrorist1Gun1          = NULL; }
    if (s_terrorist1Gun2)          { E_sceneObject_destroyResources(s_terrorist1Gun2);          s_terrorist1Gun2          = NULL; }
    if (s_dyingForwardMan1)        { E_sceneObject_destroyResources(s_dyingForwardMan1);        s_dyingForwardMan1        = NULL; }
    if (s_dyingLadyForward1)       { E_sceneObject_destroyResources(s_dyingLadyForward1);       s_dyingLadyForward1       = NULL; }
    if (s_terrorist1StartWalk)     { E_sceneObject_destroyResources(s_terrorist1StartWalk);     s_terrorist1StartWalk     = NULL; }
    if (s_almirahDoorClosed)       { E_sceneObject_destroyResources(s_almirahDoorClosed);       s_almirahDoorClosed       = NULL; }
    if (s_almirahOpen)             { E_sceneObject_destroyResources(s_almirahOpen);             s_almirahOpen             = NULL; }
    if (s_terrorist2Walking)       { E_sceneObject_destroyResources(s_terrorist2Walking);       s_terrorist2Walking       = NULL; }
    if (s_terrorist1Kick1AK47)     { E_sceneObject_destroyResources(s_terrorist1Kick1AK47);     s_terrorist1Kick1AK47     = NULL; }
    if (s_terrorist1Kick2AK47)     { E_sceneObject_destroyResources(s_terrorist1Kick2AK47);     s_terrorist1Kick2AK47     = NULL; }
    if (s_terrorist1Gun1AK47)      { E_sceneObject_destroyResources(s_terrorist1Gun1AK47);      s_terrorist1Gun1AK47      = NULL; }
    if (s_terrorist1Gun2AK47)      { E_sceneObject_destroyResources(s_terrorist1Gun2AK47);      s_terrorist1Gun2AK47      = NULL; }
    if (s_terrorist1StartWalkAK47) { E_sceneObject_destroyResources(s_terrorist1StartWalkAK47); s_terrorist1StartWalkAK47 = NULL; }
    if (s_terrorist2WalkingAK47)   { E_sceneObject_destroyResources(s_terrorist2WalkingAK47);   s_terrorist2WalkingAK47   = NULL; }

    if (s_pl_nearWaiter)    { E_pointLight_deactivate(s_pl_nearWaiter);    s_pl_nearWaiter    = NULL; }
    if (s_pl_cradleGun)     { E_pointLight_deactivate(s_pl_cradleGun);     s_pl_cradleGun     = NULL; }
    if (s_pl_gun1)          { E_pointLight_deactivate(s_pl_gun1);          s_pl_gun1          = NULL; }
    if (s_pl_gun2)          { E_pointLight_deactivate(s_pl_gun2);          s_pl_gun2          = NULL; }
    if (s_pl_gun3)          { E_pointLight_deactivate(s_pl_gun3);          s_pl_gun3          = NULL; }
    if (s_pl_gun4)          { E_pointLight_deactivate(s_pl_gun4);          s_pl_gun4          = NULL; }
    if (s_pl_gun5)          { E_pointLight_deactivate(s_pl_gun5);          s_pl_gun5          = NULL; }
    if (s_pl_ceiling_right) { E_pointLight_deactivate(s_pl_ceiling_right); s_pl_ceiling_right = NULL; }
    if (s_pl_ceiling_left)  { E_pointLight_deactivate(s_pl_ceiling_left);  s_pl_ceiling_left  = NULL; }
    if (s_pl_ceiling_corner){ E_pointLight_deactivate(s_pl_ceiling_corner);s_pl_ceiling_corner= NULL; }
    if (s_pl_corner_near)   { E_pointLight_deactivate(s_pl_corner_near);   s_pl_corner_near   = NULL; }
    if (s_pl_corner_far)    { E_pointLight_deactivate(s_pl_corner_far);    s_pl_corner_far    = NULL; }
    if (s_pl_warm_fill)     { E_pointLight_deactivate(s_pl_warm_fill);     s_pl_warm_fill     = NULL; }
    if (s_pl_outsiderooms)  { E_pointLight_deactivate(s_pl_outsiderooms);  s_pl_outsiderooms  = NULL; }
    if (s_pl_waiterBaby)    { E_pointLight_deactivate(s_pl_waiterBaby);    s_pl_waiterBaby    = NULL; }

    if (s_sl_outside_1)     { E_spotLight_deactivate(s_sl_outside_1);     s_sl_outside_1     = NULL; }
    if (s_sl_outside_2)     { E_spotLight_deactivate(s_sl_outside_2);     s_sl_outside_2     = NULL; }
    if (s_sl_outside_3)     { E_spotLight_deactivate(s_sl_outside_3);     s_sl_outside_3     = NULL; }
    if (s_sl_main_bedroom)  { E_spotLight_deactivate(s_sl_main_bedroom);  s_sl_main_bedroom  = NULL; }
    if (s_sl_outside_5)     { E_spotLight_deactivate(s_sl_outside_5);     s_sl_outside_5     = NULL; }
    if (s_sl_outside_7)     { E_spotLight_deactivate(s_sl_outside_7);     s_sl_outside_7     = NULL; }
    if (s_sl_outside_8)     { E_spotLight_deactivate(s_sl_outside_8);     s_sl_outside_8     = NULL; }
    if (s_sl_cradle)        { E_spotLight_deactivate(s_sl_cradle);        s_sl_cradle        = NULL; }
    if (s_sl_almirah)       { E_spotLight_deactivate(s_sl_almirah);       s_sl_almirah       = NULL; }
    if (s_sl_waiter)        { E_spotLight_deactivate(s_sl_waiter);        s_sl_waiter        = NULL; }
    if (s_sl_ladyDeadbody)  { E_spotLight_deactivate(s_sl_ladyDeadbody);  s_sl_ladyDeadbody  = NULL; }
    if (s_sl_10)            { E_spotLight_deactivate(s_sl_10);            s_sl_10            = NULL; }
    if (s_sl_10_copy_copy)  { E_spotLight_deactivate(s_sl_10_copy_copy);  s_sl_10_copy_copy  = NULL; }
    if (s_sl_10_copy)       { E_spotLight_deactivate(s_sl_10_copy);       s_sl_10_copy       = NULL; }
}
