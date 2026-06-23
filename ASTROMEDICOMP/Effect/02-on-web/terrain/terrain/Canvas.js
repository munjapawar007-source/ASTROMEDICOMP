// JavaScript source code
var canvas = null;
var gl = null;
var bFullScreen = false;
var canvas_original_width;
var canvas_original_height;
var requestAnimationFrame = window.requestAnimationFrame ||
                            window.mozRequestAnimationFrame ||
                            window.orequestAnimationFrame ||
                            window.webkitRequestAnimationFrame ||
    window.msRequestAnimationFrame;


var shaderProgramObject = null;
var vao          = null;
var vbo_position = null;
var vbo_color = null;
//delayed declarative statement
var mvpMatrxUniform;
var textureSamplerUniform;
var perspectiveProjectionMatrix;//undefined
//Texture related variables
var _texture;


const AMC_ATTRIBUTE_POSITION = 0;
const AMC_ATTRIBUTE_TEXCOORD = 1;


const NUM_ROWS = 113;
const NUM_COLUMNS = 113;

var vao_terrain;
var vbo_terrain_position;
var vbo_tex_rectangle;

const quadPositions = [];
const imageUV = [];

function main()
{
    // get canvas
    canvas = document.getElementById("AMC");
    if (canvas == null)
    {
        console.log("Canvas element cannot be obtained.\n");
    }
    else
    {
        console.log("Canvas element successfully obtained.\n");
    }

    //Assigning width and height
    canvas_original_width  = canvas.width;
    canvas_original_height = canvas.height;
  
    //Register our callback functions as event listeners
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
    window.addEventListener("resize", resize, false);

    //Intitialise
    initialise();

    //Resize
    resize();

    //Display
    display();
}

function keyDown(event)
{
    //Code
    switch(event.keyCode)
        {
        case 70:
        case 102:
            if (bFullScreen == false) {
                toggleFullScreen();
                bFullScreen = true;
            }
            else {
                toggleFullScreen();
                bFullScreen = false;
            }
            break;

        case 69:
        case 101:
            //Letter E
            uninitialise();
            window.close();
            break;
        }
}

function mouseDown()
{
    //Code
   
}

function toggleFullScreen()
{
    //Code
    var fullScreen_element =
        document.fullscreenelement ||
        document.mozFullScreenElement ||
        document.webkitFullscreenElement ||
        document.msFullscreenElement ||
        null;

    if (fullScreen_element == null) {
        if (canvas.requestFullscreen)//chrome
            canvas.requestFullscreen();

        else if (canvas.mozRequestFullScreen)//mozzilla
            canvas.mozRequestFullScreen();

        else if (canvas.webkitRequestFullscreen)//safari
            canvas.webkitReqeustFullscreen();

        else if (canvas.msRequestFullscreen)//edge
            canvas.msRequestFullscreen();
    }
    else
    {
        if (document.exitFullscreen) {
            document.exitFullscreen();
        }
        else if (document.mozExitFullScreen) {
            document.mozExitFullScreen();
        }
        else if (document.webkitExitFullscreen) {
            document.webkitExitFullscreen();
        }
        else if (document.msExitfullscreen) {
            document.msExitfullscreen();
        }
    }
}

//STUB function
function initialise()
{
    //Code
    //get 2d context from canvas
    gl = canvas.getContext("webgl2");
    if (gl == null) {
        console.log("Context (webgl2) element cannot be obtained.\n");
    }
    else {
        console.log("Context (webgl2) element successfully obtained.\n");
    }

    //Set viewport width & height
    gl.viewportWidth  = canvas.width;
    gl.viewportHeight = canvas.height;

    //VERTEX SHADER
    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);

    var vertexShaderSourceCode =
        (   "#version 300 es\n" +
            "in vec4 aPosition;\n" +
            "in vec2 aTexCoord;\n" +
		    "out vec2 out_TexCord;\n"  +
            "uniform mat4 uMVPMatrix;\n" +
            "uniform sampler2D uTextureSampler;\n" +
            "void main(void)\n" +
            "{\n" +
            "float imageHeight = texture(uTextureSampler,aTexCoord).r;\n" +
            "vec4 newPosition = vec4(aPosition.x,aPosition.y,-0.2*imageHeight,1.0f);\n" +
            "gl_Position=uMVPMatrix*newPosition;\n" +
            "out_TexCord = aTexCoord;\n" +
            "}\n"
        );
     

    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);

    gl.compileShader(vertexShaderObject);

    if (gl.getShaderParameter(vertexShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if (error.length > 0) {
            alert(error);
            uninitialise();
        }
    }
    else {
        console.log("Vertex shader compilation is successful.\n");
    }

   

    //FRAGMENT SHADER
    var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);

    var fragmentShaderSourceCode = 
    (
       "#version 300 es\n" +
       "precision highp float;" +
       "in vec2 out_TexCord;\n" +
       "uniform sampler2D uTextureSampler;\n" +
       "out vec4 FragColor;\n" +
       "void main(void)\n" +
       "{\n" +
       "FragColor = texture(uTextureSampler,out_TexCord);\n" +
       "}"
    );

    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);

    gl.compileShader(fragmentShaderObject);

    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0) {
            alert(error);
            uninitialise();
        }
    }
    else {
        console.log("Fragment shader compilation is successful.\n");
    }

    shaderProgramObject = gl.createProgram();

    //Attach shader
    gl.attachShader(shaderProgramObject, vertexShaderObject);
    gl.attachShader(shaderProgramObject, fragmentShaderObject);

    //Bind Attributes
    gl.bindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");
    gl.bindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");

    //Link program
    gl.linkProgram(shaderProgramObject);

    if (gl.getProgramParameter(shaderProgramObject, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if (error.length > 0) {
            alert(error);
            uninitialise();
        }
    }
    else {
        console.log("Shader program linking successful.\n");
    }

    //Get uniform location
    mvpMatrxUniform = gl.getUniformLocation(shaderProgramObject, "uMVPMatrix");
    textureSamplerUniform = gl.getUniformLocation(shaderProgramObject, "uTextureSampler");

    initializeTerrain();

    _texture = loadGLTexture("resources/sahyadri.png");

    //Depth Enable
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    //Tell OpenGL To Choose The Color To Clear Screen
    gl.clearColor(0.35, 0.35, 0.35, 1.0);

    //internally identity karto
    perspectiveProjectionMatrix = mat4.create();
}


function initializeTerrain() {


    for (let i = 0; i < NUM_ROWS; i++) {
        for (let j = 0; j < NUM_COLUMNS; j++) {

            let x0 = (i / NUM_ROWS) - 0.5;
            let y0 = (j / NUM_COLUMNS) - 0.5;
            let x1 = ((i + 1.0) / NUM_ROWS) - 0.5;
            let y1 = ((j + 1.0) / NUM_COLUMNS) - 0.5;

            const pA = [x0, y1, 0.0];
            const pB = [x0, y0, 0.0];
            const pC = [x1, y0, 0.0];
            const pD = [x1, y1, 0.0];

            quadPositions.push(pA);
            quadPositions.push(pB);
            quadPositions.push(pC);
            quadPositions.push(pD);


            let u0 = i / NUM_ROWS;
            let v0 = j / NUM_COLUMNS;
            let u1 = (i + 1.0) / NUM_ROWS;
            let v1 = (j + 1.0) / NUM_COLUMNS;

            const tA = [u0, v1];
            const tB = [u0, v0];
            const tC = [u1, v0];
            const tD = [u1, v1];

            imageUV.push(tA);
            imageUV.push(tB);
            imageUV.push(tC);
            imageUV.push(tD);
        }
    }
    console.log("Quad Positions");
    console.log(quadPositions);

    console.log("imageUV");
    console.log(imageUV);

    vao_terrain = gl.createVertexArray();
    gl.bindVertexArray(vao_terrain);

    vbo_position = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position);
     gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(quadPositions), gl.STATIC_DRAW);
  
    gl.vertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
    //unbind vbo position
    gl.bindBuffer(gl.ARRAY_BUFFER, null);


    vbo_tex_rectangle = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_tex_rectangle);
    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(imageUV), gl.STATIC_DRAW);
    gl.vertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD);
    //unbind vbo position
    gl.bindBuffer(gl.ARRAY_BUFFER, null);

    //unbind vao
    gl.bindVertexArray(null);
}

function resize()
{
    //Code
    if (bFullScreen == true) {
        canvas.width  = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {
        canvas.width  = canvas_original_width;
        canvas.height = canvas_original_height;
    }
    gl.viewport(0, 0, canvas.width, canvas.height);

    //Set Perspective Projection
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0);
    //1st FOV-Y(Field of View)
    //2nd Aspect Ratio
    //3rd Near
    //4th Far
}
var t = 0.0;
function display()
{
    //Code
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);

    gl.useProgram(shaderProgramObject);

    var modelViewMatrix   = mat4.create();
    var translationMatrix = mat4.create();
    var scaleMatrix = mat4.create();
    var rotationMatrix = mat4.create();
    var modelViewProjectionMatrix = mat4.create();

    t = t + 0.1;

    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, 4.0]);
    mat4.scale(scaleMatrix, scaleMatrix, [25.0, 25.0, 25.0]);
    
    mat4.multiply(modelViewMatrix, translationMatrix, scaleMatrix);

    mat4.rotateX(modelViewMatrix, modelViewMatrix, t);

    mat4.multiply(modelViewProjectionMatrix, perspectiveProjectionMatrix, modelViewMatrix);

    gl.uniformMatrix4fv(mvpMatrxUniform, false, modelViewProjectionMatrix);

    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, _texture);
    gl.uniform1i(textureSamplerUniform, 0);


    //Bind with vao

    //code
    // bind with vao
    gl.bindVertexArray(vao_terrain);
    // draw vertex array
    gl.drawArrays(gl.QUADS, 0, quadPositions.length / 3);
    gl.bindVertexArray(null);

    //un bind shader program object
    gl.useProgram(null);

    update();// here if required

    //Double buffering
    requestAnimationFrame(display, canvas);
}

function update()
{
    //Code
}
function uninitialise()
{
    //Code
    
    if (bFullScreen == true) {
        toggleFullScreen();
    }

    uninitializeTerrain();
    //Detach, Delete Shader Objects & Delete Shader Program Object
    if (shaderProgramObject) {
        gl.useProgram(shaderProgramObject);
        var shaderObjects = gl.getAttachedShaders(shaderProgramObject);
        for (let i = 0; i < shaderObjects.length; i++) {
            gl.detachShader(shaderProgramObject, shaderObjects[i]);
            gl.deleteShader(shaderObjects[i]);
            shaderObjects[i] = null;
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }
}

function uninitializeTerrain() {
    //Code
    //VBO 
    if (vbo_tex_rectangle) {
        gl.deleteBuffer(vbo_tex_rectangle);
        vbo_tex_rectangle = null;
    }

    if (vbo_terrain_position) {
        gl.deleteBuffer(vbo_terrain_position);
        vbo_terrain_position = null;
    }

    //VAO
    if (vao_terrain) {
        gl.deleteVertexArray(vao_terrain);
        vao_terrain = null;
    }
}
