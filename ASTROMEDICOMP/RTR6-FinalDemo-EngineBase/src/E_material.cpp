#include "E_material.h"
#include "E_shader.h"
#include "E_utils.h"
#include "E_uniformCache.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

E_material E_createDefaultMaterial()
{
    E_material mat;
    mat.shaderProgram = uberShaderProgramObject;
    mat.texture       = 0;
    mat.texture1      = 0;
    mat.texture2      = 0;
    mat.baseColor[0]  = 1.0f;
    mat.baseColor[1]  = 1.0f;
    mat.baseColor[2]  = 1.0f;
    mat.baseColor[3]  = 1.0f;
    mat.roughness     = 1.0f;
    mat.metallic      = 0.0f;
    mat.emissive[0]   = 0.0f;
    mat.emissive[1]   = 0.0f;
    mat.emissive[2]   = 0.0f;
    return mat;
}

E_material E_createMaterial(GLuint shaderProgram, GLuint texture, float r, float g, float b, float a)
{
    E_material mat;
    mat.shaderProgram = shaderProgram;
    mat.texture       = texture;
    mat.texture1      = 0;
    mat.texture2      = 0;
    mat.baseColor[0]  = r;
    mat.baseColor[1]  = g;
    mat.baseColor[2]  = b;
    mat.baseColor[3]  = a;
    mat.roughness     = 1.0f;
    mat.metallic      = 0.0f;
    mat.emissive[0]   = 0.0f;
    mat.emissive[1]   = 0.0f;
    mat.emissive[2]   = 0.0f;
    return mat;
}

E_material E_createMaterialFromFiles(const char* vertPath, const char* fragPath)
{
    char* vertSrc = E_Utils_LoadFile(vertPath);
    char* fragSrc = E_Utils_LoadFile(fragPath);

    GLuint vs   = E_compileShader(vertSrc, GL_VERTEX_SHADER);
    GLuint fs   = E_compileShader(fragSrc, GL_FRAGMENT_SHADER);
    GLuint prog = E_createProgram(vs, fs);

    free(vertSrc);
    free(fragSrc);

    return E_createMaterial(prog, 0, 1.0f, 1.0f, 1.0f, 1.0f);
}

void E_destroyMaterial(E_material* mat)
{
    if (mat->texture)
    {
        glDeleteTextures(1, &mat->texture);
        mat->texture = 0;
    }
    if (mat->texture1)
    {
        glDeleteTextures(1, &mat->texture1);
        mat->texture1 = 0;
    }
    if (mat->texture2)
    {
        glDeleteTextures(1, &mat->texture2);
        mat->texture2 = 0;
    }
}

GLuint E_loadTexture(const char* path)
{
    int w, h, channels;
    stbi_uc* pixels = stbi_load(path, &w, &h, &channels, 4);
    if (!pixels)
    {
        printf("[E_loadTexture] Failed to load: %s\n", path);
        return 0;
    }

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, 0);
    stbi_image_free(pixels);

    return tex;
}

void E_material_setTexture(E_material* mat, GLuint texture)
{
    mat->texture = texture;
}

void E_material_setInt(E_material* mat, const char* name, int value)
{
    glUseProgram(mat->shaderProgram);
    glUniform1i(E_getUniformLocation(mat->shaderProgram, name), value);
    glUseProgram(0);
}

void E_material_setFloat(E_material* mat, const char* name, float value)
{
    glUseProgram(mat->shaderProgram);
    glUniform1f(E_getUniformLocation(mat->shaderProgram, name), value);
    glUseProgram(0);
}

void E_material_setVec3(E_material* mat, const char* name, float x, float y, float z)
{
    glUseProgram(mat->shaderProgram);
    glUniform3f(E_getUniformLocation(mat->shaderProgram, name), x, y, z);
    glUseProgram(0);
}

void E_material_setVec4(E_material* mat, const char* name, float x, float y, float z, float w)
{
    glUseProgram(mat->shaderProgram);
    glUniform4f(E_getUniformLocation(mat->shaderProgram, name), x, y, z, w);
    glUseProgram(0);
}

void E_material_setMat4(E_material* mat, const char* name, const float* matrix)
{
    glUseProgram(mat->shaderProgram);
    glUniformMatrix4fv(E_getUniformLocation(mat->shaderProgram, name), 1, GL_FALSE, matrix);
    glUseProgram(0);
}
