#pragma once
#include "E_anim.h"
#include "vmath.h"

void        E_animator_setClip      (E_animator* anim, int index);
void        E_animator_setClipByName(E_animator* anim, const char* name);
void        E_animator_setSpeed     (E_animator* anim, float speed);
void        E_animator_setTime      (E_animator* anim, float time);
void        E_animator_pause        (E_animator* anim);
void        E_animator_resume       (E_animator* anim);
int         E_animator_getClipCount (E_animator* anim);
const char* E_animator_getClipName  (E_animator* anim, int index);
float       E_animator_getDuration  (E_animator* anim);

// Bone attachment
struct E_sceneObject_s* E_animator_getBoneNode    (E_animator* anim, const char* boneName);
vmath::vec3             E_animator_getBonePosition(E_animator* anim, const char* boneName);
vmath::vec3             E_animator_getBoneRotation(E_animator* anim, const char* boneName);
