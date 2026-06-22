#include "../public/05-CamaHospital.h"
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

static E_sceneObject* s_hospitalScene = NULL;
static E_sceneObject* s_ceiling = NULL;
static E_sceneObject* s_skySphere = NULL;
static E_sceneObject* s_policeJeep                        = NULL;
static E_sceneObject* s_policeJeep2                       = NULL;
static E_sceneObject* s_policeSupportFir                  = NULL;
static E_sceneObject* s_policeShooting                    = NULL;
static E_sceneObject* s_policeSupportFirShotgun           = NULL;
static E_sceneObject* s_policeShootingPistol              = NULL;
static E_sceneObject* s_policeStandingIdleWithPistol      = NULL;
static E_sceneObject* s_policeKaka1IdleCrouchingAiming    = NULL;
static E_sceneObject* s_policeKaka1PistolIdle             = NULL;
static E_sceneObject* s_policeOneRifleAimingIdle          = NULL;
static E_sceneObject* s_policeOnePistolKneelingIdle       = NULL;
static E_sceneObject* s_policeOnePistolStandingIdle       = NULL;
static E_sceneObject* s_pregnentLady           = NULL;
static E_sceneObject* s_nurseStandingNearPhone = NULL;
static E_sceneObject* s_nurseRunInside         = NULL;
static E_sceneObject* s_nurseAtReception       = NULL;

static E_spline*      s_splineNurseRunFromPhone     = NULL;
static E_spline*      s_spline_GateTop             = NULL;
static E_spline*      s_spline_ToPhone             = NULL;
static E_spline*      s_splineToNurseRun           = NULL;
static E_spline*      s_splineToNurseLeg_lookAt    = NULL;
static E_spline*      s_splineLightsOff                  = NULL;
static E_spline*      s_splineTerroristReveal_T1_lookat  = NULL;
static E_spline*      s_splineTerroristReveal_T2_lookat  = NULL;

static E_pointLight*  s_pl_10                   = NULL;
static E_pointLight*  s_pl_receptionTable        = NULL;

static E_pointLight*  s_pl_OTRight           = NULL;
static E_pointLight*  s_pl_OTLeft            = NULL;
static E_pointLight*  s_pl_OTFront1          = NULL;
static E_pointLight*  s_pl_OTFront2          = NULL;
static E_pointLight*  s_pl_OTFront3          = NULL;
static E_pointLight*  s_pl_OTLeftCorridor1   = NULL;
static E_pointLight*  s_pl_OTLeftCorridor2   = NULL;
static E_pointLight*  s_pl_OTRightCorridor2  = NULL;
static E_pointLight*  s_pl_MainDoor          = NULL;

static E_emitter*     s_emitter_gateFire          = NULL;
static E_emitter*     s_emitter_gateSmoke         = NULL;

static E_sceneObject* s_secGuard1FallingBackDeath  = NULL;
static E_sceneObject* s_secGuard1FlyingBackDeath   = NULL;
static E_sceneObject* s_secGuard1IdleStanding      = NULL;
static E_sceneObject* s_secGuard1IdleStanding2     = NULL;

static E_sceneObject* s_channelGate_open                = NULL;
static E_sceneObject* s_channelGate_closed              = NULL;
static E_sceneObject* s_channelGate_broken              = NULL;

static E_sceneObject* s_terrorist1WalkingWithGun        = NULL;
static E_sceneObject* s_terrorist2WalkingWithRifle      = NULL;
static E_sceneObject* s_terrorist1Gunplay               = NULL;
static E_sceneObject* s_terrorist2HidingAndShooting     = NULL;
static E_sceneObject* s_terrorist1WalkingWithGunAK47    = NULL;
static E_sceneObject* s_terrorist2WalkingWithRifleAK47  = NULL;
static E_sceneObject* s_terrorist1GunplayAK47           = NULL;
static E_sceneObject* s_terrorist2HidingAndShootingAK47 = NULL;

static vmath::vec3    s_terrorist1WalkingWithGunAK47PosOffset    = vmath::vec3(-0.053f, -0.154f, 0.049f);
static vmath::vec3    s_terrorist1WalkingWithGunAK47RotOffset    = vmath::vec3(-31.00f, -122.00f, 0.00f);
static vmath::vec3    s_terrorist2WalkingWithRifleAK47PosOffset  = vmath::vec3(-0.067f, -0.151f, -0.011f);
static vmath::vec3    s_terrorist2WalkingWithRifleAK47RotOffset  = vmath::vec3(-43.00f, -142.00f, 0.00f);
static vmath::vec3    s_terrorist1GunplayAK47PosOffset           = vmath::vec3(-0.017f, -0.200f, 0.078f);
static vmath::vec3    s_terrorist1GunplayAK47RotOffset           = vmath::vec3(-25.00f, -175.00f, 0.00f);
static vmath::vec3    s_terrorist2HidingAndShootingAK47PosOffset = vmath::vec3(0.059f, -0.202f, -0.106f);
static vmath::vec3    s_terrorist2HidingAndShootingAK47RotOffset = vmath::vec3(-10.00f, -11.00f, 25.00f);
static vmath::vec3    s_policeSupportFirShotgunPosOffset         = vmath::vec3(-0.096f, 0.101f, -0.207f);
static vmath::vec3    s_policeSupportFirShotgunRotOffset         = vmath::vec3(-18.00f, -76.00f, -45.00f);
static vmath::vec3    s_policeShootingPistolPosOffset            = vmath::vec3(0.037f, 0.039f, -0.210f);
static vmath::vec3    s_policeShootingPistolRotOffset            = vmath::vec3(58.00f, -123.00f, 65.00f);

static E_sceneObject* s_adityaBreathingIdle             = NULL;
static E_sceneObject* s_doctorDoingDelivery             = NULL;
static E_sceneObject* s_kanganaIdleButAlert             = NULL;
static E_sceneObject* s_nurseBendingIdle                = NULL;

static void CamaHospital_syncAK47ToTerrorist(E_sceneObject* ak47, E_sceneObject* terrorist, vmath::vec3 posOffset, vmath::vec3 rotOffset)
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
    printf("%s handPos: %.3f %.3f %.3f\n", terrorist->name, handPos[0], handPos[1], handPos[2]);
    E_sceneObject_setPosition(ak47, handPos + posOffset);
    E_sceneObject_setRotation(ak47, terrorist->rotation + rotOffset);
    E_sceneObject_setActive  (ak47, 1);
}

static E_spotLight*   s_sl_delivery      = NULL;
static E_spotLight*   s_sl_police_1      = NULL;
static E_spotLight*   s_sl_police_1_copy = NULL;
static E_spotLight*   s_sl_police_top    = NULL;
static E_spotLight*   s_sl_onReception   = NULL;
static E_spotLight*   s_sl_OT_window     = NULL;
static E_spotLight*   s_sl_washroom      = NULL;
static E_spotLight*   s_sl_jeep_lamp_1   = NULL;
static E_spotLight*   s_sl_jeep_lamp_2   = NULL;

static E_pointLight*  s_pl_upper_1       = NULL;
static E_pointLight*  s_pl_upper_2       = NULL;
static E_pointLight*  s_pl_upper_3       = NULL;
static E_pointLight*  s_pl_upper_4       = NULL;
static E_pointLight*  s_pl_upper_5       = NULL;
static E_pointLight*  s_pl_medicine               = NULL;
static E_pointLight*  s_pl_pregnantLadyCloseup    = NULL;
static E_pointLight*  s_pl_T1_gun                 = NULL;
static E_pointLight*  s_pl_blink_1       = NULL;
static E_pointLight*  s_pl_blink_2       = NULL;
static E_pointLight*  s_pl_blink_3       = NULL;
static E_pointLight*  s_pl_blink_4       = NULL;
static E_pointLight*  s_pl_blink_5       = NULL;
static E_pointLight*  s_pl_blink_6       = NULL;
static E_pointLight*  s_pl_blink_7       = NULL;
static E_pointLight*  s_pl_blink_8       = NULL;
static E_pointLight*  s_pl_blink_9       = NULL;
static E_pointLight*  s_pl_blink_10      = NULL;
static E_pointLight*  s_pl_blink_11      = NULL;
static E_pointLight*  s_pl_blink_12      = NULL;


static E_spline*      s_spline_T1_enterGate        = NULL;
static E_spline*      s_spline_T2_enterGate        = NULL;
static E_spline*      s_splineToStair_T1           = NULL;
static E_spline*      s_splineToStair_T2           = NULL;
static E_spline*      s_splineOnStair_GF_T1        = NULL;
static E_spline*      s_splineOnStair_GF_T2              = NULL;
static E_spline*      s_splinesToBathroomTopToOT_move    = NULL;
static E_spline*      s_splinesToBathroomTopToOT_lookat  = NULL;
static E_spline*      s_splinesShh_move                  = NULL;
static E_spline*      s_splinesShh_lookAt                = NULL;
static E_spline*      s_splinesT1_fastRun                = NULL;
static E_spline*      s_splinesT1_backtoOT              = NULL;
static E_spline*      s_spline_heartBeatShadow          = NULL;
static E_spline*      s_spline_heartBeatShadowLookAt    = NULL;
static E_spline*      s_splinesPoliceVanOnRoad          = NULL;
static E_spline*      s_splineT1_runAwayAfterSiren      = NULL;
static E_spline*      s_spline_sadMusic                 = NULL;
static E_spline*      s_spline_sadMusicToWindow         = NULL;
static E_spline*      s_spline_policeRevealLookat       = NULL;
static E_spline*      s_spline_showPoliceSpotLights     = NULL;
static E_spline*      s_spline_deliverCam_1             = NULL;
static E_spline*      s_spline_policeCam_1              = NULL;
static E_spline*      s_spline_deliverCam_2             = NULL;
static E_spline*      s_spline_lobbyShootingShot        = NULL;
static E_spline*      s_spline_deliveryToLobbyLookAt    = NULL;
static E_spline*      s_spline_policeDeath              = NULL;
static E_spline*      s_spline_finalDelivery            = NULL;
static E_spline*      s_spline_toMorningLookAt          = NULL;

void CamaHospital_init()
{
    s_hospitalScene      = E_loadModel("DEMO_ASSETS/05-CamaHospital/cama_v6.glb",              &engineScene, "s_hospitalScene");
    s_ceiling             = E_loadModel("DEMO_ASSETS/05-CamaHospital/cama_ceiling.glb",         &engineScene, "s_ceiling");
    {
        s_skySphere = E_loadModel("DEMO_ASSETS/SkySphere/SkyDome.glb", &engineScene, "s_skySphere");
        E_material skyMat = E_createMaterialFromFiles("DEMO_ASSETS/SkySphere/skysphere.vert", "DEMO_ASSETS/SkySphere/skysphere.frag");
        skyMat.texture  = E_loadTexture("DEMO_ASSETS/SkySphere/textures/qwantani_dawn_puresky.jpg");
        skyMat.texture1 = E_loadTexture("DEMO_ASSETS/SkySphere/textures/seamlessCloud.png");
        skyMat.texture2 = E_loadTexture("DEMO_ASSETS/SkySphere/textures/perlinNoise2.jpg");
        glProgramUniform1i(skyMat.shaderProgram, glGetUniformLocation(skyMat.shaderProgram, "uTexture_1"), 1);
        glProgramUniform1i(skyMat.shaderProgram, glGetUniformLocation(skyMat.shaderProgram, "uTexture_2"), 2);
        E_sceneObject_setMaterialAll     (s_skySphere, skyMat);
        E_sceneObject_setCastShadowAll   (s_skySphere, 0);
        E_sceneObject_setReceiveShadowAll(s_skySphere, 0);
        E_sceneObject_setPosition(s_skySphere, {0.00f,   0.00f,   0.00f});
        E_sceneObject_setRotation(s_skySphere, {0.00f, 285.00f,   0.00f});
        E_sceneObject_setScale   (s_skySphere, 100.00f);
    }
    s_channelGate_open       = E_loadModel("DEMO_ASSETS/05-CamaHospital/ChannelGate_open.glb",     &engineScene, "ChannelGate_open");
    E_sceneObject_setActive(s_channelGate_open,   1);
    s_channelGate_closed     = E_loadModel("DEMO_ASSETS/05-CamaHospital/ChannelGate_closed.glb",   &engineScene, "ChannelGate_closed");
    E_sceneObject_setActive(s_channelGate_closed, 0);
    s_channelGate_broken     = E_loadModel("DEMO_ASSETS/05-CamaHospital/ChannelGate_broken.glb",   &engineScene, "ChannelGate_broken");
    E_sceneObject_setActive(s_channelGate_broken, 0);
    s_policeJeep             = E_loadModel("DEMO_ASSETS/05-CamaHospital/JeepFinal_v1.glb",           &engineScene, "PoliceJeep");
    E_sceneObject_setPosition  (s_policeJeep, { 8.17f, -0.04f, 12.61f});
    E_sceneObject_setRotation  (s_policeJeep, { 0.00f, 132.00f,  0.00f});
    E_sceneObject_setScale     (s_policeJeep, 3.00f);
    E_sceneObject_setActive    (s_policeJeep, 0);
    E_sceneObject_setCastShadow(s_policeJeep, 1);

    s_policeJeep2            = E_loadModel("DEMO_ASSETS/05-CamaHospital/JeepFinal_v1.glb",           &engineScene, "PoliceJeep2");
    E_sceneObject_setPosition  (s_policeJeep2, {13.64f,  0.07f, 12.80f});
    E_sceneObject_setRotation  (s_policeJeep2, { 0.00f, 272.00f,  0.00f});
    E_sceneObject_setScale     (s_policeJeep2, 3.00f);
    E_sceneObject_setActive    (s_policeJeep2, 0);
    E_sceneObject_setCastShadow(s_policeJeep2, 1);

    s_policeSupportFir               = E_loadModel("DEMO_ASSETS/Common/Police/PoliceSupportFir.glb",                      &engineScene, "PoliceSupportFir");
    s_policeShooting                 = E_loadModel("DEMO_ASSETS/Common/Police/policeShooting.glb",                        &engineScene, "PoliceShooting");

    s_policeStandingIdleWithPistol   = E_loadModel("DEMO_ASSETS/Common/Police/Police_Standing_Idle_With_Pistol.glb",      &engineScene, "PoliceStandingIdleWithPistol");
    s_policeKaka1IdleCrouchingAiming = E_loadModel("DEMO_ASSETS/Common/Police/PoliceKaka1_Idle Crouching Aiming.glb",    &engineScene, "PoliceKaka1IdleCrouchingAiming");
    s_policeKaka1PistolIdle          = E_loadModel("DEMO_ASSETS/Common/Police/PoliceKaka1_Pistol Idle.glb",               &engineScene, "PoliceKaka1PistolIdle");
    s_policeOneRifleAimingIdle       = E_loadModel("DEMO_ASSETS/Common/Police/policeOne_rifle_aiming_idle.glb",           &engineScene, "PoliceOneRifleAimingIdle");
    s_policeOnePistolKneelingIdle    = E_loadModel("DEMO_ASSETS/Common/Police/policeOne_pistol_kneeling_idle.glb",        &engineScene, "PoliceOnePistolKneelingIdle");
    s_policeOnePistolStandingIdle    = E_loadModel("DEMO_ASSETS/Common/Police/policeOne_pistol_standing_idle.glb",        &engineScene, "PoliceOnePistolStandingIdle");

    E_sceneObject_setPosition  (s_policeSupportFir, {12.81f, -0.01f, 14.27f});
    E_sceneObject_setRotation  (s_policeSupportFir, {-21.00f, 209.00f, 0.00f});
    E_sceneObject_setScale     (s_policeSupportFir, 3.00f);
    E_sceneObject_setActive    (s_policeSupportFir, 0);
    E_sceneObject_setCastShadow(s_policeSupportFir, 1);
    if (s_policeSupportFir->animator) E_animator_setClipByName(s_policeSupportFir->animator, "Animation");

    s_policeSupportFirShotgun = E_loadModel("DEMO_ASSETS/Common/Police/shotgun.glb", &engineScene, "PoliceSupportFir_Shotgun");
    E_sceneObject_setCastShadowAll(s_policeSupportFirShotgun, 1);
    E_sceneObject_setScale   (s_policeSupportFirShotgun, 2.40f);
    CamaHospital_syncAK47ToTerrorist(s_policeSupportFirShotgun, s_policeSupportFir, s_policeSupportFirShotgunPosOffset, s_policeSupportFirShotgunRotOffset);

    E_sceneObject_setPosition  (s_policeShooting, { 9.42f, -0.02f, 13.40f});
    E_sceneObject_setRotation  (s_policeShooting, { 0.00f, 181.00f,  0.00f});
    E_sceneObject_setScale     (s_policeShooting, 3.00f);
    E_sceneObject_setActive    (s_policeShooting, 1);
    E_sceneObject_setCastShadow(s_policeShooting, 1);
    if (s_policeShooting->animator) E_animator_setClipByName(s_policeShooting->animator, "Animation");

    s_policeShootingPistol = E_loadModel("DEMO_ASSETS/Common/Police/pistol.glb", &engineScene, "PoliceShooting_Pistol");
    E_sceneObject_setCastShadowAll(s_policeShootingPistol, 1);
    E_sceneObject_setScale   (s_policeShootingPistol, 3.39f);
    CamaHospital_syncAK47ToTerrorist(s_policeShootingPistol, s_policeShooting, s_policeShootingPistolPosOffset, s_policeShootingPistolRotOffset);

    E_sceneObject_setPosition  (s_policeStandingIdleWithPistol, {15.74f,  0.02f, 12.14f});
    E_sceneObject_setRotation  (s_policeStandingIdleWithPistol, { 0.00f, 179.00f,  0.00f});
    E_sceneObject_setScale     (s_policeStandingIdleWithPistol, 3.00f);
    E_sceneObject_setActive    (s_policeStandingIdleWithPistol, 1);
    E_sceneObject_setCastShadow(s_policeStandingIdleWithPistol, 1);
    if (s_policeStandingIdleWithPistol->animator) E_animator_setClipByName(s_policeStandingIdleWithPistol->animator, "Animation");

    E_sceneObject_setActive    (s_policeKaka1IdleCrouchingAiming, 0);

    E_sceneObject_setPosition  (s_policeKaka1PistolIdle, {16.60f,  0.01f, 11.28f});
    E_sceneObject_setRotation  (s_policeKaka1PistolIdle, { 0.00f, 209.00f,  0.00f});
    E_sceneObject_setScale     (s_policeKaka1PistolIdle, 3.00f);
    E_sceneObject_setActive    (s_policeKaka1PistolIdle, 0);
    E_sceneObject_setCastShadow(s_policeKaka1PistolIdle, 1);
    if (s_policeKaka1PistolIdle->animator) E_animator_setClipByName(s_policeKaka1PistolIdle->animator, "Animation");

    E_sceneObject_setPosition  (s_policeOneRifleAimingIdle, { 5.44f,  0.03f, 13.57f});
    E_sceneObject_setRotation  (s_policeOneRifleAimingIdle, { 0.00f, 172.00f,  0.00f});
    E_sceneObject_setScale     (s_policeOneRifleAimingIdle, 3.00f);
    E_sceneObject_setActive    (s_policeOneRifleAimingIdle, 0);
    E_sceneObject_setCastShadow(s_policeOneRifleAimingIdle, 1);
    if (s_policeOneRifleAimingIdle->animator) E_animator_setClipByName(s_policeOneRifleAimingIdle->animator, "Armature|mixamo.com|Layer0");

    E_sceneObject_setPosition  (s_policeOnePistolKneelingIdle, { 7.53f,  0.16f, 11.19f});
    E_sceneObject_setRotation  (s_policeOnePistolKneelingIdle, { 0.00f, 149.00f,  0.00f});
    E_sceneObject_setScale     (s_policeOnePistolKneelingIdle, 3.00f);
    E_sceneObject_setActive    (s_policeOnePistolKneelingIdle, 0);
    E_sceneObject_setCastShadow(s_policeOnePistolKneelingIdle, 1);
    if (s_policeOnePistolKneelingIdle->animator) E_animator_setClipByName(s_policeOnePistolKneelingIdle->animator, "Armature|mixamo.com|Layer0");

    E_sceneObject_setPosition  (s_policeOnePistolStandingIdle, {13.52f, -0.03f, 11.52f});
    E_sceneObject_setRotation  (s_policeOnePistolStandingIdle, { 0.00f, 179.00f,  0.00f});
    E_sceneObject_setScale     (s_policeOnePistolStandingIdle, 3.00f);
    E_sceneObject_setActive    (s_policeOnePistolStandingIdle, 0);
    E_sceneObject_setCastShadow(s_policeOnePistolStandingIdle, 1);
    if (s_policeOnePistolStandingIdle->animator) E_animator_setClipByName(s_policeOnePistolStandingIdle->animator, "Armature|mixamo.com|Layer0");

    s_pregnentLady           = E_loadModel("DEMO_ASSETS/05-CamaHospital/PregnantLady_animationFixed.glb",    &engineScene, "PregnentLady");
    E_sceneObject_setPosition  (s_pregnentLady, {15.32f, 13.15f, -8.60f});
    E_sceneObject_setRotation  (s_pregnentLady, { 2.00f, 180.00f,  0.00f});
    E_sceneObject_setScale     (s_pregnentLady, 2.92f);
    E_sceneObject_setActive    (s_pregnentLady, 1);
    E_sceneObject_setCastShadow(s_pregnentLady, 1);
    if (s_pregnentLady->animator)
    {
        E_animator_setClipByName(s_pregnentLady->animator, "Animation");
        E_animator_setSpeed     (s_pregnentLady->animator, 0.65f);
    }
    s_nurseStandingNearPhone = E_loadModel("DEMO_ASSETS/05-CamaHospital/Nurseji_StandingIdle.glb", &engineScene, "NurseStandingNearPhone");
    s_nurseRunInside             = E_loadModel("DEMO_ASSETS/05-CamaHospital/Nurseji_Run.glb",                         &engineScene, "NurseRunInside");
    s_secGuard1FallingBackDeath  = E_loadModel("DEMO_ASSETS/05-CamaHospital/SecurityGuard1_FallingBackDeathglb.glb",  &engineScene, "SecurityGuard1FallingBackDeath");
    s_secGuard1FlyingBackDeath   = E_loadModel("DEMO_ASSETS/05-CamaHospital/SecurityGuard1_FlyingBackDeath.glb",    &engineScene, "SecurityGuard1FlyingBackDeath");
    s_secGuard1IdleStanding      = E_loadModel("DEMO_ASSETS/05-CamaHospital/SecurityGuard1IdleStanding.glb",          &engineScene, "SecurityGuard1IdleStanding");
    s_secGuard1IdleStanding2     = E_loadModel("DEMO_ASSETS/05-CamaHospital/SecurityGuard1IdleStanding.glb",          &engineScene, "SecurityGuard1IdleStanding2");
    s_adityaBreathingIdle            = E_loadModel("DEMO_ASSETS/05-CamaHospital/others/Aditya_Breathing_Idle.glb",              &engineScene, "AdityaBreathingIdle");
    s_doctorDoingDelivery            = E_loadModel("DEMO_ASSETS/05-CamaHospital/AnimatedDoctorDoingDelivery.glb",                &engineScene, "DoctorDoingDelivery");
    s_kanganaIdleButAlert            = E_loadModel("DEMO_ASSETS/05-CamaHospital/others/KanganaIdleButAlert.glb",                &engineScene, "KanganaIdleButAlert");
    s_nurseBendingIdle               = E_loadModel("DEMO_ASSETS/05-CamaHospital/NursebendingIdle.glb",                         &engineScene, "NurseBendingIdle");
    s_nurseAtReception               = E_loadModel("DEMO_ASSETS/05-CamaHospital/NurseAtReception.glb",                         &engineScene, "NurseAtReception");
    E_sceneObject_setPosition  (s_nurseAtReception, {14.48f,  1.80f, -8.65f});
    E_sceneObject_setRotation  (s_nurseAtReception, { 0.00f, 17.00f,  0.00f});
    E_sceneObject_setScale     (s_nurseAtReception, 0.05f);
    E_sceneObject_setActive    (s_nurseAtReception, 1);
    E_sceneObject_setCastShadow(s_nurseAtReception, 1);
    if (s_nurseAtReception->animator) E_animator_setClipByName(s_nurseAtReception->animator, "Animation");

    E_sceneObject_setPosition  (s_adityaBreathingIdle, {16.72f, 12.39f, -16.77f});
    E_sceneObject_setRotation  (s_adityaBreathingIdle, {0.00f, -178.00f, 0.00f});
    E_sceneObject_setScale     (s_adityaBreathingIdle, 3.00f);
    E_sceneObject_setActive    (s_adityaBreathingIdle, 1);
    E_sceneObject_setCastShadow(s_adityaBreathingIdle, 1);
    if (s_adityaBreathingIdle->animator) E_animator_setClipByName(s_adityaBreathingIdle->animator, "Animation");

    E_sceneObject_setPosition  (s_doctorDoingDelivery, {15.59f, 12.27f, -8.97f});
    E_sceneObject_setRotation  (s_doctorDoingDelivery, {0.00f, 255.00f,  0.00f});
    E_sceneObject_setScale     (s_doctorDoingDelivery, 2.70f);
    E_sceneObject_setActive    (s_doctorDoingDelivery, 1);
    E_sceneObject_setCastShadow(s_doctorDoingDelivery, 0);
    if (s_doctorDoingDelivery->animator) { E_animator_setClipByName(s_doctorDoingDelivery->animator, "Animation"); E_animator_setSpeed(s_doctorDoingDelivery->animator, 0.7f); }

    E_sceneObject_setPosition  (s_kanganaIdleButAlert, {10.46f, 12.45f, -9.80f});
    E_sceneObject_setRotation  (s_kanganaIdleButAlert, {0.00f, 180.00f, 0.00f});
    E_sceneObject_setScale     (s_kanganaIdleButAlert, 3.50f);
    E_sceneObject_setActive    (s_kanganaIdleButAlert, 1);
    E_sceneObject_setCastShadow(s_kanganaIdleButAlert, 1);
    if (s_kanganaIdleButAlert->animator) E_animator_setClipByName(s_kanganaIdleButAlert->animator, "Animation");

    E_sceneObject_setPosition  (s_nurseBendingIdle, {14.20f, 12.47f, -9.35f});
    E_sceneObject_setRotation  (s_nurseBendingIdle, {0.00f,  88.00f,  0.00f});
    E_sceneObject_setScale     (s_nurseBendingIdle, 2.60f);
    E_sceneObject_setActive    (s_nurseBendingIdle, 1);
    E_sceneObject_setCastShadow(s_nurseBendingIdle, 0);
    if (s_nurseBendingIdle->animator) E_animator_setClipByName(s_nurseBendingIdle->animator, "Armature|mixamo.com|Layer0");

    {
        E_spotLight spotLight_Delivery = E_spotLight_create(
            {16.78f, 13.44f, -8.05f},
            {-0.99f,  0.16f,  0.04f},
            { 1.00f,  1.00f,  1.00f},
            0.56f,
            15.00f, 20.00f,
            4.00f);
        spotLight_Delivery.flicker       = 1;
        spotLight_Delivery.flickerSpeed  = 3.000f;
        spotLight_Delivery.flickerAmount = 0.500f;
        spotLight_Delivery.castShadow    = 1;
        s_sl_delivery = E_spotLight_addToScene(&engineScene, spotLight_Delivery);
        E_spotLight_setName(s_sl_delivery, "spotLight_Delivery");
        s_sl_delivery->intensity = 0.0f;
        E_spotLight_deactivate(s_sl_delivery);
    }

    {
        E_spotLight spotlightPolice_top = E_spotLight_create(
            { 6.25f,  5.51f,  9.82f},
            { 0.62f, -0.71f,  0.35f},
            { 1.00f,  0.73f,  0.47f},
            0.81f,
            0.00f, 43.20f,
            13.65f);
        spotlightPolice_top.flicker       = 1;
        spotlightPolice_top.flickerSpeed  = 3.000f;
        spotlightPolice_top.flickerAmount = 0.234f;
        spotlightPolice_top.castShadow    = 1;
        s_sl_police_top = E_spotLight_addToScene(&engineScene, spotlightPolice_top);
        E_spotLight_setName(s_sl_police_top, "spotlightPolice_top");
        s_sl_police_top->intensity = 0.0f;
        E_spotLight_deactivate(s_sl_police_top);
    }

    {
        E_spotLight spotLight_police_1 = E_spotLight_create(
            { 7.29f,  1.86f,  10.02f},
            { 0.26f,  0.60f,  -0.99f},
            { 0.91f,  1.00f,   0.57f},
            7.91f,
            1.50f, 4.60f,
            26.51f);
        spotLight_police_1.flicker       = 1;
        spotLight_police_1.flickerSpeed  = 0.100f;
        spotLight_police_1.flickerAmount = 0.500f;
        spotLight_police_1.castShadow    = 0;
        s_sl_police_1 = E_spotLight_addToScene(&engineScene, spotLight_police_1);
        E_spotLight_setName(s_sl_police_1, "spotLight_police_1");
        s_sl_police_1->intensity = 0.0f;
        E_spotLight_deactivate(s_sl_police_1);
    }

    {
        E_spotLight spotLight_police_1_copy = E_spotLight_create(
            {16.43f,  3.04f,  12.58f},
            {-0.54f,  0.17f,  -0.92f},
            { 0.95f,  1.00f,   0.75f},
            8.72f,
            1.50f, 7.80f,
            26.51f);
        spotLight_police_1_copy.flicker       = 1;
        spotLight_police_1_copy.flickerSpeed  = 1.059f;
        spotLight_police_1_copy.flickerAmount = 0.500f;
        spotLight_police_1_copy.castShadow    = 0;
        s_sl_police_1_copy = E_spotLight_addToScene(&engineScene, spotLight_police_1_copy);
        E_spotLight_setName(s_sl_police_1_copy, "spotLight_police_1_copy");
        s_sl_police_1_copy->intensity = 0.0f;
        E_spotLight_deactivate(s_sl_police_1_copy);
    }

    {
        E_pointLight pl;
        E_pointLight* plPtr;

        pl    = E_pointLight_create({10.84f, 13.24f, -8.96f}, {1.00f, 1.00f, 1.00f}, 0.55f, 1.37f);
        plPtr = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(plPtr, "pointLight_NearKangnaIdleStanding");
        plPtr->flicker       = 1;
        plPtr->flickerSpeed  = 3.000f;
        plPtr->flickerAmount = 0.500f;
        s_pl_upper_1 = plPtr;

        pl    = E_pointLight_create({7.50f, 13.27f, -9.03f}, {1.00f, 1.00f, 1.00f}, 0.07f, 0.90f);
        plPtr = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(plPtr, "pointLight_nearPL_1");
        plPtr->flicker       = 1;
        plPtr->flickerSpeed  = 0.689f;
        plPtr->flickerAmount = 0.500f;
        s_pl_upper_2 = plPtr;

        pl    = E_pointLight_create({15.78f, 14.48f, -9.02f}, {1.00f, 1.00f, 1.00f}, 0.07f, 0.90f);
        plPtr = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(plPtr, "pointLight_nearPL_1_copy_copy_copy");
        plPtr->flicker       = 1;
        plPtr->flickerSpeed  = 0.689f;
        plPtr->flickerAmount = 0.500f;
        s_pl_upper_3 = plPtr;

        pl    = E_pointLight_create({9.01f, 13.69f, -9.16f}, {1.00f, 1.00f, 1.00f}, 0.07f, 0.90f);
        plPtr = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(plPtr, "pointLight_nearPL_1_copy_copy");
        plPtr->flicker       = 1;
        plPtr->flickerSpeed  = 0.689f;
        plPtr->flickerAmount = 0.500f;
        s_pl_upper_4 = plPtr;

        pl    = E_pointLight_create({7.53f, 14.00f, -7.94f}, {1.00f, 1.00f, 1.00f}, 0.07f, 0.90f);
        plPtr = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(plPtr, "pointLight_nearPL_1_copy");
        plPtr->flicker       = 1;
        plPtr->flickerSpeed  = 0.689f;
        plPtr->flickerAmount = 0.500f;
        s_pl_upper_5 = plPtr;
    }
    {
        E_pointLight pointLight_medicine = E_pointLight_create(
            {17.73f, 13.02f, -8.19f},
            { 0.74f,  1.00f,  0.51f},
            0.16f, 0.79f);
        s_pl_medicine = E_pointLight_addToScene(&engineScene, pointLight_medicine);
        E_pointLight_setName(s_pl_medicine, "pointLight_medicine");
        s_pl_medicine->flicker       = 0;
        s_pl_medicine->flickerSpeed  = 3.000f;
        s_pl_medicine->flickerAmount = 0.500f;
    }
    {
        E_pointLight pointLight_pregnantLadyCloseup = E_pointLight_create(
            {15.56f, 13.77f, -8.03f},
            { 1.00f,  1.00f,  1.00f},
            0.23f, 0.91f);
        s_pl_pregnantLadyCloseup = E_pointLight_addToScene(&engineScene, pointLight_pregnantLadyCloseup);
        E_pointLight_setName(s_pl_pregnantLadyCloseup, "pointLight_pregnantLadyCloseup");
        s_pl_pregnantLadyCloseup->flicker       = 1;
        s_pl_pregnantLadyCloseup->flickerSpeed  = 3.000f;
        s_pl_pregnantLadyCloseup->flickerAmount = 0.500f;
    }
    {
        E_pointLight pointLight_T1_Gun = E_pointLight_create(
            {16.29f, 10.27f, -2.23f},
            { 0.90f,  1.00f,  0.57f},
            5.00f, 1.43f);
        s_pl_T1_gun = E_pointLight_addToScene(&engineScene, pointLight_T1_Gun);
        E_pointLight_setName(s_pl_T1_gun, "pointLight_T1_Gun");
        s_pl_T1_gun->flicker       = 1;
        s_pl_T1_gun->flickerSpeed  = 5.582f;
        s_pl_T1_gun->flickerAmount = 0.868f;
        E_pointLight_deactivate(s_pl_T1_gun);
    }

    s_terrorist1WalkingWithGun   = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist1_Walking_With_Gun.glb",        &engineScene, "Terrorist1WalkingWithGun");
    E_sceneObject_setPosition  (s_terrorist1WalkingWithGun, { 5.26f, 0.07f, 10.59f});
    E_sceneObject_setRotation  (s_terrorist1WalkingWithGun, { 0.00f, 157.00f, 0.00f});
    E_sceneObject_setScale     (s_terrorist1WalkingWithGun, 3.0f);
    E_sceneObject_setActive    (s_terrorist1WalkingWithGun, 1);
    E_sceneObject_setCastShadow(s_terrorist1WalkingWithGun, 1);
    if (s_terrorist1WalkingWithGun->animator) E_animator_setClipByName(s_terrorist1WalkingWithGun->animator, "Animation");

    s_terrorist1WalkingWithGunAK47 = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist1WalkingWithGun_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist1WalkingWithGunAK47, 1);
    E_sceneObject_setScale   (s_terrorist1WalkingWithGunAK47, 2.40f);
    CamaHospital_syncAK47ToTerrorist(s_terrorist1WalkingWithGunAK47, s_terrorist1WalkingWithGun, s_terrorist1WalkingWithGunAK47PosOffset, s_terrorist1WalkingWithGunAK47RotOffset);

    s_terrorist2WalkingWithRifle = E_loadModel("DEMO_ASSETS/Common/Terrorists/Terrorist2_Walking_With_Rifle.glb",      &engineScene, "Terrorist2WalkingWithRifle");
    E_sceneObject_setPosition  (s_terrorist2WalkingWithRifle, { 4.37f, -0.04f, 10.56f});
    E_sceneObject_setRotation  (s_terrorist2WalkingWithRifle, { 0.00f, 166.00f, 0.00f});
    E_sceneObject_setScale     (s_terrorist2WalkingWithRifle, 3.0f);
    E_sceneObject_setActive    (s_terrorist2WalkingWithRifle, 1);
    E_sceneObject_setCastShadow(s_terrorist2WalkingWithRifle, 1);
    if (s_terrorist2WalkingWithRifle->animator) E_animator_setClipByName(s_terrorist2WalkingWithRifle->animator, "Animation");

    s_terrorist2WalkingWithRifleAK47 = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist2WalkingWithRifle_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist2WalkingWithRifleAK47, 1);
    E_sceneObject_setScale   (s_terrorist2WalkingWithRifleAK47, 2.40f);
    CamaHospital_syncAK47ToTerrorist(s_terrorist2WalkingWithRifleAK47, s_terrorist2WalkingWithRifle, s_terrorist2WalkingWithRifleAK47PosOffset, s_terrorist2WalkingWithRifleAK47RotOffset);

    s_terrorist1Gunplay           = E_loadModel("DEMO_ASSETS/05-CamaHospital/Terrorist1_Gunplay.glb",           &engineScene, "Terrorist1Gunplay");
    E_sceneObject_setPosition  (s_terrorist1Gunplay, {16.69f,  8.43f, -4.13f});
    E_sceneObject_setRotation  (s_terrorist1Gunplay, {14.00f, -21.00f, -5.00f});
    E_sceneObject_setScale     (s_terrorist1Gunplay, 3.00f);
    E_sceneObject_setActive    (s_terrorist1Gunplay, 1);
    E_sceneObject_setCastShadow(s_terrorist1Gunplay, 1);
    if (s_terrorist1Gunplay->animator)
    {
        E_animator_setClipByName(s_terrorist1Gunplay->animator, "Animation");
        s_terrorist1Gunplay->animator->speed = 0.5f;
    }

    s_terrorist1GunplayAK47 = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist1Gunplay_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist1GunplayAK47, 1);
    E_sceneObject_setScale   (s_terrorist1GunplayAK47, 2.40f);
    CamaHospital_syncAK47ToTerrorist(s_terrorist1GunplayAK47, s_terrorist1Gunplay, s_terrorist1GunplayAK47PosOffset, s_terrorist1GunplayAK47RotOffset);

    s_terrorist2HidingAndShooting = E_loadModel("DEMO_ASSETS/05-CamaHospital/Terrorist2_HidingAndShooting.glb", &engineScene, "Terrorist2HidingAndShooting");
    E_sceneObject_setPosition  (s_terrorist2HidingAndShooting, {13.16f,   9.27f, -3.84f});
    E_sceneObject_setRotation  (s_terrorist2HidingAndShooting, {-17.00f, 219.00f,  0.00f});
    E_sceneObject_setScale     (s_terrorist2HidingAndShooting, 3.00f);
    E_sceneObject_setActive    (s_terrorist2HidingAndShooting, 1);
    E_sceneObject_setCastShadow(s_terrorist2HidingAndShooting, 1);
    if (s_terrorist2HidingAndShooting->animator) E_animator_setClipByName(s_terrorist2HidingAndShooting->animator, "Animation");

    s_terrorist2HidingAndShootingAK47 = E_loadModel("DEMO_ASSETS/Common/Terrorists/AK47scaled.glb", &engineScene, "Terrorist2HidingAndShooting_AK47scaled");
    E_sceneObject_setCastShadowAll(s_terrorist2HidingAndShootingAK47, 1);
    E_sceneObject_setScale   (s_terrorist2HidingAndShootingAK47, 2.40f);
    CamaHospital_syncAK47ToTerrorist(s_terrorist2HidingAndShootingAK47, s_terrorist2HidingAndShooting, s_terrorist2HidingAndShootingAK47PosOffset, s_terrorist2HidingAndShootingAK47RotOffset);

    {
        s_spline_T1_enterGate = E_scene_addSpline(&engineScene, "spline_T1_enterGate");
        s_spline_T1_enterGate->numPoints    = 4;
        s_spline_T1_enterGate->points[0] = vmath::vec3(10.299f, 0.958f, -1.719f);
        s_spline_T1_enterGate->points[1] = vmath::vec3(11.599f, 0.958f, -1.719f);
        s_spline_T1_enterGate->points[2] = vmath::vec3(10.955f, 0.980f, -5.893f);
        s_spline_T1_enterGate->points[3] = vmath::vec3(11.455f, 0.980f, -7.593f);
    }
    {
        s_spline_T2_enterGate = E_scene_addSpline(&engineScene, "spline_T2_enterGate");
        s_spline_T2_enterGate->numPoints    = 4;
        s_spline_T2_enterGate->points[0] = vmath::vec3(13.131f, 0.974f, -1.763f);
        s_spline_T2_enterGate->points[1] = vmath::vec3(14.031f, 0.974f, -1.763f);
        s_spline_T2_enterGate->points[2] = vmath::vec3(12.787f, 0.968f, -5.290f);
        s_spline_T2_enterGate->points[3] = vmath::vec3(12.787f, 0.968f, -5.790f);
    }
    {
        s_splineToStair_T1 = E_scene_addSpline(&engineScene, "splineToStair_T1");
        s_splineToStair_T1->numPoints    = 6;
        s_splineToStair_T1->points[0] = vmath::vec3(11.920f, 1.010f, -17.891f);
        s_splineToStair_T1->points[1] = vmath::vec3(11.920f, 1.010f, -17.891f);
        s_splineToStair_T1->points[2] = vmath::vec3(12.513f, 0.977f, -18.308f);
        s_splineToStair_T1->points[3] = vmath::vec3(13.803f, 0.988f, -18.801f);
        s_splineToStair_T1->points[4] = vmath::vec3(14.690f, 0.965f, -18.931f);
        s_splineToStair_T1->points[5] = vmath::vec3(16.064f, 0.981f, -19.109f);
    }
    {
        s_splineToStair_T2 = E_scene_addSpline(&engineScene, "splineToStair_T2");
        s_splineToStair_T2->numPoints    = 6;
        s_splineToStair_T2->points[0] = vmath::vec3(13.151f, 1.077f, -16.904f);
        s_splineToStair_T2->points[1] = vmath::vec3(13.151f, 1.077f, -16.904f);
        s_splineToStair_T2->points[2] = vmath::vec3(14.146f, 1.077f, -17.707f);
        s_splineToStair_T2->points[3] = vmath::vec3(15.651f, 1.077f, -18.363f);
        s_splineToStair_T2->points[4] = vmath::vec3(16.667f, 1.077f, -18.647f);
        s_splineToStair_T2->points[5] = vmath::vec3(17.320f, 1.077f, -18.777f);
    }
    {
        s_splineOnStair_GF_T1 = E_scene_addSpline(&engineScene, "splineOnStair_GF_T1");
        s_splineOnStair_GF_T1->numPoints    = 4;
        s_splineOnStair_GF_T1->points[0] = vmath::vec3(19.912f, 0.975f, -20.340f);
        s_splineOnStair_GF_T1->points[1] = vmath::vec3(19.912f, 0.975f, -20.340f);
        s_splineOnStair_GF_T1->points[2] = vmath::vec3(22.325f, 2.673f, -20.381f);
        s_splineOnStair_GF_T1->points[3] = vmath::vec3(22.325f, 2.673f, -20.381f);
    }
    {
        s_splineOnStair_GF_T2 = E_scene_addSpline(&engineScene, "splineOnStair_GF_T2");
        s_splineOnStair_GF_T2->numPoints    = 4;
        s_splineOnStair_GF_T2->points[0] = vmath::vec3(20.027f, 1.046f, -19.406f);
        s_splineOnStair_GF_T2->points[1] = vmath::vec3(20.027f, 1.046f, -19.406f);
        s_splineOnStair_GF_T2->points[2] = vmath::vec3(22.126f, 2.597f, -19.321f);
        s_splineOnStair_GF_T2->points[3] = vmath::vec3(22.126f, 2.597f, -19.321f);
    }

    {
        s_splinesToBathroomTopToOT_move = E_scene_addSpline(&engineScene, "splinesToBathroomTopToOT_move");
        s_splinesToBathroomTopToOT_move->numPoints    = 7;
        s_splinesToBathroomTopToOT_move->points[0] = vmath::vec3(21.505f, 15.595f, -20.023f);
        s_splinesToBathroomTopToOT_move->points[1] = vmath::vec3(21.505f, 15.595f, -20.023f);
        s_splinesToBathroomTopToOT_move->points[2] = vmath::vec3(17.980f, 15.988f, -19.434f);
        s_splinesToBathroomTopToOT_move->points[3] = vmath::vec3(15.851f, 16.283f, -17.660f);
        s_splinesToBathroomTopToOT_move->points[4] = vmath::vec3(14.978f, 16.283f, -14.714f);
        s_splinesToBathroomTopToOT_move->points[5] = vmath::vec3(10.795f, 16.507f, -8.592f);
        s_splinesToBathroomTopToOT_move->points[6] = vmath::vec3(10.795f, 16.207f, -8.592f);
    }

    {
        s_splinesToBathroomTopToOT_lookat = E_scene_addSpline(&engineScene, "splinesToBathroomTopToOT_lookat");
        s_splinesToBathroomTopToOT_lookat->numPoints    = 8;
        s_splinesToBathroomTopToOT_lookat->points[0] = vmath::vec3(22.650f, 10.618f, -19.100f);
        s_splinesToBathroomTopToOT_lookat->points[1] = vmath::vec3(22.676f, 10.881f, -19.258f);
        s_splinesToBathroomTopToOT_lookat->points[2] = vmath::vec3(18.945f, 12.673f, -19.086f);
        s_splinesToBathroomTopToOT_lookat->points[3] = vmath::vec3(15.371f, 12.730f, -19.443f);
        s_splinesToBathroomTopToOT_lookat->points[4] = vmath::vec3(15.337f, 12.584f, -16.591f);
        s_splinesToBathroomTopToOT_lookat->points[5] = vmath::vec3(13.765f, 12.674f, -12.491f);
        s_splinesToBathroomTopToOT_lookat->points[6] = vmath::vec3(10.775f, 13.965f,  -9.380f);
        s_splinesToBathroomTopToOT_lookat->points[7] = vmath::vec3( 9.946f, 13.965f,  -9.119f);
    }

    {
        s_splinesShh_move = E_scene_addSpline(&engineScene, "splinesShh_move");
        s_splinesShh_move->numPoints    = 5;
        s_splinesShh_move->points[0] = vmath::vec3(15.278f, 16.283f, -15.914f);
        s_splinesShh_move->points[1] = vmath::vec3(11.859f, 16.305f,  -9.281f);
        s_splinesShh_move->points[2] = vmath::vec3(10.754f, 14.342f,  -8.783f);
        s_splinesShh_move->points[3] = vmath::vec3(11.450f, 14.379f, -10.014f);
        s_splinesShh_move->points[4] = vmath::vec3(11.788f, 14.444f, -10.597f);
    }

    {
        s_splinesShh_lookAt = E_scene_addSpline(&engineScene, "splinesShh_lookAt");
        s_splinesShh_lookAt->numPoints    = 7;
        s_splinesShh_lookAt->points[0] = vmath::vec3(13.765f, 12.674f, -12.491f);
        s_splinesShh_lookAt->points[1] = vmath::vec3(10.775f, 13.965f,  -9.380f);
        s_splinesShh_lookAt->points[2] = vmath::vec3(10.800f, 14.021f,  -9.710f);
        s_splinesShh_lookAt->points[3] = vmath::vec3(11.536f, 14.380f, -10.101f);
        s_splinesShh_lookAt->points[4] = vmath::vec3(13.262f, 14.465f, -11.918f);
        s_splinesShh_lookAt->points[5] = vmath::vec3(17.592f, 13.978f, -15.699f);
        s_splinesShh_lookAt->points[6] = vmath::vec3(18.164f, 13.854f, -16.842f);
    }

    {
        s_splinesT1_fastRun = E_scene_addSpline(&engineScene, "splinesT1_fastRun");
        s_splinesT1_fastRun->numPoints    = 4;
        s_splinesT1_fastRun->points[0] = vmath::vec3(18.242f, 12.746f, -11.135f);
        s_splinesT1_fastRun->points[1] = vmath::vec3(18.242f, 12.746f, -11.135f);
        s_splinesT1_fastRun->points[2] = vmath::vec3( 7.723f, 12.686f, -10.738f);
        s_splinesT1_fastRun->points[3] = vmath::vec3( 7.723f, 12.686f, -10.738f);
    }

    {
        s_splinesT1_backtoOT = E_scene_addSpline(&engineScene, "splinesT1_backtoOT");
        s_splinesT1_backtoOT->numPoints    = 4;
        s_splinesT1_backtoOT->points[0] = vmath::vec3( 9.455f, 13.032f, -10.778f);
        s_splinesT1_backtoOT->points[1] = vmath::vec3( 9.455f, 12.832f, -10.778f);
        s_splinesT1_backtoOT->points[2] = vmath::vec3(11.727f, 12.887f, -10.851f);
        s_splinesT1_backtoOT->points[3] = vmath::vec3(12.427f, 12.987f, -10.851f);
    }
    {
        s_spline_heartBeatShadow = E_scene_addSpline(&engineScene, "spline_heartBeatShadow");
        s_spline_heartBeatShadow->numPoints    = 4;
        s_spline_heartBeatShadow->points[0] = vmath::vec3( 8.016f, 14.097f,  -8.880f);
        s_spline_heartBeatShadow->points[1] = vmath::vec3( 8.016f, 14.097f,  -8.880f);
        s_spline_heartBeatShadow->points[2] = vmath::vec3(10.580f, 14.031f,  -9.522f);
        s_spline_heartBeatShadow->points[3] = vmath::vec3(10.580f, 14.031f,  -9.522f);
    }
    {
        s_spline_heartBeatShadowLookAt = E_scene_addSpline(&engineScene, "spline_heartBeatShadowLookAt");
        s_spline_heartBeatShadowLookAt->numPoints    = 4;
        s_spline_heartBeatShadowLookAt->points[0] = vmath::vec3(11.893f, 14.275f, -10.093f);
        s_spline_heartBeatShadowLookAt->points[1] = vmath::vec3(11.893f, 14.275f, -10.093f);
        s_spline_heartBeatShadowLookAt->points[2] = vmath::vec3(12.915f, 12.887f,  -4.147f);
        s_spline_heartBeatShadowLookAt->points[3] = vmath::vec3(12.915f, 12.887f,  -4.147f);
    }

    {
        s_splinesPoliceVanOnRoad = E_scene_addSpline(&engineScene, "splinesPoliceVanOnRoad");
        s_splinesPoliceVanOnRoad->numPoints    = 4;
        s_splinesPoliceVanOnRoad->points[0] = vmath::vec3(-32.110f, 0.014f, 29.317f);
        s_splinesPoliceVanOnRoad->points[1] = vmath::vec3(-32.110f, 0.014f, 29.317f);
        s_splinesPoliceVanOnRoad->points[2] = vmath::vec3( 56.311f, 0.056f, 29.352f);
        s_splinesPoliceVanOnRoad->points[3] = vmath::vec3( 56.311f, 0.056f, 29.352f);
    }

    {
        s_splineT1_runAwayAfterSiren = E_scene_addSpline(&engineScene, "splineT1_runAwayAfterSiren");
        s_splineT1_runAwayAfterSiren->numPoints    = 4;
        s_splineT1_runAwayAfterSiren->points[0] = vmath::vec3( 9.031f, 12.821f, -10.746f);
        s_splineT1_runAwayAfterSiren->points[1] = vmath::vec3( 9.031f, 12.821f, -10.746f);
        s_splineT1_runAwayAfterSiren->points[2] = vmath::vec3(15.892f, 12.912f, -10.899f);
        s_splineT1_runAwayAfterSiren->points[3] = vmath::vec3(15.892f, 12.912f, -10.899f);
    }

    E_sceneObject_setPosition  (s_secGuard1IdleStanding, {9.54f, 0.96f, -2.17f});
    E_sceneObject_setRotation  (s_secGuard1IdleStanding, {0.00f,  9.00f,  0.00f});
    E_sceneObject_setScale     (s_secGuard1IdleStanding, 0.03f);
    E_sceneObject_setActive    (s_secGuard1IdleStanding, 1);
    E_sceneObject_setCastShadow(s_secGuard1IdleStanding, 1);
    if (s_secGuard1IdleStanding->animator) E_animator_setClipByName(s_secGuard1IdleStanding->animator, "Animation");

    E_sceneObject_setPosition  (s_secGuard1IdleStanding2, {13.22f, 0.96f, -1.94f});
    E_sceneObject_setRotation  (s_secGuard1IdleStanding2, { 0.00f, -44.76f, 0.00f});
    E_sceneObject_setScale     (s_secGuard1IdleStanding2, 0.03f);
    E_sceneObject_setActive    (s_secGuard1IdleStanding2, 1);
    E_sceneObject_setCastShadow(s_secGuard1IdleStanding2, 1);
    if (s_secGuard1IdleStanding2->animator) E_animator_setClipByName(s_secGuard1IdleStanding2->animator, "Animation");

    E_sceneObject_setPosition  (s_secGuard1FallingBackDeath, {13.26f, 1.06f, -1.75f});
    E_sceneObject_setRotation  (s_secGuard1FallingBackDeath, { 0.00f, -36.00f, 0.00f});
    E_sceneObject_setScale     (s_secGuard1FallingBackDeath, 0.03f);
    E_sceneObject_setActive    (s_secGuard1FallingBackDeath, 0);
    E_sceneObject_setCastShadow(s_secGuard1FallingBackDeath, 1);
    if (s_secGuard1FallingBackDeath->animator)
    {
        E_animator_setClipByName(s_secGuard1FallingBackDeath->animator, "Animation");
        E_animator_setTime      (s_secGuard1FallingBackDeath->animator, 0.0f);
        E_animator_pause        (s_secGuard1FallingBackDeath->animator);
    }

    E_sceneObject_setPosition  (s_secGuard1FlyingBackDeath, { 9.63f, 1.00f, -2.25f});
    E_sceneObject_setRotation  (s_secGuard1FlyingBackDeath, { 0.00f, 82.00f, 0.00f});
    E_sceneObject_setScale     (s_secGuard1FlyingBackDeath, 0.03f);
    E_sceneObject_setActive    (s_secGuard1FlyingBackDeath, 0);
    E_sceneObject_setCastShadow(s_secGuard1FlyingBackDeath, 1);
    if (s_secGuard1FlyingBackDeath->animator)
    {
        E_animator_setClipByName(s_secGuard1FlyingBackDeath->animator, "Animation");
        E_animator_setTime      (s_secGuard1FlyingBackDeath->animator, 0.0f);
        E_animator_pause        (s_secGuard1FlyingBackDeath->animator);
    }
    E_sceneObject_setPosition  (s_nurseRunInside, {13.02f, 0.93f, -7.64f});
    E_sceneObject_setRotation  (s_nurseRunInside, { 0.00f, 83.00f, 0.00f});
    E_sceneObject_setScale     (s_nurseRunInside, 2.70f);
    E_sceneObject_setActive    (s_nurseRunInside, 0);
    E_sceneObject_setCastShadow(s_nurseRunInside, 1);
    if (s_nurseRunInside->animator) E_animator_setClipByName(s_nurseRunInside->animator, "Animation");

    {
        s_splineNurseRunFromPhone = E_scene_addSpline(&engineScene, "splineNurseRunFromPhone");
        s_splineNurseRunFromPhone->numPoints    = 8;
        s_splineNurseRunFromPhone->points[0] = vmath::vec3(13.114f, 0.943f,  -7.474f);
        s_splineNurseRunFromPhone->points[1] = vmath::vec3(14.114f, 1.043f,  -8.874f);
        s_splineNurseRunFromPhone->points[2] = vmath::vec3(13.090f, 0.993f, -12.283f);
        s_splineNurseRunFromPhone->points[3] = vmath::vec3(13.098f, 1.009f, -15.488f);
        s_splineNurseRunFromPhone->points[4] = vmath::vec3(14.122f, 1.022f, -17.297f);
        s_splineNurseRunFromPhone->points[5] = vmath::vec3(15.747f, 0.978f, -18.230f);
        s_splineNurseRunFromPhone->points[6] = vmath::vec3(18.133f, 0.991f, -19.086f);
        s_splineNurseRunFromPhone->points[7] = vmath::vec3(19.703f, 1.006f, -19.573f);
    }
    {
        s_spline_GateTop = E_scene_addSpline(&engineScene, "spline_GateTop");
        s_spline_GateTop->numPoints    = 5;
        s_spline_GateTop->points[0] = vmath::vec3( 7.103f, 1.543f, 25.633f);
        s_spline_GateTop->points[1] = vmath::vec3( 7.247f, 1.543f, 25.624f);
        s_spline_GateTop->points[2] = vmath::vec3(11.811f, 1.543f, 25.278f);
        s_spline_GateTop->points[3] = vmath::vec3(15.874f, 1.410f, 24.796f);
        s_spline_GateTop->points[4] = vmath::vec3(16.707f, 1.410f, 24.470f);
    }
    {
        s_spline_ToPhone = E_scene_addSpline(&engineScene, "spline_ToPhone");
        s_spline_ToPhone->numPoints    = 6;
        s_spline_ToPhone->points[0] = vmath::vec3(18.113f, 9.383f,  -2.817f);
        s_spline_ToPhone->points[1] = vmath::vec3(17.926f, 7.583f,  -2.839f);
        s_spline_ToPhone->points[2] = vmath::vec3(14.615f, 3.272f,  -1.893f);
        s_spline_ToPhone->points[3] = vmath::vec3(14.527f, 1.943f,  -5.995f);
        s_spline_ToPhone->points[4] = vmath::vec3(14.120f, 1.753f,  -7.294f);
        s_spline_ToPhone->points[5] = vmath::vec3(13.886f, 1.623f,  -7.721f);
    }
    {
        s_splineToNurseRun = E_scene_addSpline(&engineScene, "splineToNurseRun");
        s_splineToNurseRun->numPoints    = 4;
        s_splineToNurseRun->points[0] = vmath::vec3(14.292f, 1.953f, -7.354f);
        s_splineToNurseRun->points[1] = vmath::vec3(14.120f, 1.753f, -7.294f);
        s_splineToNurseRun->points[2] = vmath::vec3(13.516f, 1.065f, -7.263f);
        s_splineToNurseRun->points[3] = vmath::vec3(13.348f, 1.065f, -7.253f);
    }
    {
        s_splineToNurseLeg_lookAt = E_scene_addSpline(&engineScene, "splineToNurseLeg_lookAt");
        s_splineToNurseLeg_lookAt->numPoints    = 7;
        s_splineToNurseLeg_lookAt->points[0] = vmath::vec3(14.621f, 1.770f,  -6.875f);
        s_splineToNurseLeg_lookAt->points[1] = vmath::vec3(13.964f, 1.653f,  -7.905f);
        s_splineToNurseLeg_lookAt->points[2] = vmath::vec3(13.310f, 1.045f, -10.848f);
        s_splineToNurseLeg_lookAt->points[3] = vmath::vec3(12.909f, 1.066f, -14.742f);
        s_splineToNurseLeg_lookAt->points[4] = vmath::vec3(13.492f, 1.168f, -17.120f);
        s_splineToNurseLeg_lookAt->points[5] = vmath::vec3(16.535f, 1.248f, -18.771f);
        s_splineToNurseLeg_lookAt->points[6] = vmath::vec3(17.798f, 1.197f, -19.539f);
    }
    {
        s_splineLightsOff = E_scene_addSpline(&engineScene, "splineLightsOff");
        s_splineLightsOff->numPoints    = 5;
        s_splineLightsOff->points[0] = vmath::vec3(26.490f, 2.500f, 12.540f);
        s_splineLightsOff->points[1] = vmath::vec3(25.793f, 2.500f, 12.904f);
        s_splineLightsOff->points[2] = vmath::vec3(19.848f, 2.696f, 14.011f);
        s_splineLightsOff->points[3] = vmath::vec3( 7.783f, 2.696f, 14.491f);
        s_splineLightsOff->points[4] = vmath::vec3( 1.387f, 2.848f, 12.187f);
    }
    {
        s_splineTerroristReveal_T1_lookat = E_scene_addSpline(&engineScene, "splineTerroristReveal_T1_lookat");
        s_splineTerroristReveal_T1_lookat->numPoints    = 6;
        s_splineTerroristReveal_T1_lookat->points[0] = vmath::vec3(-5.249f, 0.000f, 12.782f);
        s_splineTerroristReveal_T1_lookat->points[1] = vmath::vec3(-4.475f, 0.000f, 12.409f);
        s_splineTerroristReveal_T1_lookat->points[2] = vmath::vec3(-0.980f, 0.000f, 10.442f);
        s_splineTerroristReveal_T1_lookat->points[3] = vmath::vec3( 0.967f, 0.000f,  9.394f);
        s_splineTerroristReveal_T1_lookat->points[4] = vmath::vec3( 2.834f, 0.000f,  9.453f);
        s_splineTerroristReveal_T1_lookat->points[5] = vmath::vec3( 5.485f, 0.000f, 10.091f);
    }
    {
        s_splineTerroristReveal_T2_lookat = E_scene_addSpline(&engineScene, "splineTerroristReveal_T2_lookat");
        s_splineTerroristReveal_T2_lookat->numPoints    = 6;
        s_splineTerroristReveal_T2_lookat->points[0] = vmath::vec3(-4.651f, 0.000f, 13.738f);
        s_splineTerroristReveal_T2_lookat->points[1] = vmath::vec3(-4.151f, 0.000f, 13.422f);
        s_splineTerroristReveal_T2_lookat->points[2] = vmath::vec3(-0.901f, 0.000f, 11.511f);
        s_splineTerroristReveal_T2_lookat->points[3] = vmath::vec3( 2.030f, 0.000f, 10.524f);
        s_splineTerroristReveal_T2_lookat->points[4] = vmath::vec3( 4.430f, 0.000f, 10.076f);
        s_splineTerroristReveal_T2_lookat->points[5] = vmath::vec3( 5.215f, 0.000f,  9.973f);
    }

    {
        E_pointLight pl = E_pointLight_create({8.31f, 13.72f, -12.60f}, {1.00f, 0.90f, 0.69f}, 0.19f, 4.49f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_OTRight = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_OTRight, "pointLight_OTRight");
    }
    {
        E_pointLight pl = E_pointLight_create({15.86f, 13.72f, -12.60f}, {1.00f, 0.90f, 0.69f}, 0.19f, 4.49f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_OTLeft = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_OTLeft, "pointLight_OTLeft");
    }
    {
        E_pointLight pl = E_pointLight_create({7.51f, 13.72f, -14.70f}, {1.00f, 0.90f, 0.69f}, 0.13f, 4.37f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_OTFront1 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_OTFront1, "pointLight_OTFront1");
    }
    {
        E_pointLight pl = E_pointLight_create({12.11f, 13.72f, -14.70f}, {1.00f, 0.90f, 0.69f}, 0.13f, 4.37f);
        pl.flicker = 1; pl.flickerSpeed = 0.481f; pl.flickerAmount = 0.500f;
        s_pl_OTFront2 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_OTFront2, "pointLight_OTFront2");
    }
    {
        E_pointLight pl = E_pointLight_create({16.81f, 13.72f, -14.70f}, {1.00f, 0.90f, 0.69f}, 0.13f, 4.37f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_OTFront3 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_OTFront3, "pointLight_OTFront3");
    }
    {
        E_pointLight pl = E_pointLight_create({4.76f, 13.78f, -11.05f}, {1.00f, 0.90f, 0.69f}, 0.13f, 4.37f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_OTLeftCorridor1 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_OTLeftCorridor1, "pointLight_OTLeftCorridor1");
    }
    {
        E_pointLight pl = E_pointLight_create({4.76f, 13.78f, -15.75f}, {1.00f, 0.90f, 0.69f}, 0.19f, 4.37f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_OTLeftCorridor2 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_OTLeftCorridor2, "pointLight_OTLeftCorridor2");
    }
    {
        E_pointLight pl = E_pointLight_create({19.95f, 13.78f, -11.60f}, {1.00f, 0.90f, 0.69f}, 0.13f, 4.37f);
        pl.flicker = 1; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_OTRightCorridor2 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_OTRightCorridor2, "pointLight_OTRIghtCorridor2");
    }
    {
        E_pointLight pl = E_pointLight_create({12.00f, 2.06f, -0.79f}, {0.35f, 0.17f, 0.17f}, 1.77f, 6.03f);
        pl.flicker = 0; pl.flickerSpeed = 0.796f; pl.flickerAmount = 0.301f;
        s_pl_MainDoor = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_MainDoor, "pointLight_MainDoor");
    }
    {
        E_pointLight pl = E_pointLight_create({22.83f, 4.16f, -19.62f}, {1.00f, 0.97f, 0.49f}, 0.20f, 3.59f);
        pl.flicker = 1; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_10 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_10, "pointLight_10");
    }
    {
        E_pointLight pl = E_pointLight_create({12.43f, 2.23f, -7.14f}, {1.00f, 1.00f, 1.00f}, 0.19f, 4.00f);
        pl.flicker = 1; pl.flickerSpeed = 0.607f; pl.flickerAmount = 0.500f;
        s_pl_receptionTable = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_receptionTable, "pointLight_receptionTable");
    }
    {
        E_pointLight pointLightBlink12 = E_pointLight_create(
            {5.19f, 2.74f, -11.03f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 4.14f);
        s_pl_blink_12 = E_pointLight_addToScene(&engineScene, pointLightBlink12);
        E_pointLight_setName(s_pl_blink_12, "pointLightBlink12");
        s_pl_blink_12->flicker       = 0;
        s_pl_blink_12->flickerSpeed  = 3.000f;
        s_pl_blink_12->flickerAmount = 0.500f;

        E_pointLight pointLightBlink11 = E_pointLight_create(
            {21.09f, 6.54f, -4.63f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 3.85f);
        s_pl_blink_11 = E_pointLight_addToScene(&engineScene, pointLightBlink11);
        E_pointLight_setName(s_pl_blink_11, "pointLightBlink11");
        s_pl_blink_11->flicker       = 0;
        s_pl_blink_11->flickerSpeed  = 3.000f;
        s_pl_blink_11->flickerAmount = 0.500f;

        E_pointLight pointLightBlink10 = E_pointLight_create(
            {21.35f, 10.55f, -3.04f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 3.52f);
        s_pl_blink_10 = E_pointLight_addToScene(&engineScene, pointLightBlink10);
        E_pointLight_setName(s_pl_blink_10, "pointLightBlink10");
        s_pl_blink_10->flicker       = 0;
        s_pl_blink_10->flickerSpeed  = 3.000f;
        s_pl_blink_10->flickerAmount = 0.500f;

        E_pointLight pointLightBlink9 = E_pointLight_create(
            {14.39f, 9.54f, -7.83f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 4.38f);
        s_pl_blink_9 = E_pointLight_addToScene(&engineScene, pointLightBlink9);
        E_pointLight_setName(s_pl_blink_9, "pointLightBlink9");
        s_pl_blink_9->flicker       = 0;
        s_pl_blink_9->flickerSpeed  = 3.000f;
        s_pl_blink_9->flickerAmount = 0.500f;

        E_pointLight pointLightBlink8 = E_pointLight_create(
            {8.10f, 6.66f, -6.55f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 3.38f);
        s_pl_blink_8 = E_pointLight_addToScene(&engineScene, pointLightBlink8);
        E_pointLight_setName(s_pl_blink_8, "pointLightBlink8");
        s_pl_blink_8->flicker       = 0;
        s_pl_blink_8->flickerSpeed  = 3.000f;
        s_pl_blink_8->flickerAmount = 0.500f;

        E_pointLight pointLightBlink7 = E_pointLight_create(
            {14.39f, 11.04f, -7.83f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 4.06f);
        s_pl_blink_7 = E_pointLight_addToScene(&engineScene, pointLightBlink7);
        E_pointLight_setName(s_pl_blink_7, "pointLightBlink7");
        s_pl_blink_7->flicker       = 0;
        s_pl_blink_7->flickerSpeed  = 3.000f;
        s_pl_blink_7->flickerAmount = 0.500f;

        E_pointLight pointLightBlink6 = E_pointLight_create(
            {7.99f, 10.84f, -7.83f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 3.94f);
        s_pl_blink_6 = E_pointLight_addToScene(&engineScene, pointLightBlink6);
        E_pointLight_setName(s_pl_blink_6, "pointLightBlink6");
        s_pl_blink_6->flicker       = 0;
        s_pl_blink_6->flickerSpeed  = 3.000f;
        s_pl_blink_6->flickerAmount = 0.500f;

        E_pointLight pointLightBlink5 = E_pointLight_create(
            {14.39f, 16.64f, -7.83f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 5.06f);
        s_pl_blink_5 = E_pointLight_addToScene(&engineScene, pointLightBlink5);
        E_pointLight_setName(s_pl_blink_5, "pointLightBlink5");
        s_pl_blink_5->flicker       = 0;
        s_pl_blink_5->flickerSpeed  = 3.000f;
        s_pl_blink_5->flickerAmount = 0.500f;

        E_pointLight pointLightBlink4 = E_pointLight_create(
            {7.99f, 16.24f, -7.83f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 5.06f);
        s_pl_blink_4 = E_pointLight_addToScene(&engineScene, pointLightBlink4);
        E_pointLight_setName(s_pl_blink_4, "pointLightBlink4");
        s_pl_blink_4->flicker       = 0;
        s_pl_blink_4->flickerSpeed  = 3.000f;
        s_pl_blink_4->flickerAmount = 0.500f;

        E_pointLight pointLightBlink3 = E_pointLight_create(
            {0.99f, 5.14f, -4.03f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 3.65f);
        s_pl_blink_3 = E_pointLight_addToScene(&engineScene, pointLightBlink3);
        E_pointLight_setName(s_pl_blink_3, "pointLightBlink3");
        s_pl_blink_3->flicker       = 0;
        s_pl_blink_3->flickerSpeed  = 3.000f;
        s_pl_blink_3->flickerAmount = 0.500f;

        E_pointLight pointLightBlink2 = E_pointLight_create(
            {0.99f, 10.84f, -4.03f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 3.65f);
        s_pl_blink_2 = E_pointLight_addToScene(&engineScene, pointLightBlink2);
        E_pointLight_setName(s_pl_blink_2, "pointLightBlink2");
        s_pl_blink_2->flicker       = 0;
        s_pl_blink_2->flickerSpeed  = 3.000f;
        s_pl_blink_2->flickerAmount = 0.500f;

        E_pointLight pointLightBlink1 = E_pointLight_create(
            {1.79f, 13.94f, -3.63f},
            {0.96f, 1.00f, 0.63f},
            5.00f, 4.06f);
        s_pl_blink_1 = E_pointLight_addToScene(&engineScene, pointLightBlink1);
        E_pointLight_setName(s_pl_blink_1, "pointLightBlink1");
        s_pl_blink_1->flicker       = 0;
        s_pl_blink_1->flickerSpeed  = 3.000f;
        s_pl_blink_1->flickerAmount = 0.500f;
    }
    {
        s_emitter_gateFire = E_emitter_addToScene(&engineScene, 200);
        E_emitter_setName       (s_emitter_gateFire, "emitter_gateFiressssssa");
        E_emitter_setPosition   (s_emitter_gateFire, {11.27f, 1.07f, -2.93f});
        E_emitter_setDirection  (s_emitter_gateFire, {-0.24f, 1.00f, 0.82f});
        E_emitter_setSpread     (s_emitter_gateFire, 17.19f);
        E_emitter_setSpawnRate  (s_emitter_gateFire, 16.69f);
        E_emitter_setLifespan   (s_emitter_gateFire, 2.98f);
        E_emitter_setSpeed      (s_emitter_gateFire, 2.20f);
        E_emitter_setSize       (s_emitter_gateFire, 1.11f, 7.23f);
        E_emitter_setColor      (s_emitter_gateFire, 1.00f, 1.00f, 1.00f, 1.00f);
        E_emitter_setOpacity    (s_emitter_gateFire, 0.20f);
        E_emitter_setPan        (s_emitter_gateFire, 0.00f, 1.00f, 0.50f);
        E_emitter_setTextureType(s_emitter_gateFire, (E_ParticleTextureType)1);
        E_emitter_deactivate    (s_emitter_gateFire);
    }
    {
        s_emitter_gateSmoke = E_emitter_addToScene(&engineScene, 200);
        E_emitter_setName       (s_emitter_gateSmoke, "emitter_gateSmoke");
        E_emitter_setPosition   (s_emitter_gateSmoke, {11.27f, 1.33f, -2.12f});
        E_emitter_setDirection  (s_emitter_gateSmoke, {-0.30f, 0.79f, 0.54f});
        E_emitter_setSpread     (s_emitter_gateSmoke, 17.19f);
        E_emitter_setSpawnRate  (s_emitter_gateSmoke, 10.00f);
        E_emitter_setLifespan   (s_emitter_gateSmoke, 11.49f);
        E_emitter_setSpeed      (s_emitter_gateSmoke, 2.45f);
        E_emitter_setSize       (s_emitter_gateSmoke, 1.50f, 5.19f);
        E_emitter_setColor      (s_emitter_gateSmoke, 0.00f, 0.00f, 0.00f, 1.00f);
        E_emitter_setOpacity    (s_emitter_gateSmoke, 0.68f);
        E_emitter_setPan        (s_emitter_gateSmoke, 0.00f, 1.00f, 0.50f);
        E_emitter_setTextureType(s_emitter_gateSmoke, (E_ParticleTextureType)0);
        E_emitter_deactivate    (s_emitter_gateSmoke);
    }
    {
        E_spotLight spotLight_onReception = E_spotLight_create(
            {10.38f, 4.51f, -13.06f},
            {0.44f, -0.64f, 0.64f},
            {0.95f, 1.00f, 0.89f},
            0.11f,
            15.00f, 25.50f,
            47.17f);
        spotLight_onReception.flicker       = 1;
        spotLight_onReception.flickerSpeed  = 3.000f;
        spotLight_onReception.flickerAmount = 0.500f;
        spotLight_onReception.castShadow    = 1;
        s_sl_onReception = E_spotLight_addToScene(&engineScene, spotLight_onReception);
        E_spotLight_setName(s_sl_onReception, "spotLight_onReception");
    }
    {
        E_spotLight SpotLightOT_window = E_spotLight_create(
            {10.38f, 15.68f, -13.97f},
            { 0.26f, -0.32f,   0.91f},
            { 1.00f,  1.00f,   1.00f},
            0.48f,
            6.80f, 15.00f,
            57.83f);
        SpotLightOT_window.flicker       = 0;
        SpotLightOT_window.flickerSpeed  = 3.000f;
        SpotLightOT_window.flickerAmount = 0.500f;
        SpotLightOT_window.castShadow    = 1;
        s_sl_OT_window = E_spotLight_addToScene(&engineScene, SpotLightOT_window);
        E_spotLight_setName(s_sl_OT_window, "SpotLightOT_window");
        E_spotLight_deactivate(s_sl_OT_window);
    }
    {
        E_spotLight spotLight_washroom = E_spotLight_create(
            {17.20f, 14.70f, -22.18f},
            {-0.34f, -0.20f,   0.92f},
            { 1.00f,  1.00f,   1.00f},
            0.36f,
            15.00f, 25.20f,
            6.41f);
        spotLight_washroom.flicker       = 1;
        spotLight_washroom.flickerSpeed  = 2.024f;
        spotLight_washroom.flickerAmount = 0.500f;
        spotLight_washroom.castShadow    = 0;
        s_sl_washroom = E_spotLight_addToScene(&engineScene, spotLight_washroom);
        E_spotLight_setName(s_sl_washroom, "spotLight_washroom");
    }
    {
        E_spotLight jeep_lamp_1 = E_spotLight_create(
            {1.49f, 1.29f, 30.05f},
            {1.00f, -0.36f, 0.04f},
            {1.00f,  1.00f, 0.67f},
            4.02f,
            7.10f, 15.00f,
            14.32f);
        jeep_lamp_1.flicker       = 1;
        jeep_lamp_1.flickerSpeed  = 3.000f;
        jeep_lamp_1.flickerAmount = 0.500f;
        jeep_lamp_1.castShadow    = 0;
        s_sl_jeep_lamp_1 = E_spotLight_addToScene(&engineScene, jeep_lamp_1);
        E_spotLight_setName(s_sl_jeep_lamp_1, "jeep_lamp_1");
        E_spotLight_deactivate(s_sl_jeep_lamp_1);
    }
    {
        E_spotLight jeep_lamp_2 = E_spotLight_create(
            {1.49f, 1.29f, 28.95f},
            {1.00f, -0.36f, 0.04f},
            {1.00f,  1.00f, 0.67f},
            3.14f,
            7.80f, 15.10f,
            14.32f);
        jeep_lamp_2.flicker       = 1;
        jeep_lamp_2.flickerSpeed  = 3.000f;
        jeep_lamp_2.flickerAmount = 0.500f;
        jeep_lamp_2.castShadow    = 0;
        s_sl_jeep_lamp_2 = E_spotLight_addToScene(&engineScene, jeep_lamp_2);
        E_spotLight_setName(s_sl_jeep_lamp_2, "jeep_lamp_2");
        E_spotLight_deactivate(s_sl_jeep_lamp_2);
    }
    {
        s_spline_sadMusic = E_scene_addSpline(&engineScene, "spline_sadMusic");
        s_spline_sadMusic->numPoints    = 6;
        s_spline_sadMusic->points[0] = vmath::vec3(14.924f, 13.791f, -8.928f);
        s_spline_sadMusic->points[1] = vmath::vec3(14.924f, 13.791f, -8.928f);
        s_spline_sadMusic->points[2] = vmath::vec3(14.563f, 13.791f, -8.723f);
        s_spline_sadMusic->points[3] = vmath::vec3(14.240f, 13.791f, -8.263f);
        s_spline_sadMusic->points[4] = vmath::vec3(14.223f, 13.791f, -7.589f);
        s_spline_sadMusic->points[5] = vmath::vec3(14.299f, 13.791f, -7.311f);
    }
    {
        s_spline_sadMusicToWindow = E_scene_addSpline(&engineScene, "spline_sadMusicToWindow");
        s_spline_sadMusicToWindow->numPoints    = 6;
        s_spline_sadMusicToWindow->points[0] = vmath::vec3(14.223f, 13.791f, -7.589f);
        s_spline_sadMusicToWindow->points[1] = vmath::vec3(14.223f, 13.791f, -7.589f);
        s_spline_sadMusicToWindow->points[2] = vmath::vec3(14.291f, 13.796f, -7.111f);
        s_spline_sadMusicToWindow->points[3] = vmath::vec3(13.100f, 13.967f, -6.056f);
        s_spline_sadMusicToWindow->points[4] = vmath::vec3( 7.352f, 13.846f, -3.355f);
        s_spline_sadMusicToWindow->points[5] = vmath::vec3( 6.334f, 13.848f, -2.680f);
    }
    {
        s_spline_policeRevealLookat = E_scene_addSpline(&engineScene, "spline_policeRevealLookat");
        s_spline_policeRevealLookat->numPoints    = 4;
        s_spline_policeRevealLookat->points[0] = vmath::vec3(15.127f, 13.649f, -7.882f);
        s_spline_policeRevealLookat->points[1] = vmath::vec3(15.127f, 13.649f, -7.882f);
        s_spline_policeRevealLookat->points[2] = vmath::vec3(11.056f,  1.043f, 13.564f);
        s_spline_policeRevealLookat->points[3] = vmath::vec3(11.056f,  1.043f, 13.564f);
    }
    {
        s_spline_showPoliceSpotLights = E_scene_addSpline(&engineScene, "splineShowPoliceSpotLights");
        s_spline_showPoliceSpotLights->numPoints    = 7;
        s_spline_showPoliceSpotLights->points[0] = vmath::vec3( 7.561f, 1.000f, 14.962f);
        s_spline_showPoliceSpotLights->points[1] = vmath::vec3( 7.561f, 1.000f, 14.962f);
        s_spline_showPoliceSpotLights->points[2] = vmath::vec3( 9.149f, 1.000f, 16.181f);
        s_spline_showPoliceSpotLights->points[3] = vmath::vec3(11.016f, 1.134f, 16.989f);
        s_spline_showPoliceSpotLights->points[4] = vmath::vec3(14.682f, 1.495f, 16.698f);
        s_spline_showPoliceSpotLights->points[5] = vmath::vec3(17.138f, 1.656f, 15.016f);
        s_spline_showPoliceSpotLights->points[6] = vmath::vec3(17.637f, 1.656f, 14.853f);
    }
    {
        s_spline_deliverCam_1 = E_scene_addSpline(&engineScene, "spline_deliverCam_1");
        s_spline_deliverCam_1->numPoints    = 6;
        s_spline_deliverCam_1->points[0] = vmath::vec3(16.802f, 13.365f, -9.457f);
        s_spline_deliverCam_1->points[1] = vmath::vec3(16.802f, 13.365f, -9.457f);
        s_spline_deliverCam_1->points[2] = vmath::vec3(16.950f, 13.365f, -7.657f);
        s_spline_deliverCam_1->points[3] = vmath::vec3(16.743f, 13.365f, -6.094f);
        s_spline_deliverCam_1->points[4] = vmath::vec3(15.978f, 13.365f, -5.750f);
        s_spline_deliverCam_1->points[5] = vmath::vec3(15.579f, 13.365f, -5.759f);
    }
    {
        s_spline_policeCam_1 = E_scene_addSpline(&engineScene, "spline_policeCam_1");
        s_spline_policeCam_1->numPoints    = 4;
        s_spline_policeCam_1->points[0] = vmath::vec3(15.146f, 0.262f, 25.464f);
        s_spline_policeCam_1->points[1] = vmath::vec3(14.146f, 0.562f, 25.464f);
        s_spline_policeCam_1->points[2] = vmath::vec3(10.111f, 0.662f, 25.709f);
        s_spline_policeCam_1->points[3] = vmath::vec3( 9.411f, 0.662f, 25.709f);
    }
    {
        s_spline_deliverCam_2 = E_scene_addSpline(&engineScene, "spline_deliverCam_2");
        s_spline_deliverCam_2->numPoints    = 4;
        s_spline_deliverCam_2->points[0] = vmath::vec3(17.502f, 13.072f, -8.704f);
        s_spline_deliverCam_2->points[1] = vmath::vec3(17.502f, 13.072f, -8.704f);
        s_spline_deliverCam_2->points[2] = vmath::vec3(17.578f, 13.072f, -7.704f);
        s_spline_deliverCam_2->points[3] = vmath::vec3(17.578f, 13.072f, -7.704f);
    }
    {
        s_spline_lobbyShootingShot = E_scene_addSpline(&engineScene, "spline_lobbyShootingShot");
        s_spline_lobbyShootingShot->numPoints    = 5;
        s_spline_lobbyShootingShot->points[0] = vmath::vec3( 8.980f, 14.084f, -2.175f);
        s_spline_lobbyShootingShot->points[1] = vmath::vec3( 8.980f, 14.084f, -2.175f);
        s_spline_lobbyShootingShot->points[2] = vmath::vec3(12.108f, 14.072f, -1.858f);
        s_spline_lobbyShootingShot->points[3] = vmath::vec3(16.819f, 10.832f, -1.582f);
        s_spline_lobbyShootingShot->points[4] = vmath::vec3(17.782f, 10.527f, -2.632f);
    }
    {
        s_spline_deliveryToLobbyLookAt = E_scene_addSpline(&engineScene, "spline_deliveryToLobbyLookAt");
        s_spline_deliveryToLobbyLookAt->numPoints    = 5;
        s_spline_deliveryToLobbyLookAt->points[0] = vmath::vec3(12.798f, 14.049f, -8.135f);
        s_spline_deliveryToLobbyLookAt->points[1] = vmath::vec3(12.798f, 14.049f, -8.135f);
        s_spline_deliveryToLobbyLookAt->points[2] = vmath::vec3(14.587f, 13.935f, -7.865f);
        s_spline_deliveryToLobbyLookAt->points[3] = vmath::vec3(13.677f, 10.163f, -3.659f);
        s_spline_deliveryToLobbyLookAt->points[4] = vmath::vec3(13.681f,  9.201f, -2.865f);
    }
    {
        s_spline_policeDeath = E_scene_addSpline(&engineScene, "spline_policeDeath");
        s_spline_policeDeath->numPoints    = 4;
        s_spline_policeDeath->points[0] = vmath::vec3(17.279f, 14.459f, -3.961f);
        s_spline_policeDeath->points[1] = vmath::vec3(17.279f, 14.459f, -3.961f);
        s_spline_policeDeath->points[2] = vmath::vec3(10.212f, 14.353f, -4.019f);
        s_spline_policeDeath->points[3] = vmath::vec3(10.212f, 14.353f, -4.019f);
    }
    {
        s_spline_finalDelivery = E_scene_addSpline(&engineScene, "spline_finalDelivery");
        s_spline_finalDelivery->numPoints    = 4;
        s_spline_finalDelivery->points[0] = vmath::vec3(12.384f, 13.593f, -9.233f);
        s_spline_finalDelivery->points[1] = vmath::vec3(12.384f, 13.593f, -9.233f);
        s_spline_finalDelivery->points[2] = vmath::vec3(10.527f, 13.839f, -4.043f);
        s_spline_finalDelivery->points[3] = vmath::vec3(10.527f, 13.839f, -4.043f);
    }
    {
        s_spline_toMorningLookAt = E_scene_addSpline(&engineScene, "spline_ToMorningLookAt");
        s_spline_toMorningLookAt->numPoints    = 6;
        s_spline_toMorningLookAt->points[0] = vmath::vec3(15.222f, 13.698f, -8.132f);
        s_spline_toMorningLookAt->points[1] = vmath::vec3(15.222f, 13.698f, -8.132f);
        s_spline_toMorningLookAt->points[2] = vmath::vec3(11.696f, 13.318f, 26.569f);
        s_spline_toMorningLookAt->points[3] = vmath::vec3(12.676f, 18.205f, 39.925f);
        s_spline_toMorningLookAt->points[4] = vmath::vec3(13.063f, 27.475f, 49.177f);
        s_spline_toMorningLookAt->points[5] = vmath::vec3(13.131f, 30.446f, 51.904f);
    }
}

void CamaHospital_begin()
{
    static int s_begun = 0;
    if (s_begun) return;
    s_begun = 1;

    E_fog_setEnabled(&engineScene.fog, 1);
    E_fog_setColor(&engineScene.fog, {0.50f, 0.50f, 0.50f});
    E_fog_setDensity(&engineScene.fog, 0.03f);
    E_fog_setType(&engineScene.fog, 0);

    E_directionalLight_setDirection(&engineScene.directionalLight, {0.00f, -1.00f, 0.00f});
    E_directionalLight_setColor(&engineScene.directionalLight, {1.00f, 1.00f, 1.00f});
    E_directionalLight_setIntensity(&engineScene.directionalLight, 0.00f);

    E_ambientLight_setColor(&engineScene.ambientLight, {1.00f, 1.00f, 1.00f});
    E_ambientLight_setIntensity(&engineScene.ambientLight, 0.10f);

    E_camera_setProjection(63.56f, 0.10f, 1000.00f);
    E_camera_setPosition(14.10f, 6.46f, 22.52f);
    E_camera_lookAtPoint(13.855f, 2.070f, -8.005f);

    CamaHospital_activate();
}

void CamaHospital_activate()
{
    if (s_hospitalScene)      E_sceneObject_setActive(s_hospitalScene,      1);
    if (s_ceiling)             E_sceneObject_setActive(s_ceiling,             1);
    if (s_skySphere)           E_sceneObject_setActive(s_skySphere,           1);
    if (s_policeJeep)                        E_sceneObject_setActive(s_policeJeep,                        0);
    if (s_policeJeep2)                       E_sceneObject_setActive(s_policeJeep2,                       0);
    if (s_policeSupportFir)                  E_sceneObject_setActive(s_policeSupportFir,                  0);
    if (s_policeShooting)                    E_sceneObject_setActive(s_policeShooting,                    0);
    if (s_policeSupportFirShotgun)           E_sceneObject_setActive(s_policeSupportFirShotgun,           0);
    if (s_policeShootingPistol)              E_sceneObject_setActive(s_policeShootingPistol,              0);
    if (s_policeStandingIdleWithPistol)      E_sceneObject_setActive(s_policeStandingIdleWithPistol,      0);
    if (s_policeKaka1IdleCrouchingAiming)    E_sceneObject_setActive(s_policeKaka1IdleCrouchingAiming,    0);
    if (s_policeKaka1PistolIdle)             E_sceneObject_setActive(s_policeKaka1PistolIdle,             0);
    if (s_policeOneRifleAimingIdle)          E_sceneObject_setActive(s_policeOneRifleAimingIdle,          0);
    if (s_policeOnePistolKneelingIdle)       E_sceneObject_setActive(s_policeOnePistolKneelingIdle,       0);
    if (s_policeOnePistolStandingIdle)       E_sceneObject_setActive(s_policeOnePistolStandingIdle,       0);
    if (s_pregnentLady)           E_sceneObject_setActive(s_pregnentLady,           1);
    if (s_nurseStandingNearPhone) E_sceneObject_setActive(s_nurseStandingNearPhone, 1);
    if (s_nurseRunInside)         E_sceneObject_setActive(s_nurseRunInside,         1);
    if (s_pl_OTRight)             E_pointLight_activate(s_pl_OTRight);
    if (s_pl_OTLeft)              E_pointLight_activate(s_pl_OTLeft);
    if (s_pl_OTFront1)            E_pointLight_activate(s_pl_OTFront1);
    if (s_pl_OTFront2)            E_pointLight_activate(s_pl_OTFront2);
    if (s_pl_OTFront3)            E_pointLight_activate(s_pl_OTFront3);
    if (s_pl_OTLeftCorridor1)     E_pointLight_activate(s_pl_OTLeftCorridor1);
    if (s_pl_OTLeftCorridor2)     E_pointLight_activate(s_pl_OTLeftCorridor2);
    if (s_pl_OTRightCorridor2)    E_pointLight_activate(s_pl_OTRightCorridor2);
    if (s_pl_MainDoor)            E_pointLight_activate(s_pl_MainDoor);
    if (s_pl_10)                  E_pointLight_activate(s_pl_10);
    if (s_pl_receptionTable)      E_pointLight_activate(s_pl_receptionTable);
    if (s_pl_blink_1)             E_pointLight_activate(s_pl_blink_1);
    if (s_pl_blink_2)             E_pointLight_activate(s_pl_blink_2);
    if (s_pl_blink_3)             E_pointLight_activate(s_pl_blink_3);
    if (s_pl_blink_4)             E_pointLight_activate(s_pl_blink_4);
    if (s_pl_blink_5)             E_pointLight_activate(s_pl_blink_5);
    if (s_pl_blink_6)             E_pointLight_activate(s_pl_blink_6);
    if (s_pl_blink_7)             E_pointLight_activate(s_pl_blink_7);
    if (s_pl_blink_8)             E_pointLight_activate(s_pl_blink_8);
    if (s_pl_blink_9)             E_pointLight_activate(s_pl_blink_9);
    if (s_pl_blink_10)            E_pointLight_activate(s_pl_blink_10);
    if (s_pl_blink_11)            E_pointLight_activate(s_pl_blink_11);
    if (s_pl_blink_12)            E_pointLight_activate(s_pl_blink_12);
    if (s_pl_upper_5)             E_pointLight_activate(s_pl_upper_5);
    if (s_pl_medicine)            E_pointLight_activate(s_pl_medicine);
    if (s_pl_pregnantLadyCloseup) E_pointLight_activate(s_pl_pregnantLadyCloseup);
    if (s_pl_T1_gun)              E_pointLight_deactivate(s_pl_T1_gun);
    if (s_sl_OT_window)           E_spotLight_deactivate(s_sl_OT_window);
    if (s_sl_washroom)            E_spotLight_activate(s_sl_washroom);
    if (s_sl_jeep_lamp_1)         E_spotLight_deactivate(s_sl_jeep_lamp_1);
    if (s_sl_jeep_lamp_2)         E_spotLight_deactivate(s_sl_jeep_lamp_2);
    if (s_emitter_gateFire)           E_emitter_deactivate(s_emitter_gateFire);
    if (s_emitter_gateSmoke)          E_emitter_deactivate(s_emitter_gateSmoke);
    if (s_secGuard1FallingBackDeath)  E_sceneObject_setActive(s_secGuard1FallingBackDeath, 0);
    if (s_secGuard1FlyingBackDeath)   E_sceneObject_setActive(s_secGuard1FlyingBackDeath,  0);
    if (s_secGuard1IdleStanding)      E_sceneObject_setActive(s_secGuard1IdleStanding,     1);
    if (s_secGuard1IdleStanding2)     E_sceneObject_setActive(s_secGuard1IdleStanding2,    1);
    if (s_terrorist1WalkingWithGun)           E_sceneObject_setActive(s_terrorist1WalkingWithGun,          1);
    if (s_terrorist2WalkingWithRifle)         E_sceneObject_setActive(s_terrorist2WalkingWithRifle,        1);
    if (s_terrorist1WalkingWithGunAK47)       E_sceneObject_setActive(s_terrorist1WalkingWithGunAK47,      1);
    if (s_terrorist2WalkingWithRifleAK47)     E_sceneObject_setActive(s_terrorist2WalkingWithRifleAK47,    1);
    if (s_terrorist1GunplayAK47)              E_sceneObject_setActive(s_terrorist1GunplayAK47,             0);
    if (s_terrorist2HidingAndShootingAK47)    E_sceneObject_setActive(s_terrorist2HidingAndShootingAK47,   0);
    if (s_adityaBreathingIdle)                E_sceneObject_setActive(s_adityaBreathingIdle,               1);
    if (s_doctorDoingDelivery)               E_sceneObject_setActive(s_doctorDoingDelivery,              1);
    if (s_kanganaIdleButAlert)                E_sceneObject_setActive(s_kanganaIdleButAlert,               1);
    if (s_nurseBendingIdle)                 E_sceneObject_setActive(s_nurseBendingIdle,                1);
}

void CamaHospital_deactivate()
{
    if (s_hospitalScene)      E_sceneObject_setActive(s_hospitalScene,      0);
    if (s_ceiling)             E_sceneObject_setActive(s_ceiling,             0);
    if (s_skySphere)           E_sceneObject_setActive(s_skySphere,           0);
    if (s_policeJeep)                        E_sceneObject_setActive(s_policeJeep,                        0);
    if (s_policeJeep2)                       E_sceneObject_setActive(s_policeJeep2,                       0);
    if (s_policeSupportFir)                  E_sceneObject_setActive(s_policeSupportFir,                  0);
    if (s_policeShooting)                    E_sceneObject_setActive(s_policeShooting,                    0);
    if (s_policeSupportFirShotgun)           E_sceneObject_setActive(s_policeSupportFirShotgun,           0);
    if (s_policeShootingPistol)              E_sceneObject_setActive(s_policeShootingPistol,              0);

    if (s_policeStandingIdleWithPistol)      E_sceneObject_setActive(s_policeStandingIdleWithPistol,      0);
    if (s_policeKaka1IdleCrouchingAiming)    E_sceneObject_setActive(s_policeKaka1IdleCrouchingAiming,    0);
    if (s_policeKaka1PistolIdle)             E_sceneObject_setActive(s_policeKaka1PistolIdle,             0);
    if (s_policeOneRifleAimingIdle)          E_sceneObject_setActive(s_policeOneRifleAimingIdle,          0);
    if (s_policeOnePistolKneelingIdle)       E_sceneObject_setActive(s_policeOnePistolKneelingIdle,       0);
    if (s_policeOnePistolStandingIdle)       E_sceneObject_setActive(s_policeOnePistolStandingIdle,       0);
    if (s_pregnentLady)           E_sceneObject_setActive(s_pregnentLady,           0);
    if (s_nurseStandingNearPhone) E_sceneObject_setActive(s_nurseStandingNearPhone, 0);
    if (s_nurseRunInside)         E_sceneObject_setActive(s_nurseRunInside,         0);
    if (s_pl_OTRight)             E_pointLight_deactivate(s_pl_OTRight);
    if (s_pl_OTLeft)              E_pointLight_deactivate(s_pl_OTLeft);
    if (s_pl_OTFront1)            E_pointLight_deactivate(s_pl_OTFront1);
    if (s_pl_OTFront2)            E_pointLight_deactivate(s_pl_OTFront2);
    if (s_pl_OTFront3)            E_pointLight_deactivate(s_pl_OTFront3);
    if (s_pl_OTLeftCorridor1)     E_pointLight_deactivate(s_pl_OTLeftCorridor1);
    if (s_pl_OTLeftCorridor2)     E_pointLight_deactivate(s_pl_OTLeftCorridor2);
    if (s_pl_OTRightCorridor2)    E_pointLight_deactivate(s_pl_OTRightCorridor2);
    if (s_pl_MainDoor)            E_pointLight_deactivate(s_pl_MainDoor);
    if (s_pl_10)                  E_pointLight_deactivate(s_pl_10);
    if (s_pl_receptionTable)      E_pointLight_deactivate(s_pl_receptionTable);
    if (s_pl_blink_1)             E_pointLight_deactivate(s_pl_blink_1);
    if (s_pl_blink_2)             E_pointLight_deactivate(s_pl_blink_2);
    if (s_pl_blink_3)             E_pointLight_deactivate(s_pl_blink_3);
    if (s_pl_blink_4)             E_pointLight_deactivate(s_pl_blink_4);
    if (s_pl_blink_5)             E_pointLight_deactivate(s_pl_blink_5);
    if (s_pl_blink_6)             E_pointLight_deactivate(s_pl_blink_6);
    if (s_pl_blink_7)             E_pointLight_deactivate(s_pl_blink_7);
    if (s_pl_blink_8)             E_pointLight_deactivate(s_pl_blink_8);
    if (s_pl_blink_9)             E_pointLight_deactivate(s_pl_blink_9);
    if (s_pl_blink_10)            E_pointLight_deactivate(s_pl_blink_10);
    if (s_pl_blink_11)            E_pointLight_deactivate(s_pl_blink_11);
    if (s_pl_blink_12)            E_pointLight_deactivate(s_pl_blink_12);
    if (s_pl_upper_5)             E_pointLight_deactivate(s_pl_upper_5);
    if (s_pl_medicine)            E_pointLight_deactivate(s_pl_medicine);
    if (s_pl_pregnantLadyCloseup) E_pointLight_deactivate(s_pl_pregnantLadyCloseup);
    if (s_pl_T1_gun)              E_pointLight_deactivate(s_pl_T1_gun);
    if (s_sl_OT_window)           E_spotLight_deactivate(s_sl_OT_window);
    if (s_sl_washroom)            E_spotLight_deactivate(s_sl_washroom);
    if (s_sl_jeep_lamp_1)         E_spotLight_deactivate(s_sl_jeep_lamp_1);
    if (s_sl_jeep_lamp_2)         E_spotLight_deactivate(s_sl_jeep_lamp_2);
    if (s_emitter_gateFire)           E_emitter_deactivate(s_emitter_gateFire);
    if (s_emitter_gateSmoke)          E_emitter_deactivate(s_emitter_gateSmoke);
    if (s_secGuard1FallingBackDeath)  E_sceneObject_setActive(s_secGuard1FallingBackDeath, 0);
    if (s_secGuard1FlyingBackDeath)   E_sceneObject_setActive(s_secGuard1FlyingBackDeath,  0);
    if (s_secGuard1IdleStanding)      E_sceneObject_setActive(s_secGuard1IdleStanding,     0);
    if (s_secGuard1IdleStanding2)     E_sceneObject_setActive(s_secGuard1IdleStanding2,    0);
    if (s_terrorist1WalkingWithGun)           E_sceneObject_setActive(s_terrorist1WalkingWithGun,          0);
    if (s_terrorist2WalkingWithRifle)         E_sceneObject_setActive(s_terrorist2WalkingWithRifle,        0);
    if (s_terrorist1WalkingWithGunAK47)       E_sceneObject_setActive(s_terrorist1WalkingWithGunAK47,      0);
    if (s_terrorist2WalkingWithRifleAK47)     E_sceneObject_setActive(s_terrorist2WalkingWithRifleAK47,    0);
    if (s_terrorist1GunplayAK47)              E_sceneObject_setActive(s_terrorist1GunplayAK47,             0);
    if (s_terrorist2HidingAndShootingAK47)    E_sceneObject_setActive(s_terrorist2HidingAndShootingAK47,   0);
    if (s_adityaBreathingIdle)                E_sceneObject_setActive(s_adityaBreathingIdle,               0);
    if (s_doctorDoingDelivery)               E_sceneObject_setActive(s_doctorDoingDelivery,              0);
    if (s_kanganaIdleButAlert)                E_sceneObject_setActive(s_kanganaIdleButAlert,               0);
    if (s_nurseBendingIdle)                 E_sceneObject_setActive(s_nurseBendingIdle,                0);
    if (s_channelGate_open)       E_sceneObject_setActive(s_channelGate_open,       0);
    if (s_channelGate_closed)     E_sceneObject_setActive(s_channelGate_closed,     0);
    if (s_channelGate_broken)     E_sceneObject_setActive(s_channelGate_broken,     0);
    if (s_nurseAtReception)       E_sceneObject_setActive(s_nurseAtReception,       0);
    if (s_terrorist1Gunplay)           E_sceneObject_setActive(s_terrorist1Gunplay,           0);
    if (s_terrorist2HidingAndShooting) E_sceneObject_setActive(s_terrorist2HidingAndShooting, 0);

}

void CamaHospital_updateWorld()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    if (now > 363.2f && now <= 370.2f)
    {
        if (now > 363.8f && s_pl_blink_4)  E_pointLight_deactivate(s_pl_blink_4);
        if (now > 364.4f && s_pl_blink_11) E_pointLight_deactivate(s_pl_blink_11);
        if (now > 365.1f && s_pl_blink_2)  E_pointLight_deactivate(s_pl_blink_2);
        if (now > 365.9f && s_pl_blink_9)  E_pointLight_deactivate(s_pl_blink_9);
        if (now > 366.6f && s_pl_blink_6)  E_pointLight_deactivate(s_pl_blink_6);
        if (now > 367.3f && s_pl_blink_12) E_pointLight_deactivate(s_pl_blink_12);
        if (now > 368.0f && s_pl_blink_1)  E_pointLight_deactivate(s_pl_blink_1);
        if (now > 368.3f && s_pl_blink_8)  E_pointLight_deactivate(s_pl_blink_8);
        if (now > 368.7f && s_pl_blink_5)  E_pointLight_deactivate(s_pl_blink_5);
        if (now > 369.1f && s_pl_blink_10) E_pointLight_deactivate(s_pl_blink_10);
        if (now > 369.5f && s_pl_blink_3)  E_pointLight_deactivate(s_pl_blink_3);
        if (now > 369.9f && s_pl_blink_7)  E_pointLight_deactivate(s_pl_blink_7);
    }

    if (s_ceiling)
        E_sceneObject_setActive(s_ceiling, now >= 405.2f ? 1 : 0);

    // Gunplay terrorists and gun light active after 7:20
    if (now >= 440.2f)
    {
        if (s_terrorist1Gunplay)           E_sceneObject_setActive(s_terrorist1Gunplay,           1);
        if (s_terrorist2HidingAndShooting) E_sceneObject_setActive(s_terrorist2HidingAndShooting, 1);
        CamaHospital_syncAK47ToTerrorist(s_terrorist1GunplayAK47, s_terrorist1Gunplay, s_terrorist1GunplayAK47PosOffset, s_terrorist1GunplayAK47RotOffset);
        CamaHospital_syncAK47ToTerrorist(s_terrorist2HidingAndShootingAK47, s_terrorist2HidingAndShooting, s_terrorist2HidingAndShootingAK47PosOffset, s_terrorist2HidingAndShootingAK47RotOffset);
    }
    else
    {
        if (s_terrorist1Gunplay)           E_sceneObject_setActive(s_terrorist1Gunplay,           0);
        if (s_terrorist2HidingAndShooting) E_sceneObject_setActive(s_terrorist2HidingAndShooting, 0);
        if (s_terrorist1GunplayAK47)           E_sceneObject_setActive(s_terrorist1GunplayAK47,           0);
        if (s_terrorist2HidingAndShootingAK47) E_sceneObject_setActive(s_terrorist2HidingAndShootingAK47, 0);
    }
    if (s_pl_T1_gun)
    {
        if (now >= 440.2f) E_pointLight_activate(s_pl_T1_gun);
        else               E_pointLight_deactivate(s_pl_T1_gun);
    }

    // Gate visibility: open by default, closed at 6:05, broken at 6:21
    if (s_channelGate_open && s_channelGate_closed && s_channelGate_broken)
    {
        int showOpen   = (now < 365.2f) ? 1 : 0;
        int showClosed = (now >= 365.2f && now < 381.2f) ? 1 : 0;
        int showBroken = (now >= 381.2f) ? 1 : 0;
        E_sceneObject_setActive(s_channelGate_open,   showOpen);
        E_sceneObject_setActive(s_channelGate_closed, showClosed);
        E_sceneObject_setActive(s_channelGate_broken, showBroken);
    }
    if (s_sl_OT_window)
    {
        if (now >= 405.2f) E_spotLight_activate(s_sl_OT_window);
        else               E_spotLight_deactivate(s_sl_OT_window);
    }
    if (s_pl_pregnantLadyCloseup)
    {
        if (now < 457.2f) E_pointLight_activate(s_pl_pregnantLadyCloseup);
        else              E_pointLight_deactivate(s_pl_pregnantLadyCloseup);
    }

    // 6:00 - 6:03  nurse runs along splineNurseRunFromPhone
    if (now > 360.2f && now <= 363.2f)
    {
        if (s_nurseAtReception) E_sceneObject_setActive(s_nurseAtReception, 0);
        if (s_nurseRunInside && s_splineNurseRunFromPhone)
        {
            E_sceneObject_setActive(s_nurseRunInside, 1);
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineNurseRunFromPhone, 360.2f, 363.2f, 0.4f);
            E_sceneObject_setPosition(s_nurseRunInside, pos);
        }
    }
    else if (s_nurseRunInside)
    {
        E_sceneObject_setActive(s_nurseRunInside, 0);
    }

    // 6:20.5  gate fire + smoke activate permanently, camera shake
    {
        static int s_fireActivated = 0;
        if (!s_fireActivated && now >= 380.7f)
        {
            s_fireActivated = 1;
            if (s_emitter_gateFire)  E_emitter_activate(s_emitter_gateFire);
            if (s_emitter_gateSmoke) E_emitter_activate(s_emitter_gateSmoke);
            E_camera_addTrauma(4.0f);
        }
    }

    // 6:20.5  death guards visible + play; idle guards hidden
    {
        static int s_guardsPlaying = 0;
        if (!s_guardsPlaying && now >= 380.7f)
        {
            s_guardsPlaying = 1;
            if (s_secGuard1FallingBackDeath)
            {
                E_sceneObject_setActive(s_secGuard1FallingBackDeath, 1);
                if (s_secGuard1FallingBackDeath->animator)
                    E_animator_resume(s_secGuard1FallingBackDeath->animator);
            }
            if (s_secGuard1FlyingBackDeath)
            {
                E_sceneObject_setActive(s_secGuard1FlyingBackDeath, 1);
                if (s_secGuard1FlyingBackDeath->animator)
                    E_animator_resume(s_secGuard1FlyingBackDeath->animator);
            }
            if (s_secGuard1IdleStanding)  E_sceneObject_setActive(s_secGuard1IdleStanding,  0);
            if (s_secGuard1IdleStanding2) E_sceneObject_setActive(s_secGuard1IdleStanding2, 0);
        }
    }

    // freeze death guards on last frame once their clip finishes
    {
        static int s_fallingDone = 0;
        if (!s_fallingDone && s_secGuard1FallingBackDeath && s_secGuard1FallingBackDeath->animator && now >= 380.7f)
        {
            float dur = E_animator_getDuration(s_secGuard1FallingBackDeath->animator);
            if (now >= 380.7f + dur)
            {
                s_fallingDone = 1;
                E_animator_setTime (s_secGuard1FallingBackDeath->animator, dur - 0.05f);
                E_animator_setSpeed(s_secGuard1FallingBackDeath->animator, 0.0f);
            }
        }
    }
    {
        static int s_flyingDone = 0;
        if (!s_flyingDone && s_secGuard1FlyingBackDeath && s_secGuard1FlyingBackDeath->animator && now >= 380.7f)
        {
            float dur = E_animator_getDuration(s_secGuard1FlyingBackDeath->animator);
            if (now >= 380.7f + dur)
            {
                s_flyingDone = 1;
                E_animator_setTime (s_secGuard1FlyingBackDeath->animator, dur - 0.05f);
                E_animator_setSpeed(s_secGuard1FlyingBackDeath->animator, 0.0f);
            }
        }
    }

    // 6:10 - 6:18  T1 and T2 move on reveal splines
    if (now > 370.2f && now <= 378.2f)
    {
        if (s_terrorist1WalkingWithGun && s_splineTerroristReveal_T1_lookat)
        {
            E_sceneObject_setActive  (s_terrorist1WalkingWithGun, 1);
            vmath::vec3 t1pos = E_spline_getPositionBetweenTime(s_splineTerroristReveal_T1_lookat, 370.2f, 378.2f, 0.8f);
            E_sceneObject_setPosition(s_terrorist1WalkingWithGun, t1pos);
            CamaHospital_syncAK47ToTerrorist(s_terrorist1WalkingWithGunAK47, s_terrorist1WalkingWithGun, s_terrorist1WalkingWithGunAK47PosOffset, s_terrorist1WalkingWithGunAK47RotOffset);
        }
        if (s_terrorist2WalkingWithRifle && s_splineTerroristReveal_T2_lookat)
        {
            E_sceneObject_setActive  (s_terrorist2WalkingWithRifle, 1);
            vmath::vec3 t2pos = E_spline_getPositionBetweenTime(s_splineTerroristReveal_T2_lookat, 370.2f, 378.2f, 0.8f);
            E_sceneObject_setPosition(s_terrorist2WalkingWithRifle, t2pos);
            CamaHospital_syncAK47ToTerrorist(s_terrorist2WalkingWithRifleAK47, s_terrorist2WalkingWithRifle, s_terrorist2WalkingWithRifleAK47PosOffset, s_terrorist2WalkingWithRifleAK47RotOffset);
        }
    }

    // 6:23 - 6:25.1  terrorists walk through gate on their splines
    else if (now > 383.2f && now <= 385.3f)
    {
        if (s_terrorist1WalkingWithGun && s_spline_T1_enterGate)
        {
            E_sceneObject_setActive(s_terrorist1WalkingWithGun, 1);
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_spline_T1_enterGate, 383.2f, 385.3f, 1.0f);
            E_sceneObject_setPosition(s_terrorist1WalkingWithGun, pos);
            E_sceneObject_setRotation(s_terrorist1WalkingWithGun, E_spline_getRotationAtT(s_spline_T1_enterGate,
                E_Utils_lerpBetweenTime(now, 383.2f, 385.3f, 0.0f, 1.0f)));
            CamaHospital_syncAK47ToTerrorist(s_terrorist1WalkingWithGunAK47, s_terrorist1WalkingWithGun, s_terrorist1WalkingWithGunAK47PosOffset, s_terrorist1WalkingWithGunAK47RotOffset);
        }
        if (s_terrorist2WalkingWithRifle && s_spline_T2_enterGate)
        {
            E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 1);
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_spline_T2_enterGate, 383.2f, 385.3f, 1.0f);
            E_sceneObject_setPosition(s_terrorist2WalkingWithRifle, pos);
            E_sceneObject_setRotation(s_terrorist2WalkingWithRifle, E_spline_getRotationAtT(s_spline_T2_enterGate,
                E_Utils_lerpBetweenTime(now, 383.2f, 385.3f, 0.0f, 1.0f)));
            CamaHospital_syncAK47ToTerrorist(s_terrorist2WalkingWithRifleAK47, s_terrorist2WalkingWithRifle, s_terrorist2WalkingWithRifleAK47PosOffset, s_terrorist2WalkingWithRifleAK47RotOffset);
        }
    }
    // 6:25.1 - 6:28  terrorists walk to stairs on their splines
    else if (now > 385.3f && now <= 388.2f)
    {
        if (s_terrorist1WalkingWithGun && s_splineToStair_T1)
        {
            E_sceneObject_setActive(s_terrorist1WalkingWithGun, 1);
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineToStair_T1, 385.3f, 388.2f, 1.0f);
            E_sceneObject_setPosition(s_terrorist1WalkingWithGun, pos);
            E_sceneObject_setRotation(s_terrorist1WalkingWithGun, E_spline_getRotationAtT(s_splineToStair_T1,
                E_Utils_lerpBetweenTime(now, 385.3f, 388.2f, 0.0f, 1.0f)));
            CamaHospital_syncAK47ToTerrorist(s_terrorist1WalkingWithGunAK47, s_terrorist1WalkingWithGun, s_terrorist1WalkingWithGunAK47PosOffset, s_terrorist1WalkingWithGunAK47RotOffset);
        }
        if (s_terrorist2WalkingWithRifle && s_splineToStair_T2)
        {
            E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 1);
            vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineToStair_T2, 385.3f, 388.2f, 1.0f);
            E_sceneObject_setPosition(s_terrorist2WalkingWithRifle, pos);
            E_sceneObject_setRotation(s_terrorist2WalkingWithRifle, E_spline_getRotationAtT(s_splineToStair_T2,
                E_Utils_lerpBetweenTime(now, 385.3f, 388.2f, 0.0f, 1.0f)));
            CamaHospital_syncAK47ToTerrorist(s_terrorist2WalkingWithRifleAK47, s_terrorist2WalkingWithRifle, s_terrorist2WalkingWithRifleAK47PosOffset, s_terrorist2WalkingWithRifleAK47RotOffset);
        }
    }
    // 6:28 - 6:31  terrorists climb stairs (lerp position)
    else if (now > 388.2f && now <= 391.2f)
    {
        static const vmath::vec3 lerpPos1_T1 = vmath::vec3(19.912f, 0.975f, -20.340f);
        static const vmath::vec3 lerpPos2_T1 = vmath::vec3(22.325f, 2.673f, -20.381f);
        static const vmath::vec3 lerpPos1_T2 = vmath::vec3(20.027f, 1.046f, -19.406f);
        static const vmath::vec3 lerpPos2_T2 = vmath::vec3(22.126f, 2.597f, -19.321f);
        float alpha = E_Utils_lerpBetweenTime(now, 388.2f, 391.2f, 0.0f, 1.0f);

        if (s_terrorist1WalkingWithGun)
        {
            E_sceneObject_setActive(s_terrorist1WalkingWithGun, 1);
            vmath::vec3 pos = lerpPos1_T1 + (lerpPos2_T1 - lerpPos1_T1) * alpha;
            E_sceneObject_setPosition(s_terrorist1WalkingWithGun, pos);
            CamaHospital_syncAK47ToTerrorist(s_terrorist1WalkingWithGunAK47, s_terrorist1WalkingWithGun, s_terrorist1WalkingWithGunAK47PosOffset, s_terrorist1WalkingWithGunAK47RotOffset);
        }
        if (s_terrorist2WalkingWithRifle)
        {
            E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 1);
            vmath::vec3 pos = lerpPos1_T2 + (lerpPos2_T2 - lerpPos1_T2) * alpha;
            E_sceneObject_setPosition(s_terrorist2WalkingWithRifle, pos);
            CamaHospital_syncAK47ToTerrorist(s_terrorist2WalkingWithRifleAK47, s_terrorist2WalkingWithRifle, s_terrorist2WalkingWithRifleAK47PosOffset, s_terrorist2WalkingWithRifleAK47RotOffset);
        }
    }
    
    // 6:31 - 6:33  climb stairs to OT
    else if (now > 391.2f && now <= 396.3f)
    {
        static const vmath::vec3 lerpPos1_T1 = vmath::vec3(20.006f,  9.029f, -20.583f);
        static const vmath::vec3 lerpPos2_T1 = vmath::vec3(22.547f, 10.731f, -20.579f);
        static const vmath::vec3 lerpPos1_T2 = vmath::vec3(20.078f,  8.976f, -19.131f);
        static const vmath::vec3 lerpPos2_T2 = vmath::vec3(22.440f, 10.718f, -19.245f);
        float alpha = E_Utils_lerpBetweenTime(now, 391.2f, 396.3f, 0.0f, 1.0f);

        if (s_terrorist1WalkingWithGun)
        {
            E_sceneObject_setActive(s_terrorist1WalkingWithGun, 1);
            vmath::vec3 pos = lerpPos1_T1 + (lerpPos2_T1 - lerpPos1_T1) * alpha;
            E_sceneObject_setPosition(s_terrorist1WalkingWithGun, pos);
            CamaHospital_syncAK47ToTerrorist(s_terrorist1WalkingWithGunAK47, s_terrorist1WalkingWithGun, s_terrorist1WalkingWithGunAK47PosOffset, s_terrorist1WalkingWithGunAK47RotOffset);
        }
        if (s_terrorist2WalkingWithRifle)
        {
            E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 1);
            vmath::vec3 pos = lerpPos1_T2 + (lerpPos2_T2 - lerpPos1_T2) * alpha;
            E_sceneObject_setPosition(s_terrorist2WalkingWithRifle, pos);
            CamaHospital_syncAK47ToTerrorist(s_terrorist2WalkingWithRifleAK47, s_terrorist2WalkingWithRifle, s_terrorist2WalkingWithRifleAK47PosOffset, s_terrorist2WalkingWithRifleAK47RotOffset);
        }
    }
    // 6:45 - 6:51  T1 fast run on upper floor
    else if (now > 405.2f && now <= 411.2f)
    {
        if (s_terrorist1WalkingWithGun && s_splinesT1_fastRun)
        {
            float t = E_Utils_lerpBetweenTime(now, 405.2f, 411.2f, 0.0f, 1.0f);
            vmath::vec3 pos = E_spline_getPositionAtT(s_splinesT1_fastRun, t);
            E_sceneObject_setActive  (s_terrorist1WalkingWithGun, 1);
            E_sceneObject_setPosition(s_terrorist1WalkingWithGun, pos);
            E_sceneObject_setRotation(s_terrorist1WalkingWithGun, E_spline_getRotationAtT(s_splinesT1_fastRun, t));
            CamaHospital_syncAK47ToTerrorist(s_terrorist1WalkingWithGunAK47, s_terrorist1WalkingWithGun, s_terrorist1WalkingWithGunAK47PosOffset, s_terrorist1WalkingWithGunAK47RotOffset);
        }
        if (s_terrorist2WalkingWithRifle) E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 0);
        if (s_terrorist2WalkingWithRifleAK47) E_sceneObject_setActive(s_terrorist2WalkingWithRifleAK47, 0);
    }
    // 6:51 - 6:57  T1 moves back toward OT
    else if (now > 411.2f && now <= 417.2f)
    {
        if (s_terrorist1WalkingWithGun && s_splinesT1_backtoOT)
        {
            float t = E_Utils_lerpBetweenTime(now, 411.2f, 417.2f, 0.0f, 1.0f);
            vmath::vec3 pos = E_spline_getPositionAtT(s_splinesT1_backtoOT, t);
            E_sceneObject_setActive  (s_terrorist1WalkingWithGun, 1);
            E_sceneObject_setPosition(s_terrorist1WalkingWithGun, pos);
            E_sceneObject_setRotation(s_terrorist1WalkingWithGun, E_spline_getRotationAtT(s_splinesT1_backtoOT, t));
            CamaHospital_syncAK47ToTerrorist(s_terrorist1WalkingWithGunAK47, s_terrorist1WalkingWithGun, s_terrorist1WalkingWithGunAK47PosOffset, s_terrorist1WalkingWithGunAK47RotOffset);
        }
        if (s_terrorist2WalkingWithRifle) E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 0);
        if (s_terrorist2WalkingWithRifleAK47) E_sceneObject_setActive(s_terrorist2WalkingWithRifleAK47, 0);
    }
    // 7:03 - 7:06  T1 runs away after siren
    else if (now > 423.2f && now <= 426.2f)
    {
        if (s_terrorist1WalkingWithGun && s_splineT1_runAwayAfterSiren)
        {
            float t = E_Utils_lerpBetweenTime(now, 423.2f, 426.2f, 0.0f, 1.0f);
            vmath::vec3 pos = E_spline_getPositionAtT(s_splineT1_runAwayAfterSiren, t);
            E_sceneObject_setActive  (s_terrorist1WalkingWithGun, 1);
            E_sceneObject_setPosition(s_terrorist1WalkingWithGun, pos);
            E_sceneObject_setRotation(s_terrorist1WalkingWithGun, E_spline_getRotationAtT(s_splineT1_runAwayAfterSiren, t));
            CamaHospital_syncAK47ToTerrorist(s_terrorist1WalkingWithGunAK47, s_terrorist1WalkingWithGun, s_terrorist1WalkingWithGunAK47PosOffset, s_terrorist1WalkingWithGunAK47RotOffset);
        }
        if (s_terrorist2WalkingWithRifle) E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 0);
        if (s_terrorist2WalkingWithRifleAK47) E_sceneObject_setActive(s_terrorist2WalkingWithRifleAK47, 0);
    }
    else
    {
        if (s_terrorist1WalkingWithGun)   E_sceneObject_setActive(s_terrorist1WalkingWithGun,   0);
        if (s_terrorist2WalkingWithRifle) E_sceneObject_setActive(s_terrorist2WalkingWithRifle, 0);
        if (s_terrorist1WalkingWithGunAK47)   E_sceneObject_setActive(s_terrorist1WalkingWithGunAK47,   0);
        if (s_terrorist2WalkingWithRifleAK47) E_sceneObject_setActive(s_terrorist2WalkingWithRifleAK47, 0);
    }

    // 6:57 - 7:03  Police jeep 1 moves on road spline, headlamps track it
    if (now > 417.2f && now <= 423.2f)
    {
        E_fog_setEnabled(&engineScene.fog, 1);
        E_fog_setColor  (&engineScene.fog, {0.50f, 0.50f, 0.50f});
        E_fog_setDensity(&engineScene.fog, 0.01f);
        E_fog_setType   (&engineScene.fog, 0);

        if (s_policeJeep && s_splinesPoliceVanOnRoad)
        {
            float t = E_Utils_lerpBetweenTime(now, 417.2f, 423.2f, 0.0f, 1.0f);
            vmath::vec3 jeepPos = E_spline_getPositionAtT(s_splinesPoliceVanOnRoad, t);
            E_sceneObject_setActive  (s_policeJeep, 1);
            E_sceneObject_setPosition(s_policeJeep, jeepPos);
            E_sceneObject_setRotation(s_policeJeep, E_spline_getRotationAtT(s_splinesPoliceVanOnRoad, t));

            if (s_sl_jeep_lamp_1)
            {
                E_spotLight_activate(s_sl_jeep_lamp_1);
                s_sl_jeep_lamp_1->position = vmath::vec3(jeepPos[0] + 1.49f, jeepPos[1] + 1.29f, jeepPos[2] + 0.765f);
            }
            if (s_sl_jeep_lamp_2)
            {
                E_spotLight_activate(s_sl_jeep_lamp_2);
                s_sl_jeep_lamp_2->position = vmath::vec3(jeepPos[0] + 1.49f, jeepPos[1] + 1.29f, jeepPos[2] + (-0.335f));
            }
        }
    }
    else if (now > 423.2f)
    {
        static int s_jeepReset_done = 0;
        if (!s_jeepReset_done)
        {
            s_jeepReset_done = 1;
            if (s_sl_jeep_lamp_1) E_spotLight_deactivate(s_sl_jeep_lamp_1);
            if (s_sl_jeep_lamp_2) E_spotLight_deactivate(s_sl_jeep_lamp_2);
            if (s_policeJeep)
            {
                E_sceneObject_setPosition(s_policeJeep, { 8.17f, -0.04f, 12.61f});
                E_sceneObject_setRotation(s_policeJeep, { 0.00f, 132.00f,  0.00f});
                E_sceneObject_setActive  (s_policeJeep, 1);
            }
        }
    }

    // 7:20+  Police jeeps and all police models become visible
    if (now > 440.2f)
    {
        if (s_policeJeep)                     E_sceneObject_setActive(s_policeJeep,                     1);
        if (s_policeJeep2)                    E_sceneObject_setActive(s_policeJeep2,                    1);
        if (s_policeSupportFir)               E_sceneObject_setActive(s_policeSupportFir,               1);
        if (s_policeShooting)                 E_sceneObject_setActive(s_policeShooting,                 1);
        CamaHospital_syncAK47ToTerrorist(s_policeSupportFirShotgun, s_policeSupportFir, s_policeSupportFirShotgunPosOffset, s_policeSupportFirShotgunRotOffset);
        CamaHospital_syncAK47ToTerrorist(s_policeShootingPistol, s_policeShooting, s_policeShootingPistolPosOffset, s_policeShootingPistolRotOffset);
        if (s_policeStandingIdleWithPistol)   E_sceneObject_setActive(s_policeStandingIdleWithPistol,   1);
        if (s_policeKaka1IdleCrouchingAiming) E_sceneObject_setActive(s_policeKaka1IdleCrouchingAiming, 1);
        if (s_policeKaka1PistolIdle)          E_sceneObject_setActive(s_policeKaka1PistolIdle,          1);
        if (s_policeOneRifleAimingIdle)       E_sceneObject_setActive(s_policeOneRifleAimingIdle,       1);
        if (s_policeOnePistolKneelingIdle)    E_sceneObject_setActive(s_policeOnePistolKneelingIdle,    1);
        if (s_policeOnePistolStandingIdle)    E_sceneObject_setActive(s_policeOnePistolStandingIdle,    1);
    }

    // 7:34+  Police and delivery spot lights: ramp on over 0.5s, then Lissajous direction sweep
    if (now > 454.2f)
    {
        float ramp = E_Utils_lerpBetweenTime(now, 454.2f, 454.7f, 0.0f, 1.0f);

        if (s_sl_delivery)        { E_spotLight_activate(s_sl_delivery);        E_spotLight_setIntensity(s_sl_delivery,        ramp * 4.13f); }
        if (s_sl_police_top)      { E_spotLight_activate(s_sl_police_top);      E_spotLight_setIntensity(s_sl_police_top,      ramp * 3.53f); }
        if (s_sl_police_1)          E_spotLight_activate(s_sl_police_1);
        if (s_sl_police_1_copy)     E_spotLight_activate(s_sl_police_1_copy);

        if (s_sl_police_1)
        {
            E_spotLight_setIntensity(s_sl_police_1, ramp * 7.91f);
            float dx = 0.420f  + 0.490f  * sinf(0.7f * now);
            float dy = 0.5075f + 0.2575f * sinf(1.4f * now + 0.5f);
            E_spotLight_setDirection(s_sl_police_1, {dx, dy, -0.99f});
        }

        if (s_sl_police_1_copy)
        {
            E_spotLight_setIntensity(s_sl_police_1_copy, ramp * 8.72f);
            float dx = -0.227f + 0.420f * sinf(0.5f * now);
            float dy =  0.277f + 0.163f * sinf(1.0f * now + 1.0f);
            E_spotLight_setDirection(s_sl_police_1_copy, {dx, dy, -0.92f});
        }
    }
}

void CamaHospital_updateCamera()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    // 5:36 - 5:48  Camera travels along gate top spline
    if (now >= 336.2f && now <= 348.2f)
    {
        E_postProcess_setBarrel(1, 0.5f, 0.0500f);
        E_camera_setProjection(63.56f, 0.10f, 1000.00f);
        vmath::vec3 pos = E_spline_getPositionBetweenTime(s_spline_GateTop, 336.2f, 348.2f, 1.0f);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(13.855f, 2.070f, -8.005f);
    }

    // 5:48 - 5:55  Camera travels along spline to phone
    if (now >= 348.2f && now <= 355.2f)
    {
        vmath::vec3 pos = E_spline_getPositionBetweenTime(s_spline_ToPhone, 348.2f, 355.2f, 1.0f);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        float lx = E_Utils_lerpBetweenTime(now, 352.2f, 360.2f, 13.855f, 13.964f);
        float ly = E_Utils_lerpBetweenTime(now, 352.2f, 360.2f,  2.070f,  1.653f);
        float lz = E_Utils_lerpBetweenTime(now, 352.2f, 360.2f, -8.005f, -7.905f);
        E_camera_lookAtPoint(lx, ly, lz);
    }

    // 5:55 - 6:00  Hold at spline end, continue lerping focus
    if (now > 355.2f && now <= 360.2f)
    {
        E_camera_setPosition(14.120f, 1.753f,  -7.294f);
        float lx = E_Utils_lerpBetweenTime(now, 352.2f, 360.2f, 13.855f, 13.964f);
        float ly = E_Utils_lerpBetweenTime(now, 352.2f, 360.2f,  2.070f,  1.653f);
        float lz = E_Utils_lerpBetweenTime(now, 352.2f, 360.2f, -8.005f, -7.905f);
        E_camera_lookAtPoint(lx, ly, lz);
    }

    // 6:00 - 6:03  Camera on nurse run spline, lookat on nurse leg spline
    if (now > 360.2f && now <= 363.2f)
    {
        vmath::vec3 pos    = E_spline_getPositionBetweenTime(s_splineToNurseRun,        360.2f, 363.2f, 1.0f);
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineToNurseLeg_lookAt, 360.2f, 363.2f, 1.0f);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(lookAt);
    }

    // 6:03 - 6:10  Camera on lights off spline
    if (now > 363.2f && now <= 370.2f)
    {
        vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineLightsOff, 363.2f, 370.2f, 1.0f);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(12.026f, 11.477f, -2.347f);
    }

    // 6:10 - 6:18  Static camera, lookat on T1 reveal spline, DOF focused on T1
    if (now > 370.2f && now <= 378.2f)
    {
        float camX = 14.513f, camY = 0.723f, camZ = 23.993f;
        E_camera_setPosition(camX, camY, camZ);
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineTerroristReveal_T1_lookat, 370.2f, 378.2f, 1.0f);
        E_camera_lookAtPoint(lookAt);

        float dx = lookAt[0] - camX;
        float dy = lookAt[1] - camY;
        float dz = lookAt[2] - camZ;
        float focusDist = sqrtf(dx*dx + dy*dy + dz*dz);
        E_postProcess_setDOF(1, focusDist, focusDist, 24.54f);

        // 6:15 - 6:18  lerp FOV to 25
        if (now > 375.2f)
            E_setFOV(E_Utils_lerpBetweenTime(now, 375.2f, 378.2f, 63.56f, 25.0f));
    }

    // reset FOV after 6:18
    if (now > 378.2f && now <= 378.4f)
        E_setFOV(63.56f);

    // 6:18 - 6:23  Static camera
    if (now > 378.2f && now <= 383.2f)
    {
        E_postProcess_setCCTV(1, 0.0010f, 1.00f, 0.00f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.0500f);
        E_camera_setPosition(5.561f, 5.911f, 2.566f);
        E_camera_lookAtPoint(12.045f, 1.078f, -3.063f);
    }

    // 6:23 - 6:25.1
    if (now > 383.2f && now <= 385.3f)
    {
        E_postProcess_setCCTV(1, 0.0010f, 1.00f, 0.00f, 1.00f);
        E_postProcess_setBarrel(1, 1.0000f, 0.0500f);
        E_camera_setPosition(15.114f, 4.001f, -19.116f);
        E_camera_lookAtPoint(12.155f, 1.228f, -4.929f);
    }

    // 6:25.1 - 6:28
    if (now > 385.3f && now <= 388.2f)
    {
        E_camera_setPosition(8.215f, 4.096f, -18.085f);
        E_camera_lookAtPoint(13.723f, 0.870f, -19.236f);
    }

    // 6:28 - 6:31
    if (now > 388.2f && now <= 391.2f)
    {
        E_camera_setPosition(17.981f, 2.507f, -23.639f);
        E_camera_lookAtPoint(22.276f, 3.902f, -20.308f);
    }

    // 6:31 - 6:45  Camera on splinesToBathroomTopToOT_move, looking straight down
    if (now > 391.2f && now <= 405.2f)
    {
        static int s_cctvOff_begun = 0;
        if (!s_cctvOff_begun) { s_cctvOff_begun = 1; E_postProcess_setCCTV(0, 0.0f, 0.0f, 0.0f, 0.0f); }
        E_setFOV(120.0f);
        float t = E_Utils_lerpBetweenTime(now, 391.2f, 405.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_splinesToBathroomTopToOT_move, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(pos[0], pos[1] - 1.0f, pos[2]);
        E_postProcess_setDOF(1, 3.12f, 2.02f, 11.06f);
    }

    // 6:45 - 6:51  Fixed cam, FOV 100, barrel max
    if (now > 405.2f && now <= 411.2f)
    {
        E_setFOV(100.0f);
        E_postProcess_setBarrel(1, 1.0000f, 0.0500f);
        E_camera_setPosition(11.133f, 14.134f, -7.386f);
        E_camera_lookAtPoint(11.645f, 14.333f, -10.116f);
        {
            float dx = 11.645f - 11.133f;
            float dy = 14.333f - 14.134f;
            float dz = -10.116f - (-7.386f);
            E_postProcess_setDOF(1, sqrtf(dx*dx + dy*dy + dz*dz), 3.00f, 10.0f);
        }
    }

    // 6:51 - 6:54  Camera moves on heartBeatShadow spline, fixed lookat, FOV 85
    if (now > 411.2f && now <= 414.2f)
    {
        E_setFOV(85.0f);
        float t = E_Utils_lerpBetweenTime(now, 411.2f, 414.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_spline_heartBeatShadow, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(11.893f, 14.275f, -10.093f);
        {
            float dx = 11.893f - pos[0];
            float dy = 14.275f - pos[1];
            float dz = -10.093f - pos[2];
            E_postProcess_setDOF(1, sqrtf(dx*dx + dy*dy + dz*dz), 3.00f, 10.0f);
        }
    }

    // 6:54 - 6:57  Hold cam, lookat pans on heartBeatShadowLookAt spline, FOV lerps to 25
    if (now > 414.2f && now <= 417.2f)
    {
        E_camera_setPosition(10.580f, 14.031f, -9.522f);
        float t = E_Utils_lerpBetweenTime(now, 414.2f, 417.2f, 0.0f, 1.0f);
        vmath::vec3 lookAt = E_spline_getPositionAtT(s_spline_heartBeatShadowLookAt, t);
        E_camera_lookAtPoint(lookAt);
        E_setFOV(E_Utils_lerpBetweenTime(now, 414.2f, 417.2f, 85.0f, 25.0f));
        {
            float dx = lookAt[0] - 10.580f;
            float dy = lookAt[1] - 14.031f;
            float dz = lookAt[2] - (-9.522f);
            E_postProcess_setDOF(1, sqrtf(dx*dx + dy*dy + dz*dz), 3.00f, 10.0f);
        }
    }

    // 6:57 - 7:03  Fixed cam, barrel ON, CCTV OFF, lookat police van on road
    if (now > 417.2f && now <= 423.2f)
    {
        E_setFOV(100.0f);
        E_postProcess_setCCTV  (0, 0.0f, 0.0f, 0.0f, 0.0f);
        E_postProcess_setBarrel(1, 1.0000f, 0.0500f);
        E_camera_setPosition(13.657f, 14.047f, -3.897f);
        if (s_splinesPoliceVanOnRoad)
        {
            float t = E_Utils_lerpBetweenTime(now, 417.2f, 423.2f, 0.0f, 1.0f);
            vmath::vec3 lookAt = E_spline_getPositionAtT(s_splinesPoliceVanOnRoad, t);
            E_camera_lookAtPoint(lookAt);
            float dx = lookAt[0] - 13.657f;
            float dy = lookAt[1] - 14.047f;
            float dz = lookAt[2] - (-3.897f);
            E_postProcess_setDOF(1, sqrtf(dx*dx + dy*dy + dz*dz), 3.00f, 10.0f);
        }
    }

    // 7:03 - 7:06  Fixed cam, lookat terrorist running away
    if (now > 423.2f && now <= 426.2f)
    {
        E_camera_setPosition(11.717f, 14.109f, -7.433f);
        E_camera_lookAtPoint(12.050f, 14.244f, -10.001f);
        {
            float dx = 12.050f - 11.717f;
            float dy = 14.244f - 14.109f;
            float dz = -10.001f - (-7.433f);
            E_postProcess_setDOF(1, sqrtf(dx*dx + dy*dy + dz*dz), 3.00f, 10.0f);
        }
    }

    // 7:12 - 7:24  Camera moves on sadMusic spline, lookat fixed on window area
    if (now >= 432.2f && now <= 444.2f)
    {
        static int s_sadMusic_begun = 0;
        if (!s_sadMusic_begun)
        {
            s_sadMusic_begun = 1;
            E_setFOV(60.0f);
            E_directionalLight_setDirection (&engineScene.directionalLight, {0.00f, -1.00f,  0.00f});
            E_directionalLight_setColor     (&engineScene.directionalLight, {1.00f,  1.00f,  1.00f});
            E_directionalLight_setIntensity (&engineScene.directionalLight, 0.00f);
            E_ambientLight_setColor         (&engineScene.ambientLight,     {1.00f,  1.00f,  1.00f});
            E_ambientLight_setIntensity     (&engineScene.ambientLight,     0.00f);
            E_fog_setEnabled (&engineScene.fog, 1);
            E_fog_setColor   (&engineScene.fog, {1.00f, 1.00f, 1.00f});
            E_fog_setDensity (&engineScene.fog, 0.09f);
            E_fog_setType    (&engineScene.fog, 0);
            E_postProcess_setFXAA      (1);
            E_postProcess_setSaturation(1.02f);
            E_postProcess_setBlur      (0.00f);
            E_postProcess_setVignette  (0.61f, 2.40f);
            E_postProcess_setDOF       (1, 5.22f, 5.00f, 15.86f);
            E_postProcess_setGodRays   (0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
            E_postProcess_setBloom     (0, 1.00f, 3.00f);
            E_postProcess_setCCTV      (0, 0.0020f, 0.10f, 0.04f, 0.25f);
            E_postProcess_setBarrel    (1, 1.0000f, 0.0500f);
            E_postProcess_setGamma     (1.02f);
        }
        float t = E_Utils_lerpBetweenTime(now, 432.2f, 444.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_spline_sadMusic, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(15.127f, 13.649f, -7.882f);
    }

    // 7:24 - 7:35  Camera moves on sadMusicToWindow spline, lookat fixed on window area
    if (now >= 444.2f && now <= 455.2f)
    {
        E_setFOV(60.0f);
        float t = E_Utils_lerpBetweenTime(now, 444.2f, 455.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_spline_sadMusicToWindow, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(15.127f, 13.649f, -7.882f);
        E_fog_setDensity(&engineScene.fog, E_Utils_lerpBetweenTime(now, 444.2f, 455.2f, 0.09f, 0.02f));
    }

    // 7:37 - 7:39  Lookat pans down on policeRevealLookat spline
    if (now >= 457.2f && now <= 459.2f)
    {
        float t = E_Utils_lerpBetweenTime(now, 457.2f, 459.2f, 0.0f, 1.0f);
        vmath::vec3 lookAt = E_spline_getPositionAtT(s_spline_policeRevealLookat, t);
        E_camera_lookAtPoint(lookAt);
        float dx = lookAt[0] - engineCamera.position[0];
        float dy = lookAt[1] - engineCamera.position[1];
        float dz = lookAt[2] - engineCamera.position[2];
        float focusDist = sqrtf(dx*dx + dy*dy + dz*dz);
        E_postProcess_setDOF(1, focusDist, focusDist, 24.54f);
    }

    // 7:39 - 7:41  FOV lerps from 60 to 30, fog density lerps to 0.02
    if (now >= 459.2f && now <= 461.2f)
    {
        E_setFOV(E_Utils_lerpBetweenTime(now, 459.2f, 461.2f, 60.0f, 30.0f));
    }

    // 7:41 - 7:47  Camera moves on showPoliceSpotLights spline, lookat fixed
    if (now >= 461.2f && now <= 467.2f)
    {
        static int s_showPoliceSpotLights_begun = 0;
        if (!s_showPoliceSpotLights_begun)
        {
            s_showPoliceSpotLights_begun = 1;
            E_setFOV(75.0f);
            E_directionalLight_setDirection (&engineScene.directionalLight, {-0.43f, -1.00f, -0.30f});
            E_directionalLight_setColor     (&engineScene.directionalLight, { 0.18f,  0.21f,  0.38f});
            E_directionalLight_setIntensity (&engineScene.directionalLight, 0.92f);
            E_ambientLight_setColor         (&engineScene.ambientLight,     { 1.00f,  1.00f,  1.00f});
            E_ambientLight_setIntensity     (&engineScene.ambientLight,     0.04f);
            E_fog_setEnabled (&engineScene.fog, 1);
            E_fog_setColor   (&engineScene.fog, {0.50f, 0.50f, 0.50f});
            E_fog_setDensity (&engineScene.fog, 0.01f);
            E_fog_setType    (&engineScene.fog, 0);
            E_postProcess_setFXAA      (1);
            E_postProcess_setSaturation(1.01f);
            E_postProcess_setBlur      (0.00f);
            E_postProcess_setVignette  (0.69f, 1.43f);
            E_postProcess_setDOF       (1, 23.03f, 13.60f, 8.49f);
            E_postProcess_setGodRays   (0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
            E_postProcess_setBloom     (0, 1.00f, 3.00f);
            E_postProcess_setCCTV      (0, 0.0000f, 0.10f, 0.05f, 0.00f);
            E_postProcess_setBarrel    (1, 0.3930f, 0.0500f);
            E_postProcess_setGamma     (1.03f);
        }
        float t = E_Utils_lerpBetweenTime(now, 461.2f, 467.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_spline_showPoliceSpotLights, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(12.123f, 9.933f, -2.911f);
    }

    // 7:47 - 7:53  Camera moves on deliverCam_1 spline, lookat fixed
    if (now >= 467.2f && now <= 473.2f)
    {
        static int s_deliverCam_1_begun = 0;
        if (!s_deliverCam_1_begun)
        {
            s_deliverCam_1_begun = 1;
            E_setFOV(50.0f);
            E_postProcess_setFXAA      (1);
            E_postProcess_setSaturation(1.01f);
            E_postProcess_setBlur      (0.00f);
            E_postProcess_setVignette  (0.78f, 1.43f);
            E_postProcess_setDOF       (1, 2.39f, 2.39f, 24.54f);
            E_postProcess_setGodRays   (0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
            E_postProcess_setBloom     (0, 1.00f, 3.00f);
            E_postProcess_setCCTV      (0, 0.0000f, 0.10f, 0.05f, 0.00f);
            E_postProcess_setBarrel    (1, 0.3580f, 0.0500f);
            E_postProcess_setGamma     (1.03f);
        }
        float t = E_Utils_lerpBetweenTime(now, 467.2f, 473.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_spline_deliverCam_1, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(15.183f, 13.607f, -8.201f);
    }

    // 7:53 - 7:59  Camera moves on policeCam_1 spline, lookat fixed
    if (now >= 473.2f && now <= 479.2f)
    {
        static int s_policeCam_1_begun = 0;
        if (!s_policeCam_1_begun)
        {
            s_policeCam_1_begun = 1;
            E_setFOV(30.0f);
            E_postProcess_setFXAA      (1);
            E_postProcess_setSaturation(1.01f);
            E_postProcess_setBlur      (0.00f);
            E_postProcess_setVignette  (0.78f, 1.43f);
            E_postProcess_setDOF       (1, 11.57f, 3.30f, 10.09f);
            E_postProcess_setGodRays   (0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
            E_postProcess_setBloom     (0, 1.00f, 3.00f);
            E_postProcess_setCCTV      (0, 0.0000f, 0.10f, 0.05f, 0.00f);
            E_postProcess_setBarrel    (1, 1.0000f, 0.0500f);
            E_postProcess_setGamma     (1.03f);
        }
        float t = E_Utils_lerpBetweenTime(now, 473.2f, 479.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_spline_policeCam_1, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(12.901f, 1.874f, 14.255f);
    }

    // 7:59 - 8:05  Camera moves on deliverCam_2 spline, lookat fixed
    if (now >= 479.2f && now <= 485.2f)
    {
        static int s_deliverCam_2_begun = 0;
        if (!s_deliverCam_2_begun)
        {
            s_deliverCam_2_begun = 1;
            E_setFOV(50.0f);
            E_postProcess_setFXAA      (1);
            E_postProcess_setSaturation(1.01f);
            E_postProcess_setBlur      (0.00f);
            E_postProcess_setVignette  (0.78f, 1.43f);
            E_postProcess_setDOF       (1, 2.39f, 2.39f, 24.54f);
            E_postProcess_setGodRays   (0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
            E_postProcess_setBloom     (0, 1.00f, 3.00f);
            E_postProcess_setCCTV      (0, 0.0000f, 0.10f, 0.05f, 0.00f);
            E_postProcess_setBarrel    (1, 0.3580f, 0.0500f);
            E_postProcess_setGamma     (1.03f);
        }
        float t = E_Utils_lerpBetweenTime(now, 479.2f, 485.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_spline_deliverCam_2, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(15.157f, 13.637f, -8.239f);
    }

    // 8:05 - 8:11  Camera moves on lobbyShootingShot spline, lookat on deliveryToLobbyLookAt spline
    if (now >= 485.2f && now <= 491.2f)
    {
        static int s_lobbyShot_begun = 0;
        if (!s_lobbyShot_begun)
        {
            s_lobbyShot_begun = 1;
            E_setFOV(88.0f);
            E_postProcess_setVignette(0.78f, 1.43f);
            E_postProcess_setBarrel  (1, 0.4220f, 0.0500f);
        }
        float t = E_Utils_lerpBetweenTime(now, 485.2f, 491.2f, 0.0f, 1.0f);
        vmath::vec3 pos    = E_spline_getPositionAtT(s_spline_lobbyShootingShot,     t);
        vmath::vec3 lookAt = E_spline_getPositionAtT(s_spline_deliveryToLobbyLookAt, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(lookAt);
        float dx = lookAt[0] - pos[0];
        float dy = lookAt[1] - pos[1];
        float dz = lookAt[2] - pos[2];
        E_postProcess_setDOF(1, sqrtf(dx*dx + dy*dy + dz*dz), 3.30f, 7.80f);
    }

    // 8:11 - 8:18  Camera moves on policeDeath spline, lookat fixed
    if (now >= 491.2f && now <= 498.2f)
    {
        static int s_policeDeath_begun = 0;
        if (!s_policeDeath_begun)
        {
            s_policeDeath_begun = 1;
            E_postProcess_setFXAA      (1);
            E_postProcess_setSaturation(1.01f);
            E_postProcess_setBlur      (0.00f);
            E_postProcess_setVignette  (0.78f, 1.43f);
            E_postProcess_setDOF       (1, 2.39f, 3.30f, 11.24f);
            E_postProcess_setGodRays   (0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
            E_postProcess_setBloom     (0, 1.00f, 3.00f);
            E_postProcess_setCCTV      (0, 0.0000f, 0.10f, 0.05f, 0.00f);
            E_postProcess_setBarrel    (1, 0.3580f, 0.0500f);
            E_postProcess_setGamma     (1.03f);
        }
        float t = E_Utils_lerpBetweenTime(now, 491.2f, 498.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_spline_policeDeath, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(10.461f, 0.183f, 11.428f);
        {
            float dx = 10.461f - pos[0];
            float dy =  0.183f - pos[1];
            float dz = 11.428f - pos[2];
            E_postProcess_setDOF(1, sqrtf(dx*dx + dy*dy + dz*dz), 3.30f, 11.24f);
        }
    }

    // 8:18 - 8:24  Camera moves on finalDelivery spline, lookat fixed
    if (now >= 498.2f && now <= 504.2f)
    {
        float t = E_Utils_lerpBetweenTime(now, 498.2f, 504.2f, 0.0f, 1.0f);
        vmath::vec3 pos = E_spline_getPositionAtT(s_spline_finalDelivery, t);
        E_camera_setPosition(pos[0], pos[1], pos[2]);
        E_camera_lookAtPoint(15.222f, 13.698f, -8.132f);
    }

    // 8:24 - 8:32  Lookat pans out on toMorningLookAt spline
    if (now >= 504.2f && now <= 512.2f)
    {
        float t = E_Utils_lerpBetweenTime(now, 504.2f, 512.2f, 0.0f, 1.0f);
        vmath::vec3 lookAt = E_spline_getPositionAtT(s_spline_toMorningLookAt, t);
        E_camera_lookAtPoint(lookAt);
    }

    // 8:32 - 8:41  Fog and ambient light lerp to morning sky
    if (now >= 512.2f && now <= 521.2f)
    {
        static int s_lissajousOff_done = 0;
        if (!s_lissajousOff_done)
        {
            s_lissajousOff_done = 1;
            if (s_sl_delivery)        E_spotLight_deactivate(s_sl_delivery);
            if (s_sl_police_top)      E_spotLight_deactivate(s_sl_police_top);
            if (s_sl_police_1)        E_spotLight_deactivate(s_sl_police_1);
            if (s_sl_police_1_copy)   E_spotLight_deactivate(s_sl_police_1_copy);
        }
        float fogR = E_Utils_lerpBetweenTime(now, 512.2f, 521.2f, 0.50f, 0.51f);
        float fogG = E_Utils_lerpBetweenTime(now, 512.2f, 521.2f, 0.50f, 0.76f);
        float fogB = E_Utils_lerpBetweenTime(now, 512.2f, 521.2f, 0.50f, 1.00f);
        float fogD = E_Utils_lerpBetweenTime(now, 512.2f, 521.2f, 0.01f, 0.002f);
        float ambR = E_Utils_lerpBetweenTime(now, 512.2f, 521.2f, 1.00f, 0.37f);
        float ambG = E_Utils_lerpBetweenTime(now, 512.2f, 521.2f, 1.00f, 0.78f);
        float ambB = E_Utils_lerpBetweenTime(now, 512.2f, 521.2f, 1.00f, 1.00f);
        E_fog_setColor         (&engineScene.fog,         {fogR, fogG, fogB});
        E_fog_setDensity       (&engineScene.fog,         fogD);
        E_ambientLight_setColor(&engineScene.ambientLight, {ambR, ambG, ambB});
        if (s_emitter_gateFire)  E_emitter_setOpacity(s_emitter_gateFire,  E_Utils_lerpBetweenTime(now, 512.2f, 521.2f, 0.20f, 0.20f));
        if (s_emitter_gateSmoke) E_emitter_setOpacity(s_emitter_gateSmoke, E_Utils_lerpBetweenTime(now, 512.2f, 521.2f, 0.68f, 0.20f));
    }

}

void CamaHospital_update()
{
    CamaHospital_updateWorld();
    CamaHospital_updateCamera();
}

void CamaHospital_uninit()
{
    if (s_hospitalScene)             { E_sceneObject_destroyResources(s_hospitalScene);             s_hospitalScene             = NULL; }
    if (s_ceiling)                    { E_sceneObject_destroyResources(s_ceiling);                    s_ceiling                    = NULL; }
    if (s_skySphere)                  { E_sceneObject_destroyResources(s_skySphere);                  s_skySphere                  = NULL; }
    if (s_channelGate_open)              { E_sceneObject_destroyResources(s_channelGate_open);              s_channelGate_open              = NULL; }
    if (s_channelGate_closed)            { E_sceneObject_destroyResources(s_channelGate_closed);            s_channelGate_closed            = NULL; }
    if (s_channelGate_broken)            { E_sceneObject_destroyResources(s_channelGate_broken);            s_channelGate_broken            = NULL; }
    if (s_policeJeep)                    { E_sceneObject_destroyResources(s_policeJeep);                    s_policeJeep                    = NULL; }
    if (s_policeJeep2)                   { E_sceneObject_destroyResources(s_policeJeep2);                   s_policeJeep2                   = NULL; }
    if (s_policeSupportFir)              { E_sceneObject_destroyResources(s_policeSupportFir);              s_policeSupportFir              = NULL; }
    if (s_policeShooting)                { E_sceneObject_destroyResources(s_policeShooting);                s_policeShooting                = NULL; }
    if (s_policeSupportFirShotgun)       { E_sceneObject_destroyResources(s_policeSupportFirShotgun);       s_policeSupportFirShotgun       = NULL; }
    if (s_policeShootingPistol)          { E_sceneObject_destroyResources(s_policeShootingPistol);          s_policeShootingPistol          = NULL; }
    if (s_policeStandingIdleWithPistol)  { E_sceneObject_destroyResources(s_policeStandingIdleWithPistol);  s_policeStandingIdleWithPistol  = NULL; }
    if (s_policeKaka1IdleCrouchingAiming){ E_sceneObject_destroyResources(s_policeKaka1IdleCrouchingAiming);s_policeKaka1IdleCrouchingAiming= NULL; }
    if (s_policeKaka1PistolIdle)         { E_sceneObject_destroyResources(s_policeKaka1PistolIdle);         s_policeKaka1PistolIdle         = NULL; }
    if (s_policeOneRifleAimingIdle)      { E_sceneObject_destroyResources(s_policeOneRifleAimingIdle);      s_policeOneRifleAimingIdle      = NULL; }
    if (s_policeOnePistolKneelingIdle)   { E_sceneObject_destroyResources(s_policeOnePistolKneelingIdle);   s_policeOnePistolKneelingIdle   = NULL; }
    if (s_policeOnePistolStandingIdle)   { E_sceneObject_destroyResources(s_policeOnePistolStandingIdle);   s_policeOnePistolStandingIdle   = NULL; }
    if (s_pregnentLady)                  { E_sceneObject_destroyResources(s_pregnentLady);                  s_pregnentLady                  = NULL; }
    if (s_nurseStandingNearPhone)        { E_sceneObject_destroyResources(s_nurseStandingNearPhone);        s_nurseStandingNearPhone        = NULL; }
    if (s_nurseRunInside)                { E_sceneObject_destroyResources(s_nurseRunInside);                s_nurseRunInside                = NULL; }
    if (s_nurseAtReception)              { E_sceneObject_destroyResources(s_nurseAtReception);              s_nurseAtReception              = NULL; }
    if (s_secGuard1FallingBackDeath)     { E_sceneObject_destroyResources(s_secGuard1FallingBackDeath);     s_secGuard1FallingBackDeath     = NULL; }
    if (s_secGuard1FlyingBackDeath)      { E_sceneObject_destroyResources(s_secGuard1FlyingBackDeath);      s_secGuard1FlyingBackDeath      = NULL; }
    if (s_secGuard1IdleStanding)         { E_sceneObject_destroyResources(s_secGuard1IdleStanding);         s_secGuard1IdleStanding         = NULL; }
    if (s_secGuard1IdleStanding2)        { E_sceneObject_destroyResources(s_secGuard1IdleStanding2);        s_secGuard1IdleStanding2        = NULL; }
    if (s_adityaBreathingIdle)           { E_sceneObject_destroyResources(s_adityaBreathingIdle);           s_adityaBreathingIdle           = NULL; }
    if (s_doctorDoingDelivery)          { E_sceneObject_destroyResources(s_doctorDoingDelivery);          s_doctorDoingDelivery          = NULL; }
    if (s_kanganaIdleButAlert)           { E_sceneObject_destroyResources(s_kanganaIdleButAlert);           s_kanganaIdleButAlert           = NULL; }
    if (s_nurseBendingIdle)            { E_sceneObject_destroyResources(s_nurseBendingIdle);            s_nurseBendingIdle            = NULL; }
    if (s_terrorist1WalkingWithGun)      { E_sceneObject_destroyResources(s_terrorist1WalkingWithGun);      s_terrorist1WalkingWithGun      = NULL; }
    if (s_terrorist2WalkingWithRifle)    { E_sceneObject_destroyResources(s_terrorist2WalkingWithRifle);    s_terrorist2WalkingWithRifle    = NULL; }
    if (s_terrorist1Gunplay)             { E_sceneObject_destroyResources(s_terrorist1Gunplay);             s_terrorist1Gunplay             = NULL; }
    if (s_terrorist2HidingAndShooting)   { E_sceneObject_destroyResources(s_terrorist2HidingAndShooting);   s_terrorist2HidingAndShooting   = NULL; }
    if (s_terrorist1WalkingWithGunAK47)    { E_sceneObject_destroyResources(s_terrorist1WalkingWithGunAK47);    s_terrorist1WalkingWithGunAK47    = NULL; }
    if (s_terrorist2WalkingWithRifleAK47)  { E_sceneObject_destroyResources(s_terrorist2WalkingWithRifleAK47);  s_terrorist2WalkingWithRifleAK47  = NULL; }
    if (s_terrorist1GunplayAK47)           { E_sceneObject_destroyResources(s_terrorist1GunplayAK47);           s_terrorist1GunplayAK47           = NULL; }
    if (s_terrorist2HidingAndShootingAK47) { E_sceneObject_destroyResources(s_terrorist2HidingAndShootingAK47); s_terrorist2HidingAndShootingAK47 = NULL; }

    if (s_emitter_gateFire)  { E_emitter_deactivate(s_emitter_gateFire);  s_emitter_gateFire  = NULL; }
    if (s_emitter_gateSmoke) { E_emitter_deactivate(s_emitter_gateSmoke); s_emitter_gateSmoke = NULL; }

    if (s_sl_delivery)        { E_spotLight_deactivate(s_sl_delivery);        s_sl_delivery        = NULL; }
    if (s_sl_police_top)      { E_spotLight_deactivate(s_sl_police_top);      s_sl_police_top      = NULL; }
    if (s_sl_police_1)        { E_spotLight_deactivate(s_sl_police_1);        s_sl_police_1        = NULL; }
    if (s_sl_police_1_copy)   { E_spotLight_deactivate(s_sl_police_1_copy);   s_sl_police_1_copy   = NULL; }
    if (s_sl_onReception)     { E_spotLight_deactivate(s_sl_onReception);     s_sl_onReception     = NULL; }
    if (s_sl_OT_window)       { E_spotLight_deactivate(s_sl_OT_window);       s_sl_OT_window       = NULL; }
    if (s_sl_washroom)        { E_spotLight_deactivate(s_sl_washroom);        s_sl_washroom        = NULL; }
    if (s_sl_jeep_lamp_1)    { E_spotLight_deactivate(s_sl_jeep_lamp_1);    s_sl_jeep_lamp_1    = NULL; }
    if (s_sl_jeep_lamp_2)    { E_spotLight_deactivate(s_sl_jeep_lamp_2);    s_sl_jeep_lamp_2    = NULL; }

    if (s_pl_10)              { E_pointLight_deactivate(s_pl_10);              s_pl_10              = NULL; }
    if (s_pl_receptionTable)  { E_pointLight_deactivate(s_pl_receptionTable);  s_pl_receptionTable  = NULL; }
    if (s_pl_OTRight)         { E_pointLight_deactivate(s_pl_OTRight);         s_pl_OTRight         = NULL; }
    if (s_pl_OTLeft)          { E_pointLight_deactivate(s_pl_OTLeft);          s_pl_OTLeft          = NULL; }
    if (s_pl_OTFront1)        { E_pointLight_deactivate(s_pl_OTFront1);        s_pl_OTFront1        = NULL; }
    if (s_pl_OTFront2)        { E_pointLight_deactivate(s_pl_OTFront2);        s_pl_OTFront2        = NULL; }
    if (s_pl_OTFront3)        { E_pointLight_deactivate(s_pl_OTFront3);        s_pl_OTFront3        = NULL; }
    if (s_pl_OTLeftCorridor1) { E_pointLight_deactivate(s_pl_OTLeftCorridor1); s_pl_OTLeftCorridor1 = NULL; }
    if (s_pl_OTLeftCorridor2) { E_pointLight_deactivate(s_pl_OTLeftCorridor2); s_pl_OTLeftCorridor2 = NULL; }
    if (s_pl_OTRightCorridor2){ E_pointLight_deactivate(s_pl_OTRightCorridor2);s_pl_OTRightCorridor2= NULL; }
    if (s_pl_MainDoor)        { E_pointLight_deactivate(s_pl_MainDoor);        s_pl_MainDoor        = NULL; }
    if (s_pl_upper_1)         { E_pointLight_deactivate(s_pl_upper_1);         s_pl_upper_1         = NULL; }
    if (s_pl_upper_2)         { E_pointLight_deactivate(s_pl_upper_2);         s_pl_upper_2         = NULL; }
    if (s_pl_upper_3)         { E_pointLight_deactivate(s_pl_upper_3);         s_pl_upper_3         = NULL; }
    if (s_pl_upper_4)         { E_pointLight_deactivate(s_pl_upper_4);         s_pl_upper_4         = NULL; }
    if (s_pl_upper_5)         { E_pointLight_deactivate(s_pl_upper_5);         s_pl_upper_5         = NULL; }
    if (s_pl_medicine)               { E_pointLight_deactivate(s_pl_medicine);               s_pl_medicine               = NULL; }
    if (s_pl_pregnantLadyCloseup)   { E_pointLight_deactivate(s_pl_pregnantLadyCloseup);   s_pl_pregnantLadyCloseup   = NULL; }
    if (s_pl_T1_gun)                { E_pointLight_deactivate(s_pl_T1_gun);                s_pl_T1_gun                = NULL; }
    if (s_pl_blink_1)         { E_pointLight_deactivate(s_pl_blink_1);         s_pl_blink_1         = NULL; }
    if (s_pl_blink_2)         { E_pointLight_deactivate(s_pl_blink_2);         s_pl_blink_2         = NULL; }
    if (s_pl_blink_3)         { E_pointLight_deactivate(s_pl_blink_3);         s_pl_blink_3         = NULL; }
    if (s_pl_blink_4)         { E_pointLight_deactivate(s_pl_blink_4);         s_pl_blink_4         = NULL; }
    if (s_pl_blink_5)         { E_pointLight_deactivate(s_pl_blink_5);         s_pl_blink_5         = NULL; }
    if (s_pl_blink_6)         { E_pointLight_deactivate(s_pl_blink_6);         s_pl_blink_6         = NULL; }
    if (s_pl_blink_7)         { E_pointLight_deactivate(s_pl_blink_7);         s_pl_blink_7         = NULL; }
    if (s_pl_blink_8)         { E_pointLight_deactivate(s_pl_blink_8);         s_pl_blink_8         = NULL; }
    if (s_pl_blink_9)         { E_pointLight_deactivate(s_pl_blink_9);         s_pl_blink_9         = NULL; }
    if (s_pl_blink_10)        { E_pointLight_deactivate(s_pl_blink_10);        s_pl_blink_10        = NULL; }
    if (s_pl_blink_11)        { E_pointLight_deactivate(s_pl_blink_11);        s_pl_blink_11        = NULL; }
    if (s_pl_blink_12)        { E_pointLight_deactivate(s_pl_blink_12);        s_pl_blink_12        = NULL; }
}
