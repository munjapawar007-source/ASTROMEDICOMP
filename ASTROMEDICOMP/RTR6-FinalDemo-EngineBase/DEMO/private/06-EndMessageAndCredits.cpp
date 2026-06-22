#include "../public/06-EndMessageAndCredits.h"
#include "../public/DEMO.hpp"
#include "engineAPIs.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "E_utils.h"
#include <GL/glew.h>

#define SLIDE_COUNT 44
#define UNCOMMON_SLIDE_INDEX       7
#define THE_TEAM_SLIDE_INDEX      30
#define UNCOMMON_FADE_OUT_START  579.0f
#define UNCOMMON_FADE_OUT_END    581.0f
#define SHADER_STORY_FADE_IN_START 588.5f
#define SHADER_STORY_FADE_IN_END   590.5f

static GLuint s_tex[SLIDE_COUNT] = {0};

static const char* s_slideFiles[SLIDE_COUNT] = {
    "DEMO_ASSETS/06-EndMessageAndCredits/Message/sec 1.1 - lives were being taken.jpg",
    "DEMO_ASSETS/06-EndMessageAndCredits/Message/sec 1.2 - bring 1 in.jpg",
    "DEMO_ASSETS/06-EndMessageAndCredits/Message/sec 1.3 - way out.jpg",
    "DEMO_ASSETS/06-EndMessageAndCredits/Message/sec 1.4 - stayed.jpg",
    "DEMO_ASSETS/06-EndMessageAndCredits/Message/sec 1.5 - ordinary.jpg",
    "DEMO_ASSETS/06-EndMessageAndCredits/Message/sec 1.6 - extraordinary.jpg",
    "DEMO_ASSETS/06-EndMessageAndCredits/Message/sec 1.7 - rises.jpg",
    "DEMO_ASSETS/06-EndMessageAndCredits/Message/sec 1.8 - uncommon.jpg",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 1, a shader story.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 2, music & score, bambai.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 3, music & score, train crash.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 4, music & score, mole.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 5, music & score, why so serious.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 6, music & score, call of duty.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 7, source materials, opengl programming guide.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 8, source materials, learn opengl, cherno.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 9, core stack .png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 10, tools.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 11, algo & tech, barrel.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 12, algo & tech, catmull.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 13, algo & tech, gertsner.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 14, algo & tech, lights and shadows.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 15, special thanks, rajib.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 16, special thanks, annapure.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 17, motion capture artist.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 18, guidance, rama gokhale.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 19, guidance, pradnya gokhale.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 20, guidance, yogeshwar.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 21, guidance, radhika.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 22, and finally.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 23, the team.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/sec 2 - 24, the team shaders.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/AB.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/DC.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/MP.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/NJ.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/RG.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/SK.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/TP.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/VG.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/YT.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/KV.jpeg",
    "DEMO_ASSETS/06-EndMessageAndCredits/bornFromthevision.png",
    "DEMO_ASSETS/06-EndMessageAndCredits/SirImage.png",
};

static const float s_slideStart[SLIDE_COUNT] = {
    540.0f,  // 9:00  - While lives were being taken
    545.0f,  // 9:05  - they were fighting to bring one in
    553.0f,  // 9:13  - They knew every way out
    557.0f,  // 9:17  - They stayed.
    562.0f,  // 9:22  - From the most ordinary people
    566.0f,  // 9:26  - in the most extraordinary moments,
    570.0f,  // 9:30  - rises the...
    575.0f,  // 9:35  - UNCOMMON
    588.0f,  // 9:48.5 - A shader story
    597.0f,  // 9:57  - Music & score, Bambai
    603.0f,  // 10:03  - Music & score, Train Crash
    609.0f,  // 10:09  - Music & score, Mole
    613.0f,  // 10:13 - Music & score, Why So Serious
    617.0f,  // 10:17 - Music & score, Call of Duty
    623.0f,  // 10:23 - Source materials, OpenGL Programming Guide
    627.0f,  // 10:27 - Source materials, Learn OpenGL, Cherno
    630.0f,  // 10:30 - Core stack
    635.0f,  // 10:35 - Tools
    639.0f,  // 10:39 - Algo & tech, Barrel
    642.0f,  // 10:42 - Algo & tech, Catmull
    646.0f,  // 10:46 - Algo & tech, Gertsner
    649.0f,  // 10:49 - Algo & tech, Lights and shadows
    653.0f,  // 10:53 - Special thanks, Rajib
    657.0f,  // 10:57 - Special thanks, Annapure
    660.0f,  // 11:00 - Motion capture artist
    664.0f,  // 11:04 - Guidance, Rama Gokhale
    668.0f,  // 11:08 - Guidance, Pradnya Gokhale
    674.0f,  // 11:14 - Guidance, Yogeshwar
    678.0f,  // 11:18 - Guidance, Radhika
    682.0f,  // 11:22 - And finally
    685.0f,  // 11:25 - The team
    687.5f,  // 11:27.5 - The team shaders
    692.0f,  // 11:31 - AB
    697.0f,  // 11:37 - DC
    701.0f,  // 11:41 - MP
    707.0f,  // 11:47 - NJ
    713.0f,  // 11:53 - RG
    717.0f,  // 11:57 - SK
    721.0f,  // 12:01 - TP
    726.0f,  // 12:06 - VG
    733.0f,  // 12:13 - YT
    741.0f,  // 12:21 - KV
    749.0f,  // 12:29 - bornFromthevision
    755.0f,  // 12:35 - SirImage
};

static const float s_slideEnd = 763.5f; // 12:43.5

void EndMessageAndCredits_init()
{
    for (int i = 0; i < SLIDE_COUNT; i++)
        s_tex[i] = E_loadTexture(s_slideFiles[i]);
}

void EndMessageAndCredits_begin()
{
    static int s_begun = 0;
    if (s_begun) return;
    s_begun = 1;

    E_postProcess_setDOF      (0, 0.0f, 0.0f, 0.0f);
    E_postProcess_setGodRays  (0, 0.5f, 0.5f, 100, 0.0f, 0.0f, 0.0f, 0.0f);
    E_postProcess_setBloom    (0, 0.0f, 0.0f);
    E_postProcess_setFadeColor(0.0f, 0.0f, 0.0f);
    E_fog_setEnabled(&engineScene.fog, 0);
    E_postProcess_setGamma(0.9f);
}

void EndMessageAndCredits_activate()  {}
void EndMessageAndCredits_deactivate(){}

void EndMessageAndCredits_updateWorld()
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    int active = -1;
    for (int i = 0; i < SLIDE_COUNT; i++)
    {
        float end = (i < SLIDE_COUNT - 1) ? s_slideStart[i + 1] : s_slideEnd;
        if (now >= s_slideStart[i] && now < end) { active = i; break; }
    }

    if (active < 0)
    {
        E_postProcess_setOverlay(0, 0);
        return;
    }

    E_postProcess_setOverlay(1, s_tex[active]);

    E_postProcess_transitionBetweenTime(s_slideStart[0], s_slideStart[0] + 2.0f, E_TRANSITION_START);

    // Cross-fade at each slide boundary (1s fade out + 1s fade in)
    for (int i = 0; i < SLIDE_COUNT - 1; i++)
    {
        if (i == UNCOMMON_SLIDE_INDEX)
        {
            E_postProcess_transitionBetweenTime(UNCOMMON_FADE_OUT_START, UNCOMMON_FADE_OUT_END, E_TRANSITION_END);
            E_postProcess_transitionBetweenTime(SHADER_STORY_FADE_IN_START, SHADER_STORY_FADE_IN_END, E_TRANSITION_START);    
        }
        else if (i == THE_TEAM_SLIDE_INDEX)
        {
            continue;
        }
        else
        {
            E_postProcess_transitionBetweenTime(s_slideStart[i + 1] - 1.0f, s_slideStart[i + 1] + 1.0f, E_TRANSITION_MID);
        }
    }

    // Fade out at the end
    E_postProcess_transitionBetweenTime(s_slideEnd - 2.0f, s_slideEnd, E_TRANSITION_END);
}

void EndMessageAndCredits_updateCamera()
{
    E_camera_setPosition  (0.00f, 0.00f, 157.30f);
    E_camera_setProjection(68.50f, 0.10f, 1000.00f);
}

void EndMessageAndCredits_update() { EndMessageAndCredits_updateWorld(); EndMessageAndCredits_updateCamera(); }

void EndMessageAndCredits_uninit()
{
    for (int i = 0; i < SLIDE_COUNT; i++)
    {
        if (s_tex[i]) { glDeleteTextures(1, &s_tex[i]); s_tex[i] = 0; }
    }
}
