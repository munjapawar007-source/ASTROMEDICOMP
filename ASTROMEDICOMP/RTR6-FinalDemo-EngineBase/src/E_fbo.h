#pragma once
#include <GL/glew.h>
#include <gl/GL.h>
#include "E_sceneObject.h"

typedef struct {
    GLuint fbo;
    GLuint colorTexture;
    GLuint emissiveTexture;
    GLuint godRayTexture;
    GLuint depthTexture;
    int    width;
    int    height;
} E_fbo;

extern E_fbo         engineFBO;
extern E_sceneObject enginePostProcessingQuad;

void E_fbo_init   (E_fbo* fbo, int width, int height);
void E_fbo_resize (E_fbo* fbo, int width, int height);
void E_fbo_bind   (E_fbo* fbo);
void E_fbo_unbind ();
void E_fbo_destroy(E_fbo* fbo);

void E_initPostProcessingQuad();
