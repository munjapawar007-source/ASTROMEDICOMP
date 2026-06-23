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

var vao_rectangle = null;
var vbo_color_rectangle = null;  //uninitialized
var vbo_position_rectangle = null;  //uninitialized

var vao_triangle = null;
var vbo_color_triangle = null;  
var vbo_position_triangle = null;

var mvpUniform; //undefined (delyed declerative)

var perspectiveProjectionMatrix; //undefined

//Rotation
var angleCube_MP = 0.0;

var requestAnimationFrame = window.requestAnimationFrame ||      // crome
    window.mozRequestAnimationFrame ||      // mozila
    window.oRequestAnimationFrame ||      // opera
    window.webkitAnimationFrame ||      // safari
    window.msRequestAnimationFrame;     // microsoft Edge

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
            "uniform mat4 uMVPMatrix;\n" +
            "void main(void)\n" +
            "{gl_Position=uMVPMatrix*aPosition;\n" +
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
            "out vec4 FragColor;\n" +
            "void main(void)\n" +
            "{FragColor=vec4(1.0,1.0,1.0,1.0);\n" +
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

    // get uniform
    mvpUniform = gl.getUniformLocation(shaderProgramObject, "uMVPMatrix");

    // provide vertex position,color,texcode, and normal etc.
    var rectangle_position = new Float32Array([
        1.0, 1.0, 0.0,	  
        -1.0, 1.0, 0.0,	  
        -1.0, -1.0, 0.0,  
        1.0, -1.0, 0.0]); 

    var triangle_position = new Float32Array([
        0.0, 1.0, 0.0,
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0]);

    //create varetx array and Bind Rectangle
    vao_rectangle = gl.createVertexArray();
    gl.bindVertexArray(vao_rectangle);

    vbo_position_rectangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_rectangle);

    gl.bufferData(gl.ARRAY_BUFFER, rectangle_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);


    // UNBIND Here
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    /* ---------------Triangle--------------- */
    //create varetx array and Bind Triangle
    vao_triangle = gl.createVertexArray();
    gl.bindVertexArray(vao_triangle);

    vbo_position_triangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_triangle);

    gl.bufferData(gl.ARRAY_BUFFER, triangle_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);

    // UNBIND Here
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    // Depth initialization
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // clear color
    gl.clearColor(0.2, 0.3, 0.2, 1.0);

    perspectiveProjectionMatrix = mat4.create();

}

function resize() {
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
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0)
    
}

function display() 
{
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    let Radian = angleCube_MP * Math.PI / 180.0;

    var modelViewMatrix = mat4.create();
    var translationMatrix = mat4.create();
    var rotationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-1.0, 0.0, -3.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, Radian);   
    mat4.multiply(modelViewMatrix,translationMatrix,rotationMatrix);
    var ModelViewProjectionMatrix = mat4.create();
    mat4.multiply(ModelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    //send this matrix to the vertex shader to the uniform
    gl.uniformMatrix4fv(mvpUniform, false, ModelViewProjectionMatrix);

    //bind with vao_rectangle
    gl.bindVertexArray(vao_rectangle);

    //draw vertex array
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    gl.bindVertexArray(null);

    /* ---------------Triangle--------------- */
    
    mat4.identity(modelViewMatrix);
    mat4.identity(translationMatrix);
    mat4.identity(rotationMatrix);
    mat4.translate(translationMatrix, translationMatrix, [1.0, 0.0, -3.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, Radian);   
    mat4.multiply(modelViewMatrix,translationMatrix,rotationMatrix);
    mat4.identity(ModelViewProjectionMatrix);
    mat4.multiply(ModelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    //send this matrix to the vertex shader to the uniform
    //gl.uniformMatrix4fv(mvpUniform, false, ModelViewProjectionMatrix);
    gl.uniformMatrix4fv(mvpUniform, false, ModelViewProjectionMatrix);

    //bind with vao_rectangle
    gl.bindVertexArray(vao_triangle);

    //draw vertex array
    gl.drawArrays(gl.TRIANGLES, 0, 3);

    gl.bindVertexArray(null);

    //unUse shader program
    gl.useProgram(null);

    //update
    update();

    // double buffering
    requestAnimationFrame(display, canvas);
}


function update() 
{
    angleCube_MP = angleCube_MP - 0.1;

    if (angleCube_MP <= 0.0) {
        angleCube_MP = angleCube_MP + 360.0;
    }
}

function uninitialize() {
    //code
    if (bFullscreen == true) {
        toggleFullscreen();
    }

     if (vbo_color_triangle) {
        gl.deleteBuffer(vbo_color_triangle);
        vbo_color_triangle = null;
    }

    if (vbo_position_triangle) {
        gl.deleteBuffer(vbo_position_triangle);
        vbo_position_triangle = null;
    }

    if (vao_triangle) {
        gl.deleteVertexArray(vao_triangle);
        vao_triangle = null;
    }

    if (vbo_color_rectangle) {
        gl.deleteBuffer(vbo_color_rectangle);
        vbo_color_rectangle = null;
    }

    if (vbo_position_rectangle) {
        gl.deleteBuffer(vbo_position_rectangle);
        vbo_position_rectangle = null;
    }

    if (vao_rectangle) {
        gl.deleteVertexArray(vao_rectangle);
        vao_rectangle = null;
    }

    //detach delete shader program object
    if (shaderProgramObject) {
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

