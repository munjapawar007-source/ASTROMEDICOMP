#pragma once
#include "E_light.h"
#include "vmath.h"

void E_directionalLight_setDirection(E_directionalLight* light, vmath::vec3 dir);
void E_directionalLight_setColor    (E_directionalLight* light, vmath::vec3 color);
void E_directionalLight_setIntensity(E_directionalLight* light, float intensity);
