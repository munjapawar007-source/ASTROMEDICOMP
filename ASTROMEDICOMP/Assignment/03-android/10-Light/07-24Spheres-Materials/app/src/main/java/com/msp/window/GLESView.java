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

   private float perspectiveProjectionMatrix[] = new float[16];

   
   private int modelMatrixUniform = 0;
   private int viewMatrixUniform = 0;
   private int projectionMatrixUniform = 0;

   // Light Related veriable
   private int laUniform = 0;
   private int ldUniform = 0; 
   private int lsUniform = 0; 

   private int kaUniform;
   private int kdUniform; 
   private int ksUniform;

   private int materialShininaseUniform;
   private int lightPositionUniform = 0;
   private int LKeyPressedUniform = 0;

   private float[] lightAmbient = {0.0f, 0.0f, 0.0f, 1.0f};
   private float[] lightSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
   private float[] lightPosition = {100.0f, 100.0f, 100.0f, 1.0f};
   private float[] lightDiffuse = {1.0f, 1.0f, 1.0f, 1.0f};
   
   private class Material
   {
      float[] materialAmbient  = new float[4];
      float[] materialDiffuse  = new float[4];
      float[] materialSpecular = new float[4];
      float materialShininnes;
   };

   private Material[] material = new Material[24];

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
      modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uModelMatrix");
      viewMatrixUniform  = GLES32.glGetUniformLocation(shaderProgramObject, "uViewMatrix");
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
      float viewMatrix[] = new float[16];
      Matrix.setIdentityM(viewMatrix,0);

      float translationMatrix[] =  new float[16];
      Matrix.setIdentityM(translationMatrix, 0);
      Matrix.translateM(translationMatrix, 0, 2.0f,-0.2f,-3.0f);
      viewMatrix = translationMatrix;
     
      //send this matrix to the vertex shader to the uniform
	   GLES32.glUniformMatrix4fv(viewMatrixUniform, 1, false, viewMatrix, 0);
	   GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);

      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(laUniform, 1, lightAmbient, 0);
         GLES32.glUniform3fv(ldUniform, 1, lightDiffuse, 0);
         GLES32.glUniform3fv(lsUniform, 1, lightSpecular, 0);
         GLES32.glUniform4fv(lightPositionUniform, 1, lightPosition, 0);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }

      sphereMaterial();

      // bind vao
      GLES32.glBindVertexArray(vao_sphere[0]);
      
      // *** draw, either by glDrawTriangles() or glDrawArrays() or glDrawElements()
      GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
    
      sphereUniform();
    
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

   private void sphereMaterial()
   {
      material[0] = new Material();
      material[0].materialAmbient = new float[]{0.0215f, 0.1745f, 0.0215f, 1.0f};
      material[0].materialDiffuse = new float[]{0.07568f, 0.61424f, 0.07568f, 1.0f};
      material[0].materialSpecular = new float[]{0.633f, 0.727811f, 0.633f, 1.0f};
      material[0].materialShininnes  = 0.6f * 128.0f;

      material[1] = new Material();
      material[1].materialAmbient = new float[]{0.135f, 0.2225f, 0.1575f, 1.0f};
      material[1].materialDiffuse = new float[]{0.54f, 0.89f, 0.63f, 1.0f};
      material[1].materialSpecular = new float[]{0.316228f, 0.316228f, 0.316228f, 1.0f};
      material[1].materialShininnes  = 0.1f * 128.0f;
      
      material[2] = new Material();
      material[2].materialAmbient = new float[]{0.05375f, 0.05f, 0.06625f, 1.0f};
      material[2].materialDiffuse = new float[]{0.18275f, 0.17f, 0.22525f, 1.0f};
      material[2].materialSpecular = new float[]{0.332741f, 0.328634f, 0.346435f, 1.0f};
      material[2].materialShininnes  = 0.3f * 128.0f;
      
      material[3] = new Material();
      material[3].materialAmbient = new float[]{0.25f, 0.20725f, 0.20725f, 1.0f};
      material[3].materialDiffuse = new float[]{1.0f, 0.829f, 0.829f, 1.0f};
      material[3].materialSpecular = new float[]{0.296648f, 0.296648f, 0.296648f, 1.0f};
      material[3].materialShininnes  = 0.088f * 128.0f;
      
      material[4] = new Material();
      material[4].materialAmbient = new float[]{0.1745f, 0.01175f, 0.01175f, 1.0f};
      material[4].materialDiffuse = new float[]{0.61424f, 0.04136f, 0.04136f, 1.0f};
      material[4].materialSpecular = new float[]{0.727811f, 0.626959f, 0.626959f, 1.0f};
      material[4].materialShininnes  = 0.6f * 128.0f;
     
      material[5] = new Material();
      material[5].materialAmbient = new float[]{0.1f, 0.18725f, 0.1745f, 1.0f};
      material[5].materialDiffuse = new float[]{0.396f, 0.74151f, 0.69102f, 1.0f};
      material[5].materialSpecular = new float[]{0.297254f, 0.30829f, 0.306678f, 1.0f};
      material[5].materialShininnes  = 0.1f * 128.0f;
      
      material[6] = new Material();
      material[6].materialAmbient = new float[]{0.329412f, 0.223529f, 0.027451f, 1.0f};
      material[6].materialDiffuse = new float[]{0.780392f, 0.568627f, 0.113725f, 1.0f};
      material[6].materialSpecular = new float[]{0.992157f, 0.941176f, 0.807843f, 1.0f};
      material[6].materialShininnes  = 0.21794872f * 128.0f;
      
      material[7] = new Material();
      material[7].materialAmbient = new float[]{0.2125f, 0.1275f, 0.054f, 1.0f};
      material[7].materialDiffuse = new float[]{0.714f, 0.4284f, 0.18144f, 1.0f};
      material[7].materialSpecular = new float[]{0.393548f, 0.271906f, 0.166721f, 1.0f};
      material[7].materialShininnes  = 0.6f * 128.0f;
      
      material[8] = new Material();
      material[8].materialAmbient = new float[]{0.25f, 0.25f, 0.25f, 1.0f};
      material[8].materialDiffuse = new float[]{0.4f, 0.4f, 0.4f, 1.0f};
      material[8].materialSpecular = new float[]{0.4f, 0.4f, 0.4f, 1.0f};
      material[8].materialShininnes  = 0.6f * 128.0f;
      
      material[9] = new Material();
      material[9].materialAmbient = new float[]{0.19125f, 0.0735f, 0.0225f, 1.0f};
      material[9].materialDiffuse = new float[]{0.7038f, 0.27048f, 0.0828f, 1.0f};
      material[9].materialSpecular = new float[]{0.256777f, 0.137622f, 0.086014f, 1.0f};
      material[9].materialShininnes  = 0.1f * 128.0f;
      
      material[10] = new Material();
      material[10].materialAmbient = new float[]{0.24725f, 0.1995f, 0.0745f, 1.0f};
      material[10].materialDiffuse = new float[]{0.75164f, 0.60648f, 0.22648f, 1.0f};
      material[10].materialSpecular = new float[]{0.628281f, 0.555802f, 0.366065f, 1.0f};
      material[10].materialShininnes  = 0.4f * 128.0f;
      
      material[11] = new Material();
      material[11].materialAmbient = new float[]{0.19225f, 0.19225f, 0.19225f, 1.0f};
      material[11].materialDiffuse = new float[]{0.50754f, 0.50754f, 0.50754f, 1.0f};
      material[11].materialSpecular = new float[]{0.508273f, 0.508273f, 0.508273f, 1.0f};
      material[11].materialShininnes  = 0.4f * 128.0f;
      
      material[12] = new Material();
      material[12].materialAmbient = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
      material[12].materialDiffuse = new float[]{0.01f, 0.01f, 0.01f, 1.0f};
      material[12].materialSpecular = new float[]{0.50f, 0.50f, 0.50f, 1.0f};
      material[12].materialShininnes  = 0.25f * 128.0f;
      
      material[13] = new Material();
      material[13].materialAmbient = new float[]{0.0f, 0.1f, 0.06f, 1.0f};
      material[13].materialDiffuse = new float[]{0.0f, 0.50980392f, 0.50980392f, 1.0f};
      material[13].materialSpecular = new float[]{0.50196078f, 0.50196078f, 0.50196078f, 1.0f};
      material[13].materialShininnes  = 0.25f * 128.0f;
      
      material[14] = new Material();
      material[14].materialAmbient = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
      material[14].materialDiffuse = new float[]{0.01f, 0.35f, 0.01f, 1.0f};
      material[14].materialSpecular = new float[]{0.45f, 0.55f, 0.45f, 1.0f};
      material[14].materialShininnes  = 0.25f * 128.0f;
      
      material[15] = new Material();
      material[15].materialAmbient = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
      material[15].materialDiffuse = new float[]{0.5f, 0.0f, 0.0f, 1.0f};
      material[15].materialSpecular = new float[]{0.7f, 0.6f, 0.6f, 1.0f};
      material[15].materialShininnes  = 0.25f * 128.0f;
      
      material[16] = new Material();
      material[16].materialAmbient = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
      material[16].materialDiffuse = new float[]{0.55f, 0.55f, 0.55f, 1.0f};
      material[16].materialSpecular = new float[]{0.70f, 0.70f, 0.70f, 1.0f};
      material[16].materialShininnes  = 0.25f * 128.0f;
      
      material[17] = new Material();
      material[17].materialAmbient = new float[]{0.0f, 0.0f, 0.0f, 1.0f};
      material[17].materialDiffuse = new float[]{0.5f, 0.5f, 0.0f, 1.0f};
      material[17].materialSpecular = new float[]{0.60f, 0.60f, 0.50f, 1.0f};
      material[17].materialShininnes  = 0.25f * 128.0f;
      
      material[18] = new Material();
      material[18].materialAmbient = new float[]{0.02f, 0.02f, 0.02f, 1.0f};
      material[18].materialDiffuse = new float[]{0.01f, 0.01f, 0.01f, 1.0f};
      material[18].materialSpecular = new float[]{0.4f, 0.4f, 0.4f, 1.0f};
      material[18].materialShininnes  = 0.078125f * 128.0f;
      
      material[19] = new Material();
      material[19].materialAmbient = new float[]{0.0f, 0.05f, 0.05f, 1.0f};
      material[19].materialDiffuse = new float[]{0.4f, 0.5f, 0.5f, 1.0f};
      material[19].materialSpecular = new float[]{0.04f, 0.7f, 0.7f, 1.0f};
      material[19].materialShininnes  = 0.078125f * 128.0f;
      
      material[20] = new Material();
      material[20].materialAmbient = new float[]{0.0f, 0.05f, 0.0f, 1.0f};
      material[20].materialDiffuse = new float[]{0.4f, 0.5f, 0.4f, 1.0f};
      material[20].materialSpecular = new float[]{0.04f, 0.7f, 0.04f, 1.0f};
      material[20].materialShininnes  = 0.078125f * 128.0f;
      
      material[21] = new Material();
      material[21].materialAmbient = new float[]{0.05f, 0.0f, 0.0f, 1.0f};
      material[21].materialDiffuse = new float[]{0.5f, 0.4f, 0.4f, 1.0f};
      material[21].materialSpecular = new float[]{0.7f, 0.04f, 0.04f, 1.0f};
      material[21].materialShininnes  = 0.078125f * 128.0f;
      
      material[22] = new Material();
      material[22].materialAmbient = new float[]{0.05f, 0.05f, 0.05f, 1.0f};
      material[22].materialDiffuse = new float[]{0.5f, 0.5f, 0.5f, 1.0f};
      material[22].materialSpecular = new float[]{0.7f, 0.7f, 0.7f, 1.0f};
      material[22].materialShininnes  = 0.078125f * 128.0f;
      
      material[23] = new Material();
      material[23].materialAmbient = new float[]{0.05f, 0.05f, 0.0f, 1.0f};
      material[23].materialDiffuse = new float[]{0.5f, 0.5f, 0.4f, 1.0f};
      material[23].materialSpecular = new float[]{0.7f, 0.7f, 0.04f, 1.0f};
      material[23].materialShininnes  = 0.078125f * 128.0f;
      
   }

   private void sphereUniform()
   {  
      float modelMatrix[] = new float[16];
      float translationMatrix[] =  new float[16];
      Matrix.setIdentityM(translationMatrix,0);
      // 1
       Matrix.setIdentityM(modelMatrix,0);
       Matrix.translateM(translationMatrix, 0, -5.5f, 3.5f,-6.0f);
       modelMatrix = translationMatrix;
       GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[0].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[0].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[0].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[0].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      // 2
      Matrix.setIdentityM(translationMatrix,0);
      Matrix.translateM(translationMatrix, 0, -5.5f,2.30f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kdUniform, 1, material[1].materialDiffuse, 0);
         GLES32.glUniform3fv(kaUniform, 1, material[1].materialAmbient, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[1].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[1].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      // 3
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -5.5f, 1.10f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[2].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[2].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[2].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[2].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //4
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -5.50f, -0.1f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[3].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[3].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[3].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[3].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //5
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -5.50f, -1.3f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[4].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[4].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[4].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[4].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //6
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -5.50f, -2.5f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[5].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[5].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[5].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[5].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
   ////////////////////////////////////////////////////////////////////////////////
      
   //7
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -3.5f,3.5f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[6].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[6].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[6].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[6].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //8
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -3.5f, 2.3f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[7].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[7].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[7].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[7].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //9
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -3.50f, 1.1f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[8].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[8].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[8].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[8].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //10
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -3.50f, -0.10f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[9].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[9].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[9].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[9].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //11
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -3.50f, -1.30f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[10].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[10].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[10].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[10].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //12
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -3.50f, -2.50f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[11].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[11].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[11].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[11].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

   ////////////////////////////////////////////////////////////////////////////////

      //13
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -1.5f, 3.5f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[12].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[12].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[12].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[12].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //14
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -1.5f, 2.30f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[13].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[13].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[13].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[13].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //15
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -1.50f, 1.10f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[14].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[14].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[14].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[14].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //16
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -1.5f, -0.10f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[15].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[15].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[15].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[15].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //17
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -1.5f, -1.30f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[16].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[16].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[16].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[16].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //18
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, -1.5f, -2.50f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[17].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[17].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[17].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[17].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      ////////////////////////////////////////////////////////////////////////////////

      //19
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, 0.5f, 3.5f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[18].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[18].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[18].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[18].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //20
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, 0.50f, 2.30f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[19].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[19].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[19].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[19].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //21
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, 0.50f, 1.1f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[20].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[20].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[20].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[20].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //22
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, 0.50f, -0.10f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[21].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[21].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[21].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[21].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //23
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, 0.50f, -1.30f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[22].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[22].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[22].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[22].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);

      //24
      Matrix.setIdentityM(modelMatrix,0);
      Matrix.translateM(translationMatrix, 0, 0.50f, -2.5f,-6.0f);
      modelMatrix = translationMatrix;
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      if (doubleTap == 1)
      {
         GLES32.glUniform3fv(kaUniform, 1, material[23].materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform, 1, material[23].materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform, 1, material[23].materialSpecular, 0);
         GLES32.glUniform1f(materialShininaseUniform, material[23].materialShininnes);
         GLES32.glUniform1i(LKeyPressedUniform, 1);
      }
      else
      {

         GLES32.glUniform1i(LKeyPressedUniform, 0);
      }
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
   }



}
