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
var laUniform = 0; // Ambient
var ldUniform = 0; // Diffuse
var lsUniform = 0; // Specular
var lightPositionUniform;
var lightSpotDirectionUniform = 0;
var lightSpotInnerCutOffAngleUniform = 0;
var lightSpotOuterCutOffAngleUniform = 0;
var lightTimeUniform = 0.0;

var kaUniform = 0; // Ambient
var kdUniform = 0; // Diffuse
var ksUniform = 0; // Specular

// material related uniforms
var materialAmbientUniform = 0;
var materialDiffuseUniform = 0;
var materialSpecularUniform = 0;
var materialShininessUniform = 0;


var LKeyPressedUniform;
let t;


var lightAmbient = [0.10, 0.10, 0.10];
var lightDiffuse = [1.0, 1.0, 1.0];
var lightSpecular = [1.0, 1.0, 1.0];
var lightPosition = new Float32Array([]);

var materialAmbient = [0.0, 0.0, 0.0];
var materialSpecular = [0.70, 0.70, 0.70];
var materialShininnes = 50.0;
var materialDiffuse = [0.50, 0.20, 0.70];

// spotlight related variables
var lightSpotDirection = [0.0, 0.0, -1.0];
var lightSpotInnerCutOffAngle = 0.25;
var lightSpotOuterCutOffAngle = 1.0;

var bAnimation = false;
var bLight = false;
var Angle = 0.0;

var requestAnimationFrame = window.requestAnimationFrame ||      // chrome
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
        `#version 300 es
    in vec4 aPosition;
    in vec3 aNormal;
    
    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    uniform vec4 uLightPosition;
    uniform bool uLkeyPressed;
    
    out vec3 out_transformedNormals;
    out vec3 out_lightDirection;
    out vec3 out_viwerVector;
    out vec3 out_worldPosition;
    
    void main(void)
    {
        if( uLkeyPressed == true)
        {
            vec4 eyeCoordinate = uViewMatrix * uModelMatrix * aPosition;
            mat3 normalMatrix = mat3(uViewMatrix * uModelMatrix);
            out_transformedNormals = normalMatrix * aNormal;
            out_lightDirection = vec3(uLightPosition - eyeCoordinate);
            out_viwerVector = -eyeCoordinate.xyz;
            out_worldPosition = vec3(uModelMatrix * aPosition);
        }
        else
        {
            out_transformedNormals  = vec3(0.0,0.0,0.0);
            out_lightDirection      = vec3(0.0,0.0,0.0);
            out_viwerVector         = vec3(0.0,0.0,0.0);
        }
        gl_Position=uProjectionMatrix * uViewMatrix * uModelMatrix * aPosition;     
    }`;

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
        `#version 300 es
        precision highp float;
        in vec3 out_transformedNormals;
        in vec3 out_viwerVector;
        in vec3 out_lightDirection;
        in vec3 out_worldPosition;
        
        uniform vec3 uLa;
        uniform vec3 uLd;
        uniform vec3 uLs;
        uniform vec4 uLightPosition;
        
        uniform vec3 uLightSpotDirection;
        uniform float uLightSpotInnerCutOffAngle;
        uniform float uLightSpotOuterCutOffAngle;
        uniform float uTime;
        
        uniform vec3 uKa;
        uniform vec3 uKd;
        uniform vec3 uKs;
        uniform float uMaterialShinines;
        
        uniform bool uLkeyPressed;
        out vec4 FragColor;
        
        void main(void)
        {
            vec3 phong_ads_Light;
            if(uLkeyPressed == true)
            {
                vec3 normalizedTransformedNormal =  normalize(out_transformedNormals);
                vec3 normalizedLightDirection = normalize(out_lightDirection);
                vec3 normalizedViwerVector = normalize(out_viwerVector);
                vec3 ambientLight = uLa * uKa;
                vec3 diffueLight = uLd * uKd * max(dot(normalizedLightDirection,normalizedTransformedNormal),0.0);
                vec3 reflectionVector = reflect(-normalizedLightDirection,normalizedTransformedNormal);
                vec3 specularLight = uLs * uKs * pow(max(dot(reflectionVector,normalizedViwerVector), 0.0),uMaterialShinines);
                float distanceToFragment = length(uLightPosition.xyz - out_worldPosition);
                float attenuation =1.0 / (1.0 + (0.000007 * distanceToFragment) + (0.000003 * (distanceToFragment * distanceToFragment)));
                float spotTheta =  dot(normalizedLightDirection , normalize(-uLightSpotDirection));
                float spotEpsilon = uLightSpotInnerCutOffAngle - uLightSpotOuterCutOffAngle;
                float spotIntesity = clamp((spotTheta - uLightSpotOuterCutOffAngle) / spotEpsilon, 0.0, 1.0);
                diffueLight = diffueLight * spotIntesity;
                specularLight = specularLight *spotIntesity;
                phong_ads_Light = ambientLight + diffueLight + specularLight; 
            }
            else
            {
                phong_ads_Light = vec3(1.0,1.0,1.0);
            }
                
            FragColor=vec4(phong_ads_Light,1.0);
        }`;

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

    // ****************** Get Shader Uniform locations (MUST get these locations after linkage) ******************
    // matrices
    modelMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelMatrix");
    viewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uViewMatrix");
    projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");

    // light related
    laUniform = gl.getUniformLocation(shaderProgramObject, "uLa");
    ldUniform = gl.getUniformLocation(shaderProgramObject, "uLd");
    lsUniform = gl.getUniformLocation(shaderProgramObject, "uLs");
    lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "uLightPosition");

    lightSpotDirectionUniform = gl.getUniformLocation(shaderProgramObject, "uLightSpotDirection");
    lightSpotInnerCutOffAngleUniform = gl.getUniformLocation(shaderProgramObject, "uLightSpotInnerCutOffAngle");
    lightSpotOuterCutOffAngleUniform = gl.getUniformLocation(shaderProgramObject, "uLightSpotOuterCutOffAngle");
    lightTimeUniform = gl.getUniformLocation(shaderProgramObject, "uTime");

    // material related
    kaUniform = gl.getUniformLocation(shaderProgramObject, "uKa");
    kdUniform = gl.getUniformLocation(shaderProgramObject, "uKd");
    ksUniform = gl.getUniformLocation(shaderProgramObject, "uKs");
    materialShininessUniform = gl.getUniformLocation(shaderProgramObject, "uMaterialShinines");

    // key press uniform
    LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "uLkeyPressed");

    /******************* SPHERE *******************/
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

    //send this matrix to the vertex shader to the uniform
    gl.uniformMatrix4fv(modelMatrixUniform, false, modelMatrix);
    gl.uniformMatrix4fv(viewMatrixUniform, false, viewMatrix);
    gl.uniformMatrix4fv(projectionMatrixUniform, false, perspectiveProjectionMatrix);
    //lightPosition[5.0,5.0,20.0];

    if (bLight == true) {
        gl.uniform3fv(laUniform, lightAmbient);
        gl.uniform3fv(ldUniform, lightDiffuse);
        gl.uniform3fv(lsUniform, lightSpecular);
        var lPosition = [];
        lightPosition[50.0, 50.0, 50.0, 1.0]

        gl.uniform4fv(lightPositionUniform, lightPosition);

        lightSpotDirection = [0.1, Math.cos(Radian), -2.0];

        gl.uniform3fv(lightSpotDirectionUniform, lightSpotDirection);

        gl.uniform1f(lightSpotInnerCutOffAngleUniform, Math.cos(degrees_to_radians(lightSpotInnerCutOffAngle + 0.2)));
        gl.uniform1f(lightSpotOuterCutOffAngleUniform, Math.cos(degrees_to_radians(lightSpotOuterCutOffAngle + 5.0)));
        gl.uniform1f(lightTimeUniform, Radian);

        // console.log("uTime",lightTimeUniform)

        gl.uniform3fv(kaUniform, materialAmbient);
        gl.uniform3fv(kdUniform, materialDiffuse);
        gl.uniform3fv(ksUniform, materialSpecular);

        gl.uniform1f(materialShininessUniform, materialShininnes);

        gl.uniform1i(LKeyPressedUniform, 1);
    }
    else {
        gl.uniform1i(LKeyPressedUniform, 0);
    }

    // Draw sphere here
    sphere.draw();


    //unUse shader program
    gl.useProgram(null);


    // //update
    // if (bAnimation == true)
    update();

    // double buffering
    requestAnimationFrame(display, canvas);
}


function update() {
    Angle = Angle + 0.4;
    if (Angle >= 360.0) {
        Angle = Angle - 360.0;
    }

    t += 0.1;

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


// Define a function named degrees_to_radians that converts degrees to radians.
function degrees_to_radians(degrees) {
    // Store the value of pi.
    var pi = Math.PI;
    // Multiply degrees by pi divided by 180 to convert to radians.
    return degrees * (pi / 180);
}

