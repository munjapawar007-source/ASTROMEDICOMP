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
import java.lang.Math;  

import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32;

//events related pakages
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;

// sphere related
import java.nio.ShortBuffer;


public class GLESView extends GLSurfaceView
      implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener 
{

   // filed/class veriable
   private Context context;
   private GestureDetector gestureDetector;
   private int shaderProgramObject;
   
   private int[] vao_sphere = new int[1];
   private int[] vbo_sphere_position = new int[1];
   private int[] vbo_sphere_normal = new int[1];
   private int[] vbo_sphere_element = new int[1];

   // matrix related veriables
   private float perspectiveProjectionMatrix[] = new float[16];
   private int modelMatrixUniform = 0;
   private int viewMatrixUniform = 0;
   private int projectionMatrixUniform = 0;

   // Light Related veriable
   private int laUniform = 0;
   private int ldUniform = 0; 
   private int lsUniform = 0; 

   private int kaUniform = 0;
   private int kdUniform = 0; 
   private int ksUniform = 0;

   private float[] lightAmbient  = {0.10f, 0.10f, 0.10f, 1.0f};
   private float[] lightDiffuse  = {1.0f, 1.0f, 1.0f, 1.0f};
   private float[] lightSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
   private float[] lightPosition = {100.0f, 100.0f, 100.0f, 1.0f};

   private float[] materialAmbient     = {0.0f, 0.0f, 0.0f, 1.0f};
   private float[] materialDiffuse     = {0.50f, 0.20f, 0.70f, 1.0f};
   private float[] materialSpecular    = {0.70f, 0.70f, 0.70f, 1.0f};
   private float   materialShininnes   = 128.0f;

   // uniforms veriables
   private int materialShininaseUniform = 0;
   private int lightPositionUniform = 0;
   private int LKeyPressedUniform = 0;
   
   private int doubleTap;

   // sphere related
   private int maxElements;
   private int numElements;
   private int numVertices;

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
      doubleTap++;
     if(doubleTap > 1)
     {
      doubleTap = 0;
     }
      return true;
   }

   @Override
   public boolean onSingleTapConfirmed(MotionEvent e) 
   {

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
         "out vec3 out_lightDirection;\n" +
         "uniform vec4 uLightPosition;\n" +
         "uniform bool uLkeyPressed;\n" +
         "void main(void)\n" +
         "{gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" +
         "if\n" +
         "(\n" +
         "uLkeyPressed == true\n" +
         ")\n" +
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
         "in vec3 out_lightDirection;\n" +
         "out vec4 FragColor;\n" +
         "uniform vec3 uLa;\n" +
         "uniform vec3 uLd;\n" +
         "uniform vec3 uLs;\n" +
         "uniform vec3 uKa;\n" +
         "uniform vec3 uKd;\n" +
         "uniform vec3 uKs;\n" +
         "uniform float uMaterialShinines;\n" +
         "uniform bool uLkeyPressed;\n" +
         "void main(void)\n" +
         "{\n" +
         "vec3 phong_ads_Light;\n" +
         "if\n" +
         "(\n" +
         "uLkeyPressed == true\n" +
         ")\n" +
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
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributs.MSP_ATTRIBUTE_VERTEX, "aPosition");
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributs.MSP_ATTRIBUTE_NORMAL, "aNormal");


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
      modelMatrixUniform      = GLES32.glGetUniformLocation(shaderProgramObject, "uModelMatrix");
      viewMatrixUniform       = GLES32.glGetUniformLocation(shaderProgramObject, "uViewMatrix");
      projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
      
      laUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLa");
      ldUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLd");
      lsUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLs");

      lightPositionUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLightPosition");

      kaUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKa");
      kdUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKd");
      ksUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uKs");

      materialShininaseUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialShinines");
      LKeyPressedUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uLkeyPressed");

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
      
      GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER,0);

      GLES32.glBindVertexArray(0);

      // Depth initialization
      GLES32.glClearDepthf(1.0f);
      GLES32.glEnable(GLES32.GL_DEPTH_TEST);
      GLES32.glDepthFunc(GLES32.GL_LEQUAL);

      // set the clear color
      GLES32.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
     
      // for light 
      doubleTap = 0;
      
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
      Matrix.setIdentityM(modelMatrix,0);

      float viewMatrix[] = new float[16];
      Matrix.setIdentityM(viewMatrix,0);

      float translationMatrix[] =  new float[16];
      Matrix.setIdentityM(translationMatrix,0);
      Matrix.translateM(translationMatrix,0,0.0f, 0.0f, -3.0f);

      modelMatrix = translationMatrix;

      //send this matrix to the vertex shader to the uniform
	   GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
	   GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
	   GLES32.glUniformMatrix4fv(projectionMatrixUniform,1,false,perspectiveProjectionMatrix,0);

      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(laUniform, 1, lightAmbient, 0);
         GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
         GLES32.glUniform3fv(lsUniform, 1, lightSpecular, 0);
         
         GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);

         GLES32.glUniform3fv(kaUniform, 1, materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, materialSpecular, 0);

         GLES32.glUniform1f(materialShininaseUniform,materialShininnes);

         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }

      // bind vao
      GLES32.glBindVertexArray(vao_sphere[0]);
      
      // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
      GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
      
      // unbind vao
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
      // destroy vao
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
