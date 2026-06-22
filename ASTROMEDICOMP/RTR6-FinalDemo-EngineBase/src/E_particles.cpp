#include "E_particles.h"
#include "E_shader.h"
#include "E_camera.h"
#include "E_utils.h"
#include "E_material.h"
#include "E_fog.h"
#include "E_uniformCache.h"
#include "DEMO.hpp"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static GLuint s_prog    = 0;
static GLuint s_quadVBO = 0;

GLuint smokeParticleTexture = 0;
GLuint fireParticleTexture    = 0;

// instance data per frame
#define E_PARTICLE_SCRATCH_MAX 4096
static float s_scratch[E_PARTICLE_SCRATCH_MAX * 4]; // pos.xyz + lifeRatio

static const char* s_vs =
    "#version 460 core\n"
    "layout(location = 0) in vec2  aQuadVertex;\n"
    "layout(location = 1) in vec3  aWorldPos;\n"
    "layout(location = 2) in float aLifeRatio;\n"
    "uniform mat4  uView;\n"
    "uniform mat4  uProjection;\n"
    "uniform float uSize;\n"
    "uniform float uSizeGrowth;\n"
    "out vec2  v_uv;\n"
    "out float v_lifeRatio;\n"
    "out float v_eyeDist;\n"
    "out float v_worldY;\n"
    "void main(void) {\n"
    "    vec3  camRight = vec3(uView[0][0], uView[1][0], uView[2][0]);\n"
    "    vec3  camUp    = vec3(uView[0][1], uView[1][1], uView[2][1]);\n"
    "    float size     = mix(uSize, uSize * uSizeGrowth, aLifeRatio);\n"
    "    vec3  worldPos = aWorldPos + (aQuadVertex.x * camRight + aQuadVertex.y * camUp) * size;\n"
    "    vec4  eyePos   = uView * vec4(worldPos, 1.0);\n"
    "    gl_Position    = uProjection * eyePos;\n"
    "    v_uv           = aQuadVertex + vec2(0.5);\n"
    "    v_lifeRatio    = aLifeRatio;\n"
    "    v_eyeDist      = length(eyePos.xyz);\n"
    "    v_worldY       = aWorldPos.y;\n"
    "}\n";

static const char* s_fs =
    "#version 460 core\n"
    "in vec2  v_uv;\n"
    "in float v_lifeRatio;\n"
    "in float v_eyeDist;\n"
    "in float v_worldY;\n"
    "uniform sampler2D uTexture;\n"
    "uniform vec4      uColor;\n"
    "uniform float     uOpacity;\n"
    "uniform vec2      uPanDir;\n"
    "uniform float     uPanSpeed;\n"
    "uniform int       ufogEnabled;\n"
    "uniform vec3      ufogColor;\n"
    "uniform float     ufogDensity;\n"
    "uniform int       uFogType;\n"
    "uniform float     uFogHeight;\n"
    "uniform float     uFogFalloff;\n"
    "uniform int       uIsFireParticle;\n"
    "layout(location = 0) out vec4 fragColor;\n"
    "layout(location = 1) out vec4 emissiveOut;\n"
    "layout(location = 2) out vec4 godRayOut;\n"
    "void main(void) {\n"
    "    vec2  pannedUV = v_uv + uPanDir * uPanSpeed * v_lifeRatio;\n"
    "    vec4  tex      = texture(uTexture, pannedUV);\n"
    "    float mask     = texture(uTexture, v_uv).a;\n"
    "    float alpha    = uColor.a * uOpacity * mask * tex.a * (1.0 - v_lifeRatio);\n"
    "    fragColor      = vec4(uColor.rgb * tex.rgb, alpha);\n"
    "    if (ufogEnabled == 1) {\n"
    "        float fogAmount;\n"
    "        if (uFogType == 0)\n"
    "            fogAmount = clamp(1.0 - exp(-ufogDensity * v_eyeDist), 0.0, 1.0);\n"
    "        else\n"
    "            fogAmount = clamp(1.0 - exp(-max(uFogHeight - v_worldY, 0.0) * uFogFalloff), 0.0, 1.0);\n"
    "        fragColor.rgb = mix(fragColor.rgb, ufogColor, fogAmount);\n"
    "        fragColor.a  *= (1.0 - fogAmount);\n"
    "    }\n"
    "    emissiveOut = (uIsFireParticle != 0) ? fragColor : vec4(0.0);\n"
    "    godRayOut   = vec4(0.0);\n"
    "}\n";

// unit quad in XY plane, 6 verts (2 triangles)
static const float s_quadVerts[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f, -0.5f,
     0.5f,  0.5f,
    -0.5f,  0.5f
};

void E_particle_renderer_init(void)
{
    GLuint vs = E_compileShader(s_vs, GL_VERTEX_SHADER);
    GLuint fs = E_compileShader(s_fs, GL_FRAGMENT_SHADER);
    s_prog    = E_createProgram(vs, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    glGenBuffers(1, &s_quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, s_quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(s_quadVerts), s_quadVerts, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    smokeParticleTexture = E_loadTexture("engineResources/textures/smoke.png");
    fireParticleTexture    = E_loadTexture("engineResources/textures/fire.png");
}

// helpers

static vmath::vec3 randomInCone(vmath::vec3 axis, float halfAngle)
{
    float theta  = ((float)rand() / (float)RAND_MAX) * 6.28318f;
    float phi    = ((float)rand() / (float)RAND_MAX) * halfAngle;
    float sinPhi = sinf(phi), cosPhi = cosf(phi);
    float cosT   = cosf(theta), sinT  = sinf(theta);

    vmath::vec3 up(0.0f, 1.0f, 0.0f);
    if (fabsf(axis[1]) > 0.99f) up = vmath::vec3(1.0f, 0.0f, 0.0f);

    vmath::vec3 right = vmath::cross(axis, up);
    float rlen = vmath::length(right);
    if (rlen < 0.0001f) return axis;
    right = right / rlen;

    vmath::vec3 perp = vmath::cross(right, axis);

    return axis * cosPhi + (right * cosT + perp * sinT) * sinPhi;
}

// emitter

void E_emitter_init(E_emitter* e, int maxParticles)
{
    memset(e, 0, sizeof(E_emitter));

    e->position[1]      = 0.0f;
    e->direction[1]     = 1.0f;  // default: upward
    e->spread           = 0.3f;
    e->spawnRate        = 10.0f;
    e->lastSpawnTime    = -1.0f;
    e->particleLifespan = 2.0f;
    e->particleSpeed    = 3.0f;
    e->particleSize     = 0.2f;
    e->color[0] = e->color[1] = e->color[2] = e->color[3] = 1.0f;
    e->opacity  = 1.0f;
    e->active           = 1;

    e->maxParticles = maxParticles;
    e->particles    = (E_particle*)malloc(maxParticles * sizeof(E_particle));
    for (int i = 0; i < maxParticles; i++)
        e->particles[i].spawnTime = -1.0f;

    e->sizeGrowth  = 3.0f;
    e->panDir[0]   = 0.0f;
    e->panDir[1]   = 1.0f;
    e->panSpeed    = 0.5f;

    // instance VBO: 4 floats per particle (pos.xyz + lifeRatio)
    glGenBuffers(1, &e->instanceVBO);
    glBindBuffer(GL_ARRAY_BUFFER, e->instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, maxParticles * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

    // VAO
    glGenVertexArrays(1, &e->vao);
    glBindVertexArray(e->vao);

    // attr 0: quad vertices, divisor 0
    glBindBuffer(GL_ARRAY_BUFFER, s_quadVBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribDivisor(0, 0);

    // attr 1: per-instance world pos (xyz), attr 2: lifeRatio (w), stride = 4 floats
    glBindBuffer(GL_ARRAY_BUFFER, e->instanceVBO);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void E_emitter_update(E_emitter* e)
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    if (e->lastSpawnTime < 0.0f)
        e->lastSpawnTime = now;

    // kill expired
    for (int i = 0; i < e->maxParticles; i++)
    {
        E_particle* p = &e->particles[i];
        if (p->spawnTime >= 0.0f && (now - p->spawnTime) >= p->lifespan)
            p->spawnTime = -1.0f;
    }

    // spawn new
    float interval = (e->spawnRate > 0.0f) ? 1.0f / e->spawnRate : 1e9f;
    while (e->lastSpawnTime + interval <= now)
    {
        e->lastSpawnTime += interval;

        for (int i = 0; i < e->maxParticles; i++)
        {
            if (e->particles[i].spawnTime < 0.0f)
            {
                E_particle* p = &e->particles[i];
                p->spawnPosition[0] = e->position[0];
                p->spawnPosition[1] = e->position[1];
                p->spawnPosition[2] = e->position[2];
                vmath::vec3 dir = randomInCone(vmath::vec3(e->direction[0], e->direction[1], e->direction[2]), e->spread) * e->particleSpeed;
                p->velocity[0] = dir[0];
                p->velocity[1] = dir[1];
                p->velocity[2] = dir[2];
                p->spawnTime   = e->lastSpawnTime;
                p->lifespan    = e->particleLifespan;
                break;
            }
        }
    }
}

void E_emitter_render(E_emitter* e)
{
    float now = (float)E_Utils_getElapsedTimeInSeconds();

    int count = 0;
    for (int i = 0; i < e->maxParticles && count < E_PARTICLE_SCRATCH_MAX; i++)
    {
        E_particle* p = &e->particles[i];
        if (p->spawnTime < 0.0f) continue;
        float age = now - p->spawnTime;
        if (age >= p->lifespan) continue;

        s_scratch[count*4+0] = p->spawnPosition[0] + p->velocity[0] * age;
        s_scratch[count*4+1] = p->spawnPosition[1] + p->velocity[1] * age;
        s_scratch[count*4+2] = p->spawnPosition[2] + p->velocity[2] * age;
        s_scratch[count*4+3] = age / p->lifespan;
        count++;
    }
    if (count == 0) return;

    glBindBuffer(GL_ARRAY_BUFFER, e->instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, count * 4 * sizeof(float), s_scratch);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint presetTex = (e->textureType == E_PARTICLE_TEXTURE_FIRE) ? fireParticleTexture : smokeParticleTexture;
    GLuint tex = (e->texture != 0) ? e->texture : presetTex;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    glUseProgram(s_prog);
    glUniformMatrix4fv(E_getUniformLocation(s_prog, "uView"),       1, GL_FALSE, engineCamera.viewMatrix);
    glUniformMatrix4fv(E_getUniformLocation(s_prog, "uProjection"), 1, GL_FALSE, engineCamera.projectionMatrix);
    glUniform1f (E_getUniformLocation(s_prog, "uSize"),       e->particleSize);
    glUniform1f (E_getUniformLocation(s_prog, "uSizeGrowth"), e->sizeGrowth);
    glUniform4fv(E_getUniformLocation(s_prog, "uColor"),    1, e->color);
    glUniform1f (E_getUniformLocation(s_prog, "uOpacity"),    e->opacity);
    glUniform2fv(E_getUniformLocation(s_prog, "uPanDir"),   1, e->panDir);
    glUniform1f (E_getUniformLocation(s_prog, "uPanSpeed"),   e->panSpeed);
    glUniform1i (E_getUniformLocation(s_prog, "uTexture"), 0);
    glUniform1i (E_getUniformLocation(s_prog, "uIsFireParticle"), e->applyBloom);

    const E_fogProperties* fog = &engineScene.fog;
    glUniform1i (E_getUniformLocation(s_prog, "ufogEnabled"), fog->enabled);
    glUniform3fv(E_getUniformLocation(s_prog, "ufogColor"),   1, fog->color);
    glUniform1f (E_getUniformLocation(s_prog, "ufogDensity"), fog->density);
    glUniform1i (E_getUniformLocation(s_prog, "uFogType"),    fog->type);
    glUniform1f (E_getUniformLocation(s_prog, "uFogHeight"),  fog->height);
    glUniform1f (E_getUniformLocation(s_prog, "uFogFalloff"), fog->falloff);

    glBindVertexArray(e->vao);
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, count);
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void E_emitter_setTexture(E_emitter* e, GLuint texture)
{
    e->texture = texture;
}

void E_emitter_destroy(E_emitter* e)
{
    if (!e->particles && !e->vao) return;
    free(e->particles);
    glDeleteVertexArrays(1, &e->vao);
    glDeleteBuffers(1, &e->instanceVBO);
    memset(e, 0, sizeof(E_emitter));
}
