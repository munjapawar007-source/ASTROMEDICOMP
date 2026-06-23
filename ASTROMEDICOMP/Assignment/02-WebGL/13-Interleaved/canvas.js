var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width = 0;
var canvas_original_height = 0;

// webGL related veriables
const MyAttributs =
{
    AMC_ATTRIBUTE_POSITION: 0,
    AMC_ATTRIBUTE_COLOR:    1,
    AMC_ATTRIBUTE_NORMAL:   2,
    AMC_ATTRIBUTE_TEXCOORD: 3,
};

var shaderProgramObject = null;  //object to give memory

var vao = null;
var vbo = null;  //uninitialized

//matrix
var perspectiveProjectionMatrix; //undefined

var modelMatrixUniform = 0;
var viewMatrixUniform = 0;
var projectionMatrixUniform = 0;

// Rotation
var Angle = 0.0;

// For Texture
var textureMarble;
var textureSamplerUniform;

// Light variable
var laUniform = 0;
var ldUniform = 0;
var lsUniform = 0;

var kaUniform = 0;
var kdUniform = 0; // color of material
var ksUniform = 0;

var materialShininaseUniform = 0;
var lightPositionUniform = 0;
var LKeyPressedUniform = 0;

var lightAmbient = [0.25, 0.25, 0.25];
var lightSpecular = [1.0, 1.0, 1.0];
var lightPosition = [100.0, 100.0, 100.0];
var lightDiffuse = [1.0, 1.0, 1.0];

var materialAmbient = [0.0, 0.0, 0.0];
var materialSpecular = [1.0, 1.0, 1.0];
var materialShininnes = 128.0;
var materialDiffuse = [0.50, 0.50, 0.50];

var bLight = false

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
        case 76: //L
		case 108: //l
			if (bLight == false)
			{
				bLight = true;
			}
			else
			{
				bLight = false;
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
            "in vec4 aColor;\n" +
            "in vec2 aTexCoord;\n" +
            "in vec3 aNormal;\n" +
            "\n" +
            "//uniform mat4 uMVPMatrix;\n" +
            "out vec4 out_color;\n" +
            "out vec2 out_TexCoord;\n" +
            "out vec3 out_transformedNormals;\n" +
            "out vec3 out_viwerVector;\n" +
            "out vec3 out_lightDirection;\n" +
            "\n" +
            "uniform vec4 uLightPosition;\n" +
            "uniform bool uLkeyPressed;\n" +
            "uniform mat4 uModelMatrix;\n" +
            "uniform mat4 uViewMatrix;\n" +
            "uniform mat4 uProjectionMatrix;\n" +
            "void main(void)\n" +
            "{\n" +
            "gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" +
            "out_color = aColor;\n" +
            "out_TexCoord = aTexCoord;\n" +
            "if(uLkeyPressed == true)\n" +
            "{\n" +
            "vec4 eyeCoordinate = uViewMatrix * uModelMatrix * aPosition;\n" +
            "mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" +
            "out_transformedNormals = normalMatrix * aNormal;\n" +
            "out_lightDirection = vec3(uLightPosition - eyeCoordinate);\n" +
            "out_viwerVector = -eyeCoordinate.xyz;\n" +
            "}\n" +
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
           "in vec4 out_color;\n" +
            "in vec2 out_TexCoord;\n" +
            "uniform sampler2D uTextureSampler;\n" +
            "in vec3 out_transformedNormals;\n" +
            "in vec3 out_viwerVector;\n" +
            "in vec3 out_lightDirection;\n" +
            "out vec4 FragColor;\n" +
            "uniform vec3 uLa;\n" +
            "uniform vec3 uLd;\n" +
            "uniform vec3 uLs;\n" +
            "uniform vec3 uKa;\n" +
            "uniform vec3 uKd;\n" +
            "uniform vec3 uKs;\n" +
            "uniform float uMaterialShinines;\n" +
            "uniform bool uLkeyPressed;\n" +
            "void main(void)\n" +
            "{\n" +
            "vec3 phong_ads_Light;\n" +
            "if(uLkeyPressed == true)\n" +
            "{\n" +
            "vec3 normalizedTransformedNormal =  normalize(out_transformedNormals);\n" +
            "vec3 normalizedLightDirection = normalize(out_lightDirection);\n" +
            "vec3 normalizedViwerVector = normalize(out_viwerVector);\n" +
            "vec3 ambientLight = uLa * uLa;\n" +
            "vec3 diffueLight = uLd * uKd * max(dot(normalizedLightDirection,normalizedTransformedNormal),0.0);\n" +
            "vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTransformedNormal);\n" +
            "vec3 specularLight = uLs * uKs * pow(max(dot(reflectionVector,normalizedViwerVector), 0.0),uMaterialShinines);\n" +
            "phong_ads_Light = ambientLight + diffueLight + specularLight; \n" +
            "}\n" +
            "else\n" +
            "{\n" +
            "phong_ads_Light = vec3(1.0,1.0,1.0);\n" +
            "}\n" +
            "vec4 tex = texture(uTextureSampler,out_TexCoord);\n" +
            "FragColor = out_color * texture(uTextureSampler,out_TexCoord) * vec4(phong_ads_Light, 1.0);\n" +
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
    gl.bindAttribLocation(shaderProgramObject, MyAttributs.AMC_ATTRIBUTE_POSITION, "aPosition"); // 0th index match cpu and gpu side
	gl.bindAttribLocation(shaderProgramObject, MyAttributs.AMC_ATTRIBUTE_COLOR, "aColor");
	gl.bindAttribLocation(shaderProgramObject, MyAttributs.AMC_ATTRIBUTE_NORMAL, "aNormal");
	gl.bindAttribLocation(shaderProgramObject, MyAttributs.AMC_ATTRIBUTE_TEXCOORD, "aTexCoord");


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

  	// set uniform here
	modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelMatrix");
	viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uViewMatrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");
	
	textureSamplerUniform =  gl.getUniformLocation(shaderProgramObject, "uTextureSampler");
	
	laUniform = gl.getUniformLocation(shaderProgramObject, "uLa");
	ldUniform = gl.getUniformLocation(shaderProgramObject, "uLd");
	lsUniform = gl.getUniformLocation(shaderProgramObject, "uLs");

	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "uLightPosition");

	kaUniform = gl.getUniformLocation(shaderProgramObject, "uKa");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "uKd");
	ksUniform = gl.getUniformLocation(shaderProgramObject, "uKs");
	materialShininaseUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialShinines");

	LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "uLkeyPressed");

    // provide vertex position,color,texcode, and normal etc.		
    var cube_PCNT =new Float32Array
    ([
	// front
	// position			// color		 // normals			// texcoords
	 1.0,  1.0,  1.0,	1.0, 0.0, 0.0,	 0.0,  0.0,  1.0,	1.0, 1.0,
	-1.0,  1.0,  1.0,	1.0, 0.0, 0.0,	 0.0,  0.0,  1.0,	0.0, 1.0,
	-1.0, -1.0,  1.0,	1.0, 0.0, 0.0,	 0.0,  0.0,  1.0,	0.0, 0.0,
	 1.0, -1.0,  1.0,	1.0, 0.0, 0.0,	 0.0,  0.0,  1.0,	1.0, 0.0,
						 
	// right			 
	// position			// color		// normals			// texcoords
	 1.0,  1.0, -1.0,	0.0, 0.0, 1.0,	 1.0,  0.0,  0.0,	1.0, 1.0,
	 1.0,  1.0,  1.0,	0.0, 0.0, 1.0,	 1.0,  0.0,  0.0,	0.0, 1.0,
	 1.0, -1.0,  1.0,	0.0, 0.0, 1.0,	 1.0,  0.0,  0.0,	0.0, 0.0,
	 1.0, -1.0, -1.0,	0.0, 0.0, 1.0,	 1.0,  0.0,  0.0,	1.0, 0.0,
						 
	// back				 
	// position			// color	     // normals	        // texcoords
	 1.0,  1.0, -1.0,	1.0, 1.0, 0.0,	 0.0,  0.0, -1.0,	1.0, 1.0,
	-1.0,  1.0, -1.0,	1.0, 1.0, 0.0,	 0.0,  0.0, -1.0,	0.0, 1.0,
	-1.0, -1.0, -1.0,	1.0, 1.0, 0.0,	 0.0,  0.0, -1.0,	0.0, 0.0,
	 1.0, -1.0, -1.0,	1.0, 1.0, 0.0,	 0.0,  0.0, -1.0,	1.0, 0.0,
						 
	// left				 
	// position			// color		 // normals			// texcoords
	-1.0,  1.0,  1.0,	1.0, 0.0, 1.0,	-1.0,  0.0,  0.0,	1.0, 1.0,
	-1.0,  1.0, -1.0,	1.0, 0.0, 1.0,	-1.0,  0.0,  0.0,	0.0, 1.0,
	-1.0, -1.0, -1.0,	1.0, 0.0, 1.0,	-1.0,  0.0,  0.0,	0.0, 0.0,
	-1.0, -1.0,  1.0,	1.0, 0.0, 1.0,	-1.0,  0.0,  0.0,	1.0, 0.0,
						 
	// top				 
	// position			// color	     // normals	        // texcoords
	 1.0,  1.0, -1.0,	0.0, 1.0, 0.0,	 0.0,  1.0,  0.0,	1.0, 1.0,
	-1.0,  1.0, -1.0,	0.0, 1.0, 0.0,	 0.0,  1.0,  0.0,	0.0, 1.0,
	-1.0,  1.0,  1.0,	0.0, 1.0, 0.0,	 0.0,  1.0,  0.0,	0.0, 0.0,
	 1.0,  1.0,  1.0,	0.0, 1.0, 0.0,	 0.0,  1.0,  0.0,	1.0, 0.0,
						 
	// bottom			 
	// position			// color		 // normals			// texcoords
	 1.0, -1.0,  1.0,	1.0, 0.5, 0.0,	 0.0, -1.0,  0.0,	1.0, 1.0,
	-1.0, -1.0,  1.0,	1.0, 0.5, 0.0,	 0.0, -1.0,  0.0,	0.0, 1.0,
	-1.0, -1.0, -1.0,	1.0, 0.5, 0.0,	 0.0, -1.0,  0.0,	0.0, 0.0,
	 1.0, -1.0, -1.0,	1.0, 0.5, 0.0,	 0.0, -1.0,  0.0,	1.0, 0.0,
    ]);

    

    //create varetx array
    // CUBE
	vao = gl.createVertexArray();
    gl.bindVertexArray(vao);

	// create a common vbo for p,c,t
	vbo = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
	gl.bufferData(gl.ARRAY_BUFFER, cube_PCNT, gl.STATIC_DRAW); // in android 2nd parameter: 24*11*4

    // float size for stride and offset
    const sizeOfFloat = Float32Array.BYTES_PER_ELEMENT; // 4 bytes
    
	// for position
	gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 11 * sizeOfFloat, 0);
	gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);

	// for color
	gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_COLOR, 3, gl.FLOAT, false, 11 * sizeOfFloat, 3 * sizeOfFloat);
	gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_COLOR);

	// for normals
	gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_NORMAL, 4, gl.FLOAT, false, 11 * sizeOfFloat, 6 * sizeOfFloat);
	gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_NORMAL);

	// for texcoord
	gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 11 * sizeOfFloat, 9 * sizeOfFloat);
	gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_TEXCOORD);

	gl.bindBuffer(gl.ARRAY_BUFFER, null);
	gl.bindVertexArray(null)

    // ********************  Texture   ********************
    textureMarble = loadGLTexture("Marble.png");
    console.log("entered into loadGLTexture()" + textureMarble);

    // Depth initialization
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

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

    let Radian = Angle * Math.PI / 180.0;

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var translationMatrix = mat4.create();
    var RotationMatrix_X = mat4.create();
    var RotationMatrix_Y = mat4.create();
    var RotationMatrix_Z = mat4.create();
    var RotationMatrix = mat4.create();


    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -4.0]);
    mat4.rotateY(RotationMatrix_Y, RotationMatrix_Y, Radian);
    mat4.rotateZ(RotationMatrix_Z, RotationMatrix_Z, Radian);
    mat4.rotateX(RotationMatrix_X, RotationMatrix_X, Radian);
    mat4.multiply( RotationMatrix, RotationMatrix_X,RotationMatrix_Z);
    mat4.multiply(modelMatrix, translationMatrix, RotationMatrix);
    mat4.identity(viewMatrix);

    //send this matrix to the vertex shader to the uniform
    gl.uniformMatrix4fv(modelMatrixUniform,  false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform,  false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform,  false, perspectiveProjectionMatrix);

    if (bLight == true)
	{
		gl.uniform3fv(ldUniform, lightDiffuse);
		gl.uniform3fv(kdUniform,  materialDiffuse);
		gl.uniform4fv(lightPositionUniform,  lightPosition);

		gl.uniform1i(LKeyPressedUniform, 1);
	}
	else
	{
		gl.uniform1i(LKeyPressedUniform, 0);
	}

    //for texture
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, textureMarble);
    gl.uniform1i(textureSamplerUniform, 0);

    //bind with vao
    gl.bindVertexArray(vao);

    //draw vertex array
    gl.drawArrays(gl.TRIANGLE_FAN, 0, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 4, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 8, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 12, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 16, 4);
	gl.drawArrays(gl.TRIANGLE_FAN, 20, 4);

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

    if(textureMarble)
	{
		gl.deleteTextures(textureMarble);
		textureMarble = null;
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


function loadGLTexture(imageFileName) {
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
