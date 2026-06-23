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

var vao = null;
var vbo = null;  //uninitialized

var modelUniform; //undefined (delyed declerative)
var viewUniform;
var projectionUniform;

var perspectiveProjectionMatrix; //undefined

// Cubemap related variables
var cubeMap_texture;
let width, height, nrChannels;
var data;
var textureSkyboxUniform;
var angleCube_MP = 0.0;

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

// function loadGLTexture(imageFileName) 
// {
//     var tex = gl.createTexture();
//     tex.image = new Image(); //javascript API(class)
//     tex.image.src = imageFileName;

//     tex.image.onload = function ()  //lambda function(clouser)
//     {
//         gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
//         gl.bindTexture(gl.TEXTURE_2D, tex);
//         gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
//         gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);  // here webGL recmonded nearest_nearest
//         gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, tex.image);
//         gl.generateMipmap(gl.TEXTURE_2D);
//         gl.bindTexture(gl.TEXTURE_2D, null);
//         return tex;

//         // call this function as follows
//         // 1) assume that we had already globaly declered var textureSmiley = null;
//         // 2) then initialize at proper corrsponding place call this function as follow
//         // 3) textureSmiley = loadGLTexture("smiley.png")

//     }
// }

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
            "in vec3 aPosition;\n" +
            "out vec3 aTexCoords;\n" +
            "uniform mat4 projection;\n" +
            "uniform mat4 view;\n" +
            "uniform mat4 uModelMatrix;\n" +
            "void main(void)\n" +
            "{\n" +
            "aTexCoords = aPosition;\n" +
            "vec4 pos  = projection *  view * vec4(aPosition,1.0);\n" +
            "gl_Position = pos.xyzw;\n" +
            //"gl_Position=uModelMatrix*vec4(aPosition,1.0);\n" +
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
            "in vec3 aTexCoords;\n" +
            "uniform samplerCube skybox;\n" +
            "out vec4 FragColor;\n" +
            "void main(void)\n" +
            "{\n" +
            "FragColor = texture(skybox, aTexCoords);\n" +
            //"FragColor=vec4(1.0f,1.0f,1.0f,1.0f);\n" +
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
    projectionUniform = gl.getUniformLocation(shaderProgramObject, "projection");
    viewUniform = gl.getUniformLocation(shaderProgramObject, "view");
    modelUniform = gl.getUniformLocation(shaderProgramObject,"uModelMatrix");
    textureSkyboxUniform = gl.getUniformLocation(shaderProgramObject, "skybox");

    // provide vertex position,color,texcode, and normal etc.
    var skyboxVertices = new Float32Array([
        // positions
        -1.0, 1.0, -1.0,
        -1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        1.0, -1.0, -1.0, //
        1.0, 1.0, -1.0,
        -1.0, 1.0, -1.0, //

        -1.0, -1.0, 1.0,
        -1.0, -1.0, -1.0,
        -1.0, 1.0, -1.0,
        -1.0, 1.0, -1.0, //
        -1.0, 1.0, 1.0,
        -1.0, -1.0, 1.0, //

        1.0, -1.0, -1.0,
        1.0, -1.0, 1.0,
        1.0, 1.0, 1.0,
        1.0, 1.0, 1.0,  //
        1.0, 1.0, -1.0, //
        1.0, -1.0, -1.0,

        -1.0, -1.0, 1.0,
        -1.0, 1.0, 1.0,
        1.0, 1.0, 1.0,
        1.0, 1.0, 1.0, //
        1.0, -1.0, 1.0,
        -1.0, -1.0, 1.0, //

        // ok
        -1.0, 1.0, -1.0,
        1.0, 1.0, -1.0,
        1.0, 1.0, 1.0,
        1.0, 1.0, 1.0,
        -1.0, 1.0, 1.0,
        -1.0, 1.0, -1.0,

        -1.0, -1.0, -1.0,
        -1.0, -1.0, 1.0,
        1.0, -1.0, -1.0,
        1.0, -1.0, -1.0,
        -1.0, -1.0, 1.0,
        1.0, -1.0, 1.0]);

    // triangle_position is a veriable and its class type object of Float32Array

    //create varetx array
    vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

    vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);

    gl.bufferData(gl.ARRAY_BUFFER, skyboxVertices, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);


    // ********************  cubeMap_texture   ********************
    cubeMap_texture = loadGLTexture();

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
    //parse Float = 
}

function display() {

    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    let Radian = angleCube_MP * Math.PI / 180.0;

    var modelViewMatrix = mat4.create();
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var translationMatrix = mat4.create();
    var rotationMatrix = mat4.create();

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -4.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, Radian);
   // mat4.multiply(modelMatrix, translationMatrix, rotationMatrix);
    modelMatrix = translationMatrix;
    viewMatrix = rotationMatrix;

    //send this matrix to the vertex shader to the uniform    
    gl.uniformMatrix4fv(projectionUniform, false, perspectiveProjectionMatrix);
    gl.uniformMatrix4fv(viewUniform, false, viewMatrix);
    gl.uniformMatrix4fv(modelUniform,false, modelMatrix);

    // for texture
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_CUBE_MAP, cubeMap_texture);
    gl.uniform1i(textureSkyboxUniform, 0);

    //bind with vao
    gl.bindVertexArray(vao);

    //draw vertex array
    gl.drawArrays(gl.TRIANGLES, 0, 36);
    gl.bindVertexArray(null);

    gl.bindTexture(gl.TEXTURE_CUBE_MAP, null);

    //unUse shader program
    gl.useProgram(null);



    //update
    update();

    // double buffering
    requestAnimationFrame(display, canvas);
}


function update() {
    angleCube_MP = angleCube_MP - 0.03;

    if (angleCube_MP <= 0.0) {
        angleCube_MP = angleCube_MP + 360.0;
    }
}

function uninitialize() {
    //code
    if (bFullscreen == true) {
        toggleFullscreen();
    }

    if (vbo) {
        gl.deleteBuffer(vbo);
        vbo = null;
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


function loadGLTexture() {
    // console.log("entered into loadCubemapTexture\n" +  imageFileName);
    const cubeMap_texture = gl.createTexture();

    if (!cubeMap_texture)
        console.error('Failed to create webGLTexture for cubemap');

    const cubeMapPath = [
        "right.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"];
    if (!cubeMapPath)
        console.error('Failed to load image path for cubemap');

    const cubemapFaces = [
        gl.TEXTURE_CUBE_MAP_POSITIVE_X,
        gl.TEXTURE_CUBE_MAP_NEGATIVE_X,
        gl.TEXTURE_CUBE_MAP_POSITIVE_Y,
        gl.TEXTURE_CUBE_MAP_NEGATIVE_Y,
        gl.TEXTURE_CUBE_MAP_POSITIVE_Z,
        gl.TEXTURE_CUBE_MAP_NEGATIVE_Z];
    if (!cubemapFaces)
        console.error('Failed to load cube faces for cubemap');


    for (let i = 0; i < 6; i++) 
    {
        const image = new Image(); //javascript API(class)
        image.src = cubeMapPath[i];


        image.onload = function ()  //lambda function(clouser)
        {
            gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, false);
            gl.bindTexture(gl.TEXTURE_CUBE_MAP, cubeMap_texture);

            gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
            gl.texParameteri(gl.TEXTURE_CUBE_MAP, gl.TEXTURE_WRAP_R, gl.CLAMP_TO_EDGE);
            gl.texImage2D(cubemapFaces[i], 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);

            gl.generateMipmap(gl.TEXTURE_CUBE_MAP);
            console.log(" Generate TEXTURE_CUBE_MAP successfully!");

            gl.bindTexture(gl.TEXTURE_CUBE_MAP, null);
        }
    }
    // image.onerror = function () { console.log("Cubemap tex failed to load at path\n" + cubeMap_texture); }

    return cubeMap_texture;

}

