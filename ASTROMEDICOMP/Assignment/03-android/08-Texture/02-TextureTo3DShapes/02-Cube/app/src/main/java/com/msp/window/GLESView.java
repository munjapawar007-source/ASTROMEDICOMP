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

   // texture
   private int texture_kundali[] = new int[1];
   private int textureSamplerUniform;

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
     
      GLES32.glVertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_TEXCOORD, 2, GLES32.GL_FLOAT, false, 0, 0);
      GLES32.glEnableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_TEXCOORD);
      GLES32.glBindBuffer(GLES32.GL_ARRAY_BUFFER, 0);

      GLES32.glBindVertexArray(0);

      
      // Texture 
      texture_kundali[0] = loadGLTexture(R.raw.vijay_kundali); // tujya resources madhe ja ani tyatil raw dir madil png ghe 
                                                      // pn to ghetani png ghet nahi to ID manun gheto ani tila decode karto
      // Enable Texturing
	   GLES32.glEnable(GLES32.GL_TEXTURE_2D);

      // Depth initialization
      GLES32.glClearDepthf(1.0f);
      GLES32.glEnable(GLES32.GL_DEPTH_TEST);
      GLES32.glDepthFunc(GLES32.GL_LEQUAL);

      // set the clear color
      GLES32.glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

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
      GLES32.glBindTexture(GLES32.GL_TEXTURE_2D, texture_kundali[0]);
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
      if(texture_kundali[0] > 0)
      {
         GLES32.glDeleteBuffers(1,texture_kundali,0);
         texture_kundali[0] = 0;
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
