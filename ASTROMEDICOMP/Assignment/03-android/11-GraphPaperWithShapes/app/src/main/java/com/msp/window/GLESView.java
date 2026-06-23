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

import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32;

//events related pakages
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;


public class GLESView extends GLSurfaceView
      implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener 
{

   // filed/class veriable
   private Context context;
   private GestureDetector gestureDetector;
   private int shaderProgramObject;
   

   private int[] vao_rectangle = new int[1];
   private int[] vbo_position_rectangle = new int[1];
   private int[] vbo_color_rectangle = new int[1];

   private int[] vao_triangle = new int[1];
   private int[] vbo_position_triangle = new int[1];
   private int[] vbo_color_triangle = new int[1];

   private int[] vao_circle = new int[1];
   private int[] vbo_circle = new int[1];
   private int[] vbo_color_circle = new int[1];

   private int[] vao_graphpaper = new int[1];
   private int[] vbo_graphpaper = new int[1];
   private int[] vbo_color_graphpaper = new int[1];

   private float perspectiveProjectionMatrix[] = new float[16];
   private int mvpMatrixUniform;

   private float STEP = 0.20f;
   private int COUNT = (int)(360.0f / STEP);
   //private float circle_position[] = new float[4 * COUNT * 3];
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
     if(singleTap > 4)
     {
      singleTap = 1;
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
      printGLESInfo(gl);

      // Vertex Shader
      final String vertexShaderSourceCode = String.format
      (
         "#version 320 es \n" +
         "in vec4 aPosition;\n" +
         "in vec4 aColor;\n" +
         "out vec4 out_color;\n" +
         "uniform mat4 uMVPMatrix;\n" +
         "void main(void)\n" +
         "{gl_Position=uMVPMatrix*aPosition;\n" +
         "out_color = aColor;\n" +
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
            System.out.println("vertex Shader Compilation log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }
      }

      // fragmant shader
      final String fragmentShaderSourceCode = String.format
      (
         "#version 320 es\n" +
         "precision highp float\n;" +
         "in vec4 out_color;\n" +
         "out vec4 FragColor;\n" +
         "void main(void)\n" +
         "{FragColor=out_color;\n" +
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
            System.out.println("vertex Shader Compilation log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }
      }

      // Create , Attach , link shader program object
      shaderProgramObject = GLES32.glCreateProgram();
      GLES32.glAttachShader(shaderProgramObject, vertexShaderObject);
      GLES32.glAttachShader(shaderProgramObject, fragmentShaderObject);

      // Bind Shader Attribute at certin index in shader to same index in host program
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributes.MSP_ATTRIBUTE_POSITION, "aPosition");
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributes.MSP_ATTRIBUTE_COLOR, "aColor");

      GLES32.glLinkProgram(shaderProgramObject);
      int shaderProgramLinkStatus[] = new int[1];
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus,0);

      if (shaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
         GLES32.glGetProgramiv(shaderProgramObject, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) {
            szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject);
            System.out.println("msp Program info log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }

      }

      // uniforms
      mvpMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMVPMatrix");

     
      allShapesGeometry();

      // Depth initialization
      GLES32.glClearDepthf(1.0f);
      GLES32.glEnable(GLES32.GL_DEPTH_TEST);
      GLES32.glDepthFunc(GLES32.GL_LEQUAL);

      // set the clear color
      GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      
      
      singleTap = 0;


      Matrix.setIdentityM(perspectiveProjectionMatrix,0);

      return 0;
   }

   private void allShapesGeometry()
   {

	   final float Rectangle_position[] = new float[]
		{
			 1.0f,  1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			 1.0f, -1.0f, 0.0f
      };

	   final float Rectangle_color[] = new float[]
		{
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f,
			0.0f, 0.0f, 1.0f
      };

      GLES32.glGenVertexArrays(1, vao_rectangle, 0);
      GLES32.glBindVertexArray(vao_rectangle[0]);

           // Position
      GLES32.glGenBuffers(1, vbo_position_rectangle,0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_rectangle[0]);

      // create a native buffer suitable for native IO for java
      // 1) set buffer size (36 byte ) 
         ByteBuffer byteBuffer = ByteBuffer.allocateDirect(Rectangle_position.length * 4);
      // 2) set order left to right OR right to left
         byteBuffer.order(ByteOrder.nativeOrder());
      // 3) create which type is data
         FloatBuffer rectanglePositionBuffer = byteBuffer.asFloatBuffer();
      // 4) fill array of rectangle position
         rectanglePositionBuffer.put( Rectangle_position);
      // 5)  set array to 0th position for repeatable use
         rectanglePositionBuffer.position(0);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, Rectangle_position.length * 4, rectanglePositionBuffer, GLES32.GL_STATIC_DRAW);
   
      GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_POSITION);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      // color
      GLES32.glGenBuffers(1, vbo_color_rectangle,0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_color_rectangle[0]);
      // create a native buffer suitable for native IO  for java
      // 1) set buffer size (36 byte ) 
         byteBuffer = ByteBuffer.allocateDirect(Rectangle_color.length * 4);
      // 2) set order left to right OR right to left
         byteBuffer.order(ByteOrder.nativeOrder());
      // 3) create which type is data
         FloatBuffer rectangleColorBuffer = byteBuffer.asFloatBuffer();
      // 4) fill array of traingle position
         rectangleColorBuffer.put(Rectangle_color);
      // 5)  set array to 0th position for repeatable use
         rectangleColorBuffer.position(0);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, Rectangle_color.length * 4, rectangleColorBuffer, GLES32.GL_STATIC_DRAW);
   
      GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_COLOR, 3, GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_COLOR);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
      GLES32.glBindVertexArray(0);

      /*-------------------------------------------------------------------------------------------------*/
      // Triangle
      final float triangle_position[] = new float[]
		{
			0.0f, 1.0f, 0.0f,
			-1.0f, -1.0f, 0.0f,
			1.0f, -1.0f, 0.0f};

	   final float triangle_color[] = new float[]
		{
			1.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 1.0f};

      GLES32.glGenVertexArrays(1, vao_triangle, 0);
      GLES32.glBindVertexArray(vao_triangle[0]);

      // POSITION
      GLES32.glGenBuffers(1, vbo_position_triangle,0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position_triangle[0]);
      
      // create a native buffer suitable for native IO for java
      // 1) set buffer size (36 byte)
         byteBuffer = ByteBuffer.allocateDirect(triangle_position.length * 4);
      // 2) set order left to right or right to left
         byteBuffer.order(ByteOrder.nativeOrder());
      // 3) create which type is data
         FloatBuffer trianglePositionBuffer = byteBuffer.asFloatBuffer();
      // 4) fill array of Triangle position
         trianglePositionBuffer.put(triangle_position);
      // 5) set array to 0th position for repeatble use
         trianglePositionBuffer.position(0);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangle_position.length * 4, trianglePositionBuffer, GLES32.GL_STATIC_DRAW);

      GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_POSITION, 3,GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_POSITION);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      // COLOR
      GLES32.glGenBuffers(1, vbo_color_triangle,0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_color_triangle[0]);

      // create a native buffer suitable for native IO for Java
      // 1) set buffer size (36 byte)
         byteBuffer = ByteBuffer.allocateDirect(triangle_color.length * 4);
      // 2) set order left to right or right to left
         byteBuffer.order(ByteOrder.nativeOrder());
      // 3) create which type is data
         FloatBuffer triangleColorBuffer = byteBuffer.asFloatBuffer();
      // 4) fill array of Triangle position
         triangleColorBuffer.put(triangle_color);
      // 5) set array to 0th position for repeatble use
         triangleColorBuffer.position(0);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, triangle_color.length * 4, triangleColorBuffer, GLES32.GL_STATIC_DRAW);

      GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_COLOR, 3,GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_COLOR);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      GLES32.glBindVertexArray(0);

      /*-------------------------------------------------------------------------------------------------*/
      float rad = 3.14159265f / 180.0f;
      float x = 0.0f, y = 0.0f;
      int segments = 360;
      int index = 0;

      final float circle_position[] = new float[segments * 3];
      for (int i = 0; i < segments; i++)
      {
         x = (float)(1.0f * Math.cos(i * rad));
         y = (float)(1.0f * Math.sin(i * rad));
         circle_position[index++] = x;
         circle_position[index++] = y;
         circle_position[index++] = 0.0f; 
      }

      final float circle_color[] = new float[segments * 3];
      for (int i=0; i<1080; i+=3)
      {
         circle_color[i] = 1.0f;
         circle_color[i+1] = 0.0f;
         circle_color[i+2] = 1.0f;
      }
       
         GLES32.glGenVertexArrays(1, vao_circle, 0);
         GLES32.glBindVertexArray(vao_circle[0]);

         // POSITION
         GLES32.glGenBuffers(1, vbo_circle,0);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_circle[0]);
         // create a native buffer suitable for native IO for java
         // 1) set buffer size (36 byte)
            byteBuffer = ByteBuffer.allocateDirect(circle_position.length * 4);
         // 2) set order left to right or right to left
            byteBuffer.order(ByteOrder.nativeOrder());
         // 3) create which type is data
            FloatBuffer circlePositionBuffer = byteBuffer.asFloatBuffer();
         // 4) fill array of Triangle position
            circlePositionBuffer.put(circle_position);
         // 5) set array to 0th position for repeatble use
            circlePositionBuffer.position(0);
         GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circle_position.length * 4, circlePositionBuffer, GLES32.GL_STATIC_DRAW);

         GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_POSITION, 3,GLES32.GL_FLOAT, false, 0, 0);
         GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_POSITION);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
         
         // COLOR
         GLES32.glGenBuffers(1, vbo_color_circle,0);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_color_circle[0]);

         // create a native buffer suitable for native IO for Java
         // 1) set buffer size (36 byte)
            byteBuffer = ByteBuffer.allocateDirect(circle_color.length * 4);
         // 2) set order left to right or right to left
            byteBuffer.order(ByteOrder.nativeOrder());
         // 3) create which type is data
            FloatBuffer circleColorBuffer = byteBuffer.asFloatBuffer();
         // 4) fill array of Triangle position
         circleColorBuffer.put(circle_color);
         // 5) set array to 0th position for repeatble use
            circleColorBuffer.position(0);
         GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, circle_color.length * 4, circleColorBuffer, GLES32.GL_STATIC_DRAW);

         GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_COLOR, 3,GLES32.GL_FLOAT, false, 0, 0);
         GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_COLOR);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

         GLES32.glBindVertexArray(0);

	/*-------------------------------------------------------------------------------------------------*/

         // vbo_graphpaper
	
      final float graph_position[] = {
         3.0f, 0.2f, 0.0f, -3.0f, 0.2f, 0.0f,
         3.0f, 0.4f, 0.0f, -3.0f, 0.4f, 0.0f,
         3.0f, 0.6f, 0.0f, -3.0f, 0.6f, 0.0f,
         3.0f, 0.8f, 0.0f, -3.0f, 0.8f, 0.0f,
         3.0f, 1.0f, 0.0f, -3.0f, 1.0f, 0.0f,
         3.0f, 1.2f, 0.0f, -3.0f, 1.2f, 0.0f,
         3.0f, 1.4f, 0.0f, -3.0f, 1.4f, 0.0f,
         3.0f, 1.6f, 0.0f, -3.0f, 1.6f, 0.0f,
         3.0f, 1.8f, 0.0f, -3.0f, 1.8f, 0.0f,
         3.0f, 2.0f, 0.0f, -3.0f, 2.0f, 0.0f,
         3.0f, 0.0f, 0.0f, -3.0f, 0.0f, 0.0f,
         3.0f, -0.2f, 0.0f, -3.0f, -0.2f, 0.0f,
         3.0f, -0.4f, 0.0f, -3.0f, -0.4f, 0.0f,
         3.0f, -0.6f, 0.0f, -3.0f, -0.6f, 0.0f,
         3.0f, -0.8f, 0.0f, -3.0f, -0.8f, 0.0f,
         3.0f, -1.0f, 0.0f, -3.0f, -1.0f, 0.0f,
         3.0f, -1.2f, 0.0f, -3.0f, -1.2f, 0.0f,
         3.0f, -1.4f, 0.0f, -3.0f, -1.4f, 0.0f,
         3.0f, -1.6f, 0.0f, -3.0f, -1.6f, 0.0f,
         3.0f, -1.8f, 0.0f, -3.0f, -1.8f, 0.0f,
         3.0f, -2.0f, 0.0f, -3.0f, -2.0f, 0.0f,

         0.2f, 3.0f, 0.0f, 0.2f, -3.0f, 0.0f,
         0.4f, 3.0f, 0.0f, 0.4f, -3.0f, 0.0f,
         0.6f, 3.0f, 0.0f, 0.6f, -3.0f, 0.0f,
         0.8f, 3.0f, 0.0f, 0.8f, -3.0f, 0.0f,
         1.0f, 3.0f, 0.0f, 1.0f, -3.0f, 0.0f,
         1.2f, 3.0f, 0.0f, 1.2f, -3.0f, 0.0f,
         1.4f, 3.0f, 0.0f, 1.4f, -3.0f, 0.0f,
         1.6f, 3.0f, 0.0f, 1.6f, -3.0f, 0.0f,
         1.8f, 3.0f, 0.0f, 1.8f, -3.0f, 0.0f,
         2.0f, 3.0f, 0.0f, 2.0f, -3.0f, 0.0f,
         0.0f, 3.0f, 0.0f, 0.0f, -3.0f, 0.0f,
         -0.2f, 3.0f, 0.0f, -0.2f, -3.0f, 0.0f,
         -0.4f, 3.0f, 0.0f, -0.4f, -3.0f, 0.0f,
         -0.6f, 3.0f, 0.0f, -0.6f, -3.0f, 0.0f,
         -0.8f, 3.0f, 0.0f, -0.8f, -3.0f, 0.0f,
         -1.0f, 3.0f, 0.0f, -1.0f, -3.0f, 0.0f,
         -1.2f, 3.0f, 0.0f, -1.2f, -3.0f, 0.0f,
         -1.4f, 3.0f, 0.0f, -1.4f, -3.0f, 0.0f,
         -1.6f, 3.0f, 0.0f, -1.6f, -3.0f, 0.0f,
         -1.8f, 3.0f, 0.0f, -1.8f, -3.0f, 0.0f,
         -2.0f, 3.0f, 0.0f, -2.0f, -3.0f, 0.0f,
      };

      final float graph_color[] = 
      {
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,

         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
         0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f,
      };
         GLES32.glGenVertexArrays(1, vao_graphpaper, 0);
         GLES32.glBindVertexArray(vao_graphpaper[0]);

         // POSITION
         GLES32.glGenBuffers(1, vbo_graphpaper,0);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_graphpaper[0]);
         // create a native buffer suitable for native IO for java
         // 1) set buffer size (36 byte)
            byteBuffer = ByteBuffer.allocateDirect(graph_position.length * 4);
         // 2) set order left to right or right to left
            byteBuffer.order(ByteOrder.nativeOrder());
         // 3) create which type is data
            FloatBuffer graphpaperPositionBuffer = byteBuffer.asFloatBuffer();
         // 4) fill array of Triangle position
            graphpaperPositionBuffer.put(graph_position);
         // 5) set array to 0th position for repeatble use
            graphpaperPositionBuffer.position(0);
         GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, graph_position.length * 4, graphpaperPositionBuffer, GLES32.GL_STATIC_DRAW);

         GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_POSITION, 3,GLES32.GL_FLOAT, false, 0, 0);
         GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_POSITION);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);
         
         // COLOR
         GLES32.glGenBuffers(1, vbo_color_graphpaper,0);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER,vbo_color_graphpaper[0]);

         // create a native buffer suitable for native IO for Java
         // 1) set buffer size (36 byte)
            byteBuffer = ByteBuffer.allocateDirect(graph_color.length * 4);
         // 2) set order left to right or right to left
            byteBuffer.order(ByteOrder.nativeOrder());
         // 3) create which type is data
            FloatBuffer graphpaperColorBuffer = byteBuffer.asFloatBuffer();
         // 4) fill array of Triangle position
         graphpaperColorBuffer.put(graph_color);
         // 5) set array to 0th position for repeatble use
            graphpaperColorBuffer.position(0);
         GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, graph_color.length * 4, graphpaperColorBuffer, GLES32.GL_STATIC_DRAW);

         GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_COLOR, 3,GLES32.GL_FLOAT, false, 0, 0);
         GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_COLOR);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

         GLES32.glBindVertexArray(0);
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

      //transformstions
      float modelViewMatrix[] = new float[16];
      Matrix.setIdentityM(modelViewMatrix,0);

      float translationMatrix[] =  new float[16];
      Matrix.setIdentityM(translationMatrix,0);
      Matrix.translateM(translationMatrix,0,0.0f, 0.0f, -3.0f);

      modelViewMatrix = translationMatrix;

     float ModelViewProjectionMatrix[] = new float[16];
      Matrix.setIdentityM(ModelViewProjectionMatrix,0);
    
      Matrix.multiplyMM(ModelViewProjectionMatrix,0,perspectiveProjectionMatrix,0,modelViewMatrix,0);

      //send this matrix to the vertex shader to the uniform
	   GLES32.glUniformMatrix4fv(mvpMatrixUniform,1,false,ModelViewProjectionMatrix,0);

      //bind with vao
	   // draw triangle
      GLES32.glBindVertexArray(vao_triangle[0]);
      if (singleTap == 1)
         GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 3);

      // Draw Rectangle
      GLES32.glBindVertexArray(vao_rectangle[0]);
      if (singleTap == 2)
         GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 4);

      // Draw Circle
      GLES32.glBindVertexArray(vao_circle[0]);
      if (singleTap == 3)
         GLES32.glDrawArrays(GLES32.GL_LINE_LOOP, 0, 360);
         
      GLES32.glBindVertexArray(vao_graphpaper[0]);
      if (singleTap == 4)
         GLES32.glDrawArrays(GLES32.GL_LINES, 0, 480);

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
          if (vbo_graphpaper[0] > 0)
      {
         GLES32.glDeleteBuffers(1, vbo_graphpaper, 0);
         vbo_graphpaper[0] = 0;
      }

      // free vao
      if (vao_graphpaper[0] > 0)
      {
         GLES32.glDeleteVertexArrays(1, vao_graphpaper, 0);
         vao_graphpaper[0] = 0;
      }

      if (vbo_circle[0] > 0)
      {
         GLES32.glDeleteBuffers(1, vbo_circle, 0);
         vbo_circle[0] = 0;
      }

      // free vao
      if (vao_circle[0] > 0)
      {
         GLES32.glDeleteVertexArrays(1, vao_circle, 0);
         vao_circle[0] = 0;
      }

      // free vbo color
      if (vbo_color_rectangle[0] > 0)
      {
         GLES32.glDeleteBuffers(1, vbo_color_rectangle, 0);
         vbo_color_rectangle[0] = 0;
      }

      // free vbo postion
      if (vbo_position_rectangle[0] > 0)
      {
         GLES32.glDeleteBuffers(1, vbo_position_rectangle, 0);
         vbo_position_rectangle[0] = 0;
      }

      // free vao
      if (vao_rectangle[0] > 0)
      {
         GLES32.glDeleteVertexArrays(1, vao_rectangle, 0);
         vao_rectangle[0] = 0;
      }

      // free vbo postion
      if (vbo_color_triangle[0] > 0)
      {
         GLES32.glDeleteBuffers(1, vbo_color_triangle, 0);
         vbo_color_triangle[0] = 0;
      }

      // free vbo postion
      if (vbo_position_triangle[0] > 0)
      {
         GLES32.glDeleteBuffers(1, vbo_position_triangle, 0);
         vbo_position_triangle[0] = 0;
      }

      // free vao
      if (vao_triangle[0] > 0)
      {
         GLES32.glDeleteVertexArrays(1, vao_triangle, 0);
         vao_triangle[0] = 0;
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
