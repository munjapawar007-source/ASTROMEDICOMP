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

// Texture related pakages
import android.graphics.BitmapFactory;
import android.graphics.Bitmap;
import android.opengl.GLUtils;

public class GLESView extends GLSurfaceView
      implements GLSurfaceView.Renderer, OnGestureListener, OnDoubleTapListener 
{

   // filed/class veriable
   private Context context;
   private GestureDetector gestureDetector;
   private int shaderProgramObject;
   private int mvpMatrixUniform;

   private int vao[] = new int[1];
   private int vbo[] = new int[1];
   private int vbo_texcoord[] = new int[1];
   private float perspectiveProjectionMatrix[] = new float[16];

   // texture
   private int textureSmiley[] = new int[1];
   private int textureSamplerUniform;
   private int keyPressUniform;
   
   private int singleTap;
   private float rectangleTexcoord[] = new float[8];

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
         "in vec2 out_TexCoord;\n" +
         "uniform mediump int uSingleTap;\n" +
         "uniform sampler2D uTextureSampler;\n" +
         "out vec4 FragColor;\n" +
         "void main(void)\n" +
         "{if(uSingleTap != 1 && uSingleTap != 2 && uSingleTap != 3 && uSingleTap != 4)\n" +
         "{ FragColor = vec4(1.0,1.0,1.0,1.0);}\n" +
         "else\n" +
         "{FragColor=texture(uTextureSampler,out_TexCoord);}\n" +
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
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributs.MSP_ATTRIBUTE_POSITION, "aPosition");
      GLES32.glBindAttribLocation(shaderProgramObject, MyAttributs.AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");

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
	   textureSamplerUniform = GLES32.glGetUniformLocation(shaderProgramObject, "uTextureSampler");
      keyPressUniform = GLES32.glGetUniformLocation(shaderProgramObject,"uSingleTap");

      // provide vertex position,color,texcode, and normal etc.
      final float Rectangle_position[] = new float[] 
      { 
         -1.0f,-1.0f,0.0f,
         1.0f,-1.0f,0.0f,
         1.0f,1.0f,0.0f,
         -1.0f,1.0f,0.0f
      };

      // final float rectangle_Texcoords[] = new float[] 
      // {
      //    1.0f, 1.0f,
      //    0.0f, 1.0f,
      //    0.0f, 0.0f,
      //    1.0f, 0.0f
      // };

      GLES32.glGenVertexArrays(1, vao, 0);
      GLES32.glBindVertexArray(vao[0]);

      // Position
      GLES32.glGenBuffers(1, vbo,0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo[0]);

      // create a native buffer suitable for native IO but for java
      // 1) set buffer size (36 byte ) 
          ByteBuffer byteBuffer = ByteBuffer.allocateDirect(Rectangle_position.length * 4);
      // 2) set order left to right OR right to left
          byteBuffer.order(ByteOrder.nativeOrder());
      // 3) create which type is data
         FloatBuffer rectanglePositionBuffer = byteBuffer.asFloatBuffer();
      // 4) fill array of rectangle_ position
         rectanglePositionBuffer.put(Rectangle_position);
      // 5)  set array to 0th position for repeatable use
          rectanglePositionBuffer.position(0);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, Rectangle_position.length * 4, rectanglePositionBuffer, GLES32.GL_STATIC_DRAW);
     
      GLES32.glVertexAttribPointer(MyAttributs.MSP_ATTRIBUTE_POSITION, 3, GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributs.MSP_ATTRIBUTE_POSITION);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      // TEXCOORD
      GLES32.glGenBuffers(1, vbo_texcoord, 0);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord[0]);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, 4 * 2 * 4, null, GLES32.GL_STATIC_DRAW);
      GLES32.glVertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_TEXCOORD);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      GLES32.glBindVertexArray(0);

      
      // Texture 
      textureSmiley[0] = loadGLTexture(R.raw.smiley); // tujya resources madhe ja ani tyatil raw dir madil png ghe 
                                                      // pn to ghetani png ghet nahi to ID manun gheto ani tila decode karto
      // Enable Texturing
	   GLES32.glEnable(GLES32.GL_TEXTURE_2D);

      // Depth initialization
      GLES32.glClearDepthf(1.0f);
      GLES32.glEnable(GLES32.GL_DEPTH_TEST);
      GLES32.glDepthFunc(GLES32.GL_LEQUAL);

      // set the clear color
      GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

      singleTap = 0;
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

   private int loadGLTexture(int imageFileResourceID)
   {
      // decide whethere android should scale or not
      BitmapFactory.Options options = new BitmapFactory.Options();
      options.inScaled = false;

      // create android compactable bitmap of our image
      Bitmap bitmap = BitmapFactory.decodeResource(context.getResources(), imageFileResourceID, options);

      int tex[] =  new int[1];

      // create texture object
      GLES32.glGenTextures(1, tex, 0);
      GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, tex[0]);
      GLES32.glPixelStorei(GLES32.GL_UNPACK_ALIGNMENT, 4);
      GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MAG_FILTER, GLES32.GL_LINEAR);
      GLES32.glTexParameteri(GLES32.GL_TEXTURE_2D, GLES32.GL_TEXTURE_MIN_FILTER, GLES32.GL_LINEAR_MIPMAP_LINEAR);

      GLUtils.texImage2D(GLES32.GL_TEXTURE_2D, 0, bitmap, 0); // internally call glTexImage2D
      GLES32.glGenerateMipmap(GLES32.GL_TEXTURE_2D);
      GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);
     
      return tex[0];
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

      // for texture
      GLES32.glActiveTexture(GLES32.GL_TEXTURE0);
      GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, textureSmiley[0]);
      GLES32.glUniform1i(textureSamplerUniform, 0);

      //bind with vao
	   GLES32.glBindVertexArray(vao[0]);
      
    
      if (singleTap == 1)
      {
         rectangleTexcoord[0] = 1.0f;
         rectangleTexcoord[1] = 1.0f;
         rectangleTexcoord[2] = 0.0f;
         rectangleTexcoord[3] = 1.0f;
         rectangleTexcoord[4] = 0.0f;
         rectangleTexcoord[5] = 0.0f;
         rectangleTexcoord[6] = 1.0f;
         rectangleTexcoord[7] = 0.0f;
         GLES32.glUniform1i(keyPressUniform,1);
      }
       if (singleTap == 2)
      {
         rectangleTexcoord[0] = 0.5f;
         rectangleTexcoord[1] = 0.5f;
         rectangleTexcoord[2] = 0.0f;
         rectangleTexcoord[3] = 0.5f;
         rectangleTexcoord[4] = 0.0f;
         rectangleTexcoord[5] = 0.0f;
         rectangleTexcoord[6] = 0.5f;
         rectangleTexcoord[7] = 0.0f;
         GLES32.glUniform1i(keyPressUniform,2);
      }
      if (singleTap == 3)
      {

         rectangleTexcoord[0] = 3.0f;
         rectangleTexcoord[1] = 3.0f;
         rectangleTexcoord[2] = 1.0f;
         rectangleTexcoord[3] = 3.0f;
         rectangleTexcoord[4] = 1.0f;
         rectangleTexcoord[5] = 1.0f;
         rectangleTexcoord[6] = 3.0f;
         rectangleTexcoord[7] = 1.0f;
         GLES32.glUniform1i(keyPressUniform,3);
      }
      if (singleTap == 4)
      {

         rectangleTexcoord[0] = 0.5f;
         rectangleTexcoord[1] = 0.5f;
         rectangleTexcoord[2] = 0.51f;
         rectangleTexcoord[3] = 0.5f;
         rectangleTexcoord[4] = 0.51f;
         rectangleTexcoord[5] = 0.51f;
         rectangleTexcoord[6] = 0.5f;
         rectangleTexcoord[7] = 0.5f;
         GLES32.glUniform1i(keyPressUniform,4);
      }

      // create a native buffer suitable for native IO but for java
      // 1) set buffer size (36 byte ) 
         ByteBuffer byteBuffer = ByteBuffer.allocateDirect(rectangleTexcoord.length * 4);
      // 2) set order left to right OR right to left
         byteBuffer.order(ByteOrder.nativeOrder());
      // 3) create which type is data
         FloatBuffer rectangleTexcoordBuffer = byteBuffer.asFloatBuffer();
      // 4) fill array of rectangle_ position
         rectangleTexcoordBuffer.put(rectangleTexcoord);
      // 5)  set array to 0th position for repeatable use
         rectangleTexcoordBuffer.position(0);
     
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, vbo_texcoord[0]);
      GLES32.glBufferData(GLES32.GL_ARRAY_BUFFER, rectangleTexcoord.length * 4, rectangleTexcoordBuffer, GLES32.GL_DYNAMIC_DRAW);
      GLES32.glVertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0); //jithe mi data bharla tithe ja ani tithun don don data bajula kar 
      GLES32.glEnableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_TEXCOORD); // data enable kar
      
      //draw vertex array
	   GLES32.glDrawArrays(GLES32.GL_TRIANGLE_FAN, 0, 4);

	   GLES32.glBindVertexArray(0);
      GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, 0);

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
      if(textureSmiley[0] > 0)
      {
         GLES32.glDeleteBuffers(1,textureSmiley,0);
         textureSmiley[0] = 0;
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

}
