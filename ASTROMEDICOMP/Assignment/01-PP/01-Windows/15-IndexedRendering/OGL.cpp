// Win32 Headers
#include<windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <time.h>
#define _USE_MATH_DEFINES 1

//OGL Related File
#include<GL/glew.h> //this headr file must be include before gl/gl.h
#include<gl/GL.h> //inside this prototypes presents
#include "OGL.h"
#include"vmath.h"
using namespace vmath;

//OpenGL Related Libraries
#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"opengl32.lib") //for Linker dyanamic libarary presents here

// MICROS
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Global Function Declaration/prototype/ signature
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

// Global Variable declaration 
//Variables Related FullScreen
BOOL gbFullScreen = FALSE;
HWND ghwnd = NULL;
DWORD dwStyle; // to hold value
WINDOWPLACEMENT wpPrev;

//Variable Related File I/O
char gszLogFileName[] = "Log.txt";
FILE* gpFile = NULL;

// Active Window Related Variable
BOOL gbActiveWindow = FALSE;

// Exit keyPress Related
BOOL gbEscapeKeyPressed = FALSE;

//OpenGL Related Variables
HDC ghdc = NULL;
HGLRC ghrc = NULL;  //  handal to Graphics rendering

//Shader related global veriables
GLuint shaderProgramObject = 0;
enum
{
	AMC_ATTRIBUTE_POSITION = 0,

};

GLuint vao = 0;
GLuint vbo_position = 0;
GLuint vbo_index = 0;

GLuint  mvpMatrixUniform = 0; //mvp model view position 

mat4 perspectiveProjectionMatrix;


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
	//Creat Log File
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
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; //owndc self device contex
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
	int m_x = (GetSystemMetrics(SM_CXSCREEN) - WIN_WIDTH)/2; //main screen Dimension in GetSystemMetrics for x And y
	int m_y = (GetSystemMetrics(SM_CYSCREEN) - WIN_HEIGHT)/2;
	// Create Window
	hwnd = CreateWindowEx(WS_EX_APPWINDOW, szAppName, TEXT("MP"), WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE, m_x, m_y, WIN_WIDTH, WIN_HEIGHT, NULL, NULL, hInstance, NULL);

	ghwnd = hwnd; // value assinged

	// show window
	ShowWindow(hwnd, iCmdShow); // show window if you not give

	// Paint Backgraound window
	UpdateWindow(hwnd);

	
	//initialize
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


	//Game Loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) //PM_REMOVE
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
			
			if (gbActiveWindow == TRUE) //to stop (game animation)window when you u open unother window
			{
				if (gbEscapeKeyPressed == TRUE)
				{
					bDone = TRUE;
				}
				//render
				display();

				// update
				update();
			}
		}
	}

	//uninitialize
	uninitialize();

	return((int)msg.wParam);
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
		ZeroMemory((void*)&wpPrev,sizeof(WINDOWPLACEMENT)); //memset
		wpPrev.length = sizeof(WINDOWPLACEMENT);
		break;
	case WM_SETFOCUS:
		gbActiveWindow = TRUE;
		break;
	case WM_KILLFOCUS:
		gbActiveWindow = FALSE;
		break;
	case WM_ERASEBKGND:  //  for smooth Rendaring
		return(0);
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
	return(DefWindowProc(hwnd, iMsg, wparam, lparam));
}

void toggleFullScreen(void) //stub function manje jya madhe code nahi
{
	// Variable declarations
	MONITORINFO mi;

	//code
	if (gbFullScreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);


		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			ZeroMemory((void*)&mi, sizeof(MONITORINFO));
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

	//code
	// Pixel Format Descreptor initialize
	ZeroMemory((void*)&pfd, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;  //certin part of memory which is filled
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;
	pfd.cDepthBits = 32;

	//GetDc
	ghdc = GetDC(ghwnd);

	if (ghdc == NULL)
	{
		fprintf(gpFile, "Get Dc Function Fail\n");
		return(-1);
	}

	//Get Matching Pixel Format Index Using Hdc And Pfd
	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd);
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat func Failed \n");
		return(-2);
	}

	// Select This pixel format of found index
	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile,"ChoosePixelFormat func Failed \n");
		return(-3);
	}

	//briging api (wsi = window system inegrations)
	//Create Randaring Consept using ghdc,pfd,choosePixelFormat
	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile,"wglCreateContext func Failed\n");
		return(-4);
	}

	//make this rendering contex as current contex
	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent func Failed\n");
		return(-5);
	}

	//initialize GLEW
	glewResult =  glewInit();
	if(glewResult != GLEW_OK)
	{
		fprintf(gpFile, " glewInit func Failed\n");
		return(-6);
	}

	//print gl info
	//printGLInfo();

	//VERTEX SHADERS
	// write the shader sorce code 2)create the shader object 3) give the shader soure code to shader object 4) copile the code 5) do error compiletion shader code 
	const GLchar* vertexShaderSourceCode = 
	"#version 460 core\n" \
		"in vec4 aPosition;\n" \
		"uniform mat4 uMVPMatrix;\n" \
	"void main(void)\n" \
	"{gl_Position=uMVPMatrix*aPosition;\n" \
	"}\n";
	// "" \""  \"" \ ""; hi string separet hot nahi karan apn tyat \ use kela ahe tya mule ti single string dharlya jate

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject,1,(const GLchar**)&vertexShaderSourceCode,NULL);
	// in 2nd para how many shader u use and 4th is the lenth of 2nd shader arrey

	glCompileShader(vertexShaderObject);

	GLint status = 0;
	GLint infoLogLenght = 0;
	GLchar* szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject,GL_COMPILE_STATUS,&status);
	if(status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject,GL_INFO_LOG_LENGTH,&infoLogLenght);
		if(infoLogLenght > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLenght * sizeof(GLchar));
		

			if(szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject,infoLogLenght,NULL,szInfoLog);
				fprintf(gpFile,"vertex Shader Compilation log = %s", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
	    }
	 uninitialize();
	}

	//FRAGMENT SHADER
	const GLchar* fargamentShaderSourceCode =
	"#version 460 core\n" \
		"out vec4 FragColor;\n" \
	"void main(void)\n" \
	"{FragColor=vec4(1.0f,1.0f,1.0f,1.0f);\n" \
	"}\n";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject,1,(const GLchar**)&fargamentShaderSourceCode,NULL);

	glCompileShader(fragmentShaderObject);

	status = 0;
	infoLogLenght = 0;
	szInfoLog = NULL;
	glGetShaderiv(fragmentShaderObject,GL_COMPILE_STATUS,&status);
	if(status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject,GL_INFO_LOG_LENGTH,&infoLogLenght);
		if(infoLogLenght > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLenght * sizeof(GLchar));

			if(szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject,infoLogLenght,NULL,szInfoLog);
				fprintf(gpFile,"FRAGMENT Shader Compilation log = %s", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
	    }
	 uninitialize();
	}

	//Create , Attach , link shader program object
	shaderProgramObject = glCreateProgram();
	glAttachShader(shaderProgramObject,vertexShaderObject);
	glAttachShader(shaderProgramObject,fragmentShaderObject);

	//Bind Shader Attribute at certin index in shader to same index in host program
	glBindAttribLocation(shaderProgramObject,AMC_ATTRIBUTE_POSITION,"aPosition"); //0th index match cpu and gpu side 

	glLinkProgram(shaderProgramObject);
	status = 0;
	infoLogLenght = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject,GL_LINK_STATUS,&status);
	if(status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject,GL_INFO_LOG_LENGTH,&infoLogLenght);

		if(infoLogLenght > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLenght * sizeof(GLchar));

			if(szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject,infoLogLenght,NULL,szInfoLog);
				fprintf(gpFile,"Shader Program Link Log",szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}

	//get the required uniform loction from the sheder
	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject,"uMVPMatrix"); //openGL mala de shader program badal ch location (shaderprogra deil uniform )

	// depth related code
	glClearDepth(1.0f);  // display madhe color buffer madhe gleyavr majya depth buffer madhe ja ani change all bits 1.0f
	glEnable(GL_DEPTH_TEST); //depth test on
	glDepthFunc(GL_LEQUAL);  //tyt tyt fragment la pass kr jyanchi value 1 peksh kami asel kiva equal asel
		
		
	// tell the openGL choose the clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	//provide vertex position,color,texcode, and normal etc.
	const GLfloat Rectangle_position[] =
	{
		1.0,1.0,0.0,
 		-1.0,1.0,0.0,
		-1.0,-1.0,0.0,
		1.0,-1.0,0.0,
	};

	const GLuint  indices[] = 
	{
		0,1,3,
		3,1,2,
	};

	// Vertex array object for vertex attribut
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);

	// Position
	glGenBuffers(1,&vbo_position); //all arrays associate with vao
	glBindBuffer(GL_ARRAY_BUFFER,vbo_position);
	glBufferData(GL_ARRAY_BUFFER,sizeof(Rectangle_position),Rectangle_position,GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION,3,GL_FLOAT,GL_FALSE,0,NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER,0);

	// element vbo
	glGenBuffers(1, &vbo_index);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_index);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);
	
	perspectiveProjectionMatrix = mat4::identity(); //this is annalogas to load identity (resize madhil) in ffp

	// warm up resize
	resize(WIN_WIDTH, WIN_HEIGHT);
	return(0);
}

void printGLInfo(void)
{
	//veriable declarations
	GLint numExtentions, i;

	//code
	//print opengGL info
	fprintf(gpFile, "OPENGL Information\n");
	fprintf(gpFile, "********************\n");
	fprintf(gpFile, "openGL Vendor : %s\n", glGetString(GL_VENDOR));
	fprintf(gpFile, "openGL Renderer : %s\n", glGetString(GL_RENDERER));
	fprintf(gpFile, "openGL Version : %s\n", glGetString(GL_VERSION));
	fprintf(gpFile, "glsl Version: %s\n",glGetString(GL_SHADING_LANGUAGE_VERSION));
	//get number of extentions
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExtentions);	

	//PRINT openGl Extentions
	for(i=0;i<numExtentions;i++)
	{
		fprintf(gpFile,"%s\n",glGetStringi(GL_EXTENSIONS,i));
	}
	fprintf(gpFile, "********************\n");



}

void resize(int width, int height)
{
	//code
	//if height by default 0 makes1
	if (height <= 0)
	{
		height = 1;
	}
	//set the viewPort
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}

void display(void)
{
	//code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//use Shader program object
	glUseProgram(shaderProgramObject);

	//transformstions
	vmath::mat4 modelViewMatrix = vmath::mat4::identity();
	vmath::mat4 translationMatrix = vmath::mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -4.0f);
	modelViewMatrix = translationMatrix;
    vmath::mat4 ModelViewProjectionMatrix = vmath::mat4::identity();
    ModelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; //order is important

	//send this matrix to the vertex shader to the uniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, ModelViewProjectionMatrix);


	//bind with vao
	glBindVertexArray(vao);

	 
	//draw vertex array
	//glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	//unUse shader program
	glUseProgram(0);

	//swap the buffers
	SwapBuffers(ghdc);	
}

void update(void)
{
	//code
}

void uninitialize(void)
{
	// Function Declarations
	void toggleFullScreen(void);

	//code
	//if user is existing FullScreen then restore back to normal 
	if (gbFullScreen == TRUE)
	{
		toggleFullScreen();
		gbFullScreen = FALSE;
	}

	if(vbo_index)
	{
      glDeleteBuffers(1,&vbo_index);
	  vbo_index = 0;

	}

	//free vbo postion
	if(vbo_position)
	{
      glDeleteBuffers(1,&vbo_position);
	  vbo_position = 0;

	}

	//free vao
	if(vao)
	{
      glDeleteVertexArrays(1,&vao);
      vao= 0;

	}

	//detach delete , shader program object
	if(shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);
		GLint numShaders;
		glGetProgramiv(shaderProgramObject,GL_ATTACHED_SHADERS,&numShaders);
		if(numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if(pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject,numShaders,NULL,pShaders);
				for(GLint i = 0; i<numShaders;i++)
				{
					glDetachShader(shaderProgramObject,pShaders[i]);
					glDeleteShader(pShaders[i]);
					
				}
			}
			free(pShaders);
			pShaders = 0;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
	}

	//make hdc current contex by relesing randering current contex
	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	//delete the randering contex
	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;
	}

	//Relase the dc
	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
		ghdc = NULL;
	}
	//Distroy Window
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

