#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

#include "stb_image.h"

#include <GL/glew.h>
#include <gl/GL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "E_modelLoader.h"
#include "E_mesh.h"
#include "E_meshCache.h"
#include "E_material.h"
#include "E_shader.h"
#include "E_scene.h"
#include "E_sceneObject.h"
#include "E_anim.h"

extern FILE* gpFile;
#define LOG(...) ((void)0)


static float* readFloats(const cgltf_accessor* acc, int* outCount)
{
    if (!acc || !acc->buffer_view || !acc->buffer_view->buffer || !acc->buffer_view->buffer->data) return NULL;
    int comps = (int)cgltf_num_components(acc->type);
    int count = (int)acc->count;
    float* buf = (float*)malloc(sizeof(float) * comps * count);
    for (int i = 0; i < count; i++)
        cgltf_accessor_read_float(acc, i, buf + i * comps, comps);
    if (outCount) *outCount = count;
    return buf;
}

static unsigned int* readIndices(const cgltf_accessor* acc, int* outCount)
{
    if (!acc || !acc->buffer_view || !acc->buffer_view->buffer || !acc->buffer_view->buffer->data) return NULL;
    int count = (int)acc->count;
    unsigned int* buf = (unsigned int*)malloc(sizeof(unsigned int) * count);
    for (int i = 0; i < count; i++)
        buf[i] = (unsigned int)cgltf_accessor_read_index(acc, i);
    if (outCount) *outCount = count;
    return buf;
}

static unsigned short* readJoints(const cgltf_accessor* acc, int* outCount)
{
    if (!acc || !acc->buffer_view || !acc->buffer_view->buffer || !acc->buffer_view->buffer->data) return NULL;
    int count = (int)acc->count;
    unsigned short* buf = (unsigned short*)malloc(sizeof(unsigned short) * 4 * count);
    for (int i = 0; i < count; i++) {
        unsigned int j[4] = { 0, 0, 0, 0 };
        cgltf_accessor_read_uint(acc, i, j, 4);
        buf[i*4+0] = (unsigned short)j[0];
        buf[i*4+1] = (unsigned short)j[1];
        buf[i*4+2] = (unsigned short)j[2];
        buf[i*4+3] = (unsigned short)j[3];
    }
    if (outCount) *outCount = count;
    return buf;
}

static GLuint uploadTexture(const cgltf_image* img, const char* baseDir)
{
    if (!img) return 0;

    int w, h, ch;
    unsigned char* pixels = NULL;

    if (img->buffer_view && img->buffer_view->buffer && img->buffer_view->buffer->data)
    {
        const unsigned char* src = (const unsigned char*)img->buffer_view->buffer->data
                                    + img->buffer_view->offset;
        pixels = stbi_load_from_memory(src, (int)img->buffer_view->size, &w, &h, &ch, 4);
    }
    else if (img->uri)
    {
        char fullPath[512];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", baseDir, img->uri);
        pixels = stbi_load(fullPath, &w, &h, &ch, 4);
    }

    if (!pixels) return 0;

    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);
    return id;
}

static void nodeLocalMatrix(const cgltf_node* node, float out[16])
{
    if (node->has_matrix) { memcpy(out, node->matrix, 64); return; }

    float tx = node->has_translation ? node->translation[0] : 0.0f;
    float ty = node->has_translation ? node->translation[1] : 0.0f;
    float tz = node->has_translation ? node->translation[2] : 0.0f;
    float qx = node->has_rotation    ? node->rotation[0]    : 0.0f;
    float qy = node->has_rotation    ? node->rotation[1]    : 0.0f;
    float qz = node->has_rotation    ? node->rotation[2]    : 0.0f;
    float qw = node->has_rotation    ? node->rotation[3]    : 1.0f;
    float sx = node->has_scale       ? node->scale[0]       : 1.0f;
    float sy = node->has_scale       ? node->scale[1]       : 1.0f;
    float sz = node->has_scale       ? node->scale[2]       : 1.0f;

    out[0]=(1-2*(qy*qy+qz*qz))*sx; out[1]=(2*(qx*qy+qw*qz))*sx;   out[2]=(2*(qx*qz-qw*qy))*sx;   out[3]=0;
    out[4]=(2*(qx*qy-qw*qz))*sy;   out[5]=(1-2*(qx*qx+qz*qz))*sy; out[6]=(2*(qy*qz+qw*qx))*sy;   out[7]=0;
    out[8]=(2*(qx*qz+qw*qy))*sz;   out[9]=(2*(qy*qz-qw*qx))*sz;   out[10]=(1-2*(qx*qx+qy*qy))*sz;out[11]=0;
    out[12]=tx; out[13]=ty; out[14]=tz; out[15]=1;
}


E_sceneObject* E_loadModel(const char* path, E_scene* scene, const char* name)
{
    // 1. Parse the GLTF/GLB file
    cgltf_options opts; memset(&opts, 0, sizeof(opts));
    cgltf_data* data = NULL;

    if (cgltf_parse_file(&opts, path, &data) != cgltf_result_success) { cgltf_free(data); return NULL; }
    if (cgltf_load_buffers(&opts, data, path) != cgltf_result_success) { cgltf_free(data); return NULL; }

    // Base directory for resolving external texture paths
    char baseDir[512] = ".";
    const char* lastSlash = path;
    for (const char* c = path; *c; c++) if (*c == '/' || *c == '\\') lastSlash = c;
    if (lastSlash != path) snprintf(baseDir, sizeof(baseDir), "%.*s", (int)(lastSlash - path), path);

    // Texture cache  one GLuint per cgltf_image
    int imageCount = (int)data->images_count;
    GLuint* texCache = (GLuint*)calloc(imageCount > 0 ? imageCount : 1, sizeof(GLuint));

    int nodeCount = (int)data->nodes_count;
    E_sceneObject** nodeObjs = (E_sceneObject**)calloc(nodeCount, sizeof(E_sceneObject*));

    // Create the animator early if the file has animations or a skin
    int hasAnim = (data->animations_count > 0 || data->skins_count > 0);
    int jointCount = (data->skins_count > 0) ? (int)data->skins[0].joints_count : 0;
    E_animator* animator = NULL;
    if (hasAnim)
        animator = E_animator_create(nodeCount, (int)data->animations_count, jointCount);

    // 2. Create one scene node per GLTF node (transform only)
    for (int i = 0; i < nodeCount; i++)
    {
        const cgltf_node* n = &data->nodes[i];
        char name[64];
        snprintf(name, sizeof(name), "%s", (n->name && n->name[0]) ? n->name : "node");

        nodeObjs[i] = (E_sceneObject*)malloc(sizeof(E_sceneObject));
        *nodeObjs[i] = E_createSceneNode(name);

        float local[16];
        nodeLocalMatrix(n, local);
        memcpy((float*)nodeObjs[i]->modelMatrix, local, 64);
        nodeObjs[i]->customMatrix = 1;

        // Store rest-pose TRS in the animator for animation channels to modify
        if (animator)
        {
            animator->nodes[i] = nodeObjs[i];
            float t[3] = { n->has_translation ? n->translation[0] : 0.0f,
                           n->has_translation ? n->translation[1] : 0.0f,
                           n->has_translation ? n->translation[2] : 0.0f };
            float r[4] = { n->has_rotation ? n->rotation[0] : 0.0f,
                           n->has_rotation ? n->rotation[1] : 0.0f,
                           n->has_rotation ? n->rotation[2] : 0.0f,
                           n->has_rotation ? n->rotation[3] : 1.0f };
            float s[3] = { n->has_scale ? n->scale[0] : 1.0f,
                           n->has_scale ? n->scale[1] : 1.0f,
                           n->has_scale ? n->scale[2] : 1.0f };
            E_animator_setNodeRest(animator, i, t, r, s);
        }
    }

    // 3. Wire up the parent/child hierarchy
    for (int i = 0; i < nodeCount; i++)
    {
        const cgltf_node* n = &data->nodes[i];
        for (int c = 0; c < (int)n->children_count; c++)
        {
            int childIdx = (int)(n->children[c] - data->nodes);
            E_sceneObject_addChild(nodeObjs[i], nodeObjs[childIdx]);
        }
    }

    // 4. For each node that has a mesh, create a primitive child per mesh primitive
    int primCount = 0;
    for (int i = 0; i < nodeCount; i++)
    {
        const cgltf_node* n = &data->nodes[i];
        if (!n->mesh) continue;

        int isSkinned = (n->skin != NULL);

        for (int p = 0; p < (int)n->mesh->primitives_count; p++)
        {
            const cgltf_primitive* prim = &n->mesh->primitives[p];
            if (prim->type != cgltf_primitive_type_triangles) continue;

            char primName[64];
            snprintf(primName, sizeof(primName), "%s_p%d", n->mesh->name ? n->mesh->name : "mesh", p);

            char cacheKey[448];
            snprintf(cacheKey, sizeof(cacheKey), "%s::%s", path, primName);

            E_sceneObject* primObj = (E_sceneObject*)malloc(sizeof(E_sceneObject));
            *primObj = E_createSceneNode(primName);
            primObj->hasMesh = 1;

            E_meshCacheEntry* cacheHit = E_meshCache_get(cacheKey);
            if (cacheHit)
            {
                primObj->mesh.vao            = cacheHit->vao;
                primObj->mesh.vbo_position   = cacheHit->vbo_position;
                primObj->mesh.vbo_color      = cacheHit->vbo_color;
                primObj->mesh.vbo_normal     = cacheHit->vbo_normal;
                primObj->mesh.vbo_texcoord   = cacheHit->vbo_texcoord;
                primObj->mesh.vbo_joints     = cacheHit->vbo_joints;
                primObj->mesh.vbo_weights    = cacheHit->vbo_weights;
                primObj->mesh.ebo            = cacheHit->ebo;
                primObj->mesh.indexCount     = cacheHit->indexCount;
                primObj->mesh.vertexCount    = cacheHit->vertexCount;
                primObj->mesh.boundCenter[0] = cacheHit->boundCenter[0];
                primObj->mesh.boundCenter[1] = cacheHit->boundCenter[1];
                primObj->mesh.boundCenter[2] = cacheHit->boundCenter[2];
                primObj->mesh.boundRadius    = cacheHit->boundRadius;
                primObj->mesh.noCull         = cacheHit->noCull;
                primObj->mesh.cacheEntry     = (void*)cacheHit;
                cacheHit->refCount++;
                if (isSkinned && animator) { primObj->isSkinned = 1; primObj->animator = animator; }
            }
            else
            {
                float*          positions = NULL; int vertexCount = 0;
                float*          normals   = NULL;
                float*          texcoords = NULL;
                float*          weights4f = NULL;
                unsigned short* joints    = NULL;

                for (int a = 0; a < (int)prim->attributes_count; a++)
                {
                    const cgltf_attribute* attr = &prim->attributes[a];
                    if      (attr->type == cgltf_attribute_type_position && attr->index == 0)
                        positions = readFloats(attr->data, &vertexCount);
                    else if (attr->type == cgltf_attribute_type_normal && attr->index == 0)
                        normals   = readFloats(attr->data, NULL);
                    else if (attr->type == cgltf_attribute_type_texcoord && attr->index == 0)
                        texcoords = readFloats(attr->data, NULL);
                    else if (attr->type == cgltf_attribute_type_joints && attr->index == 0)
                        joints    = readJoints(attr->data, NULL);
                    else if (attr->type == cgltf_attribute_type_weights && attr->index == 0)
                        weights4f = readFloats(attr->data, NULL);
                }

                if (!positions) { free(normals); free(texcoords); free(joints); free(weights4f); free(primObj); continue; }

                if (!normals)   { normals   = (float*)calloc(vertexCount * 3, sizeof(float)); for (int v=0;v<vertexCount;v++) normals[v*3+1]=1.0f; }
                if (!texcoords) { texcoords = (float*)calloc(vertexCount * 2, sizeof(float)); }

                unsigned int* indices = NULL; int indexCount = 0;
                if (prim->indices) indices = readIndices(prim->indices, &indexCount);

                if (isSkinned && joints && weights4f && animator)
                {
                    primObj->mesh      = E_createSkinnedMeshFromData(positions, vertexCount, normals,
                                             texcoords, joints, weights4f, indices, indexCount);
                    primObj->isSkinned = 1;
                    primObj->animator  = animator;
                }
                else
                {
                    primObj->mesh = E_createMeshFromData(positions, vertexCount, normals, texcoords,
                                        indices, indexCount);
                }

                primObj->mesh.cacheEntry = (void*)E_meshCache_insert(cacheKey,
                    primObj->mesh.vao, primObj->mesh.vbo_position, primObj->mesh.vbo_color,
                    primObj->mesh.vbo_normal, primObj->mesh.vbo_texcoord,
                    primObj->mesh.vbo_joints, primObj->mesh.vbo_weights, primObj->mesh.ebo,
                    primObj->mesh.vertexCount, primObj->mesh.indexCount,
                    primObj->mesh.boundCenter, primObj->mesh.boundRadius, primObj->mesh.noCull);

                free(positions); free(normals); free(texcoords); free(indices);
                free(joints); free(weights4f);
            }

            GLuint texID = 0, texMR = 0, texEmissive = 0;
            float r=1,g=1,b=1,a=1;
            float roughness=1.0f, metallic=0.0f;
            float emissive[3] = {0,0,0};
            if (prim->material)
            {
                const cgltf_pbr_metallic_roughness* pbr = &prim->material->pbr_metallic_roughness;
                r = pbr->base_color_factor[0]; g = pbr->base_color_factor[1];
                b = pbr->base_color_factor[2]; a = pbr->base_color_factor[3];
                roughness = pbr->roughness_factor;
                metallic  = pbr->metallic_factor;
                emissive[0] = prim->material->emissive_factor[0];
                emissive[1] = prim->material->emissive_factor[1];
                emissive[2] = prim->material->emissive_factor[2];

                // Albedo texture
                if (pbr->base_color_texture.texture && pbr->base_color_texture.texture->image)
                {
                    int imgIdx = (int)(pbr->base_color_texture.texture->image - data->images);
                    if (imgIdx >= 0 && imgIdx < imageCount)
                    {
                        if (!texCache[imgIdx]) texCache[imgIdx] = uploadTexture(pbr->base_color_texture.texture->image, baseDir);
                        texID = texCache[imgIdx];
                    }
                }
                // Metallic-roughness texture (G=roughness, B=metallic)
                if (pbr->metallic_roughness_texture.texture && pbr->metallic_roughness_texture.texture->image)
                {
                    int imgIdx = (int)(pbr->metallic_roughness_texture.texture->image - data->images);
                    if (imgIdx >= 0 && imgIdx < imageCount)
                    {
                        if (!texCache[imgIdx]) texCache[imgIdx] = uploadTexture(pbr->metallic_roughness_texture.texture->image, baseDir);
                        texMR = texCache[imgIdx];
                    }
                }
                // Emissive texture
                if (prim->material->emissive_texture.texture && prim->material->emissive_texture.texture->image)
                {
                    int imgIdx = (int)(prim->material->emissive_texture.texture->image - data->images);
                    if (imgIdx >= 0 && imgIdx < imageCount)
                    {
                        if (!texCache[imgIdx]) texCache[imgIdx] = uploadTexture(prim->material->emissive_texture.texture->image, baseDir);
                        texEmissive = texCache[imgIdx];
                    }
                }
            }
            primObj->material          = E_createMaterial(uberShaderProgramObject, texID, r, g, b, a);
            primObj->material.texture1 = texMR;
            primObj->material.texture2 = texEmissive;
            primObj->material.roughness   = roughness;
            primObj->material.metallic    = metallic;
            primObj->material.emissive[0] = emissive[0];
            primObj->material.emissive[1] = emissive[1];
            primObj->material.emissive[2] = emissive[2];
            if (prim->material && prim->material->alpha_mode == cgltf_alpha_mode_blend)
                primObj->transparent = 1;
            if (prim->material && prim->material->alpha_mode == cgltf_alpha_mode_mask)
            {
                E_material_setInt  (&primObj->material, "uAlphaMask",    1);
                E_material_setFloat(&primObj->material, "uAlphaCutoff",  prim->material->alpha_cutoff);
            }

            E_sceneObject_addChild(nodeObjs[i], primObj);
            primCount++;
        }
    }

    // 5. Load skin data into the animator (inverse bind matrices + joint node indices)
    if (animator && data->skins_count > 0)
    {
        const cgltf_skin* skin = &data->skins[0];
        int jc = (int)skin->joints_count;
        if (jc > E_MAX_JOINTS) jc = E_MAX_JOINTS;
        animator->jointCount = jc;

        if (skin->inverse_bind_matrices)
        {
            for (int j = 0; j < jc; j++)
                cgltf_accessor_read_float(skin->inverse_bind_matrices, j,
                    animator->inverseBindMats + j*16, 16);
        }
        else
        {
            // Identity when not provided
            for (int j = 0; j < jc; j++) {
                memset(animator->inverseBindMats + j*16, 0, 64);
                animator->inverseBindMats[j*16+0]  = 1.0f;
                animator->inverseBindMats[j*16+5]  = 1.0f;
                animator->inverseBindMats[j*16+10] = 1.0f;
                animator->inverseBindMats[j*16+15] = 1.0f;
            }
        }

        for (int j = 0; j < jc; j++)
            animator->jointNodeIndices[j] = (int)(skin->joints[j] - data->nodes);
    }

    // 6. Load animations into the animator
    if (animator && data->animations_count > 0)
    {
        for (int ai = 0; ai < (int)data->animations_count; ai++)
        {
            const cgltf_animation* a = &data->animations[ai];
            E_animClip* clip = &animator->clips[ai];
            snprintf(clip->name, sizeof(clip->name), "%s", (a->name && a->name[0]) ? a->name : "anim");

            clip->samplerCount = (int)a->samplers_count;
            clip->samplers     = (E_animSampler*)calloc(clip->samplerCount, sizeof(E_animSampler));

            for (int s = 0; s < clip->samplerCount; s++)
            {
                const cgltf_animation_sampler* sa = &a->samplers[s];
                int tc = (int)sa->input->count;
                clip->samplers[s].count      = tc;
                clip->samplers[s].components = (int)cgltf_num_components(sa->output->type);
                clip->samplers[s].times      = (float*)malloc(tc * sizeof(float));
                for (int k = 0; k < tc; k++)
                    cgltf_accessor_read_float(sa->input, k, &clip->samplers[s].times[k], 1);

                // Track duration
                if (tc > 0 && clip->samplers[s].times[tc-1] > clip->duration)
                    clip->duration = clip->samplers[s].times[tc-1];

                int vc = (int)sa->output->count;
                clip->samplers[s].values = (float*)malloc(vc * clip->samplers[s].components * sizeof(float));
                for (int k = 0; k < vc; k++)
                    cgltf_accessor_read_float(sa->output, k,
                        clip->samplers[s].values + k * clip->samplers[s].components,
                        clip->samplers[s].components);
            }

            clip->channelCount = (int)a->channels_count;
            clip->channels     = (E_animChannel*)calloc(clip->channelCount, sizeof(E_animChannel));

            for (int c = 0; c < clip->channelCount; c++)
            {
                const cgltf_animation_channel* ch = &a->channels[c];
                clip->channels[c].samplerIdx = (int)(ch->sampler - a->samplers);
                clip->channels[c].nodeIdx    = ch->target_node ? (int)(ch->target_node - data->nodes) : -1;

                switch (ch->target_path)
                {
                    case cgltf_animation_path_type_translation: clip->channels[c].path = E_ANIM_PATH_TRANSLATION; break;
                    case cgltf_animation_path_type_rotation:    clip->channels[c].path = E_ANIM_PATH_ROTATION;    break;
                    case cgltf_animation_path_type_scale:       clip->channels[c].path = E_ANIM_PATH_SCALE;       break;
                    default: clip->channels[c].nodeIdx = -1; break;
                }
            }
        }
    }

    // 7. Determine root name: use caller-supplied name, else strip filename extension from path
    char rootName[64] = "model";
    if (name && name[0])
    {
        snprintf(rootName, sizeof(rootName), "%s", name);
    }
    else
    {
        const char* slash = path;
        for (const char* c = path; *c; c++) if (*c == '/' || *c == '\\') slash = c + 1;
        snprintf(rootName, sizeof(rootName), "%s", slash);
        for (int i = 0; rootName[i]; i++) if (rootName[i] == '.') { rootName[i] = '\0'; break; }
    }

    // 8. Create a single root node and parent all GLTF roots under it
    E_sceneObject* root = (E_sceneObject*)malloc(sizeof(E_sceneObject));
    *root = E_createSceneNode(rootName);
    root->animator = animator;  // attach animator to root for UI

    if (data->scene)
    {
        for (int i = 0; i < (int)data->scene->nodes_count; i++)
        {
            int idx = (int)(data->scene->nodes[i] - data->nodes);
            E_sceneObject_addChild(root, nodeObjs[idx]);
        }
    }
    else
    {
        for (int i = 0; i < nodeCount; i++)
            if (!nodeObjs[i]->parent)
                E_sceneObject_addChild(root, nodeObjs[i]);
    }

    E_scene_add(scene, root);

    free(texCache);
    free(nodeObjs);
    cgltf_free(data);
    return root;
}

E_mesh E_loadMeshOnly(const char* path)
{
    E_mesh out; memset(&out, 0, sizeof(out));

    cgltf_options opts; memset(&opts, 0, sizeof(opts));
    cgltf_data* data = NULL;
    if (cgltf_parse_file(&opts, path, &data) != cgltf_result_success) { cgltf_free(data); return out; }
    if (cgltf_load_buffers(&opts, data, path) != cgltf_result_success) { cgltf_free(data); return out; }

    if (data->meshes_count == 0 || data->meshes[0].primitives_count == 0) { cgltf_free(data); return out; }
    cgltf_primitive* prim = &data->meshes[0].primitives[0];

    int vertCount = 0;
    float* positions = NULL;
    for (int i = 0; i < (int)prim->attributes_count; i++)
    {
        if (prim->attributes[i].type == cgltf_attribute_type_position)
        {
            positions = readFloats(prim->attributes[i].data, &vertCount);
            break;
        }
    }

    int idxCount = 0;
    unsigned int* indices = readIndices(prim->indices, &idxCount);

    if (positions && vertCount > 0)
        out = E_createMeshFromData(positions, vertCount, NULL, NULL, indices, idxCount);

    free(positions);
    free(indices);
    cgltf_free(data);
    return out;
}
