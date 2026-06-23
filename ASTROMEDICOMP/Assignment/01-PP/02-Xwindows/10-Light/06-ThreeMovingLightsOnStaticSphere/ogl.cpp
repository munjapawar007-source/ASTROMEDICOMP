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

float anglePos1 = 0.0f;
float anglePos2 = 0.0f;
float anglePos3 = 0.0f;

// Roatation
GLfloat angleSphere_MP = 0.0f;

Bool bLight = False;

Bool perVertex = True;	 // display vertex light
Bool perFragment = True; // display fargment light

// Sphere Related Veriables
float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_texture[764];
unsigned short sphere_elements[2280];

GLsizei gNumElements;
GLsizei gNumVertices;

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
                default:
                    break;
                }

                // for Alphabatic keypress
                XLookupString(&event.xkey, keys, sizeof(keys), NULL, NULL);
                switch (keys[0])
                {
                case 'f':
                case 'F':
                   if (perFragment == True)
                    {
                        perFragment = False;
                        perVertex = True;
                    }
                    else
                    {
                        perFragment = True;
                        perVertex = False;
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
                case 'Q':
		        case 'q':
                    bDone = True;
			        break;  
                case 'V':
                case 'v':
                    if (perVertex == True)
                    {
                        perFragment = True;
                        perVertex = False;
                    }
                    else
                    {
                        perFragment = False;
                        perVertex = True;
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
    void initializePerVertexShader(void);
	void initializePerFragmentShader(void);

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

	// initialize of three light
	light[0].ambient = vec4(0.0, 0.0, 0.0, 1.0);
	light[0].diffuse = vec4(1.0, 0.0, 0.0, 1.0);
	light[0].specular = vec4(1.0, 0.0, 0.0, 1.0);

	light[1].ambient = vec4(0.0f, 0.0, 0.0, 1.0);
	light[1].diffuse = vec4(0.0f, 1.0, 0.0, 1.0);
	light[1].specular = vec4(0.0f, 1.0, 0.0, 1.0);

	light[2].ambient = vec4(0.0f, 0.0, 0.0, 1.0); 
	light[2].diffuse = vec4(0.0f, 0.0, 1.0, 1.0);
	light[2].specular = vec4(0.0f, 0.0, 1.0, 1.0);
	
    // depth related code
    glClearDepth(1.0f);      // display madhe color buffer madhe gleyavr majya depth buffer madhe ja ani change all bits 1.0f
    glEnable(GL_DEPTH_TEST); // depth test on
    glDepthFunc(GL_LEQUAL);  // tyt tyt fragment la pass kr jyanchi value 1 peksh kami asel kiva equal asel

    // tell the openGL choose the clear color
    glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

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
	if (perVertex == False && perFragment == True)
	{
		glUseProgram(shaderProgramObject_PV);
	}
	else if (perFragment == False && perVertex == True)
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

	if (bLight == True)
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

	if (bLight == True)
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
    glXSwapBuffers(gpDisplay, window);
}

void update(void)
{
    // code
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
				fprintf(gpFile, "Shader Program Link Log: %s", szInfoLog);
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
				fprintf(gpFile, "Shader Program Link Log: %s", szInfoLog);
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
