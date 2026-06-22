#pragma once
#include "vmath.h"

#define E_SHADER_MAX_LIGHTS 48  // max lights sent to the shader per object

typedef struct
{
    vmath::vec3 direction;
    vmath::vec3 color;
    float intensity;
} E_directionalLight;

typedef struct
{
    vmath::vec3 color;
    float intensity;
} E_ambientLight;

typedef struct
{
    vmath::vec3 position;
    vmath::vec3 direction;
    vmath::vec3 color;
    float intensity;
    float baseIntensity;
    float cutOff;
    float outerCutOff;
    float range;
    int   active;
    int   castShadow;
    int   flicker;
    float flickerSpeed;
    float flickerAmount;
    char  name[64];
} E_spotLight;

typedef struct
{
    vmath::vec3 position;
    vmath::vec3 color;
    float intensity;
    float baseIntensity;
    float range;
    int   active;
    int   applyAttenuation;
    int   flicker;
    float flickerSpeed;
    float flickerAmount;
    char  name[64];
} E_pointLight;
