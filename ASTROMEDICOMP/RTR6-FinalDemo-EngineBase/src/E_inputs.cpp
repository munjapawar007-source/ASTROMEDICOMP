#include "E_inputs.h"
#include <Windows.h>

int   g_mouseLeftHeld = 0;
float g_mouseDeltaX   = 0.0f;
float g_mouseDeltaY   = 0.0f;

int g_key_W = 0, g_key_A = 0, g_key_S = 0, g_key_D = 0;
int g_key_Up = 0, g_key_Down = 0, g_key_Left = 0, g_key_Right = 0;
int g_key_F = 0;

static int s_prevMouseX = -1;
static int s_prevMouseY = -1;

void E_inputs_onMouseMove(int x, int y)
{
    if (s_prevMouseX >= 0 && g_mouseLeftHeld)
    {
        g_mouseDeltaX = (float)(x - s_prevMouseX);
        g_mouseDeltaY = (float)(y - s_prevMouseY);
    }
    s_prevMouseX = x;
    s_prevMouseY = y;
}

void E_inputs_onMouseButton(int held)
{
    g_mouseLeftHeld = held;
    if (!held) { s_prevMouseX = -1; s_prevMouseY = -1; }
}

void E_inputs_resetDeltas()
{
    g_mouseDeltaX = 0.0f;
    g_mouseDeltaY = 0.0f;
}

void E_inputs_onKeyDown(int vkey)
{
    switch (vkey)
    {
    case 'W':       g_key_W     = 1; break;
    case 'A':       g_key_A     = 1; break;
    case 'S':       g_key_S     = 1; break;
    case 'D':       g_key_D     = 1; break;
    case VK_UP:     g_key_Up    = 1; break;
    case VK_DOWN:   g_key_Down  = 1; break;
    case VK_LEFT:   g_key_Left  = 1; break;
    case VK_RIGHT:  g_key_Right = 1; break;
    case 'F':       g_key_F     = 1; break;
    }
}

void E_inputs_onKeyUp(int vkey)
{
    switch (vkey)
    {
    case 'W':       g_key_W     = 0; break;
    case 'A':       g_key_A     = 0; break;
    case 'S':       g_key_S     = 0; break;
    case 'D':       g_key_D     = 0; break;
    case VK_UP:     g_key_Up    = 0; break;
    case VK_DOWN:   g_key_Down  = 0; break;
    case VK_LEFT:   g_key_Left  = 0; break;
    case VK_RIGHT:  g_key_Right = 0; break;
    case 'F':       g_key_F     = 0; break;
    }
}
