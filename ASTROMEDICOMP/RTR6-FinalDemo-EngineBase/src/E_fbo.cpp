#include "E_fbo.h"
#include "E_mesh.h"
#include "E_material.h"

void E_fbo_init(E_fbo* fbo, int width, int height)
{
    fbo->width  = width;
    fbo->height = height;

    glGenFramebuffers(1, &fbo->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);

    glGenTextures(1, &fbo->colorTexture);
    glBindTexture(GL_TEXTURE_2D, fbo->colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo->colorTexture, 0);

    glGenTextures(1, &fbo->emissiveTexture);
    glBindTexture(GL_TEXTURE_2D, fbo->emissiveTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, fbo->emissiveTexture, 0);

    glGenTextures(1, &fbo->godRayTexture);
    glBindTexture(GL_TEXTURE_2D, fbo->godRayTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, fbo->godRayTexture, 0);

    GLenum drawBuffers[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, drawBuffers);

    glGenTextures(1, &fbo->depthTexture);
    glBindTexture(GL_TEXTURE_2D, fbo->depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fbo->depthTexture, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void E_fbo_resize(E_fbo* fbo, int width, int height)
{
    fbo->width  = width;
    fbo->height = height;

    glBindTexture(GL_TEXTURE_2D, fbo->colorTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, fbo->emissiveTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, fbo->godRayTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, fbo->depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void E_fbo_bind(E_fbo* fbo)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->fbo);
    glViewport(0, 0, fbo->width, fbo->height);
}

void E_fbo_unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void E_fbo_destroy(E_fbo* fbo)
{
    glDeleteTextures(1, &fbo->colorTexture);
    glDeleteTextures(1, &fbo->emissiveTexture);
    glDeleteTextures(1, &fbo->godRayTexture);
    glDeleteTextures(1, &fbo->depthTexture);
    glDeleteFramebuffers(1, &fbo->fbo);
    fbo->colorTexture    = 0;
    fbo->emissiveTexture = 0;
    fbo->godRayTexture   = 0;
    fbo->depthTexture    = 0;
    fbo->fbo             = 0;
}

void E_initPostProcessingQuad()
{
    static const float positions[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f
    };
    static const float normals[] = {
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, 1.0f
    };
    static const float texcoords[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f
    };
    static const unsigned int indices[] = { 0,1,2, 2,3,0 };

    enginePostProcessingQuad      = E_createSceneObject("PostProcessQuad");
    enginePostProcessingQuad.mesh = E_createMeshFromData(positions, 4, normals, texcoords, indices, 6);
    enginePostProcessingQuad.mesh.noCull = 1;
    enginePostProcessingQuad.material = E_createMaterialFromFiles(
        "engineResources/postprocess.vert",
        "engineResources/postprocess.frag"
    );
}
