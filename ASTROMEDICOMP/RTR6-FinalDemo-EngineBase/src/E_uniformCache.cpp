#include "E_uniformCache.h"
#include <string.h>

#define CACHE_CAPACITY 8192  // must be power of 2
#define CACHE_MASK     (CACHE_CAPACITY - 1)
#define NAME_MAX_LEN   64

typedef struct {
    GLuint prog;
    char   name[NAME_MAX_LEN];
    GLint  loc;
    int    used;
} CacheEntry;

static CacheEntry s_cache[CACHE_CAPACITY];

static unsigned int hashKey(GLuint prog, const char* name)
{
    unsigned int h = prog * 2654435761u;
    for (const char* p = name; *p; p++)
        h = h * 31u + (unsigned char)*p;
    return h;
}

GLint E_getUniformLocation(GLuint prog, const char* name)
{
    unsigned int slot = hashKey(prog, name) & CACHE_MASK;

    for (int i = 0; i < CACHE_CAPACITY; i++)
    {
        CacheEntry* e = &s_cache[slot];
        if (!e->used)
        {
            e->used = 1;
            e->prog = prog;
            strncpy(e->name, name, NAME_MAX_LEN - 1);
            e->name[NAME_MAX_LEN - 1] = '\0';
            e->loc  = glGetUniformLocation(prog, name);
            return e->loc;
        }
        if (e->prog == prog && strcmp(e->name, name) == 0)
            return e->loc;

        slot = (slot + 1) & CACHE_MASK;
    }

    return glGetUniformLocation(prog, name);
}
