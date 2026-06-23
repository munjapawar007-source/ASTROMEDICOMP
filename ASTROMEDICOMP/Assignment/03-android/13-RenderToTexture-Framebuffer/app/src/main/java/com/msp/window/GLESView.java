package com.msp.window;

import android.content.Context;

//OpenGL Related pakages
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;
import android.opengl.Matrix;

//pakages for java native I/O (pakages are classes name or pakage)
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;
import java.nio.ShortBuffer;


import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32;

//events related pakages
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

// Texture related pakages
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;

public class GLESView extends GLSurfaceView
      implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener 
{

   // filed class veriable
   private Context context;
   private GestureDetector gestureDetector;
   private int shaderProgramObject;
   private int mvpMatrixUniform;

   private int vao[] = new int[1];
   private int vbo[] = new int[1];
   private int vbo_texcoord[] = new int[1];
   private float perspectiveProjectionMatrix[] = new float[16];

   private float angle_MP;

   // Fbo related global veriables
   private int winWidth;
   private int winHeight;

   private  int FBO_WIDTH = 512;
   private  int FBO_HEIGHT = 512;

   private int fbo[] = new int[1];
   private int rbo[] = new int[1];
   private int fbo_Texture[] = new int[1];
   int fboResult = -1;

   // Sphere Related Veriables
   private float perspectiveProjectionMatrix_sphere[] = new float[16];
   private int shaderProgramObject_sphere = 0;

   private int modelMatrixUniform_sphere = 0;
   private int viewMatrixUniform_sphere = 0;
   private int projectionMatrixUniform_sphere = 0;

   // Light Related veriable
   private int laUniform_sphere = 0;
   private int ldUniform_sphere = 0; 
   private int lsUniform_sphere = 0; 

   private int kaUniform_sphere = 0;
   private int kdUniform_sphere = 0; 
   private int ksUniform_sphere = 0;

   private int materialShininaseUniform_sphere = 0;
   private int lightPositionUniform_sphere = 0;
   private int LKeyPressedUniform_sphere = 0;

   private float lightAmbient_sphere[] = {0.10f, 0.10f, 0.10f, 1.0f};
   private float lightSpecular_sphere[] = {1.0f, 1.0f, 1.0f, 1.0f};
   private float lightPosition_sphere[] = {100.0f, 100.0f, 100.0f, 1.0f};
   private float lightDiffuse_sphere[] = {1.0f, 1.0f, 1.0f, 1.0f};

   private float materialAmbient_sphere[] = {0.0f, 0.0f, 0.0f, 1.0f};
   private float materialSpecular_sphere[] = {0.70f, 0.70f, 0.70f, 1.0f};
   private float materialShininnes_sphere = 128.0f;
   private float materialDiffuse_sphere[] = {0.50f, 0.20f, 0.70f, 1.0f};

   private int[] vao_sphere = new int[1];
   private int[] vbo_sphere_position = new int[1];
   private int[] vbo_sphere_normal = new int[1];
   private int[] vbo_sphere_element = new int[1];

   private int maxElements;
   private int numElements;
   private int numVertices;

   // texture
   private int textureSamplerUniform;

   private int singleTap;

   public GLESView(Context _context) 
   {
      super(_context);
      // context is recommanded
      context = _context;

      // initialize opengl
      setEGLContextClientVersion(3);
      setRenderer(this);
      // jeva render mode empty hoil teva renderdirty mode on kr
      setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);

      // get gesture detectore
      gestureDetector = new GestureDetector(_context, this, null, false);

      // set this class as double tap listener
      gestureDetector.setOnDoubleTapListener(this);

   }

   // 3 implement GLSurfaceView.RENDERMODE
   @Override
   public void onSurfaceCreated(GL10 gl, EGLConfig config) 
   {
      // code
      int iResult = initialize(gl);

      if (iResult != 0) 
      {
         System.out.println("MSP: initialize() failed");
         System.exit(0);
      }
   }

   @Override
   public void onSurfaceChanged(GL10 gl, int width, int height) 
   {
      // code
      Resize(width, height);

   }

   @Override
   public void onDrawFrame(GL10 gl) 
   {
      // code
      Display();
      Update();
   }

   // implement 1 method for touch
   @Override
   public boolean onTouchEvent(MotionEvent e) 
   {
      // code
      if (!gestureDetector.onTouchEvent(e)) 
      {
         super.onTouchEvent(e);
      }

      return true;
   }

   // 3 method on double tap
   @Override
   public boolean onDoubleTap(MotionEvent e) 
   {

      return true;
   }

   // 3 method on double tap
   @Override
   public boolean onDoubleTapEvent(MotionEvent e) 
   {

      return true;
   }

   @Override
   public boolean onSingleTapConfirmed(MotionEvent e) 
   {
      singleTap++;
      if(singleTap > 1)
         singleTap = 0;

      return true;
   }

   // 6 method from Gesture OnGestureListener
   @Override
   public boolean onDown(MotionEvent e) 
   {

      return true;
   }

   @Override
   public boolean onFling(MotionEvent e1, MotionEvent e2, float velocityX, float velocityY) 
   {

      return true;
   }

   @Override
   public void onLongPress(MotionEvent e) 
   {

   }

   // for swip
   @Override
   public boolean onScroll(MotionEvent e1, MotionEvent e2, float distanceX, float distanceY) 
   {
      // code
      Uninitialize();
      System.exit(0);

      return true;
   }

   @Override
   public void onShowPress(MotionEvent e) 
   {
      // code

   }

   @Override
   public boolean onSingleTapUp(MotionEvent e) 
   {
      // tap kelyaverti fingre uchalyavr event
      // gela pahije manun he use karta

      return true;
   }

   // our custome opengl method
   private int initialize(GL10 gl) 
   {
      // code
      printGLESInfo(gl);

      // Vertex Shader
      final String vertexShaderSourceCode = String.format
      (
         "#version 320 es \n" +
         "in vec4 aPosition;\n" +
         "in vec2 aTexCoord;\n" +
         "out vec2 out_TexCoord;\n" +
         "uniform mat4 uMVPMatrix;\n" +
         "void main(void)\n" +
         "{gl_Position=uMVPMatrix*aPosition;\n" +
         "out_TexCoord = aTexCoord;\n" +
         "}\n"
      );
      // + use to concat string (string la jodnyach kam karto)

      int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
      GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
      GLES32.glCompileShader(vertexShaderObject);

      int shaderCompileStatus[] = new int[1];
      int infoLogLenght[] = new int[1];
      String szInfoLog = null;
      GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
      if (shaderCompileStatus[0] == GLES32.GL_FALSE) {
         GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) {
            szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
            System.out.println("MSP: vertex Shader Compilation log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }
      }

      // fragmant shader
      final String fragmentShaderSourceCode = String.format
      (
         "#version 320 es\n" +
         "precision highp float;" +
         "in vec2 out_TexCoord;\n" +
         "uniform highp sampler2D uTextureSampler;\n" +
         "out vec4 FragColor;\n" +
         "void main(void)\n" +
         "{FragColor=texture(uTextureSampler,out_TexCoord);\n" +
         "}\n"
      );

      int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
      GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
      GLES32.glCompileShader(fragmentShaderObject);

      shaderCompileStatus[0] = 0;
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
      if (shaderCompileStatus[0] == GLES32.GL_FALSE) 
      {
         GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) 
         {
            szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
            System.out.println("MSP: Fragment Shader Compilation log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }
      }

      // Create , Attach , link shader program object
      shaderProgramObject = GLES32.glCreateProgram();
      GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
      GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

      // Bind Shader Attribute at certin index in shader to same index in host program
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributs.MSP_ATTRIBUTE_POSITION, "aPosition");
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributs.MSP_ATTRIBUTE_TEXCOORD, "aTexCoord");

      GLES32.glLinkProgram(shaderProgramObject);
      int shaderProgramLinkStatus[] = new int[1];
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus,0);

      if (shaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
         GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) {
            szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
            System.out.println("MSP: Program info log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }

      }

      // uniforms
      mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMVPMatrix");
	   textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uTextureSampler");

      // provide vertex position,color,texcode, and normal etc.
      final float cube_position[] = new float[] 
      { 
        // front
			 1.0f,  1.0f, 1.0f,	// top-right of front
			-1.0f,  1.0f, 1.0f,	// top-left of front
			-1.0f, -1.0f, 1.0f, // bottom-left of front
			 1.0f, -1.0f, 1.0f,	// bottom-right of front

			// right
			1.0f,  1.0f, -1.0f,	// top-right of right
			1.0f,  1.0f,  1.0f,	// top-left of right
			1.0f, -1.0f,  1.0f,	// bottom-left of right
			1.0f, -1.0f, -1.0f, // bottom-right of right

			// back
			 1.0f,  1.0f, -1.0f,	 // top-right of back
			-1.0f,  1.0f, -1.0f,	 // top-left of back
			-1.0f, -1.0f, -1.0f, // bottom-left of back
			 1.0f, -1.0f, -1.0f,	 // bottom-right of back

			// left
			-1.0f,  1.0f,  1.0f,	 // top-right of left
			-1.0f,  1.0f, -1.0f,	 // top-left of left
			-1.0f, -1.0f, -1.0f, // bottom-left of left
			-1.0f, -1.0f,  1.0f,	 // bottom-right of left

			// top
			 1.0f, 1.0f, -1.0f,	// top-right of top
			-1.0f, 1.0f, -1.0f, // top-left of top
			-1.0f, 1.0f,  1.0f,	// bottom-left of top
			 1.0f, 1.0f,  1.0f,	// bottom-right of top

			// bottom
			 1.0f, -1.0f,  1.0f,	 // top-right of bottom
			-1.0f, -1.0f,  1.0f,	 // top-left of bottom
			-1.0f, -1.0f, -1.0f, // bottom-left of bottom
			 1.0f, -1.0f, -1.0f,	 // bottom-right of bottom
      };

      final float cube_Texcoords[] = new float[] 
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

      GLES32.glGenVertexArrays(1, vao, 0);
      GLES32.glBindVertexArray(vao[0]);

      // Position
      GLES32.glGenBuffers(1, vbo,0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);

      // create a native buffer suitable for native IO but for java
      // 1) set buffer size (36 byte ) 
          ByteBuffer byteBuffer = ByteBuffer.allocateDirect(cube_position.length * 4);
      // 2) set order left to right OR right to left
          byteBuffer.order(ByteOrder.nativeOrder());
      // 3) create which type is data
         FloatBuffer cubePositionBuffer = byteBuffer.asFloatBuffer();
      // 4) fill array of rectangle_ position
         cubePositionBuffer.put(cube_position);
      // 5)  set array to 0th position for repeatable use
          cubePositionBuffer.position(0);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cube_position.length * 4, cubePositionBuffer, GLES32.GL_STATIC_DRAW);
     
      GLES32.glVertexAttribPointer(MyAttributs.MSP_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributs.MSP_ATTRIBUTE_POSITION);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      // TEXCOORD
      GLES32.glGenBuffers(1, vbo_texcoord, 0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord[0]);

      // create a native buffer suitable for native IO but for java
      // 1) set buffer size (36 byte ) 
         byteBuffer = ByteBuffer.allocateDirect(cube_Texcoords.length * 4);
      // 2) set order left to right OR right to left
          byteBuffer.order(ByteOrder.nativeOrder());
      // 3) create which type is data
         FloatBuffer cubeTexcoordsBuffer = byteBuffer.asFloatBuffer();
      // 4) fill array of rectangle_ position
         cubeTexcoordsBuffer.put(cube_Texcoords);
      // 5)  set array to 0th position for repeatable use
          cubeTexcoordsBuffer.position(0);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, cube_Texcoords.length * 4, cubeTexcoordsBuffer, GLES32.GL_STATIC_DRAW);
     
      GLES32.glVertexAttribPointer(MyAttributs.MSP_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributs.MSP_ATTRIBUTE_TEXCOORD);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      GLES32.glBindVertexArray(0);

      
      // Depth initialization
      GLES32.glClearDepthf(1.0f);
      GLES32.glEnable(GLES32.GL_DEPTH_TEST);
      GLES32.glDepthFunc(GLES32.GL_LEQUAL);

      // set the clear color
      GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

      Matrix.setIdentityM(perspectiveProjectionMatrix,0);

      singleTap = 0;
      
	// create fbo and if succssefull call sphere
	if (createAndPrepareFBOForDrawing(FBO_WIDTH,FBO_HEIGHT) == true)
	{
      System.out.println("MSP: FBO creation successfull");
		fboResult = initialize_Sphere();

		if (fboResult != 0)
		{
         System.out.println("MSP: fboResult failed");
			return (-7);
		}
		else
		{
         System.out.println("MSP: initialize_Sphere successfull");
		}
	}
	else
	{
      System.out.println("MSP: FBO creation failed");
		return (-8);
	}

      return 0;
   }

   private void printGLESInfo(GL10 gl)
   {
      // code
      String glesVendor = gl.glGetString(GL10.GL_VENDOR);
      String glesRender = gl.glGetString(GL10.GL_RENDERER);
      String glesVersion = gl.glGetString(GL10.GL_VERSION);

      System.out.println("MSP: " + glesVendor);
      System.out.println("MSP: " + glesRender);
      System.out.println("MSP: " + glesVersion);

   }

      
   private boolean createAndPrepareFBOForDrawing(int textureWidth, int textureHeight)
{
	//veriable declaration

	//code
	//step1:check whethere texture width and height are compatiable
	int maxRenderBufferSize[] = new int[1];
	GLES32.glGetIntegerv(GLES32.GL_MAX_RENDERBUFFER_SIZE, maxRenderBufferSize, 0);
	if(maxRenderBufferSize[0] < textureWidth || maxRenderBufferSize[0] < textureHeight )
	{
     System.out.println("MSP: FBO size exceeds max renderbuffer size");
        return false;
	}

	//step2: create new farme buffer for sphere
	GLES32.glGenFramebuffers(1, fbo, 0);

	//step3:bind with newly created buffer
	GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER,fbo[0]);
	
	//step4:create render buffer as place holder for our depth and our frame buffer
	GLES32.glGenRenderbuffers(1, rbo, 0);

	//step5: bind this new crated render buffer
	GLES32.glBindRenderbuffer(GLES32.GL_RENDERBUFFER, rbo[0]);

	//step6: give specific storeage to this render buffer so depth needed for sphere will reside here
	GLES32.glRenderbufferStorage(GLES32.GL_RENDERBUFFER,GLES32.GL_DEPTH_COMPONENT24,textureWidth,textureHeight); // 16 bit it is recommanded as depth for mobile

	//step7: create an empty texture but fully compactabale texture object for our incoming object
	GLES32.glGenTextures(1, fbo_Texture, 0);
	GLES32.glBindTexture(GLES32.GL_TEXTURE_2D,fbo_Texture[0]);

	//step8: to create full fitured textured give appropreate parameter
	GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_S, GLES32.GL_CLAMP_TO_EDGE);
	GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_WRAP_T, GLES32.GL_CLAMP_TO_EDGE);
	GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
	GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR);

	//stpe9: now create this texture for our fbo
	GLES32.glTexImage2D(GLES32.GL_TEXTURE_2D, 0, GLES32.GL_RGBA, textureWidth,textureHeight,0, GLES32.GL_RGBA, GLES32.GL_UNSIGNED_BYTE, null); //5_6_5 for human unstood greenary

	//step11: give this empty texture to frame buffer
	GLES32.glFramebufferTexture2D(GLES32.GL_FRAMEBUFFER, GLES32.GL_COLOR_ATTACHMENT0, GLES32.GL_TEXTURE_2D,fbo_Texture[0], 0);

	//step12: attach priviously created famebuffer to our frame buffer
	GLES32.glFramebufferRenderbuffer(GLES32.GL_FRAMEBUFFER, GLES32.GL_DEPTH_ATTACHMENT, GLES32.GL_RENDERBUFFER,rbo[0]);

	//step13: check whethere our frame buffer complete or not
	if(GLES32.glCheckFramebufferStatus(GLES32.GL_FRAMEBUFFER)!= GLES32.GL_FRAMEBUFFER_COMPLETE)
	{
      System.out.println("MSP: Framebuffer incomplete");
		return(false);
	}

	//step14:unbind
   GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
   GLES32.glBindRenderbuffer(GLES32.GL_RENDERBUFFER, 0);
   GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, 0);

	return (true);

}	

   private int initialize_Sphere()
   {
      // Vertex Shader
      final String vertexShaderSourceCode = String.format
      (
         "#version 320 es \n" +
         "in vec4 aPosition;\n" +
         "in vec3 aNormal;\n" +
         "uniform mat4 uModelMatrix;\n" +
         "uniform mat4 uViewMatrix;\n" +
         "uniform mat4 uProjectionMatrix;\n" +
         "out vec3 out_transformedNormals;\n" +
         "out vec3 out_viwerVector;\n" +
         "out vec3 out_lightDirection;\n" +
         "uniform vec4 uLightPosition;\n" +
         "uniform int uLkeyPressed;\n" +
         "void main(void)\n" +
         "{gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" +
         "if(uLkeyPressed == 1)\n" +
         "{\n" +
         "vec4 eyeCoordinate = uViewMatrix * uModelMatrix * aPosition;\n" +
         "mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" +
         "out_transformedNormals = normalMatrix * aNormal;\n" +
         "out_lightDirection = vec3(uLightPosition - eyeCoordinate);\n" +
         "out_viwerVector = -eyeCoordinate.xyz;\n" +
         "}\n" +
         "}\n"
      );
      // + use to concat string (string la jodnyach kam karto)

      int vertexShaderObject = GLES32.glCreateShader(GLES32.GL_VERTEX_SHADER);
      GLES32.glShaderSource(vertexShaderObject, vertexShaderSourceCode);
      GLES32.glCompileShader(vertexShaderObject);

      int shaderCompileStatus[] = new int[1];
      int infoLogLenght[] = new int[1];
      String szInfoLog = null;
      GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
      if (shaderCompileStatus[0] == GLES32.GL_FALSE) {
         GLES32.glGetShaderiv(vertexShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) {
            szInfoLog = GLES32.glGetShaderInfoLog(vertexShaderObject);
            System.out.println("MSP: vertex Shader Compilation log" + szInfoLog);
            uninitialize_sphere();
            System.exit(0);
         }
      }

      // fragmant shader
      final String fragmentShaderSourceCode = String.format
      (
         "#version 320 es\n" +
         "precision highp float;" +
         "precision highp int;" +
         "in vec3 out_transformedNormals;\n" +
         "in vec3 out_viwerVector;\n" +
         "in vec3 out_lightDirection;\n" +
         "out vec4 FragColor;\n" +
         "uniform vec3 uLa;\n" +
         "uniform vec3 uLd;\n" +
         "uniform vec3 uLs;\n" +
         "uniform vec3 uKa;\n" +
         "uniform vec3 uKd;\n" +
         "uniform vec3 uKs;\n" +
         "uniform float uMaterialShinines;\n" +
         "uniform int uLkeyPressed;\n" +
         "void main(void)\n" +
         "{\n" +
         "vec3 phong_ads_Light;\n" +
         "if(uLkeyPressed == 1)\n" +
         "{\n" +
         "vec3 normalizedTransformedNormal =  normalize(out_transformedNormals);\n" +
         "vec3 normalizedLightDirection = normalize(out_lightDirection);\n" +
         "vec3 normalizedViwerVector = normalize(out_viwerVector);\n" +
         "vec3 ambientLight = uLa * uKa;\n" +
         "vec3 diffueLight = uLd * uKd * max(dot(normalizedLightDirection,normalizedTransformedNormal),0.0);\n" +
         "vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTransformedNormal);\n" +
         "vec3 specularLight = uLs * uKs * pow(max(dot(reflectionVector,normalizedViwerVector), 0.0),uMaterialShinines);\n" +
         "phong_ads_Light = ambientLight + diffueLight + specularLight; \n" +
         "}\n" +
         "else\n" +
         "{\n" +
         "phong_ads_Light = vec3(1.0,1.0,1.0);\n" +
         "}\n" +
         "FragColor=vec4(phong_ads_Light,1.0);\n" +
         "}\n"
      );

      int fragmentShaderObject = GLES32.glCreateShader(GLES32.GL_FRAGMENT_SHADER);
      GLES32.glShaderSource(fragmentShaderObject, fragmentShaderSourceCode);
      GLES32.glCompileShader(fragmentShaderObject);

      shaderCompileStatus[0] = 0;
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
      if (shaderCompileStatus[0] == GLES32.GL_FALSE) 
      {
         GLES32.glGetShaderiv(fragmentShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) 
         {
            szInfoLog = GLES32.glGetShaderInfoLog(fragmentShaderObject);
            System.out.println("MSP: fragment Shader Compilation log" + szInfoLog);
            uninitialize_sphere();
            System.exit(0);
         }
      }

      // Create , Attach , link shader program object
      shaderProgramObject_sphere = GLES32.glCreateProgram();
      GLES32.glAttachShader(shaderProgramObject_sphere, vertexShaderObject);
      GLES32.glAttachShader(shaderProgramObject_sphere, fragmentShaderObject);

      // Bind Shader Attribute at certin index in shader to same index in host program
      GLES32.glBindAttribLocation(shaderProgramObject_sphere, MyAttributs.MSP_ATTRIBUTE_VERTEX, "aPosition");
      GLES32.glBindAttribLocation(shaderProgramObject_sphere, MyAttributs.MSP_ATTRIBUTE_NORMAL, "aNormal");


      GLES32.glLinkProgram(shaderProgramObject_sphere);
      int shaderProgramLinkStatus[] = new int[1];
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetProgramiv(shaderProgramObject_sphere, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus,0);

      if (shaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
         GLES32.glGetProgramiv(shaderProgramObject_sphere, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) {
            szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject_sphere);
            System.out.println("MSP: Program info log" + szInfoLog);
            uninitialize_sphere();
            System.exit(0);
         }

      }

      // uniforms
      modelMatrixUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uModelMatrix");
      viewMatrixUniform_sphere =  GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uViewMatrix");
      projectionMatrixUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uProjectionMatrix");
      laUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uLa");
      ldUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uLd");
      lsUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uLs");

      lightPositionUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uLightPosition");

      kaUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uKa");
      kdUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uKd");
      ksUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uKs");
      materialShininaseUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uMaterialShinines");

      LKeyPressedUniform_sphere = GLES32.glGetUniformLocation(shaderProgramObject_sphere, "uLkeyPressed");


      Sphere sphere=new Sphere();
      float sphere_vertices[]=new float[1146];
      float sphere_normals[]=new float[1146];
      float sphere_textures[]=new float[764];
      short sphere_elements[]=new short[2280];
      sphere.getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
      numVertices = sphere.getNumberOfSphereVertices();
      numElements = sphere.getNumberOfSphereElements();

      // vao
      GLES32.glGenVertexArrays(1,vao_sphere,0);
      GLES32.glBindVertexArray(vao_sphere[0]);
      
      // position vbo
      GLES32.glGenBuffers(1,vbo_sphere_position,0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_position[0]);
      
      ByteBuffer byteBuffer=ByteBuffer.allocateDirect(sphere_vertices.length * 4);
      byteBuffer.order(ByteOrder.nativeOrder());
      FloatBuffer verticesBuffer=byteBuffer.asFloatBuffer();
      verticesBuffer.put(sphere_vertices);
      verticesBuffer.position(0);
      
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                           sphere_vertices.length * 4,
                           verticesBuffer,
                           GLES32.GL_STATIC_DRAW);
      
      GLES32.glVertexAttribPointer(MyAttributs.MSP_ATTRIBUTE_VERTEX,
                                    3,
                                    GLES32.GL_FLOAT,
                                    false,0,0);
      
      GLES32.glEnableVertexAttribArray(MyAttributs.MSP_ATTRIBUTE_VERTEX);
      
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
      
      // normal vbo
      GLES32.glGenBuffers(1,vbo_sphere_normal,0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_sphere_normal[0]);
      
      byteBuffer=ByteBuffer.allocateDirect(sphere_normals.length * 4);
      byteBuffer.order(ByteOrder.nativeOrder());
      verticesBuffer=byteBuffer.asFloatBuffer();
      verticesBuffer.put(sphere_normals);
      verticesBuffer.position(0);
      
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER,
                           sphere_normals.length * 4,
                           verticesBuffer,
                           GLES32.GL_STATIC_DRAW);
      
      GLES32.glVertexAttribPointer(MyAttributs.MSP_ATTRIBUTE_NORMAL,
                                    3,
                                    GLES32.GL_FLOAT,
                                    false,0,0);
      
      GLES32.glEnableVertexAttribArray(MyAttributs.MSP_ATTRIBUTE_NORMAL);
      
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,0);
      
      // element vbo
      GLES32.glGenBuffers(1,vbo_sphere_element,0);
      GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,vbo_sphere_element[0]);
      
      byteBuffer=ByteBuffer.allocateDirect(sphere_elements.length * 2);
      byteBuffer.order(ByteOrder.nativeOrder());
      ShortBuffer elementsBuffer=byteBuffer.asShortBuffer();
      elementsBuffer.put(sphere_elements);
      elementsBuffer.position(0);
      
      GLES32.glBufferData(GLES32.GL_ELEMENT_ARRAY_BUFFER,
                           sphere_elements.length * 2,
                           elementsBuffer,
                           GLES32.GL_STATIC_DRAW);
      
      //GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

      GLES32.glBindVertexArray(0);

      // Depth initialization
      GLES32.glClearDepthf(1.0f);
      GLES32.glEnable(GLES32.GL_DEPTH_TEST);
      GLES32.glDepthFunc(GLES32.GL_LEQUAL);

      // set the clear color
      GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

      Matrix.setIdentityM(perspectiveProjectionMatrix_sphere, 0);

      resize_Sphere(FBO_WIDTH, FBO_HEIGHT);

      return 0;
      
   }

   private void Resize(int width, int height) 
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
      GLES32.glViewport(0, 0, width, height);

      //set Perspective Projection Matrix
      Matrix.perspectiveM(perspectiveProjectionMatrix,0,45.0f, (float)width / (float)height,0.1f,100.0f);
   }

    private void resize_Sphere(int width, int height) 
   {
      // code
      // if height by default 0 makes1
      if (height <= 0) 
      {
         height = 1;
      }

      // set the viewPort
      GLES32.glViewport(0, 0, width, height);

      //set Perspective Projection Matrix
      Matrix.perspectiveM(perspectiveProjectionMatrix_sphere, 0, 45.0f, (float)width / (float)height, 0.1f, 100.0f);

   }

   private void Display() 
   {

      //call sphere related code
      if(fboResult == 0) //means initialize_sphere is successful
      {
         display_sphere();
      
      }

      //call cube resize to compatisate sphere's resize
      Resize(winWidth,winHeight);

      //clear color again to compensate the effect clear color of sphere
      GLES32.glClearColor(1.0f,1.0f,1.0f,1.0f);


      // code
      GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

      //use Shader program object
	   GLES32.glUseProgram(shaderProgramObject);

     //transformstions
      float modelViewMatrix[] = new float[16];
      float translationMatrix[] =  new float[16];
      float rotationMatrix[] =  new float[16];

      Matrix.setIdentityM(modelViewMatrix,0);
      Matrix.setIdentityM(translationMatrix,0);
      Matrix.setIdentityM(rotationMatrix, 0);

      Matrix.translateM(translationMatrix,0,0.0f, 0.0f, -5.0f);
      Matrix.rotateM(rotationMatrix, 0, angle_MP, 0.0f, 1.0f, 0.0f);
      Matrix.multiplyMM(modelViewMatrix,0,translationMatrix,0,rotationMatrix,0);


     float ModelViewProjectionMatrix[] = new float[16];
      Matrix.setIdentityM(ModelViewProjectionMatrix,0);
    
      Matrix.multiplyMM(ModelViewProjectionMatrix,0,perspectiveProjectionMatrix,0,modelViewMatrix,0);

      //send this matrix to the vertex shader to the uniform
	   GLES32.glUniformMatrix4fv(mvpMatrixUniform,1,false,ModelViewProjectionMatrix,0);

      // for texture
      GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
      GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, fbo_Texture[0]);
      GLES32.glUniform1i(textureSamplerUniform, 0);

      //bind with vao
	   GLES32.glBindVertexArray(vao[0]);

      //draw vertex array
      GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);
	   GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 4, 4);
	   GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 8, 4);
	   GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 12, 4);
	   GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 16, 4);
	   GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 20, 4);

	   GLES32.glBindVertexArray(0);
      GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

      //unUse shader program
      GLES32.glUseProgram(0);


      requestRender();
   }

   
private void display_sphere()
{

	//do fbo related changes
	//if(fbo[0] != 0)
	//{
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER, fbo[0]);
	

	//call sphere resize to compasate to cube resize
	resize_Sphere(FBO_WIDTH,FBO_HEIGHT);

	//call sphere clear color to compasate to cube clear color
	GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	// code
	GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
	// use Shader program object
	GLES32.glUseProgram(shaderProgramObject_sphere);

	// transformstions
	 //transformstions
      float modelMatrix[] = new float[16];
      float viewMatrix[]  = new float[16];

      float translationMatrix[]  =  new float[16];
      float rotationMatrix[]     =  new float[16];

      Matrix.setIdentityM(modelMatrix,0);
      Matrix.setIdentityM(viewMatrix,0);
      Matrix.setIdentityM(translationMatrix,0);
      Matrix.setIdentityM(rotationMatrix, 0);

      Matrix.translateM(translationMatrix,0,0.0f, 0.0f, -5.0f);
      Matrix.rotateM(rotationMatrix, 0, angle_MP, 0.0f, 1.0f, 0.0f);
      //Matrix.multiplyMM(modelMatrix,0,translationMatrix,0,rotationMatrix,0);
      
      modelMatrix = translationMatrix;

      // send this matrix to the vertex shader to the uniform
      GLES32.glUniformMatrix4fv(modelMatrixUniform_sphere, 1, false, modelMatrix, 0);
      GLES32.glUniformMatrix4fv(viewMatrixUniform_sphere, 1, false, viewMatrix, 0);
      GLES32.glUniformMatrix4fv(projectionMatrixUniform_sphere, 1, false, perspectiveProjectionMatrix_sphere, 0);

	if (singleTap == 1)
	{
		GLES32.glUniform3fv(laUniform_sphere, 1, lightAmbient_sphere, 0);
		GLES32.glUniform3fv(ldUniform_sphere, 1, lightDiffuse_sphere, 0);
		GLES32.glUniform3fv(lsUniform_sphere, 1, lightSpecular_sphere, 0);

		GLES32.glUniform4fv(lightPositionUniform_sphere, 1, lightPosition_sphere, 0);

		GLES32.glUniform3fv(kaUniform_sphere, 1, materialAmbient_sphere, 0);
		GLES32.glUniform3fv(kdUniform_sphere, 1, materialDiffuse_sphere, 0);
		GLES32.glUniform3fv(ksUniform_sphere, 1, materialSpecular_sphere, 0);

		GLES32.glUniform1f(materialShininaseUniform_sphere, materialShininnes_sphere);

		GLES32.glUniform1i(LKeyPressedUniform_sphere, 1);
	}
	else
	{
		GLES32.glUniform1i(LKeyPressedUniform_sphere, 0);
	}
	
   // bind with vao
	GLES32.glBindVertexArray(vao_sphere[0]);

	// *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
   GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
   GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
      
	// *** unbind vao ***
	GLES32.glBindVertexArray(0);

	// unUse shader program
	GLES32.glUseProgram(0);

	if(fbo[0] != 0)
	{
		GLES32.glBindFramebuffer(GLES32.GL_FRAMEBUFFER,0);
	}
	
}



   private void Update() 
   {
      // code
       angle_MP = angle_MP - 0.1f;

      if (angle_MP <= 0.0f)
      {
         angle_MP = angle_MP + 360.0f;
      }
   }

   private void Uninitialize() 
   {
      // code
   if(rbo[0] > 0)
	{
		GLES32.glDeleteRenderbuffers(1,rbo, 0);
		rbo[0] = 0;
	}

	if(fbo[0] > 0)
	{
		GLES32.glDeleteFramebuffers(1,fbo, 0);
		fbo[0] = 0;
	}

	if(fboResult == 0) //sphere initialization successful
	{
		uninitialize_sphere();
	}

      if(vbo_texcoord[0] > 0)
      {
         GLES32.glDeleteBuffers(1,vbo_texcoord,0);
         vbo_texcoord[0] = 0;

      }

      if(vbo[0] > 0)
      {
         GLES32.glDeleteBuffers(1,vbo,0);
         vbo[0] = 0;
      }

       if(vao[0] > 0)
      {
         GLES32.glDeleteVertexArrays(1,vao,0);
         vao[0] = 0;
      }

      //detach delete shader program object
      if(shaderProgramObject > 0)
      {
         GLES32.glUseProgram(shaderProgramObject);
         GLES32.glUseProgram(0);

         //return value
         int retVal[] = new int[1];

         GLES32.glGetProgramiv(shaderProgramObject,GLES32.GL_ATTACHED_SHADERS,retVal,0);

         int numAttachedShaders = retVal[0];
         if(numAttachedShaders > 0)
         {
            int shaderObjects[] = new int[numAttachedShaders];
           
            GLES32.glGetAttachedShaders(shaderProgramObject,numAttachedShaders,retVal,0,shaderObjects,0);

            for(int i = 0; i<numAttachedShaders;i++)
            {
               GLES32.glDetachShader(shaderProgramObject,shaderObjects[i]);
               GLES32.glDeleteShader(shaderObjects[i]);
               shaderObjects[i] = 0;
            }
         }
         GLES32.glUseProgram(0);
         GLES32.glDeleteProgram(shaderProgramObject);
      }

   }

   
private void uninitialize_sphere()
{
	
	if(vao_sphere[0] != 0)
   {
      GLES32.glDeleteVertexArrays(1, vao_sphere, 0);
      vao_sphere[0]=0;
   }
   
   // destroy position vbo
   if(vbo_sphere_position[0] != 0)
   {
      GLES32.glDeleteBuffers(1, vbo_sphere_position, 0);
      vbo_sphere_position[0]=0;
   }
   
   // destroy normal vbo
   if(vbo_sphere_normal[0] != 0)
   {
      GLES32.glDeleteBuffers(1, vbo_sphere_normal, 0);
      vbo_sphere_normal[0]=0;
   }
   
   // destroy element vbo
   if(vbo_sphere_element[0] != 0)
   {
      GLES32.glDeleteBuffers(1, vbo_sphere_element, 0);
      vbo_sphere_element[0]=0;
   }

   //detach delete shader program object
   if(shaderProgramObject_sphere > 0)
   {
      GLES32.glUseProgram(shaderProgramObject_sphere);
      GLES32.glUseProgram(0);

      //return value
      int retVal[] = new int[1];

      GLES32.glGetProgramiv(shaderProgramObject_sphere,GLES32.GL_ATTACHED_SHADERS,retVal,0);

      int numAttachedShaders = retVal[0];
      if(numAttachedShaders > 0)
      {
         int shaderObjects[] = new int[numAttachedShaders];
         
         GLES32.glGetAttachedShaders(shaderProgramObject_sphere,numAttachedShaders,retVal,0,shaderObjects,0);

         for(int i = 0; i<numAttachedShaders;i++)
         {
            GLES32.glDetachShader(shaderProgramObject_sphere,shaderObjects[i]);
            GLES32.glDeleteShader(shaderObjects[i]);
            shaderObjects[i] = 0;
         }
      }
      GLES32.glUseProgram(0);
      GLES32.glDeleteProgram(shaderProgramObject_sphere);
   }


}


}
