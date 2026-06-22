#include "E_camera.h"
#include "E_inputs.h"
#include "E_postProcess.h"
#include "E_engineFlags.h"
#include <math.h>
#include <stdio.h>

static bool           s_cameraManualControl = true;
static E_sceneObject* s_focusNode           = NULL;

static E_sceneObject* s_splineFollower[E_SPLINE_MAX_COUNT];
static float          s_splineFollowerT[E_SPLINE_MAX_COUNT];
static float          s_splineFollowerSpeed[E_SPLINE_MAX_COUNT];

static float s_shakeTrauma    = 0.0f;
static float s_shakeTime      = 0.0f;
static float s_shakeDecayRate = 0.8f;
static float s_shakeMaxYaw    = 0.03f;
static float s_shakeMaxPitch  = 0.02f;

void E_initCamera()
{
    engineCamera.position  = vmath::vec3(0.0f, 2.0f, 8.0f);
    engineCamera.yaw       = 0.0f;
    engineCamera.pitch     = -0.2f;
    engineCamera.moveSpeed = 0.05f;

    engineCamera.fov    = 45.0f;
    engineCamera.zNear  = 0.1f;
    engineCamera.zFar   = 1000.0f;
    engineCamera.width  = 800.0f;
    engineCamera.height = 600.0f;

    engineCamera.projectionMatrix = vmath::perspective(
        engineCamera.fov,
        engineCamera.width / engineCamera.height,
        engineCamera.zNear,
        engineCamera.zFar);

    engineCamera.focusPoint = vmath::vec3(0.0f, 0.0f, 0.0f);
    E_buildCameraView();
}

void E_buildCameraView()
{
    float shake = s_shakeTrauma * s_shakeTrauma;
    float t     = s_shakeTime;
    float shakeYaw   = shake * s_shakeMaxYaw   * (0.6f * sinf(t * 23.1f) + 0.3f * sinf(t * 47.3f) + 0.1f * sinf(t * 91.7f));
    float shakePitch = shake * s_shakeMaxPitch * (0.6f * sinf(t * 29.7f) + 0.3f * sinf(t * 53.1f) + 0.1f * sinf(t * 71.3f));

    float yaw   = engineCamera.yaw   + shakeYaw;
    float pitch = engineCamera.pitch + shakePitch;
    vmath::vec3 forward = vmath::normalize(vmath::vec3(
        cosf(pitch) * sinf(yaw),
        sinf(pitch),
       -cosf(pitch) * cosf(yaw)));
    vmath::vec3 worldUp = vmath::vec3(0.0f, 1.0f, 0.0f);
    vmath::vec3 right   = vmath::normalize(vmath::cross(forward, worldUp));
    vmath::vec3 up      = vmath::cross(right, forward);
    engineCamera.front      = engineCamera.position + forward;
    engineCamera.viewMatrix = vmath::lookat(engineCamera.position, engineCamera.front, up);
}

void E_updateCamera(float deltaTime)
{
    float mouseSensitivity = 0.002f;
    if (g_mouseLeftHeld)
    {
        engineCamera.yaw   += g_mouseDeltaX * mouseSensitivity;
        engineCamera.pitch -= g_mouseDeltaY * mouseSensitivity;
    }

    float rotateSpeed = 1.5f * deltaTime;
    if (g_key_Left)  engineCamera.yaw   -= rotateSpeed;
    if (g_key_Right) engineCamera.yaw   += rotateSpeed;
    if (g_key_Up)    engineCamera.pitch += rotateSpeed;
    if (g_key_Down)  engineCamera.pitch -= rotateSpeed;

    if (engineCamera.pitch >  1.5f) engineCamera.pitch =  1.5f;
    if (engineCamera.pitch < -1.5f) engineCamera.pitch = -1.5f;

    float yaw   = engineCamera.yaw;
    float pitch = engineCamera.pitch;

    vmath::vec3 forward = vmath::vec3(
        cosf(pitch) * sinf(yaw),
        sinf(pitch),
       -cosf(pitch) * cosf(yaw));

    forward            = vmath::normalize(forward);
    vmath::vec3 worldUp = vmath::vec3(0.0f, 1.0f, 0.0f);
    vmath::vec3 right   = vmath::normalize(vmath::cross(forward, worldUp));
    vmath::vec3 up      = vmath::cross(right, forward);

    float speed = engineCamera.moveSpeed * deltaTime * 60.0f;
    if (g_key_W) engineCamera.position = engineCamera.position + (forward * speed);
    if (g_key_S) engineCamera.position = engineCamera.position - (forward * speed);
    if (g_key_A) engineCamera.position = engineCamera.position - (right   * speed);
    if (g_key_D) engineCamera.position = engineCamera.position + (right   * speed);

    E_buildCameraView();
    E_inputs_resetDeltas();
}

void E_destroyCamera()
{
}

void E_camera_addTrauma(float amount)
{
    s_shakeTrauma += amount;
    if (s_shakeTrauma > 1.0f) s_shakeTrauma = 1.0f;
}

void E_camera_updateShake(float dt)
{
    if (s_shakeTrauma <= 0.0f) return;
    s_shakeTime   += dt;
    s_shakeTrauma -= s_shakeDecayRate * dt;
    if (s_shakeTrauma < 0.0f) s_shakeTrauma = 0.0f;
    E_buildCameraView();
}

void E_camera_setShakeParams(float maxYaw, float maxPitch, float decayRate)
{
    s_shakeMaxYaw    = maxYaw;
    s_shakeMaxPitch  = maxPitch;
    s_shakeDecayRate = decayRate;
}

void E_resizeCamera(float width, float height)
{
    engineCamera.width  = width;
    engineCamera.height = height;
    engineCamera.projectionMatrix = vmath::perspective(
        engineCamera.fov,
        engineCamera.width / engineCamera.height,
        engineCamera.zNear,
        engineCamera.zFar);
}

void E_setFOV(float fov)
{
    engineCamera.fov = fov;
    engineCamera.projectionMatrix = vmath::perspective(
        engineCamera.fov,
        engineCamera.width / engineCamera.height,
        engineCamera.zNear,
        engineCamera.zFar);
}

void E_camera_setPosition(float x, float y, float z)
{
    engineCamera.position = vmath::vec3(x, y, z);
    E_buildCameraView();
}

void E_camera_setPosition(vmath::vec3 pos)
{
    engineCamera.position = pos;
    E_buildCameraView();
}

void E_camera_setFocusPoint(float x, float y, float z)
{
    engineCamera.focusPoint = vmath::vec3(x, y, z);
    E_buildCameraView();
}

void E_camera_lookAtPoint(float x, float y, float z)
{
    float dx = x - engineCamera.position[0];
    float dy = y - engineCamera.position[1];
    float dz = z - engineCamera.position[2];
    float len = sqrtf(dx*dx + dy*dy + dz*dz);
    if (len < 0.0001f) return;
    engineCamera.pitch = asinf(dy / len);
    engineCamera.yaw   = atan2f(dx, -dz);
    E_buildCameraView();
}

void E_camera_lookAtPoint(vmath::vec3 pos)
{
    E_camera_lookAtPoint(pos[0], pos[1], pos[2]);
}

void E_camera_lookAtObject(E_sceneObject* obj)
{
    if (!obj) return;
    E_camera_lookAtPoint(obj->position[0], obj->position[1], obj->position[2]);
}

void E_camera_setProjection(float fov, float zNear, float zFar)
{
    engineCamera.zNear = zNear;
    engineCamera.zFar  = zFar;
    E_setFOV(fov);
}

int E_camera_isManualControl()
{
    return s_cameraManualControl ? 1 : 0;
}

void E_camera_updateSplineFollow(E_scene* scene, float dt)
{
    if (!s_cameraManualControl && s_focusNode)
        E_camera_lookAtObject(s_focusNode);

    if (!scene->splineFollow) return;
    if (scene->activeSpline < 0 || scene->activeSpline >= scene->splineCount) return;

    E_spline* s = &scene->splines[scene->activeSpline];
    if (s->numPoints < 4) return;

    scene->splineT += scene->splineDir * scene->splineSpeed * dt;
    if (scene->splineT >= 1.0f) { scene->splineT = 1.0f; scene->splineDir = -1.0f; }
    if (scene->splineT <= 0.0f) { scene->splineT = 0.0f; scene->splineDir =  1.0f; }

    engineCamera.position = E_spline_getPositionAtT(s, scene->splineT);

    vmath::vec3 toFocus = engineCamera.focusPoint - engineCamera.position;
    float len = sqrtf(toFocus[0]*toFocus[0] + toFocus[1]*toFocus[1] + toFocus[2]*toFocus[2]);
    if (len > 0.0001f)
    {
        toFocus        = toFocus * (1.0f / len);
        engineCamera.pitch = asinf(toFocus[1]);
        engineCamera.yaw   = atan2f(toFocus[0], -toFocus[2]);
    }
    E_buildCameraView();

    for (int si = 0; si < scene->splineCount; si++)
    {
        if (!s_splineFollower[si]) continue;
        E_spline* sp = &scene->splines[si];
        if (sp->numPoints < 4) continue;
        if (E_FLAG_ANIMATIONS)
            s_splineFollowerT[si] += s_splineFollowerSpeed[si] * dt;
        if (s_splineFollowerT[si] > 1.0f) s_splineFollowerT[si] -= 1.0f;
        E_sceneObject_setPosition(s_splineFollower[si], E_spline_getPositionAtT(sp, s_splineFollowerT[si]));
        E_sceneObject_setRotation(s_splineFollower[si], E_spline_getRotationAtT(sp, s_splineFollowerT[si]));
    }
}

