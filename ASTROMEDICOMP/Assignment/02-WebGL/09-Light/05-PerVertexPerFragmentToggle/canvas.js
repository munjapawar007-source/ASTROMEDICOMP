var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width = 0;
var canvas_original_height = 0;

var shaderProgramObject_PV = null;
var shaderProgramObject_PF = null;

// FOR PER VERTEX VERIABLE
var perspectiveProjectionMatrix_PV;
var modelMatrixUniform_PV = 0;
var viewMatrixUniform_PV = 0;
var projectionMatrixUniform_PV = 0;
var laUniform_PV = 0;
var ldUniform_PV = 0;
var lsUniform_PV = 0;
var kaUniform_PV = 0;
var kdUniform_PV = 0;
var ksUniform_PV = 0;
var materialShininaseUniform_PV = 0;
var lightPositionUniform_PV = 0;
var LKeyPressedUniform_PV = 0;

// FOR PER FRAGMENT VERIABLE
var perspectiveProjectionMatrix_PF;
var modelMatrixUniform_PF = 0;
var viewMatrixUniform_PF = 0;
var projectionMatrixUniform_PF = 0;
var laUniform_PF = 0;
var ldUniform_PF = 0;
var lsUniform_PF = 0;
var kaUniform_PF = 0;
var kdUniform_PF = 0;
var ksUniform_PF = 0;
var materialShininaseUniform_PF = 0;
var lightPositionUniform_PF = 0;
var LKeyPressedUniform_PF = 0;

var perVertex = true;	 // display vertex light
var perFragment = false; // display fargment light


var lightAmbient = [0.0, 0.0, 0.0];
var lightSpecular = [1.0, 1.0, 1.0];
var lightPosition = [100.0, 100.0, 100.0, 1.0];
var lightDiffuse = [1.0, 1.0, 1.0];

var materialAmbient = [0.0, 0.0, 0.0];
var materialSpecular = [1.0, 1.0, 1.0];
var materialShininnes = 100.0;
var materialDiffuse = [0.50, 0.50, 0.50];

var bAnimation = false;
var bLight = false;
var Angle = 0.0;

// sphere
var sphere = null;



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
        case 27:  // Esc
            if (bFullscreen == false) {
                toggleFullscreen();
                bFullscreen = true;
            }
            else {
                toggleFullscreen();
                bFullscreen = false;
            }
            break;
        case 65: // 'A':
        case 97: // 'a':
            if (bAnimation == false) {
                bAnimation = true;
            }
            else {
                bAnimation = false;
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
        case 70:  // F
        case 102: // f
            if (perFragment == true) {
                perFragment = false;
                perVertex = true;
            }
            else {
                perFragment = true;
                perVertex = false;
            }
            break;
        case 86:  // V
        case 118: // v
            if (perVertex == true) {
                perFragment = true;
                perVertex = false;
            }
            else {
                perFragment = false;
                perVertex = true;
            }
            break;

        case 81:  // Q
        case 113: // q
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

    vertexShader();
    fragmantShader();

    sphere = new Mesh();
    makeSphere(sphere, 2.0, 50, 50);

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
    if (bFullscreen == true) 
    {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else 
    {
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

    let Radian = Angle * Math.PI / 180.0;

    var modelMatrix = mat4.create();
    var viewMatrix = mat4.create();
    var translationMatrix = mat4.create();
    var RotationMatrix_X = mat4.create();
    var RotationMatrix_Y = mat4.create();
    var RotationMatrix_Z = mat4.create();
    var RotationMatrix = mat4.create();


    mat4.translate(translationMatrix, translationMatrix, [0.0, 0.0, -6.0]);
    mat4.rotateY(RotationMatrix_Y, RotationMatrix_Y, Radian);
    mat4.rotateZ(RotationMatrix_Z, RotationMatrix_Z, Radian);
    mat4.rotateX(RotationMatrix_X, RotationMatrix_X, Radian);
    mat4.multiply(RotationMatrix, RotationMatrix_X, RotationMatrix_Y, RotationMatrix_Z);

    mat4.multiply(modelMatrix, translationMatrix, RotationMatrix);
    mat4.identity(viewMatrix);
 
    
   
    
    if ( perVertex == true)
	{
		gl.useProgram(shaderProgramObject_PV);

        //send this matrix to the vertex shader to the uniform
        gl.uniformMatrix4fv(modelMatrixUniform_PV, false, modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform_PV, false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform_PV, false, perspectiveProjectionMatrix);

	}
	else 
	{
		gl.useProgram(shaderProgramObject_PF);
        
        //send this matrix to the vertex shader to the uniform
        gl.uniformMatrix4fv(modelMatrixUniform_PF, false, modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform_PF, false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform_PF, false, perspectiveProjectionMatrix);

	}




    // per vertex
    if (bLight == true) {
        gl.uniform3fv(laUniform_PV, lightAmbient);
        gl.uniform3fv(ldUniform_PV, lightDiffuse);
        gl.uniform3fv(lsUniform_PV, lightSpecular);

        gl.uniform4fv(lightPositionUniform_PV, lightPosition);

        gl.uniform3fv(kaUniform_PV, materialAmbient);
        gl.uniform3fv(kdUniform_PV, materialDiffuse);
        gl.uniform3fv(ksUniform_PV, materialSpecular);

        gl.uniform1f(materialShininaseUniform_PV, materialShininnes);

        gl.uniform1i(LKeyPressedUniform_PV, 1);
    }
    else {
        gl.uniform1i(LKeyPressedUniform_PV, 0);
    }


    // per fragment
    if (bLight == true) {
        gl.uniform3fv(laUniform_PF, lightAmbient);
        gl.uniform3fv(ldUniform_PF, lightDiffuse);
        gl.uniform3fv(lsUniform_PF, lightSpecular);

        gl.uniform4fv(lightPositionUniform_PF, lightPosition);

        gl.uniform3fv(kaUniform_PF, materialAmbient);
        gl.uniform3fv(kdUniform_PF, materialDiffuse);
        gl.uniform3fv(ksUniform_PF, materialSpecular);

        gl.uniform1f(materialShininaseUniform_PF, materialShininnes);

        gl.uniform1i(LKeyPressedUniform_PF, 1);
    }
    else {
        gl.uniform1i(LKeyPressedUniform_PF, 0);
    }


    // Draw sphere here
    sphere.draw();


    //unUse shader program
    gl.useProgram(null);


    //update
    if (bAnimation == true)
        update();

    // double buffering
    requestAnimationFrame(display, canvas);
}

function update() {
    Angle = Angle + 0.4;
    if (Angle >= 360.0) {
        Angle = Angle - 360.0;
    }
}

function uninitialize() {
    //code
    if (bFullscreen == true) {
        toggleFullscreen();
    }

    if (sphere) {
        sphere.deallocate();
        sphere = null;
    }


    //detach delete shader program object
    if (shaderProgramObject_PV) {
        gl.useProgram(shaderProgramObject_PV);
        var shaderObject = gl.getAttachedShaders(shaderProgramObject_PV);

        for (let i = 0; i < shaderObject.length; i++) {
            gl.detachShader(shaderProgramObject_PV, shaderObject[i]);
            gl.deleteShader(shaderObject[i]);
            shaderObject[i] = null;
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject_PV);
        shaderProgramObject_PV = null;
    }

      if (shaderProgramObject_PF) {
        gl.useProgram(shaderProgramObject_PF);
        var shaderObject = gl.getAttachedShaders(shaderProgramObject_PF);

        for (let i = 0; i < shaderObject.length; i++) {
            gl.detachShader(shaderProgramObject_PF, shaderObject[i]);
            gl.deleteShader(shaderObject[i]);
            shaderObject[i] = null;
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject_PF);
        shaderProgramObject_PF = null;
    }
}

function vertexShader()
{
        // Vertex Shader
    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    var vertexShaderSourceCode_PV =
        (
            "#version 300 es\n" +
            "in vec4 aPosition;\n" +
            "in vec3 aNormal;\n" +
            "uniform mat4 uModelMatrix;\n" +
            "uniform mat4 uViewMatrix;\n" +
            "uniform mat4 uProjectionMatrix;\n" +
            "uniform vec3 uLa;\n" +
            "uniform vec3 uLd;\n" +
            "uniform vec3 uLs;\n" +
            "uniform vec3 uKa;\n" +
            "uniform vec3 uKd;\n" +
            "uniform vec3 uKs;\n" +
            "uniform float uMaterialShinines;\n" +
            "uniform vec4 uLightPosition;\n" +
            "uniform int uLkeyPressed;\n" +
            "out vec3 out_phong_ads_Light;\n" +
            "void main(void)\n" +
            "{gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" +
            "if\n" +
            "(\n" +
            "uLkeyPressed == 1\n" +
            ")\n" +
            "{\n" +
            "vec4 eyeCoordinate = uViewMatrix * uModelMatrix * aPosition;\n" +
            "mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" +
            "vec3 transformedNormal = normalize(normalMatrix * aNormal);\n" +
            "vec3 lightDirection = normalize(vec3(uLightPosition - eyeCoordinate));\n" +
            "vec3 ambientLight = uLa * uLa;\n" +
            "vec3 diffuseLight = uLd * uKd * max(dot(lightDirection,transformedNormal),0.0);\n" +
            "vec3 reflectionVector = reflect(-lightDirection,transformedNormal);\n" +
            "vec3 viwerVector =normalize(-eyeCoordinate.xyz);\n" +
            "vec3 specularLight = uLs * uKs * pow(max(dot(reflectionVector,viwerVector), 0.0),uMaterialShinines);\n" +
            "out_phong_ads_Light = ambientLight + diffuseLight + specularLight; \n" +
            "}\n" +
            "else\n" +
            "{\n" +
            "out_phong_ads_Light = vec3(1.0,1.0,1.0);\n" +
            "}\n" +
            "}\n"
        );

    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode_PV);
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
    var fargamentShaderSourceCode_PV =
        (
            "#version 300 es\n" +
            "precision highp float;" +
            "in vec3 out_phong_ads_Light;\n" +
            "out vec4 FragColor;\n" +
            "void main(void)\n" +
            "{FragColor=vec4(out_phong_ads_Light,1.0);\n" +
            "}\n"
        );

    gl.shaderSource(fragmentShaderObject, fargamentShaderSourceCode_PV);
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

    shaderProgramObject_PV = gl.createProgram();
    gl.attachShader(shaderProgramObject_PV, vertexShaderObject);
    gl.attachShader(shaderProgramObject_PV, fragmentShaderObject);

    // prelink attributs
    gl.bindAttribLocation(shaderProgramObject_PV, WebGLMacros.VDG_ATTRIBUTE_VERTEX, "aPosition");
    gl.bindAttribLocation(shaderProgramObject_PV, WebGLMacros.VDG_ATTRIBUTE_NORMAL, "aNormal");

    //link
    gl.linkProgram(shaderProgramObject_PV);
    if (gl.getProgramParameter(shaderProgramObject_PV, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject_PV);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }
    else {
        console.log(" Shader program link successfully");
    }

    // get uniform
    modelMatrixUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uModelMatrix");
    viewMatrixUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uViewMatrix");
    projectionMatrixUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uProjectionMatrix");
    laUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uLa");
    ldUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uLd");
    lsUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uLs");

    lightPositionUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uLightPosition");

    kaUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uKa");
    kdUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uKd");
    ksUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uKs");
    materialShininaseUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uMaterialShinines");
    LKeyPressedUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uLkeyPressed");
}

function fragmantShader()
{
    
    // Vertex Shader
    var vertexShaderObject = gl.createShader(gl.VERTEX_SHADER);
    var vertexShaderSourceCode_PF =
        (
            "#version 300 es\n" +
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
            "{gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;\n" +
            "if\n" +
            "(\n" +
            "uLkeyPressed == true\n" +
            ")\n" +
            "{\n" +
            "vec4 eyeCoordinate = uViewMatrix * uModelMatrix * aPosition;\n" +
            "mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);\n" +
            "out_transformedNormals = normalMatrix * aNormal;\n" +
            "out_lightDirection = vec3(uLightPosition - eyeCoordinate);\n" +
            "out_viwerVector = -eyeCoordinate.xyz;\n" +
            "}\n" +
            "}\n"
        );

    gl.shaderSource(vertexShaderObject, vertexShaderSourceCode_PF);
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
    var fargamentShaderSourceCode_PF =
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
            "if\n" +
            "(\n" +
            "uLkeyPressed == true\n" +
            ")\n" +
            "{\n" +
            "vec3 normalizedTransformedNormal =  normalize(out_transformedNormals);\n" +
            "vec3 normalizedLightDirection = normalize(out_lightDirection);\n" +
            "vec3 normalizedViwerVector = normalize(out_viwerVector);\n" +
            "vec3 ambientLight = uLa * uKa;\n" +
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

    gl.shaderSource(fragmentShaderObject, fargamentShaderSourceCode_PF);
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

    shaderProgramObject_PF = gl.createProgram();
    gl.attachShader(shaderProgramObject_PF, vertexShaderObject);
    gl.attachShader(shaderProgramObject_PF, fragmentShaderObject);

    // prelink attributs
    gl.bindAttribLocation(shaderProgramObject_PF, WebGLMacros.VDG_ATTRIBUTE_VERTEX, "aPosition");
    gl.bindAttribLocation(shaderProgramObject_PF, WebGLMacros.VDG_ATTRIBUTE_NORMAL, "aNormal");

    //link
    gl.linkProgram(shaderProgramObject_PF);
    if (gl.getProgramParameter(shaderProgramObject_PF, gl.LINK_STATUS) == false) {
        var error = gl.getProgramInfoLog(shaderProgramObject_PF);
        if (error.length > 0) {
            alert(error);
            uninitialize();
        }
    }
    else {
        console.log(" Shader program link successfully");
    }

    // get uniform
    modelMatrixUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uModelMatrix");
    viewMatrixUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uViewMatrix");
    projectionMatrixUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uProjectionMatrix");
    laUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uLa");
    ldUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uLd");
    lsUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uLs");

    lightPositionUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uLightPosition");

    kaUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uKa");
    kdUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uKd");
    ksUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uKs");
    materialShininaseUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uMaterialShinines");
    LKeyPressedUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uLkeyPressed");

}

