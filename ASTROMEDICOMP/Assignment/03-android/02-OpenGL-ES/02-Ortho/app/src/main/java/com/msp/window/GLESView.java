package com.msp.window;

import android.content.Context;

//OpenGL Related pakages
import android.opengl.GLSurfaceView;
import javax.microedition.khronos.opengles.GL10;
import javax.microedition.khronos.egl.EGLConfig;
import android.opengl.GLES32; 
import android.opengl.Matrix;

//events related pakages
import android.view.MotionEvent;
import android.view.GestureDetector;
import android.view.GestureDetector.OnGestureListener;
import android.view.GestureDetector.OnDoubleTapListener;


public class GLESView extends GLSurfaceView 
      implements GLSurfaceView.Renderer, OnGestureListener,OnDoubleTapListener 
{
   // filed/class veriable
   private Context context;
   private GestureDetector gestureDetector;
   private int orthographicProjectionMatrix = new float[16];


   public GLESView(Context _context) 
   {
      super(_context);
      //context is recommanded
      context = _context;
   
      //initialize opengl
      setEGLContextClientVersion(3);
      setRenderer(this);
      //jeva render  mode empty hoil teva renderdirty mode on kr
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
      //code
      int iResult = initialize(gl);

      if(iResult != 0)
      {
         System.out.println("MSP: initialize() failed");
         System.exit(0);
      }
   }

   @Override
   public void onSurfaceChanged(GL10 gl, int width, int height)
   {
      //code
      Resize(width, height);

   }

   @Override
   public void onDrawFrame(GL10 gl)
   {
      //code
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
      //code
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

   //our custome opengl method
   private int initialize(GL10 gl)
   {
      //code
      printGLESInfo(gl);

      //start openGl-ES 3.2
      //Depth initialization
      GLES32.glClearDepthf(1.0f);
      GLES32.glEnable(GLES32.GL_DEPTH_TEST);
      GLES32.glDepthFunc(GLES32.GL_LEQUAL);

      //set the clear color
      GLES32.glClearColor(0.0f,0.0f,1.0f,1.0f);
      Matrix.setIdentityM(orthographicProjectionMatrix,0);
      return 0;
   }

   private void printGLESInfo(GL10 gl)
   {
      //code
      String glesVendor = gl.glGetString(GL10.GL_VENDOR);
      String glesRender = gl.glGetString(GL10.GL_RENDERER);
      String glesVersion = gl.glGetString(GL10.GL_VERSION);

      System.out.println("MSP: "+glesVendor);
         System.out.println("MSP: "+glesRender);
            System.out.println("MSP: "+glesVersion);

   }

   private void Resize(int width, int height)
   {
      //code
      // if height by default 0 makes1
      if (height <= 0)
      {
         height = 1;
      }
      // set the viewPort
      GLES32.glViewport(0, 0, width, height);
      
      
	if(width <= height)
	{
      orthographicProjectionMatrix = Matrix.orthoM(     
                                                   -100.0f, //left
                                                   100.0f, //right
                                                   (-100.0f * ((float)height / (float)width)), //bottom
                                                   (100.0f * ((float)height / (float)width)), //top
                                                   -100.0f, //near
                                                   100.0f); //far
	}
	else
	{
		 orthographicProjectionMatrix = Matrix.orthoM
                                                ((-100.0f * ((float)width / (float)height)),
                                                   (100.0f * ((float)width / (float)height)),
                                                   -100.0f,
                                                   100.0f,
                                                   -100.0f,
                                                   100.0f);
	}

   }

   private void Display()
   {
      //code
      GLES32.glClear(GLES32.GL_COLOR_BUFFER_BIT | GLES32.GL_DEPTH_BUFFER_BIT);
   
    
      requestRender();
   }

   private void Update()
   {
      //code
   }

   private void Uninitialize()
   {
      //code
   }


}
