#include "E_meshCache.h"
#include <string.h>
#include <stdio.h>

static E_meshCacheEntry s_cache[E_MESH_CACHE_MAX];

E_meshCacheEntry* E_meshCache_get(const char* key)
{
    for (int i = 0; i < E_MESH_CACHE_MAX; i++)
    {
        if (s_cache[i].key[0] != '\0' && strcmp(s_cache[i].key, key) == 0)
            return &s_cache[i];
    }
    return NULL;
}

E_meshCacheEntry* E_meshCache_insert(const char* key,
    GLuint vao, GLuint vbo_pos, GLuint vbo_col, GLuint vbo_nor,
    GLuint vbo_tex, GLuint vbo_jnt, GLuint vbo_wgt, GLuint ebo,
    int vertexCount, int indexCount,
    float* boundCenter, float boundRadius, int noCull)
{
    for (int i = 0; i < E_MESH_CACHE_MAX; i++)
    {
        if (s_cache[i].key[0] != '\0') continue;

        E_meshCacheEntry* e = &s_cache[i];
        snprintf(e->key, sizeof(e->key), "%s", key);
        e->vao          = vao;
        e->vbo_position = vbo_pos;
        e->vbo_color    = vbo_col;
        e->vbo_normal   = vbo_nor;
        e->vbo_texcoord = vbo_tex;
        e->vbo_joints   = vbo_jnt;
        e->vbo_weights  = vbo_wgt;
        e->ebo          = ebo;
        e->vertexCount  = vertexCount;
        e->indexCount   = indexCount;
        e->boundCenter[0] = boundCenter[0];
        e->boundCenter[1] = boundCenter[1];
        e->boundCenter[2] = boundCenter[2];
        e->boundRadius    = boundRadius;
        e->noCull         = noCull;
        e->refCount       = 1;
        return e;
    }
    printf("[MeshCache] WARNING: cache full (%d entries), mesh won't be cached\n", E_MESH_CACHE_MAX);
    return NULL;
}

void E_meshCache_release(E_meshCacheEntry* entry)
{
    if (!entry) return;
    entry->refCount--;
    if (entry->refCount > 0) return;

    if (entry->vao)          { glDeleteVertexArrays(1, &entry->vao);     entry->vao          = 0; }
    if (entry->vbo_position) { glDeleteBuffers(1, &entry->vbo_position); entry->vbo_position = 0; }
    if (entry->vbo_color)    { glDeleteBuffers(1, &entry->vbo_color);    entry->vbo_color    = 0; }
    if (entry->vbo_normal)   { glDeleteBuffers(1, &entry->vbo_normal);   entry->vbo_normal   = 0; }
    if (entry->vbo_texcoord) { glDeleteBuffers(1, &entry->vbo_texcoord); entry->vbo_texcoord = 0; }
    if (entry->vbo_joints)   { glDeleteBuffers(1, &entry->vbo_joints);   entry->vbo_joints   = 0; }
    if (entry->vbo_weights)  { glDeleteBuffers(1, &entry->vbo_weights);  entry->vbo_weights  = 0; }
    if (entry->ebo)          { glDeleteBuffers(1, &entry->ebo);          entry->ebo          = 0; }

    entry->key[0] = '\0';  // mark slot as free
}

void E_meshCache_clear(void)
{
    for (int i = 0; i < E_MESH_CACHE_MAX; i++)
    {
        if (s_cache[i].key[0] != '\0')
        {
            s_cache[i].refCount = 1;  // ensure release actually fires
            E_meshCache_release(&s_cache[i]);
        }
    }
}
