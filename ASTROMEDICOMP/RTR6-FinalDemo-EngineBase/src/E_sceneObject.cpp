#include "E_sceneObject.h"
#include "E_anim.h"
#include "E_scene.h"
#include "E_shader.h"
#include "E_camera.h"
#include "E_utils.h"
#include "E_uniformCache.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

const GLfloat triangle_position[] = { 0.0f,  1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f };
const GLfloat triangle_color[]    = { 1.0f,  0.0f, 0.0f,  0.0f,  1.0f, 0.0f, 0.0f,  0.0f, 1.0f };

static void initHierarchy(E_sceneObject* obj)
{
    obj->parent        = NULL;
    obj->children      = (E_sceneObject**)malloc(4 * sizeof(E_sceneObject*));
    obj->childCount    = 0;
    obj->childCapacity = 4;
}

E_sceneObject E_createSceneObject(const char* name)
{
    E_sceneObject obj;
    strncpy(obj.name, name, sizeof(obj.name) - 1);
    obj.name[sizeof(obj.name) - 1] = '\0';
    obj.mesh         = E_createMesh(triangle_position, triangle_color);
    obj.material     = E_createDefaultMaterial();
    obj.hasMesh      = 1;
    obj.position     = vmath::vec3(0.0f, 0.0f, 0.0f);
    obj.rotation     = vmath::vec3(0.0f, 0.0f, 0.0f);
    obj.scale        = vmath::vec3(1.0f, 1.0f, 1.0f);
    obj.active        = 1;
    obj.customMatrix  = 0;
    obj.uniformScale  = 1;
    obj.transparent   = 0;
    obj.castShadow    = 1;
    obj.receiveShadow = 1;
    obj.writeColor    = 1;
    obj.godRayEmitter = 0;
    obj.cullRadius    = 0.0f;
    obj.animator      = NULL;
    obj.isSkinned     = 0;
    obj.matrixDirty   = 0;
    initHierarchy(&obj);
    E_sceneObject_updateMatrix(&obj);
    obj.worldMatrix = obj.modelMatrix;
    return obj;
}

E_sceneObject E_createSceneNode(const char* name)
{
    E_sceneObject obj;
    strncpy(obj.name, name, sizeof(obj.name) - 1);
    obj.name[sizeof(obj.name) - 1] = '\0';
    memset(&obj.mesh,     0, sizeof(E_mesh));
    memset(&obj.material, 0, sizeof(E_material));
    obj.hasMesh       = 0;
    obj.position      = vmath::vec3(0.0f, 0.0f, 0.0f);
    obj.rotation      = vmath::vec3(0.0f, 0.0f, 0.0f);
    obj.scale         = vmath::vec3(1.0f, 1.0f, 1.0f);
    obj.active        = 1;
    obj.customMatrix  = 0;
    obj.uniformScale  = 1;
    obj.transparent   = 0;
    obj.castShadow    = 1;
    obj.receiveShadow = 1;
    obj.writeColor    = 1;
    obj.godRayEmitter = 0;
    obj.cullRadius    = 0.0f;
    obj.animator      = NULL;
    obj.isSkinned     = 0;
    obj.matrixDirty   = 0;
    initHierarchy(&obj);
    E_sceneObject_updateMatrix(&obj);
    obj.worldMatrix = obj.modelMatrix;
    return obj;
}

void E_sceneObject_setMaterial(E_sceneObject* obj, E_material mat)
{
    E_destroyMaterial(&obj->material);
    obj->material = mat;
}

void E_sceneObject_setMaterialAll(E_sceneObject* obj, E_material mat)
{
    if (!obj) return;
    if (obj->hasMesh) { E_destroyMaterial(&obj->material); obj->material = mat; }
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setMaterialAll(obj->children[i], mat);
}

void E_sceneObject_setAlbedo(E_sceneObject* obj, GLuint texture)
{
    if (!obj) return;
    obj->material.texture = texture;
}

void E_sceneObject_setAlbedoAll(E_sceneObject* obj, GLuint texture)
{
    if (!obj) return;
    if (obj->hasMesh) obj->material.texture = texture;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setAlbedoAll(obj->children[i], texture);
}

void E_sceneObject_setBaseColor(E_sceneObject* obj, float r, float g, float b, float a)
{
    if (!obj) return;
    obj->material.baseColor[0] = r;
    obj->material.baseColor[1] = g;
    obj->material.baseColor[2] = b;
    obj->material.baseColor[3] = a;
}

void E_sceneObject_setShader(E_sceneObject* obj, GLuint shaderProgram)
{
    if (!obj) return;
    obj->material.shaderProgram = shaderProgram;
}

void E_sceneObject_setShaderAll(E_sceneObject* obj, GLuint shaderProgram)
{
    if (!obj) return;
    if (obj->hasMesh) obj->material.shaderProgram = shaderProgram;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setShaderAll(obj->children[i], shaderProgram);
}

void E_sceneObject_setBaseColorAll(E_sceneObject* obj, float r, float g, float b, float a)
{
    if (!obj) return;
    obj->material.baseColor[0] = r;
    obj->material.baseColor[1] = g;
    obj->material.baseColor[2] = b;
    obj->material.baseColor[3] = a;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setBaseColorAll(obj->children[i], r, g, b, a);
}

void E_sceneObject_setTransparentAll(E_sceneObject* obj, int transparent)
{
    if (!obj) return;
    obj->transparent = transparent;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setTransparentAll(obj->children[i], transparent);
}

void E_sceneObject_setCullRadius(E_sceneObject* obj, float radius)
{
    if (!obj) return;
    obj->cullRadius = radius;
}

void E_sceneObject_setCullRadiusAll(E_sceneObject* obj, float radius)
{
    if (!obj) return;
    obj->cullRadius = radius;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setCullRadiusAll(obj->children[i], radius);
}

void E_sceneObject_setCastShadow(E_sceneObject* obj, int castShadow)
{
    if (!obj) return;
    obj->castShadow = castShadow;
}

void E_sceneObject_setCastShadowAll(E_sceneObject* obj, int castShadow)
{
    if (!obj) return;
    obj->castShadow = castShadow;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setCastShadowAll(obj->children[i], castShadow);
}

void E_sceneObject_setReceiveShadow(E_sceneObject* obj, int receiveShadow)
{
    if (!obj) return;
    obj->receiveShadow = receiveShadow;
}

void E_sceneObject_setReceiveShadowAll(E_sceneObject* obj, int receiveShadow)
{
    if (!obj) return;
    obj->receiveShadow = receiveShadow;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setReceiveShadowAll(obj->children[i], receiveShadow);
}

void E_sceneObject_setWriteColorAll(E_sceneObject* obj, int writeColor)
{
    if (!obj) return;
    obj->writeColor = writeColor;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setWriteColorAll(obj->children[i], writeColor);
}

void E_sceneObject_setGodRayEmitterAll(E_sceneObject* obj, int v)
{
    if (!obj) return;
    obj->godRayEmitter = v;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_setGodRayEmitterAll(obj->children[i], v);
}

void E_sceneObject_setPosition(E_sceneObject* obj, vmath::vec3 pos)
{
    if (!obj) return;
    obj->position    = pos;
    obj->matrixDirty = 1;
}

void E_sceneObject_setRotation(E_sceneObject* obj, vmath::vec3 rot)
{
    if (!obj) return;
    obj->rotation    = rot;
    obj->matrixDirty = 1;
}

void E_sceneObject_setScale(E_sceneObject* obj, float s)
{
    if (!obj) return;
    obj->scale       = vmath::vec3(s, s, s);
    obj->matrixDirty = 1;
}

void E_sceneObject_setScaleXYZ(E_sceneObject* obj, vmath::vec3 scale)
{
    if (!obj) return;
    obj->scale       = scale;
    obj->matrixDirty = 1;
}

void E_sceneObject_setActive(E_sceneObject* obj, int active)
{
    if (!obj) return;
    obj->active = active;
}

void E_sceneObject_updateMatrix(E_sceneObject* obj)
{
    obj->modelMatrix = vmath::translate(obj->position)
                     * vmath::rotate(obj->rotation[0], obj->rotation[1], obj->rotation[2])
                     * vmath::scale(obj->scale);
}

void E_sceneObject_updateWorldMatrices(E_sceneObject* obj, const float* parentWorldMatrix)
{
    if (!obj->customMatrix && obj->matrixDirty)
    {
        E_sceneObject_updateMatrix(obj);
        obj->matrixDirty = 0;
    }

    if (parentWorldMatrix)
        obj->worldMatrix = *(const vmath::mat4*)parentWorldMatrix * obj->modelMatrix;
    else
        obj->worldMatrix = obj->modelMatrix;

    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_updateWorldMatrices(obj->children[i], (const float*)obj->worldMatrix);
}

void E_sceneObject_addChild(E_sceneObject* parent, E_sceneObject* child)
{
    if (parent->childCount == parent->childCapacity)
    {
        parent->childCapacity *= 2;
        parent->children = (E_sceneObject**)realloc(parent->children,
            parent->childCapacity * sizeof(E_sceneObject*));
    }
    parent->children[parent->childCount++] = child;
    child->parent = parent;
}

static int   s_skipTransparent    = 0;
static int   s_frustumCullEnabled = 1;
static float s_frustumPlanes[6][4]; // [plane][a,b,c,d], normalized

void E_frustum_setCullEnabled(int enabled) { s_frustumCullEnabled = enabled; }

void E_frustum_update(void)
{
    vmath::mat4 vp = engineCamera.projectionMatrix * engineCamera.viewMatrix;
    const float* m = (const float*)vp; // column-major: m[col*4 + row]

    static const int prows[6] = { 0, 0, 1, 1, 2, 2 };
    static const int signs[6] = { 1,-1, 1,-1, 1,-1 };
    for (int i = 0; i < 6; i++)
    {
        int r = prows[i], sg = signs[i];
        for (int j = 0; j < 4; j++)
            s_frustumPlanes[i][j] = m[3 + j*4] + sg * m[r + j*4];

        float len = vmath::length(vmath::vec3(s_frustumPlanes[i][0],
                                              s_frustumPlanes[i][1],
                                              s_frustumPlanes[i][2]));
        if (len > 0.0001f)
        {
            s_frustumPlanes[i][0] /= len;
            s_frustumPlanes[i][1] /= len;
            s_frustumPlanes[i][2] /= len;
            s_frustumPlanes[i][3] /= len;
        }
    }
}

static int sphereInFrustum(vmath::vec3 center, float radius)
{
    static const float bias = 3.0f; // inflate frustum to avoid false culling near edges/camera
    for (int i = 0; i < 6; i++)
    {
        if (i == 4) continue; // skip near plane  hardware clips it; sphere test here culls close objects incorrectly
        vmath::vec3 n(s_frustumPlanes[i][0], s_frustumPlanes[i][1], s_frustumPlanes[i][2]);
        if (vmath::dot(n, center) + s_frustumPlanes[i][3] < -(radius + bias))
            return 0;
    }
    return 1;
}

void E_sceneObject_setSkipTransparent(int skip) { s_skipTransparent = skip; }

static void drawMesh(E_sceneObject* obj, void* sceneVoid)
{
    if (!obj->hasMesh || obj->material.shaderProgram == 0) return;

    E_scene* scene = (E_scene*)sceneVoid;
    float elapsedTime = (float)E_Utils_getElapsedTimeInSeconds();
    GLuint tex = (obj->material.texture != 0) ? obj->material.texture : defaultTexture;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    if (obj->material.texture1 != 0)
    {
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, obj->material.texture1);
    }
    if (obj->material.texture2 != 0)
    {
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, obj->material.texture2);
    }

    if (s_frustumCullEnabled && !obj->mesh.noCull)
    {
        const vmath::mat4& wm = obj->worldMatrix;
        float s0 = vmath::length(vmath::vec3(wm[0][0], wm[0][1], wm[0][2]));
        float s1 = vmath::length(vmath::vec3(wm[1][0], wm[1][1], wm[1][2]));
        float s2 = vmath::length(vmath::vec3(wm[2][0], wm[2][1], wm[2][2]));
        float maxScale = s0 > s1 ? (s0 > s2 ? s0 : s2) : (s1 > s2 ? s1 : s2);

        vmath::vec3 wc;
        float r = 0.0f;

        if (obj->cullRadius > 0.0f)
        {
            wc = vmath::vec3(wm[3][0], wm[3][1], wm[3][2]);
            r  = obj->cullRadius * maxScale;
        }
        else if (obj->mesh.boundRadius > 0.0f)
        {
            vmath::vec3 lc(obj->mesh.boundCenter[0], obj->mesh.boundCenter[1], obj->mesh.boundCenter[2]);
            wc = vmath::vec3(
                wm[0][0]*lc[0] + wm[1][0]*lc[1] + wm[2][0]*lc[2] + wm[3][0],
                wm[0][1]*lc[0] + wm[1][1]*lc[1] + wm[2][1]*lc[2] + wm[3][1],
                wm[0][2]*lc[0] + wm[1][2]*lc[1] + wm[2][2]*lc[2] + wm[3][2]
            );
            r = obj->mesh.boundRadius * maxScale;
        }

        if (r > 0.0f && !sphereInFrustum(wc, r)) return;
    }

    if (obj->isSkinned && obj->animator && obj->animator->jointCount > 0)
    {
        GLuint prog = skinnedShaderProgramObject;
        glUseProgram(prog);
        GLuint boneBlockIdx = glGetUniformBlockIndex(prog, "BoneBlock");
        if (boneBlockIdx != GL_INVALID_INDEX)
        {
            glUniformBlockBinding(prog, boneBlockIdx, 0);
            glBindBufferBase(GL_UNIFORM_BUFFER, 0, obj->animator->boneUBO);
        }
        glUniformMatrix4fv(E_getUniformLocation(prog, "uView"),       1, GL_FALSE, engineCamera.viewMatrix);
        glUniformMatrix4fv(E_getUniformLocation(prog, "uProjection"), 1, GL_FALSE, engineCamera.projectionMatrix);
        glUniform1f(E_getUniformLocation(prog, "uTime"), elapsedTime);
        glUniform4fv(E_getUniformLocation(prog, "uBaseColor"), 1, obj->material.baseColor);
        glUniform1i(E_getUniformLocation(prog, "uHasTexture"), 1);
        glUniform1i(E_getUniformLocation(prog, "uAlbedo"), 0);
        glUniform1i(E_getUniformLocation(prog, "uMetallicRoughness"),     1);
        glUniform1i(E_getUniformLocation(prog, "uEmissiveTex"),           2);
        glUniform1i(E_getUniformLocation(prog, "uHasMetallicRoughness"),  obj->material.texture1 != 0);
        glUniform1i(E_getUniformLocation(prog, "uHasEmissiveTex"),        obj->material.texture2 != 0);
        glUniform1f(E_getUniformLocation(prog, "uRoughness"),   obj->material.roughness);
        glUniform1f(E_getUniformLocation(prog, "uMetallic"),    obj->material.metallic);
        glUniform3fv(E_getUniformLocation(prog, "uEmissive"),1, obj->material.emissive);
        glUniform1i(E_getUniformLocation(prog, "uGodRayEmitter"), obj->godRayEmitter);
        glUniform3fv(E_getUniformLocation(prog, "uCameraPos"), 1, engineCamera.position);
        if (scene)
        {
            float zeroPos[3] = { 0, 0, 0 };
            E_scene_applyFog(scene, prog);
            E_scene_applyLighting(scene, prog, zeroPos);
            if (!obj->receiveShadow)
                glProgramUniform1i(prog, E_getUniformLocation(prog, "uShadowEnabled"), 0);
        }
    }
    else
    {
        GLuint prog = obj->material.shaderProgram;
        glUseProgram(prog);
        glUniformMatrix4fv(E_getUniformLocation(prog, "uModel"),      1, GL_FALSE, obj->worldMatrix);
        glUniformMatrix4fv(E_getUniformLocation(prog, "uView"),       1, GL_FALSE, engineCamera.viewMatrix);
        glUniformMatrix4fv(E_getUniformLocation(prog, "uProjection"), 1, GL_FALSE, engineCamera.projectionMatrix);
        glUniform1f(E_getUniformLocation(prog, "uTime"), elapsedTime);
        glUniform4fv(E_getUniformLocation(prog, "uBaseColor"), 1, obj->material.baseColor);
        glUniform1i(E_getUniformLocation(prog, "uHasTexture"), 1);
        glUniform1i(E_getUniformLocation(prog, "uAlbedo"), 0);
        glUniform1i(E_getUniformLocation(prog, "uMetallicRoughness"),     1);
        glUniform1i(E_getUniformLocation(prog, "uEmissiveTex"),           2);
        glUniform1i(E_getUniformLocation(prog, "uHasMetallicRoughness"),  obj->material.texture1 != 0);
        glUniform1i(E_getUniformLocation(prog, "uHasEmissiveTex"),        obj->material.texture2 != 0);
        glUniform1f(E_getUniformLocation(prog, "uRoughness"),   obj->material.roughness);
        glUniform1f(E_getUniformLocation(prog, "uMetallic"),    obj->material.metallic);
        glUniform3fv(E_getUniformLocation(prog, "uEmissive"),1, obj->material.emissive);
        glUniform1i(E_getUniformLocation(prog, "uGodRayEmitter"), obj->godRayEmitter);
        glUniform3fv(E_getUniformLocation(prog, "uCameraPos"), 1, engineCamera.position);
        if (scene)
        {
            const float* wm = (const float*)obj->worldMatrix;
            float worldPos[3] = { wm[12], wm[13], wm[14] };
            E_scene_applyFog(scene, prog);
            E_scene_applyLighting(scene, prog, worldPos);
            if (!obj->receiveShadow)
                glProgramUniform1i(prog, E_getUniformLocation(prog, "uShadowEnabled"), 0);
        }
    }

    if (!obj->writeColor)
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glBindVertexArray(obj->mesh.vao);
    if (obj->mesh.indexCount > 0)
        glDrawElements(GL_TRIANGLES, obj->mesh.indexCount, GL_UNSIGNED_INT, 0);
    else
        glDrawArrays(GL_TRIANGLES, 0, obj->mesh.vertexCount);
    glBindVertexArray(0);
    if (!obj->writeColor)
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if (obj->material.texture2 != 0) { glActiveTexture(GL_TEXTURE2); glBindTexture(GL_TEXTURE_2D, 0); }
    if (obj->material.texture1 != 0) { glActiveTexture(GL_TEXTURE1); glBindTexture(GL_TEXTURE_2D, 0); }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glUseProgram(0);
}

void E_renderSceneObject(E_sceneObject* obj, void* sceneVoid, const float* parentWorldMatrix)
{
    if (!obj->customMatrix && obj->matrixDirty)
    {
        E_sceneObject_updateMatrix(obj);
        obj->matrixDirty = 0;
    }

    if (parentWorldMatrix)
        obj->worldMatrix = *(const vmath::mat4*)parentWorldMatrix * obj->modelMatrix;
    else
        obj->worldMatrix = obj->modelMatrix;

    if (!s_skipTransparent || !obj->transparent)
        drawMesh(obj, sceneVoid);

    for (int i = 0; i < obj->childCount; i++)
    {
        E_sceneObject* child = obj->children[i];
        if (child->active)
            E_renderSceneObject(child, sceneVoid, (const float*)obj->worldMatrix);
    }
}

void E_renderSceneObjectSingle(E_sceneObject* obj, void* sceneVoid)
{
    if (obj->transparent)
        drawMesh(obj, sceneVoid);
}

void E_sceneObject_destroyResources(E_sceneObject* obj)
{
    if (!obj) return;
    for (int i = 0; i < obj->childCount; i++)
        E_sceneObject_destroyResources(obj->children[i]);
    if (obj->hasMesh)
    {
        E_destroyMesh(&obj->mesh);
        E_destroyMaterial(&obj->material);
    }
    obj = NULL;
}

void E_destroySceneObject() {}
