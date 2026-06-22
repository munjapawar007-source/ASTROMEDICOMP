#pragma once
#include <GL/glew.h>
#include <gl/GL.h>

typedef struct {
    GLuint shaderProgram;
    GLuint texture;    // albedo (unit 0)
    GLuint texture1;   // metallic-roughness: G=roughness B=metallic (unit 1)
    GLuint texture2;   // emissive (unit 2)
    float  baseColor[4];
    float  roughness;
    float  metallic;
    float  emissive[3];
} E_material;

E_material E_createDefaultMaterial();
E_material E_createMaterial(GLuint shaderProgram, GLuint texture, float r, float g, float b, float a);
E_material E_createMaterialFromFiles(const char* vertPath, const char* fragPath);
void       E_destroyMaterial(E_material* mat);

GLuint E_loadTexture(const char* path);

void E_material_setTexture(E_material* mat, GLuint texture);
void E_material_setInt    (E_material* mat, const char* name, int value);
void E_material_setFloat(E_material* mat, const char* name, float value);
void E_material_setVec3 (E_material* mat, const char* name, float x, float y, float z);
void E_material_setVec4 (E_material* mat, const char* name, float x, float y, float z, float w);
void E_material_setMat4 (E_material* mat, const char* name, const float* matrix);
