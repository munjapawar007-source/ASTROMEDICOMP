var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width = 0;
var canvas_original_height = 0;


var shaderProgramObject = null;  //object to give memory

var modelMatrixUniform;
var viewMatrixUniform;
var projectionMatrixUniform;

var perspectiveProjectionMatrix; //undefined

// sphere
var sphere = null;

// Light Related veriable
var laUniform = 0;
var ldUniform = 0; // Light Diffuse
var lsUniform = 0;

var kaUniform = 0;
var kdUniform = 0; // color of material
var ksUniform = 0;

var materialShininaseUniform;
var lightPositionUniform;
var LKeyPressedUniform;

var lightAmbient = [0.0, 0.0, 0.0];
var lightSpecular = [1.0, 1.0, 1.0];
var lightPosition = [100.0, 100.0, 100.0, 1.0];
var lightDiffuse = [1.0, 1.0, 1.0];

class Material {
    constructor() {
        this.materialAmbient =  [];
        this.materialDiffuse =  [];
        this.materialSpecular = [];
        this.materialShininess = 0;

    }

}
let materials = [];

var bAnimation = false;
var bLight = false;
var Angle = 0.0;

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
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.VDG_ATTRIBUTE_VERTEX, "aPosition");
    gl.bindAttribLocation(shaderProgramObject, WebGLMacros.VDG_ATTRIBUTE_NORMAL, "aNormal");

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
    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelMatrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");
    laUniform = gl.getUniformLocation(shaderProgramObject, "uLa");
    ldUniform = gl.getUniformLocation(shaderProgramObject, "uLd");
    lsUniform = gl.getUniformLocation(shaderProgramObject, "uLs");

    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "uLightPosition");

    kaUniform = gl.getUniformLocation(shaderProgramObject, "uKa");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "uKd");
    ksUniform = gl.getUniformLocation(shaderProgramObject, "uKs");
    materialShininaseUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialShinines");
    LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "uLkeyPressed");

    sphere = new Mesh();
    makeSphere(sphere, 2.0, 50, 50);

    // Depth initialization
    gl.clearDepth(1.0);
    gl.enable(gl.DEPTH_TEST);
    gl.depthFunc(gl.LEQUAL);

    // clear color
    gl.clearColor(0.80, 0.80, 0.80, 1.0);

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


function display() {
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    let Radian = Angle * Math.PI / 180.0;

    var viewMatrix = mat4.create();
    var RotationMatrix_X = mat4.create();
    var RotationMatrix_Y = mat4.create();
    var RotationMatrix_Z = mat4.create();
    var RotationMatrix = mat4.create();

     mat4.translate(viewMatrix, viewMatrix, [2.0, -0.20, -1.0]);
    mat4.rotateY(RotationMatrix_Y, RotationMatrix_Y, Radian);
    mat4.rotateZ(RotationMatrix_Z, RotationMatrix_Z, Radian);
    mat4.rotateX(RotationMatrix_X, RotationMatrix_X, Radian);
    mat4.multiply(RotationMatrix, RotationMatrix_X, RotationMatrix_Y, RotationMatrix_Z);

    //mat4.multiply(modelMatrix, translationMatrix, RotationMatrix);
   // mat4.identity(viewMatrix);

    //send this matrix to the vertex shader to the uniform
    // gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);

    if (bLight == true) {
        gl.uniform3fv(laUniform, lightAmbient);
        gl.uniform3fv(ldUniform, lightDiffuse);
        gl.uniform3fv(lsUniform, lightSpecular);
        gl.uniform4fv(lightPositionUniform, lightPosition);

        gl.uniform1i(LKeyPressedUniform, 1);
    }
    else {
        gl.uniform1i(LKeyPressedUniform, 0);
    }

    // sphere material
    sphereMaterial();

    // draw 24 sphere
    sphereUniform();

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


function sphereMaterial() {

    materials[0] = new Material();
    materials[0].materialAmbient = [0.0215, 0.1745, 0.0215];
    materials[0].materialDiffuse = [0.07568, 0.61424, 0.07568];
    materials[0].materialSpecular = [0.633, 0.727811, 0.633];
    materials[0].materialShininess = 0.6 * 128;

    materials[1] = new Material();
    materials[1].materialAmbient = [0.135, 0.2225, 0.1575];
    materials[1].materialDiffuse = [0.54, 0.89, 0.63];
    materials[1].materialSpecular = [0.316228, 0.316228, 0.316228];
    materials[1].materialShininess = 0.1 * 128;

    materials[2] = new Material();
    materials[2].materialAmbient = [0.05375, 0.05, 0.06625];
    materials[2].materialDiffuse = [0.18275, 0.17, 0.22525];
    materials[2].materialSpecular = [0.332741, 0.328634, 0.346435];
    materials[2].materialShininess = 0.3 * 128;

    materials[3] = new Material();
    materials[3].materialAmbient = [0.25, 0.20725, 0.20725];
    materials[3].materialDiffuse = [1.0, 0.829, 0.829];
    materials[3].materialSpecular = [0.296648, 0.296648, 0.296648];
    materials[3].materialShininess = 0.088 * 128;

    materials[4] = new Material();
    materials[4].materialAmbient = [0.1745, 0.01175, 0.01175];
    materials[4].materialDiffuse = [0.61424, 0.04136, 0.04136];
    materials[4].materialSpecular = [0.727811, 0.626959, 0.626959];
    materials[4].materialShininess = 0.6 * 128;

    materials[5] = new Material();
    materials[5].materialAmbient = [0.1, 0.18725, 0.1745];
    materials[5].materialDiffuse = [0.396, 0.74151, 0.69102];
    materials[5].materialSpecular = [0.297254, 0.30829, 0.306678];
    materials[5].materialShininess = 0.1 * 128;

    materials[6] = new Material();
    materials[6].materialAmbient = [0.329412, 0.223529, 0.027451];
    materials[6].materialDiffuse = [0.780392, 0.568627, 0.113725];
    materials[6].materialSpecular = [0.992157, 0.941176, 0.807843];
    materials[6].materialShininess = 0.21794872 * 128;

    materials[7] = new Material();
    materials[7].materialAmbient = [0.2125, 0.1275, 0.054];
    materials[7].materialDiffuse = [0.714, 0.4284, 0.18144];
    materials[7].materialSpecular = [0.393548, 0.271906, 0.166721];
    materials[7].materialShininess = 0.6 * 128;

    materials[8] = new Material();
    materials[8].materialAmbient = [0.25, 0.25, 0.25];
    materials[8].materialDiffuse = [0.4, 0.4, 0.4];
    materials[8].materialSpecular = [0.4, 0.4, 0.4];
    materials[8].materialShininess = 0.6 * 128;

    materials[9] = new Material();
    materials[9].materialAmbient = [0.19125, 0.0735, 0.0225];
    materials[9].materialDiffuse = [0.7038, 0.27048, 0.0828];
    materials[9].materialSpecular = [0.256777, 0.137622, 0.086014];
    materials[9].materialShininess = 0.1 * 128;

    materials[10] = new Material();
    materials[10].materialAmbient = [0.24725, 0.1995, 0.0745];
    materials[10].materialDiffuse = [0.75164, 0.60648, 0.22648];
    materials[10].materialSpecular = [0.628281, 0.555802, 0.366065];
    materials[10].materialShininess = 0.4 * 128;

    materials[11] = new Material();
    materials[11].materialAmbient = [0.19225, 0.19225, 0.19225];
    materials[11].materialDiffuse = [0.50754, 0.50754, 0.50754];
    materials[11].materialSpecular = [0.508273, 0.508273, 0.508273];
    materials[11].materialShininess = 0.4 * 128;

    materials[12] = new Material();
    materials[12].materialAmbient = [0.0, 0.0, 0.0];
    materials[12].materialDiffuse = [0.01, 0.01, 0.01];
    materials[12].materialSpecular = [0.50, 0.50, 0.50];
    materials[12].materialShininess = 0.25 * 128;

    materials[13] = new Material();
    materials[13].materialAmbient = [0.0, 0.1, 0.06];
    materials[13].materialDiffuse = [0.0, 0.50980392, 0.50980392];
    materials[13].materialSpecular = [0.50196078, 0.50196078, 0.50196078];
    materials[13].materialShininess = 0.25 * 128;

    materials[14] = new Material();
    materials[14].materialAmbient = [0.0, 0.0, 0.0];
    materials[14].materialDiffuse = [0.01, 0.35, 0.01];
    materials[14].materialSpecular = [0.45, 0.55, 0.45];
    materials[14].materialShininess = 0.25 * 128;

    materials[15] = new Material();
    materials[15].materialAmbient = [0.0, 0.0, 0.0];
    materials[15].materialDiffuse = [0.5, 0.0, 0.0];
    materials[15].materialSpecular = [0.7, 0.6, 0.6];
    materials[15].materialShininess = 0.25 * 128;

    materials[16] = new Material();
    materials[16].materialAmbient = [0.0, 0.0, 0.0];
    materials[16].materialDiffuse = [0.55, 0.55, 0.55];
    materials[16].materialSpecular = [0.70, 0.70, 0.70];
    materials[16].materialShininess = 0.25 * 128;

    materials[17] = new Material();
    materials[17].materialAmbient = [0.0, 0.0, 0.0];
    materials[17].materialDiffuse = [0.5, 0.5, 0.0];
    materials[17].materialSpecular = [0.60, 0.60, 0.50];
    materials[17].materialShininess = 0.25 * 128;

    materials[18] = new Material();
    materials[18].materialAmbient = [0.02, 0.02, 0.02];
    materials[18].materialDiffuse = [0.01, 0.01, 0.01];
    materials[18].materialSpecular = [0.4, 0.4, 0.4];
    materials[18].materialShininess = 0.078125 * 128;

    materials[19] = new Material();
    materials[19].materialAmbient = [0.0, 0.05, 0.05];
    materials[19].materialDiffuse = [0.4, 0.5, 0.5];
    materials[19].materialSpecular = [0.04, 0.7, 0.7];
    materials[19].materialShininess = 0.078125 * 128;

    materials[20] = new Material();
    materials[20].materialAmbient = [0.0, 0.05, 0.0];
    materials[20].materialDiffuse = [0.4, 0.5, 0.4];
    materials[20].materialSpecular = [0.04, 0.7, 0.04];
    materials[20].materialShininess = 0.078125 * 128;

    materials[21] = new Material();
    materials[21].materialAmbient = [0.05, 0.0, 0.0];
    materials[21].materialDiffuse = [0.5, 0.4, 0.4];
    materials[21].materialSpecular = [0.7, 0.04, 0.04];
    materials[21].materialShininess = 0.078125 * 128;

    materials[22] = new Material();
    materials[22].materialAmbient = [0.05, 0.05, 0.05];
    materials[22].materialDiffuse = [0.5, 0.5, 0.5];
    materials[22].materialSpecular = [0.7, 0.7, 0.7];
    materials[22].materialShininess = 0.078125 * 128;

    materials[23] = new Material();
    materials[23].materialAmbient = [0.05, 0.05, 0.0];
    materials[23].materialDiffuse = [0.5, 0.5, 0.4];
    materials[23].materialSpecular = [0.7, 0.7, 0.04];
    materials[23].materialShininess = 0.078125 * 128;

    materials[24] = new Material();
    materials[24].materialAmbient = [0.0215, 0.1745, 0.0215];
    materials[24].materialDiffuse = [0.07568, 0.61424, 0.07568];
    materials[24].materialSpecular = [0.633, 0.727811, 0.633];
    materials[24].materialShininess = 0.078125 * 128;
}

function sphereUniform() {

    // 1
    var modelMatrix = mat4.create();
    var translationMatrix = mat4.create();
    var scaleMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-5.5, 3.5, -6.0]);
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[0].materialAmbient);
        gl.uniform3fv(kdUniform, materials[0].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[0].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[0].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    // else {
    //     glUniform1i(LKeyPressedUniform, 0);
    // }
    // Draw sphere here
    sphere.draw();

    // 2
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-5.5, 2.30, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kdUniform, materials[1].materialDiffuse);
        gl.uniform3fv(kaUniform, materials[1].materialAmbient);
        gl.uniform3fv(ksUniform, materials[1].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[1].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    // 3
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-5.5, 1.10, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[2].materialAmbient);
        gl.uniform3fv(kdUniform, materials[2].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[2].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[2].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //4
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-5.50, -0.1, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[3].materialAmbient);
        gl.uniform3fv(kdUniform, materials[3].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[3].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[3].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //5
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-5.50, -1.3, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[4].materialAmbient);
        gl.uniform3fv(kdUniform, materials[4].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[4].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[4].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //6
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-5.50, -2.5, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[5].materialAmbient);
        gl.uniform3fv(kdUniform, materials[5].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[5].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[5].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();
    ////////////////////////////////////////////////////////////////////////////////

    //7
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-3.5, 3.5, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[6].materialAmbient);
        gl.uniform3fv(kdUniform, materials[6].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[6].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[6].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //8
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-3.5, 2.3, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[7].materialAmbient);
        gl.uniform3fv(kdUniform, materials[7].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[7].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[7].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //9
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-3.50, 1.1, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[8].materialAmbient);
        gl.uniform3fv(kdUniform, materials[8].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[8].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[8].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //10
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-3.50, -0.10, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[9].materialAmbient);
        gl.uniform3fv(kdUniform, materials[9].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[9].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[9].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //11
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-3.50, -1.30, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[10].materialAmbient);
        gl.uniform3fv(kdUniform, materials[10].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[10].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[10].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //12
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-3.50, -2.50, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[11].materialAmbient);
        gl.uniform3fv(kdUniform, materials[11].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[11].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[11].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    ////////////////////////////////////////////////////////////////////////////////

    //13
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-1.5, 3.5, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[12].materialAmbient);
        gl.uniform3fv(kdUniform, materials[12].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[12].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[12].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //14
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-1.5, 2.30, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[13].materialAmbient);
        gl.uniform3fv(kdUniform, materials[13].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[13].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[13].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //15
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-1.50, 1.10, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[14].materialAmbient);
        gl.uniform3fv(kdUniform, materials[14].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[14].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[14].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //16
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-1.5, -0.10, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);

    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[15].materialAmbient);
        gl.uniform3fv(kdUniform, materials[15].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[15].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[15].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //17
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-1.5, -1.30, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[16].materialAmbient);
        gl.uniform3fv(kdUniform, materials[16].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[16].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[16].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //18
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [-1.5, -2.50, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[17].materialAmbient);
        gl.uniform3fv(kdUniform, materials[17].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[17].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[17].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    ////////////////////////////////////////////////////////////////////////////////

    //19
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [0.5, 3.5, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[18].materialAmbient);
        gl.uniform3fv(kdUniform, materials[18].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[18].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[18].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //20
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [0.50, 2.30, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[19].materialAmbient);
        gl.uniform3fv(kdUniform, materials[19].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[19].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[19].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //21
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [0.50, 1.1, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[20].materialAmbient);
        gl.uniform3fv(kdUniform, materials[20].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[20].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[20].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //22
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [0.50, -0.10, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[21].materialAmbient);
        gl.uniform3fv(kdUniform, materials[21].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[21].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[21].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //23
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [0.50, -1.30, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[22].materialAmbient);
        gl.uniform3fv(kdUniform, materials[22].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[22].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[22].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();

    //24
    modelMatrix = mat4.create();
    translationMatrix = mat4.create();
    mat4.translate(translationMatrix, translationMatrix, [0.50, -2.5, -6.0]);
    scaleMatrix = mat4.create();
    mat4.scale(scaleMatrix, scaleMatrix, [0.2, 0.2, 0.2]);
    mat4.multiply(modelMatrix, translationMatrix, scaleMatrix);
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    if (bLight == true) {
        gl.uniform3fv(kaUniform, materials[23].materialAmbient);
        gl.uniform3fv(kdUniform, materials[23].materialDiffuse);
        gl.uniform3fv(ksUniform, materials[23].materialSpecular);
        gl.uniform1f(materialShininaseUniform, materials[23].materialShininess);
        gl.uniform1i(LKeyPressedUniform, 1);
    }
    //else {
    //  glUniform1i(LKeyPressedUniform, 0);
    //  }
    // Draw sphere here
    sphere.draw();
}
