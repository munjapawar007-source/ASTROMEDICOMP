#pragma once
#include "E_light.h"
#include "E_scene.h"
#include "vmath.h"

E_spotLight E_spotLight_create(vmath::vec3 pos, vmath::vec3 dir, vmath::vec3 color,
                               float intensity,
                               float innerDeg, float outerDeg,
                               float range);

void E_spotLight_setName      (E_spotLight* sl, const char* name);
void E_spotLight_setPosition  (E_spotLight* sl, vmath::vec3 pos);
void E_spotLight_setDirection (E_spotLight* sl, vmath::vec3 dir);
void E_spotLight_setColor     (E_spotLight* sl, vmath::vec3 color);
void E_spotLight_setIntensity (E_spotLight* sl, float intensity);
void E_spotLight_setConeAngles(E_spotLight* sl, float innerDeg, float outerDeg);
void E_spotLight_setRange     (E_spotLight* sl, float range);
void E_spotLight_activate     (E_spotLight* sl);
void E_spotLight_deactivate   (E_spotLight* sl);

E_spotLight* E_spotLight_addToScene        (E_scene* scene, E_spotLight light);
void         E_spotLight_removeFromScene   (E_scene* scene, E_spotLight* light);

