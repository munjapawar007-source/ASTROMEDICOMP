#include "E_shader.h"
#include "E_utils.h"
#include "E_material.h"

extern FILE *gpFile;
GLuint uberShaderProgramObject;
GLuint skinnedShaderProgramObject;
GLuint shadowShaderProgramObject;
GLuint defaultTexture;

GLuint E_compileShader(const char* source, GLenum shaderType)
{
    GLuint shaderObject = glCreateShader(shaderType);
    glShaderSource(shaderObject, 1, (const GLchar **)&source, NULL);
    glCompileShader(shaderObject);

    GLint status = 0;
    GLint infoLogLength = 0;
    GLchar *szInfoLog = NULL;
    glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);

    char* shaderTypeLog = NULL; 
    
    switch(shaderType)
    {
        case GL_VERTEX_SHADER:
            shaderTypeLog = "VERTEX ";
            break;
        case GL_FRAGMENT_SHADER:
            shaderTypeLog = "FRAGMENT ";
            break;
    }

    if (status == GL_FALSE)
    {
        glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar));
            if (szInfoLog != NULL)
            {
                glGetShaderInfoLog(shaderObject, infoLogLength, NULL, szInfoLog);
                fprintf(gpFile, "%sShader compilation log:\n%s\n", shaderTypeLog, szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
    }
    else
    {
        fprintf(gpFile, "%sShader compilation Success\n", shaderTypeLog);
    }

    return shaderObject;
}

GLuint E_createProgram(GLuint vsObject, GLuint fsObject)
{
    GLuint shaderProgramObject = glCreateProgram();
    glAttachShader(shaderProgramObject, vsObject);
    glAttachShader(shaderProgramObject, fsObject);

    // bind shader arrtibute in shader to same index in host program
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_COLOR, "aColor");

    glLinkProgram(shaderProgramObject);

    GLint status = 0;
    GLint infoLogLength = 0;
    char *szInfoLog = NULL;
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
        if (infoLogLength > 0)
        {
            szInfoLog = (GLchar *)malloc(infoLogLength * sizeof(GLchar));
            if (szInfoLog != NULL)
            {
                glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
                fprintf(gpFile, "Shader Program Linking log:\n%s\n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
    }
    else
    {
        fprintf(gpFile, "Shader Program Linking Success\n");
    }

    return shaderProgramObject;
}

void E_initDefaultEngineResources()
{
    const GLchar *uberVertexShaderSourceCode = E_Utils_LoadFile("engineResources/uber.vert");
    const GLchar *uberFragmentShaderSourceCode = E_Utils_LoadFile("engineResources/uber.frag");

    GLuint vertexShaderObject = E_compileShader(uberVertexShaderSourceCode, GL_VERTEX_SHADER);
    GLuint fragmentShaderObject = E_compileShader(uberFragmentShaderSourceCode, GL_FRAGMENT_SHADER);
    uberShaderProgramObject = E_createProgram(vertexShaderObject, fragmentShaderObject);

    const GLchar* skinnedVertSrc = E_Utils_LoadFile("engineResources/skinned.vert");
    GLuint skinnedVS = E_compileShader(skinnedVertSrc, GL_VERTEX_SHADER);
    skinnedShaderProgramObject = E_createProgram(skinnedVS, fragmentShaderObject);

    const GLchar* shadowVertSrc = E_Utils_LoadFile("engineResources/shadow.vert");
    const GLchar* shadowFragSrc = E_Utils_LoadFile("engineResources/shadow.frag");
    GLuint shadowVS = E_compileShader(shadowVertSrc, GL_VERTEX_SHADER);
    GLuint shadowFS = E_compileShader(shadowFragSrc, GL_FRAGMENT_SHADER);
    shadowShaderProgramObject = E_createProgram(shadowVS, shadowFS);

    defaultTexture = E_loadTexture("engineResources/textures/defaultColoredGrid.jpeg");

}