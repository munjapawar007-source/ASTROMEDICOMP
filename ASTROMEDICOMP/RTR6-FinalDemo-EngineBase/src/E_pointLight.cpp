#include "E_pointLight.h"
#include "E_camera.h"
#include <stdio.h>
#include <stdlib.h>

E_pointLight E_pointLight_create(vmath::vec3 pos, vmath::vec3 color,
                                 float intensity, float range)
{
    E_pointLight pl = {};
    pl.active        = 1;
    pl.position      = pos;
    pl.color         = color;
    pl.intensity      = intensity;
    pl.baseIntensity  = intensity;
    pl.range          = range;
    pl.applyAttenuation = 1;
    pl.flicker          = 0;
    pl.flickerSpeed     = 3.0f;
    pl.flickerAmount    = 0.5f;
    snprintf(pl.name, sizeof(pl.name), "pointLight");
    return pl;
}

void E_pointLight_setName(E_pointLight* pl, const char* name)
{
    snprintf(pl->name, sizeof(pl->name), "%s", name);
}

void E_pointLight_setPosition(E_pointLight* pl, vmath::vec3 pos)   { pl->position = pos; }
void E_pointLight_setColor   (E_pointLight* pl, vmath::vec3 color) { pl->color    = color; }

void E_pointLight_setIntensity(E_pointLight* pl, float intensity)
{
    pl->intensity = intensity;
}

void E_pointLight_setRange(E_pointLight* pl, float range)
{
    pl->range = range;
}

void E_pointLight_activate(E_pointLight* pl)
{
    if (pl) pl->active = 1;
}

void E_pointLight_deactivate(E_pointLight* pl)
{
    if (pl) pl->active = 0;
}

E_pointLight* E_pointLight_addToScene(E_scene* scene, E_pointLight light)
{
    if (scene->pointLightCount == scene->pointLightCapacity)
    {
        scene->pointLightCapacity *= 2;
        scene->pointLights = (E_pointLight*)realloc(scene->pointLights, scene->pointLightCapacity * sizeof(E_pointLight));
    }
    int idx = scene->pointLightCount++;
    scene->pointLights[idx] = light;
    snprintf(scene->pointLights[idx].name, sizeof(scene->pointLights[idx].name), "pointLight_%d", idx);
    return &scene->pointLights[idx];
}

void E_pointLight_removeFromScene(E_scene* scene, E_pointLight* light)
{
    if (!scene || !light) return;
    int index = (int)(light - scene->pointLights);
    if (index < 0 || index >= scene->pointLightCount) return;
    for (int i = index; i < scene->pointLightCount - 1; i++)
        scene->pointLights[i] = scene->pointLights[i + 1];
    scene->pointLightCount--;
}

