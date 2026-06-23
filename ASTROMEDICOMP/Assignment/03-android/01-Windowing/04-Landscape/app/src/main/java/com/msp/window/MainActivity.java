package com.msp.window;

import android.os.Bundle;
import android.graphics.Color;
import androidx.appcompat.app.AppCompatActivity;

//packages for fullscreen
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.core.view.WindowInsetsCompat;

//orientation related pacakge
import android.content.pm.ActivityInfo;

public class MainActivity extends AppCompatActivity
{
 @Override
 protected void onCreate(Bundle savedInstanceState)
 {
    super.onCreate(savedInstanceState);
    
    //force lanscape
    //setRequestedOriantaion(activityInfo.SCREEN-ORIENTATION-LANDSCAPE);
   setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

    //*Fullscreen
      //hide action bar
      getSupportActionBar().hide();

      //tell the android system to make your window expand edge to edge
      WindowCompat.setDecorFitsSystemWindows(getWindow(),false);

      //get window insets controller
      WindowInsetsControllerCompat windowInsetsControllerCompat = WindowCompat.getInsetsController(getWindow(),getWindow().getDecorView()); 

      //now tell the insets controller to hide remanining bar and insets
      windowInsetsControllerCompat.hide(WindowInsetsCompat.Type.systemBars() | WindowInsetsCompat.Type.ime());

    //*set background  
    getWindow().getDecorView().setBackgroundColor(Color.BLACK);
    MyTextView myTextView = new MyTextView(this);


    setContentView(myTextView);
 }
}


