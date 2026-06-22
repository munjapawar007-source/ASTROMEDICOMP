/*
 *   Copyright (c) 2025
 *   All rights reserved.
 */

#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/glew.h>
#include <gl/GL.h>

#include "vmath.h"
using namespace vmath;

#include "E_utils.h"
#include "E_shader.h"
#include "E_camera.h"
#include "E_mesh.h"
#include "E_sceneObject.h"
#include "E_inputs.h"
#include "E_scene.h"
#include "E_fbo.h"
#include "E_postProcess.h"

#include "DEMO/public/DEMO.hpp"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

#include "engineResources/base/Icon.h"

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

BOOL gbActiveWindow = FALSE;
BOOL gbEscapeKeyIsPressed = FALSE;
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
DWORD dwStyle;
WINDOWPLACEMENT wpPrev;

char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

HDC ghdc = NULL;
HGLRC ghrc = NULL;

E_scene engineScene;
E_camera engineCamera;
E_fbo engineFBO;
E_sceneObject enginePostProcessingQuad;

static double s_prevTime = 0.0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
    int initialize(void);
    void display(void);
    void update(void);
    void uninitialize(void);

    WNDCLASSEX wndclass;
    HWND hwnd;
    MSG msg;
    TCHAR szAppName[] = TEXT("RTR6");
    BOOL bDone = FALSE;

    gpFile = fopen(gszLogFileName, "w");
    if (gpFile == NULL)
    {
        MessageBox(NULL, TEXT("Log file creation failed!"), TEXT("Error!"), MB_OK);
        exit(0);
    }
    else
    {
        fprintf(gpFile, "Program started successfully \n");
    }

    wndclass.cbSize = sizeof(WNDCLASSEX);
    wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    wndclass.lpfnWndProc = WndProc;
    wndclass.hInstance = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName = NULL;
    wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

    RegisterClassEx(&wndclass);

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    hwnd = CreateWindowEx(WS_EX_APPWINDOW,
                          szAppName,
                          TEXT("SWEET - RTR6"),
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
                          screenWidth / 2 - WIN_WIDTH / 2,
                          screenHeight / 2 - WIN_HEIGHT / 2,
                          WIN_WIDTH,
                          WIN_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ghwnd = hwnd;

    ShowWindow(hwnd, iCmdShow);
    UpdateWindow(hwnd);

    int result = initialize();

    if (result != 0)
    {
        fprintf(gpFile, "initiaize() failed \n");
        DestroyWindow(hwnd);
        hwnd = NULL;
    }
    else
    {
        fprintf(gpFile, "initialize() completed successfully \n");
    }

    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    while (bDone == FALSE)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bDone = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if (gbActiveWindow == TRUE)
            {
                if (gbEscapeKeyIsPressed == TRUE)
                {
                    bDone = TRUE;
                }

                display();
                update();
            }
        }
    }

    uninitialize();

    return ((int)msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    void toggleFullscreen(void);
    void resize(int, int);
    void uninitialize(void);

    switch (iMsg)
    {
    case WM_CREATE:
        ZeroMemory((void *)&wpPrev, sizeof(WINDOWPLACEMENT));
        wpPrev.length = sizeof(WINDOWPLACEMENT);
        break;
    case WM_SETFOCUS:
        gbActiveWindow = TRUE;
        break;
    case WM_KILLFOCUS:
        gbActiveWindow = FALSE;
        break;
    case WM_ERASEBKGND:
        return (0);
    case WM_SIZE:
        resize(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_KEYDOWN:
        E_inputs_onKeyDown((int)wParam);
        if (wParam == VK_ESCAPE) gbEscapeKeyIsPressed = TRUE;
        break;
    case WM_KEYUP:
        if (wParam == 'F')
        {
            toggleFullscreen();
            gbFullScreen = !gbFullScreen;
        }
        E_inputs_onKeyUp((int)wParam);
        break;
    case WM_MOUSEMOVE:
        E_inputs_onMouseMove((int)LOWORD(lParam), (int)HIWORD(lParam));
        break;
    case WM_RBUTTONDOWN:
        E_inputs_onMouseButton(1);
        break;
    case WM_RBUTTONUP:
        E_inputs_onMouseButton(0);
        break;
    case WM_CLOSE:
        uninitialize();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }

    return (DefWindowProc(hwnd, iMsg, wParam, lParam));
}

void toggleFullscreen(void)
{
    MONITORINFO mi;

    if (gbFullScreen == FALSE)
    {
        dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
        if (dwStyle & WS_OVERLAPPEDWINDOW)
        {
            ZeroMemory((void *)&mi, sizeof(MONITORINFO));
            mi.cbSize = sizeof(MONITORINFO);

            if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
            {
                SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
                SetWindowPos(ghwnd, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }
    }
    else
    {
        SetWindowPlacement(ghwnd, &wpPrev);
        SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
        SetWindowPos(ghwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
        ShowCursor(TRUE);
    }
}

int initialize(void)
{
    void printGLInfo(void);
    void resize(int, int);
    void uninitialize(void);

    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;
    GLenum glewResult;

    ZeroMemory((void *)&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cRedBits = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;

    ghdc = GetDC(ghwnd);

    if (ghdc == NULL)
    {
        fprintf(gpFile, "GetDC function failed! \n");
        return (-1);
    }

    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);

    if (iPixelFormatIndex == 0)
    {
        fprintf(gpFile, "ChoosePixelFormat function failed! \n");
        return (-2);
    }

    if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
    {
        fprintf(gpFile, "SetPixelFormat function failed! \n");
        return (-3);
    }

    ghrc = wglCreateContext(ghdc);

    if (ghrc == NULL)
    {
        fprintf(gpFile, "wglCreateContext function failed! \n");
        return (-4);
    }

    if (wglMakeCurrent(ghdc, ghrc) == FALSE)
    {
        fprintf(gpFile, "wglMakeCurrent function failed! \n");
        return (-5);
    }

    glewResult = glewInit();
    if (glewResult != GLEW_OK)
    {
        fprintf(gpFile, "glewInit() function failed! \n");
        return (-6);
    }

    printGLInfo();

    {
        typedef BOOL (APIENTRY *PFNWGLSWAPINTERVALEXTPROC)(int);
        PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT_fn = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
        if (wglSwapIntervalEXT_fn)
            wglSwapIntervalEXT_fn(1);
    }

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

    E_initDefaultEngineResources();
    E_spline_renderer_init();
    E_particle_renderer_init();
    E_initCamera();
    E_fbo_init(&engineFBO, WIN_WIDTH, WIN_HEIGHT);
    E_initPostProcessingQuad();
    E_material_setFloat(&enginePostProcessingQuad.material, "uGamma", 1.6f);
    E_material_setInt(&enginePostProcessingQuad.material, "uDepthTexture", 1);
    E_material_setInt(&enginePostProcessingQuad.material, "uEmissiveTex", 2);
    E_material_setInt(&enginePostProcessingQuad.material, "uGodRayTex", 3);
    DEMO_init();

    if (!gbFullScreen) { toggleFullscreen(); gbFullScreen = TRUE; }
    ShowCursor(FALSE);

    RECT rc;
    GetClientRect(ghwnd, &rc);

    fprintf(gpFile, "\nWIN_WIDTH %d, WIN_HEIGHT %d\n", WIN_WIDTH, WIN_HEIGHT);
    fprintf(gpFile, "Client Area Width %d, Client Area Height %d\n\n", rc.right - rc.left, rc.bottom - rc.top);

    resize(rc.right - rc.left, rc.bottom - rc.top);

    return (0);
}

void display(void)
{
    E_frustum_update();

    E_fbo_bind(&engineFBO);
    if (engineScene.fog.enabled)
        glClearColor(engineScene.fog.color[0], engineScene.fog.color[1], engineScene.fog.color[2], 1.0f);
    else
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (E_FLAG_WIREFRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    DEMO_render();
    if (E_FLAG_WIREFRAME) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    E_fbo_unbind();

    glViewport(0, 0, engineFBO.width, engineFBO.height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    E_material_setTexture(&enginePostProcessingQuad.material, engineFBO.colorTexture);
    E_material_setFloat(&enginePostProcessingQuad.material, "uZNear", engineCamera.zNear);
    E_material_setFloat(&enginePostProcessingQuad.material, "uZFar", engineCamera.zFar);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, engineFBO.depthTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, engineFBO.emissiveTexture);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, engineFBO.godRayTexture);
    glActiveTexture(GL_TEXTURE0);
    E_material_setInt(&enginePostProcessingQuad.material, "uPassthrough", !E_FLAG_POSTPROCESS);
    E_postProcess_bindOverlay();
    E_renderSceneObject(&enginePostProcessingQuad, NULL, NULL);

    SwapBuffers(ghdc);
}

void update(void)
{
    double now = E_Utils_getElapsedTimeInSeconds();
    float dt = (float)(now - s_prevTime);
    s_prevTime = now;

    E_camera_updateSplineFollow(&engineScene, dt);
    if (E_camera_isManualControl() && !engineScene.splineFollow)
        E_updateCamera(dt);
    DEMO_update(dt);
    E_camera_updateShake(dt);
}

void printGLInfo(void)
{
    GLint numExtensions, i;

    fprintf(gpFile, "OPENGL INFORMATION \n");
    fprintf(gpFile, "****************** \n");
    fprintf(gpFile, "OpenGL Vendor : %s \n", glGetString(GL_VENDOR));
    fprintf(gpFile, "OpenGL Renderer : %s \n", glGetString(GL_RENDERER));
    fprintf(gpFile, "OpenGL Version : %s \n", glGetString(GL_VERSION));
    fprintf(gpFile, "GLSL Version : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));
    fprintf(gpFile, "****************** \n");

    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);

    for (i = 0; i < numExtensions; i++)
    {
        fprintf(gpFile, "Extension %d: %s\n", i, glGetStringi(GL_EXTENSIONS, i));
    }
}

void resize(int width, int height)
{
    if (height <= 0)
    {
        height = 1;
    }

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    E_resizeCamera(width, height);

    if (engineFBO.fbo)
        E_fbo_resize(&engineFBO, width, height);
}

void uninitialize(void)
{
    void toggleFullscreen(void);

    DEMO_uninit();
    E_fbo_destroy(&engineFBO);

    if (gbFullScreen == TRUE)
    {
        toggleFullscreen();
        gbFullScreen = FALSE;
    }

    if (wglGetCurrentContext() == ghrc)
    {
        wglMakeCurrent(NULL, NULL);
    }

    if (ghrc)
    {
        wglDeleteContext(ghrc);
    }
    ghrc = NULL;

    if (ghdc)
    {
        ReleaseDC(ghwnd, ghdc);
    }
    ghrc = NULL;

    if (ghwnd)
    {
        DestroyWindow(ghwnd);
    }
    ghwnd = NULL;

    if (gpFile)
    {
        fprintf(gpFile, "Program terminated successfully \n");
        fclose(gpFile);
        gpFile = NULL;
    }
}
