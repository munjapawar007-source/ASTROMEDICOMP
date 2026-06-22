#include "E_emitter.h"
#include "E_camera.h"
#include "vmath.h"
#include <math.h>
#include <stdio.h>

void E_emitter_setPosition(E_emitter* e, vmath::vec3 pos)
{
    e->position[0] = pos[0];
    e->position[1] = pos[1];
    e->position[2] = pos[2];
}

void E_emitter_setDirection(E_emitter* e, vmath::vec3 dir)
{
    e->direction[0] = dir[0];
    e->direction[1] = dir[1];
    e->direction[2] = dir[2];
}

void E_emitter_setSpread(E_emitter* e, float degrees)
{
    e->spread = vmath::radians(degrees);
}

void E_emitter_setSpawnRate(E_emitter* e, float rate)
{
    e->spawnRate = rate;
}

void E_emitter_setLifespan(E_emitter* e, float lifespan)
{
    e->particleLifespan = lifespan;
}

void E_emitter_setSpeed(E_emitter* e, float speed)
{
    e->particleSpeed = speed;
}

void E_emitter_setSize(E_emitter* e, float size, float sizeGrowth)
{
    e->particleSize = size;
    e->sizeGrowth   = sizeGrowth;
}

void E_emitter_setColor(E_emitter* e, float r, float g, float b, float a)
{
    e->color[0] = r;
    e->color[1] = g;
    e->color[2] = b;
    e->color[3] = a;
}

void E_emitter_setName(E_emitter* e, const char* name)
{
    snprintf(e->name, sizeof(e->name), "%s", name);
}

void E_emitter_setOpacity(E_emitter* e, float opacity)
{
    e->opacity = opacity;
}

void E_emitter_setPan(E_emitter* e, float dirX, float dirY, float speed)
{
    e->panDir[0] = dirX;
    e->panDir[1] = dirY;
    e->panSpeed  = speed;
}

void E_emitter_setTextureType(E_emitter* e, E_ParticleTextureType type)
{
    e->textureType = (int)type;
    e->applyBloom  = (type == E_PARTICLE_TEXTURE_FIRE) ? 1 : 0;
}

void E_emitter_activate(E_emitter* e)
{
    if (e) e->active = 1;
}

void E_emitter_deactivate(E_emitter* e)
{
    if (e) e->active = 0;
}

E_emitter* E_emitter_addToScene(E_scene* scene, int maxParticles)
{
    if (scene->emitterCount >= E_EMITTER_MAX_COUNT) return NULL;
    int idx = scene->emitterCount++;
    E_emitter_init(&scene->emitters[idx], maxParticles);
    snprintf(scene->emitters[idx].name, sizeof(scene->emitters[idx].name), "emitter_%d", idx);
    return &scene->emitters[idx];
}

void E_emitter_removeFromScene(E_scene* scene, E_emitter* e)
{
    if (!scene || !e) return;
    int idx = (int)(e - scene->emitters);
    if (idx < 0 || idx >= scene->emitterCount) return;
    E_emitter_destroy(&scene->emitters[idx]);
    for (int i = idx; i < scene->emitterCount - 1; i++)
        scene->emitters[i] = scene->emitters[i + 1];
    scene->emitterCount--;
}

