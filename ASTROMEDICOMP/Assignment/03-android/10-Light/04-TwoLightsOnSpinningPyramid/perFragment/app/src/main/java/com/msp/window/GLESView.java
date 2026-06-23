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

   // files class veriable
   private Context context;
   private GestureDetector gestureDetector;
   private int shaderProgramObject;

   private int vao[] = new int[1];
   private int vbo_position[] = new int[1];
   private int vbo_normal[] = new int[1];
  
   // matrix uniform
   private float perspectiveProjectionMatrix[] = new float[16];
   private int modelMatrixUniform;
   private int viewMatrixUniform;
   private int projectionMatrixUniform;

   // Light Related variables
   private int[] laUniform = new int[2];
   private int[] ldUniform = new int[2]; 
   private int[] lsUniform = new int[2];

   private int kaUniform = 0;
   private int kdUniform = 0; 
   private int ksUniform = 0;

   private int materialShininaseUniform = 0;
   private int lightPositionUniform[]   = new int[2];
   private int LKeyPressedUniform       = 0;

   private float[] materialAmbient    = {0.0f, 0.0f, 0.0f, 1.0f};
   private float[] materialDiffuse    = {0.50f, 0.50f, 0.50f, 1.0f};
   private float[] materialSpecular   = {1.0f, 1.0f, 1.0f, 1.0f};
   private float   materialShininnes  = 50.0f;

   private class Light
   {
      float[] ambient   = new float[4];
      float[] diffuse   = new float[4];
      float[] specular  = new float[4];
      float[] position  = new float[4];
   };
   private Light[] light = new Light[2];

   // globle veriables
   private float angle_MP;
   private int doubleTapLight;
   private int singleTapAnimation;

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
      doubleTapLight++;
      if(doubleTapLight > 1)
         doubleTapLight = 0;
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
      singleTapAnimation++;
      if(singleTapAnimation > 1)
         singleTapAnimation = 0;
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
         "in vec3 aNormal;\n" +
         "uniform mat4 uModelMatrix;\n" +
         "uniform mat4 uViewMatrix;\n" +
         "uniform mat4 uProjectionMatrix;\n" +
         "out vec3 out_transformedNormals;\n" +
         "out vec3 out_viwerVector;\n" +
         "out vec3 out_lightDirection[2];\n" +
         "uniform vec4 uLightPosition[2];\n" +
         "uniform int uLkeyPressed;\n" +
         "void main(void)\n" +
         "{gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" + 
         "if(uLkeyPressed == 1)\n" +
         "{\n" +
         "vec4 eyeCoordinate = uViewMatrix * uModelMatrix * aPosition;\n" +
         "mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" +
         "out_transformedNormals = normalMatrix * aNormal;\n" +
         "for(int i=0;i<2;i++)\n" +
         "{\n" +
         "out_lightDirection[i] = vec3(uLightPosition[i] - eyeCoordinate);\n" +
         "out_viwerVector = -eyeCoordinate.xyz;\n" +
         "}\n" +
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
            System.out.println("vertex Shader Compilation log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }
      }

      // fragmant shader
      final String fragmentShaderSourceCode = String.format
      (
         "#version 320 es\n" +
         "precision highp float;" +
         "in vec3 out_transformedNormals;\n" +
         "in vec3 out_viwerVector;\n" +
         "in vec3 out_lightDirection[2];\n" +
         "out vec4 FragColor;\n" +
         "uniform vec3 uLa[2];\n" +
         "uniform vec3 uLd[2];\n" +
         "uniform vec3 uLs[2];\n" +
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
         "vec3 normalizedLightDirection[3];\n" +
         "vec3 ambientLight[2];\n" +
         "vec3 diffuseLight[2];\n" +
         "vec3 reflectionVector[2];\n" +
         "vec3 specularLight[2];\n" +
         "vec3 normalizedTransformedNormal =  normalize(out_transformedNormals);\n" +
         "vec3 normalizedViwerVector = normalize(out_viwerVector);\n" +
         "for(int i=0;i<2;i++)\n" +
         "{\n" +
         "normalizedLightDirection[i] = normalize(out_lightDirection[i]);\n" +
         "ambientLight[i] = uLa[i] * uKa;\n" +
         "diffuseLight[i] = uLd[i] * uKd * max(dot(normalizedLightDirection[i],normalizedTransformedNormal),0.0);\n" +
         "reflectionVector[i] = reflect(-normalizedLightDirection[i],normalizedTransformedNormal);\n" +
         "specularLight[i] = uLs[i] * uKs * pow(max(dot(reflectionVector[i],normalizedViwerVector), 0.0),uMaterialShinines);\n" +
         "phong_ads_Light = phong_ads_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];\n" +
         "}\n" +
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
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributes.MSP_ATTRIBUTE_VERTEX, "aPosition");
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributes.MSP_ATTRIBUTE_NORMAL, "aNormal");

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
      modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uModelMatrix");
      viewMatrixUniform  = GLES32.glGetUniformLocation(shaderProgramObject, "uViewMatrix");
      projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
      
      laUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uLa[0]");
      ldUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uLd[0]");
      lsUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uLs[0]");
      lightPositionUniform[0] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightPosition[0]");

      laUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "uLa[1]");
      ldUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "uLd[1]");
      lsUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "uLs[1]");
      lightPositionUniform[1] = GLES32.glGetUniformLocation(shaderProgramObject, "uLightPosition[1]");

      kaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKa");
      kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKd");
      ksUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKs");
      materialShininaseUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialShinines");
      LKeyPressedUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLkeyPressed");

      // provide vertex position,color,texcode, and normal etc.
      final float pyramid_position[] = new float[] 
      {
         // front
         0.0f,  1.0f, 0.0f,	 // front-top
        -1.0f, -1.0f, 1.0f,    // front-left
         1.0f, -1.0f, 1.0f,	 // front-right

        // right
        0.0f,  1.0f,  0.0f,	  // right-top
        1.0f, -1.0f,  1.0f,	  // right-left
        1.0f, -1.0f, -1.0f,     // right-right

        // back
        0.0f,   1.0f,   0.0f,	  // back-top
        1.0f,  -1.0f,  -1.0f,	  // back-left
       -1.0f,  -1.0f,  -1.0f,   // back-right

        // left
         0.0f,  1.0f,  0.0f,	 // left-top
        -1.0f, -1.0f, -1.0f,   // left-left
        -1.0f, -1.0f,  1.0f,   // left-right
      };

     final float pyramid_normal[] = new float[]
     {
         
      // front
      0.000000f, 0.447214f, 0.894427f, // front-top
      0.000000f, 0.447214f, 0.894427f, // front-left
      0.000000f, 0.447214f, 0.894427f, // front-right

      // right
      0.894427f, 0.447214f, 0.000000f, // right-top
      0.894427f, 0.447214f, 0.000000f, // right-left
      0.894427f, 0.447214f, 0.000000f, // right-right

      // back
      0.000000f, 0.447214f, -0.894427f, // back-top
      0.000000f, 0.447214f, -0.894427f, // back-left
      0.000000f, 0.447214f, -0.894427f, // back-right

      // left
      -0.894427f, 0.447214f, 0.000000f, // left-top
      -0.894427f, 0.447214f, 0.000000f, // left-left
      -0.894427f, 0.447214f, 0.000000f, // left-right
   
	  };

     
      
      GLES32.glGenVertexArrays(1, vao, 0);
      GLES32.glBindVertexArray(vao[0]);

      // Position
         GLES32.glGenBuffers(1, vbo_position,0);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_position[0]);

         // create a native buffer suitable for native IO but for java
         // 1) set buffer size (36 byte ) 
            ByteBuffer byteBuffer = ByteBuffer.allocateDirect(pyramid_position.length * 4);
         // 2) set order left to right OR right to left
            byteBuffer.order(ByteOrder.nativeOrder());
         // 3) create which type is data
            FloatBuffer pyramidPositionBuffer = byteBuffer.asFloatBuffer();
         // 4) fill array of traingle position
            pyramidPositionBuffer.put( pyramid_position);
         // 5)  set array to 0th position for repeatable use
            pyramidPositionBuffer.position(0);
         GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramid_position.length * 4, pyramidPositionBuffer, GLES32.GL_STATIC_DRAW);
      
         GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_VERTEX, 3, GLES32.GL_FLOAT, false, 0, 0);
         GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_VERTEX);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      // Normal
         GLES32.glGenBuffers(1, vbo_normal,0);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_normal[0]);
         // create a native buffer suitable for native IO but for java
         // 1) set buffer size (36 byte ) 
            byteBuffer = ByteBuffer.allocateDirect(pyramid_normal.length * 4);
         // 2) set order left to right OR right to left
            byteBuffer.order(ByteOrder.nativeOrder());
         // 3) create which type is data
            FloatBuffer pyramidNormalBuffer = byteBuffer.asFloatBuffer();
         // 4) fill array of traingle position
            pyramidNormalBuffer.put(pyramid_normal);
         // 5)  set array to 0th position for repeatable use
            pyramidNormalBuffer.position(0);
         GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, pyramid_normal.length * 4, pyramidNormalBuffer, GLES32.GL_STATIC_DRAW);
      
         GLES32.glVertexAttribPointer(MyAttributes.MSP_ATTRIBUTE_NORMAL, 3, GLES32.GL_FLOAT, false, 0, 0);
         GLES32.glEnableVertexAttribArray(MyAttributes.MSP_ATTRIBUTE_NORMAL);
         GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      GLES32.glBindVertexArray(0);

      // Depth initialization
      GLES32.glClearDepthf(1.0f);
      GLES32.glEnable(GLES32.GL_DEPTH_TEST);
      GLES32.glDepthFunc(GLES32.GL_LEQUAL);

      // set the clear color
      GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

      // initialize of two light
      light[0] = new Light();
      light[0].ambient   = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
      light[0].diffuse   = new float[]{1.0f, 0.0f, 0.0f, 1.0f};
      light[0].specular  = new float[]{1.0f, 0.0f, 0.0f, 1.0f};
      light[0].position  = new float[]{-2.0f, 0.0f, 0.0f, 1.0f};

      light[1] = new Light();
      light[1].ambient   = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
      light[1].diffuse   = new float[]{0.0f, 0.0f, 1.0f, 1.0f};
      light[1].specular  = new float[]{0.0f, 0.0f, 1.0f, 1.0f};
      light[1].position  = new float[]{2.0f, 0.0f, 0.0f, 1.0f};

      doubleTapLight = 0;
      singleTapAnimation = 0;

      Matrix.setIdentityM(perspectiveProjectionMatrix,0);

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
      Matrix.multiplyMM(modelMatrix,0,translationMatrix,0,rotationMatrix,0);

      // send this matrix to the vertex shader to the uniform
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
      GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);

      if (doubleTapLight == 1)
      {
         GLES32.glUniform3fv(laUniform[0], 1, light[0].ambient, 0);
         GLES32.glUniform3fv(ldUniform[0], 1, light[0].diffuse, 0);
         GLES32.glUniform3fv(lsUniform[0], 1, light[0].specular, 0);
         GLES32.glUniform4fv(lightPositionUniform[0], 1, light[0].position, 0);

         GLES32.glUniform3fv(laUniform[1], 1, light[1].ambient, 0);
         GLES32.glUniform3fv(ldUniform[1], 1, light[1].diffuse, 0);
         GLES32.glUniform3fv(lsUniform[1], 1, light[1].specular, 0);
         GLES32.glUniform4fv(lightPositionUniform[1], 1, light[1].position, 0);

         GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);

         GLES32.glUniform1f(materialShininaseUniform, materialShininnes);

         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }

      //bind with vao
	   GLES32.glBindVertexArray(vao[0]);

      //draw vertex array
	   GLES32.glDrawArrays(GLES32.GL_TRIANGLES,0, 12);

	   GLES32.glBindVertexArray(0);

      //unUse shader program
      GLES32.glUseProgram(0);


      requestRender();
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
      if(vbo_normal[0] > 0)
      {
         GLES32.glDeleteBuffers(1,vbo_normal,0);
         vbo_normal[0] = 0;
      }

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
