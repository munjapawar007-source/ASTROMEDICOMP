#include "E_fog.h"
#include "vmath.h"
#include <stdio.h>

void E_fog_setEnabled(E_fogProperties* fog, int enabled)
{
    fog->enabled = enabled;
}

void E_fog_setColor(E_fogProperties* fog, vmath::vec3 color)
{
    fog->color[0] = color[0];
    fog->color[1] = color[1];
    fog->color[2] = color[2];
}

void E_fog_setDensity(E_fogProperties* fog, float density)
{
    fog->density = density;
}

void E_fog_setType(E_fogProperties* fog, int type)
{
    fog->type = type;
}

void E_fog_setHeight(E_fogProperties* fog, float height, float falloff)
{
    fog->height  = height;
    fog->falloff = falloff;
}

