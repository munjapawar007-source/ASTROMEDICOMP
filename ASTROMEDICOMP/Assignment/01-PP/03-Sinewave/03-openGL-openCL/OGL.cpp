// Win32 Headers
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES 1
#include <math.h>

// OGL Related File
#include <GL/glew.h> //this headr file must be include before gl/gl.h
#include <gl/GL.h>	 //inside this prototypes presents
#include "OGL.h"
#include "vmath.h"
using namespace vmath;

// OpenCL Header files and library
#include <CL/opencl.h>
#pragma comment(lib, "opencl.lib")

// OpenGL Related Libraries
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib") // for Linker dyanamic libarary presents here

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
	AMC_ATTRIBUTE_POSITION = 0,

};

GLuint vao = 0;
GLuint vbo_cpu = 0;

GLuint mvpMatrixUniform = 0; // mvp model view position
mat4 perspectiveProjectionMatrix;

// sinwave
const unsigned int gMeshWidth = 1024;
const unsigned int gMeshHeight = 1024;
const unsigned int gMeshDepth = 4;

#define MESH_ARRAY_SIZE gMeshWidth * gMeshHeight * gMeshDepth

float position[gMeshWidth][gMeshHeight][gMeshDepth];
GLuint vbo_gpu = 0;
float animationTime = 0.0f;
GLuint colorSinewaveUniform = 0;

// sinewave color
BOOL RED = FALSE;
BOOL YALLOW = FALSE;
BOOL BLUE = FALSE;

// openCL related variables
cl_int oclResult;  
cl_mem oclGraphicsResource; 		// mem for memory and it is already pointer
cl_device_id oclDeviceID;			// struct pointer
cl_context oclContext = NULL;		// struct pointer
cl_command_queue oclCommandQueue; 	// struct pointer
cl_program oclProgram = NULL;   	// struct pointer
cl_kernel oclKernel = NULL;  		// struct pointer

BOOL bOnGPU = FALSE;

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
//color key
		case 'R':
		case 'r':
			RED = TRUE;
			YALLOW = FALSE;
			BLUE = FALSE;
			break;

		case 'Y':
		case 'y':
			YALLOW = TRUE;
			RED = FALSE;
			BLUE = FALSE;
			break;

		case 'B':
		case 'b':
			BLUE = TRUE;
			YALLOW = FALSE;
			RED = FALSE;
			break;
// run code on cpu / gpu key
		case 'C':
		case 'c':
			bOnGPU = FALSE;
			break;
		case 'G':
		case 'g':
			bOnGPU = TRUE;
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

	// opencl related initializaton
	cl_platform_id oclPlatformID;
	cl_device_id *oclDeviceIDs = NULL; // struct pointer to pointer array
	cl_uint devCount = 0;

	// step1: first get platform id
	oclResult = clGetPlatformIDs(1, &oclPlatformID, NULL);
	if (oclResult != CL_SUCCESS)
	{
		fprintf(gpFile, "clGetPlatformIDs Failed\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}
	
	// step2: get opencl supported device count
	oclResult = clGetDeviceIDs(oclPlatformID, CL_DEVICE_TYPE_GPU, 0, NULL,&devCount);
	if (oclResult != CL_SUCCESS)
	{
		fprintf(gpFile, "clGetDeviceIDs Failed 1st call.\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}
	else if (devCount == 0)
	{
		fprintf(gpFile, "clGetDeviceIDs Successed but openCL device not found\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	// accordingly allocate memory to our device id array
	oclDeviceIDs = (cl_device_id *)malloc(sizeof(cl_device_id) * devCount);

	// now fill our device id array by calling clGetdeviceid function
	oclResult = clGetDeviceIDs(oclPlatformID, CL_DEVICE_TYPE_GPU, devCount, oclDeviceIDs, NULL);
	if (oclResult != CL_SUCCESS)
	{
		fprintf(gpFile, "clGetDeviceIDs Failed 2nd call.\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	// from above array select the 0th device
	oclDeviceID = oclDeviceIDs[0];

	// free the memory give to array
	free(oclDeviceIDs);

	// initialize openCL context proprtys array // for linux:	(cl_context_properties)glXGetCurrentContext(); // CL_XGL_DISPLAY_KHR, (cl_context_properties)glXGetCurrntDC
	cl_context_properties oclContextProperties[] = {
		CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), 
		CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),		  
		CL_CONTEXT_PLATFORM, (cl_context_properties)oclPlatformID, 0};

	// create context using above array
	oclContext = clCreateContext(oclContextProperties, 1, &oclDeviceID, NULL, NULL, &oclResult);
	if (oclResult != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateContext Failed.\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	// create opencl command Queue
	oclCommandQueue = clCreateCommandQueueWithProperties(oclContext, oclDeviceID, 0, &oclResult);
	if (oclResult != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateCommandQueueWithProperties Failed.\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	
	// right openCL source Code
	const char *oclKernelSourceCode = "__kernel void sineWaveKernel(__global float4 *pos, unsigned int width, unsigned int height, float time)" \
									  "{" \
									  "unsigned int i = get_global_id(0);" \
									  "unsigned int j = get_global_id(1);" \
									  "float u = (float)i / (float)width;" \
									  "float v = (float)j / (float)height;" \
									  "u = u * 2.0 - 1.0;" \
									  "v = v * 2.0 - 1.0;" \
									  "float frequency = 4.0f;" \
									  "float w = sin(u * frequency + time) * cos(v * frequency + time) * 0.5;" \
									  "pos[j * width + i] = (float4)(u, w, v, 1.0f);" \
									  "}";

							
	// create openck program using above kernel source code
	oclProgram = clCreateProgramWithSource(oclContext, 1, (const char **)&oclKernelSourceCode, NULL, &oclResult);
		fprintf(gpFile, "__DBG__5\n");
	fflush(gpFile);
	if (oclResult != CL_SUCCESS)
	{
		fprintf(gpFile, "clCreateProgramWithSource Failed.\n");

		uninitialize();

		exit(EXIT_FAILURE);
	}

	// build above opencl program
	oclResult = clBuildProgram(oclProgram, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
	if (oclResult != CL_SUCCESS)
	{
		cl_int oclBuildResult;
		char szBuffer[512];
		oclBuildResult = clGetProgramBuildInfo(oclProgram, oclDeviceID, CL_PROGRAM_BUILD_LOG, sizeof(szBuffer), szBuffer, NULL);
		if (oclBuildResult != CL_SUCCESS)
		{
			fprintf(gpFile, "clGetProgramBuildInfo Failed.\n");
			uninitialize();
			exit(EXIT_FAILURE);
		}
		fprintf(gpFile, " program build log: %s.\n", szBuffer);
		uninitialize();
		exit(EXIT_FAILURE);
	}

	// create openCl Kernel
	oclKernel = clCreateKernel(oclProgram,"sineWaveKernel",&oclResult);
	if (oclResult != CL_SUCCESS)
	{
		fprintf(gpFile,"clCreateKernel Failed.\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	// VERTEX SHADERS
	//  write the shader sorce code 2)create the shader object 3) give the shader soure code to shader object 4) Compile Shader the code 5) do error compiletion shader code
	const GLchar *vertexShaderSourceCode =
		"#version 460 core\n"
		"in vec4 aPosition;\n"
		"uniform mat4 uMVPMatrix;\n"
		"void main(void)\n"
		"{gl_Position=uMVPMatrix*aPosition;\n"
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
		"#version 460 core\n"
		"out vec4 FragColor;\n"
		"uniform vec4 uColorSinewave;\n"
		"void main(void)\n"
		"{FragColor=uColorSinewave;\n"
		"}\n";

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
	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition"); // 0th index match cpu and gpu side

	glLinkProgram(shaderProgramObject);
	status = 0;
	infoLogLenght = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLenght);

		if (infoLogLenght > 0)
		{
			szInfoLog = (GLchar *)malloc(infoLogLenght * sizeof(GLchar));

			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLenght, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log: %s\n", szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	// get the required uniform loction from the sheder
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix"); // openGL mala de shader program badal ch location (shaderprogra deil uniform )
	colorSinewaveUniform = glGetUniformLocation(shaderProgramObject, "uColorSinewave");

	
	// provide vertex position,color,texcode, and normal etc.
	for (int i = 0; i < (int)gMeshWidth; i++)
	{
		for (int j = 0; j < (int)gMeshHeight; j++)
		{
			for (int k = 0; k < (int)gMeshDepth; k++)
			{
				position[i][j][k] = 0.0f;
			}
		}
	}

	// Vertex array object for vertex attribut
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Position
	glGenBuffers(1, &vbo_cpu); // all arrays associate with vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_cpu);
	glBufferData(GL_ARRAY_BUFFER, MESH_ARRAY_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW); // dynamic draw use here for in future we can change array size on runtime
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Position for GPU
	glGenBuffers(1, &vbo_gpu); // all arrays associate with vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu);
	glBufferData(GL_ARRAY_BUFFER, MESH_ARRAY_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	// depth related code
	glClearDepth(1.0f);		 // display madhe color buffer madhe gleyavr majya depth buffer madhe ja ani change all bits 1.0f
	glEnable(GL_DEPTH_TEST); // depth test on
	glDepthFunc(GL_LEQUAL);	 // tyt tyt fragment la pass kr jyanchi value 1 peksh kami asel kiva equal asel


	// tell the openGL choose the clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// create opencl graphics Resource
	oclGraphicsResource = clCreateFromGLBuffer(oclContext,CL_MEM_WRITE_ONLY,vbo_gpu,&oclResult);
	if (oclResult != CL_SUCCESS)
	{
		fprintf(gpFile,"clCreateFromGLBuffer Failed.\n");
		uninitialize();
		exit(EXIT_FAILURE);
	}

	perspectiveProjectionMatrix = mat4::identity(); // this is annalogas to load identity (resize madhil) in ffp

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
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	// code
	void sineWave(unsigned int, unsigned int, float);
	void uninitialize(void);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use Shader program object
	glUseProgram(shaderProgramObject);

	// transformstions
	vmath::mat4 modelViewMatrix = vmath::mat4::identity();
	vmath::mat4 ModelViewProjectionMatrix = vmath::mat4::identity();
	ModelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order is important

	// send this matrix to the vertex shader to the uniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, ModelViewProjectionMatrix);

	if (RED == TRUE)
		glUniform4fv(colorSinewaveUniform, 1, vmath::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	if (YALLOW == TRUE)
		glUniform4fv(colorSinewaveUniform, 1, vmath::vec4(1.0f, 1.0f, 0.0f, 1.0f));

	if (BLUE == TRUE)
		glUniform4fv(colorSinewaveUniform, 1, vmath::vec4(0.0f, 0.0f, 1.0f, 1.0f));

	if (RED == FALSE && YALLOW == FALSE && BLUE == FALSE)
		glUniform4fv(colorSinewaveUniform, 1, vmath::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// bind with vao
	glBindVertexArray(vao);

	if(bOnGPU == TRUE)
	{
		// set 0th kernel orgumant
		oclResult = clSetKernelArg(oclKernel,0,sizeof(cl_mem),(void*)&oclGraphicsResource);
		if (oclResult != CL_SUCCESS)
		{
			fprintf(gpFile,"clSetKernelArg 0th Failed.\n");
			uninitialize();
			exit(EXIT_FAILURE);
		}

		// set 1th kernel orgumant
		oclResult = clSetKernelArg(oclKernel,1,sizeof(unsigned int),&gMeshWidth);
		if (oclResult != CL_SUCCESS)
		{
			fprintf(gpFile,"clSetKernelArg 1st Failed.\n");
			uninitialize();
			exit(EXIT_FAILURE);
		}

		// set 2th kernel orgumant
		oclResult = clSetKernelArg(oclKernel,2,sizeof(unsigned int),&gMeshHeight);
		if (oclResult != CL_SUCCESS)
		{
			fprintf(gpFile,"clSetKernelArg 2nd Failed.\n");
			uninitialize();
			exit(EXIT_FAILURE);
		}

		// set 3rd kernel orgumant
		oclResult = clSetKernelArg(oclKernel,3,sizeof(float),&animationTime);
		if (oclResult != CL_SUCCESS)
		{
			fprintf(gpFile,"clSetKernelArg 3rd Failed.\n");
			uninitialize();
			exit(EXIT_FAILURE);
		}

		// Acquire openCL Graphics resource
		oclResult = clEnqueueAcquireGLObjects(oclCommandQueue,1,&oclGraphicsResource,0,NULL,NULL); //4th waight event cha array asel tr 
		
		if (oclResult != CL_SUCCESS)
		{
			fprintf(gpFile,"clEnqueueAcquireGLObjects Failed.\n");
			uninitialize();
			exit(EXIT_FAILURE);
		}

		// call openCL Kerenel
		size_t globalWorkSize[2];
		globalWorkSize[0] = gMeshWidth;
		globalWorkSize[1] = gMeshHeight;

		size_t localWorkSize[2];
		localWorkSize[0] = 8;
		localWorkSize[1] = 8;

		oclResult = clEnqueueNDRangeKernel(oclCommandQueue,oclKernel,2,NULL,globalWorkSize,localWorkSize,0,NULL,NULL);
		if (oclResult != CL_SUCCESS)
		{
			fprintf(gpFile,"clEnqueueNDRangeKernel Failed.\n");
			uninitialize();
			exit(EXIT_FAILURE);
		}

	// Release openCL Graphics resource
		oclResult = clEnqueueReleaseGLObjects(oclCommandQueue,1,&oclGraphicsResource,0,NULL,NULL); //4th waight event cha array asel tr 
		if (oclResult != CL_SUCCESS)
		{
			fprintf(gpFile,"clEnqueueReleaseGLObjects Failed.\n");
			uninitialize();
			exit(EXIT_FAILURE);
		}

		// finsh the command Queue
		clFinish(oclCommandQueue);

		//bind with GPU BUFFER
		glBindBuffer(GL_ARRAY_BUFFER, vbo_gpu); // recording aikane

	}
	else
	{
		sineWave(gMeshWidth, gMeshHeight, animationTime);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_cpu);
		glBufferData(GL_ARRAY_BUFFER, MESH_ARRAY_SIZE * sizeof(float), position, GL_DYNAMIC_DRAW);
	
	}

	// Draw sinewave
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	// draw vertex array
	glDrawArrays(GL_POINTS, 0, gMeshWidth * gMeshHeight);

	// unBind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// unUse shader program
	glUseProgram(0);

	// swap the buffers
	SwapBuffers(ghdc);
}

void sineWave(unsigned int mesh_width, unsigned int mesh_height, float time)
{
	// code
	for (int i = 0; i < (int)gMeshWidth; i++)
	{
		for (int j = 0; j < (int)gMeshHeight; j++)
		{
			for (int k = 0; k < (int)gMeshDepth; k++)
			{
				float u = (float)i / (float)mesh_width;
				float v = (float)j / (float)mesh_height;

				u = u * 2.0 - 1.0;  //inshort normalize here
				v = v * 2.0 - 1.0;

				float frequency = 6.0f;		// yeka wave madhe kiti pattern ale pahije
				float w = sin(u * frequency + time) * cos(v * frequency + time) * 0.5;

				if (k == 0)
				{
					position[i][j][k] = u;
				}

				if (k == 1)
				{
					position[i][j][k] = w;
				}

				if (k == 2)
				{
					position[i][j][k] = v;
				}

				if (k == 3)
				{
					position[i][j][k] = 1.0f;
				}
			}
		}
	}
}

void update(void)
{
	// code
	animationTime = animationTime + 0.01;
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

	if(vbo_gpu)
	{
		if(oclGraphicsResource)
		{
			clReleaseMemObject(oclGraphicsResource);
			oclGraphicsResource = NULL;
		}
		glDeleteBuffers(1,&vbo_gpu);
		vbo_gpu = 0;
	}

	if(oclKernel)
	{
		clReleaseKernel(oclKernel);
		oclKernel =NULL;
	}

	if(oclProgram)
	{
		clReleaseProgram(oclProgram);
		oclProgram = NULL;
	}

	if(oclCommandQueue)
	{
		clReleaseCommandQueue(oclCommandQueue);
		oclCommandQueue = NULL;
	}

	if(oclContext)
	{
		clReleaseContext(oclContext);
		oclContext = NULL;
	}
	
	// free vbo postion
	if (vbo_cpu)
	{
		glDeleteBuffers(1, &vbo_cpu);
		vbo_cpu = 0;
	}

	// free vao
	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

	// detach delete shader program object
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
