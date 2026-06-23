// stander heder files
#include <stdio.h>  //printf
#include <stdlib.h> //for exit
#include <memory.h> // for memset
#include <math.h>
#include <time.h>
#define _USE_MATH_DEFINES 1

// xlib hederfiles
#include <X11/Xlib.h>       //for xlib api
#include <X11/Xutil.h>      //for visule info and related api
#include <X11/XKBlib.h>     //for keyboard Related API

// OpenGl Related Header files
#include <GL/glew.h>        //<GL/glew.h> headr file must be include before gl/gl.h
#include <GL/gl.h>
#include <GL/glx.h>         //for bridging api
#include "vmath.h"
using namespace vmath;
#include "Sphere.h"         // sphere related header file 

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600


// globel veriables
Display *gpDisplay = NULL; // display 1st veriable in xlib
XVisualInfo *visualInfo = NULL;
Window window;     // represent to window
Colormap colormap; // grapics card madhe color yeka certain arrange madhe mananeg kelele astat ani he hardware resource ahe

Bool bFullscreen = False;
Bool bActiveWindow = False;

// Variable Related logFile I/O
char gsLogFileName[] = "Log.txt";
FILE *gpFile = NULL;

// openGL related veriables
typedef GLXContext (*GLXCreateContextAttribsARBPROC)(Display *, GLXFBConfig, GLXContext, Bool, const int *);
GLXCreateContextAttribsARBPROC GLXCreateContextAttribsARB = NULL;
GLXFBConfig glxFBConfig;
GLXContext glxContext = NULL;

// Shader related global veriables
GLuint shaderProgramObject = 0;

enum
{
    AMC_ATTRIBUTE_VERTEX = 0,
    AMC_ATTRIBUTE_NORMAL,

};
GLuint gVao_sphere = 0;
GLuint vbo_sphere_position = 0;
GLuint gVbo_sphere_normal = 0;
GLuint gVbo_sphere_element = 0;

mat4 perspectiveProjectionMatrix;

GLuint modelMatrixUniform = 0;
GLuint viewMatrixUniform = 0;
GLuint projectionMatrixUniform = 0;

// Light Related veriable
GLuint laUniform = 0;
GLuint ldUniform = 0; // Light Diffuse
GLuint lsUniform = 0; // Light Diffuse

GLuint kaUniform;
GLuint kdUniform; // color of material
GLuint ksUniform;

GLfloat materialShininaseUniform;
GLuint lightPositionUniform = 0;
GLuint LKeyPressedUniform = 0;

GLfloat lightAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat lightSpecular[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition[] = {100.0f, 100.0f, 100.0f, 1.0f};
GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};

struct Material
{
	vec4 materialAmbient;
	vec4 materialDiffuse;
	vec4 materialSpecular;
	float materialShininnes;
};
struct Material material[24];

// Sphere Related Veriables
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_texture[764];
unsigned short sphere_elements[2280];

GLsizei gNumElements;
GLsizei gNumVertices;

Bool bLight = False;


int main(void)
{
    // function declarations
    void toggleFullscreen(void);
    int Initialize(void);
    void resize(int, int);
    void display(void);
    void update(void);
    void uninitialize(void);

    // variable declarations
    int defaultScreen;
    int defaultDepth;
    XSetWindowAttributes windowAttributes;
    Atom windowMangerDelateAtom;
    XEvent event;
    Screen *screen = NULL;
    int screenWidth, screenHeight;

    KeySym keySym;
    char keys[26]; // although we need 0 index coventionaly array size need Alphabetical a to z or A to Z

    GLXFBConfig *pGLXFBConfigs = NULL;
    GLXFBConfig bestGLXFBConfig;
    XVisualInfo *pXVisualInfo;
    int iNumFBConfigs;

    int framebufferAttributes[] =
        {GLX_X_RENDERABLE, True,
         GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
         GLX_RENDER_TYPE, GLX_RGBA_BIT,
         GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
         GLX_DOUBLEBUFFER, True,
         GLX_RED_SIZE, 8,
         GLX_GREEN_SIZE, 8,
         GLX_BLUE_SIZE, 8,
         GLX_ALPHA_SIZE, 8,
         GLX_DEPTH_SIZE, 24,
         GLX_STENCIL_SIZE, 8, None};

    Bool bDone = False;

    // code
    // Create Log File
    gpFile = fopen(gsLogFileName, "w");
    if (gpFile == NULL)
    {
        printf("Log File Creation Failed.\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(gpFile, "Step 1: Program Started Successfully.\n");
    }

    gpDisplay = XOpenDisplay(NULL); // client server shi connect karto null = display name
    if (gpDisplay == NULL)          // open the connection with xserver
    {
        fprintf(gpFile, "xOpen Display Failed to connet with server\n");
        uninitialize();
        exit(EXIT_FAILURE); // 0 for successful exit
    }
    fprintf(gpFile, "XOpenDisplay success\n");

    // create the diffualt screen object
    defaultScreen = XDefaultScreen(gpDisplay); // xserver and xclient cha madhyast cha number de
    // get diffualt depth
    defaultDepth = XDefaultDepth(gpDisplay, defaultScreen);

    pGLXFBConfigs = glXChooseFBConfig(gpDisplay, defaultScreen, framebufferAttributes, &iNumFBConfigs);
    if (pGLXFBConfigs == NULL)
    {
        fprintf(gpFile, "Step 2: pGLXFBConfigs failed\n");
        uninitialize();
        exit(EXIT_FAILURE);
    }
    fprintf(gpFile, "found number of FBConfigs: %d \n", iNumFBConfigs);

    int indexOfBestFBConfig = -1;
    int indexOfWorstFBConfig = -1;
    int bestNumberOfSamples = -1;
    int worstNumberOfSample = 999;

    for (int i = 0; i < iNumFBConfigs; i++)
    {
        pXVisualInfo = glXGetVisualFromFBConfig(gpDisplay, pGLXFBConfigs[i]);
        if (pXVisualInfo)
        {
            int sampleBuffers, samples;
            glXGetFBConfigAttrib(gpDisplay, pGLXFBConfigs[i], GLX_SAMPLE_BUFFERS, &sampleBuffers);

            glXGetFBConfigAttrib(gpDisplay, pGLXFBConfigs[i], GLX_SAMPLES, &samples);
            if (indexOfBestFBConfig < 0 || sampleBuffers && samples > bestNumberOfSamples)
            {
                indexOfBestFBConfig = i;
                bestNumberOfSamples = samples;
            }
            if (indexOfWorstFBConfig < 0 || !sampleBuffers || samples < worstNumberOfSample)
            {
                indexOfWorstFBConfig = i;
                worstNumberOfSample = samples;
            }
            XFree(pXVisualInfo);
        }
    }

    bestGLXFBConfig = pGLXFBConfigs[indexOfBestFBConfig];

    // set the globle fbconfig
    glxFBConfig = bestGLXFBConfig;
    XFree(pGLXFBConfigs);

    // choose fb config
    visualInfo = glXGetVisualFromFBConfig(gpDisplay, glxFBConfig);

    // set window Attributes(properties)
    memset((void *)&windowAttributes, 0, sizeof(XSetWindowAttributes));
    windowAttributes.border_pixel = 0;
    windowAttributes.background_pixmap = 0;
    windowAttributes.background_pixel = XBlackPixel(gpDisplay, visualInfo->screen);
    windowAttributes.colormap = XCreateColormap(gpDisplay,
                                                XRootWindow(gpDisplay, visualInfo->screen),
                                                visualInfo->visual,
                                                AllocNone);

    colormap = windowAttributes.colormap;
    windowAttributes.event_mask = KeyPressMask | ButtonPressMask | FocusChangeMask | StructureNotifyMask | ExposureMask; // PointerMotionMask is for mouse move // VisibilityChangeMask is for window hide

    // create window
    window = XCreateWindow(
        gpDisplay,
        XRootWindow(gpDisplay, visualInfo->screen),
        0, // x
        0, // y
        WIN_WIDTH,
        WIN_HEIGHT,
        0,
        visualInfo->depth,
        InputOutput,
        visualInfo->visual,
        CWBorderPixel | CWBackPixel | CWEventMask | CWColormap,
        &windowAttributes);

    if (!window)
    {
        printf("xWindow Create failed\n");
        uninitialize();
        exit(EXIT_FAILURE);
    }

    // Create Atom For Window Manger to distroy the window
    windowMangerDelateAtom = XInternAtom(gpDisplay, "WM_DELETE_WINDOW", True); // WM=WINDOW MANGER

    XSetWMProtocols(gpDisplay, window, &windowMangerDelateAtom, 1);

    // map the window to show it
    XMapWindow(gpDisplay, window);

    // set window title
    XStoreName(gpDisplay, window, "MSP:xWindow");

    // centering of window
    screen = XScreenOfDisplay(gpDisplay, visualInfo->screen);
    screenWidth = XWidthOfScreen(screen);
    screenHeight = XHeightOfScreen(screen);
    XMoveWindow(gpDisplay, window, screenWidth / 2 - WIN_WIDTH / 2, screenHeight / 2 - WIN_HEIGHT / 2);

    // initialize
    int iResult = Initialize();
    if (iResult == -1)
    {
        uninitialize();
        exit(EXIT_FAILURE);
    }
    else
    {
        fprintf(gpFile, "Successfully\n");
    }

    // GAME loop
    while (bDone == False)
    {
        while (XPending(gpDisplay)) // message queue madhe asanare massage returen karto ani 0 la stop hoto
        {
            XNextEvent(gpDisplay, &event);
            switch (event.type)
            {
            case MapNotify:
                break;

            case FocusIn:
                bActiveWindow = True;
                break;

            case FocusOut:
                bActiveWindow = False;
                break;

            case ConfigureNotify:
                resize(event.xconfigure.width, event.xconfigure.height);
                break;

            case KeyPress:
                keySym = XkbKeycodeToKeysym(gpDisplay, event.xkey.keycode, 0, 0); // user ne press keleli key la keycode madhe convert karto
                switch (keySym)                                                   // for escape
                {
                case XK_Escape:
                    bDone = True;
                    break;
                default:
                    break;
                }

                // for Alphabatic keypress
                XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
                switch (keys[0])
                {
                case 'f':
                case 'F':
                    if (bFullscreen == False)
                    {
                        toggleFullscreen();
                        bFullscreen = True;
                    }
                    else
                    {
                        toggleFullscreen();
                        bFullscreen = False;
                    }
                    break;
                case 'L':
                case 'l':
                    if (bLight == False)
                    {
                        bLight = True;
                    }
                    else
                    {
                        bLight = False;
                    }
                    break;                    
                default:
                    break;
                }
                break;

            case ButtonPress:
                break;

            case Expose:
                break;

            case 33: // in winodoes wm_destroy
                bDone = True;
                break;
            default:
                break;
            }
        }
        // Rendaring
        if (bActiveWindow == True)
        {
            display();
            update();
        }
    }

    uninitialize();
    return (0);
}

void toggleFullscreen(void)
{
    // code
    Atom windowMangerNormalStateAtom = XInternAtom(gpDisplay, "_NET_WM_STATE", False);
    Atom windowMangerFullscreenStateAtom = XInternAtom(gpDisplay, "_NET_WM_STATE_FULLSCREEN", False);

    XEvent event;
    memset((void *)&event, 0, sizeof(XEvent));
    event.type = ClientMessage;
    event.xclient.window = window;
    event.xclient.message_type = windowMangerNormalStateAtom;
    event.xclient.format = 32;
    event.xclient.data.l[0] = bFullscreen ? 0 : 1;
    event.xclient.data.l[1] = windowMangerFullscreenStateAtom;

    // set above event to x server
    XSendEvent(gpDisplay, XRootWindow(gpDisplay, visualInfo->screen), False, SubstructureNotifyMask, &event);
}

int Initialize(void)
{
    void printGLInfo(void);
    void resize(int width, int height);
    void uninitialize(void);

    GLenum glewResult;

    // code
    // get function pointer
    GLXCreateContextAttribsARB = (GLXCreateContextAttribsARBPROC)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");

    // declear atrtibs array
    GLint attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB,
        4,
        GLX_CONTEXT_MINOR_VERSION_ARB,
        5,
        GLX_CONTEXT_PROFILE_MASK_ARB, GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None};

    glxContext = GLXCreateContextAttribsARB(gpDisplay, glxFBConfig, 0, True, attribs); // 0 =shareble context for multiple screen, True=for hardware rendaring
    if (!glxContext)
    {
        GLXCreateContextAttribsARB = (GLXCreateContextAttribsARBPROC)glXGetProcAddressARB((GLubyte *)"glXCreateContextAttribsARB");

        GLint attribs[] = {
            GLX_CONTEXT_MAJOR_VERSION_ARB,
            1,
            GLX_CONTEXT_MINOR_VERSION_ARB,
            0,
            None};
        fprintf(gpFile, "can not get openGL4.5 context but recive lesser context\n");
    }
    else
    {
        fprintf(gpFile, "recived openGL 4.5 context successfully\n");
    }

    glXMakeCurrent(gpDisplay, window, glxContext);

    // initialize GLEW
    glewResult = glewInit();

    if (glewResult != GLEW_OK)
    {
        fprintf(gpFile, " glewInit func Failed\n");
        return (-1);
    }

    // print gl info
    //printGLInfo();

    
	// VERTEX SHADERS
	//  write the shader sorce code 2)create the shader object 3) give the shader soure code to shader object 4) copile the code 5) do error compiletion shader code
	const GLchar *vertexShaderSourceCode =
		"#version 460 core\n"
		"in vec4 aPosition;\n"
		"in vec3 aNormal;\n"
		"uniform mat4 uModelMatrix;\n"
		"uniform mat4 uViewMatrix;\n"
		"uniform mat4 uProjectionMatrix;\n"
		"out vec3 out_transformedNormals;\n"
		"out vec3 out_viwerVector;\n"
		"out vec3 out_lightDirection;\n"
		"uniform vec4 uLightPosition;\n"
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
		"out_lightDirection = vec3(uLightPosition - eyeCoordinate);\n"
		"out_viwerVector = -eyeCoordinate.xyz;\n"
		"}\n"
		"}\n"; // "" \""  \"" \ ""; hi string separet hot nahi karan apn tyat \ use kela ahe tya mule ti single string dharlya jate

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
		"in vec3 out_transformedNormals;\n"
		"in vec3 out_viwerVector;\n"
		"in vec3 out_lightDirection;\n"
		"out vec4 FragColor;\n"
		"uniform vec3 uLa;\n"
		"uniform vec3 uLd;\n"
		"uniform vec3 uLs;\n"
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
		"vec3 normalizedTransformedNormal =  normalize(out_transformedNormals);\n"
		"vec3 normalizedLightDirection = normalize(out_lightDirection);\n"
		"vec3 normalizedViwerVector = normalize(out_viwerVector);\n"
		"vec3 ambientLight = uLa * uKa;\n"
		"vec3 diffueLight = uLd * uKd * max(dot(normalizedLightDirection,normalizedTransformedNormal),0.0);\n"
		"vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTransformedNormal);\n"
		"vec3 specularLight = uLs * uKs * pow(max(dot(reflectionVector,normalizedViwerVector), 0.0),uMaterialShinines);\n"
		"phong_ads_Light = ambientLight + diffueLight + specularLight; \n"
		"}\n"
		"else\n"
		"{\n"
		"phong_ads_Light = vec3(1.0,1.0,1.0);\n"
		"}\n"
		"FragColor=vec4(phong_ads_Light,1.0);\n"
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
				fprintf(gpFile, "Shader Program Link Log: %s", szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize();
	}


	
	modelMatrixUniform = glGetUniformLocation(shaderProgramObject, "uModelMatrix");
	viewMatrixUniform = glGetUniformLocation(shaderProgramObject, "uViewMatrix");
	projectionMatrixUniform = glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
	laUniform = glGetUniformLocation(shaderProgramObject, "uLa");
	ldUniform = glGetUniformLocation(shaderProgramObject, "uLd");
	lsUniform = glGetUniformLocation(shaderProgramObject, "uLs");

	lightPositionUniform = glGetUniformLocation(shaderProgramObject, "uLightPosition");

	kaUniform = glGetUniformLocation(shaderProgramObject, "uKa");
	kdUniform = glGetUniformLocation(shaderProgramObject, "uKd");
	ksUniform = glGetUniformLocation(shaderProgramObject, "uKs");
	materialShininaseUniform = glGetUniformLocation(shaderProgramObject, "uMaterialShinines");
	LKeyPressedUniform = glGetUniformLocation(shaderProgramObject, "uLkeyPressed");

    
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
	glVertexAttribPointer(AMC_ATTRIBUTE_VERTEX, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_VERTEX);
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
    glClearDepth(1.0f);      // display madhe color buffer madhe gleyavr majya depth buffer madhe ja ani change all bits 1.0f
    glEnable(GL_DEPTH_TEST); // depth test on
    glDepthFunc(GL_LEQUAL);  // tyt tyt fragment la pass kr jyanchi value 1 peksh kami asel kiva equal asel

    // tell the openGL choose the clear color
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

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
   
	void sphereMaterial(void);
	void sphereUniform(void);

	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// use Shader program object
	glUseProgram(shaderProgramObject);

	// transformstions
	vmath::mat4 viewMatrix = vmath::mat4::identity();
	viewMatrix = vmath::translate(2.0f,-0.4f,-1.0f);

	// send this matrix to the vertex shader to the uniform
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, perspectiveProjectionMatrix);

	if (bLight == True)
	{
		glUniform3fv(laUniform, 1, lightAmbient);
		glUniform3fv(ldUniform, 1, lightDiffuse);
		glUniform3fv(lsUniform, 1, lightSpecular);
		glUniform4fv(lightPositionUniform, 1, lightPosition);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}

	// sphere material
	sphereMaterial();

	// bind with vao
	glBindVertexArray(gVao_sphere);
	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gVbo_sphere_element);

	// draw 24 sphere
	sphereUniform();

	// *** unbind vao ***
	glBindVertexArray(0);

	// unUse shader program
	glUseProgram(0);

    // swap the buffers
    glXSwapBuffers(gpDisplay, window);
}

void update(void)
{
    // code
}

void uninitialize(void)
{
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

    // free vbo of position
    if (vbo_sphere_position)
    {
        glDeleteBuffers(1, &vbo_sphere_position);
        vbo_sphere_position = 0;
    }

    // free gVao_sphere
    if (gVao_sphere) 
    {
        glDeleteVertexArrays(1, &gVao_sphere);
    }

    // detach, delete shader objects and delete shader program object
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
                    pShaders[i] = 0;
                }
            }
            free(pShaders);
            pShaders = NULL;
        }
        glUseProgram(0);
        glDeleteProgram(shaderProgramObject);
        shaderProgramObject = 0;
    }

    GLXContext currentContext = glXGetCurrentContext();
    if (currentContext && currentContext == glxContext)
    {
        glXMakeCurrent(gpDisplay, 0, 0);
    }

    if (glxContext)
    {
        glXDestroyContext(gpDisplay, glxContext);
        glxContext = NULL;
    }

    if (window)
    {
        XDestroyWindow(gpDisplay, window);
    }

    if (colormap) // by convetionly display globaly and it should be hardware resoure so unitialize
    {
        XFreeColormap(gpDisplay, colormap);
    }

    if (visualInfo)
    {
        free(visualInfo);
        visualInfo = NULL;
    }

    if (gpDisplay)
    {
        XCloseDisplay(gpDisplay);
        gpDisplay = NULL;
    }

    // Close the log file
    if (gpFile)
    {
        fprintf(gpFile, "Log File Successfully Close\n");
        fclose(gpFile);
        gpFile = NULL;
    }
}

void sphereMaterial(void)
{

	
	material[0].materialAmbient = vec4(0.0215f, 0.1745f, 0.0215f, 1.0f);
	material[0].materialDiffuse = vec4(0.07568f, 0.61424f, 0.07568f, 1.0f);
	material[0].materialSpecular = vec4(0.633, 0.727811, 0.633, 1.0f);
	material[0].materialShininnes = 0.6f * 128;

	material[1].materialAmbient = vec4(0.135, 0.2225, 0.1575, 1.0f);
	material[1].materialDiffuse = vec4(0.54, 0.89, 0.63, 1.0f);
	material[1].materialSpecular = vec4(0.316228, 0.316228, 0.316228, 1.0f);
	material[1].materialShininnes = 0.1f * 128;

	material[2].materialAmbient = vec4(0.05375, 0.05, 0.06625, 1.0f);
	material[2].materialDiffuse = vec4(0.18275, 0.17, 0.22525, 1.0f);
	material[2].materialSpecular = vec4(0.332741, 0.328634, 0.346435, 1.0f);
	material[2].materialShininnes = 0.3f * 128;

	material[3].materialAmbient = vec4(0.25, 0.20725, 0.20725, 1.0f);
	material[3].materialDiffuse = vec4(1.0, 0.829, 0.829, 1.0f);
	material[3].materialSpecular = vec4(0.296648, 0.296648, 0.296648, 1.0f);
	material[3].materialShininnes = 0.088f * 128;

	material[4].materialAmbient = vec4(0.1745, 0.01175, 0.01175, 1.0f);
	material[4].materialDiffuse = vec4(0.61424, 0.04136, 0.04136, 1.0f);
	material[4].materialSpecular = vec4(0.727811, 0.626959, 0.626959, 1.0f);
	material[4].materialShininnes = 0.6f * 128;

	material[5].materialAmbient = vec4(0.1, 0.18725, 0.1745, 1.0f);
	material[5].materialDiffuse = vec4(0.396, 0.74151, 0.69102, 1.0f);
	material[5].materialSpecular = vec4(0.297254, 0.30829, 0.306678, 1.0f);
	material[5].materialShininnes = 0.1f * 128;

	material[6].materialAmbient = vec4(0.329412, 0.223529, 0.027451, 1.0f);
	material[6].materialDiffuse = vec4(0.780392, 0.568627, 0.113725, 1.0f);
	material[6].materialSpecular = vec4(0.992157, 0.941176, 0.807843, 1.0f);
	material[6].materialShininnes = 0.21794872f * 128;

	material[7].materialAmbient = vec4(0.2125, 0.1275, 0.054, 1.0f);
	material[7].materialDiffuse = vec4(0.714, 0.4284, 0.18144, 1.0f);
	material[7].materialSpecular = vec4(0.393548, 0.271906, 0.166721, 1.0f);
	material[7].materialShininnes = 0.6f * 128;

	material[8].materialAmbient = vec4(0.25, 0.25, 0.25, 1.0f);
	material[8].materialDiffuse = vec4(0.4, 0.4, 0.4, 1.0f);
	material[8].materialSpecular = vec4(0.4, 0.4, 0.4, 1.0f);
	material[8].materialShininnes = 0.6f * 128;

	material[9].materialAmbient = vec4(0.19125, 0.0735, 0.0225, 1.0f);
	material[9].materialDiffuse = vec4(0.7038, 0.27048, 0.0828, 1.0f);
	material[9].materialSpecular = vec4(0.256777, 0.137622, 0.086014, 1.0f);
	material[9].materialShininnes = 0.1f * 128;

	material[10].materialAmbient = vec4(0.24725, 0.1995, 0.0745, 1.0f);
	material[10].materialDiffuse = vec4(0.75164, 0.60648, 0.22648, 1.0f);
	material[10].materialSpecular = vec4(0.628281, 0.555802, 0.366065, 1.0f);
	material[10].materialShininnes = 0.4f * 128;

	material[11].materialAmbient = vec4(0.19225, 0.19225, 0.19225, 1.0f);
	material[11].materialDiffuse = vec4(0.50754, 0.50754, 0.50754, 1.0f);
	material[11].materialSpecular = vec4(0.508273, 0.508273, 0.508273, 1.0f);
	material[11].materialShininnes = 0.4f * 128;

	material[12].materialAmbient = vec4(0.0, 0.0f, 0.0, 1.0f);
	material[12].materialDiffuse = vec4(0.01, 0.01, 0.01, 1.0f);
	material[12].materialSpecular = vec4(0.50, 0.50, 0.50, 1.0f);
	material[12].materialShininnes = 0.25f * 128;

	material[13].materialAmbient = vec4(0.0, 0.1, 0.06, 1.0f);
	material[13].materialDiffuse = vec4(0.0, 0.50980392, 0.50980392, 1.0f);
	material[13].materialSpecular = vec4(0.50196078, 0.50196078, 0.50196078, 1.0f);
	material[13].materialShininnes = 0.25f * 128;

	material[14].materialAmbient = vec4(0.0, 0.0, 0.0, 1.0f);
	material[14].materialDiffuse = vec4(0.01, 0.35, 0.01, 1.0f);
	material[14].materialSpecular = vec4(0.45, 0.55, 0.45, 1.0f);
	material[14].materialShininnes = 0.25f * 128;

	material[15].materialAmbient = vec4(0.0, 0.0, 0.0, 1.0f);
	material[15].materialDiffuse = vec4(0.5, 0.0, 0.0, 1.0f);
	material[15].materialSpecular = vec4(0.7, 0.6, 0.6, 1.0f);
	material[15].materialShininnes = 0.25f * 128;

	material[16].materialAmbient = vec4(0.0, 0.0, 0.0, 1.0f);
	material[16].materialDiffuse = vec4(0.55, 0.55, 0.55, 1.0f);
	material[16].materialSpecular = vec4(0.70, 0.70, 0.70, 1.0f);
	material[16].materialShininnes = 0.25f * 128;

	material[17].materialAmbient = vec4(0.0, 0.0, 0.0, 1.0f);
	material[17].materialDiffuse = vec4(0.5, 0.5, 0.0, 1.0f);
	material[17].materialSpecular = vec4(0.60, 0.60, 0.50, 1.0f);
	material[17].materialShininnes = 0.25f * 128;

	material[18].materialAmbient = vec4(0.02, 0.02, 0.02, 1.0f);
	material[18].materialDiffuse = vec4(0.01, 0.01, 0.01, 1.0f);
	material[18].materialSpecular = vec4(0.4, 0.4, 0.4, 1.0f);
	material[18].materialShininnes = 0.078125f * 128;

	material[19].materialAmbient = vec4(0.0, 0.05, 0.05, 1.0f);
	material[19].materialDiffuse = vec4(0.4, 0.5, 0.5, 1.0f);
	material[19].materialSpecular = vec4(0.04, 0.7, 0.7, 1.0f);
	material[19].materialShininnes = 0.078125f * 128;

	material[20].materialAmbient = vec4(0.0, 0.05, 0.0, 1.0f);
	material[20].materialDiffuse = vec4(0.4, 0.5, 0.4, 1.0f);
	material[20].materialSpecular = vec4(0.04, 0.7, 0.04, 1.0f);
	material[20].materialShininnes = 0.078125f * 128;

	material[21].materialAmbient = vec4(0.05, 0.0, 0.0, 1.0f);
	material[21].materialDiffuse = vec4(0.5, 0.4, 0.4, 1.0f);
	material[21].materialSpecular = vec4(0.7, 0.04, 0.04, 1.0f);
	material[21].materialShininnes = 0.078125f * 128;

	material[22].materialAmbient = vec4(0.05, 0.05, 0.05, 1.0f);
	material[22].materialDiffuse = vec4(0.5, 0.5, 0.5, 1.0f);
	material[22].materialSpecular = vec4(0.7, 0.7, 0.7, 1.0f);
	material[22].materialShininnes = 0.078125f * 128;

	material[23].materialAmbient = vec4(0.05, 0.05, 0.0, 1.0f);
	material[23].materialDiffuse = vec4(0.5, 0.5, 0.4, 1.0f);
	material[23].materialSpecular = vec4(0.7, 0.7, 0.04, 1.0f);
	material[23].materialShininnes = 0.078125f * 128;

	material[24].materialAmbient = vec4(0.0215f, 0.1745f, 0.0215f, 1.0f);
	material[24].materialDiffuse = vec4(0.07568f, 0.61424f, 0.07568f, 1.0f);
	material[24].materialSpecular = vec4(0.633, 0.727811, 0.633, 1.0f);
	material[24].materialShininnes = 0.078125f * 128;
}

void sphereUniform(void)
{
	
	// 1
	vmath::mat4 modelMatrix = vmath::mat4::identity();
    vmath::mat4 scaleMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-5.5f, 3.5f,-6.0f) ;
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[0].materialAmbient);
		glUniform3fv(kdUniform, 1, material[0].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[0].materialSpecular);
		glUniform1f(materialShininaseUniform, material[0].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// 2
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-5.5f,2.30f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kdUniform, 1, material[1].materialDiffuse);
		glUniform3fv(kaUniform, 1, material[1].materialAmbient);
		glUniform3fv(ksUniform, 1, material[1].materialSpecular);
		glUniform1f(materialShininaseUniform, material[1].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	// 3
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-5.5f, 1.10f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[2].materialAmbient);
		glUniform3fv(kdUniform, 1, material[2].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[2].materialSpecular);
		glUniform1f(materialShininaseUniform, material[2].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//4
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-5.50f, -0.1f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[3].materialAmbient);
		glUniform3fv(kdUniform, 1, material[3].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[3].materialSpecular);
		glUniform1f(materialShininaseUniform, material[3].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//5
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-5.50f, -1.3f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[4].materialAmbient);
		glUniform3fv(kdUniform, 1, material[4].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[4].materialSpecular);
		glUniform1f(materialShininaseUniform, material[4].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//6
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-5.50f, -2.5f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[5].materialAmbient);
		glUniform3fv(kdUniform, 1, material[5].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[5].materialSpecular);
		glUniform1f(materialShininaseUniform, material[5].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
////////////////////////////////////////////////////////////////////////////////
	
//7
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-3.5f,3.5f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[6].materialAmbient);
		glUniform3fv(kdUniform, 1, material[6].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[6].materialSpecular);
		glUniform1f(materialShininaseUniform, material[6].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//8
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-3.5f, 2.3f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[7].materialAmbient);
		glUniform3fv(kdUniform, 1, material[7].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[7].materialSpecular);
		glUniform1f(materialShininaseUniform, material[7].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//9
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-3.50f, 1.1f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[8].materialAmbient);
		glUniform3fv(kdUniform, 1, material[8].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[8].materialSpecular);
		glUniform1f(materialShininaseUniform, material[8].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//10
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-3.50f, -0.10f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[9].materialAmbient);
		glUniform3fv(kdUniform, 1, material[9].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[9].materialSpecular);
		glUniform1f(materialShininaseUniform, material[9].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//11
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-3.50f, -1.30f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[10].materialAmbient);
		glUniform3fv(kdUniform, 1, material[10].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[10].materialSpecular);
		glUniform1f(materialShininaseUniform, material[10].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//12
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-3.50f, -2.50f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[11].materialAmbient);
		glUniform3fv(kdUniform, 1, material[11].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[11].materialSpecular);
		glUniform1f(materialShininaseUniform, material[11].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

////////////////////////////////////////////////////////////////////////////////

	//13
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-1.5f, 3.5f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[12].materialAmbient);
		glUniform3fv(kdUniform, 1, material[12].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[12].materialSpecular);
		glUniform1f(materialShininaseUniform, material[12].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//14
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-1.5f, 2.30f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[13].materialAmbient);
		glUniform3fv(kdUniform, 1, material[13].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[13].materialSpecular);
		glUniform1f(materialShininaseUniform, material[13].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//15
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-1.50f, 1.10f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[14].materialAmbient);
		glUniform3fv(kdUniform, 1, material[14].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[14].materialSpecular);
		glUniform1f(materialShininaseUniform, material[14].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//16
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-1.5f, -0.10f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[15].materialAmbient);
		glUniform3fv(kdUniform, 1, material[15].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[15].materialSpecular);
		glUniform1f(materialShininaseUniform, material[15].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//17
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-1.5f, -1.30f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[16].materialAmbient);
		glUniform3fv(kdUniform, 1, material[16].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[16].materialSpecular);
		glUniform1f(materialShininaseUniform, material[16].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//18
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(-1.5f, -2.50f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[17].materialAmbient);
		glUniform3fv(kdUniform, 1, material[17].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[17].materialSpecular);
		glUniform1f(materialShininaseUniform, material[17].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	////////////////////////////////////////////////////////////////////////////////

	//19
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(0.5f, 3.5f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[18].materialAmbient);
		glUniform3fv(kdUniform, 1, material[18].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[18].materialSpecular);
		glUniform1f(materialShininaseUniform, material[18].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//20
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(0.50f, 2.30f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[19].materialAmbient);
		glUniform3fv(kdUniform, 1, material[19].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[19].materialSpecular);
		glUniform1f(materialShininaseUniform, material[19].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//21
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(0.50f, 1.1f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[20].materialAmbient);
		glUniform3fv(kdUniform, 1, material[20].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[20].materialSpecular);
		glUniform1f(materialShininaseUniform, material[20].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//22
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(0.50f, -0.10f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[21].materialAmbient);
		glUniform3fv(kdUniform, 1, material[21].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[21].materialSpecular);
		glUniform1f(materialShininaseUniform, material[21].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//23
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(0.50f, -1.30f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[22].materialAmbient);
		glUniform3fv(kdUniform, 1, material[22].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[22].materialSpecular);
		glUniform1f(materialShininaseUniform, material[22].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);

	//24
	modelMatrix = vmath::mat4::identity();
	modelMatrix = vmath::translate(0.50f, -2.5f,-6.0f);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
	if (bLight == True)
	{
		glUniform3fv(kaUniform, 1, material[23].materialAmbient);
		glUniform3fv(kdUniform, 1, material[23].materialDiffuse);
		glUniform3fv(ksUniform, 1, material[23].materialSpecular);
		glUniform1f(materialShininaseUniform, material[23].materialShininnes);
		glUniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform, 0);
	}
	glDrawElements(GL_TRIANGLES, gNumElements, GL_UNSIGNED_SHORT, 0);
}
