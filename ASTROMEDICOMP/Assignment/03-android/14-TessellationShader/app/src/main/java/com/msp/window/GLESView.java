package com.msp.window;

import android.content.Context;

//OpenGL Related packages
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;
import android.opengl.Matrix;

//packages for java native I/O (packages are classes name or package)
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32;

//events related packages
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;


public class GLESView extends GLSurfaceView
      implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener 
{

   // filed/class variable
   private Context context;
   private final GestureDetector gestureDetector;
   private int shaderProgramObject;
  
   private final int[] vao = new int[1];
   private final int[] vbo_position = new int[1];

   private final float[] perspectiveProjectionMatrix = new float[16];
   private int mvpMatrixUniform;

   // Tesselltion related veriable
   private int numberOfLineSegmentsUniform   = 0;
   private int numberOfLineStripsUniform     = 0;
   private int lineColorsUniform             = 0;
   private int uiNumberOfLineSegments = 1;

   private FloatBuffer lineColorBuffer; 
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
         uiNumberOfLineSegments++;
			if (uiNumberOfLineSegments >= 30)
			{
				uiNumberOfLineSegments = 30;
			}
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
      //printGLESInfo(gl);
            System.out.println("MSP: initialize");
	// **************** VERTEX SHADER ****************
      final String vertexShaderSourceCode = String.format
      (
         "#version 320 es \n" +
         "in vec2 vPosition;\n" +
         "void main(void)\n" +
         "{gl_Position = vec4(vPosition, 0.0, 1.0);\n" +
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
            System.out.println("MSP:vertex Shader Compilation log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }
      }

      // **************** TESS_CONTROL SHADER ****************
      final String tessellationControlShaderSourceCode = String.format
      (
         "#version 320 es\n" +
         "precision highp float;" +
         "precision highp int;" +
         " layout(vertices=4)out;\n" +
         " uniform int uNumberOfLineSegments;\n" +
         " uniform int uNumberOfLineStrips;\n" +
         "void main(void)\n" +
         "{\n" +
            "gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID ].gl_Position;\n" +
            "gl_TessLevelOuter[0] = float(uNumberOfLineStrips);\n" +
            "gl_TessLevelOuter[1] = float(uNumberOfLineSegments);\n" +
         "}\n"
      );

      int tessellationControlShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_CONTROL_SHADER);
      GLES32.glShaderSource(tessellationControlShaderObject, tessellationControlShaderSourceCode);
      GLES32.glCompileShader(tessellationControlShaderObject);

      shaderCompileStatus[0] = 0;
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetShaderiv(tessellationControlShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
      if (shaderCompileStatus[0] == GLES32.GL_FALSE) 
      {
         GLES32.glGetShaderiv(tessellationControlShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) 
         {
            szInfoLog = GLES32.glGetShaderInfoLog(tessellationControlShaderObject);
            System.out.println("MSP:TESS_CONTROL Shader Compilation log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }
      }

      // **************** TESS_EVALUATION SHADER ****************
      final String tessellationEvaluationShaderSourceCode = String.format
      (
         "#version 320 es\n" +
         "precision highp float;" +
         "precision highp int;" +
         " layout(isolines)in;\n" +
         " uniform mat4 uMVPMatrix;\n" +
         "void main(void)\n" +
         "{\n" +
            "vec3 p0 = gl_in[0].gl_Position.xyz;\n" +
            "vec3 p1 = gl_in[1].gl_Position.xyz;\n" +
            "vec3 p2 = gl_in[2].gl_Position.xyz;\n" +
            "vec3 p3 = gl_in[3].gl_Position.xyz;\n" +
            "vec3 p = p0 * (1.0f - gl_TessCoord.x) * (1.0f - gl_TessCoord.x) * (1.0f - gl_TessCoord.x) + p1 * 3.0f * (gl_TessCoord.x) * (1.0f - gl_TessCoord.x) * (1.0f - gl_TessCoord.x) + p2 * 3.0f * (gl_TessCoord.x) * (gl_TessCoord.x) * (1.0f - gl_TessCoord.x) + p3 * (gl_TessCoord.x) * (gl_TessCoord.x) * (gl_TessCoord.x);\n" +
            "gl_Position = uMVPMatrix * vec4(p, 1.0f);\n" +
         "}\n"
      );

      int tessellationEvaluationShaderObject = GLES32.glCreateShader(GLES32.GL_TESS_EVALUATION_SHADER);
      GLES32.glShaderSource(tessellationEvaluationShaderObject, tessellationEvaluationShaderSourceCode);
      GLES32.glCompileShader(tessellationEvaluationShaderObject);

      shaderCompileStatus[0] = 0;
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetShaderiv(tessellationEvaluationShaderObject, GLES32.GL_COMPILE_STATUS, shaderCompileStatus, 0);
      if (shaderCompileStatus[0] == GLES32.GL_FALSE) 
      {
         GLES32.glGetShaderiv(tessellationEvaluationShaderObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) 
         {
            szInfoLog = GLES32.glGetShaderInfoLog(tessellationEvaluationShaderObject);
            System.out.println("MSP:TESS_EVALUATION Shader Compilation log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }
      }

      System.out.println("MSP: initialize-1");

	// **************** FRAGMENT SHADER ****************
      final String fragmentShaderSourceCode = String.format
      (
         "#version 320 es\n" +
         "precision highp float;" +
         "precision highp int;" +
         "uniform vec4 uLineColor;\n" +
         "out vec4 FragColor;\n" +
         "void main(void)\n" +
         "{FragColor = uLineColor;\n" +
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
            System.out.println("MSP: FRAGMENT Shader Compilation log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }
      }

      // Create , Attach , link shader program object
      shaderProgramObject = GLES32.glCreateProgram();
      GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
      GLES32.glAttachShader(shaderProgramObject, tessellationControlShaderObject);
      GLES32.glAttachShader(shaderProgramObject, tessellationEvaluationShaderObject);
      GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

      System.out.println("MSP: initialize-2");

      // Bind Shader Attribute at certin index in shader to same index in host program
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributes.MSP_ATTRIBUTE_POSITION, "vPosition");

      GLES32.glLinkProgram(shaderProgramObject);
      int shaderProgramLinkStatus[] = new int[1];
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus,0);

      if (shaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
         GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) {
            szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
            System.out.println("MSP: Get Program info log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }

      }

      // get the required uniform loction from the sheder
      mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMVPMatrix"); 
      numberOfLineSegmentsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uNumberOfLineSegments"); 
      numberOfLineStripsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uNumberOfLineStrips");
      lineColorsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLineColor");

       System.out.println("MSP: initialize-3");
      // provide vertex position,color,texcode, and normal etc.
      final float line_position[] = new float[] 
      {
         -1.0f, -1.0f,
			-0.5f,  1.0f,
			 0.5f,  -1.0f,
			 1.0f,  1.0f
      };

      
      GLES32.glGenVertexArrays(1, vao, 0);
      GLES32.glBindVertexArray(vao[0]);

      /*-------------RECTANGLE------------*/
      // Position
      GLES32.glGenBuffers(1, vbo_position,0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

      // create a native buffer suitable for native IO for java
      // 1) set buffer size (36 byte ) 
         ByteBuffer byteBuffer = ByteBuffer.allocateDirect(line_position.length * 4);
      // 2) set order left to right OR right to left
         byteBuffer.order(ByteOrder.nativeOrder());
      // 3) create which type is data
         FloatBuffer linePositionBuffer = byteBuffer.asFloatBuffer();
      // 4) fill array of rectangle position
         linePositionBuffer.put( line_position);
      // 5)  set array to 0th position for repeatable use
         linePositionBuffer.position(0);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, line_position.length * 4, linePositionBuffer, GLES32.GL_STATIC_DRAW);
   
      GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_POSITION, 2, GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_POSITION);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
      GLES32.glBindVertexArray(0);


      // Depth initialization
      GLES32.glClearDepthf(1.0f);
      GLES32.glEnable(GLES32.GL_DEPTH_TEST);
      GLES32.glDepthFunc(GLES32.GL_LEQUAL);

      // set the clear color
      GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
     
      Matrix.setIdentityM(perspectiveProjectionMatrix,0);
       System.out.println("MSP: initialize-4");

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

   private void Resize(int width, int height) 
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
      Matrix.perspectiveM(perspectiveProjectionMatrix,0,45.0f, (float)width / (float)height,0.1f,100.0f);

   }

   private void Display() 
   {
        
      // code
      GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

      //use Shader program object
	   GLES32.glUseProgram(shaderProgramObject);

     System.out.println("MSP: Display-1");
         
      //transformstions
      float modelViewMatrix[] = new float[16];
      float translationMatrix[] =  new float[16];
       
      Matrix.setIdentityM(modelViewMatrix,0);
      Matrix.setIdentityM(translationMatrix,0);

      Matrix.translateM(translationMatrix,0, 0.0f, 0.0f, -5.0f);
      modelViewMatrix = translationMatrix;
     System.out.println("MSP: Display-2");
      /*float colorUniform[] = new float[4];
      colorUniform[0] = 1.0f;
      colorUniform[1] = 1.0f;
      colorUniform[2] = 0.0f;
      colorUniform[3] = 1.0f;*/

      lineColorBuffer = FloatBuffer.wrap(new float[] { 1.0f, 0.0f, 0.0f, 1.0f });


      float ModelViewProjectionMatrix[] = new float[16];
      Matrix.setIdentityM(ModelViewProjectionMatrix,0);
    
      Matrix.multiplyMM(ModelViewProjectionMatrix,0,perspectiveProjectionMatrix,0,modelViewMatrix,0);
       

      //send this matrix to the vertex shader to the uniform
	   GLES32.glUniformMatrix4fv(mvpMatrixUniform,1,false,ModelViewProjectionMatrix,0);
      
      GLES32.glUniform1i(numberOfLineSegmentsUniform, uiNumberOfLineSegments);
      GLES32.glUniform1i(numberOfLineStripsUniform, 1); //as we have only one line
      
        System.out.println("MSP: Display-3");
    GLES32.glUniform4fv(lineColorsUniform, 1, lineColorBuffer);
     //Color
      System.out.println("MSP: " + lineColorsUniform);


       /* ---------------Line--------------- */
      //bind with vao
	   GLES32.glBindVertexArray(vao[0]);

      // Deside how many vertices we created in 1 patch
	   GLES32.glPatchParameteri(GLES32.GL_PATCH_VERTICES,4);
      // draw vertex array
      GLES32.glDrawArrays(GLES32.GL_PATCHES, 0, 4);
       System.out.println("MSP: initialize-12");
	   GLES32.glBindVertexArray(0);

      //unUse shader program
      GLES32.glUseProgram(0);

      requestRender();
   }

   private void Update() 
   {
      // code
     
    
   }

   private void Uninitialize() 
   {
      // code
      if(vbo_position[0] > 0)
      {
         GLES32.glDeleteBuffers(1,vbo_position,0);
         vbo_position[0] = 0;
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

}
