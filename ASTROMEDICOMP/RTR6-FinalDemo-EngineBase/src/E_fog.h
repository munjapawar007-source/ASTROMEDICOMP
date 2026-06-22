#pragma once
#include "vmath.h"

typedef struct
{
    int   enabled;
    float color[3];
    float density;
    int   type;     // 0 = depth, 1 = height
    float height;
    float falloff;
} E_fogProperties;

void E_fog_setEnabled (E_fogProperties* fog, int enabled);
void E_fog_setColor   (E_fogProperties* fog, vmath::vec3 color);
void E_fog_setDensity (E_fogProperties* fog, float density);
void E_fog_setType    (E_fogProperties* fog, int type);
void E_fog_setHeight  (E_fogProperties* fog, float height, float falloff);

