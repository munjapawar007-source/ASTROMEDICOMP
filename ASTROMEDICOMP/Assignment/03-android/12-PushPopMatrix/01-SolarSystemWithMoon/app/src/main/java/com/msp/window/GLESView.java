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

// sphere related
import java.nio.ShortBuffer;
import java.util.Stack;

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

   private int materialColorUniform;

   // sphere related variables
   private int maxElements;
   private int numElements;
   private int numVertices;

   //Solar System variables
   private float year;
   private float date;

   // satack releated variable
   Stack<float[]> stack = new Stack<>();
   
   private float[] currentMatrix = new float[16];

   private void pushMatrix()
   {
      stack.push(currentMatrix);    // add element
   }

   private void popMatrix()
   {
      if (!stack.isEmpty())                // check stack is empty or not
      {
         currentMatrix = stack.peek();    // show top element (not remove)
         stack.pop();                     // remove top element 
      }
   }

   private int singleTap;
   private int doubleTap;


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
      doubleTap++;
      if(doubleTap > 2)
      {
         doubleTap = 0;
      }
      
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
      if(singleTap > 2)
      {
         singleTap = 0;
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
         "uniform mat4 uModelMatrix;\n" +
         "uniform mat4 uViewMatrix;\n" +
         "uniform mat4 uProjectionMatrix;\n" +
         "void main(void)\n" +
         "{\n" +
         "   gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" +
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
         "uniform vec3 uMaterialColor;\n" +
         "out vec4 FragColor;\n" +
         "void main(void)\n" +
         "{\n" +
         "FragColor = vec4(uMaterialColor, 1.0f);\n" +
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
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributs.MSP_ATTRIBUTE_COLOR, "aColor");


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

     // get the required uniform loction from the sheder
    modelMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uModelMatrix");
    viewMatrixUniform =  GLES32.glGetUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uProjectionMatrix");
    materialColorUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uMaterialColor");

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

      year = 0.0f;
      date = 0.0f;

      singleTap = 0;
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
      Matrix.setIdentityM(currentMatrix,0);

      float viewMatrix[] = new float[16];
      Matrix.setIdentityM(viewMatrix,0);

      float modelMatrix[] =  new float[16];
      Matrix.setIdentityM(modelMatrix,0);

      float translationMatrix[] =  new float[16];
      Matrix.setIdentityM(translationMatrix,0);

      float scaleMatrix[] =  new float[16];
      Matrix.setIdentityM(scaleMatrix,0);

      float rotationMatrix[] =  new float[16];
      Matrix.setIdentityM(rotationMatrix,0);
     // Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -3.0f);

   
      // Set the camera position (View matrix)
      Matrix.setLookAtM(viewMatrix, 0, 0.0f, 0.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0f, 1.0f, 0.0f);

      //send this matrix to the vertex shader to the uniform
	   GLES32.glUniformMatrix4fv(viewMatrixUniform,1,false,viewMatrix,0);
	   GLES32.glUniformMatrix4fv(projectionMatrixUniform, 1, false, perspectiveProjectionMatrix, 0);

      // bind vao
      GLES32.glBindVertexArray(vao_sphere[0]);
      GLES32.glBindBuffer(GLES32.GL_ELEMENT_ARRAY_BUFFER, vbo_sphere_element[0]);
      
      pushMatrix();

      // SUN
      //adjust the poll of sphers of sun
      Matrix.rotateM(rotationMatrix, 0, 90.0f, 0.0f, 1.0f, 0.0f);
      Matrix.translateM(translationMatrix, 0, 0.0f, 0.0f, -0.50f);
      Matrix.multiplyMM(currentMatrix, 0, translationMatrix, 0, rotationMatrix, 0);
  
      Matrix.scaleM(scaleMatrix, 0, 2.0f, 2.0f, 2.0f);
      Matrix.multiplyMM(modelMatrix, 0, currentMatrix, 0, scaleMatrix, 0);
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      GLES32.glUniform3f(materialColorUniform, 1.0f, 1.0f, 0.0f);
      
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
      
      popMatrix();

      // Earth
      pushMatrix();
      Matrix.rotateM(rotationMatrix, 0, year, 0.0f, 1.0f, 0.0f);
      Matrix.multiplyMM(currentMatrix, 0, currentMatrix, 0, rotationMatrix, 0);

      Matrix.translateM(translationMatrix, 0, -1.5f, -0.10f, 0.0f);
      Matrix.multiplyMM(currentMatrix, 0, currentMatrix, 0, translationMatrix, 0);
      Matrix.rotateM(rotationMatrix, 0, 90.0f, 0.0f, 1.0f, 0.0f);
      Matrix.multiplyMM(currentMatrix, 0, currentMatrix, 0, rotationMatrix, 0);

      Matrix.rotateM(rotationMatrix, 0, date, 0.0f, 1.0f, 0.0f);
      Matrix.multiplyMM(currentMatrix, 0, currentMatrix, 0, rotationMatrix, 0);

      Matrix.scaleM(scaleMatrix, 0, 0.60f, 0.6f, 0.60f);
      Matrix.multiplyMM(modelMatrix, 0, currentMatrix, 0, scaleMatrix, 0);
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      GLES32.glUniform3f(materialColorUniform, 0.4f,0.7f,1.0f);
      
      GLES32.glDrawElements(GLES32.GL_LINES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
      popMatrix();

      // Moon
      pushMatrix();
      Matrix.rotateM(rotationMatrix, 0, year, 0.0f, 1.0f, 0.0f);
      Matrix.multiplyMM(currentMatrix, 0, currentMatrix, 0, rotationMatrix, 0);

      Matrix.translateM(translationMatrix, 0, 2.0f,0.20f,0.0f);
      Matrix.multiplyMM(currentMatrix, 0, currentMatrix, 0, translationMatrix, 0);
      Matrix.rotateM(rotationMatrix, 0, 90.0f, 0.0f, 1.0f, 0.0f);
      Matrix.multiplyMM(currentMatrix, 0, currentMatrix, 0, rotationMatrix, 0);

      Matrix.rotateM(rotationMatrix, 0, date, 0.0f, 1.0f, 0.0f);
      Matrix.multiplyMM(currentMatrix, 0, currentMatrix, 0, rotationMatrix, 0);

      Matrix.scaleM(scaleMatrix, 0, 0.19f, 0.20f, 0.19f);
      Matrix.multiplyMM(modelMatrix, 0, currentMatrix, 0, scaleMatrix, 0);
      GLES32.glUniformMatrix4fv(modelMatrixUniform, 1, false, modelMatrix, 0);
      GLES32.glUniform3f(materialColorUniform, 1.0f,0.9f,1.0f);
      
      GLES32.glDrawElements(GLES32.GL_TRIANGLES, numElements, GLES32.GL_UNSIGNED_SHORT, 0);
      popMatrix();
      
      // unbind vao
      GLES32.glBindVertexArray(0);

      //unUse shader program
      GLES32.glUseProgram(0);


      requestRender();
   }

   private void Update() 
   {
      // code
      
      if (singleTap == 1)
      {
         year =(year + 0.5f) % 360.0f;
      }
      if (singleTap == 2)
      {
         year =(year - 0.5f) % 360.0f;
      }

      if(doubleTap == 1)
         date =(date + 0.5f) % 360.0f;

      if(doubleTap == 2)
         date =(date - 0.5f) % 360.0f; 
      
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
