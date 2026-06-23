var canvas = null;
var context = null;
var bFullscreen = false;

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

    // get 2D context from canvas
     context = canvas.getContext("2d");
     if (context == null) 
    {
        console.log("context can not be obtain\n");
    }
    else
    {
        console.log("context successfully obtain\n");
    }

    // tell the context to make the canvas background color black
    context.fillStyle = "black";    //we can use "#000000"
    context.fillRect(0,0,canvas.width,canvas.height);

    // call drawText
    drawText("Hello World !!!");
   
    // register our callback func
    window.addEventListener("keydown",keyDown,false);
    window.addEventListener("click",mouseDown,false);

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
    }

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

function drawText(text)
{
    // code

    //  to show the text centralley set context attribute accordingly
    context.textAlign = "center";      //this allows text to show Horizontaly
    context.textBaseLine = "middle";    // this allows text to show verticaly

    //  set the text font
    context.font = "48px sans-serif";

    //  set the color of text
    context.fillStyle = "#00ff00";      //lime color (green)

    // set the text
    context.fillText(text,canvas.width/2,canvas.height/2); 

}
