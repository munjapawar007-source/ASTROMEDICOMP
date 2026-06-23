// common header files
#include <windows.h> // Win32 API
#include <stdio.h>  // for file IO
#include <stdlib.h> // for exit()

// OpenGL header files
#include <gl/glew.h> // for the Graphics Library Extension Wrangler (GLEW) library - MUST be included before GL.h
#include <gl/GL.h>   // for OpenGL API

// vmath (Vermillion Math)
#include "vmath.h"
using namespace vmath; // we will not be using short-forms. writing this line just to understand the syntax

// user-defined headers
#include "OGL.h"
#include "Sphere.h"

// macros
#define WIN_WIDTH  800
#define WIN_HEIGHT 600
#define WIN_TITLE  L"Flashlight on mouse"

// for writing a log + auto-flush the buffer
#define WRITE_LOG(...) { fprintf(gpFILE, __VA_ARGS__); fflush(gpFILE); }

// for log file beautification
#define BORDER "*********************************************************************************\n"

// link with the OpenGL library
#pragma comment(lib, "OpenGL32.lib")

// link with the GLEW library
#pragma comment(lib, "glew32.lib")

// link with the Sphere library
#pragma comment(lib, "Sphere.lib")

// global function declarations
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// global variable declarations
// for fullscreen
HWND ghwnd             = NULL;
BOOL gbActive          = FALSE;
DWORD dwStyle          = 0;
WINDOWPLACEMENT wpPrev = {sizeof(WINDOWPLACEMENT)};
BOOL gbFullscreen      = FALSE;

// for file IO
FILE *gpFILE = NULL;

// OpenGL related global variables
HDC ghdc   = NULL;
HGLRC ghrc = NULL;

GLuint shaderProgramObject  = 0;

enum
{
    AMC_ATTRIBUTE_POSITION = 0,
    AMC_ATTRIBUTE_COLOR,
    AMC_ATTRIBUTE_TEXCOORD,
    AMC_ATTRIBUTE_NORMAL
};

// for sphere
// VAO and VBOs
GLuint vao          = 0;
GLuint vbo_position = 0;
GLuint vbo_normal   = 0;
GLuint vbo_texcoord = 0;
GLuint vbo_elements = 0;

// to get number of sphere elements
GLuint numSphereElements = 0;

// uniforms
// mvpMatrixUniform split into 3 :
GLuint modelMatrixUniform      = 0;
GLuint viewMatrixUniform       = 0;
GLuint projectionMatrixUniform = 0;

// light related uniforms
GLuint lightAmbientUniform              = 0; 
GLuint lightDiffuseUniform              = 0;
GLuint lightSpecularUniform             = 0; 
GLuint lightPositionUniform             = 0;
GLuint lightSpotDirectionUniform        = 0;
GLuint lightSpotInnerCutOffAngleUniform = 0;
GLuint lightSpotOuterCutOffAngleUniform = 0;

// material related uniforms
GLuint materialAmbientUniform   = 0;
GLuint materialDiffuseUniform   = 0;
GLuint materialSpecularUniform  = 0;
GLuint materialShininessUniform = 0;

// key press uniform
GLuint keyPressUniform = 0;

// boolean global variables
BOOL bLightingEnabled  = FALSE;

// light properties
GLfloat lightAmbient[]  = {0.1f, 0.1f, 0.1f, 1.0f}; // light-black ambient light (last component is alpha, which is meaningless until blending is enabled)
GLfloat lightDiffuse[]  = {1.0f, 0.0f, 0.0f, 1.0f}; // white diffuse light
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f}; // white specular light
GLfloat lightPosition[] = {0.0f, 0.0f, 20.0f, 1.0f};
                        // x,    y,    z,    w (1 or 0 means something different here)
						// for w : 1 = positional light (table lamp), 0 = directional light (located at infinity, eg. The Sun)

GLfloat lightSpotDirection[]      = {0.0f, 0.0f, -1.0f};
GLfloat lightSpotInnerCutOffAngle = 0.25f;
GLfloat lightSpotOuterCutOffAngle = 1.0f;

// material properties
GLfloat materialAmbient[]   = {0.0f, 0.0f, 0.0f, 1.0f}; // black ambient color for material
GLfloat materialDiffuse[]   = {1.0f, 0.0f, 0.0f, 1.0f}; // white diffuse color for material 
GLfloat materialSpecular[]  = {1.0f, 1.0f, 1.0f, 1.0f}; // white specular color for material
GLfloat materialShininess   = 50.0f; 

BOOL gbLeftMouseButtonDown = FALSE;
int winWidth  = WIN_WIDTH;
int winHeight = WIN_HEIGHT;

// projection matrix
vmath::mat4 perspectiveProjectionMatrix; // mat4 is in vmath.h

#pragma region windowing_functions

// entry-point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
    // function declarations
    int initialize(void);
    void uninitialize(void);
    void display(void);
    void update(void);

    // local variable declarations
    WNDCLASSEXW wndclass;
    HWND hwnd;
    MSG msg;
    WCHAR szAppName[] = L"Window";
    int iResult = 0;
    BOOL bDone = FALSE;

    int iOriginX, iOriginY;

    // code
    // File IO
    if(fopen_s(&gpFILE, "Log.txt", "w") != 0)
    {
        MessageBoxW(NULL, L"Log file cannot be opened.", L"Error", MB_OK | MB_ICONERROR);
        exit(0);
    }
    WRITE_LOG("Program successfully started.\n");
    
    // beautification
    WRITE_LOG(BORDER);

    // WNDCLASSEXW initialization
    wndclass.cbSize        = sizeof(WNDCLASSEXW);
    wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wndclass.cbClsExtra    = 0;
    wndclass.cbWndExtra    = 0;
    wndclass.hInstance     = hInstance;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hIcon         = LoadIconW(hInstance, MAKEINTRESOURCEW(MYICON));
    wndclass.hCursor       = LoadCursorW(NULL, IDC_ARROW);
    wndclass.lpfnWndProc   = WndProc;
    wndclass.lpszClassName = szAppName;
    wndclass.lpszMenuName  = NULL;
    wndclass.hIconSm       = LoadIconW(hInstance, MAKEINTRESOURCEW(MYICON));

    // WNDCLASSEXW registration
    RegisterClassExW(&wndclass);

    // Calculate origin to center the window
    iOriginX = (GetSystemMetrics(SM_CXSCREEN) / 2) - (WIN_WIDTH / 2);
    iOriginY = (GetSystemMetrics(SM_CYSCREEN) / 2) - (WIN_HEIGHT / 2);

    // Create the window in memory
    hwnd = CreateWindowExW(WS_EX_APPWINDOW,
                           szAppName,
                           WIN_TITLE,
                           WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
                           iOriginX,
                           iOriginY,
                           WIN_WIDTH,
                           WIN_HEIGHT,
                           HWND_DESKTOP,
                           NULL,
                           hInstance,
                           NULL);

    // copy local window handle into global window handle
    ghwnd = hwnd;

    // initialization
    iResult = initialize();
    if(iResult != 0)
    {
        MessageBoxW(hwnd, L"initialize() failed.", L"Error", MB_OK | MB_ICONERROR);
        DestroyWindow(hwnd);
    }
    WRITE_LOG("OpenGL was initialized successfully.\n");
    WRITE_LOG(BORDER);

    // Show the window
    ShowWindow(hwnd, iCmdShow);

    SetForegroundWindow(hwnd);
    SetFocus(hwnd);

    // Gameloop
    while(bDone == FALSE)
    {
        if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if(msg.message == WM_QUIT)
                bDone = TRUE;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            if(gbActive == TRUE)
            {
                // Render
                display();

                // Update
                update();
            }            
        }
    }

    // uninitialization
    uninitialize();

    return((int)msg.wParam);
}

// callback function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
    // function prototypes
    void ToggleFullscreen(void);
    void resize(int, int);


    // code
    switch(iMsg)
    {
    case WM_SETFOCUS:
        gbActive = TRUE;
        break;
    case WM_KILLFOCUS:
        gbActive = FALSE;
        break;
    case WM_SIZE:
        resize(LOWORD(lParam), HIWORD(lParam));
        break;
    case WM_ERASEBKGND:
        return(0);
    case WM_KEYDOWN:
        switch(LOWORD(wParam))
        {
        case VK_ESCAPE:
            DestroyWindow(hwnd);
            break;
        default:
            break;
        }
        break;
    case WM_CHAR:
        switch(LOWORD(wParam))
        {
        case 'F':
        case 'f':
            if(gbFullscreen == FALSE)
            {
                ToggleFullscreen();
                gbFullscreen = TRUE;
            }
            else
            {
                ToggleFullscreen();
                gbFullscreen = FALSE;
            }
            break;
        case 'L':
        case 'l':
            if(bLightingEnabled == TRUE)
            {
                bLightingEnabled = FALSE;
            }
            else
            {
                bLightingEnabled = TRUE;
            }
            break;
        default:
            break;
        }
        break;
    case WM_LBUTTONDOWN:
        gbLeftMouseButtonDown = TRUE;
        break;
    case WM_LBUTTONUP:
        gbLeftMouseButtonDown = FALSE;
        break;
    case WM_CLOSE:
        DestroyWindow(hwnd); // to avoid keeping the app running in background
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        break;
    }

    return(DefWindowProcW(hwnd, iMsg, wParam, lParam));
}

void ToggleFullscreen(void)
{
    // local variable declarations
    MONITORINFO monitorinfo = {sizeof(MONITORINFO)};

    // code
    if(gbFullscreen == FALSE)
    {
        dwStyle = GetWindowLongW(ghwnd, GWL_STYLE);
        if(dwStyle & WS_OVERLAPPEDWINDOW)
        {
            if(GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfoW(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &monitorinfo))
            {
                SetWindowLongW(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);

                SetWindowPos(ghwnd,
                             HWND_TOP,
                             monitorinfo.rcMonitor.left,
                             monitorinfo.rcMonitor.top,
                             monitorinfo.rcMonitor.right - monitorinfo.rcMonitor.left,
                             monitorinfo.rcMonitor.bottom - monitorinfo.rcMonitor.top,
                             SWP_NOZORDER | SWP_FRAMECHANGED);
            }
        }

        ShowCursor(FALSE);
    }
    else
    {
        SetWindowPlacement(ghwnd, &wpPrev);

        SetWindowLongW(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);

        SetWindowPos(ghwnd,
                     HWND_TOP,
                     0,
                     0,
                     0,
                     0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED);
    
        ShowCursor(TRUE);
    }
}

#pragma endregion

#pragma region ogl_functions

int initialize(void)
{
    // function declarations
    void printGLInfo(void);
    void uninitialize(void);
    void resize(int width, int height);

    // local variables
    PIXELFORMATDESCRIPTOR pfd;
    int iPixelFormatIndex = 0;
    const char *szBorder  = "**********************************************************************\n";

    // code
    ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR));

    // step 1 - initialization of PIXELFORMATDESCRIPTOR
    pfd.nSize      = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion   = 1;
    pfd.dwFlags    = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA; // same as GLUT_RGBA
    pfd.cColorBits = 32;
    pfd.cRedBits   = 8;
    pfd.cGreenBits = 8;
    pfd.cBlueBits  = 8;
    pfd.cAlphaBits = 8;
    pfd.cDepthBits = 32;

    // step 2 - get the dc
    ghdc = GetDC(ghwnd);
    if(ghdc == NULL)
    {
        WRITE_LOG("GetDC() failed.\n");
        return(-1);
    }

    // step 3 - tell the DC to find a match for our pfd and get index for closest match from OS's many pfds (plural)
    iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
    if(iPixelFormatIndex == 0)
    {
        WRITE_LOG("ChoosePixelFormat() failed.\n");
        return(-2);
    }

    // step 4 - set obtained pixel format
    if(SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)   // returns TRUE if successful, FALSE if failed
    {
        WRITE_LOG("SetPixelFormat() failed.\n");
        return(-3);
    }

    // step 5 - create OpenGL context (rendering context) from device context 
    ghrc = wglCreateContext(ghdc);
    if(ghrc == NULL)
    {
        WRITE_LOG("wglCreateContext() failed.\n");
        return(-4);
    }

    // step 6 - make rendering context current
    if(wglMakeCurrent(ghdc, ghrc) == FALSE)
    {
        WRITE_LOG("wglMakeCurrent() failed.\n");
        return(-5);
    }

    // step 7 - initialize GLEW
    // MOST important code for PP =>
    if(glewInit() != GLEW_OK)
    {
        WRITE_LOG("Critical error : glewInit() failed to initialize GLEW.\nExitting now...\n");
        return(-6);
    }

    // Print OpenGL information to the log file
    printGLInfo();

    // ****************** Vertex Shader ******************
    // step 1 : write the Vertex Shader code
    const GLchar *vertexShaderSourceCode =
    "#version 460 core\n"                                                            \
    "\n"                                                                             \
    "in vec4 aPosition;\n"                                                           \
    "in vec3 aNormal;\n"                                                             \
    "\n"                                                                             \
    "uniform mat4 uModelMatrix;\n"                                                   \
    "uniform mat4 uViewMatrix;\n"                                                    \
    "uniform mat4 uProjectionMatrix;\n"                                              \
    "\n"                                                                             \
    "uniform vec4 uLightPosition;\n"                                                 \
    "\n"                                                                             \
    "uniform int uKeyPress;\n"                                                       \
    "\n"                                                                             \
    "out vec3 oTransformedNormals;\n"                                                \
    "out vec3 oLightDirection;\n"                                                    \
    "out vec3 oViewerVector;\n"                                                      \
    "out vec3 oWorldPosition;\n"                                                     \
    "\n"                                                                             \
    "void main(void)\n"                                                              \
    "{\n"                                                                            \
    "   if(uKeyPress == 1)\n"                                                        \
    "   {\n"                                                                         \
    "       vec4 eyeCoordinates = uViewMatrix * uModelMatrix * aPosition;\n"         \
    "       \n"                                                                      \
    "       oTransformedNormals = mat3(uViewMatrix * uModelMatrix) * aNormal;\n"     \
    "       oLightDirection     = vec3(uLightPosition - eyeCoordinates);\n"          \
    "       oViewerVector       = -eyeCoordinates.xyz;\n"                            \
    "       \n"                                                                      \
    "       oWorldPosition = vec3(uModelMatrix * aPosition);\n"                      \
    "   }\n"                                                                         \
    "   else\n"                                                                      \
    "   {\n"                                                                         \
    "       oTransformedNormals = vec3(0.0, 0.0, 0.0);\n"                            \
    "       oLightDirection     = vec3(0.0, 0.0, 0.0);\n"                            \
    "       oViewerVector       = vec3(0.0, 0.0, 0.0);\n"                            \
    "   }\n"                                                                         \
    "   \n"                                                                          \
    "   gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" \
    "}";

    // step 2 : create Vertex Shader Object
    GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
    
    // step 3 : give the Vertex Shader source code to OpenGL
    glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);
    
    // step 4 : let OpenGL compile the Vertex Shader
    glCompileShader(vertexShaderObject);

    // step 5 : check for Vertex Shader compilation errors
    GLint status        = 0; 
    GLint infoLogLength = 0;
    GLchar *szInfoLog   = NULL;

    // (a) Get compile status
    glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

    // (b) If there are any compilation errors, get length of the compilation error info log
    if(status == GL_FALSE)
    {
        glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        // allocate memory only if the length of the log is valid
        if(infoLogLength > 0)
        {
            // (c) Declare a string variable and allocate memory equal to above length
            szInfoLog = (GLchar*)malloc((infoLogLength + 1) * sizeof(GLchar));

            // if memory is allocated successfully
            if(szInfoLog != NULL)
            {
                // (d) Get the compilation error info log into this memory allocated string
                glGetShaderInfoLog(vertexShaderObject, infoLogLength + 1, NULL, szInfoLog);

                // (e) Print the compilation error info log
                WRITE_LOG("Vertex Shader Compilation Error Log : %s\n", szInfoLog);
    
                // for separating extensions and error log
                WRITE_LOG(BORDER);

                // (f) Free the allocated memory
                free(szInfoLog);
                szInfoLog = NULL; // just a pre-caution
            }
        }

        // (g) uninitialize() and exit (DestroyWindow())
        uninitialize();
    }
    WRITE_LOG("Vertex Shader was compiled successfully. Source code : \n\n%s\n\n%s", vertexShaderSourceCode, szBorder);

    // ****************** Fragment Shader ******************
    // step 6 : Write the source code of Fragment Shader
    const GLchar *fragmentShaderSourceCode =
    "#version 460 core\n"                                                                                                                                     \
    "\n"                                                                                                                                                      \
    "in vec3 oTransformedNormals;\n"                                                                                                                          \
    "in vec3 oLightDirection;\n"                                                                                                                              \
    "in vec3 oViewerVector;\n"                                                                                                                                \
    "in vec3 oWorldPosition;\n"                                                     \
    "\n"                                                                                                                                                      \
    "uniform vec3 uLightAmbient;\n"                                                                                                                           \
    "uniform vec3 uLightDiffuse;\n"                                                                                                                           \
    "uniform vec3 uLightSpecular;\n"                                                                                                                          \
    "uniform vec4 uLightPosition;\n"                                                                                                                          \
    "\n"                                                                                                                                                      \
    "uniform vec3 uLightSpotDirection;\n"                                                                                                                     \
    "uniform float uLightSpotInnerCutOffAngle;\n"                                                                                                             \
    "uniform float uLightSpotOuterCutOffAngle;\n"                                                                                                             \
    "\n"                                                                                                                                                      \
    "uniform vec3 uMaterialAmbient;\n"                                                                                                                        \
    "uniform vec3 uMaterialDiffuse;\n"                                                                                                                        \
    "uniform vec3 uMaterialSpecular;\n"                                                                                                                       \
    "uniform float uMaterialShininess;\n"                                                                                                                     \
    "\n"                                                                                                                                                      \
    "uniform int uKeyPress;\n"                                                                                                                                \
    "\n"                                                                                                                                                      \
    "out vec4 FragColor;\n"                                                                                                                                   \
    "\n"                                                                                                                                                      \
    "void main(void)\n"                                                                                                                                       \
    "{\n"                                                                                                                                                     \
    "   vec3 Phong_ADS_Light;\n"                                                                                                                              \
    "   if(uKeyPress == 1)\n"                                                                                                                                 \
    "   {\n"                                                                                                                                                  \
    "       vec3 normalizedTransformedNormals = normalize(oTransformedNormals);\n"                                                                            \
    "       vec3 normalizedLightDirection     = normalize(oLightDirection);\n"                                                                                \
    "       vec3 normalizedViewerVector       = normalize(oViewerVector);\n"                                                                                  \
    "       \n"                                                                                                                                               \
    "       vec3 ambientLight     = uLightAmbient * uMaterialAmbient;\n"                                                                                      \
    "       vec3 diffuseLight     = uLightDiffuse * uMaterialDiffuse * max(dot(normalizedLightDirection, normalizedTransformedNormals), 0.0);\n"              \
    "       vec3 reflectionVector = reflect(-normalizedLightDirection, normalizedTransformedNormals);"                                                        \
    "       vec3 specularLight    = uLightSpecular * uMaterialSpecular * pow(max(dot(reflectionVector, normalizedViewerVector), 0.0), uMaterialShininess);\n" \
    "       \n" \
    "       float distanceToFragment = length(uLightPosition.xyz - oWorldPosition);\n" \
    "       float attenuation        = 1.0 / (1.0 + (0.000007 * distanceToFragment) + (0.000003 * (distanceToFragment * distanceToFragment)));\n" \
    "       \n" \
    "       float spotTheta   = dot(normalizedLightDirection, normalize(-uLightSpotDirection));\n" \
    "       float spotEpsilon = uLightSpotInnerCutOffAngle - uLightSpotOuterCutOffAngle;\n" \
    "       float spotIntensity = clamp((spotTheta - uLightSpotOuterCutOffAngle) / spotEpsilon, 0.0, 1.0);\n" \
    "       \n" \
    "       diffuseLight *= spotIntensity;\n" \
    "       specularLight *= spotIntensity;\n" \
    "       Phong_ADS_Light = ambientLight + diffuseLight + specularLight;\n"                                                                           \
    "   }\n"                                                                                                                                                  \
    "   else\n"                                                                                                                                               \
    "   {\n"                                                                                                                                                  \
    "       Phong_ADS_Light = vec3(1.0, 1.0, 1.0);\n"                                                                                                         \
    "   }\n"                                                                                                                                                  \
    "   FragColor = vec4(Phong_ADS_Light, 1.0);\n"                                                                                                            \
    "}";

    // step 7 : Follow all above steps for Fragment Shader
    // Create the Fragment Shader Object
    GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

    // Give Fragment Shader source code to OpenGL
    glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);

    // Let OpenGL Compile Fragment Shader
    glCompileShader(fragmentShaderObject);
    
    // Check for Fragment Shader compilation errors (if any)
    // re-initialize declared variables for reuse
    status        = 0;
    infoLogLength = 0;
    szInfoLog     = NULL;

    // get compile status
    glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

    // if there are any compilation errors, get length of the compilation error info log
    if(status == GL_FALSE)
    {
        glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        // if log length is valid
        if(infoLogLength > 0)
        {
            // declare a string variable and allocate memory equal to above length
            szInfoLog = (GLchar*)malloc((infoLogLength + 1) * sizeof(GLchar));

            // if memory is allocated successfully
            if(szInfoLog != NULL)
            {
                // get the compilation error info log into this memory allocated string
                glGetShaderInfoLog(fragmentShaderObject, infoLogLength + 1, NULL, szInfoLog);

                // print the compilation error info log
                WRITE_LOG("Fragment Shader Compilation Error Log : %s\n", szInfoLog);

                // for separating extensions and error log
                WRITE_LOG(BORDER);

                // free the allocated memory
                free(szInfoLog);
                szInfoLog = NULL; // pre-caution
            }
        }

        // uninitialize() and exit
        uninitialize(); 
    }
    WRITE_LOG("Fragment Shader was compiled successfully. Source code : \n\n%s\n\n%s", fragmentShaderSourceCode, szBorder);

    // ****************** Shader Program ******************
    // step 8 : create shader program object (from book)
    shaderProgramObject = glCreateProgram();

    // step 9 : attach both shaders to this program
    glAttachShader(shaderProgramObject, vertexShaderObject);
    glAttachShader(shaderProgramObject, fragmentShaderObject);

    // step 10 : bind the attribute locations with the shader program object
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "aNormal");

    // step 11 : link the shader program
    glLinkProgram(shaderProgramObject);

    // step 12 : do above shader compilation related steps for linking error
    // ready the local variables for reuse
    status        = 0;
    infoLogLength = 0;
    szInfoLog     = NULL;

    // get the link status
    glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);

    // if there are any linking errors, get the linking error info log
    if(status == GL_FALSE)
    {
        glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);

        // if error info log length is valid, continue
        if(infoLogLength > 0)
        {
            // declare a string variable and allocate memory equal to above length
            szInfoLog = (GLchar*)malloc((infoLogLength + 1) * sizeof(GLchar));

            // if memory allocation was successful
            if(szInfoLog != NULL)
            {
                // get the linking error info log into this memory allocated string
                glGetProgramInfoLog(shaderProgramObject, infoLogLength + 1, NULL, szInfoLog);

                // print the linking error info log
                WRITE_LOG("Shader Program Linking Error Log : %s\n", szInfoLog);
                
                // for separating extensions and error log
                WRITE_LOG(BORDER);

                // free the allocated memory
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }

        // uninitialize() and exit
        uninitialize(); 
    }
    WRITE_LOG("Shader Program was linked successfully.\n");

    // ****************** Get Shader Uniform locations (MUST get these locations after linkage) ******************
    // matrices
    modelMatrixUniform      = glGetUniformLocation(shaderProgramObject, "uModelMatrix");
    viewMatrixUniform       = glGetUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");

    // light related
    lightAmbientUniform = glGetUniformLocation(shaderProgramObject, "uLightAmbient");
    lightDiffuseUniform = glGetUniformLocation(shaderProgramObject, "uLightDiffuse");
    lightSpecularUniform = glGetUniformLocation(shaderProgramObject, "uLightSpecular");
    lightPositionUniform = glGetUniformLocation(shaderProgramObject, "uLightPosition");
    lightSpotDirectionUniform = glGetUniformLocation(shaderProgramObject, "uLightSpotDirection");
    lightSpotInnerCutOffAngleUniform = glGetUniformLocation(shaderProgramObject, "uLightSpotInnerCutOffAngle");
    lightSpotOuterCutOffAngleUniform = glGetUniformLocation(shaderProgramObject, "uLightSpotOuterCutOffAngle");

    // material related
    materialAmbientUniform = glGetUniformLocation(shaderProgramObject, "uMaterialAmbient");
    materialDiffuseUniform = glGetUniformLocation(shaderProgramObject, "uMaterialDiffuse");
    materialSpecularUniform = glGetUniformLocation(shaderProgramObject, "uMaterialSpecular");
    materialShininessUniform = glGetUniformLocation(shaderProgramObject, "uMaterialShininess");

    // key press uniform
    keyPressUniform = glGetUniformLocation(shaderProgramObject, "uKeyPress");

    // get sphere geometry data from DLL
    float sphere_positions[1146];
    float sphere_normals[1146];
    float sphere_texCoords[764];
    unsigned short sphere_elements[2280];

    getSphereVertexData(sphere_positions, sphere_normals, sphere_texCoords, sphere_elements);
    numSphereElements = getNumberOfSphereElements();

    // ****************** SPHERE ******************
    // VAO (Vertex Array Object)
    // Create Vertex Array Object
    glGenVertexArrays(1, &vao);

    // Bind with VAO
    glBindVertexArray(vao);

    // ****************** VBO (Vertex Buffer Object) for position ******************
    // create Vertex Buffer Object for position
    glGenBuffers(1, &vbo_position);

    // bind with VBO of position
    glBindBuffer(GL_ARRAY_BUFFER, vbo_position);

    // organize the vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_positions), sphere_positions, GL_STATIC_DRAW /* usage */);
    
    // tell the shader how to interpret this vertex attribute's data
    glVertexAttribPointer(
                          AMC_ATTRIBUTE_POSITION,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          NULL
                         );

    glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

    // unbind with VBO of position
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ****************** VBO (Vertex Buffer Object) for normal ******************
    // create VBO for normal
    glGenBuffers(1, &vbo_normal);

    // bind with VBO of normal
    glBindBuffer(GL_ARRAY_BUFFER, vbo_normal);

    // organize the vertex data
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

    // tell the shader how to interpret this vertex attribute's data
    glVertexAttribPointer(
                          AMC_ATTRIBUTE_NORMAL,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          NULL
                         );

    glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

    // unbind with VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ****************** VBO (Vertex Buffer Object) for texCoords ******************
    glGenBuffers(1, &vbo_texcoord);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_texcoord);
    {
        glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_texCoords), sphere_texCoords, GL_STATIC_DRAW);
        glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
        glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // ****************** VBO (Vertex Buffer Object) for elements ******************
    glGenBuffers(1, &vbo_elements);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);
    {
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // unbind with VAO
    glBindVertexArray(0);

    // here, OpenGL starts...!!! 
    // enabling depth (sequence matters)
    glClearDepth(1.0f); // clear the depth buffer using 1s (set all bits to 1)
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // SET the clear color of window to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // glClearColor() sets the color buffer bit internally

    // initialize perspective projection matrix
    perspectiveProjectionMatrix = vmath::mat4::identity();

    // warm-up resize call
    resize(WIN_WIDTH, WIN_HEIGHT);

    return(0);
}

void printGLInfo(void)
{
    // variable declarations
    GLint numExtensions = 0;
    GLint i;

    // code
    WRITE_LOG("OpenGL Vendor   : %s\n", glGetString(GL_VENDOR));                   // OpenGL Implementation (Driver) is given by whom? On my PC, it should be NVIDIA
    WRITE_LOG("OpenGL Renderer : %s\n", glGetString(GL_RENDERER));                 // OpenGL Driver (NVIDIA) Version Number
    WRITE_LOG("OpenGL Version  : %s\n", glGetString(GL_VERSION));                  // OpenGL Version
    WRITE_LOG("GLSL Version    : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION)); // Graphic Library Shading Language (GLSL) Version Number
    
    // beautification
    WRITE_LOG(BORDER);

    // Listing of supported extensions
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions); // number of extensions supported by your graphic card OpenGL driver

    for(i = 0; i < numExtensions; i++)
    {
        WRITE_LOG("%s\n", glGetStringi(GL_EXTENSIONS, i)); // glGetStringi() gives the corresponding string to an integer value 
    }

    // beautification
    WRITE_LOG(BORDER);
}

void resize(int width, int height)
{
    // code
    if(height <= 0)
        height = 1;

    winWidth  = width;
    winHeight = height;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);

    // Set Perspective Projection matrix
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
    // code
    // CLEAR the window with the current clear color (which was set as black using glClearColor())
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* color buffer, depth buffer */

    // 1. use Shader Program Object
    glUseProgram(shaderProgramObject);

    // ***************** SPHERE *****************
    // Transformation    
    // translation
    vmath::mat4 translationMatrix = vmath::mat4::identity();
    translationMatrix             = vmath::translate(0.0f, 0.0f, -2.0f);

    vmath::mat4 modelMatrix = translationMatrix;
    vmath::mat4 viewMatrix  = vmath::mat4::identity();

    // push the matrices into Vertex Shader's uniforms
    glUniformMatrix4fv(
                        modelMatrixUniform,       /* uniform location */ 
                        1,                        /* count */ 
                        GL_FALSE,                 /* transpose this matrix? (Rows to Columns / Columns to Rows) */
                        modelMatrix               /* value */
                       );

    glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);

    glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

    // if lighting is enabled
    if(bLightingEnabled == TRUE)
    {
        // send 1 to the shader's keyPress uniform
        glUniform1i(keyPressUniform, 1);

        // send light properties and material properties to the shader's uniforms
        glUniform3fv(lightAmbientUniform, 1, lightAmbient);
        glUniform3fv(lightDiffuseUniform, 1, lightDiffuse);
        glUniform3fv(lightSpecularUniform, 1, lightSpecular);
        glUniform4fv(lightPositionUniform, 1, lightPosition);

        glUniform3fv(lightSpotDirectionUniform, 1, lightSpotDirection);
        glUniform1f(lightSpotInnerCutOffAngleUniform, cosf(vmath::radians(lightSpotInnerCutOffAngle)));
        glUniform1f(lightSpotOuterCutOffAngleUniform, cosf(vmath::radians(lightSpotOuterCutOffAngle)));

        glUniform3fv(materialAmbientUniform, 1, materialAmbient);
        glUniform3fv(materialDiffuseUniform, 1, materialDiffuse);
        glUniform3fv(materialSpecularUniform, 1, materialSpecular);
        glUniform1f(materialShininessUniform, materialShininess);
    }
    else // lighting is not enabled
    {
        // send 0 to the shader's keyPress uniform
        glUniform1i(keyPressUniform, 0);
    }

    glBindVertexArray(vao); // bind with VAO
    {
        // bind with element buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_elements);        

        // Draw the sphere - don't draw with arrays, but draw with elements using glDrawElements()
        glDrawElements(GL_TRIANGLES, numSphereElements, GL_UNSIGNED_SHORT /**** Very important parameter ****/, 0);
    }
    glBindVertexArray(0); // un-bind with VAO

    // un-use Shader Program Object
    glUseProgram(0);

    SwapBuffers(ghdc);
}

void update(void)
{
    float lerp(float x, float x0, float x1, float y0, float y1);

    // code
    if(gbLeftMouseButtonDown)
    {
        POINT cursorPos;
        GetCursorPos(&cursorPos);

        lightPosition[0] = lerp((float)cursorPos.x, 0.0f, (float)winWidth, -1.0f, 1.0f);    
        lightPosition[1] = lerp((float)cursorPos.y, (float)winHeight, 0.0f, -0.5f, 0.5f);    
    }
}

void uninitialize(void)
{
    // function declarations
    void ToggleFullscreen(void);

    // code
    // ************ delete Sphere's VBOs and VAO ************
    // delete VBO of elements
    if(vbo_elements)
    {
        glDeleteBuffers(1, &vbo_elements);
        vbo_elements = 0;
    }

    // delete VBO of texCoords
    if(vbo_texcoord)
    {
        glDeleteBuffers(1, &vbo_texcoord);
        vbo_texcoord = 0;
    }

    // delete VBO of normals
    if(vbo_normal)
    {
        glDeleteBuffers(1, &vbo_normal);
        vbo_normal = 0;
    }    

    // delete VBO of position
    if(vbo_position)
    {
        glDeleteBuffers(1, &vbo_position);
        vbo_position = 0;
    }

    // delete VAO
    if(vao)
    {
        glDeleteBuffers(1, &vao);
        vao = 0;
    }

    // uninitialize the Shader Objects and Shader Program object
    if(shaderProgramObject)
    {
        // 1. use Shader Program object
        glUseProgram(shaderProgramObject);

        // 2. get number of attached shaders
        GLint numShaders = 0;
        glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);

        // 3. create and allocate array of Shader Objects according to the number of attached shaders
        if(numShaders > 0)
        {
            GLuint *pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
        
            // if malloc() was successful
            if(pShaders != NULL)
            {
                // 4. get Shader Objects into this allocated array
                glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);

                // 5. Detach and delete Shaders iteratively (in loop)
                for(GLint i = 0; i < numShaders; i++)
                {
                    glDetachShader(shaderProgramObject, pShaders[i]);
                    glDeleteShader(pShaders[i]);
                    pShaders[i] = NULL;
                }

                // 6. Free the memory allocated for array of Shader Objects
                free(pShaders);
                pShaders = NULL;
            }
        }

        // 7. unuse Shader Program Object
        glUseProgram(0);

        // 8. Delete Shader Program
        glDeleteProgram(shaderProgramObject);
        shaderProgramObject = 0;
    }

    // if application is exitting in fullscreen
    if(gbFullscreen == TRUE)
    {
        ToggleFullscreen();
        gbFullscreen = FALSE;
    }

    // Make the hdc as current DC
    if(wglGetCurrentContext() == ghrc)
    {
        wglMakeCurrent(NULL, NULL);
    }

    // Delete rendering context
    if(ghrc)
    {
        wglDeleteContext(ghrc);
        ghrc = NULL;
    }

    // Release the hdc
    if(ghdc)
    {
        ReleaseDC(ghwnd, ghdc);
        ghdc = NULL;
    }

    // Destroy window
    if(ghwnd)
    {
        DestroyWindow(ghwnd);
        ghwnd = NULL;
    }

    // close the log file
    if(gpFILE)
    {
        WRITE_LOG("Program ended successfully.\n");
        fclose(gpFILE);
        gpFILE = NULL;
    }
}

float lerp(float x, float x0, float x1, float y0, float y1)
{
    // code
    return((x - x0) / (x1 - x0) * (y1 - y0) + y0);
}

#pragma endregion
