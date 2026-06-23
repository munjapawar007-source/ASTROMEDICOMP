var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width = 0;
var canvas_original_height = 0;

var requestAnimationFrame = window.requestAnimationFrame     ||      // crome
                            window.mozRequestAnimationFrame  ||      // mozila
                            window.oRequestAnimationFrame    ||      // opera
                            window.webkitAnimationFrame      ||      // safari
                            window.msRequestAnimationFrame;          // microsoft Edge

function main() 
{
    //  get canvas
     canvas = document.getElementById("msp");
    if (canvas == null) 
    {
        console.log("canvas element can not be obtain\n");
    }
    else
    {
        console.log("canvas element successfully obtain\n");
    }

    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    // register our callback func
    window.addEventListener("keydown",keyDown,false);
    window.addEventListener("click",mouseDown,false);
    window.addEventListener("resize",resize,false);

    //initialize
    initialize();
    resize();
    display();

}

function keyDown(event)
{
    //  code
    //alert("key is press");
    switch(event.keyCode)
    {
        case 70:  // F
        case 102: // f
            if(bFullscreen == false)
            {
                toggleFullscreen();
                bFullscreen = true;
            }
            else
            {
                toggleFullscreen();
                bFullscreen = false;
            }
        break;
        case 27:    //Escape Key
       // case 69:    // E
       // case 101:   // e
            uninitialize();
            window.close();
        break;    
    };

}

function mouseDown()
{
    //  code
   // alert("mouse is clicked");

}

function toggleFullscreen()
{
    // code
    /* 3 steps
        1) fullscreen navacha element ahe ka 
        2) null asel tr fullsceern
        3) null nasel tr no fullscreen
    */

    var fullscreen_element = 
                    document.fullscreenElement       || 
                    document.mozFullScreenElement    ||
                    document.webkitFullscreenElement ||
                    document.msFullscreenElement     ||
                    null;

    if(fullscreen_element == null)
    {
        if(canvas.requestFullscreen)
        { 
            canvas.requestFullscreen();  //for google croom
        }
        else if(canvas.mozRequestFullScreen)
        {
            canvas.mozRequestFullScreen();

        }
        else if(canvas.webkitRequestFullscreen)
        {
            canvas.webkitRequestFullscreen();
        }
        else if(canvas.msRequestFullscreen)
        {
            canvas.msRequestFullscreen();
        }
      
    }
    else
    {
        if(document.exitFullscreen)
        {
            document.exitFullscreen();
        }
        else if(document.mozCancelFullScreen)
        {
            document.mozCancelFullScreen();

        }
        else if(document.webkitExitFullscreen)
        {
            document.webkitExitFullscreen();
        }
        else if(document.msExitFullscreen)
        {
            document.msExitFullscreen();
        }
    }

    drawText("Hello World !!!");

}

function initialize()
{
    // get 2D context from canvas
    gl = canvas.getContext("webgl2");
    if (gl == null) 
    {
        console.log(" webgl2 context can not be obtain\n");
    }
    else
    {
        console.log(" webgl2 context successfully obtain\n");
    }

    // set view port width and height
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;
    
    // clear color
    gl.clearColor(0.0,0.0,1.0,1.0);
    
}

function resize()
{
    // code
    if(bFullscreen == true)
    {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else
    {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

   gl.viewport(0.0,0.0,canvas.width,canvas.height);
}

function display()
{
    gl.clear(gl.COLOR_BUFFER_BIT);


    //update
    update();

    // double buffering
    requestAnimationFrame(display,canvas);
}


function update()
{

}

function uninitialize()
{
    //code
}

