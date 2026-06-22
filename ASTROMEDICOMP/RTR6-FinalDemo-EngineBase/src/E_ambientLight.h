#pragma once
#include "E_light.h"
#include "vmath.h"

void E_ambientLight_setColor    (E_ambientLight* light, vmath::vec3 color);
void E_ambientLight_setIntensity(E_ambientLight* light, float intensity);
