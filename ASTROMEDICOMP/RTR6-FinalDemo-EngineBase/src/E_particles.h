#pragma once
#include <GL/glew.h>
#include <gl/GL.h>

#define E_EMITTER_MAX_COUNT 48

typedef enum
{
    E_PARTICLE_TEXTURE_SMOKE = 0,
    E_PARTICLE_TEXTURE_FIRE  = 1,
    E_PARTICLE_TEXTURE_COUNT = 2
} E_ParticleTextureType;

typedef struct
{
    float spawnPosition[3];
    float velocity[3];
    float spawnTime;
    float lifespan;
} E_particle;

typedef struct
{
    // Spawn transform
    float position[3];
    float direction[3];
    float spread;

    // Spawn settings
    float spawnRate; 
    float lastSpawnTime;

    // Per-particle settings
    float particleLifespan;
    float particleSpeed;
    float particleSize;
    float sizeGrowth;
    float panDir[2];
    float panSpeed;
    float color[4];
    float opacity;

    // Particle pool
    E_particle* particles;
    int         maxParticles;

    // GPU
    GLuint vao;
    GLuint instanceVBO; 
    GLuint texture;
    int    textureType;

    int  active;
    int  applyBloom;
    char name[64];
} E_emitter;

extern GLuint smokeParticleTexture;
extern GLuint fireParticleTexture;

void E_particle_renderer_init(void);   // compile shader, build shared quad VBO, load textures

void E_emitter_init      (E_emitter* e, int maxParticles);
void E_emitter_setTexture(E_emitter* e, GLuint texture);
void E_emitter_update    (E_emitter* e);
void E_emitter_render    (E_emitter* e);
void E_emitter_destroy   (E_emitter* e);
