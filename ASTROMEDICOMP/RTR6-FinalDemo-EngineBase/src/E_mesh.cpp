#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include "E_mesh.h"
#include "E_meshCache.h"

E_mesh E_createMesh(const GLfloat* positionArray, const GLfloat* colorArray)
{
    E_mesh tempMesh;

    tempMesh.vao = 0;
    tempMesh.vbo_position = 0;
    tempMesh.vbo_color = 0;
    tempMesh.vbo_normal = 0;
    tempMesh.vbo_texcoord = 0;
    tempMesh.vbo_joints = 0;
    tempMesh.vbo_weights = 0;
    tempMesh.ebo = 0;
    tempMesh.indexCount = 0;
    tempMesh.vertexCount = 3;
    tempMesh.boundCenter[0] = 0.0f;
    tempMesh.boundCenter[1] = 0.0f;
    tempMesh.boundCenter[2] = 0.0f;
    tempMesh.boundRadius    = 0.0f;
    tempMesh.noCull         = 1;
    tempMesh.cacheEntry     = NULL;

    glGenVertexArrays(1, &tempMesh.vao);
    glBindVertexArray(tempMesh.vao);

    // position
    glGenBuffers(1, &tempMesh.vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, tempMesh.vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*3, positionArray, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // color
    glGenBuffers(1, &tempMesh.vbo_color);
    glBindBuffer(GL_ARRAY_BUFFER, tempMesh.vbo_color);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*3, colorArray, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return tempMesh;
}

E_mesh E_createMeshFromData(
    const float* positions, int vertexCount,
    const float* normals,
    const float* texcoords,
    const unsigned int* indices, int indexCount)
{
    E_mesh tempMesh;

    tempMesh.vao = 0;
    tempMesh.vbo_position = 0;
    tempMesh.vbo_color = 0;
    tempMesh.vbo_normal = 0;
    tempMesh.vbo_texcoord = 0;
    tempMesh.vbo_joints = 0;
    tempMesh.vbo_weights = 0;
    tempMesh.ebo = 0;
    tempMesh.indexCount = indexCount;
    tempMesh.vertexCount = vertexCount;
    tempMesh.noCull     = 0;
    tempMesh.cacheEntry = NULL;

    // Bounding sphere: AABB center + max distance
    if (positions && vertexCount > 0)
    {
        float minX = positions[0], maxX = positions[0];
        float minY = positions[1], maxY = positions[1];
        float minZ = positions[2], maxZ = positions[2];
        for (int v = 1; v < vertexCount; v++)
        {
            float x = positions[v*3+0], y = positions[v*3+1], z = positions[v*3+2];
            if (x < minX) minX = x; if (x > maxX) maxX = x;
            if (y < minY) minY = y; if (y > maxY) maxY = y;
            if (z < minZ) minZ = z; if (z > maxZ) maxZ = z;
        }
        tempMesh.boundCenter[0] = (minX + maxX) * 0.5f;
        tempMesh.boundCenter[1] = (minY + maxY) * 0.5f;
        tempMesh.boundCenter[2] = (minZ + maxZ) * 0.5f;
        float r = 0.0f;
        for (int v = 0; v < vertexCount; v++)
        {
            float dx = positions[v*3+0] - tempMesh.boundCenter[0];
            float dy = positions[v*3+1] - tempMesh.boundCenter[1];
            float dz = positions[v*3+2] - tempMesh.boundCenter[2];
            float d  = dx*dx + dy*dy + dz*dz;
            if (d > r) r = d;
        }
        tempMesh.boundRadius = sqrtf(r);
    }
    else
    {
        tempMesh.boundCenter[0] = tempMesh.boundCenter[1] = tempMesh.boundCenter[2] = 0.0f;
        tempMesh.boundRadius    = 0.0f;
    }

    glGenVertexArrays(1, &tempMesh.vao);
    glBindVertexArray(tempMesh.vao);

    // position
    glGenBuffers(1, &tempMesh.vbo_position);
    glBindBuffer(GL_ARRAY_BUFFER, tempMesh.vbo_position);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*vertexCount, positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // normal
    if (normals)
    {
        glGenBuffers(1, &tempMesh.vbo_normal);
        glBindBuffer(GL_ARRAY_BUFFER, tempMesh.vbo_normal);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*3*vertexCount, normals, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // texcoord
    if (texcoords)
    {
        glGenBuffers(1, &tempMesh.vbo_texcoord);
        glBindBuffer(GL_ARRAY_BUFFER, tempMesh.vbo_texcoord);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*vertexCount, texcoords, GL_STATIC_DRAW);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(3);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // indices
    if (indices && indexCount > 0)
    {
        glGenBuffers(1, &tempMesh.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tempMesh.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*indexCount, indices, GL_STATIC_DRAW);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    printf("Mesh: %d\n", vertexCount);
    return tempMesh;
}

E_mesh E_createSkinnedMeshFromData(
    const float*          positions, int vertexCount,
    const float*          normals,
    const float*          texcoords,
    const unsigned short* joints,
    const float*          weights,
    const unsigned int*   indices,  int indexCount)
{
    E_mesh m = E_createMeshFromData(positions, vertexCount, normals, texcoords, indices, indexCount);

    glBindVertexArray(m.vao);

    if (joints) {
        glGenBuffers(1, &m.vbo_joints);
        glBindBuffer(GL_ARRAY_BUFFER, m.vbo_joints);
        glBufferData(GL_ARRAY_BUFFER, sizeof(unsigned short) * 4 * vertexCount, joints, GL_STATIC_DRAW);
        glVertexAttribIPointer(4, 4, GL_UNSIGNED_SHORT, 0, NULL);
        glEnableVertexAttribArray(4);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (weights) {
        glGenBuffers(1, &m.vbo_weights);
        glBindBuffer(GL_ARRAY_BUFFER, m.vbo_weights);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * vertexCount, weights, GL_STATIC_DRAW);
        glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(5);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    glBindVertexArray(0);
    return m;
}

void E_destroyMesh(E_mesh* mesh)
{
    if (!mesh) return;
    if (mesh->cacheEntry)
    {
        E_meshCache_release((E_meshCacheEntry*)mesh->cacheEntry);
        mesh->cacheEntry = NULL;
        return;
    }
    if (mesh->vao)          { glDeleteVertexArrays(1, &mesh->vao);          mesh->vao          = 0; }
    if (mesh->vbo_position) { glDeleteBuffers(1, &mesh->vbo_position);      mesh->vbo_position = 0; }
    if (mesh->vbo_color)    { glDeleteBuffers(1, &mesh->vbo_color);         mesh->vbo_color    = 0; }
    if (mesh->vbo_normal)   { glDeleteBuffers(1, &mesh->vbo_normal);        mesh->vbo_normal   = 0; }
    if (mesh->vbo_texcoord) { glDeleteBuffers(1, &mesh->vbo_texcoord);      mesh->vbo_texcoord = 0; }
    if (mesh->vbo_joints)   { glDeleteBuffers(1, &mesh->vbo_joints);        mesh->vbo_joints   = 0; }
    if (mesh->vbo_weights)  { glDeleteBuffers(1, &mesh->vbo_weights);       mesh->vbo_weights  = 0; }
    if (mesh->ebo)          { glDeleteBuffers(1, &mesh->ebo);               mesh->ebo          = 0; }
}

