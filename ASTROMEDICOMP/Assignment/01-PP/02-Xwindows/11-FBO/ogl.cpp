// stander heder files
#include <stdio.h>  //printf
#include <stdlib.h> //for exit
#include <memory.h> // for memset
#include <math.h>
#include <time.h>
#define _USE_MATH_DEFINES 1

// xlib hederfiles
#include <X11/Xlib.h>   //for xlib api
#include <X11/Xutil.h>  //for visule info and related api
#include <X11/XKBlib.h> //for keyboard Related API

// OpenGl Related Header files
#include <GL/glew.h>
//<GL/glew.h> headr file must be include before gl/gl.h
#include <GL/gl.h>
#include <GL/glx.h> //for bridging api
#include <SOIL/SOIL.h>
#include "vmath.h"
#include "Sphere.h"
using namespace vmath;

// Macros
#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// fbo
#define FBO_WIDTH 512
#define FBO_HEIGHT 512


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
	AMC_ATTRIBUTE_POSITION = 0,
	AMC_ATTRIBUTE_COLOR,
	AMC_ATTRIBUTE_TEXCORD,
	AMC_ATTRIBUTE_VERTEX,
	AMC_ATTRIBUTE_NORMAL,

};

GLuint vao_cube = 0;
GLuint vbo_position_cube = 0;
GLuint vbo_TexCoord_cube = 0;

GLuint mvpMatrixUniform = 0; // mvp model view position
mat4 perspectiveProjectionMatrix;

// Roatation
GLfloat angleCube_MP = 0.0f;

// For Texture
GLuint textureSamplerUniform = 0;

// Fbo related global veriables
int winWidth;
int winHeight;

GLuint fbo;
GLuint rbo;
GLuint fbo_Texture;
int fboResult = -1;

// Sphere Related Veriables
mat4 perspectiveProjectionMatrix_sphere;
GLuint shaderProgramObject_sphere = 0;

GLuint gVao_sphere = 0;
GLuint vbo_sphere_position = 0;
GLuint gVbo_sphere_normal = 0;
GLuint gVbo_sphere_element = 0;
GLsizei gNumElements;
GLsizei gNumVertices;

GLuint mvpMatrixUniform_sphere = 0; // mvp model view position

GLuint modelMatrixUniform_sphere = 0;
GLuint viewMatrixUniform_sphere = 0;
GLuint projectionMatrixUniform_sphere = 0;

// Light Related veriable
GLuint laUniform_sphere = 0;
GLuint ldUniform_sphere = 0; 
GLuint lsUniform_sphere = 0; 

GLuint kaUniform_sphere = 0;
GLuint kdUniform_sphere = 0; // color of material
GLuint ksUniform_sphere = 0;

GLuint materialShininaseUniform_sphere = 0;
GLuint lightPositionUniform_sphere = 0;
GLuint LKeyPressedUniform_sphere = 0;

GLfloat lightAmbient_sphere[] = {0.10f, 0.10f, 0.10f, 1.0f};
GLfloat lightSpecular_sphere[] = {1.0f, 1.0f, 1.0f, 1.0f};
GLfloat lightPosition_sphere[] = {100.0f, 100.0f, 100.0f, 1.0f};
GLfloat lightDiffuse_sphere[] = {1.0f, 1.0f, 1.0f, 1.0f};

GLfloat materialAmbient_sphere[] = {0.0f, 0.0f, 0.0f, 1.0f};
GLfloat materialSpecular_sphere[] = {0.70f, 0.70f, 0.70f, 1.0f};
GLfloat materialShininnes_sphere = 128.0f;
GLfloat materialDiffuse_sphere[] = {0.50f, 0.20f, 0.70f, 1.0f};

Bool bLight_sphere = False;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_texture[764];
unsigned short sphere_elements[2280];


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
                if (bLight_sphere == False)
                {
                    bLight_sphere = True;
                }
                else
                {
                    bLight_sphere = False;
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
    Bool createAndPrepareFBOForDrawing(GLint, GLint);
	int initialize_Sphere(void);

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
        "in vec2 aTexCoord;\n" \
		"out vec2 out_TexCord;\n" \
        "uniform mat4 uMVPMatrix;\n"
        "void main(void)\n"
        "{gl_Position=uMVPMatrix*aPosition;\n"
        "out_TexCord = aTexCoord;\n"
        "}\n";
    // "" \""  \"" \ ""; hi string separet hot nahi karan apn tyat \ use kela ahe tya mule ti single string dharlya jate

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
                fprintf(gpFile, "vertex Shader Compilation log = %s\n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // FRAGMENT SHADER
    const GLchar *fargamentShaderSourceCode =
        "#version 460 core\n"
        "in vec2 out_TexCord;\n" \
        "uniform sampler2D uTextureSampler;\n"
		"out vec4 FragColor;\n" \
        "void main(void)\n" \
		"{FragColor = texture(uTextureSampler,out_TexCord);\n" \
        "}\n"; \

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
                fprintf(gpFile, "FRAGMENT Shader Compilation log = %s\n", szInfoLog);
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
    glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCORD, "aTexCoord");

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
                fprintf(gpFile, "ShaderProgram Link Log: %s\n", szInfoLog);
                free(szInfoLog);
                szInfoLog = NULL;
            }
        }
        uninitialize();
    }

    // get the required uniform loction from the sheder
    mvpMatrixUniform = glGetUniformLocation(shaderProgramObject, "uMVPMatrix"); // openGL mala de shader program badal ch location (shaderprogra deil uniform )
    textureSamplerUniform = glGetUniformLocation(shaderProgramObject, "uTextureSampler");

   // provide vertex position,color,texcode, and normal etc.
	const GLfloat cube_position[] =
		{
			// front
			1.0f, 1.0f, 1.0f,	// top-right of front
			-1.0f, 1.0f, 1.0f,	// top-left of front
			-1.0f, -1.0f, 1.0f, // bottom-left of front
			1.0f, -1.0f, 1.0f,	// bottom-right of front

			// right
			1.0f, 1.0f, -1.0f,	// top-right of right
			1.0f, 1.0f, 1.0f,	// top-left of right
			1.0f, -1.0f, 1.0f,	// bottom-left of right
			1.0f, -1.0f, -1.0f, // bottom-right of right

			// back
			1.0f, 1.0f, -1.0f,	 // top-right of back
			-1.0f, 1.0f, -1.0f,	 // top-left of back
			-1.0f, -1.0f, -1.0f, // bottom-left of back
			1.0f, -1.0f, -1.0f,	 // bottom-right of back

			// left
			-1.0f, 1.0f, 1.0f,	 // top-right of left
			-1.0f, 1.0f, -1.0f,	 // top-left of left
			-1.0f, -1.0f, -1.0f, // bottom-left of left
			-1.0f, -1.0f, 1.0f,	 // bottom-right of left

			// top
			1.0f, 1.0f, -1.0f,	// top-right of top
			-1.0f, 1.0f, -1.0f, // top-left of top
			-1.0f, 1.0f, 1.0f,	// bottom-left of top
			1.0f, 1.0f, 1.0f,	// bottom-right of top

			// bottom
			1.0f, -1.0f, 1.0f,	 // top-right of bottom
			-1.0f, -1.0f, 1.0f,	 // top-left of bottom
			-1.0f, -1.0f, -1.0f, // bottom-left of bottom
			1.0f, -1.0f, -1.0f,	 // bottom-right of bottom
		};

	const GLfloat cube_Texcoords[] =
		{
			// front
			1.0f, 1.0f, // top-right of front
			0.0f, 1.0f, // top-left of front
			0.0f, 0.0f, // bottom-left of front
			1.0f, 0.0f, // bottom-right of front

			// right
			1.0f, 1.0f, // top-right of right
			0.0f, 1.0f, // top-left of right
			0.0f, 0.0f, // bottom-left of right
			1.0f, 0.0f, // bottom-right of right

			// back
			1.0f, 1.0f, // top-right of back
			0.0f, 1.0f, // top-left of back
			0.0f, 0.0f, // bottom-left of back
			1.0f, 0.0f, // bottom-right of back

			// left
			1.0f, 1.0f, // top-right of left
			0.0f, 1.0f, // top-left of left
			0.0f, 0.0f, // bottom-left of left
			1.0f, 0.0f, // bottom-right of left

			// top
			1.0f, 1.0f, // top-right of top
			0.0f, 1.0f, // top-left of top
			0.0f, 0.0f, // bottom-left of top
			1.0f, 0.0f, // bottom-right of top

			// bottom
			1.0f, 1.0f, // top-right of bottom
			0.0f, 1.0f, // top-left of bottom
			0.0f, 0.0f, // bottom-left of bottom
			1.0f, 0.0f, // bottom-right of bottom
		};

	// CUBE
	glGenVertexArrays(1, &vao_cube);
	glBindVertexArray(vao_cube);

	// Position
	glGenBuffers(1, &vbo_position_cube); // all arrays associate with vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_position_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_position), cube_position, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// texcoord
	glGenBuffers(1, &vbo_TexCoord_cube); // all arrays associate with vao
	glBindBuffer(GL_ARRAY_BUFFER, vbo_TexCoord_cube);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_Texcoords), cube_Texcoords, GL_STATIC_DRAW);
	glVertexAttribPointer(AMC_ATTRIBUTE_TEXCORD, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCORD);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

    // Enable Texturing
	glEnable(GL_TEXTURE_2D);

    // depth related code
    glClearDepth(1.0f);      // display madhe color buffer madhe gleyavr majya depth buffer madhe ja ani change all bits 1.0f
    glEnable(GL_DEPTH_TEST); // depth test on
    glDepthFunc(GL_LEQUAL);  // tyt tyt fragment la pass kr jyanchi value 1 peksh kami asel kiva equal asel

    // tell the openGL choose the clear color
    glClearColor(0.2f, 0.3f, 0.2f, 1.0f);

    perspectiveProjectionMatrix = mat4::identity(); // this is annalogas to load identity (resize madhil) in ffp

    // warm up resize
    resize(WIN_WIDTH, WIN_HEIGHT);

    // create fbo and if succssefull call sphere
	if (createAndPrepareFBOForDrawing(FBO_WIDTH,FBO_HEIGHT) == True)
	{
		fprintf(gpFile, "FBO creation successfull\n");
		fboResult = initialize_Sphere();

		if (fboResult != 0)
		{
			fprintf(gpFile, "fbo failed\n");
			return (-2);
		}
		else
		{
			fprintf(gpFile, "initialize_Sphere SUCCESSFULL\n");
		}
	}
	else
	{
		fprintf(gpFile, "fbo creation failed\n");
		return (-3);
	}


    return (0);
}


Bool createAndPrepareFBOForDrawing(GLint textureWidth, GLint textureHeight)
{
	//veriable declaration

	//code
	//step1:check whethere texture width and height are compatiable
	GLint maxRenderBufferSize;
	glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,&maxRenderBufferSize);
	if(maxRenderBufferSize <= textureWidth || maxRenderBufferSize <= textureHeight )
	{
		fprintf(gpFile, "fbo widht height are exceeding maxRenderBufferSize\n");
		return(False);
	}

	//step2: create new farme buffer for sphere
	glGenFramebuffers(1,&fbo);

	//step3:bind with newly created buffer
	glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	
	//step4:create render buffer as place holder for our depth and our frame buffer
	glGenRenderbuffers(1,&rbo);

	//step5: bind this new crated render buffer
	glBindRenderbuffer(GL_RENDERBUFFER,rbo);

	//step6: give specific storeage to this render buffer so depth needed for sphere will reside here
	glRenderbufferStorage(GL_RENDERBUFFER,GL_DEPTH_COMPONENT16,textureWidth,textureHeight); // 16 bit it is recommanded as depth for mobile

	//step7: create an empty texture but fully compactabale texture object for our incoming object
	glGenTextures(1,&fbo_Texture);
	glBindTexture(GL_TEXTURE_2D,fbo_Texture);

	//step8: to create full fitured textured give appropreate parameter
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

	//stpe9: now create this texture for our fbo
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,textureWidth,textureHeight,0,GL_RGB, GL_UNSIGNED_BYTE,NULL); //5_6_5 for human unstood greenary


	//step11: give this empty texture to frame buffer
	glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D,fbo_Texture,0);

	//step12: attach priviously created famebuffer to our frame buffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,rbo);

	//step13: check whethere our frame buffer complete or not
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER)!= GL_FRAMEBUFFER_COMPLETE)
	{
		fprintf(gpFile,"frame buffer NOT complete\n");
		return(False);
	}

	//step14:unbind
	glBindFramebuffer(GL_FRAMEBUFFER,0);
	return (True);

}	


int initialize_Sphere(void)
{
	// Function Declaration
	void resize_Sphere(int, int);
	void uninitialize_sphere(void);

	// VERTEX SHADERS
	//  write the shader sorce code
	// 2)create the shader object 
	// 3) give the shader soure code to shader object 
	// 4) copile the code 
	// 5) do error compiletion shader code

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
		uninitialize_sphere();
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
		uninitialize_sphere();
	}

	// Create , Attach , link shader program object
	shaderProgramObject_sphere = glCreateProgram();
	glAttachShader(shaderProgramObject_sphere, vertexShaderObject);
	glAttachShader(shaderProgramObject_sphere, fragmentShaderObject);

	// Bind Shader Attribute at certin index in shader to same index in host program
	glBindAttribLocation(shaderProgramObject_sphere, AMC_ATTRIBUTE_VERTEX, "aPosition"); // 0th index match cpu and gpu side
	glBindAttribLocation(shaderProgramObject_sphere, AMC_ATTRIBUTE_NORMAL, "aNormal");

	glLinkProgram(shaderProgramObject_sphere);
	status = 0;
	infoLogLenght = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject_sphere, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_sphere, GL_INFO_LOG_LENGTH, &infoLogLenght);

		if (infoLogLenght > 0)
		{
			szInfoLog = (GLchar *)malloc(infoLogLenght * sizeof(GLchar));

			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject_sphere, infoLogLenght, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program Link Log: %s", szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialize_sphere();
	}

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

	modelMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uModelMatrix");
	viewMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uViewMatrix");
	projectionMatrixUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uProjectionMatrix");
	laUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uLa");
	ldUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uLd");
	lsUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uLs");

	lightPositionUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uLightPosition");

	kaUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uKa");
	kdUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uKd");
	ksUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uKs");
	materialShininaseUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uMaterialShinines");

	LKeyPressedUniform_sphere = glGetUniformLocation(shaderProgramObject_sphere, "uLkeyPressed");

	// depth related code
	glClearDepth(1.0f);		 // display madhe color buffer madhe gleyavr majya depth buffer madhe ja ani change all bits 1.0f
	glEnable(GL_DEPTH_TEST); // depth test on
	glDepthFunc(GL_LEQUAL);	 // tyt tyt fragment la pass kr jyanchi value 1 peksh kami asel kiva equal asel

	// from here on word OPENGL code starts
	//  tell the openGL choose the clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	perspectiveProjectionMatrix_sphere = mat4::identity(); // this is annalogas to load identity (resize madhil) in ffp

	// warm up resize
	resize_Sphere(FBO_WIDTH, FBO_HEIGHT);

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

    winWidth = width;
	winHeight = height;

    // set the viewPort
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}


void resize_Sphere(int width, int height)
{
    // code
    // if height by default 0 makes1
    if (height <= 0)
    {
        height = 1;
    }
    
    winWidth = width;
	winHeight = height;

    // set the viewPort
    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    perspectiveProjectionMatrix_sphere = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
}



void display(void)
{
    void resize(int,int);
	void display_sphere(void);

	//call sphere related code
	if(fboResult == 0) //means initialize_sphere is successful
	{
		display_sphere();
	
	}

	//call cube resize to compatisate sphere's resize
	resize(winWidth,winHeight);

	//clear color again to compensate the effect clear color of sphere
	glClearColor(1.0f,1.0f,1.0f,1.0f);


    // code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// use Shader program object
	glUseProgram(shaderProgramObject);

	//**************************
	// transformstions
	vmath::mat4 modelViewMatrix = vmath::mat4::identity();
	vmath::mat4 translationMatrix = vmath::mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -6.0f);
	vmath::mat4 scaleMatrix = vmath::mat4::identity();
	scaleMatrix = vmath::scale(0.75f, 0.75f, 0.75f);

	vmath::mat4 rotationMatrixX = vmath::mat4::identity();
	rotationMatrixX = vmath::rotate(angleCube_MP, 1.0f, 0.0f, 0.0f);
	vmath::mat4 rotationMatrixY = vmath::mat4::identity();
	rotationMatrixY = vmath::rotate(angleCube_MP, 0.0f, 1.0f, 0.0f);
	vmath::mat4 rotationMatrixZ = vmath::mat4::identity();
	rotationMatrixZ = vmath::rotate(angleCube_MP, 0.0f, 0.0f, 1.0f);
	vmath::mat4 rotationMatrix = vmath::mat4::identity();
	rotationMatrix = rotationMatrixX * rotationMatrixY * rotationMatrixZ;

	modelViewMatrix = translationMatrix * scaleMatrix * rotationMatrix; // order is important
	vmath::mat4 ModelViewProjectionMatrix = vmath::mat4::identity();
	ModelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order is important

	// send this matrix to the vertex shader to the uniform
	glUniformMatrix4fv(mvpMatrixUniform, 1, GL_FALSE, ModelViewProjectionMatrix);

	//for texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,fbo_Texture);
	glUniform1i(textureSamplerUniform,0);

	glBindVertexArray(vao_cube);
	// draw vertex array
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
	glDrawArrays(GL_TRIANGLE_FAN, 20, 4);

	glBindTexture(GL_TEXTURE_2D,0);
	glBindVertexArray(0);


    // unUse shader program
    glUseProgram(0);

    // swap the buffers
    glXSwapBuffers(gpDisplay, window);
}


void display_sphere(void)
{
	void resize_Sphere(int, int);

	//do fbo related changes
	if(fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,fbo);
	}

	//call sphere resize to compasate to cube resize
	resize_Sphere(FBO_WIDTH,FBO_HEIGHT);

	//call sphere clear color to compasate to cube clear color
	glClearColor(0.0,0.0,0.0,1.0);


	// code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// use Shader program object
	glUseProgram(shaderProgramObject_sphere);

	// transformstions
	vmath::mat4 modelMatrix = vmath::mat4::identity();
	vmath::mat4 viewMatrix = vmath::mat4::identity();
	vmath::mat4 translationMatrix = vmath::mat4::identity();
	translationMatrix = vmath::translate(0.0f, 0.0f, -3.0f);
	modelMatrix = translationMatrix;

	// send this matrix to the vertex shader to the uniform
	glUniformMatrix4fv(modelMatrixUniform_sphere, 1, GL_FALSE, modelMatrix);
	glUniformMatrix4fv(viewMatrixUniform_sphere, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(projectionMatrixUniform_sphere, 1, GL_FALSE, perspectiveProjectionMatrix_sphere);

	if (bLight_sphere == True)
	{
		glUniform3fv(laUniform_sphere, 1, lightAmbient_sphere);
		glUniform3fv(ldUniform_sphere, 1, lightDiffuse_sphere);
		glUniform3fv(lsUniform_sphere, 1, lightSpecular_sphere);

		glUniform4fv(lightPositionUniform_sphere, 1, lightPosition_sphere);

		glUniform3fv(kaUniform_sphere, 1, materialAmbient_sphere);
		glUniform3fv(kdUniform_sphere, 1, materialDiffuse_sphere);
		glUniform3fv(ksUniform_sphere, 1, materialSpecular_sphere);

		glUniform1f(materialShininaseUniform_sphere, materialShininnes_sphere);

		glUniform1i(LKeyPressedUniform_sphere, 1);
	}
	else
	{
		glUniform1i(LKeyPressedUniform_sphere, 0);
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

	if(fbo)
	{
		glBindFramebuffer(GL_FRAMEBUFFER,0);
	}
	
}


void update(void)
{
    // code
    angleCube_MP = angleCube_MP - 0.05f;

	if (angleCube_MP <= 0.0f)
	{
		angleCube_MP = angleCube_MP + 360.0f;
	}
}

void uninitialize(void)
{
   // Function Declarations
	void uninitialize_sphere(void);
	void toggleFullScreen(void);
    
    if(rbo)
	{
		glDeleteRenderbuffers(1,&rbo);
		rbo = 0;
	}

	if(fbo)
	{
		glDeleteFramebuffers(1,&fbo);
		fbo=0;
	}

	if(fboResult == 0) //sphere initialization successful
	{
		uninitialize_sphere();
	}


	if (fbo_Texture)
	{
		glDeleteTextures(1, &fbo_Texture);
		fbo_Texture = 0;
	}

	// free vbo postion
	if (vbo_TexCoord_cube)
	{
		glDeleteBuffers(1, &vbo_TexCoord_cube);
		vbo_TexCoord_cube = 0;
	}

	// free vbo postion
	if (vbo_position_cube)
	{
		glDeleteBuffers(1, &vbo_position_cube);
		vbo_position_cube = 0;
	}

	// free vao
	if (vao_cube)
	{
		glDeleteVertexArrays(1, &vao_cube);
		vao_cube = 0;
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


void uninitialize_sphere(void)
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
	if (shaderProgramObject_sphere)
	{
		glUseProgram(shaderProgramObject_sphere);
		GLint numShaders;
		glGetProgramiv(shaderProgramObject_sphere, GL_ATTACHED_SHADERS, &numShaders);
		if (numShaders > 0)
		{
			GLuint *pShaders = (GLuint *)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject_sphere, numShaders, NULL, pShaders);
				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject_sphere, pShaders[i]);
					glDeleteShader(pShaders[i]);
				}
			}
			free(pShaders);
			pShaders = 0;
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject_sphere);
	}


}


