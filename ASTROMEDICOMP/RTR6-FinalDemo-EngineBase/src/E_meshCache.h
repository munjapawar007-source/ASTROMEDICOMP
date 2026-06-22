#pragma once
#include <GL/glew.h>

#define E_MESH_CACHE_MAX 4096

typedef struct {
    char   key[384];            // "path::meshName"  empty key means slot is free
    GLuint vao;
    GLuint vbo_position;
    GLuint vbo_color;
    GLuint vbo_normal;
    GLuint vbo_texcoord;
    GLuint vbo_joints;
    GLuint vbo_weights;
    GLuint ebo;
    int    vertexCount;
    int    indexCount;
    float  boundCenter[3];
    float  boundRadius;
    int    noCull;
    int    refCount;
} E_meshCacheEntry;

// Returns existing entry for this key, or NULL if not cached.
E_meshCacheEntry* E_meshCache_get(const char* key);

// Stores a new entry (refCount=1). Returns NULL if cache is full (mesh still works, just not cached).
E_meshCacheEntry* E_meshCache_insert(const char* key,
    GLuint vao, GLuint vbo_pos, GLuint vbo_col, GLuint vbo_nor,
    GLuint vbo_tex, GLuint vbo_jnt, GLuint vbo_wgt, GLuint ebo,
    int vertexCount, int indexCount,
    float* boundCenter, float boundRadius, int noCull);

// Decrements refCount. Deletes GL resources and frees the slot when it reaches 0.
void E_meshCache_release(E_meshCacheEntry* entry);

// Force-releases all entries. Call once at engine shutdown.
void E_meshCache_clear(void);
