#pragma once

// Mouse state
extern int   g_mouseLeftHeld;
extern float g_mouseDeltaX;
extern float g_mouseDeltaY;

// Key state
extern int g_key_W, g_key_A, g_key_S, g_key_D;
extern int g_key_Up, g_key_Down, g_key_Left, g_key_Right;
extern int g_key_F;

void E_inputs_onMouseMove(int x, int y);
void E_inputs_onMouseButton(int held);
void E_inputs_resetDeltas();
void E_inputs_onKeyDown(int vkey);
void E_inputs_onKeyUp(int vkey);
