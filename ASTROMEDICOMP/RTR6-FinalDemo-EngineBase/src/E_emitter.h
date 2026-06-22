#pragma once
#include "E_particles.h"
#include "E_scene.h"
#include "vmath.h"

void E_emitter_setPosition   (E_emitter* e, vmath::vec3 pos);
void E_emitter_setDirection  (E_emitter* e, vmath::vec3 dir);
void E_emitter_setSpread     (E_emitter* e, float degrees);
void E_emitter_setSpawnRate  (E_emitter* e, float rate);
void E_emitter_setLifespan   (E_emitter* e, float lifespan);
void E_emitter_setSpeed      (E_emitter* e, float speed);
void E_emitter_setSize       (E_emitter* e, float size, float sizeGrowth);
void E_emitter_setName       (E_emitter* e, const char* name);
void E_emitter_setColor      (E_emitter* e, float r, float g, float b, float a);
void E_emitter_setOpacity    (E_emitter* e, float opacity);
void E_emitter_setPan        (E_emitter* e, float dirX, float dirY, float speed);
void E_emitter_setTextureType(E_emitter* e, E_ParticleTextureType type);
void E_emitter_activate      (E_emitter* e);
void E_emitter_deactivate    (E_emitter* e);

E_emitter* E_emitter_addToScene     (E_scene* scene, int maxParticles);
void       E_emitter_removeFromScene(E_scene* scene, E_emitter* e);

