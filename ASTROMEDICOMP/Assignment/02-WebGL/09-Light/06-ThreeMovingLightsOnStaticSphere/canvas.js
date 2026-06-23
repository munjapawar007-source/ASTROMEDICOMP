var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width = 0;
var canvas_original_height = 0;

var shaderProgramObject_PV = null;
var shaderProgramObject_PF = null;

// FOR PER VERTEX VERIABLE
var perspectiveProjectionMatrix_PV;
var modelMatrixUniform_PV;
var viewMatrixUniform_PV;
var projectionMatrixUniform_PV;
var laUniform_PV = [];
var ldUniform_PV = [];
var lsUniform_PV = [];
var kaUniform_PV = 0;
var kdUniform_PV = 0;
var ksUniform_PV = 0;
var materialShininaseUniform_PV;
var lightPositionUniform_PV = [];
let LKeyPressedUniform_PV;

// FOR PER FRAGMENT VERIABLE
var perspectiveProjectionMatrix_PF;
var modelMatrixUniform_PF;
var viewMatrixUniform_PF;
var projectionMatrixUniform_PF;
var laUniform_PF = [];
var ldUniform_PF = [];
var lsUniform_PF = [];
var kaUniform_PF = 0;
var kdUniform_PF = 0;
var ksUniform_PF = 0;
var materialShininaseUniform_PF = 0;
var lightPositionUniform_PF = [];
let LKeyPressedUniform_PF;

var perVertex = true;	 // display vertex light
var perFragment = true; // display fargment light


var materialAmbient = [1.0, 1.0, 1.0];
var materialSpecular = [1.0, 1.0, 1.0];
var materialShininnes = 125.0;
var materialDiffuse = [0.80, 0.80, 0.80];

var bAnimation = false;
var bLight = false;
var Angle = 0.0;


let myLight = [];
myLight[0] = {};
myLight[1] = {};
myLight[2] = {};

let myPosition = [];
myPosition[0] = {};
myPosition[1] = {};
myPosition[2] = {};

var anglePos1 = 0.0;
var anglePos2 = 0.0;
var anglePos3 = 0.0;

// sphere
var sphere = null;

var requestAnimationFrame = window.requestAnimationFrame ||      // crome
    window.mozRequestAnimationFrame ||                           // mozila
    window.oRequestAnimationFrame ||                             // opera
    window.webkitAnimationFrame ||                               // safari
    window.msRequestAnimationFrame;                              // microsoft Edge

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
    gl.clearColor(0.20, 0.20, 0.20, 1.0);

    // initialize of light
    myLight[0].ambient = [0.0, 0.0, 0.0];
    myLight[0].diffuse = [1.0, 0.0, 0.0];
    myLight[0].specular = [1.0, 0.0, 0.0];

    myLight[1].ambient = [0.0, 0.0, 0.0];
    myLight[1].diffuse = [0.0, 0.0, 1.0];
    myLight[1].specular = [0.0, 0.0, 1.0];

    myLight[2].ambient = [0.0, 0.0, 0.0];
    myLight[2].diffuse = [0.0, 1.0, 0.0];
    myLight[2].specular = [0.0, 1.0, 0.0];

    perspectiveProjectionMatrix_PV = mat4.create();
    perspectiveProjectionMatrix_PF = mat4.create();

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
    mat4.perspective(perspectiveProjectionMatrix_PF, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0)
    mat4.perspective(perspectiveProjectionMatrix_PV, 45.0, parseFloat(canvas.width) / parseFloat(canvas.height), 0.1, 100.0)


}

function display() {

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

    if (perVertex == false && perFragment == true) {
        gl.useProgram(shaderProgramObject_PV);

        //send this matrix to the vertex shader to the uniform
        gl.uniformMatrix4fv(modelMatrixUniform_PV, false, modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform_PV, false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform_PV, false, perspectiveProjectionMatrix_PV);

    }
    else if (perFragment == false && perVertex == true) {
        gl.useProgram(shaderProgramObject_PF);
        //per fragment
        gl.uniformMatrix4fv(modelMatrixUniform_PF, false, modelMatrix);
        gl.uniformMatrix4fv(viewMatrixUniform_PF, false, viewMatrix);
        gl.uniformMatrix4fv(projectionMatrixUniform_PF, false, perspectiveProjectionMatrix_PF);
    }


    myPosition[0].position0 = [Math.sin(anglePos1), 1.0 + Math.cos(anglePos1), 0.0, 1.0];
    myPosition[1].position1 = [2.0 + Math.sin(anglePos2), 0.0, Math.cos(anglePos2), 1.0];
    myPosition[2].position2 = [Math.cos(anglePos3), Math.sin(anglePos3), 0.0, 1.0];

    if (bLight == true) {
        // 1) send proper size 
        gl.uniform3fv(laUniform_PV[0], myLight[0].ambient);
        gl.uniform3fv(ldUniform_PV[0], myLight[0].diffuse);
        gl.uniform3fv(lsUniform_PV[0], myLight[0].specular);
        gl.uniform4fv(lightPositionUniform_PV[0], myPosition[0].position0);

        gl.uniform3fv(laUniform_PV[1], myLight[1].ambient);
        gl.uniform3fv(ldUniform_PV[1], myLight[1].diffuse);
        gl.uniform3fv(lsUniform_PV[1], myLight[1].specular);
        gl.uniform4fv(lightPositionUniform_PV[1], myPosition[1].position1);

        gl.uniform3fv(laUniform_PV[2], myLight[2].ambient);
        gl.uniform3fv(ldUniform_PV[2], myLight[2].diffuse);
        gl.uniform3fv(lsUniform_PV[2], myLight[2].specular);
        gl.uniform4fv(lightPositionUniform_PV[2], myPosition[2].position2);

        gl.uniform3fv(kaUniform_PV, materialAmbient);
        gl.uniform3fv(kdUniform_PV, materialDiffuse);
        gl.uniform3fv(ksUniform_PV, materialSpecular);

        gl.uniform1f(materialShininaseUniform_PV, materialShininnes);

        gl.uniform1i(LKeyPressedUniform_PV, 1);
    }
    else {
        gl.uniform1i(LKeyPressedUniform_PV, 0);
    }




    if (bLight == true) 
    {

        gl.uniform3fv(laUniform_PF[0], myLight[0].ambient);
        gl.uniform3fv(ldUniform_PF[0], myLight[0].diffuse);
        gl.uniform3fv(lsUniform_PF[0], myLight[0].specular);
        gl.uniform4fv(lightPositionUniform_PF[0], myPosition[0].position0);

        gl.uniform3fv(laUniform_PF[1], myLight[1].ambient);
        gl.uniform3fv(ldUniform_PF[1], myLight[1].diffuse);
        gl.uniform3fv(lsUniform_PF[1], myLight[1].specular);
        gl.uniform4fv(lightPositionUniform_PF[1], myPosition[1].position1);

        gl.uniform3fv(laUniform_PF[2], myLight[2].ambient);
        gl.uniform3fv(ldUniform_PF[2], myLight[2].diffuse);
        gl.uniform3fv(lsUniform_PF[2], myLight[2].specular);
        gl.uniform4fv(lightPositionUniform_PF[2], myPosition[2].position2);

        gl.uniform3fv(kaUniform_PF, materialAmbient);
        gl.uniform3fv(kdUniform_PF, materialDiffuse);
        gl.uniform3fv(ksUniform_PF, materialSpecular);

        gl.uniform1f(materialShininaseUniform_PF, materialShininnes);

        gl.uniform1i(LKeyPressedUniform_PF, 1);
    }
    else 
    {
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

    anglePos1 = anglePos1 + 0.1;
	if (anglePos1 > 360.0)
	{
		anglePos1 = anglePos1 - 360.0;
	}

	anglePos2 = anglePos2 + 0.1;
	if (anglePos2 > 360.0)
	{
		anglePos2 = anglePos2 - 360.0;
	}

	anglePos3 = anglePos3 + 0.1;
	if (anglePos3 > 360.0)
	{
		anglePos3 = anglePos3 - 360.0;
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

function vertexShader() {
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
            "uniform vec3 uLa[3];\n" +
            "uniform vec3 uLd[3];\n" +
            "uniform vec3 uLs[3];\n" +
            "uniform vec3 uKa;\n" +
            "uniform vec3 uKd;\n" +
            "uniform vec3 uKs;\n" +
            "uniform float uMaterialShinines;\n" +
            "uniform vec4 uLightPosition[3];\n" +
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
            "vec3 viwerVector =normalize(-eyeCoordinate.xyz);\n" +
            "vec3 lightDirection[3];\n" +
            "vec3 ambientLight[3];\n" +
            "vec3 diffuseLight[3];\n" +
            "vec3 reflectionVector[3];\n" +
            "vec3 specularLight[3];\n" +
            "out_phong_ads_Light = vec3(0.0,0.0,0.0);\n" +
            "for\n" +
            "(\n" +
            "int i=0;i<3;i++\n" +
            ")\n" +
            "{\n" +
            "lightDirection[i]=normalize(vec3(uLightPosition[i] - eyeCoordinate));\n" +
            "ambientLight[i] = uLa[i] * uKa;\n" +
            "diffuseLight[i] = uLd[i] * uKd * max(dot(lightDirection[i],transformedNormal),0.0);\n" +
            "reflectionVector[i] = reflect(-lightDirection[i],transformedNormal);\n" +
            "specularLight[i] = uLs[i] * uKs * pow(max(dot(reflectionVector[i],viwerVector), 0.0),uMaterialShinines);\n" +
            "out_phong_ads_Light = out_phong_ads_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];\n" +
            "}\n" +
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
    laUniform_PV[0] = gl.getUniformLocation(shaderProgramObject_PV, "uLa[0]");
    ldUniform_PV[0] = gl.getUniformLocation(shaderProgramObject_PV, "uLd[0]");
    lsUniform_PV[0] = gl.getUniformLocation(shaderProgramObject_PV, "uLs[0]");
    lightPositionUniform_PV[0] = gl.getUniformLocation(shaderProgramObject_PV, "uLightPosition[0]");

    laUniform_PV[1] = gl.getUniformLocation(shaderProgramObject_PV, "uLa[1]");
    ldUniform_PV[1] = gl.getUniformLocation(shaderProgramObject_PV, "uLd[1]");
    lsUniform_PV[1] = gl.getUniformLocation(shaderProgramObject_PV, "uLs[1]");
    lightPositionUniform_PV[1] = gl.getUniformLocation(shaderProgramObject_PV, "uLightPosition[1]");

    laUniform_PV[2] = gl.getUniformLocation(shaderProgramObject_PV, "uLa[2]");
    ldUniform_PV[2] = gl.getUniformLocation(shaderProgramObject_PV, "uLd[2]");
    lsUniform_PV[2] = gl.getUniformLocation(shaderProgramObject_PV, "uLs[2]");
    lightPositionUniform_PV[2] = gl.getUniformLocation(shaderProgramObject_PV, "uLightPosition[2]");

    kaUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uKa");
    kdUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uKd");
    ksUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uKs");
    materialShininaseUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uMaterialShinines");
    LKeyPressedUniform_PV = gl.getUniformLocation(shaderProgramObject_PV, "uLkeyPressed");
}

function fragmantShader() {

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
            "out vec3 out_lightDirection[3];\n" +
            "uniform vec4 uLightPosition[3];\n" +
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
            "for\n" +
            "(\n" +
            "int i=0;i<3;i++\n" +
            ")\n" +
            "{\n" +
            "out_lightDirection[i] = normalize(vec3(uLightPosition[i] - eyeCoordinate));\n" +
            "out_viwerVector = -eyeCoordinate.xyz;\n" +
            "}\n" +
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
            "in vec3 out_lightDirection[3];\n" +
            "out vec4 FragColor;\n" +
            "uniform vec3 uLa[3];\n" +
            "uniform vec3 uLd[3];\n" +
            "uniform vec3 uLs[3];\n" +
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
            "vec3 normalizedLightDirection[3];\n" +
            "vec3 ambientLight[3];\n" +
            "vec3 diffuseLight[3];\n" +
            "vec3 reflectionVector[3];\n" +
            "vec3 specularLight[3];\n" +
            "vec3 normalizedTransformedNormal =  normalize(out_transformedNormals);\n" +
            "vec3 normalizedViwerVector = normalize(out_viwerVector);\n" +
            "for\n" +
            "(\n" +
            "int i=0;i<3;i++\n" +
            ")\n" +
            "{\n" +
            "normalizedLightDirection[i] = normalize(out_lightDirection[i]);\n" +
            "ambientLight[i] = uLa[i] * uKa;\n" +
            "diffuseLight[i] = uLd[i] * uKd * max(dot(normalizedLightDirection[i],normalizedTransformedNormal),0.0);\n" +
            "reflectionVector[i] = reflect(-normalizedLightDirection[i],normalizedTransformedNormal);\n" +
            "specularLight[i] = uLs[i] * uKs * pow(max(dot(reflectionVector[i],normalizedViwerVector), 0.0),uMaterialShinines);\n" +
            "phong_ads_Light = phong_ads_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];\n" +
            "}\n" +
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

    laUniform_PF[0] = gl.getUniformLocation(shaderProgramObject_PF, "uLa[0]");
    ldUniform_PF[0] = gl.getUniformLocation(shaderProgramObject_PF, "uLd[0]");
    lsUniform_PF[0] = gl.getUniformLocation(shaderProgramObject_PF, "uLs[0]");
    lightPositionUniform_PF[0] = gl.getUniformLocation(shaderProgramObject_PF, "uLightPosition[0]");

    laUniform_PF[1] = gl.getUniformLocation(shaderProgramObject_PF, "uLa[1]");
    ldUniform_PF[1] = gl.getUniformLocation(shaderProgramObject_PF, "uLd[1]");
    lsUniform_PF[1] = gl.getUniformLocation(shaderProgramObject_PF, "uLs[1]");
    lightPositionUniform_PF[1] = gl.getUniformLocation(shaderProgramObject_PF, "uLightPosition[1]");

    laUniform_PF[2] = gl.getUniformLocation(shaderProgramObject_PF, "uLa[2]");
    ldUniform_PF[2] = gl.getUniformLocation(shaderProgramObject_PF, "uLd[2]");
    lsUniform_PF[2] = gl.getUniformLocation(shaderProgramObject_PF, "uLs[2]");
    lightPositionUniform_PF[2] = gl.getUniformLocation(shaderProgramObject_PF, "uLightPosition[2]");

    kaUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uKa");
    kdUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uKd");
    ksUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uKs");
    materialShininaseUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uMaterialShinines");
    LKeyPressedUniform_PF = gl.getUniformLocation(shaderProgramObject_PF, "uLkeyPressed");

}

