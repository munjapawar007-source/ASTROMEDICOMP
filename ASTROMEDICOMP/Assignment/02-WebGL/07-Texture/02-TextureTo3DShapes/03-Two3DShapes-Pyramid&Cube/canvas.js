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

var vao_cube = null;
var vbo_texcoord_cube = null;  //uninitialized
var vbo_position_cube = null;  //uninitialized

var vao_pyramid = null;
var vbo_texcoord_pyramid = null;
var vbo_position_pyramid = null;

var mvpUniform; //undefined (delyed declerative)

var perspectiveProjectionMatrix; //undefined

//rotation
var angleCube_MP = 0.0;
var texKundali;
var texStone;
var textureSamplerUniform;

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

    // get uniform
    mvpUniform = gl.getUniformLocation(shaderProgramObject, "uMVPMatrix");
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "uTextureSampler");

    // provide vertex position,color,texcode, and normal etc.
    var cube_position = new Float32Array([
        // front
        1.0, 1.0, 1.0,	// top-right of front
        -1.0, 1.0, 1.0,	// top-left of front
        -1.0, -1.0, 1.0, // bottom-left of front
        1.0, -1.0, 1.0,	// bottom-right of front

        // right
        1.0, 1.0, -1.0,	// top-right of right
        1.0, 1.0, 1.0,	// top-left of right
        1.0, -1.0, 1.0,	// bottom-left of right
        1.0, -1.0, -1.0, // bottom-right of right

        // back
        1.0, 1.0, -1.0,	 // top-right of back
        -1.0, 1.0, -1.0,	 // top-left of back
        -1.0, -1.0, -1.0, // bottom-left of back
        1.0, -1.0, -1.0,	 // bottom-right of back

        // left
        -1.0, 1.0, 1.0,	 // top-right of left
        -1.0, 1.0, -1.0,	 // top-left of left
        -1.0, -1.0, -1.0, // bottom-left of left
        -1.0, -1.0, 1.0,	 // bottom-right of left

        // top
        1.0, 1.0, -1.0,	// top-right of top
        -1.0, 1.0, -1.0, // top-left of top
        -1.0, 1.0, 1.0,	// bottom-left of top
        1.0, 1.0, 1.0,	// bottom-right of top

        // bottom
        1.0, -1.0, 1.0,	 // top-right of bottom
        -1.0, -1.0, 1.0,	 // top-left of bottom
        -1.0, -1.0, -1.0, // bottom-left of bottom
        1.0, -1.0, -1.0,	 // bottom-right of bottom 
    ]); // position

    var cube_texcoords = new Float32Array([
        // front
        1.0, 1.0, // top-right of front
        0.0, 1.0, // top-left of front
        0.0, 0.0, // bottom-left of front
        1.0, 0.0, // bottom-right of front

        // right
        1.0, 1.0, // top-right of right
        0.0, 1.0, // top-left of right
        0.0, 0.0, // bottom-left of right
        1.0, 0.0, // bottom-right of right

        // back
        1.0, 1.0, // top-right of back
        0.0, 1.0, // top-left of back
        0.0, 0.0, // bottom-left of back
        1.0, 0.0, // bottom-right of back

        // left
        1.0, 1.0, // top-right of left
        0.0, 1.0, // top-left of left
        0.0, 0.0, // bottom-left of left
        1.0, 0.0, // bottom-right of left

        // top
        1.0, 1.0, // top-right of top
        0.0, 1.0, // top-left of top
        0.0, 0.0, // bottom-left of top
        1.0, 0.0, // bottom-right of top

        // bottom
        1.0, 1.0, // top-right of bottom
        0.0, 1.0, // top-left of bottom
        0.0, 0.0, // bottom-left of bottom
        1.0, 0.0, // bottom-right of bottom   
    ]);

    var pyramid_position = new Float32Array([
        // front
        0.0, 1.0, 0.0,	    // front-top
        -1.0, -1.0, 1.0,    // front-left
        1.0, -1.0, 1.0,	    // front-right

        // right
        0.0, 1.0, 0.0,	    // right-top
        1.0, -1.0, 1.0,	    // right-left
        1.0, -1.0, -1.0,     // right-right

        // back
        0.0, 1.0, 0.0,	     // back-top
        1.0, -1.0, -1.0,	 // back-left
        -1.0, -1.0, -1.0,    // back-right

        // left
        0.0, 1.0, 0.0,	    // left-top
        -1.0, -1.0, -1.0,   // left-left
        -1.0, -1.0, 1.0]);  // left-right


    var pyramid_texcoords = new Float32Array([
        // front
        0.5, 1.0, // front-top
        0.0, 0.0, // front-left
        1.0, 0.0, // front-right

        // right
        0.5, 1.0, // right-top
        1.0, 0.0, // right-left
        0.0, 0.0, // right-right

        // back
        0.5, 1.0, // back-top
        0.0, 0.0, // back-left
        1.0, 0.0, // back-right

        // left
        0.5, 1.0, // left-top
        1.0, 0.0, // left-left
        0.0, 0.0, // left-right
    ]);


    //create varetx array and Bind cube
    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    // position
    vbo_position_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_cube);
    gl.bufferData(gl.ARRAY_BUFFER, cube_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);

    // Texcoord
    vbo_texcoord_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texcoord_cube);
    gl.bufferData(gl.ARRAY_BUFFER, cube_texcoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_TEXCOORD);

    // UNBIND Here
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    /* ---------------pyramid--------------- */
    //create varetx array and Bind pyramid
    vao_pyramid = gl.createVertexArray();
    gl.bindVertexArray(vao_pyramid);

    // position
    vbo_position_pyramid = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_pyramid);
    gl.bufferData(gl.ARRAY_BUFFER, pyramid_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);

    // Texcoord
    vbo_texcoord_pyramid = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_texcoord_pyramid);
    gl.bufferData(gl.ARRAY_BUFFER, pyramid_texcoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_TEXCOORD);

    // UNBIND Here
    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

    // ********************  Texture   ********************
    texKundali = loadGLTexture("Vijay_Kundali.png");
    console.log("entered into loadGLTexture()" + texKundali);

    texStone = loadGLTexture("stone.png");
    console.log("entered into loadGLTexture()" + texStone);

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
    mat4.translate(translationMatrix, translationMatrix, [-1.50, 0.0, -5.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, Radian);
    mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);
    var ModelViewProjectionMatrix = mat4.create();
    mat4.multiply(ModelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    //send this matrix to the vertex shader to the uniform
    gl.uniformMatrix4fv(mvpUniform, false, ModelViewProjectionMatrix);
     
    //for texture
	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D,texKundali);
	gl.uniform1i(textureSamplerUniform,0);

    //bind with vao_cube
    gl.bindVertexArray(vao_cube);

    //draw vertex array
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
    gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

    gl.bindVertexArray(null);
    gl.bindTexture(gl.TEXTURE_2D, null);

    /* ---------------pyramid--------------- */
    mat4.identity(modelViewMatrix);
    mat4.identity(translationMatrix);
    mat4.identity(rotationMatrix);
    mat4.translate(translationMatrix, translationMatrix, [1.50, 0.0, -5.0]);
    mat4.rotateY(rotationMatrix, rotationMatrix, Radian);
    mat4.multiply(modelViewMatrix, translationMatrix, rotationMatrix);
    mat4.identity(ModelViewProjectionMatrix);
    mat4.multiply(ModelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    //send this matrix to the vertex shader to the uniform
    gl.uniformMatrix4fv(mvpUniform, false, ModelViewProjectionMatrix);

    //for texture
	gl.activeTexture(gl.TEXTURE0);
	gl.bindTexture(gl.TEXTURE_2D,texStone);
	gl.uniform1i(textureSamplerUniform,0)

    //bind with vao_pyramid
    gl.bindVertexArray(vao_pyramid);

    //draw vertex array
    gl.drawArrays(gl.TRIANGLES, 0, 12);

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

    // if (vbo_texcoord_pyramid) {
    //     gl.deleteBuffer(vbo_texcoord_pyramid);
    //     vbo_texcoord_pyramid = null;
    // }

    if (vbo_position_pyramid) {
        gl.deleteBuffer(vbo_position_pyramid);
        vbo_position_pyramid = null;
    }

    if (vao_pyramid) {
        gl.deleteVertexArray(vao_pyramid);
        vao_pyramid = null;
    }

    if (vbo_texcoord_cube) {
        gl.deleteBuffer(vbo_texcoord_cube);
        vbo_texcoord_cube = null;
    }

    if (vbo_position_cube) {
        gl.deleteBuffer(vbo_position_cube);
        vbo_position_cube = null;
    }

    if (vao_cube) {
        gl.deleteVertexArray(vao_cube);
        vao_cube = null;
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

function loadGLTexture(imageFileName) 
{
    console.log("entered into loadGLTexture()");
    const tex = gl.createTexture();
    
    const image = new Image(); //javascript API(class)
    image.src = imageFileName;

    image.onload = function ()  //lambda function(clouser)
    {
        gl.pixelStorei(gl.UNPACK_FLIP_Y_WEBGL, true);
        gl.bindTexture(gl.TEXTURE_2D, tex);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
        gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR);  // here webGL recmonded nearest_nearest
        gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);
        gl.generateMipmap(gl.TEXTURE_2D);
        gl.bindTexture(gl.TEXTURE_2D, null);
    }
        
    return tex;
 
    // call this function as follows
    // 1) assume that we had already globaly declered var textureSmiley = null;
    // 2) then initialize at proper corrsponding place call this function as follow
    // 3) textureSmiley = loadGLTexture("smiley.png")  
}
