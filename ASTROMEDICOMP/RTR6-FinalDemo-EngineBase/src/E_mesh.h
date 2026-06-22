#pragma once
#include <GL/glew.h>
#include <gl/GL.h>

typedef struct {
    GLuint vao;

    GLuint vbo_position;
    GLuint vbo_color;
    GLuint vbo_normal;
    GLuint vbo_texcoord;
    GLuint vbo_joints;
    GLuint vbo_weights;
    GLuint ebo;

    int indexCount;
    int vertexCount;

    float boundCenter[3];
    float boundRadius;
    int   noCull;        // 1 = always render, skip frustum test

    void* cacheEntry;    // non-NULL = GL handles owned by E_meshCache; destroyMesh calls release
} E_mesh;

E_mesh E_createMesh(const GLfloat* positionArray, const GLfloat* colorArray);
void   E_destroyMesh(E_mesh* mesh);

E_mesh E_createMeshFromData(
    const float*        positions,  int vertexCount,
    const float*        normals,
    const float*        texcoords,
    const unsigned int* indices,    int indexCount
);

E_mesh E_createSkinnedMeshFromData(
    const float*          positions,  int vertexCount,
    const float*          normals,
    const float*          texcoords,
    const unsigned short* joints,    // [vertexCount * 4]
    const float*          weights,   // [vertexCount * 4]
    const unsigned int*   indices,   int indexCount
);
