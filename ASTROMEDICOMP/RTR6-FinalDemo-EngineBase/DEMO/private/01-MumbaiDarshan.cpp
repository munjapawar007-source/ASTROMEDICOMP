#include "../public/01-MumbaiDarshan.h"
#include "../public/DEMO.hpp"
#include "engineAPIs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "E_utils.h"
#include "E_animation.h"
#include "E_material.h"
#include "E_water.h"
#include <GL/glew.h>

E_sceneObject* MUMBAI_OCEAN          = NULL;
E_sceneObject* MUMBAI_SCENE          = NULL;
E_sceneObject* MUMBAI_TAJ            = NULL;
E_sceneObject* MUMBAI_GOI            = NULL;
E_sceneObject* MUMBAI_DARSHAN_BUS    = NULL;
E_sceneObject* MUMBAI_OCEAN_FLOOR    = NULL;
E_sceneObject* MUMBAI_SKYSPHERE      = NULL;
E_sceneObject* MUMBAI_TIGHTROPE      = NULL;
E_sceneObject* MUMBAI_TIGHTROPE_GIRL = NULL;
E_sceneObject* MUMBAI_RADIO          = NULL;
E_sceneObject* RopeGirlFocusPt       = NULL;
E_sceneObject* MUMBAI_PLANE          = NULL;
E_sceneObject* MUMBAI_TAXI           = NULL;
E_sceneObject* MUMBAI_BF             = NULL;
E_sceneObject* MUMBAI_GF             = NULL;
E_sceneObject* MUMBAI_CST_EMISSIVE    = NULL;
E_spline*      ESpline_BUS1           = NULL;
E_spline*      ESpline_MumbaiTaxi     = NULL;
E_spline*      ESpline_RadioToGirl    = NULL;
E_spline*      ESpline_RoundTheGirl   = NULL;
E_spline*      PlaneSplineMumbai      = NULL;

static E_spline* s_splineBambai   = NULL;
static E_spline* s_splineMoviePoster        = NULL;
static E_spline* s_splineCamPosMoviePoster   = NULL;
static E_spline* s_splineCouple                = NULL;
static E_spline* s_splineCSTCamPos          = NULL;
static E_spline* s_splineCSTCamLookAt       = NULL;

static E_emitter*    s_emitterTajFire1      = NULL;
static E_emitter*    s_emitterTajSmoke1     = NULL;
static E_pointLight* s_plTajFire = NULL;

static E_pointLight* s_pl_girl        = NULL;
static E_pointLight* s_pl_radio       = NULL;
static E_pointLight* s_pl_vadapav     = NULL;
static E_pointLight* s_pl_taxi        = NULL;
static E_pointLight* s_pl_planeRightWing = NULL;
static E_pointLight* s_pl_planeLeftWing  = NULL;
static vmath::vec3   s_planeRightWingOffset = vmath::vec3(0.0f, 0.0f, 0.0f);
static vmath::vec3   s_planeLeftWingOffset  = vmath::vec3(0.0f, 0.0f, 0.0f);
static E_spotLight*  s_sl_GOI_white   = NULL;
static E_spotLight*  s_sl_GOI_orange  = NULL;
static E_spotLight*  s_sl_GOI_green   = NULL;
static E_spotLight*  s_sl_Taj_1       = NULL;
static E_spotLight*  s_sl_Taj_2       = NULL;
static E_spotLight*  s_sl_Taj_3       = NULL;
static E_spotLight*  s_sl_Taj_4       = NULL;
static E_spotLight*  s_sl_GOI_side    = NULL;
static E_spotLight*  s_sl_poster_1    = NULL;
static E_spotLight*  s_sl_poster_2    = NULL;
static E_spotLight*  s_sl_poster_3    = NULL;
static E_spotLight*  s_sl_poster_4    = NULL;
static E_spotLight*  s_sl_fp_1        = NULL;
static E_spotLight*  s_sl_fp_2        = NULL;
static E_spotLight*  s_sl_fp_3        = NULL;
static E_spotLight*  s_sl_fp_4        = NULL;
static E_spotLight*  s_sl_fp_5        = NULL;
static E_spotLight*  s_sl_fp_6        = NULL;
static E_spotLight*  s_sl_road_1      = NULL;
static E_spotLight*  s_sl_road_2      = NULL;
static E_spotLight*  s_sl_road_3      = NULL;
static E_spotLight*  s_sl_road_4      = NULL;
static E_spotLight*  s_sl_road_5      = NULL;
static E_spotLight*  s_sl_road_6      = NULL;
static E_spotLight*  s_sl_nearGirl        = NULL;
static E_spotLight*  s_sl_nearCouple      = NULL;
static E_spotLight*  s_sl_onBus           = NULL;
static E_spotLight*  s_sl_nearWalkingMan  = NULL;
static E_spotLight*  s_sl_CST_1           = NULL;
static E_spotLight*  s_sl_CST_2           = NULL;
static E_spotLight*  s_sl_CST_3           = NULL;
static E_spotLight*  s_sl_CST_4           = NULL;
static E_spotLight*  s_sl_CST_5           = NULL;
static E_spotLight*  s_sl_CST_6           = NULL;
static E_pointLight* s_pl_CSTCenter       = NULL;
static E_pointLight* s_pl_CSTCompound     = NULL;
static E_pointLight* s_pl_Couple          = NULL;
static E_spotLight*  s_sl_busHeadlamp_left  = NULL;
static E_spotLight*  s_sl_busHeadlamp_right = NULL;
static E_pointLight* s_pl_insideMovingBus   = NULL;

// Footpath 1 walkers  3 parallel lanes, 7 per lane
#define FP1_LANE_COUNT  3
#define FP1_PER_LANE    7
#define s_fp1TOTAL       (FP1_LANE_COUNT * FP1_PER_LANE)

static E_sceneObject* s_fp1Walkers[s_fp1TOTAL];
static E_spline*      s_fp1Splines[FP1_LANE_COUNT];
static float          s_fp1T[s_fp1TOTAL];

// Footpath 2  single lane
#define s_fp2TOTAL 7

static E_sceneObject* s_fp2Walkers[s_fp2TOTAL];
static E_spline*      s_fp2Spline;
static float          s_fp2T[s_fp2TOTAL];

static E_sceneObject* s_fpWalkerRdGirl = NULL;
static E_spline*      s_fpWalkerRdGirlSpline = NULL;
static E_spline*      s_fpWalkerRdGirlLookAtSpline = NULL;

// Crowd watching the tightrope walker
#define GOI_TOTAL 24

static E_sceneObject* s_goiCrowd[GOI_TOTAL];

// Big Ships
#define s_bigShip1_COUNT 2
#define s_bigShip2_COUNT 2

static E_sceneObject* s_bigShip1[s_bigShip1_COUNT];
static E_sceneObject* s_bigShip2[s_bigShip2_COUNT];

// Boats
#define s_boatsB1_COUNT 12

static E_sceneObject* s_boatsB1[s_boatsB1_COUNT];
static float          s_boatsB1X[s_boatsB1_COUNT];
static float          s_boatsB1Z[s_boatsB1_COUNT];
static float          s_boatsB1Phase[s_boatsB1_COUNT];

static vmath::vec3 MumbaiDarshan_rotatePlaneOffset(vmath::vec3 offset, vmath::vec3 planeRot)
{
    vmath::mat4 rot = vmath::rotate(planeRot[0], planeRot[1], planeRot[2]);
    return vmath::vec3(
        rot[0][0] * offset[0] + rot[1][0] * offset[1] + rot[2][0] * offset[2],
        rot[0][1] * offset[0] + rot[1][1] * offset[1] + rot[2][1] * offset[2],
        rot[0][2] * offset[0] + rot[1][2] * offset[1] + rot[2][2] * offset[2]
    );
}

static void MumbaiDarshan_updatePlaneWingLights(vmath::vec3 planePos, vmath::vec3 planeRot)
{
    if (s_pl_planeRightWing)
        E_pointLight_setPosition(s_pl_planeRightWing, planePos + MumbaiDarshan_rotatePlaneOffset(s_planeRightWingOffset, planeRot));
    if (s_pl_planeLeftWing)
        E_pointLight_setPosition(s_pl_planeLeftWing, planePos + MumbaiDarshan_rotatePlaneOffset(s_planeLeftWingOffset, planeRot));
}

void MumbaiDarshan_init()
{
    MUMBAI_SKYSPHERE = E_loadModel("DEMO_ASSETS/SkySphere/SkyDome.glb", &engineScene, "MUMBAI_SKYSPHERE");
    E_material skyMat = E_createMaterialFromFiles("DEMO_ASSETS/SkySphere/skysphere.vert", "DEMO_ASSETS/SkySphere/skysphere.frag");
    skyMat.texture  = E_loadTexture("DEMO_ASSETS/SkySphere/textures/qwantani_dawn_puresky.jpg");
    skyMat.texture1 = E_loadTexture("DEMO_ASSETS/SkySphere/textures/seamlessCloud.png");
    skyMat.texture2 = E_loadTexture("DEMO_ASSETS/SkySphere/textures/perlinNoise2.jpg");
    glProgramUniform1i(skyMat.shaderProgram, glGetUniformLocation(skyMat.shaderProgram, "uTexture_1"), 1);
    glProgramUniform1i(skyMat.shaderProgram, glGetUniformLocation(skyMat.shaderProgram, "uTexture_2"), 2);
    E_sceneObject_setMaterialAll(MUMBAI_SKYSPHERE, skyMat);
    E_sceneObject_setCastShadowAll  (MUMBAI_SKYSPHERE, 0);
    E_sceneObject_setReceiveShadowAll(MUMBAI_SKYSPHERE, 0);

    MUMBAI_OCEAN_FLOOR = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/OceanFloor.glb", &engineScene, "MUMBAI_OCEAN_FLOOR");

    MUMBAI_OCEAN = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/OceanGridMumbai.glb", &engineScene, "MUMBAI_OCEAN");
    E_material waterMat = E_createMaterialFromFiles("engineResources/water.vert", "engineResources/water.frag");
    E_sceneObject_setShaderAll(MUMBAI_OCEAN, waterMat.shaderProgram);
    E_water_setProgram(waterMat.shaderProgram);
    glProgramUniform1f(waterMat.shaderProgram, glGetUniformLocation(waterMat.shaderProgram, "uWaveScale"),          1.555f);
    glProgramUniform1f(waterMat.shaderProgram, glGetUniformLocation(waterMat.shaderProgram, "uWaveAmplitudeScale"), 0.168f);
    glProgramUniform1f(waterMat.shaderProgram, glGetUniformLocation(waterMat.shaderProgram, "uWaveFrequencyScale"), 0.954f);
    glProgramUniform1f(waterMat.shaderProgram, glGetUniformLocation(waterMat.shaderProgram, "uWaveSpeedScale"),     2.458f);
    glProgramUniform1f(waterMat.shaderProgram, glGetUniformLocation(waterMat.shaderProgram, "uWaveSteepnessScale"), 0.951f);
    E_sceneObject_setCastShadowAll (MUMBAI_OCEAN, 0);
    E_sceneObject_setBaseColorAll  (MUMBAI_OCEAN, 1.0f, 1.0f, 1.0f, 0.8f);
    E_sceneObject_setTransparentAll(MUMBAI_OCEAN, 1);

    MUMBAI_SCENE = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/MumbaiScene_v10.glb", &engineScene, "MUMBAI_SCENE");

    MUMBAI_CST_EMISSIVE = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/CSMT_PNG_Export.glb", &engineScene, "MUMBAI_CST_EMISSIVE");

    MUMBAI_TAJ = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/TajEmissive.glb", &engineScene, "MUMBAI_TAJ");

    MUMBAI_GOI = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/GOI.glb", &engineScene, "MUMBAI_GOI");

    MUMBAI_DARSHAN_BUS = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/EmissiveRustedBus.glb", &engineScene, "MUMBAI_DARSHAN_BUS");
    E_sceneObject_setScale(MUMBAI_DARSHAN_BUS, 0.3f);

    MUMBAI_TAXI = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/taxi.glb", &engineScene, "MUMBAI_TAXI");
    E_sceneObject_setPosition(MUMBAI_TAXI, {76.319f, 0.05f, 18.748f});
    E_sceneObject_setRotation(MUMBAI_TAXI, {0.00f, 180.00f, 0.00f});
    E_sceneObject_setScale   (MUMBAI_TAXI, 0.35f);

    E_pointLight pointLight_Taxi = E_pointLight_create(
        {76.32f, 0.45f, -1.73f},
        {0.35f, 0.24f, 1.00f},
        0.15f, 0.49f);
    s_pl_taxi = E_pointLight_addToScene(&engineScene, pointLight_Taxi);
    E_pointLight_setName(s_pl_taxi, "pointLight_Taxi");
    s_pl_taxi->flicker       = 0;
    s_pl_taxi->flickerSpeed  = 3.000f;
    s_pl_taxi->flickerAmount = 0.500f;

    MUMBAI_TIGHTROPE = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/TightRopeWalk/WalkingRopeSetup.glb", &engineScene, "MUMBAI_TIGHTROPE");
    E_sceneObject_setPosition(MUMBAI_TIGHTROPE, {-29.55f, 0.00f, 6.01f});
    E_sceneObject_setScale   (MUMBAI_TIGHTROPE, 2.00f);

    MUMBAI_TIGHTROPE_GIRL = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/TightRopeWalk/WalkingRopeGirl.glb", &engineScene, "MUMBAI_TIGHTROPE_GIRL");
    E_sceneObject_setPosition(MUMBAI_TIGHTROPE_GIRL, {-29.55f, 0.00f, 6.01f});
    E_sceneObject_setScale   (MUMBAI_TIGHTROPE_GIRL, 2.00f);
    E_animator_setClipByName (MUMBAI_TIGHTROPE_GIRL->animator, "Running");
    E_animator_setSpeed      (MUMBAI_TIGHTROPE_GIRL->animator, 0.5f);

    E_spotLight GOI_white = E_spotLight_create(
        {-31.73f, 0.13f, 10.96f},
        {-0.08f, 0.51f, 0.86f},
        {1.00f, 1.00f, 1.00f},
        1.01f,
        15.00f, 34.30f,
        8.12f);
    s_sl_GOI_white = E_spotLight_addToScene(&engineScene, GOI_white);
    E_spotLight_setName(s_sl_GOI_white, "GOI_white");

    E_spotLight GOI_orange = E_spotLight_create(
        {-28.63f, 0.08f, 10.78f},
        {-0.23f, 0.43f, 0.87f},
        {1.00f, 0.38f, 0.00f},
        10.56f,
        15.00f, 24.80f,
        4.57f);
    s_sl_GOI_orange = E_spotLight_addToScene(&engineScene, GOI_orange);
    E_spotLight_setName(s_sl_GOI_orange, "GOI_orange");

    E_spotLight GOI_green = E_spotLight_create(
        {-35.51f, 0.33f, 11.03f},
        {0.33f, 0.31f, 0.89f},
        {0.00f, 1.00f, 0.00f},
        1.65f,
        15.00f, 25.80f,
        5.68f);
    s_sl_GOI_green = E_spotLight_addToScene(&engineScene, GOI_green);
    E_spotLight_setName(s_sl_GOI_green, "GOI_green");

    E_spotLight Taj_light1 = E_spotLight_create(
        {-54.96f, -0.08f, 7.64f},
        {-0.21f, 0.92f, -0.33f},
        {1.00f, 0.32f, 0.00f},
        2.74f,
        15.00f, 62.10f,
        9.59f);
    s_sl_Taj_1 = E_spotLight_addToScene(&engineScene, Taj_light1);
    E_spotLight_setName(s_sl_Taj_1, "Taj_light1");

    E_spotLight Taj_light2 = E_spotLight_create(
        {-51.07f, 0.02f, 5.08f},
        {-0.33f, 0.79f, -0.52f},
        {1.00f, 1.00f, 1.00f},
        0.60f,
        15.00f, 51.10f,
        9.59f);
    s_sl_Taj_2 = E_spotLight_addToScene(&engineScene, Taj_light2);
    E_spotLight_setName(s_sl_Taj_2, "Taj_light2");

    E_spotLight Taj_light3 = E_spotLight_create(
        {-46.42f, 0.17f, 3.62f},
        {-0.36f, 0.72f, -0.59f},
        {0.00f, 1.00f, 0.00f},
        0.90f,
        15.00f, 54.20f,
        9.59f);
    s_sl_Taj_3 = E_spotLight_addToScene(&engineScene, Taj_light3);
    E_spotLight_setName(s_sl_Taj_3, "Taj_light3");

    E_spotLight Taj_light4 = E_spotLight_create(
        {-46.55f, 11.59f, 4.65f},
        {-0.65f, -0.64f, -0.41f},
        {0.21f, 0.00f, 1.00f},
        5.00f,
        15.00f, 62.10f,
        10.00f);
    s_sl_Taj_4 = E_spotLight_addToScene(&engineScene, Taj_light4);
    E_spotLight_setName(s_sl_Taj_4, "Taj_light4");

    E_pointLight girlPointLight = E_pointLight_create(
        {-28.81f, 0.95f, 5.24f},
        {1.00f, 0.93f, 0.50f},
        0.0f, 1.24f);
    s_pl_girl = E_pointLight_addToScene(&engineScene, girlPointLight);
    E_pointLight_setName(s_pl_girl, "girlPointLight");

    E_spotLight GOI_sideLight = E_spotLight_create(
        {-26.62f, 0.69f, 19.11f},
        {-0.61f, 0.31f, -0.73f},
        {0.00f, 0.18f, 1.00f},
        3.83f,
        15.00f, 25.20f,
        5.68f);
    s_sl_GOI_side = E_spotLight_addToScene(&engineScene, GOI_sideLight);
    E_spotLight_setName(s_sl_GOI_side, "GOI_sideLight");

    MUMBAI_RADIO = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/Radio.glb", &engineScene, "MUMBAI_RADIO");
    s_pl_radio = E_pointLight_addToScene(&engineScene, E_pointLight_create({-28.36f, 0.09f, 4.41f}, {1.00f, 1.00f, 1.00f}, 0.65f, 0.67f));
    E_pointLight_setName(s_pl_radio, "radioPointLight");

    {
        E_pointLight pointLight_vadapav = E_pointLight_create(
            {4.27f, 0.34f, 21.68f},
            {1.00f, 0.99f, 0.44f},
            0.04f, 4.00f);
        s_pl_vadapav = E_pointLight_addToScene(&engineScene, pointLight_vadapav);
        E_pointLight_setName(s_pl_vadapav, "pointLight_vadapav");
        s_pl_vadapav->flicker       = 0;
        s_pl_vadapav->flickerSpeed  = 3.000f;
        s_pl_vadapav->flickerAmount = 0.500f;
    }
    E_sceneObject_setPosition(MUMBAI_RADIO, {-28.49f, 0.00f, 4.88f});
    E_sceneObject_setRotation(MUMBAI_RADIO, {0.00f, 156.00f, 0.00f});
    E_sceneObject_setScale   (MUMBAI_RADIO, 0.05f);
    E_animator_setClipByName (MUMBAI_RADIO->animator, "arrow.001|arrow.001Action");

    ESpline_RadioToGirl = E_scene_addSpline(&engineScene, "ESpline_RadioToGirl");
    ESpline_RadioToGirl->numPoints = 4;
    ESpline_RadioToGirl->points[0] = vmath::vec3(-28.469f, 0.055f, 4.799f);
    ESpline_RadioToGirl->points[1] = vmath::vec3(-28.476f, 0.049f, 4.826f);
    ESpline_RadioToGirl->points[2] = vmath::vec3(-29.202f, 0.864f, 5.907f);
    ESpline_RadioToGirl->points[3] = vmath::vec3(-29.433f, 1.056f, 6.110f);

    RopeGirlFocusPt = (E_sceneObject*)malloc(sizeof(E_sceneObject));
    *RopeGirlFocusPt = E_createSceneNode("RopeGirlFocusPt");
    E_sceneObject_setPosition(RopeGirlFocusPt, {-29.25f, 0.90f, 5.94f});
    E_scene_addOwned(&engineScene, RopeGirlFocusPt);

    ESpline_RoundTheGirl = E_scene_addSpline(&engineScene, "ESpline_RoundTheGirl");
    ESpline_RoundTheGirl->numPoints = 4;
    ESpline_RoundTheGirl->points[0] = vmath::vec3(-25.832f, 0.633f, 5.419f);
    ESpline_RoundTheGirl->points[1] = vmath::vec3(-25.916f, 0.633f, 4.999f);
    ESpline_RoundTheGirl->points[2] = vmath::vec3(-26.022f, 0.733f, 4.487f);
    ESpline_RoundTheGirl->points[3] = vmath::vec3(-27.058f, 0.738f, 3.456f);


    // Footpath 1 splines  lanes at x = -3.295, -2.954, -2.614
    static const float FP1_ROT_Y[FP1_LANE_COUNT] = { 0.0f, 180.0f, 0.0f };

    s_fp1Splines[0]            = E_scene_addSpline(&engineScene, "FP1_Spline_0");
    s_fp1Splines[0]->numPoints = 4;
    s_fp1Splines[0]->points[0] = vmath::vec3(-3.295f, 0.000f,  -2.400f);
    s_fp1Splines[0]->points[1] = vmath::vec3(-3.295f, 0.000f,  -7.500f);
    s_fp1Splines[0]->points[2] = vmath::vec3(-3.295f, 0.000f,  51.000f);
    s_fp1Splines[0]->points[3] = vmath::vec3(-3.295f, 0.000f,  52.000f);

    s_fp1Splines[1]            = E_scene_addSpline(&engineScene, "FP1_Spline_1");
    s_fp1Splines[1]->numPoints = 4;
    s_fp1Splines[1]->points[0] = vmath::vec3(-2.954f, 0.000f,  52.000f);
    s_fp1Splines[1]->points[1] = vmath::vec3(-2.954f, 0.000f,  53.000f);
    s_fp1Splines[1]->points[2] = vmath::vec3(-2.954f, 0.000f,  -5.237f);
    s_fp1Splines[1]->points[3] = vmath::vec3(-2.954f, 0.000f,   0.000f);

    s_fp1Splines[2]            = E_scene_addSpline(&engineScene, "FP1_Spline_2");
    s_fp1Splines[2]->numPoints = 4;
    s_fp1Splines[2]->points[0] = vmath::vec3(-2.614f, 0.000f,   0.000f);
    s_fp1Splines[2]->points[1] = vmath::vec3(-2.614f, 0.000f, -17.507f);
    s_fp1Splines[2]->points[2] = vmath::vec3(-2.614f, 0.000f,  51.000f);
    s_fp1Splines[2]->points[3] = vmath::vec3(-2.614f, 0.000f,  52.000f);

    static const char* walkGlbs[9] = {
        "DEMO_ASSETS/Common/Generic human model/Walking/WalkingMan1.glb",
        "DEMO_ASSETS/Common/Generic human model/Walking/Women4Walking.glb",
        "DEMO_ASSETS/Common/Generic human model/WalkingNew/Human1_Walking_Scaled.glb",
        "DEMO_ASSETS/Common/Generic human model/WalkingNew/Human2_Walking_Scaled.glb",
        "DEMO_ASSETS/Common/Generic human model/WalkingNew/Human3_Walking_Scaled.glb",
        "DEMO_ASSETS/Common/Generic human model/WalkingNew/Human4_Walking_Scaled.glb",
        "DEMO_ASSETS/Common/Generic human model/WalkingNew/Human5_Walking_Scaled.glb",
        "DEMO_ASSETS/Common/Generic human model/WalkingNew/Human6_Walking_Scaled.glb",
        "DEMO_ASSETS/Common/Generic human model/WalkingNew/Josh_Walking_Scaled.glb",
    };

    for (int i = 0; i < s_fp1TOTAL; i++)
    {
        int  lane = i / FP1_PER_LANE;
        char name[32];
        sprintf(name, "FP1_Walker_%d", i);
        s_fp1Walkers[i] = E_loadModel(walkGlbs[i % 6], &engineScene, name);
        E_sceneObject_setScale   (s_fp1Walkers[i], 0.01f);
        E_sceneObject_setRotation(s_fp1Walkers[i], vmath::vec3(0.0f, FP1_ROT_Y[lane], 0.0f));
        s_fp1T[i] = (float)(i % FP1_PER_LANE) / FP1_PER_LANE;
        E_sceneObject_setPosition(s_fp1Walkers[i], E_spline_evaluate(s_fp1Splines[lane], s_fp1T[i]));
        if (s_fp1Walkers[i]->animator)
            E_animator_setTime(s_fp1Walkers[i]->animator,
                E_animator_getDuration(s_fp1Walkers[i]->animator) * ((float)i / s_fp1TOTAL));
    }

    s_fp2Spline            = E_scene_addSpline(&engineScene, "FP2_Spline");
    s_fp2Spline->numPoints = 4;
    s_fp2Spline->points[0] = vmath::vec3(3.841f, 0.0f,  0.0f);
    s_fp2Spline->points[1] = vmath::vec3(3.841f, 0.0f,  1.0f);
    s_fp2Spline->points[2] = vmath::vec3(3.841f, 0.0f, 51.0f);
    s_fp2Spline->points[3] = vmath::vec3(3.841f, 0.0f, 52.0f);

    for (int i = 0; i < s_fp2TOTAL; i++)
    {
        char name[32];
        sprintf(name, "FP2_Walker_%d", i);
        s_fp2Walkers[i] = E_loadModel(walkGlbs[i % 2], &engineScene, name);
        E_sceneObject_setScale   (s_fp2Walkers[i], 0.01f);
        s_fp2T[i] = (float)i / s_fp2TOTAL;
        E_sceneObject_setPosition(s_fp2Walkers[i], E_spline_evaluate(s_fp2Spline, s_fp2T[i]));
        if (s_fp2Walkers[i]->animator)
            E_animator_setTime(s_fp2Walkers[i]->animator,
                E_animator_getDuration(s_fp2Walkers[i]->animator) * ((float)i / s_fp2TOTAL));
    }

    s_fpWalkerRdGirlSpline = E_scene_addSpline(&engineScene, "s_fpWalkerRdGirlSpline");
    s_fpWalkerRdGirlSpline->numPoints = 4;
    s_fpWalkerRdGirlSpline->points[0] = vmath::vec3(-44.467f, 0.000f, 4.049f);
    s_fpWalkerRdGirlSpline->points[1] = vmath::vec3(-33.849f, 0.000f, 1.618f);
    s_fpWalkerRdGirlSpline->points[2] = vmath::vec3(-28.380f, 0.000f, 4.373f);
    s_fpWalkerRdGirlSpline->points[3] = vmath::vec3(-39.480f, 0.000f, 4.573f);

    s_fpWalkerRdGirlLookAtSpline = E_scene_addSpline(&engineScene, "s_fpWalkerRdGirl_LOOKAT_SPLI");
    s_fpWalkerRdGirlLookAtSpline->numPoints = 6;
    s_fpWalkerRdGirlLookAtSpline->points[0] = vmath::vec3(-27.184f, 2.650f, 5.171f);
    s_fpWalkerRdGirlLookAtSpline->points[1] = vmath::vec3(-25.884f, 2.650f, 3.871f);
    s_fpWalkerRdGirlLookAtSpline->points[2] = vmath::vec3(-26.712f, 1.158f, -0.332f);
    s_fpWalkerRdGirlLookAtSpline->points[3] = vmath::vec3(-26.941f, 0.786f, 2.564f);
    s_fpWalkerRdGirlLookAtSpline->points[4] = vmath::vec3(-28.476f, 0.049f, 4.826f);
    s_fpWalkerRdGirlLookAtSpline->points[5] = vmath::vec3(-31.676f, 0.049f, 4.826f);

    s_fpWalkerRdGirl = E_loadModel(walkGlbs[0], &engineScene, "s_fpWalkerRdGirl");
    E_sceneObject_setScale   (s_fpWalkerRdGirl, 0.01f);
    E_sceneObject_setPosition(s_fpWalkerRdGirl, s_fpWalkerRdGirlSpline->points[0]);
    E_sceneObject_setActive  (s_fpWalkerRdGirl, 0);

    // Crowd watching the tightrope walker  circle with natural jitter
    static const float GOI_CENTER_X = -29.55f;
    static const float GOI_CENTER_Z =   6.01f;
    static const float GOI_RADIUS   =   2.0f;
    static const float PI           =   3.14159265f;
    static const char* s_goiCrowd_MODELS[] = {
        // "DEMO_ASSETS/Common/Generic human model/Claping/Anthony_Happy_Hand_Gesture_Low_Poly.glb",
        "DEMO_ASSETS/Common/Generic human model/Clapping/Josh_Clapping_Low_Poly.glb",
        // "DEMO_ASSETS/Common/Generic human model/Claping/Tony_Clapping_Low_Poly.glb",
        "DEMO_ASSETS/Common/Generic human model/Talking/TalkingMan1.glb"
    };
    static const int s_goiCrowd_MODEL_COUNT = sizeof(s_goiCrowd_MODELS) / sizeof(s_goiCrowd_MODELS[0]);
    for (int i = 0; i < GOI_TOTAL; i++)
    {
        char name[32];
        sprintf(name, "GOI_Crowd_%d", i);
        float angle  = 2.0f * PI * i / GOI_TOTAL;
        float r      = GOI_RADIUS + sinf((float)i * 7.3f) * 0.3f + cosf((float)i * 3.7f) * 0.2f;
        float ry     = atan2f(-sinf(angle), -cosf(angle)) * (180.0f / PI);
        int modelIdx = i % s_goiCrowd_MODEL_COUNT;
        s_goiCrowd[i] = E_loadModel(s_goiCrowd_MODELS[modelIdx], &engineScene, name);
        if (!s_goiCrowd[i]) continue;
        E_sceneObject_setScale   (s_goiCrowd[i], modelIdx == 1 ? 0.01f : 1.00f);
        E_sceneObject_setRotation(s_goiCrowd[i], vmath::vec3(0.0f, ry, 0.0f));
        E_sceneObject_setPosition(s_goiCrowd[i], vmath::vec3(GOI_CENTER_X + sinf(angle) * r, 0.0f, GOI_CENTER_Z + cosf(angle) * r));
        if (s_goiCrowd[i]->animator)
            E_animator_setTime(s_goiCrowd[i]->animator,
                E_animator_getDuration(s_goiCrowd[i]->animator) * ((float)i / GOI_TOTAL));
    }

    // Big ships
    s_bigShip1[0] = E_loadModel("DEMO_ASSETS/Common/Environmental model/BigShip1.glb", &engineScene, "s_bigShip1_0");
    E_sceneObject_setPosition(s_bigShip1[0], {-11.89f, -1.17f, 95.15f});
    E_sceneObject_setRotation(s_bigShip1[0], {0.00f, 266.00f, 0.00f});
    E_sceneObject_setScale   (s_bigShip1[0], 1.00f);
    E_sceneObject_setCastShadow(s_bigShip1[0], 1);

    s_bigShip1[1] = E_loadModel("DEMO_ASSETS/Common/Environmental model/BigShip1.glb", &engineScene, "s_bigShip1_1");
    E_sceneObject_setPosition(s_bigShip1[1], {0.00f, -1.20f, 79.90f});
    E_sceneObject_setRotation(s_bigShip1[1], {0.00f, 170.00f, 0.00f});
    E_sceneObject_setScale   (s_bigShip1[1], 1.04f);
    E_sceneObject_setCastShadow(s_bigShip1[1], 1);

    s_bigShip2[0] = E_loadModel("DEMO_ASSETS/Common/Environmental model/BigShip2.glb", &engineScene, "s_bigShip2_0");
    E_sceneObject_setPosition(s_bigShip2[0], {-73.40f, -0.40f, 49.70f});
    E_sceneObject_setRotation(s_bigShip2[0], {0.00f, 42.00f, 0.00f});
    E_sceneObject_setScale   (s_bigShip2[0], 1.00f);
    E_sceneObject_setCastShadow(s_bigShip2[0], 1);

    s_bigShip2[1] = E_loadModel("DEMO_ASSETS/Common/Environmental model/BigShip2.glb", &engineScene, "s_bigShip2_1");
    E_sceneObject_setPosition(s_bigShip2[1], {-46.10f, -0.80f, 45.80f});
    E_sceneObject_setRotation(s_bigShip2[1], {0.00f, -153.00f, 0.00f});
    E_sceneObject_setScale   (s_bigShip2[1], 2.09f);
    E_sceneObject_setCastShadow(s_bigShip2[1], 1);

    srand(42);
    for (int i = 0; i < s_boatsB1_COUNT; i++)
    {
        char name[32];
        sprintf(name, "BOAT_B1_%d", i);
        s_boatsB1X[i]     = -25.507f + ((float)rand() / RAND_MAX) * 14.414f;
        s_boatsB1Z[i]     =  10.786f + ((float)rand() / RAND_MAX) * 33.483f;
        s_boatsB1Phase[i] =            ((float)rand() / RAND_MAX) * 6.2832f;
        s_boatsB1[i] = E_loadModel("DEMO_ASSETS/Common/Environmental model/fisherBoat1.glb", &engineScene, name);
        E_sceneObject_setPosition(s_boatsB1[i], vmath::vec3(s_boatsB1X[i], -1.35f, s_boatsB1Z[i]));
        E_sceneObject_setRotation(s_boatsB1[i], vmath::vec3(0.0f, ((float)rand() / RAND_MAX) * 360.0f, 0.0f));
    }

    ESpline_BUS1 = E_scene_addSpline(&engineScene, "ESpline_BUS1");
    ESpline_BUS1->numPoints = 4;
    ESpline_BUS1->points[0] = vmath::vec3(1.751f, 0.000f, -5.546f);
    ESpline_BUS1->points[1] = vmath::vec3(1.751f, 0.000f, -3.916f);
    ESpline_BUS1->points[2] = vmath::vec3(1.751f, 0.000f, 32.130f);
    ESpline_BUS1->points[3] = vmath::vec3(1.751f, 0.000f, 39.613f);

    ESpline_MumbaiTaxi = E_scene_addSpline(&engineScene, "ESpline_MumbaiTaxi");
    ESpline_MumbaiTaxi->numPoints = 4;
    ESpline_MumbaiTaxi->points[0] = vmath::vec3(76.319f, 0.000f, 5.429f);
    ESpline_MumbaiTaxi->points[1] = vmath::vec3(76.319f, 0.000f, 3.283f);
    ESpline_MumbaiTaxi->points[2] = vmath::vec3(76.319f, 0.000f, -1.732f);
    ESpline_MumbaiTaxi->points[3] = vmath::vec3(76.319f, 0.000f, -3.748f);

    PlaneSplineMumbai = E_scene_addSpline(&engineScene, "PlaneSplineMumbai");
    PlaneSplineMumbai->numPoints = 4;
    PlaneSplineMumbai->points[0] = vmath::vec3(-24.597f, 34.0f, -53.860f);
    PlaneSplineMumbai->points[1] = vmath::vec3(-25.039f, 34.0f, -50.224f);
    PlaneSplineMumbai->points[2] = vmath::vec3( 40.000f, 35.0f,  80.000f);
    PlaneSplineMumbai->points[3] = vmath::vec3( 55.000f, 35.0f, 110.000f);

    s_splineBambai = E_scene_addSpline(&engineScene, "s_splineBambai");
    s_splineBambai->numPoints = 4;
    s_splineBambai->points[0] = vmath::vec3(-46.237f, 0.331f, 17.264f);
    s_splineBambai->points[1] = vmath::vec3(-45.437f, 0.331f, 18.741f);
    s_splineBambai->points[2] = vmath::vec3(-39.750f, 0.331f, 29.240f);
    s_splineBambai->points[3] = vmath::vec3(-38.793f, 0.331f, 31.007f);

    s_splineMoviePoster = E_scene_addSpline(&engineScene, "s_splineMoviePoster");
    s_splineMoviePoster->numPoints = 4;
    s_splineMoviePoster->points[0] = vmath::vec3(4.325f, 0.571f, 21.596f);
    s_splineMoviePoster->points[1] = vmath::vec3(3.684f, 0.428f, 20.853f);
    s_splineMoviePoster->points[2] = vmath::vec3(3.712f, 0.807f, 14.221f);
    s_splineMoviePoster->points[3] = vmath::vec3(3.630f, 0.807f, 13.654f);

    s_splineCamPosMoviePoster = E_scene_addSpline(&engineScene, "s_splineCamPosMoviePoster");
    s_splineCamPosMoviePoster->numPoints = 4;
    s_splineCamPosMoviePoster->points[0] = vmath::vec3(-1.433f, 0.701f, 21.225f);
    s_splineCamPosMoviePoster->points[1] = vmath::vec3(-1.435f, 0.701f, 21.179f);
    s_splineCamPosMoviePoster->points[2] = vmath::vec3(-0.965f, 0.681f, 19.006f);
    s_splineCamPosMoviePoster->points[3] = vmath::vec3(-0.915f, 0.679f, 18.779f);

    s_splineCouple = E_scene_addSpline(&engineScene, "s_splineCouple");
    s_splineCouple->numPoints = 6;
    s_splineCouple->points[0] = vmath::vec3(-6.883f, 0.433f, 18.762f);
    s_splineCouple->points[1] = vmath::vec3(-5.556f, 0.433f, 17.978f);
    s_splineCouple->points[2] = vmath::vec3(-4.674f, 0.433f, 18.216f);
    s_splineCouple->points[3] = vmath::vec3(-4.228f, 0.433f, 19.306f);
    s_splineCouple->points[4] = vmath::vec3(-4.630f, 0.433f, 20.567f);
    s_splineCouple->points[5] = vmath::vec3(-5.480f, 0.433f, 20.481f);

    s_splineCSTCamPos = E_scene_addSpline(&engineScene, "s_splineCSTCamPos");
    s_splineCSTCamPos->numPoints = 4;
    s_splineCSTCamPos->points[0] = vmath::vec3(79.508f, 0.244f,  10.732f);
    s_splineCSTCamPos->points[1] = vmath::vec3(79.914f, 0.244f,   9.992f);
    s_splineCSTCamPos->points[2] = vmath::vec3(79.853f, 2.700f, -21.283f);
    s_splineCSTCamPos->points[3] = vmath::vec3(79.594f, 0.583f, -24.429f);

    s_splineCSTCamLookAt = E_scene_addSpline(&engineScene, "s_splineCSTCamLookAt");
    s_splineCSTCamLookAt->numPoints = 4;
    s_splineCSTCamLookAt->points[0] = vmath::vec3(59.346f, 0.893f, -5.712f);
    s_splineCSTCamLookAt->points[1] = vmath::vec3(58.707f, 1.633f, -5.651f);
    s_splineCSTCamLookAt->points[2] = vmath::vec3(58.127f, 9.109f, -5.506f);
    s_splineCSTCamLookAt->points[3] = vmath::vec3(58.044f, 9.863f, -5.507f);

    E_spotLight spotLightCST6 = E_spotLight_create(
        {61.32f, -2.03f, 5.23f},
        {-0.00f, 1.00f, 0.07f},
        {1.00f, 0.85f, 0.00f},
        1.31f,
        22.20f, 44.00f,
        12.85f);
    spotLightCST6.flicker       = 0;
    spotLightCST6.flickerSpeed  = 3.000f;
    spotLightCST6.flickerAmount = 0.500f;
    spotLightCST6.castShadow    = 0;
    s_sl_CST_6 = E_spotLight_addToScene(&engineScene, spotLightCST6);
    E_spotLight_setName(s_sl_CST_6, "spotLightCST6");

    E_spotLight spotLightCST5 = E_spotLight_create(
        {56.27f, -0.10f, -13.44f},
        {-0.33f, 0.94f, -0.00f},
        {1.00f, 0.85f, 0.00f},
        0.82f,
        28.30f, 58.00f,
        13.64f);
    spotLightCST5.flicker       = 0;
    spotLightCST5.flickerSpeed  = 3.000f;
    spotLightCST5.flickerAmount = 0.500f;
    spotLightCST5.castShadow    = 0;
    s_sl_CST_5 = E_spotLight_addToScene(&engineScene, spotLightCST5);
    E_spotLight_setName(s_sl_CST_5, "spotLightCST5");

    E_spotLight spotLightCST4 = E_spotLight_create(
        {59.68f, -1.23f, -5.23f},
        {-0.66f, 1.00f, 0.01f},
        {1.00f, 0.85f, 0.00f},
        0.42f,
        23.90f, 32.00f,
        67.46f);
    spotLightCST4.flicker       = 0;
    spotLightCST4.flickerSpeed  = 3.000f;
    spotLightCST4.flickerAmount = 0.500f;
    spotLightCST4.castShadow    = 0;
    s_sl_CST_4 = E_spotLight_addToScene(&engineScene, spotLightCST4);
    E_spotLight_setName(s_sl_CST_4, "spotLightCST4");

    E_spotLight spotLightCST3 = E_spotLight_create(
        {71.92f, -0.34f, -21.54f},
        {-0.38f, 0.93f, -0.03f},
        {1.00f, 0.85f, 0.00f},
        0.58f,
        16.50f, 68.20f,
        22.88f);
    spotLightCST3.flicker       = 0;
    spotLightCST3.flickerSpeed  = 3.000f;
    spotLightCST3.flickerAmount = 0.500f;
    spotLightCST3.castShadow    = 0;
    s_sl_CST_3 = E_spotLight_addToScene(&engineScene, spotLightCST3);
    E_spotLight_setName(s_sl_CST_3, "spotLightCST3");

    E_spotLight spotLightCST2 = E_spotLight_create(
        {56.27f, -0.10f, 1.16f},
        {-0.33f, 0.94f, -0.00f},
        {1.00f, 0.85f, 0.00f},
        0.58f,
        28.30f, 57.90f,
        13.64f);
    spotLightCST2.flicker       = 0;
    spotLightCST2.flickerSpeed  = 3.000f;
    spotLightCST2.flickerAmount = 0.500f;
    spotLightCST2.castShadow    = 0;
    s_sl_CST_2 = E_spotLight_addToScene(&engineScene, spotLightCST2);
    E_spotLight_setName(s_sl_CST_2, "spotLightCST2");

    E_spotLight spotLightCST1 = E_spotLight_create(
        {60.92f, -0.89f, -18.68f},
        {0.02f, 0.42f, 0.24f},
        {1.00f, 0.85f, 0.00f},
        1.23f,
        22.20f, 36.40f,
        12.85f);
    spotLightCST1.flicker       = 0;
    spotLightCST1.flickerSpeed  = 3.000f;
    spotLightCST1.flickerAmount = 0.500f;
    spotLightCST1.castShadow    = 0;
    s_sl_CST_1 = E_spotLight_addToScene(&engineScene, spotLightCST1);
    E_spotLight_setName(s_sl_CST_1, "spotLightCST1");

    E_pointLight pointLightCSTCenter = E_pointLight_create(
        {62.34f, 2.73f, -3.35f},
        {1.00f, 1.00f, 1.00f},
        2.53f, 4.11f);
    s_pl_CSTCenter = E_pointLight_addToScene(&engineScene, pointLightCSTCenter);
    E_pointLight_setName(s_pl_CSTCenter, "pointLightCSTCenter");
    s_pl_CSTCenter->flicker       = 0;
    s_pl_CSTCenter->flickerSpeed  = 3.000f;
    s_pl_CSTCenter->flickerAmount = 0.500f;

    E_pointLight pointLightCSTCompound = E_pointLight_create(
        {69.57f, 0.28f, -4.72f},
        {1.00f, 1.00f, 1.00f},
        0.18f, 10.00f);
    s_pl_CSTCompound = E_pointLight_addToScene(&engineScene, pointLightCSTCompound);
    E_pointLight_setName(s_pl_CSTCompound, "pointLightCSTCompound");
    s_pl_CSTCompound->flicker       = 0;
    s_pl_CSTCompound->flickerSpeed  = 3.000f;
    s_pl_CSTCompound->flickerAmount = 0.500f;

    MUMBAI_PLANE = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/low_poly_cargo_plane.glb", &engineScene, "MUMBAI_PLANE");
    MUMBAI_BF    = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/bf.glb", &engineScene, "MUMBAI_BF");
    MUMBAI_GF    = E_loadModel("DEMO_ASSETS/01-MumbaiDarshan/gf.glb", &engineScene, "MUMBAI_GF");
    E_sceneObject_setPosition    (MUMBAI_GF,  { -5.39f, -0.03f, 19.45f});
    E_sceneObject_setRotation    (MUMBAI_GF,  {  0.00f, -90.00f, 0.00f});
    E_sceneObject_setScale       (MUMBAI_GF,  1.00f);
    E_sceneObject_setCastShadowAll(MUMBAI_GF, 1);
    if (MUMBAI_GF->animator) E_animator_setClipByName(MUMBAI_GF->animator, "Animation");
    E_sceneObject_setPosition    (MUMBAI_BF,  { -5.46f, -0.01f, 19.21f});
    E_sceneObject_setRotation    (MUMBAI_BF,  {  0.00f, -90.00f, 0.00f});
    E_sceneObject_setScale       (MUMBAI_BF,  1.00f);
    E_sceneObject_setCastShadowAll(MUMBAI_BF, 1);
    if (MUMBAI_BF->animator) E_animator_setClipByName(MUMBAI_BF->animator, "Animation");
    E_sceneObject_setScale(MUMBAI_PLANE, 0.05f);

    s_planeRightWingOffset = vmath::vec3(-2.54f, -0.55f, -2.40f);
    s_planeLeftWingOffset  = vmath::vec3( 2.54f, -0.55f, -2.40f);
    vmath::vec3 planeRightWingLightPos = MUMBAI_PLANE->position + MumbaiDarshan_rotatePlaneOffset(s_planeRightWingOffset, MUMBAI_PLANE->rotation);
    vmath::vec3 planeLeftWingLightPos  = MUMBAI_PLANE->position + MumbaiDarshan_rotatePlaneOffset(s_planeLeftWingOffset,  MUMBAI_PLANE->rotation);

    E_pointLight pointLightPlaneRightWing = E_pointLight_create(
        planeRightWingLightPos,
        {1.00f, 0.00f, 0.00f},
        20.00f, 1.58f);
    s_pl_planeRightWing = E_pointLight_addToScene(&engineScene, pointLightPlaneRightWing);
    E_pointLight_setName(s_pl_planeRightWing, "pointLightPlaneRightWing");
    s_pl_planeRightWing->flicker       = 1;
    s_pl_planeRightWing->flickerSpeed  = 1.762f;
    s_pl_planeRightWing->flickerAmount = 1.000f;

    E_pointLight pointLightPlaneLeftWing = E_pointLight_create(
        planeLeftWingLightPos,
        {0.00f, 1.00f, 0.00f},
        20.00f, 0.50f);
    s_pl_planeLeftWing = E_pointLight_addToScene(&engineScene, pointLightPlaneLeftWing);
    E_pointLight_setName(s_pl_planeLeftWing, "pointLightPlaneLeftWing");
    s_pl_planeLeftWing->flicker       = 1;
    s_pl_planeLeftWing->flickerSpeed  = 0.768f;
    s_pl_planeLeftWing->flickerAmount = 1.000f;
    MumbaiDarshan_updatePlaneWingLights(MUMBAI_PLANE->position, MUMBAI_PLANE->rotation);
    {
        E_pointLight pl = E_pointLight_create({-5.23f, 0.89f, 18.89f}, {1.00f, 1.00f, 0.21f}, 1.37f, 0.80f);
        pl.flicker = 1; pl.flickerSpeed = 1.097f; pl.flickerAmount = 0.500f;
        s_pl_Couple = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Couple, "pointLight_Couple");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {0.32f, 0.36f, 0.80f},
            {-0.02f, -0.12f, 0.99f},
            {1.00f, 1.00f, 0.36f},
            5.0f,
            1.6f, 15.0f,
            4.0f);
        sl.castShadow = 0;
        s_sl_busHeadlamp_left = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_busHeadlamp_left, "busHeadlamp_left");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-0.38f, 0.36f, 0.80f},
            {-0.02f, -0.12f, 0.99f},
            {1.00f, 1.00f, 0.36f},
            5.0f,
            1.6f, 15.0f,
            4.0f);
        sl.castShadow = 0;
        s_sl_busHeadlamp_right = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_busHeadlamp_right, "busHeadlamp_right");
    }
    {
        E_pointLight pl = E_pointLight_create(
            {0.03f, 0.61f, -0.18f},
            {1.00f, 1.00f, 1.00f},
            0.14f, 0.83f);
        s_pl_insideMovingBus = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_insideMovingBus, "pointLight_insideMovingBus");
        s_pl_insideMovingBus->flicker       = 0;
        s_pl_insideMovingBus->flickerSpeed  = 3.000f;
        s_pl_insideMovingBus->flickerAmount = 0.500f;
    }

    {
        E_spotLight posterSpotlight_1 = E_spotLight_create(
            {5.23f, 2.69f, 14.70f}, {0.00f, -1.00f, 0.00f},
            {1.00f, 1.00f, 0.48f}, 1.32f, 28.60f, 32.70f, 3.58f);
        posterSpotlight_1.flicker       = 1;
        posterSpotlight_1.flickerSpeed  = 8.0f;
        posterSpotlight_1.flickerAmount = 0.18f;
        s_sl_poster_1 = E_spotLight_addToScene(&engineScene, posterSpotlight_1);
        E_spotLight_setName(s_sl_poster_1, "posterSpotlight_1");
    }
    {
        E_spotLight posterSpotlight_2 = E_spotLight_create(
            {5.23f, 2.79f, 16.60f}, {0.00f, -1.00f, 0.00f},
            {1.00f, 1.00f, 0.48f}, 1.55f, 27.30f, 30.70f, 3.58f);
        posterSpotlight_2.flicker       = 1;
        posterSpotlight_2.flickerSpeed  = 9.5f;
        posterSpotlight_2.flickerAmount = 0.15f;
        s_sl_poster_2 = E_spotLight_addToScene(&engineScene, posterSpotlight_2);
        E_spotLight_setName(s_sl_poster_2, "posterSpotlight_2");
    }
    {
        E_spotLight posterSpotlight_3 = E_spotLight_create(
            {5.23f, 2.79f, 18.50f}, {0.00f, -1.00f, 0.00f},
            {1.00f, 1.00f, 0.48f}, 1.55f, 27.30f, 30.70f, 3.58f);
        posterSpotlight_3.flicker       = 1;
        posterSpotlight_3.flickerSpeed  = 7.2f;
        posterSpotlight_3.flickerAmount = 0.20f;
        s_sl_poster_3 = E_spotLight_addToScene(&engineScene, posterSpotlight_3);
        E_spotLight_setName(s_sl_poster_3, "posterSpotlight_3");
    }
    {
        E_spotLight posterSpotlight_4 = E_spotLight_create(
            {5.23f, 2.79f, 20.30f}, {0.00f, -1.00f, 0.00f},
            {1.00f, 1.00f, 0.48f}, 1.55f, 27.30f, 30.70f, 3.58f);
        posterSpotlight_4.flicker       = 1;
        posterSpotlight_4.flickerSpeed  = 11.0f;
        posterSpotlight_4.flickerAmount = 0.16f;
        s_sl_poster_4 = E_spotLight_addToScene(&engineScene, posterSpotlight_4);
        E_spotLight_setName(s_sl_poster_4, "posterSpotlight_4");
    }

    // Street lights - footpath lane
    E_spotLight streetlightFootpath_1 = E_spotLight_create(
        {-2.41f, 1.13f, 44.72f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightFootpath_1.flicker = 1; streetlightFootpath_1.flickerSpeed = 9.1f;  streetlightFootpath_1.flickerAmount = 0.14f;
    s_sl_fp_1 = E_spotLight_addToScene(&engineScene, streetlightFootpath_1);
    E_spotLight_setName(s_sl_fp_1, "streetlightFootpath_1");

    E_spotLight streetlightFootpath_2 = E_spotLight_create(
        {-2.61f, 1.21f, 37.42f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightFootpath_2.flicker = 1; streetlightFootpath_2.flickerSpeed = 11.7f; streetlightFootpath_2.flickerAmount = 0.12f;
    s_sl_fp_2 = E_spotLight_addToScene(&engineScene, streetlightFootpath_2);
    E_spotLight_setName(s_sl_fp_2, "streetlightFootpath_2");

    E_spotLight streetlightFootpath_3 = E_spotLight_create(
        {-2.49f, 1.22f, 30.09f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightFootpath_3.flicker = 1; streetlightFootpath_3.flickerSpeed = 8.3f;  streetlightFootpath_3.flickerAmount = 0.17f;
    s_sl_fp_3 = E_spotLight_addToScene(&engineScene, streetlightFootpath_3);
    E_spotLight_setName(s_sl_fp_3, "streetlightFootpath_3");

    E_spotLight streetlightFootpath_4 = E_spotLight_create(
        {-2.41f, 1.24f, 22.80f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightFootpath_4.flicker = 1; streetlightFootpath_4.flickerSpeed = 13.2f; streetlightFootpath_4.flickerAmount = 0.13f;
    s_sl_fp_4 = E_spotLight_addToScene(&engineScene, streetlightFootpath_4);
    E_spotLight_setName(s_sl_fp_4, "streetlightFootpath_4");

    E_spotLight streetlightFootpath_5 = E_spotLight_create(
        {-2.46f, 1.22f, 15.53f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightFootpath_5.flicker = 1; streetlightFootpath_5.flickerSpeed = 10.4f; streetlightFootpath_5.flickerAmount = 0.16f;
    s_sl_fp_5 = E_spotLight_addToScene(&engineScene, streetlightFootpath_5);
    E_spotLight_setName(s_sl_fp_5, "streetlightFootpath_5");

    E_spotLight streetlightFootpath_6 = E_spotLight_create(
        {-2.51f, 1.21f, 8.20f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightFootpath_6.flicker = 1; streetlightFootpath_6.flickerSpeed = 7.8f;  streetlightFootpath_6.flickerAmount = 0.15f;
    s_sl_fp_6 = E_spotLight_addToScene(&engineScene, streetlightFootpath_6);
    E_spotLight_setName(s_sl_fp_6, "streetlightFootpath_6");

    // Street lights - road middle lane (x = 0.840)
    E_spotLight streetlightRoadMiddle_1 = E_spotLight_create(
        {0.840f, 1.13f, 44.72f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightRoadMiddle_1.flicker = 1; streetlightRoadMiddle_1.flickerSpeed = 12.5f; streetlightRoadMiddle_1.flickerAmount = 0.13f;
    s_sl_road_1 = E_spotLight_addToScene(&engineScene, streetlightRoadMiddle_1);
    E_spotLight_setName(s_sl_road_1, "streetlightRoadMiddle_1");

    E_spotLight streetlightRoadMiddle_2 = E_spotLight_create(
        {0.840f, 1.21f, 37.42f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightRoadMiddle_2.flicker = 1; streetlightRoadMiddle_2.flickerSpeed = 8.9f;  streetlightRoadMiddle_2.flickerAmount = 0.15f;
    s_sl_road_2 = E_spotLight_addToScene(&engineScene, streetlightRoadMiddle_2);
    E_spotLight_setName(s_sl_road_2, "streetlightRoadMiddle_2");

    E_spotLight streetlightRoadMiddle_3 = E_spotLight_create(
        {0.840f, 1.22f, 30.09f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightRoadMiddle_3.flicker = 1; streetlightRoadMiddle_3.flickerSpeed = 14.1f; streetlightRoadMiddle_3.flickerAmount = 0.12f;
    s_sl_road_3 = E_spotLight_addToScene(&engineScene, streetlightRoadMiddle_3);
    E_spotLight_setName(s_sl_road_3, "streetlightRoadMiddle_3");

    E_spotLight streetlightRoadMiddle_4 = E_spotLight_create(
        {0.840f, 1.24f, 22.80f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightRoadMiddle_4.flicker = 1; streetlightRoadMiddle_4.flickerSpeed = 10.0f; streetlightRoadMiddle_4.flickerAmount = 0.14f;
    s_sl_road_4 = E_spotLight_addToScene(&engineScene, streetlightRoadMiddle_4);
    E_spotLight_setName(s_sl_road_4, "streetlightRoadMiddle_4");

    E_spotLight streetlightRoadMiddle_5 = E_spotLight_create(
        {0.840f, 1.22f, 15.53f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightRoadMiddle_5.flicker = 1; streetlightRoadMiddle_5.flickerSpeed = 7.5f;  streetlightRoadMiddle_5.flickerAmount = 0.18f;
    s_sl_road_5 = E_spotLight_addToScene(&engineScene, streetlightRoadMiddle_5);
    E_spotLight_setName(s_sl_road_5, "streetlightRoadMiddle_5");

    E_spotLight streetlightRoadMiddle_6 = E_spotLight_create(
        {0.840f, 1.21f, 8.20f}, {-0.06f, -1.00f, -0.08f},
        {1.00f, 1.00f, 1.00f}, 0.54f, 47.40f, 60.70f, 6.86f);
    streetlightRoadMiddle_6.flicker = 1; streetlightRoadMiddle_6.flickerSpeed = 11.3f; streetlightRoadMiddle_6.flickerAmount = 0.13f;
    s_sl_road_6 = E_spotLight_addToScene(&engineScene, streetlightRoadMiddle_6);
    E_spotLight_setName(s_sl_road_6, "streetlightRoadMiddle_6");

    {
        E_spotLight spotLight_nearGirl = E_spotLight_create(
            {-29.23f, 0.24f, 3.21f},
            {-0.18f,  0.10f, 0.98f},
            {0.20f,   0.20f, 0.20f},
            2.01f,
            15.00f, 53.10f,
            11.70f);
        spotLight_nearGirl.flicker       = 1;
        spotLight_nearGirl.flickerSpeed  = 0.876f;
        spotLight_nearGirl.flickerAmount = 0.500f;
        spotLight_nearGirl.castShadow    = 1;
        s_sl_nearGirl = E_spotLight_addToScene(&engineScene, spotLight_nearGirl);
        E_spotLight_setName(s_sl_nearGirl, "spotLight_nearGirl");
    }

    {
        E_spotLight spotLight_nearCouple = E_spotLight_create(
            {-6.00f, 1.17f, 17.63f},
            {0.24f, -0.40f,  0.88f},
            {0.47f,  0.67f,  1.00f},
            0.45f,
            15.00f, 20.00f,
            37.88f);
        spotLight_nearCouple.flicker       = 0;
        spotLight_nearCouple.flickerSpeed  = 3.000f;
        spotLight_nearCouple.flickerAmount = 0.500f;
        spotLight_nearCouple.castShadow    = 1;
        s_sl_nearCouple = E_spotLight_addToScene(&engineScene, spotLight_nearCouple);
        E_spotLight_setName(s_sl_nearCouple, "spotLight_nearCouple");
    }

    {
        E_spotLight spotLight_onBus = E_spotLight_create(
            {-0.38f, 2.34f,  20.44f},
            { 0.46f, -0.29f, -0.84f},
            { 1.00f,  1.00f,  1.00f},
            0.44f,
            15.00f, 34.70f,
            51.53f);
        spotLight_onBus.flicker       = 1;
        spotLight_onBus.flickerSpeed  = 0.433f;
        spotLight_onBus.flickerAmount = 0.500f;
        spotLight_onBus.castShadow    = 1;
        s_sl_onBus = E_spotLight_addToScene(&engineScene, spotLight_onBus);
        E_spotLight_setName(s_sl_onBus, "spotLight_onBus");
    }

    {
        E_spotLight spotLight_nearWalkingMan = E_spotLight_create(
            {-1.86f, 1.28f, 13.67f},
            {-0.17f, -0.24f, 0.96f},
            { 1.00f,  1.00f, 0.75f},
            0.23f,
            15.00f, 20.00f,
            33.15f);
        spotLight_nearWalkingMan.flicker       = 1;
        spotLight_nearWalkingMan.flickerSpeed  = 1.320f;
        spotLight_nearWalkingMan.flickerAmount = 0.500f;
        spotLight_nearWalkingMan.castShadow    = 1;
        s_sl_nearWalkingMan = E_spotLight_addToScene(&engineScene, spotLight_nearWalkingMan);
        E_spotLight_setName(s_sl_nearWalkingMan, "spotLight_nearWalkingMan");
    }
}

void MumbaiDarshan_begin()
{
    static int s_begun = 0;
    if (s_begun) return;
    s_begun = 1;

    E_scene_setShadowArea(&engineScene, 0.0f, 0.0f, 0.0f, 100.0f);

    E_directionalLight_setDirection(&engineScene.directionalLight, {0.50f, -0.48f, -1.00f});
    E_directionalLight_setColor(&engineScene.directionalLight, {1.00f, 0.66f, 0.45f});
    E_directionalLight_setIntensity(&engineScene.directionalLight, 0.53f);

    E_ambientLight_setColor(&engineScene.ambientLight, {0.46f, 0.73f, 1.00f});
    E_ambientLight_setIntensity(&engineScene.ambientLight, 0.15f);

    E_postProcess_setFXAA      (1);
    E_postProcess_setSaturation(1.02f);
    E_postProcess_setBlur      (0.00f);
    E_postProcess_setVignette  (0.97f, 1.00f);
    E_postProcess_setDOF       (1, 10.00f, 2.00f, 3.00f);
    E_postProcess_setGodRays(1, 0.500f, 0.500f, 167, 0.239f, 0.800f, 0.248f, 0.486f);
    E_postProcess_setBloom     (1, 1.00f, 3.00f);

    E_fog_setEnabled(&engineScene.fog, 0);
    E_fog_setColor  (&engineScene.fog, {0.07f, 0.07f, 0.07f});
    E_fog_setDensity(&engineScene.fog, 0.008f);
    E_fog_setType   (&engineScene.fog, 0);

    MumbaiDarshan_activate();

    E_camera_setPosition (-28.643f, 0.165f, 4.415f);
    E_camera_setProjection(45.00f, 0.10f, 1000.00f);

    E_sceneObject_setPosition(MUMBAI_SKYSPHERE, {0.00f, -24.40f, 0.00f});
    E_sceneObject_setRotation(MUMBAI_SKYSPHERE, {0.00f, -152.00f, 0.00f});
    E_sceneObject_setScale   (MUMBAI_SKYSPHERE, 100.00f);

#if DEMO_RECORD == 1
    // Lights off
    for (int i = 0; i < engineScene.spotLightCount;  i++) engineScene.spotLights[i].active    = 0;
    for (int i = 0; i < engineScene.pointLightCount; i++) engineScene.pointLights[i].active   = 0;
    E_directionalLight_setIntensity(&engineScene.directionalLight, 0.0f);

    // Shadows off
    E_scene_setShadowEnabled(&engineScene, 0);
    engineScene.spotShadowEnabled = 0;

    // Animations paused
    if (MUMBAI_TIGHTROPE_GIRL->animator) E_animator_pause(MUMBAI_TIGHTROPE_GIRL->animator);
    if (MUMBAI_RADIO->animator)          E_animator_pause(MUMBAI_RADIO->animator);
    for (int i = 0; i < s_fp1TOTAL;  i++) if (s_fp1Walkers[i]->animator)  E_animator_pause(s_fp1Walkers[i]->animator);
    for (int i = 0; i < s_fp2TOTAL;  i++) if (s_fp2Walkers[i]->animator)  E_animator_pause(s_fp2Walkers[i]->animator);
    if (s_fpWalkerRdGirl->animator)   E_animator_pause(s_fpWalkerRdGirl->animator);
    for (int i = 0; i < GOI_TOTAL;  i++) if (s_goiCrowd[i] && s_goiCrowd[i]->animator) E_animator_pause(s_goiCrowd[i]->animator);
#endif
}

void MumbaiDarshan_activate()
{
    E_sceneObject_setActive(MUMBAI_SKYSPHERE,      1);
    E_sceneObject_setActive(MUMBAI_OCEAN_FLOOR,    1);
    E_sceneObject_setActive(MUMBAI_OCEAN,          1);
    E_sceneObject_setActive(MUMBAI_SCENE,          1);
    if (MUMBAI_CST_EMISSIVE)        E_sceneObject_setActive(MUMBAI_CST_EMISSIVE,        1);
    E_sceneObject_setActive(MUMBAI_TAJ,            1);
    E_sceneObject_setActive(MUMBAI_GOI,            1);
    E_sceneObject_setActive(MUMBAI_DARSHAN_BUS,    1);
    E_sceneObject_setActive(MUMBAI_TAXI,           1);
    E_sceneObject_setActive(MUMBAI_TIGHTROPE,      1);
    E_sceneObject_setActive(MUMBAI_TIGHTROPE_GIRL, 1);
    E_sceneObject_setActive(MUMBAI_RADIO,          1);
    E_sceneObject_setActive(MUMBAI_PLANE,          0); // shown only during 49-53s
    if (MUMBAI_BF) E_sceneObject_setActive(MUMBAI_BF, 1);
    if (MUMBAI_GF) E_sceneObject_setActive(MUMBAI_GF, 1);
    E_sceneObject_setActive(s_fpWalkerRdGirl,   1);

    // if (s_emitterTajFire1)      s_emitterTajFire1->active      = 1;
    // if (s_emitterTajSmoke1)     s_emitterTajSmoke1->active     = 1;
    // if (s_plTajFire) s_plTajFire->active  = 1;

    if (s_pl_girl)       E_pointLight_activate(s_pl_girl);
    if (s_pl_radio)      E_pointLight_activate(s_pl_radio);
    if (s_pl_vadapav)    E_pointLight_activate(s_pl_vadapav);
    if (s_pl_taxi)       E_pointLight_activate(s_pl_taxi);
    if (s_pl_planeRightWing) E_pointLight_activate(s_pl_planeRightWing);
    if (s_pl_planeLeftWing)  E_pointLight_activate(s_pl_planeLeftWing);
    if (s_pl_CSTCenter)       E_pointLight_activate(s_pl_CSTCenter);
    if (s_pl_CSTCompound)     E_pointLight_activate(s_pl_CSTCompound);
    if (s_sl_GOI_white)  E_spotLight_activate(s_sl_GOI_white);
    if (s_sl_GOI_orange) E_spotLight_activate(s_sl_GOI_orange);
    if (s_sl_GOI_green)  E_spotLight_activate(s_sl_GOI_green);
    if (s_sl_Taj_1)      E_spotLight_activate(s_sl_Taj_1);
    if (s_sl_Taj_2)      E_spotLight_activate(s_sl_Taj_2);
    if (s_sl_Taj_3)      E_spotLight_activate(s_sl_Taj_3);
    if (s_sl_Taj_4)      E_spotLight_activate(s_sl_Taj_4);
    if (s_sl_GOI_side)   E_spotLight_activate(s_sl_GOI_side);
    if (s_sl_poster_1)   E_spotLight_activate(s_sl_poster_1);
    if (s_sl_poster_2)   E_spotLight_activate(s_sl_poster_2);
    if (s_sl_poster_3)   E_spotLight_activate(s_sl_poster_3);
    if (s_sl_poster_4)   E_spotLight_activate(s_sl_poster_4);
    if (s_sl_fp_1)       E_spotLight_activate(s_sl_fp_1);
    if (s_sl_fp_2)       E_spotLight_activate(s_sl_fp_2);
    if (s_sl_fp_3)       E_spotLight_activate(s_sl_fp_3);
    if (s_sl_fp_4)       E_spotLight_activate(s_sl_fp_4);
    if (s_sl_fp_5)       E_spotLight_activate(s_sl_fp_5);
    if (s_sl_fp_6)       E_spotLight_activate(s_sl_fp_6);
    if (s_sl_road_1)     E_spotLight_activate(s_sl_road_1);
    if (s_sl_road_2)     E_spotLight_activate(s_sl_road_2);
    if (s_sl_road_3)     E_spotLight_activate(s_sl_road_3);
    if (s_sl_road_4)     E_spotLight_activate(s_sl_road_4);
    if (s_sl_road_5)          E_spotLight_activate(s_sl_road_5);
    if (s_sl_road_6)          E_spotLight_activate(s_sl_road_6);
    if (s_sl_nearGirl)        E_spotLight_activate(s_sl_nearGirl);
    if (s_sl_nearCouple)      E_spotLight_activate(s_sl_nearCouple);
    if (s_sl_onBus)           E_spotLight_activate(s_sl_onBus);
    if (s_sl_busHeadlamp_left)  E_spotLight_activate(s_sl_busHeadlamp_left);
    if (s_sl_busHeadlamp_right) E_spotLight_activate(s_sl_busHeadlamp_right);
    if (s_pl_insideMovingBus)   E_pointLight_activate(s_pl_insideMovingBus);
    if (s_sl_nearWalkingMan)  E_spotLight_activate(s_sl_nearWalkingMan);
    if (s_sl_CST_1)           E_spotLight_activate(s_sl_CST_1);
    if (s_sl_CST_2)           E_spotLight_activate(s_sl_CST_2);
    if (s_sl_CST_3)           E_spotLight_activate(s_sl_CST_3);
    if (s_sl_CST_4)           E_spotLight_activate(s_sl_CST_4);
    if (s_sl_CST_5)           E_spotLight_activate(s_sl_CST_5);
    if (s_sl_CST_6)           E_spotLight_activate(s_sl_CST_6);

    for (int i = 0; i < s_fp1TOTAL;      i++) E_sceneObject_setActive(s_fp1Walkers[i], 1);
    for (int i = 0; i < s_fp2TOTAL;      i++) E_sceneObject_setActive(s_fp2Walkers[i], 1);
    for (int i = 0; i < GOI_TOTAL;      i++) E_sceneObject_setActive(s_goiCrowd[i],   1);
    for (int i = 0; i < s_boatsB1_COUNT;  i++) E_sceneObject_setActive(s_boatsB1[i],  1);
    for (int i = 0; i < s_bigShip1_COUNT;  i++) E_sceneObject_setActive(s_bigShip1[i],  1);
    for (int i = 0; i < s_bigShip2_COUNT;  i++) E_sceneObject_setActive(s_bigShip2[i],  1);
}

void MumbaiDarshan_deactivate()
{
    if (MUMBAI_SKYSPHERE)      E_sceneObject_setActive(MUMBAI_SKYSPHERE,      0);
    if (MUMBAI_OCEAN_FLOOR)    E_sceneObject_setActive(MUMBAI_OCEAN_FLOOR,    0);
    if (MUMBAI_OCEAN)          E_sceneObject_setActive(MUMBAI_OCEAN,          0);
    if (MUMBAI_SCENE)          E_sceneObject_setActive(MUMBAI_SCENE,          0);
    if (MUMBAI_CST_EMISSIVE)   E_sceneObject_setActive(MUMBAI_CST_EMISSIVE,   0);
    if (MUMBAI_TAJ)            E_sceneObject_setActive(MUMBAI_TAJ,            0);
    if (MUMBAI_GOI)            E_sceneObject_setActive(MUMBAI_GOI,            0);
    if (MUMBAI_DARSHAN_BUS)    E_sceneObject_setActive(MUMBAI_DARSHAN_BUS,    0);
    if (MUMBAI_TAXI)           E_sceneObject_setActive(MUMBAI_TAXI,           0);
    if (MUMBAI_TIGHTROPE)      E_sceneObject_setActive(MUMBAI_TIGHTROPE,      0);
    if (MUMBAI_TIGHTROPE_GIRL) E_sceneObject_setActive(MUMBAI_TIGHTROPE_GIRL, 0);
    if (MUMBAI_RADIO)          E_sceneObject_setActive(MUMBAI_RADIO,          0);
    if (MUMBAI_PLANE)          E_sceneObject_setActive(MUMBAI_PLANE,          0);
    if (s_fpWalkerRdGirl)      E_sceneObject_setActive(s_fpWalkerRdGirl,      0);
    if (MUMBAI_BF)             E_sceneObject_setActive(MUMBAI_BF,             0);
    if (MUMBAI_GF)             E_sceneObject_setActive(MUMBAI_GF,             0);

    // if (s_emitterTajFire1)      s_emitterTajFire1->active      = 0;
    // if (s_emitterTajSmoke1)     s_emitterTajSmoke1->active     = 0;
    // if (s_plTajFire) s_plTajFire->active  = 0;

    if (s_pl_girl)       E_pointLight_deactivate(s_pl_girl);
    if (s_pl_radio)      E_pointLight_deactivate(s_pl_radio);
    if (s_pl_vadapav)    E_pointLight_deactivate(s_pl_vadapav);
    if (s_pl_taxi)       E_pointLight_deactivate(s_pl_taxi);
    if (s_pl_planeRightWing) E_pointLight_deactivate(s_pl_planeRightWing);
    if (s_pl_planeLeftWing)  E_pointLight_deactivate(s_pl_planeLeftWing);
    if (s_pl_CSTCenter)       E_pointLight_deactivate(s_pl_CSTCenter);
    if (s_pl_CSTCompound)     E_pointLight_deactivate(s_pl_CSTCompound);
    if (s_sl_GOI_white)  E_spotLight_deactivate(s_sl_GOI_white);
    if (s_sl_GOI_orange) E_spotLight_deactivate(s_sl_GOI_orange);
    if (s_sl_GOI_green)  E_spotLight_deactivate(s_sl_GOI_green);
    if (s_sl_Taj_1)      E_spotLight_deactivate(s_sl_Taj_1);
    if (s_sl_Taj_2)      E_spotLight_deactivate(s_sl_Taj_2);
    if (s_sl_Taj_3)      E_spotLight_deactivate(s_sl_Taj_3);
    if (s_sl_Taj_4)      E_spotLight_deactivate(s_sl_Taj_4);
    if (s_sl_GOI_side)   E_spotLight_deactivate(s_sl_GOI_side);
    if (s_sl_poster_1)   E_spotLight_deactivate(s_sl_poster_1);
    if (s_sl_poster_2)   E_spotLight_deactivate(s_sl_poster_2);
    if (s_sl_poster_3)   E_spotLight_deactivate(s_sl_poster_3);
    if (s_sl_poster_4)   E_spotLight_deactivate(s_sl_poster_4);
    if (s_sl_fp_1)       E_spotLight_deactivate(s_sl_fp_1);
    if (s_sl_fp_2)       E_spotLight_deactivate(s_sl_fp_2);
    if (s_sl_fp_3)       E_spotLight_deactivate(s_sl_fp_3);
    if (s_sl_fp_4)       E_spotLight_deactivate(s_sl_fp_4);
    if (s_sl_fp_5)       E_spotLight_deactivate(s_sl_fp_5);
    if (s_sl_fp_6)       E_spotLight_deactivate(s_sl_fp_6);
    if (s_sl_road_1)     E_spotLight_deactivate(s_sl_road_1);
    if (s_sl_road_2)     E_spotLight_deactivate(s_sl_road_2);
    if (s_sl_road_3)     E_spotLight_deactivate(s_sl_road_3);
    if (s_sl_road_4)     E_spotLight_deactivate(s_sl_road_4);
    if (s_sl_road_5)          E_spotLight_deactivate(s_sl_road_5);
    if (s_sl_road_6)          E_spotLight_deactivate(s_sl_road_6);
    if (s_sl_nearGirl)        E_spotLight_deactivate(s_sl_nearGirl);
    if (s_sl_nearCouple)      E_spotLight_deactivate(s_sl_nearCouple);
    if (s_sl_onBus)           E_spotLight_deactivate(s_sl_onBus);
    if (s_sl_busHeadlamp_left)  E_spotLight_deactivate(s_sl_busHeadlamp_left);
    if (s_sl_busHeadlamp_right) E_spotLight_deactivate(s_sl_busHeadlamp_right);
    if (s_pl_insideMovingBus)   E_pointLight_deactivate(s_pl_insideMovingBus);
    if (s_sl_nearWalkingMan)  E_spotLight_deactivate(s_sl_nearWalkingMan);
    if (s_sl_CST_1)           E_spotLight_deactivate(s_sl_CST_1);
    if (s_sl_CST_2)           E_spotLight_deactivate(s_sl_CST_2);
    if (s_sl_CST_3)           E_spotLight_deactivate(s_sl_CST_3);
    if (s_sl_CST_4)           E_spotLight_deactivate(s_sl_CST_4);
    if (s_sl_CST_5)           E_spotLight_deactivate(s_sl_CST_5);
    if (s_sl_CST_6)           E_spotLight_deactivate(s_sl_CST_6);

    for (int i = 0; i < s_fp1TOTAL;      i++) E_sceneObject_setActive(s_fp1Walkers[i], 0);
    for (int i = 0; i < s_fp2TOTAL;      i++) E_sceneObject_setActive(s_fp2Walkers[i], 0);
    for (int i = 0; i < GOI_TOTAL;      i++) E_sceneObject_setActive(s_goiCrowd[i],   0);
    for (int i = 0; i < s_boatsB1_COUNT;  i++) E_sceneObject_setActive(s_boatsB1[i],  0);
    for (int i = 0; i < s_bigShip1_COUNT;  i++) E_sceneObject_setActive(s_bigShip1[i],  0);
    for (int i = 0; i < s_bigShip2_COUNT;  i++) E_sceneObject_setActive(s_bigShip2[i],  0);
}

void MumbaiDarshan_updateWorld()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    if (now > 76.2f && now <= 80.2f)
    {
        vmath::vec3 planePos = E_spline_getPositionBetweenTime(PlaneSplineMumbai, 76.2f, 80.2f, 0.6f);
        vmath::vec3 planeRot = E_spline_getRotationBetweenTime(PlaneSplineMumbai, 76.2f, 80.2f, 0.6f);
        E_sceneObject_setActive  (MUMBAI_PLANE, 1);
        E_sceneObject_setPosition(MUMBAI_PLANE, planePos);
        E_sceneObject_setRotation(MUMBAI_PLANE, planeRot);
        MumbaiDarshan_updatePlaneWingLights(planePos, planeRot);
    }
    else if (now > 80.2f)
    {
        E_sceneObject_setActive(MUMBAI_PLANE, 1);
    }

    if (now > 51.2f && now <= 65.5f)
    {
        E_sceneObject_setPosition(s_fpWalkerRdGirl, E_spline_getPositionBetweenTime(s_fpWalkerRdGirlSpline, 51.2f, 66.2f, 1.0f));
        E_sceneObject_setRotation(s_fpWalkerRdGirl, E_spline_getRotationBetweenTime(s_fpWalkerRdGirlSpline, 51.2f, 66.2f, 1.0f));
    }
    else if (now > 65.5f)
    {
        if (s_fpWalkerRdGirl->animator) E_animator_pause(s_fpWalkerRdGirl->animator);
    }

    vmath::vec3 busOffset = vmath::vec3(0.25f,0.0f,0.0f);
    E_sceneObject_setPosition(MUMBAI_DARSHAN_BUS, E_spline_getPositionLooped(ESpline_BUS1, 0.03f) + busOffset);
    if (s_sl_busHeadlamp_left)  E_spotLight_setPosition(s_sl_busHeadlamp_left,  vmath::vec3(MUMBAI_DARSHAN_BUS->position[0]+0.32f,  MUMBAI_DARSHAN_BUS->position[1]+0.36f, MUMBAI_DARSHAN_BUS->position[2]+0.80f));
    if (s_sl_busHeadlamp_right) E_spotLight_setPosition(s_sl_busHeadlamp_right, vmath::vec3(MUMBAI_DARSHAN_BUS->position[0]-0.38f, MUMBAI_DARSHAN_BUS->position[1]+0.36f, MUMBAI_DARSHAN_BUS->position[2]+0.80f));
    if (s_pl_insideMovingBus)   E_pointLight_setPosition(s_pl_insideMovingBus,  vmath::vec3(MUMBAI_DARSHAN_BUS->position[0]+0.03f,  MUMBAI_DARSHAN_BUS->position[1]+0.61f, MUMBAI_DARSHAN_BUS->position[2]-0.18f));

    if (now > 104.2f && now <= 109.2f)
    {
        E_sceneObject_setPosition(MUMBAI_TAXI, E_spline_getPositionBetweenTime(ESpline_MumbaiTaxi, 104.2f, 109.2f, 1.0f));
        E_sceneObject_setRotation(MUMBAI_TAXI, E_spline_getRotationBetweenTime(ESpline_MumbaiTaxi, 104.2f, 109.2f, 1.0f));
    }
    if (s_pl_taxi)
    {
        E_pointLight_setPosition(s_pl_taxi, vmath::vec3(MUMBAI_TAXI->position[0], 0.45f, MUMBAI_TAXI->position[2]));
    }

    static const float FP1_SPEED = 0.00024f;
    for (int i = 0; i < s_fp1TOTAL; i++)
    {
        int lane = i / FP1_PER_LANE;
        s_fp1T[i] += FP1_SPEED;
        if (s_fp1T[i] > 1.0f) s_fp1T[i] -= 1.0f;
        E_sceneObject_setPosition(s_fp1Walkers[i], E_spline_evaluate(s_fp1Splines[lane], s_fp1T[i]));
    }

    static const float FP2_SPEED = 0.00024f;
    for (int i = 0; i < s_fp2TOTAL; i++)
    {
        s_fp2T[i] += FP2_SPEED;
        if (s_fp2T[i] > 1.0f) s_fp2T[i] -= 1.0f;
        E_sceneObject_setPosition(s_fp2Walkers[i], E_spline_evaluate(s_fp2Spline, s_fp2T[i]));
    }

    for (int i = 0; i < s_boatsB1_COUNT; i++)
    {
        float y = -1.149f + sinf(now * 0.7f + s_boatsB1Phase[i]) * 0.05f;
        E_sceneObject_setPosition(s_boatsB1[i], vmath::vec3(s_boatsB1X[i], y, s_boatsB1Z[i]));
    }
}

// 53 sec to 57.5 sec -> sone chandi ki dagariya tu dekh babuba
// 57.5 sec to 1:02 sec -> jadu tone ki bajariya mein
// 1:02 to 1:08 -> jadu tone ... maya ki nagariya mein badalleeee
// 1:08 to 1:12.5 -> jhat pat badle mukaddar....
// 1:12.5 to 1:17 -> ee hai bambai...
// 1:17 to 1:22 -> ee hai bambai...

// CST
// 1:22 to 1:26 -> shehnai
// 1:26 to 1:31 -> shehnai 2
// 1:31 to 1:35 -> shehnai again
// 1:35 to 1:39 -> shehnai 2 again
// 1:39 to 1:43 -> strings

void MumbaiDarshan_updateCamera()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    if (now > 51.2f && now <= 72.2f)
    {
        vmath::vec3 focusPt = (now <= 66.2f)
            ? E_spline_getPositionBetweenTime(s_fpWalkerRdGirlLookAtSpline, 51.2f, 66.2f, 1.0f)
            : E_spline_getPositionBetweenTime(ESpline_RadioToGirl, 66.2f, 72.2f, 1.0f);

        if (now <= 66.2f && s_fpWalkerRdGirl->animator)
        {
            vmath::vec3 walkerHead = E_animator_getBonePosition(s_fpWalkerRdGirl->animator, "mixamorig:HeadTop_End");
            E_camera_setPosition(walkerHead[0], walkerHead[1], walkerHead[2]);
            E_setFOV(E_Utils_lerpBetweenTime(now, 51.2f, 66.2f, 62.0f, 45.0f));
        }

        E_camera_lookAtPoint(focusPt);
        E_setFOV(E_Utils_lerpBetweenTime(now, 66.2f, 72.2f, 45.0f, 27.0f));
        { vmath::vec3 _d = focusPt - engineCamera.position; E_postProcess_setDOF(1, sqrtf(_d[0]*_d[0]+_d[1]*_d[1]+_d[2]*_d[2]), 3.0f, 6.0f); }
    }

    if (now > 72.2f && now <= 76.2f)
    {
        E_camera_setPosition(E_spline_getPositionBetweenTime(ESpline_RoundTheGirl, 72.2f, 76.2f, 0.5f));
        E_setFOV(14.0f);
        E_camera_lookAtObject(RopeGirlFocusPt);
        { vmath::vec3 _d = RopeGirlFocusPt->position - engineCamera.position; E_postProcess_setDOF(1, sqrtf(_d[0]*_d[0]+_d[1]*_d[1]+_d[2]*_d[2]), 5.0f, 10.0f); }
    }

    if (now > 76.2f && now <= 80.2f)
    {
        E_camera_setPosition(E_spline_getPositionBetweenTime(s_splineBambai, 76.2f, 80.2f, 0.75f));
        E_setFOV(45.0f);
        E_camera_lookAtObject(MUMBAI_PLANE);
        { vmath::vec3 _d = MUMBAI_PLANE->position - engineCamera.position; E_postProcess_setDOF(1, sqrtf(_d[0]*_d[0]+_d[1]*_d[1]+_d[2]*_d[2]), 5.0f, 10.0f); }
    }

    if (now > 80.2f && now <= 84.7f)
    {
        E_fog_setEnabled(&engineScene.fog, 0);
        E_camera_setPosition(4.290f, 0.344f, 28.291f);
        E_setFOV(10.0f);
        vmath::vec3 busLookAt = MUMBAI_DARSHAN_BUS->position + vmath::vec3(0.0f, 0.717f, 0.0f);
        E_camera_lookAtPoint(busLookAt);
        { vmath::vec3 _d = busLookAt - engineCamera.position; E_postProcess_setDOF(1, sqrtf(_d[0]*_d[0]+_d[1]*_d[1]+_d[2]*_d[2]), 5.0f, 10.0f); }
    }

    if (now > 84.7f && now <= 89.2f)
    {
        // vmath::vec3 busPos = MUMBAI_DARSHAN_BUS->position + vmath::vec3(0.0f, 0.717f, 0.0f);
        // E_camera_setPosition(busPos[0], busPos[1], busPos[2]);
        // E_setFOV(E_Utils_lerpBetweenTime(now, 57.5f, 62.0f, 60.0f, 20.0f));
        // vmath::vec3 focusPt = vmath::vec3(-51.234f, 3.451f, 2.718f);
        // E_camera_lookAtPoint(focusPt);
        // E_postProcess_applyDOF(focusPt, 15.0f, 5.0f);
        vmath::vec3 walkerHead = E_animator_getBonePosition(s_fp1Walkers[20]->animator, "mixamorig:HeadTop_End");
        E_camera_setPosition(walkerHead[0], walkerHead[1], walkerHead[2]);
        E_setFOV(15.0f);
        E_camera_lookAtObject(s_bigShip1[0]);
        { vmath::vec3 _d = s_bigShip1[0]->position - engineCamera.position; E_postProcess_setDOF(1, sqrtf(_d[0]*_d[0]+_d[1]*_d[1]+_d[2]*_d[2]), 5.0f, 4.0f); }
    }

    if (now > 89.2f && now <= 95.2f)
    {
        // vmath::vec3 walkerHead = E_animator_getBonePosition(s_fp1Walkers[20]->animator, "mixamorig:HeadTop_End");
        // E_camera_setPosition(walkerHead[0], walkerHead[1], walkerHead[2]);
        // E_setFOV(15.0f);
        // E_camera_lookAtObject(s_bigShip1[0]);
        // E_postProcess_applyDOF(s_bigShip1[0]->position, 5.0f, 4.0f);
        E_setFOV(E_Utils_lerpBetweenTime(now, 89.2f, 95.2f, 15.0f, 55.0f));
        vmath::vec3 camPos = E_spline_getPositionBetweenTime(s_splineCamPosMoviePoster, 89.2f, 95.2f, 1.0f);
        E_camera_setPosition(camPos[0], camPos[1], camPos[2]);
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineMoviePoster, 89.2f, 95.2f, 1.0f);
        E_camera_lookAtPoint(lookAt);
        { vmath::vec3 _d = lookAt - engineCamera.position; E_postProcess_setDOF(1, sqrtf(_d[0]*_d[0]+_d[1]*_d[1]+_d[2]*_d[2]), 5.0f, 4.0f); }
    }

    // if (now > 64.0f && now <= 68.0f)
    // {
    //     E_setFOV(E_Utils_lerpBetweenTime(now, 64.0f, 68.0f, 50.0f, 15.0f));
    //     vmath::vec3 camPos = E_spline_getPositionBetweenTime(s_splineCamPosMoviePoster, 55.0f, 70.0f, 1.0f);
    //     E_camera_setPosition(camPos[0], camPos[1], camPos[2]);
    //     vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineMoviePoster, 64.0f, 68.0f, 1.0f);
    //     E_camera_lookAtPoint(lookAt);
    //     E_postProcess_applyDOF(lookAt, 5.0f, 4.0f);
    // }

    if (now > 95.2f && now <= 104.2f)
    {
        E_setFOV(30.0f);
        vmath::vec3 camPos = E_spline_getPositionBetweenTime(s_splineCouple, 87.2f, 107.2f, 1.0f);
        E_camera_setPosition(camPos[0], camPos[1], camPos[2]);
        vmath::vec3 lookAt = vmath::vec3(-5.717f, 0.559f, 19.480f);
        E_camera_lookAtPoint(lookAt);
        { vmath::vec3 _d = lookAt - engineCamera.position; E_postProcess_setDOF(1, sqrtf(_d[0]*_d[0]+_d[1]*_d[1]+_d[2]*_d[2]), 3.0f, 5.0f); }
    }

    // 77-82s: CST fly-by
    if (now > 104.2f && now <= 109.2f)
    {
        E_setFOV(E_Utils_lerpBetweenTime(now, 104.2f, 109.2f, 90.0f, 15.0f));
        vmath::vec3 camPos = MUMBAI_TAXI->position + vmath::vec3(-0.085f, 0.42f, 0.0f);
        vmath::vec3 lookAt = E_spline_getPositionBetweenTime(s_splineCSTCamLookAt, 99.2f, 113.2f, 0.5f);
        E_camera_setPosition(camPos[0], camPos[1], camPos[2]);
        E_camera_lookAtPoint(lookAt);
        { vmath::vec3 _d = lookAt - engineCamera.position; E_postProcess_setDOF(1, sqrtf(_d[0]*_d[0]+_d[1]*_d[1]+_d[2]*_d[2]), 5.0f, 4.0f); }
    }
}

void MumbaiDarshan_update()
{
    MumbaiDarshan_updateWorld();
    MumbaiDarshan_updateCamera();
}

void MumbaiDarshan_uninit()
{
    if (s_emitterTajFire1)  { E_emitter_deactivate(s_emitterTajFire1);  s_emitterTajFire1  = NULL; }
    if (s_emitterTajSmoke1) { E_emitter_deactivate(s_emitterTajSmoke1); s_emitterTajSmoke1 = NULL; }

    if (s_pl_taxi)          { E_pointLight_deactivate(s_pl_taxi);          s_pl_taxi          = NULL; }
    if (s_pl_girl)          { E_pointLight_deactivate(s_pl_girl);          s_pl_girl          = NULL; }
    if (s_pl_radio)         { E_pointLight_deactivate(s_pl_radio);         s_pl_radio         = NULL; }
    if (s_pl_vadapav)       { E_pointLight_deactivate(s_pl_vadapav);       s_pl_vadapav       = NULL; }
    if (s_pl_planeRightWing){ E_pointLight_deactivate(s_pl_planeRightWing);s_pl_planeRightWing= NULL; }
    if (s_pl_planeLeftWing) { E_pointLight_deactivate(s_pl_planeLeftWing); s_pl_planeLeftWing = NULL; }

    if (s_sl_GOI_white)      { E_spotLight_deactivate(s_sl_GOI_white);      s_sl_GOI_white      = NULL; }
    if (s_sl_GOI_orange)     { E_spotLight_deactivate(s_sl_GOI_orange);     s_sl_GOI_orange     = NULL; }
    if (s_sl_GOI_green)      { E_spotLight_deactivate(s_sl_GOI_green);      s_sl_GOI_green      = NULL; }
    if (s_sl_Taj_1)          { E_spotLight_deactivate(s_sl_Taj_1);          s_sl_Taj_1          = NULL; }
    if (s_sl_Taj_2)          { E_spotLight_deactivate(s_sl_Taj_2);          s_sl_Taj_2          = NULL; }
    if (s_sl_Taj_3)          { E_spotLight_deactivate(s_sl_Taj_3);          s_sl_Taj_3          = NULL; }
    if (s_sl_Taj_4)          { E_spotLight_deactivate(s_sl_Taj_4);          s_sl_Taj_4          = NULL; }
    if (s_sl_GOI_side)       { E_spotLight_deactivate(s_sl_GOI_side);       s_sl_GOI_side       = NULL; }
    if (s_sl_poster_1)       { E_spotLight_deactivate(s_sl_poster_1);       s_sl_poster_1       = NULL; }
    if (s_sl_poster_2)       { E_spotLight_deactivate(s_sl_poster_2);       s_sl_poster_2       = NULL; }
    if (s_sl_poster_3)       { E_spotLight_deactivate(s_sl_poster_3);       s_sl_poster_3       = NULL; }
    if (s_sl_poster_4)       { E_spotLight_deactivate(s_sl_poster_4);       s_sl_poster_4       = NULL; }
    if (s_sl_fp_1)           { E_spotLight_deactivate(s_sl_fp_1);           s_sl_fp_1           = NULL; }
    if (s_sl_fp_2)           { E_spotLight_deactivate(s_sl_fp_2);           s_sl_fp_2           = NULL; }
    if (s_sl_fp_3)           { E_spotLight_deactivate(s_sl_fp_3);           s_sl_fp_3           = NULL; }
    if (s_sl_fp_4)           { E_spotLight_deactivate(s_sl_fp_4);           s_sl_fp_4           = NULL; }
    if (s_sl_fp_5)           { E_spotLight_deactivate(s_sl_fp_5);           s_sl_fp_5           = NULL; }
    if (s_sl_fp_6)           { E_spotLight_deactivate(s_sl_fp_6);           s_sl_fp_6           = NULL; }
    if (s_sl_road_1)         { E_spotLight_deactivate(s_sl_road_1);         s_sl_road_1         = NULL; }
    if (s_sl_road_2)         { E_spotLight_deactivate(s_sl_road_2);         s_sl_road_2         = NULL; }
    if (s_sl_road_3)         { E_spotLight_deactivate(s_sl_road_3);         s_sl_road_3         = NULL; }
    if (s_sl_road_4)         { E_spotLight_deactivate(s_sl_road_4);         s_sl_road_4         = NULL; }
    if (s_sl_road_5)         { E_spotLight_deactivate(s_sl_road_5);         s_sl_road_5         = NULL; }
    if (s_sl_road_6)         { E_spotLight_deactivate(s_sl_road_6);         s_sl_road_6         = NULL; }
    if (s_sl_nearGirl)       { E_spotLight_deactivate(s_sl_nearGirl);       s_sl_nearGirl       = NULL; }
    if (s_sl_nearCouple)     { E_spotLight_deactivate(s_sl_nearCouple);     s_sl_nearCouple     = NULL; }
    if (s_sl_onBus)          { E_spotLight_deactivate(s_sl_onBus);          s_sl_onBus          = NULL; }
    if (s_sl_nearWalkingMan) { E_spotLight_deactivate(s_sl_nearWalkingMan); s_sl_nearWalkingMan = NULL; }

    E_sceneObject_destroyResources(MUMBAI_SKYSPHERE);      MUMBAI_SKYSPHERE      = NULL;
    E_sceneObject_destroyResources(MUMBAI_OCEAN_FLOOR);    MUMBAI_OCEAN_FLOOR    = NULL;
    E_sceneObject_destroyResources(MUMBAI_OCEAN);          MUMBAI_OCEAN          = NULL;
    E_sceneObject_destroyResources(MUMBAI_SCENE);          MUMBAI_SCENE          = NULL;
    E_sceneObject_destroyResources(MUMBAI_DARSHAN_BUS);    MUMBAI_DARSHAN_BUS    = NULL;
    E_sceneObject_destroyResources(MUMBAI_TAXI);           MUMBAI_TAXI           = NULL;
    E_sceneObject_destroyResources(MUMBAI_TIGHTROPE);      MUMBAI_TIGHTROPE      = NULL;
    E_sceneObject_destroyResources(MUMBAI_TIGHTROPE_GIRL); MUMBAI_TIGHTROPE_GIRL = NULL;
    E_sceneObject_destroyResources(MUMBAI_RADIO);          MUMBAI_RADIO          = NULL;
    E_sceneObject_destroyResources(MUMBAI_PLANE);          MUMBAI_PLANE          = NULL;
    E_sceneObject_destroyResources(s_fpWalkerRdGirl);      s_fpWalkerRdGirl      = NULL;
}
