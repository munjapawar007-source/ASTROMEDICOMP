var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width = 0;
var canvas_original_height = 0;

// webGL related veriables
const MyAttributs =
{
    AMC_ATTRIBUTE_POSITION: 0,
    AMC_ATTRIBUTE_COLOR: 1,

};

var shaderProgramObject = null;  //object to give memory

var perspectiveProjectionMatrix;
var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var materialColorUniform = 0;

// sphere
var sphere = null;

var shoulder = 0.0;
var elbow = 0.0;
var wrist = 0.0;

// matrix stack (Array used as stack)
let matrixStack = [];

// current matrix (identity)
var currentMatrix = mat4.create(); 


// push current matrix onto stack
function pushMatrix() {
    // clone is IMPORTANT (avoid reference copy)
    matrixStack.push(mat4.clone(currentMatrix));
}

// pop matrix from stack
function popMatrix() {
    if (matrixStack.length > 0) {
        currentMatrix = matrixStack.pop();
    }
}



var requestAnimationFrame = window.requestAnimationFrame ||      // crome
    window.mozRequestAnimationFrame ||      // mozila
    window.oRequestAnimationFrame ||      // opera
    window.webkitAnimationFrame ||      // safari
    window.msRequestAnimationFrame;          // microsoft Edge

function main() {
    //  get canvas
    canvas = document.getElementById("msp");
    if (canvas == null) {
        console.log("canvas element can not be obtained.\n");
    }
    else {
        console.log("canvas element successfully obtained.\n");
    }

    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    // register our callback func
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);

    //initialize
    initialize();
    resize();
    display();

}

function keyDown(event) {
    //  code
    //alert("key is press");
    switch (event.keyCode) {
        case 70:  // F
        case 102: // f
            if (bFullscreen == false) {
                toggleFullscreen();
                bFullscreen = true;
            }
            else {
                toggleFullscreen();
                bFullscreen = false;
            }
            break;
       case 83:     //'S'
            shoulder += 3;    
        break;
        case 115:   //'s'
            shoulder -= 3;
        break;
        case 69:    //'E'
            elbow += 3;    
        break;
        case 101:   //'e'
            elbow -= 3;
        break;
		case 87:    //'W'
            wrist += 3;    
        break;
        case 77:    //w'
            wrist -= 3;
            break;
        case 27:    //Escape Key
            // case 69:    // E
            // case 101:   // e
            uninitialize();
            window.close();
            break;
    };

}

function mouseDown() {
    //  code
    // alert("mouse is clicked");

}

function toggleFullscreen() {
    // code
    /* 3 steps
        1) fullscreen navacha element ahe ka 
        2) null asel tr fullsceern
        3) null nasel tr no fullscreen
    */

    var fullscreen_element =
        document.fullscreenelement ||
        document.mozFullScreenElement ||
        document.webkitFullscreenElement ||
        document.msFullscreenElement ||
        null;

    if (fullscreen_element == null) {
        if (canvas.requestFullscreen) {
            canvas.requestFullscreen();  //for google croom
        }
        else if (canvas.mozRequestFullScreen) {
            canvas.mozRequestFullScreen();

        }
        else if (canvas.webkitRequestFullscreen) {
            canvas.webkitRequestFullscreen();
        }
        else if (canvas.msRequestFullscreen) {
            canvas.msRequestFullscreen();
        }

    }
    else {
        if (document.exitFullscreen) {
            document.exitFullscreen();
        }
        else if (document.mozCancelFullScreen) {
            document.mozCancelFullScreen();

        }
        else if (document.webkitExitFullscreen) {
            document.webkitExitFullscreen();
        }
        else if (document.msExitFullscreen) {
            document.msExitFullscreen();
        }
    }

}

function initialize() {
    // get 2D context from canvas
    gl = canvas.getContext("webgl2");
    if (gl == null) {
        console.log(" webgl2 context can not be obtained.\n");
    }
    else {
        console.log(" webgl2 context successfully obtained.\n");
    }

    // set view port width and height
    gl.viewportWidth = canvas.width;
    gl.viewportHeight = canvas.height;


    // Vertex Shader
    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    var vertexShaderSourceCode =
        (
            "#version 300 es \n" +
            "in vec4 aPosition;\n" +
            "uniform mat4 uModelMatrix;\n" +
            "uniform mat4 uViewMatrix;\n" +
            "uniform mat4 uProjectionMatrix;\n" +
            "void main(void)\n" +
            "{\n" +
            "   gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" +
            "}\n"
        );

    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
    gl.compileShader(vertexShaderObject);

    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }
    else {
        console.log("vertex shader compilation successfull\n")
    }

    // fragmant shader
    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
    var fragmentShaderSourceCode =
        (
            "#version 300 es\n" +
            "precision highp float;" +
            "uniform vec3 uMaterialColor;\n" +
            "out vec4 FragColor;\n" +
            "void main(void)\n" +
            "{\n" +
            "   FragColor = vec4(uMaterialColor, 1.0f);\n" +
            "}\n"
        );

    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);
    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }

    }
    else {
        console.log(" Fragment shader compilation successfull\n")
    }

    shaderProgramObject = gl.createProgram();
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);

    // prelink attributs
    gl.bindAttribLocation(shaderProgramObject, MyAttributs.AMC_ATTRIBUTE_POSITION, "aPosition");
    gl.bindAttribLocation(shaderProgramObject, MyAttributs.AMC_ATTRIBUTE_COLOR, "aColor");

    //link
    gl.linkProgram(shaderProgramObject);
    if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }
    else {
        console.log(" Shader program link successfully");
    }

   // get the required uniform loction from the sheder
	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelMatrix");
    viewMatrixUniform =  gl.getUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");
    materialColorUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialColor");

    sphere = new Mesh();
    makeSphere(sphere, 2.0, 30, 30);

    // Depth initialization
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    perspectiveProjectionMatrix = mat4.create();

}

function resize() 
{
    // code
    if (bFullscreen == true) {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    gl.viewport(0.0, 0.0, canvas.width, canvas.height);

    // set perspective projection
    mat4.perspective(perspectiveProjectionMatrix, 45.0 * Math.PI / 180.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0)


}

function display() 
{
    // veriable decleration 
    let Radian = Math.PI / 180.0;

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    var viewMatrix = mat4.create();
    var modelMatrix = mat4.create();
    var translationMatrix = mat4.create();
    var scaleMatrix = mat4.create();
    var RotationMatrix = mat4.create();

    viewMatrix = mat4.lookAt(viewMatrix, [0.0, 0.0, 5.0], [0.0, 0.0, 0.0], [0.0, 1.0, 0.0]);
    mat4.identity(currentMatrix);
    mat4.translate(currentMatrix, currentMatrix, [-2.0, 0.0, -6.0]);

    //send this matrix to the vertex shader to the uniform
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);
    gl.uniform3f(materialColorUniform, 0.50, 0.35, 0.050);
 
    pushMatrix();

    // SHOULDER
    
    mat4.identity(RotationMatrix);  
    mat4.rotateZ(RotationMatrix, RotationMatrix, shoulder * Radian);
    mat4.multiply(currentMatrix, currentMatrix, RotationMatrix);
    mat4.identity(translationMatrix);
    mat4.translate(translationMatrix, translationMatrix, [1.0, 0.0, 0.0]);
    mat4.multiply(currentMatrix, currentMatrix, translationMatrix);

    pushMatrix();
    mat4.identity(scaleMatrix);
    mat4.scale(scaleMatrix, scaleMatrix, [1.0, 0.25, 1.0]);
    mat4.multiply(currentMatrix, currentMatrix, scaleMatrix);
    modelMatrix = currentMatrix;
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    sphere.draw(gl.TRIANGLES);
    popMatrix();

    // ELBOW
    mat4.identity(translationMatrix);
    mat4.translate(translationMatrix, translationMatrix, [2.0, 0.0, 0.0]);
    mat4.multiply(currentMatrix, currentMatrix, translationMatrix);

    mat4.identity(RotationMatrix);
    mat4.rotateZ(RotationMatrix, RotationMatrix, elbow * Radian);
    mat4.multiply(currentMatrix, currentMatrix, RotationMatrix);
   
    mat4.identity(translationMatrix);
    mat4.translate(translationMatrix, translationMatrix, [2.0, 0.0, 0.0]);
    mat4.multiply(currentMatrix, currentMatrix, translationMatrix);

    pushMatrix();
    mat4.identity(scaleMatrix)
    mat4.scale(scaleMatrix, scaleMatrix, [1.0, 0.2, 1.0]);
    mat4.multiply(currentMatrix, currentMatrix, scaleMatrix);
    modelMatrix = currentMatrix;
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    sphere.draw(gl.TRIANGLES);           
    popMatrix();

    // wrist
    mat4.identity(translationMatrix);
    mat4.translate(translationMatrix, translationMatrix, [1.0, 0.0, 0.0]);
    mat4.multiply(currentMatrix, currentMatrix, translationMatrix);
   
    mat4.identity(RotationMatrix);
    mat4.rotateZ(RotationMatrix, RotationMatrix, wrist * Radian);
    mat4.multiply(currentMatrix, currentMatrix, RotationMatrix);
    
    mat4.identity(translationMatrix);
    mat4.translate(translationMatrix, translationMatrix, [1.0, 0.0, 0.0]);
    mat4.multiply(currentMatrix, currentMatrix, translationMatrix);

    pushMatrix();
    mat4.identity(scaleMatrix);
    mat4.scale(scaleMatrix, scaleMatrix, [0.30, 0.2, 1.0]);
    mat4.multiply(currentMatrix, currentMatrix, scaleMatrix);
    modelMatrix = currentMatrix;
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    sphere.draw(gl.TRIANGLES);
    popMatrix();

    popMatrix();

    //unUse shader program
    gl.useProgram(null);


    //update
    update();

    // double buffering
    requestAnimationFrame(display, canvas);
}


function update() {

}

function uninitialize() 
{
    //code
    if (bFullscreen == true) {
        toggleFullscreen();
    }

    if (sphere) 
    {
        sphere.deallocate();
        sphere = null;
    }


    //detach delete shader program object
    if (shaderProgramObject) 
    {
        gl.useProgram(shaderProgramObject);
        var shaderObject = gl.getAttachedShaders(shaderProgramObject);

        for (let i = 0; i < shaderObject.length; i++) {
            gl.detachShader(shaderProgramObject, shaderObject[i]);
            gl.deleteShader(shaderObject[i]);
            shaderObject[i] = null;
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }
}

