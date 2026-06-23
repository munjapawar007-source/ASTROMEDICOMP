function main() 
{
    //  get canvas
    var canvas = document.getElementById("msp");
    if (canvas == null) 
    {
        console.log("canvas element can not be obtain\n");
    }
    else
    {
        console.log("canvas element successfully obtain\n");
    }

    // get 2D context from canvas
    var context = canvas.getContext("2d");
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

    //  to show the text centralley set context attribute accordingly
    context.textAlign = "center";      //this allows text to show Horizontaly
    context.textBaseLine = "middle";    // this allows text to show verticaly

    //  set the text font
    context.font = "48px sans-serif";

    //  set the color of text
    context.fillStyle = "#00ff00";      //lime color (green)

    // set the text
    var str = "Hello World !!!";
    context.fillText(str,canvas.width/2,canvas.height/2); 

    // register our callback func
    window.addEventListener("keydown",keyDown,false);
    window.addEventListener("click",mouseDown,false);

}

function keyDown(event)
{
    //  code
    alert("key is press");
}

function mouseDown()
{
    //  code
    alert("mouse is clicked");
}


