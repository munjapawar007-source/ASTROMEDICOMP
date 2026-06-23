// Win32 Headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define _USE_MATH_DEFINES 1

// OGL Related File
#include <GL/glew.h> //this headr file must be include before gl/gl.h
#include <gl/GL.h>	 //inside this prototypes presents
#include "OGL.h"
#include "vmath.h"
#include "Sphere.h"
using namespace vmath;

// OpenGL Related Libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib") // for Linker dyanamic libarary presents here

// Sphere Related Libraries
#pragma comment(lib, "Sphere.lib")

// MICROS
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global Function Declaration/prototype/ signature
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable declaration
// Variables Related FullScreen
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
DWORD dwStyle; // to hold value
WINDOWPLACEMENT wpPrev;

// Variable Related File I/O
char gszLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

// Active Window Related Variable
BOOL gbActiveWindow = FALSE;

// Exit keyPress Related
BOOL gbEscapeKeyPressed = FALSE;

// OpenGL Related Variables
HDC ghdc = NULL;
HGLRC ghrc = NULL; //  handal to Graphics rendering

// Shader related global veriables
GLuint shaderProgramObject = 0;
enum
{
	AMC_ATTRIBUTE_VERTEX = 0,
	AMC_ATTRIBUTE_NORMAL,
};

GLuint Vao = 0;
GLuint vbo_position = 0;
GLuint Vbo_normal = 0;
GLuint Vbo_element = 0;

mat4 perspectiveProjectionMatrix;

GLuint modelMatrixUniform = 0;
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;

// ====== Screen Information ======
GLuint screenResolutionUniform = 0;
float screenWidth = WIN_WIDTH;
float screenHeight = WIN_HEIGHT;

GLuint inverseProjectionUniform = 0;
GLuint inverseViewUniform = 0;
GLuint cameraPositionUniform = 0;
GLuint sunDirectionUniform = 0;

// Entry Point Function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int iCmdShow)
{
	// function
	int initialize(void);
	void display(void);
	void update(void);
	void uninitialize(void);

	// Variable Declarations
	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("RTR6");
	BOOL bDone = FALSE;

	// code
	// Creat Log File
	gpFile = fopen(gszLogFileName, "w");
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("Log File Creation Failed"), TEXT("Error"), MB_OK);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Program Started Successfully.\n");
	}

	// Window Class Initialization
	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // owndc self device contex
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(DKGRAY_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Registration Of Window Class
	RegisterClassEx(&wndclass);

	// CREATE WINDOW CENTER
	int m_x = (GetSystemMetrics(SM_CXSCREEN) - WIN_WIDTH) / 2; // main screen Dimension in GetSystemMetrics for x And y
	int m_y = (GetSystemMetrics(SM_CYSCREEN) - WIN_HEIGHT) / 2;
	// Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("MP"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, m_x, m_y, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	ghwnd = hwnd; // value assinged

	// show window
	ShowWindow(hwnd, iCmdShow); // show window if you not give

	// Paint Backgraound window
	UpdateWindow(hwnd);

	// initialize
	if (initialize() != 0)
	{
		fprintf(gpFile, "initialize failed\n");
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}
	else
	{
		fprintf(gpFile, "initialize() Function Completed Successfully...\n");
	}

	// SET this Window as foreGround And Active Window
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	// Game Loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) // PM_REMOVE
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

			if (gbActiveWindow == TRUE) // to stop (game animation)window when you u open unother window
			{
				if (gbEscapeKeyPressed == TRUE)
				{
					bDone = TRUE;
				}
				// render
				display();

				// update
				update();
			}
		}
	}

	// uninitialize
	uninitialize();

	return ((int)msg.wParam);
}

// CAll Back Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wparam, LPARAM lparam)
{
	// Function Declaration
	void toggleFullScreen(void);
	void resize(int, int);
	void uninitialize(void);

	// code
	switch (iMsg)
	{
	case WM_CREATE:
		ZeroMemory((void *)&wpPrev, sizeof(WINDOWPLACEMENT)); // memset
		wpPrev.length = sizeof(WINDOWPLACEMENT);
		break;
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND: //  for smooth Rendaring
		return (0);
	case WM_SIZE:
		resize(LOWORD(lparam), HIWORD(lparam));
		break;
	case WM_KEYDOWN:
		switch (wparam)
		{
		case VK_ESCAPE:
			gbEscapeKeyPressed = TRUE;
			DestroyWindow(ghwnd);
			break;
		default:
			break;
		}
		break;
	case WM_CHAR:
		switch (wparam)
		{
		case 'F':
		case 'f':
			if (gbFullScreen == FALSE)
			{
				toggleFullScreen();
				gbFullScreen = TRUE;
			}
			else
			{
				toggleFullScreen();
				gbFullScreen = FALSE;
			}
			break;
		default:
			break;
		}
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
	return (DefWindowProc(hwnd, iMsg, wparam, lparam));
}

void toggleFullScreen(void) // stub function manje jya madhe code nahi
{
	// Variable declarations
	MONITORINFO mi;

	// code
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
		ShowCursor(FALSE);
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
	// Function Declaration
	void printGLInfo(void);
	void resize(int, int);
	void uninitialize(void);

	// Variable Declarations
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	GLenum glewResult;

	// code
	//  Pixel Format Descreptor initialize
	ZeroMemory((void *)&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER; // certin part of memory which is filled
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	// GetDc
	ghdc = GetDC(ghwnd);

	if (ghdc == NULL)
	{
		fprintf(gpFile, "Get Dc Function Fail\n");
		return (-1);
	}

	// Get Matching Pixel Format Index Using Hdc And Pfd
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat func Failed \n");
		return (-2);
	}

	// Select This pixel format of found index
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "ChoosePixelFormat func Failed \n");
		return (-3);
	}

	// briging api (wsi = window system inegrations)
	// Create Randaring Consept using ghdc,pfd,choosePixelFormat
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext func Failed\n");
		return (-4);
	}

	// make this rendering contex as current contex
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent func Failed\n");
		return (-5);
	}

	// initialize GLEW
	glewResult = glewInit();
	if (glewResult != GLEW_OK)
	{
		fprintf(gpFile, " glewInit func Failed\n");
		return (-6);
	}

	// print gl info
	// printGLInfo();

	// VERTEX SHADERS
	//  write the shader sorce code 2)create the shader object 3) give the shader soure code to shader object 4) copile the code 5) do error compiletion shader code
	const GLchar *vertexShaderSourceCode =
		"#version 460 core\n"
		"in vec2 aPosition;\n"
		"out vec2 fragmentUV;"
		"void main(void)\n"
		"{\n"
		" fragmentUV = aPosition * 0.5 + 0.5;"
		"gl_Position = vec4(aPosition, 0.0, 1.0);\n"
		"}\n";

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode, NULL);
	// in 2nd para how many shader u use and 4th is the lenth of 2nd shader arrey

	glCompileShader(vertexShaderObject);

	GLint status = 0;
	GLint infoLogLenght = 0;
	GLchar *szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLenght);
		if (infoLogLenght > 0)
		{
			szInfoLog = (GLchar *)malloc(infoLogLenght * sizeof(GLchar));

			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLenght, NULL, szInfoLog);
				fprintf(gpFile, "vertex Shader Compilation log = %s", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	// FRAGMENT SHADER
	const GLchar *fargamentShaderSourceCode =
		// "#version 460 core\n"
		// "const int sampleCount = 16;\n"
		// "const float PI = 3.14159265359;\n" \

		// "const float planetRadius = 6360000.0;\n" \
		// "const float atmosphereRadius = 6460000.0;\n" \

		// "const float rayleighScaleHeight = 8000.0;\n" \
		// "const float mieScaleHeight  = 1200.0;\n" \

		// "const vec3 betaRayleigh = vec3(5.8e-6f, 13.5e-6f, 33.1e-6f);\n" \
		// "const vec3 betamei = vec3(21e-6f);\n" \

		// "uniform vec3 uCameraPosition;\n" \
		// "uniform vec3 uSunDirection;\n" \
		// "uniform mat4 uInverseProjectionMatrix;\n" \
		// "uniform mat4 uInverseViewMatrix;\n" \

		// "in vec2 fragmentUV;\n" \
		// "out vec4 fragColor;\n" \
		// "vec3 generateWorldRay()\n"
		// "{\n"
		// "    vec2 ndc = fragmentUV * 2.0 - 1.0;\n"
		// "    vec3 rayDir = normalize(vec3(ndc.x, ndc.y, -1.5));\n"
		// "    return rayDir;\n"
		// "}\n"
		// "bool raySphereIntersect(vec3 ro, vec3 rd, float radius, out float t0, out float t1)\n" \
		// "{\n" \
		// "  float a = dot(rd, rd);\n" \
		// "  float b = 2.0 * dot(ro, rd);\n" \
		// "  float c = dot(ro, ro) - radius * radius;\n" \
		// "  float d = b * b - 4.0 * a * c;\n" \
		// "  if(d < 0.0)\n" \
		// "     return false;\n" \
		// "  float sqrtD = sqrt(d);\n" \
		// "  t0 = (-b - sqrtD) / (2.0 * a);\n" \
		// "  t1 = (-b + sqrtD) / (2.0 * a);\n" \
		// "  return true;\n" \
		// "}\n" \
		// "float rayleighPhase(float cosTheta)\n"
		// "{\n" \
		// 	"return(3.0 / (16.0 * PI)) * (1.0 +cosTheta * cosTheta);\n"
		// "}\n" \
		// "void main()\n" \
		// "{\n" \
		// "	vec3 rayDir = generateWorldRay();\n" \
		// "	float t0 = 0.0f, t1 = 0.0f;\n" \
		// "	bool hit = raySphereIntersect(uCameraPosition, rayDir, atmosphereRadius, t0, t1);\n" \
		// "	float segmentLength = (t1 - t0) / float(sampleCount);\n" \
		// "	float currentT = t0;\n"
		// "	float totalRayleigh = 0.0f;\n"
		// "	for(int i = 0; i < sampleCount; i++)\n"
		// "	{\n" \
		// "   	vec3 samplePosition = uCameraPosition + rayDir * currentT;\n	// ray ke along ek point\n"
		// "   	float height = length(samplePosition) - planetRadius;\n		// planet surface pasunchi height\n"
		// "   	float density = exp(-height / rayleighScaleHeight);\n"
		// "   	totalRayleigh = totalRayleigh + density * segmentLength;	// integration\n"
		// "   	currentT = currentT + segmentLength;"
		// "	}\n" \
		// " 	float mu = dot(rayDir, uSunDirection);\n"
		// "	vec3 rayleighColor = betaRayleigh * totalRayleigh * rayleighPhase(mu);\n"
		// "	fragColor = vec4(rayleighColor * 200000.0 , 1.0);\n" \
		// "}\n";


R"(#version 460 core

const int sampleCount = 16;
const int lightSampleCount = 8; // Samples for the Sun-to-Point path
const float PI = 3.14159265359;

const float planetRadius = 6360000.0;
const float atmosphereRadius = 6420000.0; // Slightly thinner for better looks
const float rayleighScaleHeight = 8000.0;

const vec3 betaRayleigh = vec3(5.5e-6, 13.0e-6, 22.4e-6); // Standard Earth values

uniform vec3 uCameraPosition;
uniform vec3 uSunDirection;
uniform mat4 uInverseProjectionMatrix;
uniform mat4 uInverseViewMatrix;

in vec2 fragmentUV;
out vec4 fragColor;

// Helper: Calculate optical depth (density over distance)
float getDensityAtHeight(float h) {
    return exp(-h / rayleighScaleHeight);
}

bool raySphereIntersect(vec3 ro, vec3 rd, float radius, out float t0, out float t1) {
    float b = dot(ro, rd);
    float c = dot(ro, ro) - radius * radius;
    float d = b * b - c;
    if (d < 0.0) return false;
    float sqrtD = sqrt(d);
    t0 = -b - sqrtD;
    t1 = -b + sqrtD;
    return true;
}

void main() {
    // 1. Generate World-Space Ray
    vec4 clipPos = vec4(fragmentUV * 2.0 - 1.0, -1.0, 1.0);
    vec4 viewPos = uInverseProjectionMatrix * clipPos;
    viewPos /= viewPos.w;
    vec3 rayDir = normalize((uInverseViewMatrix * vec4(normalize(viewPos.xyz), 0.0)).xyz);

    // 2. Find Atmosphere boundaries
    float t0, t1;
    if (!raySphereIntersect(uCameraPosition, rayDir, atmosphereRadius, t0, t1) || t1 < 0.0) {
        fragColor = vec4(0.0, 0.0, 0.1, 1.0); // Deep space
        return;
    }
    t0 = max(t0, 0.0);

    // 3. Clip ray if it hits the ground
    float tG0, tG1;
    if (raySphereIntersect(uCameraPosition, rayDir, planetRadius, tG0, tG1)) {
        if (tG0 > 0.0) t1 = min(t1, tG0);
    }

    // 4. Integration Loop
    float segmentLength = (t1 - t0) / float(sampleCount);
    float currentT = t0;
    vec3 totalScattering = vec3(0.0);
    float opticalDepthView = 0.0;

    for (int i = 0; i < sampleCount; i++) {
        vec3 samplePos = uCameraPosition + rayDir * (currentT + segmentLength * 0.5);
        float h = length(samplePos) - planetRadius;
        float hr = getDensityAtHeight(h) * segmentLength;
        opticalDepthView += hr;

        // --- SECONDARY LOOP: Light from Sun to Sample Point ---
        float ts0, ts1;
        raySphereIntersect(samplePos, uSunDirection, atmosphereRadius, ts0, ts1);
        float segmentLengthLight = ts1 / float(lightSampleCount);
        float opticalDepthLight = 0.0;
        
        for (int j = 0; j < lightSampleCount; j++) {
            vec3 lightSamplePos = samplePos + uSunDirection * (float(j) + 0.5) * segmentLengthLight;
            float hLight = length(lightSamplePos) - planetRadius;
            opticalDepthLight += getDensityAtHeight(hLight) * segmentLengthLight;
        }

        // Combined attenuation (Beer-Lambert Law)
        vec3 transmittance = exp(-(betaRayleigh * (opticalDepthView + opticalDepthLight)));
        totalScattering += hr * transmittance;
        currentT += segmentLength;
    }

    // 5. Final Color with Rayleigh Phase
    float cosTheta = dot(rayDir, uSunDirection);
    float phase = (3.0 / (16.0 * PI)) * (1.0 + cosTheta * cosTheta);
    vec3 finalColor = totalScattering * betaRayleigh * phase * 20.0; // Intensity multiplier

    // 6. Tonemapping (Crucial! Converts HDR to LDR)
    finalColor = 1.0 - exp(-1.0 * finalColor);

    fragColor = vec4(finalColor, 1.0);
})";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fargamentShaderSourceCode, NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLenght = 0;
	szInfoLog = NULL;
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLenght);
		if (infoLogLenght > 0)
		{
			szInfoLog = (GLchar *)malloc(infoLogLenght * sizeof(GLchar));

			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLenght, NULL, szInfoLog);
				fprintf(gpFile, "FRAGMENT Shader Compilation log = %s", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	// Create , Attach , link shader program object
	shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	// Bind Shader Attribute at certin index in shader to same index in host program
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_VERTEX, "aPosition"); // 0th index match cpu and gpu side
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glLinkProgram(shaderProgramObject);
	status = 0;
	infoLogLenght = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLenght);

		if (infoLogLenght = 0)
		{
			szInfoLog = (GLchar *)malloc(infoLogLenght * sizeof(GLchar));

			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLenght, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log", szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	// get uniform
	inverseProjectionUniform = glGetUniformLocation(shaderProgramObject, "uInverseProjectionMatrix");
	inverseViewUniform = glGetUniformLocation(shaderProgramObject, "uInverseViewMatrix");
	cameraPositionUniform = glGetUniformLocation(shaderProgramObject, "uCameraPosition");
	sunDirectionUniform = glGetUniformLocation(shaderProgramObject, "uSunDirection");

	const GLfloat quadVertices[] =
		{
			-1.0f, -1.0f,
			1.0f, -1.0f,
			1.0f, 1.0f,
			-1.0f, -1.0f,
			1.0f, 1.0f,
			-1.0f, 1.0f};

	// Vertex array object for vertex attribut
	glGenVertexArrays(1, &Vao);
	glBindVertexArray(Vao);

	// Position vbo
	glGenBuffers(1, &vbo_position); // all arrays associate with vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_VERTEX, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_VERTEX);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	// glGenBuffers(1, &Vbo_normal);
	// glBindBuffer(GL_ARRAY_BUFFER, Vbo_normal);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	// glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	// glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// depth related code
	glClearDepth(1.0f); // display madhe color buffer madhe gleyavr majya depth buffer madhe ja ani change all bits 1.0f
	// glEnable(GL_DEPTH_TEST); // depth test on
	glDepthFunc(GL_LEQUAL); // tyt tyt fragment la pass kr jyanchi value 1 peksh kami asel kiva equal asel

	//  tell the openGL choose the clear color
	glClearColor(0.20f, 0.20f, 0.20f, 1.0f);

	perspectiveProjectionMatrix = mat4::identity(); // this is annalogas to load identity (resize madhil) in ffp

	// warm up resize
	resize(WIN_WIDTH, WIN_HEIGHT);
	return (0);
}

void sphere(void)
{

	// // provide vertex position,color,texcode, and normal etc.
	// getSphereVertexData(sphere_vertices, sphere_normals, sphere_texture, sphere_elements);
	// gNumVertices = getNumberOfSphereVertices();
	// gNumElements = getNumberOfSphereElements();

	// // Vertex array object for vertex attribut
	// glGenVertexArrays(1, &gVao_sphere);
	// glBindVertexArray(gVao_sphere);

	// // Position vbo
	// glGenBuffers(1, &vbo_sphere_position); // all arrays associate with vao
	// glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	// glVertexAttribPointer(AMC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	// glEnableVertexAttribArray(AMC_ATTRIBUTE_VERTEX);
	// glBindBuffer(GL_ARRAY_BUFFER, 0);

	// // normal vbo
	// glGenBuffers(1, &gVbo_sphere_normal);
	// glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);

	// glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	// glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);

	// glBindBuffer(GL_ARRAY_BUFFER, 0);

	// // element vbo
	// glGenBuffers(1, &gVbo_sphere_element);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	// glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	// glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// // unbind vao
	// glBindVertexArray(0);
}

void printGLInfo(void)
{
	// veriable declarations
	GLint numExtentions, i;

	// code
	// print opengGL info
	fprintf(gpFile, "OPENGL Information\n");
	fprintf(gpFile, "********************\n");
	fprintf(gpFile, "openGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "openGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "openGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "glsl Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	// get number of extentions
	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtentions);

	// PRINT openGl Extentions
	for (i = 0; i < numExtentions; i++)
	{
		fprintf(gpFile, "%s\n", glGetStringi(GL_EXTENSIONS, i));
	}
	fprintf(gpFile, "********************\n");
}

void resize(int width, int height)
{
	// code
	// if height by default 0 makes1
	if (height <= 0)
	{
		height = 1;
	}
	// set the viewPort
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// use Shader program object
	glUseProgram(shaderProgramObject);

	// transformstions
	vmath::mat4 viewMatrix = vmath::mat4::identity();
	vmath::mat4 inverseViewMatrix = viewMatrix;
	// vmath::mat4 inverseProjectionMatrix = vmath::mat4::identity();

	// 1. Get the same values you used in resize()
	float fov = 45.0f;
	float aspect = (float)screenWidth / (float)screenHeight;
	float near_AS = 0.1f;
	float far_AS = 100.0f;

	// 2. Calculate the components
	float tangent = tan(fov * 0.5f * M_PI / 180.0f);
	float a = 1.0f / (aspect * tangent);
	float b = 1.0f / tangent;
	float c = (far_AS + near_AS) / (near_AS - far_AS);
	float d = (2.0f * far_AS * near_AS) / (near_AS - far_AS);

	// 3. This is the Inverse Projection Matrix manually constructed
	vmath::mat4 inverseProjectionMatrix = vmath::mat4(
		vmath::vec4(1.0f / a, 0.0f, 0.0f, 0.0f),
		vmath::vec4(0.0f, 1.0f / b, 0.0f, 0.0f),
		vmath::vec4(0.0f, 0.0f, 0.0f, -1.0f),
		vmath::vec4(0.0f, 0.0f, 1.0f / d, c / d));

	glUniformMatrix4fv(inverseViewUniform, 1, GL_FALSE, inverseViewMatrix);
	glUniformMatrix4fv(inverseProjectionUniform, 1, GL_FALSE, inverseProjectionMatrix);

	// Camera setup
	vmath::vec3 cameraPosition = vmath::vec3(0.0f, 0.0f, 6360000.0f + 1000.0f);
	vmath::vec3 sunDirection = vmath::normalize(vmath::vec3(0.0f, 0.10f, -1.0f));

	// camera
	glUniform3fv(cameraPositionUniform, 1, cameraPosition);
	// sun
	glUniform3fv(sunDirectionUniform, 1, sunDirection);

	// bind with vao
	glBindVertexArray(Vao);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// *** unbind vao ***
	glBindVertexArray(0);

	// unUse shader program
	glUseProgram(0);

	// swap the buffers
	SwapBuffers(ghdc);
}

void update(void)
{
	// code
}

void uninitialize(void)
{
	// Function Declarations
	void toggleFullScreen(void);

	// code
	// if user is existing FullScreen then restore back to normal
	if (gbFullScreen == TRUE)
	{
		toggleFullScreen();
		gbFullScreen = FALSE;
	}

	// free vbo postion
	if (vbo_position)
	{
		glDeleteBuffers(1, &vbo_position);
		vbo_position = 0;
	}

	// free vao
	if (Vao)
	{
		glDeleteVertexArrays(1, &Vao);
		Vao = 0;
	}

	// detach delete , shader program object
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLint numShaders;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
				}
			}
			free(pShaders);
			pShaders = 0;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
	}

	// make hdc current contex by relesing randering current contex
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	// delete the randering contex
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	// Relase the dc
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	// Distroy Window
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}
	// close the File
	if (gpFile)
	{
		fprintf(gpFile, "Program Terminated Successfully...");
		fclose(gpFile);
		gpFile = NULL;
	}
}
