// win32 headers
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

// OpenGl related headers
#include <GL/glew.h> // MUST be included before gl.h
#include <gl/GL.h>

enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR    = 1,
    AMC_ATTRIBUTE_NORMAL   = 2,
    AMC_ATTRIBUTE_TEXCOORD = 3,
    AMC_ATTRIBUTE_JOINTS   = 4,
    AMC_ATTRIBUTE_WEIGHTS  = 5,
};

extern GLuint uberShaderProgramObject;
extern GLuint skinnedShaderProgramObject;
extern GLuint shadowShaderProgramObject;
extern GLuint defaultTexture;

GLuint E_compileShader(const char* source, GLenum shaderType);
GLuint E_createProgram(GLuint vsObject, GLuint fsObject);
void E_initDefaultEngineResources();