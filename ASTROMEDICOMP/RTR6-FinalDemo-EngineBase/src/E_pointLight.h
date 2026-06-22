#pragma once
#include "E_light.h"
#include "E_scene.h"
#include "vmath.h"

E_pointLight E_pointLight_create(vmath::vec3 pos, vmath::vec3 color,
                                 float intensity, float range);

void E_pointLight_setName     (E_pointLight* pl, const char* name);
void E_pointLight_setPosition (E_pointLight* pl, vmath::vec3 pos);
void E_pointLight_setColor    (E_pointLight* pl, vmath::vec3 color);
void E_pointLight_setIntensity(E_pointLight* pl, float intensity);
void E_pointLight_setRange    (E_pointLight* pl, float range);
void E_pointLight_activate    (E_pointLight* pl);
void E_pointLight_deactivate  (E_pointLight* pl);

E_pointLight* E_pointLight_addToScene     (E_scene* scene, E_pointLight light);
void          E_pointLight_removeFromScene(E_scene* scene, E_pointLight* light);

