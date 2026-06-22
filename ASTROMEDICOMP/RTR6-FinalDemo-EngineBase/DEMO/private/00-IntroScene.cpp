#include "../public/00-IntroScene.h"
#include "../public/DEMO.hpp"
#include "E_utils.h"
#include "E_camera.h"
#include "E_modelLoader.h"
#include "E_material.h"
#include "E_animation.h"
#include "engineAPIs.h"

E_sceneObject* SHADERS_TEXT = NULL;
E_sceneObject* Mountain = NULL;
E_sceneObject* WaterGrid = NULL;
E_sceneObject* Sun = NULL;

E_sceneObject* Dragon = NULL;
E_emitter* emitter_dragonFire = NULL;

E_spline* E_spline_toMountain = NULL;
E_spline* E_spline_FollowCamtoMountain = NULL;
static E_spline*      E_spline_dragonMove  = NULL;
static E_pointLight*  s_pointLight_0       = NULL;

void IntroScene_init()
{
    SHADERS_TEXT = E_loadModel("DEMO_ASSETS/00-IntroScene/Shaders_Text.glb", &engineScene, "SHADERS_TEXT");
    Mountain = E_loadModel("DEMO_ASSETS/00-IntroScene/Mountains.glb", &engineScene, "Mountain");
    WaterGrid = E_loadModel("DEMO_ASSETS/00-IntroScene/waterGrid.glb", &engineScene, "WaterGrid");
    Sun = E_loadModel("DEMO_ASSETS/00-IntroScene/Sun.glb", &engineScene, "Sun");
    E_sceneObject_setPosition(Sun, {-5.91f, 268.88f, 1000.00f});
    E_sceneObject_setRotation(Sun, {0.00f, -284.00f, 0.00f});
    E_sceneObject_setScale(Sun, 600.00f);
    E_sceneObject_setActive(Sun, 1);
    E_sceneObject_setCastShadowAll(Sun, 0);

    Dragon = E_loadModel("DEMO_ASSETS/00-IntroScene/White.Dragon.glb", &engineScene, "Dragon");
    E_sceneObject_setPosition(Dragon, {-6.00f, 59.31f, 162.00f});
    E_sceneObject_setRotation(Dragon, {0.00f, -180.00f, 0.00f});
    E_sceneObject_setScale(Dragon, 3.00f);
    E_sceneObject_setActive(Dragon, 0);
    E_animator_setClipByName(Dragon->animator, "Attack_Fly_1");

    emitter_dragonFire = E_emitter_addToScene(&engineScene, 200);
    E_emitter_setPosition (emitter_dragonFire, {-6.54f, 107.53f, 143.06f});
    E_emitter_setDirection(emitter_dragonFire, {-0.00f, -1.70f, -0.83f});
    E_emitter_setSpread   (emitter_dragonFire, 6.90f);
    E_emitter_setSpawnRate(emitter_dragonFire, 251.46f);
    E_emitter_setLifespan (emitter_dragonFire, 1.04f);
    E_emitter_setSpeed    (emitter_dragonFire, 20.00f);
    E_emitter_setSize     (emitter_dragonFire, 1.33f, 10.00f);
    E_emitter_setColor    (emitter_dragonFire, 1.00f, 0.76f, 0.38f, 1.00f);
    E_emitter_setOpacity  (emitter_dragonFire, 0.39f);
    E_emitter_setPan      (emitter_dragonFire, 0.07f, 0.02f, 1.52f);
    E_emitter_setTextureType(emitter_dragonFire, (E_ParticleTextureType)1);

    E_pointLight pointLight_0 = E_pointLight_create(
        {-5.26f, 71.68f, 139.96f},
        {1.00f, 1.00f, 1.00f},
        2.0f, 20.00f);
    s_pointLight_0 = E_pointLight_addToScene(&engineScene, pointLight_0);
    E_pointLight_setName(s_pointLight_0, "pointLight_0");
    s_pointLight_0->flicker       = 1;
    s_pointLight_0->flickerSpeed  = 2.085f;
    s_pointLight_0->flickerAmount = 0.316f;
    s_pointLight_0->active        = 0;

    E_scene_setShadowArea(&engineScene, 0.0f, 0.0f, 0.0f, 1000.0f);

    E_material waterMat = E_createMaterialFromFiles("engineResources/water.vert", "engineResources/water.frag");
    E_sceneObject_setShaderAll(WaterGrid, waterMat.shaderProgram);
    E_sceneObject_setReceiveShadowAll(WaterGrid, 0);
    E_sceneObject_setCastShadowAll(WaterGrid, 0);

    E_sceneObject_setPosition(SHADERS_TEXT, {-6.60f, 70.200, 147.38f});
    E_sceneObject_setRotation(SHADERS_TEXT, {3.55f, 180.00f, 0.00f});
    E_sceneObject_setScale(SHADERS_TEXT, 15.00f);
    E_sceneObject_setActive(SHADERS_TEXT, 1);

    E_sceneObject_setPosition(Mountain, {0.00f, 0.00f, 0.00f});
    E_sceneObject_setRotation(Mountain, {0.00f, 0.00f, 0.00f});
    E_sceneObject_setScale(Mountain, 1.0f);
    E_sceneObject_setActive(Mountain, 1);

    E_spline_toMountain = E_scene_addSpline(&engineScene, "E_spline_toMountain");
    E_spline_toMountain->numPoints = 5;
    E_spline_toMountain->points[0] = vmath::vec3(-11.067f, -14.244f, -742.429f);
    E_spline_toMountain->points[1] = vmath::vec3(-3.490f, -20.831f, -540.427f);
    E_spline_toMountain->points[2] = vmath::vec3(0.203f, -28.107f, -316.668f);
    E_spline_toMountain->points[3] = vmath::vec3(-5.171f, 65.536f, 51.229f);
    E_spline_toMountain->points[4] = vmath::vec3(-4.605f, 81.111f, 141.476f);


    E_spline_FollowCamtoMountain = E_scene_addSpline(&engineScene, "introFollowSpline");
    E_spline_FollowCamtoMountain->numPoints = 5;
    E_spline_FollowCamtoMountain->points[0] = vmath::vec3(1.748f, -48.743f, -540.314f);
    E_spline_FollowCamtoMountain->points[1] = vmath::vec3(1.711f, -49.330f, -500.417f);
    E_spline_FollowCamtoMountain->points[2] = vmath::vec3(-10.880f, -47.453f, -277.131f);
    E_spline_FollowCamtoMountain->points[3] = vmath::vec3(-6.904f, 72.330f, 147.752f);
    E_spline_FollowCamtoMountain->points[4] = vmath::vec3(-2.581f, 87.480f, 160.076f);

    E_spline_dragonMove = E_scene_addSpline(&engineScene, "E_spline_dragonMove");
    E_spline_dragonMove->numPoints = 10;
    E_spline_dragonMove->points[0] = vmath::vec3(128.025f, 137.557f, 188.372f);
    E_spline_dragonMove->points[1] = vmath::vec3(113.722f, 123.348f, 181.020f);
    E_spline_dragonMove->points[2] = vmath::vec3( 85.200f, 101.637f, 164.306f);
    E_spline_dragonMove->points[3] = vmath::vec3( 27.267f,  78.029f, 129.335f);
    E_spline_dragonMove->points[4] = vmath::vec3(-35.499f,  74.164f, 125.630f);
    E_spline_dragonMove->points[5] = vmath::vec3(-98.853f,  93.922f, 173.015f);
    E_spline_dragonMove->points[6] = vmath::vec3(-69.354f, 143.646f, 329.720f);
    E_spline_dragonMove->points[7] = vmath::vec3(  3.128f, 144.771f, 332.570f);
    E_spline_dragonMove->points[8] = vmath::vec3( -6.162f,  59.077f, 162.838f);
    E_spline_dragonMove->points[9] = vmath::vec3( -4.206f,  81.042f, 127.896f);

}

void IntroScene_updateWorld()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    if (now >= 27.2f && now <= 51.2f)
    {
        float y = E_Utils_lerpBetweenTime(now, 27.2f, 44.2f, -17.0f, 70.200f);
        E_sceneObject_setPosition(SHADERS_TEXT, { -6.60f, y, 147.38f });
    }

    if (now >= 27.2f && now < 35.2f)
    {
        E_sceneObject_setActive(Dragon, 1);
        E_sceneObject_setWriteColorAll(Dragon, 0);
        vmath::vec3 camPos = E_spline_getPositionBetweenTime(E_spline_toMountain, 27.2f, 44.2f, 1.0f);
        vmath::vec3 dragonPos = camPos + vmath::vec3(-5.600f, 15.700f, 62.400f);
        if (now >= 31.2f)
            dragonPos[1] += E_Utils_lerpBetweenTime(now, 31.2f, 35.2f, 0.0f, 30.0f);
        E_sceneObject_setPosition(Dragon, dragonPos);
        E_sceneObject_setRotation(Dragon, {78.529f, 0.0f, 0.0f});
    }

    if (now >= 35.2f && now <= 48.2f)
    {
        E_sceneObject_setActive  (Dragon, 1);
        E_sceneObject_setWriteColorAll(Dragon, 1);
        E_sceneObject_setPosition(Dragon, E_spline_getPositionBetweenTime(E_spline_dragonMove, 35.2f, 48.2f, 1.0f));
        E_sceneObject_setRotation(Dragon, E_spline_getRotationBetweenTime(E_spline_dragonMove, 35.2f, 48.2f, 1.0f));
        float maxBlur = E_Utils_lerpBetweenTime(now, 40.2f, 41.2f, 10.0f, 0.0f);
        E_postProcess_setDOF(1, 10.0f, 2.0f, maxBlur);
    }

    if (Dragon && Dragon->animator && emitter_dragonFire)
    {
        emitter_dragonFire->active = (now >= 47.2f && now <= 51.2f) ? 1 : 0;
        if (s_pointLight_0)
        {
            s_pointLight_0->active = emitter_dragonFire->active;
            if (s_pointLight_0->active)
            {
                float fadeT = E_Utils_lerpBetweenTime(now, 47.2f, 48.2f, 0.0f, 1.0f);
                s_pointLight_0->baseIntensity = fadeT * 2.45f;
                s_pointLight_0->intensity     = s_pointLight_0->baseIntensity;
            }
        }
        if (emitter_dragonFire->active)
        {
            vmath::vec3 mouthPosOfDragon = E_animator_getBonePosition(Dragon->animator, "tongue_bone03_end_074");
            E_emitter_setPosition(emitter_dragonFire, {mouthPosOfDragon[0], mouthPosOfDragon[1] - 1.0f, mouthPosOfDragon[2]});
        }
    }
}

void IntroScene_updateCamera()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    if (now >= 27.2f && now <= 51.2f)
    {
        E_camera_setPosition(E_spline_getPositionBetweenTime(E_spline_toMountain, 27.2f, 44.2f, 1.0f));
        vmath::vec3 focusPt = E_spline_getPositionBetweenTime(E_spline_FollowCamtoMountain, 27.2f, 44.2f, 1.0f);
        E_camera_lookAtPoint(focusPt);
        E_setFOV(E_Utils_lerpBetweenTime(now, 41.2f, 51.2f, 60.0f, 30.0f));
        float dx = focusPt[0] - engineCamera.position[0];
        float dy = focusPt[1] - engineCamera.position[1];
        float dz = focusPt[2] - engineCamera.position[2];
        //E_postProcess_setDOF(1, sqrtf(dx*dx + dy*dy + dz*dz), 2.0f, 10.0f);
    }
}

void IntroScene_update()
{
    IntroScene_updateWorld();
    IntroScene_updateCamera();
}


// xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void IntroScene_begin()
{
    static int s_begun = 0;
    if (s_begun) return;
    s_begun = 1;

    E_postProcess_setSaturation(1.03f);
    E_postProcess_setBlur(0.00f);
    E_postProcess_setVignette(0.00f, 0.50f);
    E_postProcess_setDOF(1, 10.00f, 2.00f, 10.00f);
    E_postProcess_setGodRays(0, 0.500f, 0.500f, 100, 0.300f, 0.970f, 0.900f, 0.500f);
    E_postProcess_setBloom(1, 1.0f, 8.0f);

    E_directionalLight_setDirection(&engineScene.directionalLight, {0.11f, -1.00f, -0.90f});
    E_directionalLight_setColor(&engineScene.directionalLight, {1.00f, 1.00f, 0.67f});
    E_directionalLight_setIntensity(&engineScene.directionalLight, 1.00f);

    E_fog_setEnabled(&engineScene.fog, 0);
    E_fog_setColor(&engineScene.fog, {0.86f, 0.94f, 1.00f});
    E_fog_setDensity(&engineScene.fog, 0.01f);
    E_fog_setType(&engineScene.fog, 0);

    E_setFOV(60.0f);

    IntroScene_activate();

}

void IntroScene_activate()
{
    E_sceneObject_setActive(SHADERS_TEXT, 1);
    E_sceneObject_setActive(Mountain,     1);
    E_sceneObject_setActive(WaterGrid,    1);
    E_sceneObject_setActive(Sun,          1);
    E_sceneObject_setActive(Dragon,       0); // managed by updateWorld
    if (emitter_dragonFire) emitter_dragonFire->active = 0;
    if (s_pointLight_0)     s_pointLight_0->active     = 0;
}

void IntroScene_deactivate()
{
    E_sceneObject_setActive(SHADERS_TEXT, 0);
    E_sceneObject_setActive(Mountain,     0);
    E_sceneObject_setActive(WaterGrid,    0);
    E_sceneObject_setActive(Sun,          0);
    E_sceneObject_setActive(Dragon,       0);
    if (emitter_dragonFire) emitter_dragonFire->active = 0;
    if (s_pointLight_0)     s_pointLight_0->active     = 0;
}

void IntroScene_uninit()
{
    E_sceneObject_destroyResources(Sun);          Sun          = NULL;
    E_sceneObject_destroyResources(SHADERS_TEXT); SHADERS_TEXT = NULL;
    E_sceneObject_destroyResources(Mountain);     Mountain     = NULL;
    E_sceneObject_destroyResources(WaterGrid);    WaterGrid    = NULL;
    E_sceneObject_destroyResources(Dragon);       Dragon       = NULL;
    if (emitter_dragonFire) { E_emitter_deactivate(emitter_dragonFire); emitter_dragonFire = NULL; }
    if (s_pointLight_0) { E_pointLight_deactivate(s_pointLight_0); s_pointLight_0 = NULL; }
    E_spline_toMountain          = NULL;
    E_spline_FollowCamtoMountain = NULL;
    E_spline_dragonMove          = NULL;
}
