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
   private int shaderProgramObject_pv;
   private int shaderProgramObject_pf;
   
   private int[] vao_sphere = new int[1];
   private int[] vbo_sphere_position = new int[1];
   private int[] vbo_sphere_normal = new int[1];
   private int[] vbo_sphere_element = new int[1];

   /* ---------perVertex-------- */
      // matrix related veriables
   private float perspectiveProjectionMatrix_pv[] = new float[16];
   private int modelMatrixUniform_pv = 0;
   private int viewMatrixUniform_pv = 0;
   private int projectionMatrixUniform_pv = 0;
      // Light Related veriable
   private int laUniform_pv = 0;
   private int ldUniform_pv = 0; 
   private int lsUniform_pv = 0; 
   private int kaUniform_pv = 0;
   private int kdUniform_pv = 0; 
   private int ksUniform_pv = 0;
      // uniforms veriables
   private int materialShininaseUniform_pv;
   private int lightPositionUniform_pv;
   private int LKeyPressedUniform_pv;

   /* ---------perFragment-------- */
      // matrix related veriables
   private float perspectiveProjectionMatrix_pf[] = new float[16];
   private int modelMatrixUniform_pf = 0;
   private int viewMatrixUniform_pf = 0;
   private int projectionMatrixUniform_pf = 0;
      // Light Related veriable
   private int laUniform_pf = 0;
   private int ldUniform_pf = 0; 
   private int lsUniform_pf = 0; 
   private int kaUniform_pf = 0;
   private int kdUniform_pf = 0; 
   private int ksUniform_pf = 0;
      // uniforms veriables
   private int materialShininaseUniform_pf = 0;
   private int lightPositionUniform_pf = 0;
   private int LKeyPressedUniform_pf = 0;

   private float[] lightAmbient  = {0.10f, 0.10f, 0.10f, 1.0f};
   private float[] lightDiffuse  = {1.0f, 1.0f, 1.0f, 1.0f};
   private float[] lightSpecular = {1.0f, 1.0f, 1.0f, 1.0f};
   private float[] lightPosition = {100.0f, 100.0f, 100.0f, 1.0f};

   private float[] materialAmbient     = {0.0f, 0.0f, 0.0f, 1.0f};
   private float[] materialDiffuse     = {0.50f, 0.20f, 0.70f, 1.0f};
   private float[] materialSpecular    = {0.70f, 0.70f, 0.70f, 1.0f};
   private float   materialShininnes   = 128.0f;


   
   private int doubleTapForLight;
   private int singleTapForToggle;

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
      doubleTapForLight++;
     if(doubleTapForLight > 1)
     {
         doubleTapForLight = 0;
     }
      return true;
   }

   @Override
   public boolean onSingleTapConfirmed(MotionEvent e) 
   {

        singleTapForToggle++;
     if(singleTapForToggle > 2)
         singleTapForToggle = 0;

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

      perVertex();
      perFragment();

      Sphere sphere = new Sphere();
      float sphere_vertices[] = new float[1146];
      float sphere_normals[]  = new float[1146];
      float sphere_textures[] = new float[764];
      short sphere_elements[] = new short[2280];
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
      doubleTapForLight = 0;
      singleTapForToggle = 0;

      Matrix.setIdentityM(perspectiveProjectionMatrix_pf,0);
      Matrix.setIdentityM(perspectiveProjectionMatrix_pv,0);

      return 0;
   }

   private void perVertex()
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
         "uniform vec3 uLa;\n" +
         "uniform vec3 uLd;\n" +
         "uniform vec3 uLs;\n" +
         "uniform vec3 uKa;\n" +
         "uniform vec3 uKd;\n" +
         "uniform vec3 uKs;\n" +
         "uniform float uMaterialShinines;\n" +
         "uniform vec4 uLightPosition;\n" +
         "uniform int uLkeyPressed;\n" +
         "out vec3 out_phong_ads_Light;\n" +
         "void main(void)\n" +
         "{gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" +
         "if\n" +
         "(\n" +
         "uLkeyPressed == 1\n" +
         ")\n" +
         "{\n" +
         "vec4 eyeCoordinate = uViewMatrix * uModelMatrix * aPosition;\n" +
         "mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" +
         "vec3 transformedNormal = normalize(normalMatrix * aNormal);\n" +
         "vec3 lightDirection = normalize(vec3(uLightPosition - eyeCoordinate));\n" +
         "vec3 ambientLight = uLa * uLa;\n" +
         "vec3 diffuseLight = uLd * uKd * max(dot(lightDirection,transformedNormal),0.0);\n" +
         "vec3 reflectionVector = reflect(-lightDirection,transformedNormal);\n" +
         "vec3 viwerVector =normalize(-eyeCoordinate.xyz);\n" +
         "vec3 specularLight = uLs * uKs * pow(max(dot(reflectionVector,viwerVector), 0.0),uMaterialShinines);\n" +
         "out_phong_ads_Light = ambientLight + diffuseLight + specularLight; \n" +
         "}\n" +
         "else\n" +
         "{\n" +
         "out_phong_ads_Light = vec3(1.0,1.0,1.0);\n" +
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
         "in vec3 out_phong_ads_Light;\n" +
         "out vec4 FragColor;\n" +
         "void main(void)\n" +
         "{FragColor=vec4(out_phong_ads_Light,1.0);\n" +
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
      shaderProgramObject_pv = GLES32.glCreateProgram();
      GLES32.glAttachShader(shaderProgramObject_pv, vertexShaderObject);
      GLES32.glAttachShader(shaderProgramObject_pv, fragmentShaderObject);

      // Bind Shader Attribute at certin index in shader to same index in host program
      GLES32.glBindAttribLocation(shaderProgramObject_pv, MyAttributs.MSP_ATTRIBUTE_VERTEX, "aPosition");
      GLES32.glBindAttribLocation(shaderProgramObject_pv, MyAttributs.MSP_ATTRIBUTE_NORMAL, "aNormal");


      GLES32.glLinkProgram(shaderProgramObject_pv);
      int shaderProgramLinkStatus[] = new int[1];
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetProgramiv(shaderProgramObject_pv, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus,0);

      if (shaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
         GLES32.glGetProgramiv(shaderProgramObject_pv, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) {
            szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject_pv);
            System.out.println("msp Program info log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }

      }

      // uniforms
      modelMatrixUniform_pv      = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uModelMatrix");
      viewMatrixUniform_pv       = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uViewMatrix");
      projectionMatrixUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uProjectionMatrix");
      
      laUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLa");
      ldUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLd");
      lsUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLs");

      lightPositionUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLightPosition");

      kaUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uKa");
      kdUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uKd");
      ksUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uKs");

      materialShininaseUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uMaterialShinines");
      LKeyPressedUniform_pv = GLES32.glGetUniformLocation(shaderProgramObject_pv, "uLkeyPressed");

   }

   private void perFragment()
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
      shaderProgramObject_pf = GLES32.glCreateProgram();
      GLES32.glAttachShader(shaderProgramObject_pf, vertexShaderObject);
      GLES32.glAttachShader(shaderProgramObject_pf, fragmentShaderObject);

      // Bind Shader Attribute at certin index in shader to same index in host program
      GLES32.glBindAttribLocation(shaderProgramObject_pf, MyAttributs.MSP_ATTRIBUTE_VERTEX, "aPosition");
      GLES32.glBindAttribLocation(shaderProgramObject_pf, MyAttributs.MSP_ATTRIBUTE_NORMAL, "aNormal");


      GLES32.glLinkProgram(shaderProgramObject_pf);
      int shaderProgramLinkStatus[] = new int[1];
      infoLogLenght[0] = 0;
      szInfoLog = null;

      GLES32.glGetProgramiv(shaderProgramObject_pf, GLES32.GL_LINK_STATUS, shaderProgramLinkStatus,0);

      if (shaderProgramLinkStatus[0] == GLES32.GL_FALSE) {
         GLES32.glGetProgramiv(shaderProgramObject_pf, GLES32.GL_INFO_LOG_LENGTH, infoLogLenght, 0);

         if (infoLogLenght[0] > 0) {
            szInfoLog = GLES32.glGetProgramInfoLog(shaderProgramObject_pf);
            System.out.println("msp Program info log" + szInfoLog);
            Uninitialize();
            System.exit(0);
         }

      }

      // uniforms
      modelMatrixUniform_pf      = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uModelMatrix");
      viewMatrixUniform_pf       = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uViewMatrix");
      projectionMatrixUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uProjectionMatrix");
      
      laUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLa");
      ldUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLd");
      lsUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLs");

      lightPositionUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLightPosition");

      kaUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uKa");
      kdUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uKd");
      ksUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uKs");

      materialShininaseUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uMaterialShinines");
      LKeyPressedUniform_pf = GLES32.glGetUniformLocation(shaderProgramObject_pf, "uLkeyPressed");
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
      Matrix.perspectiveM(perspectiveProjectionMatrix_pv,0,45.0f, (float)width / (float)height,0.1f,100.0f);
      Matrix.perspectiveM(perspectiveProjectionMatrix_pf,0,45.0f, (float)width / (float)height,0.1f,100.0f);

   }

   private void Display() 
   {
      // code
      GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);

      //use Shader program object

      if(singleTapForToggle == 1)
	      GLES32.glUseProgram(shaderProgramObject_pv);

      if(singleTapForToggle == 2)
         GLES32.glUseProgram(shaderProgramObject_pf);

      //transformstions
      float modelMatrix[] = new float[16];
      Matrix.setIdentityM(modelMatrix,0);

      float viewMatrix[] = new float[16];
      Matrix.setIdentityM(viewMatrix,0);

      float translationMatrix[] =  new float[16];
      Matrix.setIdentityM(translationMatrix,0);
      Matrix.translateM(translationMatrix,0,0.0f, 0.0f, -3.0f);
      modelMatrix = translationMatrix;

       /* ---------perVertex-------- */
      //send this matrix to the vertex shader to the uniform
	   GLES32.glUniformMatrix4fv(modelMatrixUniform_pv, 1, false, modelMatrix, 0);
	   GLES32.glUniformMatrix4fv(viewMatrixUniform_pv, 1, false, viewMatrix, 0);
	   GLES32.glUniformMatrix4fv(projectionMatrixUniform_pv,1,false,perspectiveProjectionMatrix_pf,0);

      if (doubleTapForLight == 1)
      {
         GLES32.glUniform3fv(laUniform_pv, 1, lightAmbient, 0);
         GLES32.glUniform3fv(ldUniform_pv, 1, lightDiffuse, 0);
         GLES32.glUniform3fv(lsUniform_pv, 1, lightSpecular, 0);
         
         GLES32.glUniform4fv(lightPositionUniform_pv, 1, lightPosition, 0);

         GLES32.glUniform3fv(kaUniform_pv, 1, materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform_pv, 1, materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform_pv, 1, materialSpecular, 0);

         GLES32.glUniform1f(materialShininaseUniform_pv,materialShininnes);

         GLES32.glUniform1i(LKeyPressedUniform_pv, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform_pv, 0);
      }

      /* ---------perFragment-------- */
      //send this matrix to the vertex shader to the uniform
	   GLES32.glUniformMatrix4fv(modelMatrixUniform_pf, 1, false, modelMatrix, 0);
	   GLES32.glUniformMatrix4fv(viewMatrixUniform_pf, 1, false, viewMatrix, 0);
	   GLES32.glUniformMatrix4fv(projectionMatrixUniform_pf,1,false,perspectiveProjectionMatrix_pf,0);

      if (doubleTapForLight == 1)
      {
         GLES32.glUniform3fv(laUniform_pf, 1, lightAmbient, 0);
         GLES32.glUniform3fv(ldUniform_pf, 1, lightDiffuse, 0);
         GLES32.glUniform3fv(lsUniform_pf, 1, lightSpecular, 0);
         
         GLES32.glUniform4fv(lightPositionUniform_pf, 1, lightPosition, 0);

         GLES32.glUniform3fv(kaUniform_pf, 1, materialAmbient, 0);
         GLES32.glUniform3fv(kdUniform_pf, 1, materialDiffuse, 0);
         GLES32.glUniform3fv(ksUniform_pf, 1, materialSpecular, 0);

         GLES32.glUniform1f(materialShininaseUniform_pf,materialShininnes);

         GLES32.glUniform1i(LKeyPressedUniform_pf, 1);
      }
      else
      {
         GLES32.glUniform1i(LKeyPressedUniform_pf, 0);
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

      /* ---------perVertex-------- */
      //detach delete shader program object
      if(shaderProgramObject_pv > 0)
      {
         GLES32.glUseProgram(shaderProgramObject_pv);
         GLES32.glUseProgram(0);

         //return value
         int retVal[] = new int[1];

         GLES32.glGetProgramiv(shaderProgramObject_pv,GLES32.GL_ATTACHED_SHADERS,retVal,0);

         int numAttachedShaders = retVal[0];
         if(numAttachedShaders > 0)
         {
            int shaderObjects[] = new int[numAttachedShaders];
           
            GLES32.glGetAttachedShaders(shaderProgramObject_pv,numAttachedShaders,retVal,0,shaderObjects,0);

            for(int i = 0; i<numAttachedShaders;i++)
            {
               GLES32.glDetachShader(shaderProgramObject_pv,shaderObjects[i]);
               GLES32.glDeleteShader(shaderObjects[i]);
               shaderObjects[i] = 0;
            }
         }
         GLES32.glUseProgram(0);
         GLES32.glDeleteProgram(shaderProgramObject_pv);
      }

      /* ---------perFragment-------- */
      //detach delete shader program object
      if(shaderProgramObject_pf > 0)
      {
         GLES32.glUseProgram(shaderProgramObject_pf);
         GLES32.glUseProgram(0);

         //return value
         int retVal[] = new int[1];

         GLES32.glGetProgramiv(shaderProgramObject_pf,GLES32.GL_ATTACHED_SHADERS,retVal,0);

         int numAttachedShaders = retVal[0];
         if(numAttachedShaders > 0)
         {
            int shaderObjects[] = new int[numAttachedShaders];
           
            GLES32.glGetAttachedShaders(shaderProgramObject_pf,numAttachedShaders,retVal,0,shaderObjects,0);

            for(int i = 0; i<numAttachedShaders;i++)
            {
               GLES32.glDetachShader(shaderProgramObject_pf,shaderObjects[i]);
               GLES32.glDeleteShader(shaderObjects[i]);
               shaderObjects[i] = 0;
            }
         }
         GLES32.glUseProgram(0);
         GLES32.glDeleteProgram(shaderProgramObject_pf);
      }

   }

}
