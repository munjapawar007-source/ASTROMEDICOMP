#pragma once
#include "vmath.h"
#include "E_scene.h"

typedef struct
{
    vmath::mat4 viewMatrix;
    vmath::mat4 projectionMatrix;

    vmath::vec3 position;
    vmath::vec3 front;          // lookat target (position + forward dir)
    vmath::vec3 focusPoint;     // fixed world target used when not in manual mode

    float yaw;                  // radians, horizontal rotation
    float pitch;                // radians, vertical rotation
    float moveSpeed;

    float fov;
    float width;
    float height;
    float zNear;
    float zFar;
} E_camera;

extern E_camera engineCamera;

void E_initCamera();
void E_buildCameraView();
void E_updateCamera(float deltaTime);
void E_destroyCamera();
void E_resizeCamera(float width, float height);
void E_setFOV(float fov);

void E_camera_setPosition  (float x, float y, float z);
void E_camera_setPosition  (vmath::vec3 pos);
void E_camera_setFocusPoint(float x, float y, float z);
void E_camera_lookAtPoint  (float x, float y, float z);
void E_camera_lookAtPoint  (vmath::vec3 pos);
void E_camera_lookAtObject (E_sceneObject* obj);
void E_camera_setProjection(float fov, float zNear, float zFar);

int  E_camera_isManualControl();
void E_camera_updateSplineFollow(E_scene* scene, float dt);

void E_camera_addTrauma   (float amount);
void E_camera_updateShake (float dt);
void E_camera_setShakeParams(float maxYaw, float maxPitch, float decayRate);
