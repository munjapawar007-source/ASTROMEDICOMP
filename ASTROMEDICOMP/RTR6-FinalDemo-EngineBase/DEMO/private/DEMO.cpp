#include "../public/DEMO.hpp"

int E_FLAG_LIGHTING = 1;
int E_FLAG_SHADOWS = 1;
int E_FLAG_POSTPROCESS = 1;
int E_FLAG_ANIMATIONS = 1;
int E_FLAG_WIREFRAME = 0;

#define LOAD_AMCIntro 1
#define LOAD_IntroScene 1
#define LOAD_MumbaiDarshan 1
#define LOAD_CSTInterior 1
#define LOAD_TajDinning 1
#define LOAD_BedroomScene 1
#define LOAD_CamaHospital 1
#define LOAD_EndMessageAndCredits 1

#include "../public/00-AMCIntro.h"
#include "../public/00-IntroScene.h"
#include "../public/01-MumbaiDarshan.h"
#include "../public/02-CSTInterior.h"
#include "../public/03-TajDinningToTVBlast.h"
#include "../public/04-BedroomScene.h"
#include "../public/05-CamaHospital.h"
#include "../public/06-EndMessageAndCredits.h"
#include "E_utils.h"
#include "E_postProcess.h"
#include "E_scene.h"
#include "E_sceneObject.h"
#include <Windows.h>
#include <string.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#define AMCINTRO_STARTTIME 0.0f
#define INTROSCENE_STARTTIME 27.2f
#define SCENEONE_STARTTIME 51.2f
#define SCENETWO_STARTTIME 109.2f
#define SCENETHREE_STARTTIME 159.2f
#define SCENEFOUR_STARTTIME 241.2f
#define SCENEFIVE_STARTTIME 336.2f
#define SCENESIX_STARTTIME 540.2f

static const char* s_godRayPrefixes[] = {
    "polySurface88_metal_0",
    "Object_54",
    "Object_24",
    "support_beams_0",
    NULL
};

static void markGodRayEmitters(E_sceneObject* obj)
{
    if (!obj) return;
    int p;
    for (p = 0; s_godRayPrefixes[p] != NULL; p++)
    {
        int len = (int)strlen(s_godRayPrefixes[p]);
        if (strncmp(obj->name, s_godRayPrefixes[p], len) == 0 &&
            (obj->name[len] == '\0' || obj->name[len] == '.'))
        {
            E_sceneObject_setGodRayEmitterAll(obj, 1);
            break;
        }
    }
    int i;
    for (i = 0; i < obj->childCount; i++)
        markGodRayEmitters(obj->children[i]);
}

void DEMO_init()
{
    E_scene_init(&engineScene);
    E_scene_setShadowEnabled(&engineScene, 1);
    E_scene_setShadowArea(&engineScene, 0.0f, 0.0f, 0.0f, 80.0f);
    E_postProcess_setFade(1.0f);

    #if LOAD_AMCIntro
    AMCIntro_init();
    #endif
    #if LOAD_IntroScene
    IntroScene_init();
    #endif
    #if LOAD_MumbaiDarshan
    MumbaiDarshan_init();
    #endif
    #if LOAD_CSTInterior
    CSTInterior_init();
    #endif
    #if LOAD_TajDinning
    TajDinning_init();
    #endif
    #if LOAD_BedroomScene
    BedroomScene_init();
    #endif
    #if LOAD_CamaHospital
    CamaHospital_init();
    #endif
    #if LOAD_EndMessageAndCredits
    EndMessageAndCredits_init();
    #endif

    #if LOAD_AMCIntro
    AMCIntro_activate();
    #endif
    #if LOAD_IntroScene
    IntroScene_deactivate();
    #endif
    #if LOAD_MumbaiDarshan
    MumbaiDarshan_deactivate();
    #endif
    #if LOAD_CSTInterior
    CSTInterior_deactivate();
    #endif
    #if LOAD_TajDinning
    TajDinning_deactivate();
    #endif
    #if LOAD_BedroomScene
    BedroomScene_deactivate();
    #endif
    #if LOAD_CamaHospital
    CamaHospital_deactivate();
    #endif
    #if LOAD_EndMessageAndCredits
    EndMessageAndCredits_deactivate();
    #endif

    {
        int i;
        for (i = 0; i < engineScene.count; i++)
            markGodRayEmitters(engineScene.objects[i]);
    }

    mciSendStringA("open DEMO_ASSETS\\Audios\\Final_Storyboard_V7_Audio.wav type waveaudio alias demoaudio", NULL, 0, NULL);
    mciSendStringA("play demoaudio from 0", NULL, 0, NULL);

    g_readyToStart = 1;
}

void DEMO_update(float dt)
{
    if (!g_readyToStart) return;

    float t = (float)E_Utils_getElapsedTimeInSeconds();

    static int s_amcIntro_freed = 0;
    static int s_intro_freed = 0;
    static int s_mumbai_freed = 0;
    static int s_cst_freed = 0;
    static int s_taj_freed = 0;
    static int s_bedroom_freed = 0;
    static int s_cama_freed = 0;

    #define SCENE_FREE_BUFFER 5.0f

    if (!s_amcIntro_freed && t > INTROSCENE_STARTTIME + SCENE_FREE_BUFFER)
    {
        #if LOAD_AMCIntro
        AMCIntro_uninit();
        #endif
        s_amcIntro_freed = 1;
    }
    if (!s_intro_freed && t > SCENEONE_STARTTIME + SCENE_FREE_BUFFER)
    {
        #if LOAD_IntroScene
        IntroScene_uninit();
        #endif
        s_intro_freed = 1;
    }
    if (!s_mumbai_freed && t > SCENETWO_STARTTIME + SCENE_FREE_BUFFER)
    {
        #if LOAD_MumbaiDarshan
        MumbaiDarshan_uninit();
        #endif
        s_mumbai_freed = 1;
    }
    if (!s_cst_freed && t > SCENETHREE_STARTTIME + SCENE_FREE_BUFFER)
    {
        #if LOAD_CSTInterior
        CSTInterior_uninit();
        #endif
        s_cst_freed = 1;
    }
    if (!s_taj_freed && t > SCENEFOUR_STARTTIME + SCENE_FREE_BUFFER)
    {
        #if LOAD_TajDinning
        TajDinning_uninit();
        #endif
        s_taj_freed = 1;
    }
    if (!s_bedroom_freed && t > SCENEFIVE_STARTTIME + SCENE_FREE_BUFFER)
    {
        #if LOAD_BedroomScene
        BedroomScene_uninit();
        #endif
        s_bedroom_freed = 1;
    }
    if (!s_cama_freed && t > SCENESIX_STARTTIME + SCENE_FREE_BUFFER)
    {
        #if LOAD_CamaHospital
        CamaHospital_uninit();
        #endif
        s_cama_freed = 1;
    }

    E_postProcess_transitionBetweenTime(0.0f, 5.0f, E_TRANSITION_START);
    E_postProcess_transitionBetweenTime(INTROSCENE_STARTTIME - 0.4f, INTROSCENE_STARTTIME + 0.5f, E_TRANSITION_MID);
    E_postProcess_transitionBetweenTime(SCENEONE_STARTTIME - 2.0f, SCENEONE_STARTTIME + 2.0f, E_TRANSITION_MID);
    E_postProcess_transitionBetweenTime(SCENETWO_STARTTIME - 2.0f, SCENETWO_STARTTIME + 2.0f, E_TRANSITION_MID);
    E_postProcess_transitionBetweenTime(SCENETHREE_STARTTIME - 2.0f, SCENETHREE_STARTTIME + 2.0f, E_TRANSITION_MID);
    E_postProcess_transitionBetweenTime(SCENEFOUR_STARTTIME - 2.0f, SCENEFOUR_STARTTIME + 2.0f, E_TRANSITION_MID);
    E_postProcess_transitionBetweenTime(SCENEFIVE_STARTTIME - 2.0f, SCENEFIVE_STARTTIME + 2.0f, E_TRANSITION_MID);
    E_postProcess_transitionBetweenTime(521.0f, SCENESIX_STARTTIME, E_TRANSITION_END);

    if (t > AMCINTRO_STARTTIME && t <= INTROSCENE_STARTTIME)
    {
        #if LOAD_AMCIntro
        AMCIntro_begin();
        if (E_FLAG_ANIMATIONS) AMCIntro_updateWorld();
        AMCIntro_updateCamera();
        #endif
    }
    else if (t > INTROSCENE_STARTTIME && t <= SCENEONE_STARTTIME)
    {
        #if LOAD_AMCIntro
        if (!s_amcIntro_freed) AMCIntro_deactivate();
        #endif
        #if LOAD_IntroScene
        IntroScene_begin();
        if (E_FLAG_ANIMATIONS) IntroScene_updateWorld();
        IntroScene_updateCamera();
        #endif
    }
    else if (t > SCENEONE_STARTTIME && t <= SCENETWO_STARTTIME)
    {
        #if LOAD_IntroScene
        if (!s_intro_freed) IntroScene_deactivate();
        #endif
        #if LOAD_MumbaiDarshan
        MumbaiDarshan_begin();
        if (E_FLAG_ANIMATIONS) MumbaiDarshan_updateWorld();
        MumbaiDarshan_updateCamera();
        #endif
    }
    else if (t > SCENETWO_STARTTIME && t <= SCENETHREE_STARTTIME)
    {
        #if LOAD_MumbaiDarshan
        if (!s_mumbai_freed) MumbaiDarshan_deactivate();
        #endif
        #if LOAD_CSTInterior
        CSTInterior_begin();
        if (E_FLAG_ANIMATIONS) CSTInterior_updateWorld();
        CSTInterior_updateCamera();
        #endif
    }
    else if (t > SCENETHREE_STARTTIME && t <= SCENEFOUR_STARTTIME)
    {
        #if LOAD_CSTInterior
        if (!s_cst_freed) CSTInterior_deactivate();
        #endif
        #if LOAD_TajDinning
        TajDinning_begin();
        if (E_FLAG_ANIMATIONS) TajDinning_updateWorld();
        TajDinning_updateCamera();
        #endif
    }
    else if (t > SCENEFOUR_STARTTIME && t <= SCENEFIVE_STARTTIME)
    {
        #if LOAD_TajDinning
        if (!s_taj_freed) TajDinning_deactivate();
        #endif
        #if LOAD_BedroomScene
        BedroomScene_begin();
        if (E_FLAG_ANIMATIONS) BedroomScene_updateWorld();
        BedroomScene_updateCamera();
        #endif
    }
    else if (t > SCENEFIVE_STARTTIME && t <= SCENESIX_STARTTIME)
    {
        #if LOAD_BedroomScene
        if (!s_bedroom_freed) BedroomScene_deactivate();
        #endif
        #if LOAD_CamaHospital
        CamaHospital_begin();
        if (E_FLAG_ANIMATIONS) CamaHospital_updateWorld();
        CamaHospital_updateCamera();
        #endif
    }
    else if (t > SCENESIX_STARTTIME)
    {
        #if LOAD_CamaHospital
        if (!s_cama_freed) CamaHospital_deactivate();
        #endif
        #if LOAD_EndMessageAndCredits
        EndMessageAndCredits_begin();
        if (E_FLAG_ANIMATIONS) EndMessageAndCredits_updateWorld();
        EndMessageAndCredits_updateCamera();
        #endif
    }
}

void DEMO_render()
{
    if (!g_readyToStart) return;

    E_scene_render(&engineScene);
}

void DEMO_uninit()
{
    mciSendStringA("stop demoaudio", NULL, 0, NULL);
    mciSendStringA("close demoaudio", NULL, 0, NULL);
    AMCIntro_uninit();
    IntroScene_uninit();
    MumbaiDarshan_uninit();
    CSTInterior_uninit();
    TajDinning_uninit();
    BedroomScene_uninit();
    CamaHospital_uninit();
    EndMessageAndCredits_uninit();
    E_scene_destroy(&engineScene);
}
