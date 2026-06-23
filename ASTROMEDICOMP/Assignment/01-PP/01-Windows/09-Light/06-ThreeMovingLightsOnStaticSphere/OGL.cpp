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
GLuint shaderProgramObject_PV = 0;
GLuint shaderProgramObject_PF = 0;
enum
{
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_NORMAL,
};

GLuint gVao_sphere = 0;
GLuint vbo_sphere_position = 0;
GLuint gVbo_sphere_normal = 0;
GLuint gVbo_sphere_element = 0;

// FOR PER VERTEX VERIABLE
mat4 perspectiveProjectionMatrix_PV;
GLuint modelMatrixUniform_PV = 0;
GLuint viewMatrixUniform_PV = 0;
GLuint projectionMatrixUniform_PV = 0;
GLuint laUniform_PV[3];
GLuint ldUniform_PV[3];
GLuint lsUniform_PV[3];
GLuint kaUniform_PV = 0;
GLuint kdUniform_PV = 0;
GLuint ksUniform_PV = 0;
GLuint materialShininaseUniform_PV = 0;
GLuint lightPositionUniform_PV[3];
GLuint LKeyPressedUniform_PV = 0;

// FOR PER FRAGMENT VERIABLE
mat4 perspectiveProjectionMatrix_PF;
GLuint modelMatrixUniform_PF = 0;
GLuint viewMatrixUniform_PF = 0;
GLuint projectionMatrixUniform_PF = 0;
GLuint laUniform_PF[3];
GLuint ldUniform_PF[3];
GLuint lsUniform_PF[3];
GLuint kaUniform_PF = 0;
GLuint kdUniform_PF = 0;
GLuint ksUniform_PF = 0;
GLuint materialShininaseUniform_PF = 0;
GLuint lightPositionUniform_PF[3];
GLuint LKeyPressedUniform_PF = 0;

struct Light
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position[3];
};

struct Light light[3];

GLfloat materialAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialSpecular[] = {0.5f, 0.5f, 0.5f, 1.0f};
GLfloat materialShininnes = 128.0f;
GLfloat materialDiffuse[] = {0.9f, 0.90f, 0.90f, 1.0f};

BOOL bAnimation = FALSE;
BOOL bLight = FALSE;

float anglePos1 = 0.0f;
float anglePos2 = 0.0f;
float anglePos3 = 0.0f;

// Roatation
GLfloat angleSphere_MP = 0.0f;

BOOL perVertex = TRUE;	 // display vertex light
BOOL perFragment = TRUE; // display fargment light

// Sphere Related Veriables
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_texture[764];
unsigned short sphere_elements[2280];

GLsizei gNumElements;
GLsizei gNumVertices;

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
	case WM_CHAR:
		switch (wparam)
		{
		case 'A':
		case 'a':
			if (bAnimation == FALSE)
			{
				bAnimation = TRUE;
			}
			else
			{
				bAnimation = FALSE;
			}
			break;
		case 'F':
		case 'f':
			if (perFragment == TRUE)
			{
				perFragment = FALSE;
				perVertex = TRUE;
			}
			else
			{
				perFragment = TRUE;
				perVertex = FALSE;
			}
			break;
		case 'L':
		case 'l':
			if (bLight == FALSE)
			{
				bLight = TRUE;
			}
			else
			{
				bLight = FALSE;
			}
			break;

		case 'Q':
		case 'q':
			gbEscapeKeyPressed = TRUE;
			DestroyWindow(ghwnd);
			break;
		case 'V':
		case 'v':
			if (perVertex == TRUE)
			{
				perFragment = TRUE;
				perVertex = FALSE;
			}
			else
			{
				perFragment = FALSE;
				perVertex = TRUE;
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
	void initializePerVertexShader(void);
	void initializePerFragmentShader(void);

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
	printGLInfo();

	initializePerVertexShader();   // FOR PER VERTEX SHADER
	initializePerFragmentShader(); // FOR PER FRAGMENT SHADER

	// provide vertex position,color,texcode, and normal etc.
	getSphereVertexData(sphere_vertices, sphere_normals, sphere_texture, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();

	// Vertex array object for vertex attribut
	glGenVertexArrays(1, &gVao_sphere);
	glBindVertexArray(gVao_sphere);

	// Position vbo
	glGenBuffers(1, &vbo_sphere_position); // all arrays associate with vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_sphere_position);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_vertices), sphere_vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// normal vbo
	glGenBuffers(1, &gVbo_sphere_normal);
	glBindBuffer(GL_ARRAY_BUFFER, gVbo_sphere_normal);
	glBufferData(GL_ARRAY_BUFFER, sizeof(sphere_normals), sphere_normals, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// element vbo
	glGenBuffers(1, &gVbo_sphere_element);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphere_elements), sphere_elements, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// unbind vao
	glBindVertexArray(0);

	// depth related code
	glClearDepth(1.0f);		 // display madhe color buffer madhe gleyavr majya depth buffer madhe ja ani change all bits 1.0f
	glEnable(GL_DEPTH_TEST); // depth test on
	glDepthFunc(GL_LEQUAL);	 // tyt tyt fragment la pass kr jyanchi value 1 peksh kami asel kiva equal asel

	// from here on word OPENGL code starts
	//  tell the openGL choose the clear color
	glClearColor(0.250f, 0.250f, 0.250f, 1.0f);

	// initialize of three light
	light[0].ambient = vec4(0.0, 0.0, 0.0, 1.0);
	light[0].diffuse = vec4(1.0, 0.0, 0.0, 1.0);
	light[0].specular = vec4(1.0, 0.0, 0.0, 1.0);
	light[0].position[0] = vec4(0.0, 0.0, 0.1, 1.0);

	light[1].ambient = vec4(0.0f, 0.0, 0.0, 1.0);
	light[1].diffuse = vec4(0.0f, 0.0, 1.0, 1.0);
	light[1].specular = vec4(0.0f, 0.0, 1.0, 1.0);

	light[2].ambient = vec4(0.0f, 0.0, 0.0, 1.0); 
	light[2].diffuse = vec4(0.0f, 1.0, 0.0, 1.0);
	light[2].specular = vec4(0.0f, 1.0, 0.0, 1.0);
	
	perspectiveProjectionMatrix_PV = mat4::identity(); // this is annalogas to load identity (resize madhil) in ffp
	perspectiveProjectionMatrix_PF = mat4::identity();

	// warm up resize
	resize(WIN_WIDTH, WIN_HEIGHT);
	return (0);
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
	perspectiveProjectionMatrix_PV = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
	perspectiveProjectionMatrix_PF = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//  use Shader program object
	if (perVertex == FALSE && perFragment == TRUE)
	{
		glUseProgram(shaderProgramObject_PV);
	}
	else if (perFragment == FALSE && perVertex == TRUE)
	{
		glUseProgram(shaderProgramObject_PF);
	}

	// transformstions
	vmath::mat4 modelMatrix = vmath::mat4::identity();
	vmath::mat4 viewMatrix = vmath::mat4::identity();
	vmath::mat4 translationMatrix = vmath::mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -2.0f);

	vmath::mat4 rotationMatrixY = vmath::mat4::identity();
	rotationMatrixY = vmath::rotate(angleSphere_MP, 0.0f, 1.0f, 0.0f);
	vmath::mat4 rotationMatrix = vmath::mat4::identity();
	rotationMatrix = rotationMatrixY;

	modelMatrix = translationMatrix;

	// FOR PER VERTEX UNIFORM
	glUniformMatrix4fv(modelMatrixUniform_PV, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_PV, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_PV, 1, GL_FALSE, perspectiveProjectionMatrix_PV);

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform_PV[0], 1, light[0].ambient);
		glUniform3fv(ldUniform_PV[0], 1, light[0].diffuse);
		glUniform3fv(lsUniform_PV[0], 1, light[0].specular);
		glUniform4fv(lightPositionUniform_PV[0], 1, light[0].position[0]);

		glUniform3fv(laUniform_PV[1], 1, light[1].ambient);
		glUniform3fv(ldUniform_PV[1], 1, light[1].diffuse);
		glUniform3fv(lsUniform_PV[1], 1, light[1].specular);
		glUniform4fv(lightPositionUniform_PV[1], 1, light[1].position[1]);

		glUniform3fv(laUniform_PV[2], 1, light[2].ambient);
		glUniform3fv(ldUniform_PV[2], 1, light[2].diffuse);
		glUniform3fv(lsUniform_PV[2], 1, light[2].specular);
		glUniform4fv(lightPositionUniform_PV[2], 1, light[2].position[2]);

		glUniform3fv(kaUniform_PV, 1, materialAmbient);
		glUniform3fv(kdUniform_PV, 1, materialDiffuse);
		glUniform3fv(ksUniform_PV, 1, materialSpecular);
		glUniform1f(materialShininaseUniform_PV, materialShininnes);
		glUniform1i(LKeyPressedUniform_PV, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform_PV, 0);
	}

	// FOR PER FRAGMENT UNIFORM
	glUniformMatrix4fv(modelMatrixUniform_PF, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_PF, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_PF, 1, GL_FALSE, perspectiveProjectionMatrix_PF);

	if (bLight == TRUE)
	{
		glUniform3fv(laUniform_PF[0], 1, light[0].ambient);
		glUniform3fv(ldUniform_PF[0], 1, light[0].diffuse);
		glUniform3fv(lsUniform_PF[0], 1, light[0].specular);
		glUniform4fv(lightPositionUniform_PF[0], 1, light[0].position[0]);

		glUniform3fv(laUniform_PF[1], 1, light[1].ambient);
		glUniform3fv(ldUniform_PF[1], 1, light[1].diffuse);
		glUniform3fv(lsUniform_PF[1], 1, light[1].specular);
		glUniform4fv(lightPositionUniform_PF[1], 1, light[1].position[1]);

		glUniform3fv(laUniform_PF[2], 1, light[2].ambient);
		glUniform3fv(ldUniform_PF[2], 1, light[2].diffuse);
		glUniform3fv(lsUniform_PF[2], 1, light[2].specular);
		glUniform4fv(lightPositionUniform_PF[2], 1, light[2].position[2]);

		glUniform3fv(kaUniform_PF, 1, materialAmbient);
		glUniform3fv(kdUniform_PF, 1, materialDiffuse);
		glUniform3fv(ksUniform_PF, 1, materialSpecular);
		glUniform1f(materialShininaseUniform_PF, materialShininnes);
		glUniform1i(LKeyPressedUniform_PF, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform_PF, 0);
	}

	// bind with vao
	glBindVertexArray(gVao_sphere);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	// glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

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
	angleSphere_MP = angleSphere_MP + 0.01f;

	if (angleSphere_MP >= 360.0f)
	{
		angleSphere_MP = angleSphere_MP - 360.0f;
	}

	anglePos1 = anglePos1 + 0.005f;
	if (anglePos1 > 360.0f)
	{
		anglePos1 = anglePos1 - 360.0f;
	}

	anglePos2 = anglePos2 + 0.005f;
	if (anglePos2 > 360.0f)
	{
		anglePos2 = anglePos2 - 360.0f;
	}

	anglePos3 = anglePos3 + 0.005f;
	if (anglePos3 > 360.0f)
	{
		anglePos3 = anglePos3 - 360.0f;
	}

		light[0].position[0] = vec4(cos(anglePos1),sin(anglePos1),0.0,1.0);
		light[1].position[1] = vec4(0.0f,sin(anglePos2),cos(anglePos2), 1.0f);
		light[2].position[2] = vec4(sin(anglePos3), 0.0f,cos(anglePos3), 1.0f);
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

	// free vbo Element
	if (gVbo_sphere_element)
	{
		glDeleteBuffers(1, &gVbo_sphere_element);
		gVbo_sphere_element = 0;
	}
	// free vbo Normal
	if (gVbo_sphere_normal)
	{
		glDeleteBuffers(1, &gVbo_sphere_normal);
		gVbo_sphere_normal = 0;
	}

	// free vbo postion
	if (vbo_sphere_position)
	{
		glDeleteBuffers(1, &vbo_sphere_position);
		vbo_sphere_position = 0;
	}

	// free vao
	if (gVao_sphere)
	{
		glDeleteVertexArrays(1, &gVao_sphere);
		gVao_sphere = 0;
	}

	// detach delete , shader program object
	// PER VERTEX
	if (shaderProgramObject_PV)
	{
		glUseProgram(shaderProgramObject_PV);
		GLint numShaders;
		glGetProgramiv(shaderProgramObject_PV, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PV, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_PV, pShaders[i]);
					glDeleteShader(pShaders[i]);
				}
			}
			free(pShaders);
			pShaders = 0;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_PV);
	}

	// PER FRAGMENT
	if (shaderProgramObject_PF)
	{
		glUseProgram(shaderProgramObject_PF);
		GLint numShaders;
		glGetProgramiv(shaderProgramObject_PF, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_PF, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_PF, pShaders[i]);
					glDeleteShader(pShaders[i]);
				}
			}
			free(pShaders);
			pShaders = 0;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_PF);
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

void initializePerVertexShader(void)
{
	// VERTEX SHADERS
	//  write the shader sorce code 2)create the shader object 3) give the shader soure code to shader object 4) copile the code 5) do error compiletion shader code
	const GLchar *vertexShaderSourceCode_PV =
		"#version 460 core\n"
		"in vec4 aPosition;\n"
		"in vec3 aNormal;\n"
		"uniform mat4 uModelMatrix;\n"
		"uniform mat4 uViewMatrix;\n"
		"uniform mat4 uProjectionMatrix;\n"
		"uniform vec3 uLa[3];\n"
		"uniform vec3 uLd[3];\n"
		"uniform vec3 uLs[3];\n"
		"uniform vec3 uKa;\n"
		"uniform vec3 uKd;\n"
		"uniform vec3 uKs;\n"
		"uniform float uMaterialShinines;\n"
		"uniform vec4 uLightPosition[3];\n"
		"uniform int uLkeyPressed;\n"
		"out vec3 out_phong_ads_Light;\n"
		"void main(void)\n"
		"{gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n"
		"if\n"
		"(\n"
		"uLkeyPressed == 1\n"
		")\n"
		"{\n"
		"vec4 eyeCoordinate = uViewMatrix * uModelMatrix * aPosition;\n"
		"mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n"
		"vec3 transformedNormal = normalize(normalMatrix * aNormal);\n"
		"vec3 viwerVector =normalize(-eyeCoordinate.xyz);\n"
		"vec3 lightDirection[3];\n"
		"vec3 ambientLight[3];\n"
		"vec3 diffuseLight[3];\n"
		"vec3 reflectionVector[3];\n"
		"vec3 specularLight[3];\n"
		"out_phong_ads_Light = vec3(0.0,0.0,0.0);\n"
		"for\n"
		"(\n"
		"int i=0;i<3;i++\n"
		")\n"
		"{\n"
		"lightDirection[i]=normalize(vec3(uLightPosition[i] - eyeCoordinate));\n"
		"ambientLight[i] = uLa[i] * uKa;\n"
		"diffuseLight[i] = uLd[i] * uKd * max(dot(lightDirection[i],transformedNormal),0.0);\n"
		"reflectionVector[i] = reflect(-lightDirection[i],transformedNormal);\n"
		"specularLight[i] = uLs[i] * uKs * pow(max(dot(reflectionVector[i],viwerVector), 0.0),uMaterialShinines);\n"
		"out_phong_ads_Light = out_phong_ads_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];\n"
		"}\n"
		"}\n"
		"else\n"
		"{\n"
		"out_phong_ads_Light = vec3(1.0,1.0,1.0);\n"
		"}\n"
		"}\n";
	// "" \""  \"" \ ""; hi string separet hot nahi karan apn tyat \ use kela ahe tya mule ti single string dharlya jate

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode_PV, NULL);
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
	const GLchar *fargamentShaderSourceCode_PV =
		"#version 460 core\n"
		"in vec3 out_phong_ads_Light;\n"
		"out vec4 FragColor;\n"
		"void main(void)\n"
		"{FragColor=vec4(out_phong_ads_Light,1.0);\n"
		"}\n";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fargamentShaderSourceCode_PV, NULL);

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
	shaderProgramObject_PV = glCreateProgram();
	glAttachShader(shaderProgramObject_PV, vertexShaderObject);
	glAttachShader(shaderProgramObject_PV, fragmentShaderObject);

	// Bind Shader Attribute at certin index in shader to same index in host program
	glBindAttribLocation(shaderProgramObject_PV, AMC_ATTRIBUTE_POSITION, "aPosition"); // 0th index match cpu and gpu side
	glBindAttribLocation(shaderProgramObject_PV, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glLinkProgram(shaderProgramObject_PV);
	status = 0;
	infoLogLenght = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject_PV, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_PV, GL_INFO_LOG_LENGTH, &infoLogLenght);

		if (infoLogLenght > 0)
		{
			szInfoLog = (GLchar *)malloc(infoLogLenght * sizeof(GLchar));

			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_PV, infoLogLenght, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log", szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	modelMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uModelMatrix");
	viewMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uViewMatrix");
	projectionMatrixUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uProjectionMatrix");

	laUniform_PV[0] = glGetUniformLocation(shaderProgramObject_PV, "uLa[0]");
	ldUniform_PV[0] = glGetUniformLocation(shaderProgramObject_PV, "uLd[0]");
	lsUniform_PV[0] = glGetUniformLocation(shaderProgramObject_PV, "uLs[0]");
	lightPositionUniform_PV[0] = glGetUniformLocation(shaderProgramObject_PV, "uLightPosition[0]");

	laUniform_PV[1] = glGetUniformLocation(shaderProgramObject_PV, "uLa[1]");
	ldUniform_PV[1] = glGetUniformLocation(shaderProgramObject_PV, "uLd[1]");
	lsUniform_PV[1] = glGetUniformLocation(shaderProgramObject_PV, "uLs[1]");
	lightPositionUniform_PV[1] = glGetUniformLocation(shaderProgramObject_PV, "uLightPosition[1]");

	laUniform_PV[2] = glGetUniformLocation(shaderProgramObject_PV, "uLa[2]");
	ldUniform_PV[2] = glGetUniformLocation(shaderProgramObject_PV, "uLd[2]");
	lsUniform_PV[2] = glGetUniformLocation(shaderProgramObject_PV, "uLs[2]");
	lightPositionUniform_PV[2] = glGetUniformLocation(shaderProgramObject_PV, "uLightPosition[2]");

	kaUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uKa");
	kdUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uKd");
	ksUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uKs");
	materialShininaseUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uMaterialShinines");
	LKeyPressedUniform_PV = glGetUniformLocation(shaderProgramObject_PV, "uLkeyPressed");
}

void initializePerFragmentShader(void)
{
	// VERTEX SHADERS
	//  write the shader sorce code 2)create the shader object 3) give the shader soure code to shader object 4) copile the code 5) do error compiletion shader code
	const GLchar *vertexShaderSourceCode_PF =
		"#version 460 core\n"
		"in vec4 aPosition;\n"
		"in vec3 aNormal;\n"
		"uniform mat4 uModelMatrix;\n"
		"uniform mat4 uViewMatrix;\n"
		"uniform mat4 uProjectionMatrix;\n"
		"out vec3 out_transformedNormals;\n"
		"out vec3 out_viwerVector;\n"
		"out vec3 out_lightDirection[3];\n"
		"uniform vec4 uLightPosition[3];\n"
		"uniform int uLkeyPressed;\n"
		"void main(void)\n"
		"{gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n"
		"if\n"
		"(\n"
		"uLkeyPressed == 1\n"
		")\n"
		"{\n"
		"vec4 eyeCoordinate = uViewMatrix * uModelMatrix * aPosition;\n"
		"mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n"
		"out_transformedNormals = normalMatrix * aNormal;\n"
		"for\n"
		"(\n"
		"int i=0;i<3;i++\n"
		")\n"
		"{\n"
		"out_lightDirection[i] = vec3(uLightPosition[i] - eyeCoordinate);\n"
		"out_viwerVector = -eyeCoordinate.xyz;\n"
		"}\n"
		"}\n"
		"}\n"; // "" \""  \"" \ ""; hi string separet hot nahi karan apn tyat \ use kela ahe tya mule ti single string dharlya jate

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar **)&vertexShaderSourceCode_PF, NULL);
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
	const GLchar *fargamentShaderSourceCode_PF =
		"#version 460 core\n"
		"in vec3 out_transformedNormals;\n"
		"in vec3 out_viwerVector;\n"
		"in vec3 out_lightDirection[3];\n"
		"out vec4 FragColor;\n"
		"uniform vec3 uLa[3];\n"
		"uniform vec3 uLd[3];\n"
		"uniform vec3 uLs[3];\n"
		"uniform vec3 uKa;\n"
		"uniform vec3 uKd;\n"
		"uniform vec3 uKs;\n"
		"uniform float uMaterialShinines;\n"
		"uniform int uLkeyPressed;\n"
		"void main(void)\n"
		"{\n"
		"vec3 phong_ads_Light;\n"
		"if\n"
		"(\n"
		"uLkeyPressed == 1\n"
		")\n"
		"{\n"
		"vec3 normalizedLightDirection[3];\n"
		"vec3 ambientLight[3];\n"
		"vec3 diffuseLight[3];\n"
		"vec3 reflectionVector[3];\n"
		"vec3 specularLight[3];\n"
		"vec3 normalizedTransformedNormal =  normalize(out_transformedNormals);\n"
		"vec3 normalizedViwerVector = normalize(out_viwerVector);\n"
		"for\n"
		"(\n"
		"int i=0;i<3;i++\n"
		")\n"
		"{\n"
		"normalizedLightDirection[i] = normalize(out_lightDirection[i]);\n"
		"ambientLight[i] = uLa[i] * uKa;\n"
		"diffuseLight[i] = uLd[i] * uKd * max(dot(normalizedLightDirection[i],normalizedTransformedNormal),0.0);\n"
		"reflectionVector[i] = reflect(-normalizedLightDirection[i],normalizedTransformedNormal);\n"
		"specularLight[i] = uLs[i] * uKs * pow(max(dot(reflectionVector[i],normalizedViwerVector), 0.0),uMaterialShinines);\n"
		"phong_ads_Light = phong_ads_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];\n"
		"}\n"
		"}\n"
		"else\n"
		"{\n"
		"phong_ads_Light = vec3(1.0,1.0,1.0);\n"
		"}\n"
		"FragColor=vec4(phong_ads_Light,1.0);\n"
		"}\n";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar **)&fargamentShaderSourceCode_PF, NULL);

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
	shaderProgramObject_PF = glCreateProgram();
	glAttachShader(shaderProgramObject_PF, vertexShaderObject);
	glAttachShader(shaderProgramObject_PF, fragmentShaderObject);

	// Bind Shader Attribute at certin index in shader to same index in host program
	glBindAttribLocation(shaderProgramObject_PF, AMC_ATTRIBUTE_POSITION, "aPosition"); // 0th index match cpu and gpu side
	glBindAttribLocation(shaderProgramObject_PF, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glLinkProgram(shaderProgramObject_PF);
	status = 0;
	infoLogLenght = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject_PF, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_PF, GL_INFO_LOG_LENGTH, &infoLogLenght);

		if (infoLogLenght > 0)
		{
			szInfoLog = (GLchar *)malloc(infoLogLenght * sizeof(GLchar));

			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_PF, infoLogLenght, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log", szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	modelMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uModelMatrix");
	viewMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uViewMatrix");
	projectionMatrixUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uProjectionMatrix");
	laUniform_PF[0] = glGetUniformLocation(shaderProgramObject_PF, "uLa[0]");
	ldUniform_PF[0] = glGetUniformLocation(shaderProgramObject_PF, "uLd[0]");
	lsUniform_PF[0] = glGetUniformLocation(shaderProgramObject_PF, "uLs[0]");
	lightPositionUniform_PF[0] = glGetUniformLocation(shaderProgramObject_PF, "uLightPosition[0]");

	laUniform_PF[1] = glGetUniformLocation(shaderProgramObject_PF, "uLa[1]");
	ldUniform_PF[1] = glGetUniformLocation(shaderProgramObject_PF, "uLd[1]");
	lsUniform_PF[1] = glGetUniformLocation(shaderProgramObject_PF, "uLs[1]");
	lightPositionUniform_PF[1] = glGetUniformLocation(shaderProgramObject_PF, "uLightPosition[1]");

	laUniform_PF[2] = glGetUniformLocation(shaderProgramObject_PF, "uLa[2]");
	ldUniform_PF[2] = glGetUniformLocation(shaderProgramObject_PF, "uLd[2]");
	lsUniform_PF[2] = glGetUniformLocation(shaderProgramObject_PF, "uLs[2]");
	lightPositionUniform_PF[2] = glGetUniformLocation(shaderProgramObject_PF, "uLightPosition[2]");

	kaUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uKa");
	kdUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uKd");
	ksUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uKs");
	materialShininaseUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uMaterialShinines");

	LKeyPressedUniform_PF = glGetUniformLocation(shaderProgramObject_PF, "uLkeyPressed");
}
