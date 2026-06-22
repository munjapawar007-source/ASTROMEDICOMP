#include "E_spotLight.h"
#include "E_camera.h"
#include "vmath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

E_spotLight E_spotLight_create(vmath::vec3 pos, vmath::vec3 dir, vmath::vec3 color,
                               float intensity,
                               float innerDeg, float outerDeg,
                               float range)
{
    if (innerDeg > outerDeg) outerDeg = innerDeg;
    E_spotLight sl      = {};
    sl.active           = 1;
    sl.position  = pos;
    sl.direction = dir;
    sl.color     = color;
    sl.intensity        = intensity;
    sl.baseIntensity    = intensity;
    sl.cutOff           = cosf(vmath::radians(innerDeg));
    sl.outerCutOff      = cosf(vmath::radians(outerDeg));
    sl.range            = range;
    sl.castShadow       = 0;
    sl.flicker          = 0;
    sl.flickerSpeed     = 3.0f;
    sl.flickerAmount    = 0.5f;
    snprintf(sl.name, sizeof(sl.name), "spotLight");
    return sl;
}

void E_spotLight_setName(E_spotLight* sl, const char* name)
{
    snprintf(sl->name, sizeof(sl->name), "%s", name);
}

void E_spotLight_setPosition (E_spotLight* sl, vmath::vec3 pos)   { sl->position  = pos; }
void E_spotLight_setDirection(E_spotLight* sl, vmath::vec3 dir)   { sl->direction = dir; }
void E_spotLight_setColor    (E_spotLight* sl, vmath::vec3 color) { sl->color     = color; }

void E_spotLight_setIntensity(E_spotLight* sl, float intensity)
{
    sl->intensity = intensity;
}

void E_spotLight_setConeAngles(E_spotLight* sl, float innerDeg, float outerDeg)
{
    if (innerDeg > outerDeg) outerDeg = innerDeg;
    sl->cutOff      = cosf(vmath::radians(innerDeg));
    sl->outerCutOff = cosf(vmath::radians(outerDeg));
}

void E_spotLight_setRange(E_spotLight* sl, float range)
{
    sl->range = range;
}

void E_spotLight_activate(E_spotLight* sl)
{
    if (sl) sl->active = 1;
}

void E_spotLight_deactivate(E_spotLight* sl)
{
    if (sl) sl->active = 0;
}

E_spotLight* E_spotLight_addToScene(E_scene* scene, E_spotLight light)
{
    if (scene->spotLightCount == scene->spotLightCapacity)
    {
        scene->spotLightCapacity *= 2;
        scene->spotLights = (E_spotLight*)realloc(scene->spotLights, scene->spotLightCapacity * sizeof(E_spotLight));
    }
    int idx = scene->spotLightCount++;
    scene->spotLights[idx] = light;
    snprintf(scene->spotLights[idx].name, sizeof(scene->spotLights[idx].name), "spotLight_%d", idx);
    return &scene->spotLights[idx];
}

void E_spotLight_removeFromScene(E_scene* scene, E_spotLight* light)
{
    if (!scene || !light) return;
    int index = (int)(light - scene->spotLights);
    if (index < 0 || index >= scene->spotLightCount) return;
    for (int i = index; i < scene->spotLightCount - 1; i++)
        scene->spotLights[i] = scene->spotLights[i + 1];
    scene->spotLightCount--;
}

