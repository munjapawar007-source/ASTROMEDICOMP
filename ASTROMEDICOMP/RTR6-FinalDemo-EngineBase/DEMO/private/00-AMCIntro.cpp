#include "../public/00-AMCIntro.h"
#include "../public/DEMO.hpp"
#include "engineAPIs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "E_utils.h"
#include "E_spline.h"
#include "E_material.h"
#include "E_fog.h"
#include "E_video.h"
#include <GL/glew.h>

static E_sceneObject* s_theatre       = NULL;
static E_sceneObject* s_theatreScreen = NULL;
static E_video*       s_screenVideo   = NULL;

static E_spline*      s_splineToScreen   = NULL;

static E_pointLight*  s_pl_Exit         = NULL;
static E_pointLight*  s_pl_Ceiling1     = NULL;
static E_pointLight*  s_pl_Ceiling2     = NULL;
static E_pointLight*  s_pl_Ceiling3     = NULL;
static E_pointLight*  s_pl_Ceiling4     = NULL;
static E_pointLight*  s_pl_Ceiling5     = NULL;
static E_pointLight*  s_pl_Ceiling6     = NULL;
static E_pointLight*  s_pl_Ceiling7     = NULL;
static E_pointLight*  s_pl_Ceiling8     = NULL;
static E_pointLight*  s_pl_Ceiling9     = NULL;
static E_pointLight*  s_pl_Ceiling10    = NULL;

static E_pointLight*  s_pl_11              = NULL;

static E_spotLight*   s_sl_FromScreen      = NULL;
static E_spotLight*   s_sl_RightSpeaker    = NULL;
static E_spotLight*   s_sl_LeftSpeaker     = NULL;
static E_spotLight*   s_sl_RightSpeaker2   = NULL;
static E_spotLight*   s_sl_LeftSpeaker2    = NULL;

void AMCIntro_init()
{
    s_theatre       = E_loadModel("DEMO_ASSETS/00-AMCIntro/Theatre.glb",       &engineScene, "AMC_Theatre");
    s_theatreScreen = E_loadModel("DEMO_ASSETS/00-AMCIntro/TheatreScreen.glb", &engineScene, "AMC_TheatreScreen");
    s_screenVideo   = E_video_load("DEMO_ASSETS/00-AMCIntro/AMC_Intro.mp4", 0);
    {
        E_material videoMat = E_createMaterialFromFiles("engineResources/video.vert", "engineResources/video.frag");
        videoMat.baseColor[0] = videoMat.baseColor[1] = videoMat.baseColor[2] = videoMat.baseColor[3] = 1.0f;
        E_sceneObject_setMaterialAll(s_theatreScreen, videoMat);
    }
    E_sceneObject_setCastShadowAll   (s_theatreScreen, 0);
    E_sceneObject_setReceiveShadowAll(s_theatreScreen, 0);
    //E_sceneObject_setScaleXYZ        (s_theatreScreen, {1.0f, -1.0f, 1.0f});

    {
        s_splineToScreen = E_scene_addSpline(&engineScene, "splineToScreen");
        s_splineToScreen->numPoints    = 5;
        s_splineToScreen->points[0] = vmath::vec3(-0.076f, 4.979f, -10.717f);
        s_splineToScreen->points[1] = vmath::vec3(-0.061f, 4.385f,  -9.341f);
        s_splineToScreen->points[2] = vmath::vec3(-0.098f, 3.863f,  -4.974f);
        s_splineToScreen->points[3] = vmath::vec3(-0.030f, 6.836f,   9.855f);
        s_splineToScreen->points[4] = vmath::vec3(-0.394f, 3.411f,  12.580f);
    }

    {
        E_pointLight pl = E_pointLight_create({13.89f, 3.25f, 11.16f}, {0.98f, 0.98f, 0.98f}, 0.19f, 1.88f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Exit = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Exit, "pointLight_Exit");
    }
    {
        E_pointLight pl = E_pointLight_create({10.66f, 13.56f, 13.10f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling1 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling1, "pointLight_Ceiling1");
    }
    {
        E_pointLight pl = E_pointLight_create({5.26f, 13.56f, 13.10f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling2 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling2, "pointLight_Ceiling2");
    }
    {
        E_pointLight pl = E_pointLight_create({0.00f, 13.56f, 13.10f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling3 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling3, "pointLight_Ceiling3");
    }
    {
        E_pointLight pl = E_pointLight_create({-5.26f, 13.56f, 13.10f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling4 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling4, "pointLight_Ceiling4");
    }
    {
        E_pointLight pl = E_pointLight_create({-10.66f, 13.56f, 13.10f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling5 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling5, "pointLight_Ceiling5");
    }
    {
        E_pointLight pl = E_pointLight_create({10.66f, 13.56f, 2.00f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling6 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling6, "pointLight_Ceiling6");
    }
    {
        E_pointLight pl = E_pointLight_create({5.26f, 13.56f, 2.00f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling7 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling7, "pointLight_Ceiling7");
    }
    {
        E_pointLight pl = E_pointLight_create({0.00f, 13.56f, 2.00f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling8 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling8, "pointLight_Ceiling8");
    }
    {
        E_pointLight pl = E_pointLight_create({-5.26f, 13.56f, 2.00f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling9 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling9, "pointLight_Ceiling9");
    }
    {
        E_pointLight pl = E_pointLight_create({-10.66f, 13.56f, 2.00f}, {1.00f, 1.00f, 1.00f}, 0.63f, 1.15f);
        pl.flicker = 0; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_Ceiling10 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_Ceiling10, "pointLight_Ceiling10");
    }

    {
        E_pointLight pl = E_pointLight_create({0.00f, 8.70f, -2.80f}, {1.00f, 1.00f, 1.00f}, 0.32f, 10.00f);
        pl.flicker = 1; pl.flickerSpeed = 3.000f; pl.flickerAmount = 0.500f;
        s_pl_11 = E_pointLight_addToScene(&engineScene, pl);
        E_pointLight_setName(s_pl_11, "pointLight_11");
    }

    {
        E_spotLight sl = E_spotLight_create(
            {-0.49f, 2.69f, 18.47f},
            {-0.00f, -0.12f, -0.99f},
            {0.98f, 0.53f, 0.43f},
            0.94f,
            30.20f, 60.60f,
            32.80f);
        sl.flicker = 1; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_FromScreen = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_FromScreen, "spotLight_FromScreen");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-13.44f, 10.02f, 4.79f},
            {0.00f, -1.00f, 0.00f},
            {0.97f, 0.67f, 0.44f},
            7.97f,
            15.00f, 20.00f,
            4.00f);
        sl.flicker = 0; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_RightSpeaker = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_RightSpeaker, "spotLight_RightSpeaker");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {13.44f, 10.02f, 4.79f},
            {0.00f, -1.00f, 0.00f},
            {0.97f, 0.67f, 0.44f},
            7.97f,
            15.00f, 20.00f,
            4.00f);
        sl.flicker = 0; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_LeftSpeaker = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_LeftSpeaker, "spotLight_LeftSpeaker");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {-13.44f, 9.02f, 10.49f},
            {0.00f, -1.00f, 0.00f},
            {0.97f, 0.67f, 0.44f},
            7.97f,
            15.00f, 20.00f,
            4.00f);
        sl.flicker = 0; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_RightSpeaker2 = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_RightSpeaker2, "spotLight_RightSpeaker2");
    }
    {
        E_spotLight sl = E_spotLight_create(
            {13.44f, 9.02f, 10.49f},
            {0.00f, -1.00f, 0.00f},
            {0.97f, 0.67f, 0.44f},
            7.97f,
            15.00f, 20.00f,
            4.11f);
        sl.flicker = 0; sl.flickerSpeed = 3.000f; sl.flickerAmount = 0.500f; sl.castShadow = 0;
        s_sl_LeftSpeaker2 = E_spotLight_addToScene(&engineScene, sl);
        E_spotLight_setName(s_sl_LeftSpeaker2, "spotLight_LeftSpeaker2");
    }
}

void AMCIntro_begin()
{
    static int s_begun = 0;
    if (s_begun) return;
    s_begun = 1;

    if (s_screenVideo) E_video_rewind(s_screenVideo);
    E_fog_setEnabled(&engineScene.fog, 0);

    E_postProcess_setFXAA(1);
    E_postProcess_setSaturation(1.01f);
    E_postProcess_setBlur(0.00f);
    E_postProcess_setVignette(0.07f, 1.00f);
    E_postProcess_setGodRays(0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
    E_postProcess_setBloom(1, 1.00f, 5.11f);

    E_directionalLight_setIntensity(&engineScene.directionalLight, 0.00f);
    E_ambientLight_setColor    (&engineScene.ambientLight, {0.08f, 0.73f, 1.00f});
    E_ambientLight_setIntensity(&engineScene.ambientLight, 0.17f);

    E_setFOV(89.0f);
    E_camera_setProjection(89.0f, 0.01f, 1000.00f);
    E_camera_setPosition(s_splineToScreen->points[0][0], s_splineToScreen->points[0][1], s_splineToScreen->points[0][2]);
    E_camera_lookAtPoint(vmath::vec3(-0.096f, 6.516f, 18.269f));

    AMCIntro_activate();
}

void AMCIntro_activate()
{
    if (s_theatre)       E_sceneObject_setActive(s_theatre,       1);
    if (s_theatreScreen) E_sceneObject_setActive(s_theatreScreen, 1);
    if (s_pl_11)         E_pointLight_activate(s_pl_11);
    if (s_pl_Exit)       E_pointLight_activate(s_pl_Exit);
    if (s_pl_Ceiling1)   E_pointLight_activate(s_pl_Ceiling1);
    if (s_pl_Ceiling2)   E_pointLight_activate(s_pl_Ceiling2);
    if (s_pl_Ceiling3)   E_pointLight_activate(s_pl_Ceiling3);
    if (s_pl_Ceiling4)   E_pointLight_activate(s_pl_Ceiling4);
    if (s_pl_Ceiling5)   E_pointLight_activate(s_pl_Ceiling5);
    if (s_pl_Ceiling6)   E_pointLight_activate(s_pl_Ceiling6);
    if (s_pl_Ceiling7)   E_pointLight_activate(s_pl_Ceiling7);
    if (s_pl_Ceiling8)   E_pointLight_activate(s_pl_Ceiling8);
    if (s_pl_Ceiling9)   E_pointLight_activate(s_pl_Ceiling9);
    if (s_pl_Ceiling10)  E_pointLight_activate(s_pl_Ceiling10);
    if (s_sl_FromScreen)    E_spotLight_activate(s_sl_FromScreen);
    if (s_sl_RightSpeaker)  E_spotLight_activate(s_sl_RightSpeaker);
    if (s_sl_LeftSpeaker)   E_spotLight_activate(s_sl_LeftSpeaker);
    if (s_sl_RightSpeaker2) E_spotLight_activate(s_sl_RightSpeaker2);
    if (s_sl_LeftSpeaker2)  E_spotLight_activate(s_sl_LeftSpeaker2);
}

void AMCIntro_deactivate()
{
    if (s_theatre)       E_sceneObject_setActive(s_theatre,       0);
    if (s_theatreScreen) E_sceneObject_setActive(s_theatreScreen, 0);
    if (s_pl_11)         E_pointLight_deactivate(s_pl_11);
    if (s_pl_Exit)       E_pointLight_deactivate(s_pl_Exit);
    if (s_pl_Ceiling1)   E_pointLight_deactivate(s_pl_Ceiling1);
    if (s_pl_Ceiling2)   E_pointLight_deactivate(s_pl_Ceiling2);
    if (s_pl_Ceiling3)   E_pointLight_deactivate(s_pl_Ceiling3);
    if (s_pl_Ceiling4)   E_pointLight_deactivate(s_pl_Ceiling4);
    if (s_pl_Ceiling5)   E_pointLight_deactivate(s_pl_Ceiling5);
    if (s_pl_Ceiling6)   E_pointLight_deactivate(s_pl_Ceiling6);
    if (s_pl_Ceiling7)   E_pointLight_deactivate(s_pl_Ceiling7);
    if (s_pl_Ceiling8)   E_pointLight_deactivate(s_pl_Ceiling8);
    if (s_pl_Ceiling9)   E_pointLight_deactivate(s_pl_Ceiling9);
    if (s_pl_Ceiling10)  E_pointLight_deactivate(s_pl_Ceiling10);
    if (s_sl_FromScreen)    E_spotLight_deactivate(s_sl_FromScreen);
    if (s_sl_RightSpeaker)  E_spotLight_deactivate(s_sl_RightSpeaker);
    if (s_sl_LeftSpeaker)   E_spotLight_deactivate(s_sl_LeftSpeaker);
    if (s_sl_RightSpeaker2) E_spotLight_deactivate(s_sl_RightSpeaker2);
    if (s_sl_LeftSpeaker2)  E_spotLight_deactivate(s_sl_LeftSpeaker2);
}

void AMCIntro_updateWorld()
{
    if (s_screenVideo && s_theatreScreen)
    {
        E_video_update(s_screenVideo);
        GLuint frame = E_video_getTexture(s_screenVideo);
        E_sceneObject_setAlbedoAll(s_theatreScreen, frame);
    }
}

void AMCIntro_updateCamera()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    E_setFOV(E_Utils_lerpBetweenTime(now, 3.0f, 27.2f, 89.0f, 50.0f));

    vmath::vec3 pos = E_spline_getPositionBetweenTime(s_splineToScreen, 3.0f, 27.2f, 1.0f);
    E_camera_setPosition(pos[0], pos[1], pos[2]);

    vmath::vec3 focusPt = vmath::vec3(-0.096f, 6.350f, 18.269f);
    E_camera_lookAtPoint(focusPt);
    E_postProcess_applyDOF(focusPt, 9.92f, 18.03f);
}

void AMCIntro_update()
{
    AMCIntro_updateWorld();
    AMCIntro_updateCamera();
}

void AMCIntro_uninit()
{
    if (s_screenVideo) E_video_destroy(s_screenVideo);
    s_screenVideo = NULL;
    E_sceneObject_destroyResources(s_theatre);          s_theatre = NULL;
    E_sceneObject_destroyResources(s_theatreScreen);    s_theatreScreen = NULL;

    if (s_pl_Exit)     { E_pointLight_deactivate(s_pl_Exit);     s_pl_Exit     = NULL; }
    if (s_pl_Ceiling1) { E_pointLight_deactivate(s_pl_Ceiling1); s_pl_Ceiling1 = NULL; }
    if (s_pl_Ceiling2) { E_pointLight_deactivate(s_pl_Ceiling2); s_pl_Ceiling2 = NULL; }
    if (s_pl_Ceiling3) { E_pointLight_deactivate(s_pl_Ceiling3); s_pl_Ceiling3 = NULL; }
    if (s_pl_Ceiling4) { E_pointLight_deactivate(s_pl_Ceiling4); s_pl_Ceiling4 = NULL; }
    if (s_pl_Ceiling5) { E_pointLight_deactivate(s_pl_Ceiling5); s_pl_Ceiling5 = NULL; }
    if (s_pl_Ceiling6) { E_pointLight_deactivate(s_pl_Ceiling6); s_pl_Ceiling6 = NULL; }
    if (s_pl_Ceiling7) { E_pointLight_deactivate(s_pl_Ceiling7); s_pl_Ceiling7 = NULL; }
    if (s_pl_Ceiling8) { E_pointLight_deactivate(s_pl_Ceiling8); s_pl_Ceiling8 = NULL; }
    if (s_pl_Ceiling9) { E_pointLight_deactivate(s_pl_Ceiling9); s_pl_Ceiling9 = NULL; }
    if (s_pl_Ceiling10){ E_pointLight_deactivate(s_pl_Ceiling10);s_pl_Ceiling10 = NULL; }
    if (s_pl_11)       { E_pointLight_deactivate(s_pl_11);       s_pl_11       = NULL; }

    if (s_sl_FromScreen)    { E_spotLight_deactivate(s_sl_FromScreen);    s_sl_FromScreen    = NULL; }
    if (s_sl_RightSpeaker)  { E_spotLight_deactivate(s_sl_RightSpeaker);  s_sl_RightSpeaker  = NULL; }
    if (s_sl_LeftSpeaker)   { E_spotLight_deactivate(s_sl_LeftSpeaker);   s_sl_LeftSpeaker   = NULL; }
    if (s_sl_RightSpeaker2) { E_spotLight_deactivate(s_sl_RightSpeaker2); s_sl_RightSpeaker2 = NULL; }
    if (s_sl_LeftSpeaker2)  { E_spotLight_deactivate(s_sl_LeftSpeaker2);  s_sl_LeftSpeaker2  = NULL; }
}
