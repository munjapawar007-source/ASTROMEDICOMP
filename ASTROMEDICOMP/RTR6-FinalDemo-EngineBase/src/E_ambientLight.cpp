#include "E_ambientLight.h"
#include <stdio.h>

void E_ambientLight_setColor(E_ambientLight* light, vmath::vec3 color)
{
    light->color[0] = color[0];
    light->color[1] = color[1];
    light->color[2] = color[2];
}

void E_ambientLight_setIntensity(E_ambientLight* light, float intensity)
{
    light->intensity = intensity;
}

