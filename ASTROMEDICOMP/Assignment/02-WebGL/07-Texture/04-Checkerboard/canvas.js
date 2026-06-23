var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width = 0;
var canvas_original_height = 0;

// webGL related veriables
const MyAttributs =
{
    AMC_ATTRIBUTE_POSITION: 0,
    AMC_ATTRIBUTE_TEXCOORD: 1,
};


var shaderProgramObject = null;  //object to give memory

var vao = null;
var vbo_TexCoordRectangle = null;  //uninitialized
var vbo_position = null;  //uninitialized

var mvpUniform; //undefined (delyed declerative)

var perspectiveProjectionMatrix; //undefined

// Rotation
var Angle = 0.0;

// Texture
const CHECKERBOARD_WIDTH = 64;
const CHECKERBOARD_HEIGHT = 64;
var CHECKERBOARD_DEPTH = 4;
let Checkerboard = new Uint8Array(CHECKERBOARD_HEIGHT*CHECKERBOARD_WIDTH*4); // openGL is column mesure
let texture_Checkerboard;
var textureSamplerUniform;

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
            "in vec2 aTexCoord;\n" +
            "out vec2 out_TexCoord;\n" +
            "uniform mat4 uMVPMatrix;\n" +
            "void main(void)\n" +
            "{gl_Position=uMVPMatrix*aPosition;\n" +
            "out_TexCoord = aTexCoord;\n" +
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
            "in vec2 out_TexCoord;\n" +
            "uniform sampler2D uTextureSampler;\n" +
            "out vec4 FragColor;\n" +
            "void main(void)\n" +
            "{FragColor=texture(uTextureSampler,out_TexCoord);\n" +
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
    gl.bindAttribLocation(shaderProgramObject,MyAttributs.AMC_ATTRIBUTE_TEXCOORD,"aTexCoord");

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
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "uTextureSampler");

    // provide vertex position,color,texcode, and normal etc.

    var rectangle_TexCoords = new Float32Array([

        1.0, 1.0,
        0.0, 1.0,
        0.0, 0.0,
        1.0, 0.0
    ]);


    //create varetx array
    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);

    // texcoord
    vbo_texcoord = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texcoord)
    gl.bufferData(gl.ARRAY_BUFFER, rectangle_TexCoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_TEXCOORD);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    // ********************  Texture   ********************
    loadGLTexture();
    
    // Depth initialization
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    perspectiveProjectionMatrix = mat4.create();

}


function loadGLTexture() {
    //function
    MakeCheckerboard();

    // console.log("entered into loadGLTexture()");
    texture_Checkerboard = gl.createTexture();

    gl.bindTexture(gl.TEXTURE_2D, texture_Checkerboard);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.NEAREST);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.NEAREST);  // here webGL recmonded nearest_nearest
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.REPEAT);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.REPEAT);
      
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, CHECKERBOARD_WIDTH, CHECKERBOARD_HEIGHT, 0, gl.RGBA, gl.UNSIGNED_BYTE, Checkerboard);
        //gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, Checkerboard);
        
    gl.bindTexture(gl.TEXTURE_2D, null);
    
    return texture_Checkerboard;
}


function MakeCheckerboard() {
    // variable declaration
    var i;
    var j;
    let c;
    // code
    for (i = 0; i < CHECKERBOARD_HEIGHT; i++) {
       

        for (j = 0; j < CHECKERBOARD_WIDTH; j++) 
        {
            c = ((((i & 0x8) === 0) ^ ((j & 0x8) === 0)) * 255);
            
            let index = (i * CHECKERBOARD_WIDTH + j) * 4;

            Checkerboard[index + 0] = c;
            Checkerboard[index + 1] = c;
            Checkerboard[index + 2] = c;
            Checkerboard[index + 3] = 255;
        }
    }
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
    //parse Float = 
}

function display() 
{
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    let Radian = Angle * Math.PI / 180.0;

    var modelViewMatrix = mat4.create();
    var translationMatrix = mat4.create();
    var RotationMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -4.0]);
    mat4.rotateY(RotationMatrix, RotationMatrix, Radian);
    mat4.multiply(modelViewMatrix, translationMatrix, RotationMatrix);

    var ModelViewProjectionMatrix = mat4.create();
    mat4.multiply(ModelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    //send this matrix to the vertex shader to the uniform
    gl.uniformMatrix4fv(mvpUniform, false, ModelViewProjectionMatrix);

    //for texture
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, texture_Checkerboard);
    gl.uniform1i(textureSamplerUniform, 0);

    //bind with vao
    gl.bindVertexArray(vao);

    let rectanglePosition = new Float32Array(12);;
    rectanglePosition[0] = 0.0;
	rectanglePosition[1] = 1.0;
	rectanglePosition[2] = 0.0;
	rectanglePosition[3] = -2.0;
	rectanglePosition[4] = 1.0;
	rectanglePosition[5] = 0.0;
	rectanglePosition[6] = -2.0;
	rectanglePosition[7] = -1.0;
	rectanglePosition[8] = 0.0;
	rectanglePosition[9] = 0.0;
	rectanglePosition[10] = -1.0;
	rectanglePosition[11] = 0.0;

    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, rectanglePosition, gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    // draw vertex array
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    // 2nd Rectangle
	rectanglePosition[0] = 2.41421;
	rectanglePosition[1] = 1.0;
	rectanglePosition[2] = -1.41421;
	rectanglePosition[3] = 1.0;
	rectanglePosition[4] = 1.0;
	rectanglePosition[5] = 0.0;
	rectanglePosition[6] = 1.0;
	rectanglePosition[7] = -1.0;
	rectanglePosition[8] = 0.0;
	rectanglePosition[9] = 2.41421;
	rectanglePosition[10] = -1.0;
	rectanglePosition[11] = -1.41421;

    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
    gl.bufferData(gl.ARRAY_BUFFER, rectanglePosition, gl.DYNAMIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //draw vertex array
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);

    gl.bindVertexArray(null);
    gl.bindTexture(gl.TEXTURE_2D, null);

    //unUse shader program
    gl.useProgram(null);

    //update
    update();

    // double buffering
    requestAnimationFrame(display, canvas);
}


function update() {
    Angle = Angle + 0.3;
    if (Angle >= 360.0) {
        Angle = Angle - 360.0;
    }

}

function uninitialize() {
    //code
    if (bFullscreen == true) {
        toggleFullscreen();
    }

    if (texture_Checkerboard)
	{
		gl.deleteTextures(texture_Checkerboard);
		texture_Checkerboard = null;
	}

    if (vbo_texcoord) {
        gl.deleteBuffer(vbo_texcoord);
        vbo_texcoord = null;
    }

    if (vbo_position) {
        gl.deleteBuffer(vbo_position);
        vbo_position = null;
    }

    if (vao) {
        gl.deleteVertexArray(vao);
        vao = null;
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

