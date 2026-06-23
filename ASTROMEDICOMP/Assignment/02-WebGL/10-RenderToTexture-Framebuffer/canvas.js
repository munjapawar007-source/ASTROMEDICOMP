var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width = 0;
var canvas_original_height = 0;

// micros
const FBO_WIDTH  = 512;
const FBO_HEIGHT = 512;

// webGL related veriables
const MyAttributs =
{
    AMC_ATTRIBUTE_POSITION: 0,
	AMC_ATTRIBUTE_COLOR:    1,
	AMC_ATTRIBUTE_TEXCOORD:  2,
	AMC_ATTRIBUTE_VERTEX:   3,
	AMC_ATTRIBUTE_NORMAL:   4,
};

var shaderProgramObject = null;  //object to give memory

var vao_cube = null;
var vbo_position_cube = null;  //uninitialized
var vbo_TexCoord_cube = null;  //uninitialized
var mvpUniform; //undefined (delyed declerative)
var perspectiveProjectionMatrix; //undefined

// Rotation
var Angle = 0.0;

// Texture
var textureSamplerUniform;

// Fbo related global veriables
var winWidth;
var winHeight;

var fbo;
var rbo;
var fbo_Texture;
var fboResult = -1;

// Sphere Related Veriables
var sphere = null;
var perspectiveProjectionMatrix_sphere;
var shaderProgramObject_sphere;

var modelMatrixUniform_sphere;
var viewMatrixUniform_sphere;
var projectionMatrixUniform_sphere;

// Light Related veriable
var laUniform_sphere = 0;
var ldUniform_sphere = 0; 
var lsUniform_sphere = 0; 

var kaUniform_sphere = 0;
var kdUniform_sphere = 0; 
var ksUniform_sphere = 0;

var materialShininaseUniform_sphere;
var lightPositionUniform_sphere;
var LKeyPressedUniform_sphere;

var lightAmbient_sphere = [0.0, 0.0, 0.0];
var lightSpecular_sphere = [1.0, 1.0, 1.0];
var lightPosition_sphere = [100.0, 100.0, 100.0, 1.0];
var lightDiffuse_sphere = [1.0, 1.0, 1.0];

var materialAmbient_sphere = [0.0, 0.0, 0.0];
var materialSpecular_sphere = [0.700, 0.70, 0.70];
var materialShininnes_sphere = 128.0;
var materialDiffuse_sphere = [0.50, 0.20, 0.70];

var bLight = false;

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
        case 76:  //'L':
        case 108: //'l':
            if (bLight == false) {
                bLight = true;
            }
            else {
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
    var cube_position = new Float32Array([
        // front
        1.0,  1.0, 1.0,	// top-right of front
        -1.0,  1.0, 1.0,	// top-left of front
        -1.0, -1.0, 1.0, // bottom-left of front
        1.0, -1.0, 1.0,	// bottom-right of front

        // right
        1.0,  1.0, -1.0,	// top-right of right
        1.0,  1.0,  1.0,	// top-left of right
        1.0, -1.0,  1.0,	// bottom-left of right
        1.0, -1.0, -1.0, // bottom-right of right

        // back
        1.0,  1.0, -1.0,	 // top-right of back
        -1.0,  1.0, -1.0,	 // top-left of back
        -1.0, -1.0, -1.0, // bottom-left of back
        1.0, -1.0, -1.0,	 // bottom-right of back

        // left
        -1.0,  1.0,  1.0,	 // top-right of left
        -1.0,  1.0, -1.0,	 // top-left of left
        -1.0, -1.0, -1.0, // bottom-left of left
        -1.0, -1.0,  1.0,	 // bottom-right of left

        // top
        1.0, 1.0, -1.0,	// top-right of top
        -1.0, 1.0, -1.0, // top-left of top
        -1.0, 1.0,  1.0,	// bottom-left of top
        1.0, 1.0,  1.0,	// bottom-right of top

        // bottom
        1.0, -1.0,  1.0,	 // top-right of bottom
        -1.0, -1.0,  1.0,	 // top-left of bottom
        -1.0, -1.0, -1.0, // bottom-left of bottom
        1.0, -1.0, -1.0,	 // bottom-right of bottom
    ]);

    var cube_TexCoords = new Float32Array([
     
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


    //create varetx array
    vao_cube = gl.createVertexArray();
    gl.bindVertexArray(vao_cube);

    vbo_position_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_position_cube);

    gl.bufferData(gl.ARRAY_BUFFER, cube_position, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION, 3, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);

    // texcoord
    vbo_TexCoord_cube = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vbo_TexCoord_cube)
    gl.bufferData(gl.ARRAY_BUFFER, cube_TexCoords, gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_TEXCOORD, 2, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_TEXCOORD);

    gl.bindBuffer(gl.ARRAY_BUFFER, null);
    gl.bindVertexArray(null);

 
    // Depth initialization
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // clear color
    gl.clearColor(0.0, 0.0, 0.0, 1.0);

    perspectiveProjectionMatrix = mat4.create();

    // create fbo and if succssefull call sphere
	if (createAndPrepareFBOForDrawing(FBO_WIDTH,FBO_HEIGHT) == true)
	{
		console.log("initialize: FBO creation successfull\n");
		fboResult = initialize_Sphere();

		if (fboResult != 0)
		{
			console.log("initialize: fbo failed\n");
			
		}
		else
		{
			console.log("initialize: initialize_Sphere SUCCESSFULL\n");
		}
	}
	else
	{
		console.log("initialize: fbo creation failed\n");	
	}


}


function createAndPrepareFBOForDrawing( textureWidth, textureHeight)
{
	//veriable declaration

	//code
	//step1:check whethere texture width and height are compatiable
	var maxRenderBufferSize;
	maxRenderBufferSize = gl.getParameter(gl.MAX_RENDERBUFFER_SIZE);
	if(maxRenderBufferSize <= textureWidth || maxRenderBufferSize <= textureHeight )
	{
		console.log("fbo widht height are exceeding maxRenderBufferSize\n");
		return(false);
	}

	//step2: create new farme buffer for sphere
	fbo = gl.createFramebuffer();

	//step3:bind with newly created buffer
	gl.bindFramebuffer(gl.FRAMEBUFFER,fbo);
	
	//step4:create render buffer as place holder for our depth and our frame buffer
	rbo = gl.createRenderbuffer();

	//step5: bind this new crated render buffer
	gl.bindRenderbuffer(gl.RENDERBUFFER,rbo);

	//step6: give specific storeage to this render buffer so depth needed for sphere will reside here
	gl.renderbufferStorage(gl.RENDERBUFFER,gl.DEPTH_COMPONENT16,textureWidth,textureHeight); // 16 bit it is recommanded as depth for mobile

	//step7: create an empty texture but fully compactabale texture object for our incoming object
	fbo_Texture = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D,fbo_Texture);

	//step8: to create full fitured textured give appropreate parameter
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S,gl.CLAMP_TO_EDGE);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T,gl.CLAMP_TO_EDGE);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER,gl.LINEAR);
	gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER,gl.LINEAR);

	//stpe9: now create this texture for our fbo
	gl.texImage2D(gl.TEXTURE_2D,0,gl.RGB,textureWidth,textureHeight,0,gl.RGB,gl.UNSIGNED_SHORT_5_6_5,null); //5_6_5 for human unstood greenary

	//step10:
	gl.generateMipmap(gl.TEXTURE_2D);

	//step11: give this empty texture to frame buffer
	gl.framebufferTexture2D(gl.FRAMEBUFFER,gl.COLOR_ATTACHMENT0,gl.TEXTURE_2D,fbo_Texture,0);

	//step12: attach priviously created famebuffer to our frame buffer
	gl.framebufferRenderbuffer(gl.FRAMEBUFFER,gl.DEPTH_ATTACHMENT,gl.RENDERBUFFER,rbo);

	//step13: check whethere our frame buffer complete or not
	if(gl.checkFramebufferStatus(gl.FRAMEBUFFER)!= gl.FRAMEBUFFER_COMPLETE)
	{
		console.log("frame buffer complated\n");
		return(false);
	}

	//step14:unbind
	gl.bindFramebuffer(gl.FRAMEBUFFER,null);
	return (true);

}	

function initialize_Sphere()
{
     // Vertex Shader
    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    var vertexShaderSourceCode =
        (
            "#version 300 es \n" +
            "in vec4 aPosition;\n" +
            "in vec3 aNormal;\n" +
            "uniform mat4 uModelMatrix;\n" +
            "uniform mat4 uViewMatrix;\n" +
            "uniform mat4 uProjectionMatrix;\n" +
            "out vec3 out_transformedNormals;\n" +
            "out vec3 out_viwerVector;\n" +
            "out vec3 out_lightDirection;\n" +
            "uniform vec4 uLightPosition;\n" +
            "uniform bool uLkeyPressed;\n" +
            "void main(void)\n" +
            "{\n" +
                "gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" +
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
            uninitialize_sphere();
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
                    "vec3 normalizedViwerVector = normalize(out_lightDirection);\n" +
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
            "FragColor=vec4(phong_ads_Light,1.0);\n" +
            "}\n"
        );

    gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
    gl.compileShader(fragmentShaderObject);
    if (gl.getShaderParameter(fragmentShaderObject, gl.COMPILE_STATUS) == false) {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if (error.length > 0) {
            alert(error);
            uninitialize_sphere();
        }

    }
    else {
        console.log(" Fragment shader compilation successfull\n")
    }

    shaderProgramObject_sphere = gl.createProgram();
    gl.attachShader(shaderProgramObject_sphere, vertexShaderObject);
    gl.attachShader(shaderProgramObject_sphere, fragmentShaderObject);

    // prelink attributs
    gl.bindAttribLocation(shaderProgramObject_sphere, MyAttributs.AMC_ATTRIBUTE_POSITION, "aPosition");

    //link
    gl.linkProgram(shaderProgramObject_sphere);
    if (gl.getProgramParameter(shaderProgramObject_sphere, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject_sphere);
        if (error.length > 0) {
            alert(error);
            uninitialize_sphere();
        }
    }
    else {
        console.log(" Shader program link successfully");
    }

     // get uniform
    modelMatrixUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uModelMatrix");
    viewMatrixUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uViewMatrix");
    projectionMatrixUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uProjectionMatrix");
    laUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uLa");
    ldUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uLd");
    lsUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uLs");

    lightPositionUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uLightPosition");

    kaUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uKa");
    kdUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uKd");
    ksUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uKs");
    materialShininaseUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uMaterialShinines");
    LKeyPressedUniform_sphere = gl.getUniformLocation(shaderProgramObject_sphere, "uLkeyPressed");

    sphere = new Mesh();
    makeSphere(sphere, 2.0, 50, 50);

    // Depth initialization
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // clear color
    gl.clearColor(0.2, 0.3, 0.2, 1.0);

    perspectiveProjectionMatrix_sphere = mat4.create();

    return 0;
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

    winWidth = canvas.width;
    winHeight = canvas.height

    gl.viewport(0.0, 0.0, canvas.width, canvas.height);

    // set perspective projection
    mat4.perspective(perspectiveProjectionMatrix, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0)
    //parse Float = 
}

function resize_sphere(width, height) {
    // code
    if (bFullscreen == true) {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

    gl.viewport(0.0, 0.0, width, height);

    // set perspective projection
    mat4.perspective(perspectiveProjectionMatrix_sphere, 45.0, parseFloat(width) / parseFloat(height), 0.1, 100.0)
    //parse Float = 
}

function display() 
{   
	//call sphere related code
	if(fboResult === 0) //means initialize_sphere is successful
	{
		display_sphere();
	
	}

	//call cube resize to compatisate sphere's resize
	resize(winWidth,winHeight);

	//clear color again to compensate the effect clear color of sphere
	gl.clearColor(1.0,1.0,1.0,1.0);

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
    gl.bindTexture(gl.TEXTURE_2D, fbo_Texture);
    gl.uniform1i(textureSamplerUniform, 0);

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

    //unUse shader program
    gl.useProgram(null);

    //update
    update();

    // double buffering
    requestAnimationFrame(display, canvas);
}


function display_sphere()
{
	//do fbo related changes
	if(fbo)
	{
		gl.bindFramebuffer(gl.FRAMEBUFFER,fbo);
	}

	//call sphere resize to compasate to cube resize
	resize_sphere(FBO_WIDTH,FBO_HEIGHT);

	//call sphere clear color to compasate to cube clear color
	gl.clearColor(0.0,0.0,0.0,1.0);

	// code
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	// use Shader program object
	gl.useProgram(shaderProgramObject_sphere);

	// transformstions
    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var translationMatrix = mat4.create();
   //  var RotationMatrix = mat4.create();
   
    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);
  //  mat4.rotateY(RotationMatrix, RotationMatrix, Radian);
    //mat4.multiply(modelViewMatrix, translationMatrix, RotationMatrix);

    modelMatrix = translationMatrix
    mat4.multiply(modelMatrixUniform_sphere, perspectiveProjectionMatrix, modelMatrix);

	// send this matrix to the vertex shader to the uniform
	gl.uniformMatrix4fv(modelMatrixUniform_sphere, false, modelMatrix);
	gl.uniformMatrix4fv(viewMatrixUniform_sphere, false, viewMatrix);
	gl.uniformMatrix4fv(projectionMatrixUniform_sphere,  false, perspectiveProjectionMatrix_sphere);

	if (bLight == true)
	{
		gl.uniform3fv(laUniform_sphere, lightAmbient_sphere);
		gl.uniform3fv(ldUniform_sphere, lightDiffuse_sphere);
		gl.uniform3fv(lsUniform_sphere, lightSpecular_sphere);

		gl.uniform4fv(lightPositionUniform_sphere, lightPosition_sphere);

		gl.uniform3fv(kaUniform_sphere, materialAmbient_sphere);
		gl.uniform3fv(kdUniform_sphere, materialDiffuse_sphere);
		gl.uniform3fv(ksUniform_sphere, materialSpecular_sphere);

		gl.uniform1f(materialShininaseUniform_sphere, materialShininnes_sphere);

		gl.uniform1i(LKeyPressedUniform_sphere, 1);
	}
	else
	{
		gl.uniform1i(LKeyPressedUniform_sphere, 0);
	}

 sphere.draw();

	// unUse shader program
	gl.useProgram(null);

	if(fbo)
	{
		gl.bindFramebuffer(gl.FRAMEBUFFER, null);
	}
	
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

    if (vbo_TexCoord_cube) {
        gl.deleteBuffer(vbo_TexCoord_cube);
        vbo_TexCoord_cube = null;
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

function uninitialize_sphere()
{
    //detach delete shader program object
    if (shaderProgramObject_sphere) {
        gl.useProgram(shaderProgramObject_sphere);
        var shaderObject = gl.getAttachedShaders(shaderProgramObject_sphere);

        for (let i = 0; i < shaderObject.length; i++) {
            gl.detachShader(shaderProgramObject_sphere, shaderObject[i]);
            gl.deleteShader(shaderObject[i]);
            shaderObject[i] = null;
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject_sphere);
        shaderProgramObject_sphere = null;
    }
}
