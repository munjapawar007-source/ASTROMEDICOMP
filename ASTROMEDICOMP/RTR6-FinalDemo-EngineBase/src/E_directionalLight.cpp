#include "E_directionalLight.h"
#include <stdio.h>

void E_directionalLight_setDirection(E_directionalLight* light, vmath::vec3 dir)
{
    light->direction[0] = dir[0];
    light->direction[1] = dir[1];
    light->direction[2] = dir[2];
}

void E_directionalLight_setColor(E_directionalLight* light, vmath::vec3 color)
{
    light->color[0] = color[0];
    light->color[1] = color[1];
    light->color[2] = color[2];
}

void E_directionalLight_setIntensity(E_directionalLight* light, float intensity)
{
    light->intensity = intensity;
}

