var canvas = null;
var gl = null;
var bFullscreen = false;
var canvas_original_width = 0;
var canvas_original_height = 0;

// webGL related veriables
const MyAttributs =
{ 
    AMC_ATTRIBUTE_POSITION:0,
    AMC_ATTRIBUTE_NORMAL: 1,
};

var shaderProgramObject = null;  //object to give memory

var vao = null;
var vbo = null;  //uninitialized
var vbo_normal_pyramid = null;

//var mvpUniform; //undefined (delyed declerative)
var modelViewMatrixUniform;
var projectionMatrixUniform;

var  perspectiveProjectionMatrix; //undefined

// Light Related veriable
var ldUniform; // Light Diffuse
var kdUniform; // color of material
var lightPositionUniform ;
var LKeyPressedUniform;

var lightDiffuse =  [1.0, 1.0, 1.0];
var materialDiffuse =  [0.50, 0.50, 0.5];
var lightPosition = [0.0, 0.0, 2.0, 1.0];

var bAnimation = false;
var bLight = false;

//Roation
var Angle = 0.0;

var requestAnimationFrame = window.requestAnimationFrame     ||      // crome
                            window.mozRequestAnimationFrame  ||      // mozila
                            window.oRequestAnimationFrame    ||      // opera
                            window.webkitAnimationFrame      ||      // safari
                            window.msRequestAnimationFrame;          // microsoft Edge

function main() 
{
    //  get canvas
     canvas = document.getElementById("msp");
    if (canvas == null) 
    {
        console.log("canvas element can not be obtained.\n");
    }
    else
    {
        console.log("canvas element successfully obtained.\n");
    }

    canvas_original_width = canvas.width;
    canvas_original_height = canvas.height;

    // register our callback func
    window.addEventListener("keydown",keyDown,false);
    window.addEventListener("click",mouseDown,false);
    window.addEventListener("resize",resize,false);

    //initialize
    initialize();
    resize();
    display();

}

function keyDown(event)
{
    //  code
    //alert("key is press");
    switch(event.keyCode)
    {
        case 70:  // F
        case 102: // f
            if(bFullscreen == false)
            {
                toggleFullscreen();
                bFullscreen = true;
            }
            else
            {
                toggleFullscreen();
                bFullscreen = false;
            }
        break;
        case 65: // 'A':
		case 97: // 'a':
			if (bAnimation == false)
			{
				bAnimation = true;
			}
			else
			{
				bAnimation = false;
			}
			break;
		case 76:  //'L':
		case 108: //'l':
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

function mouseDown()
{
    //  code
   // alert("mouse is clicked");

}

function toggleFullscreen()
{
    // code
    /* 3 steps
        1) fullscreen navacha element ahe ka 
        2) null asel tr fullsceern
        3) null nasel tr no fullscreen
    */

    var fullscreen_element = 
                    document.fullscreenelement       || 
                    document.mozFullScreenElement    ||
                    document.webkitFullscreenElement ||
                    document.msFullscreenElement     ||
                    null;

    if(fullscreen_element == null)
    {
        if(canvas.requestFullscreen)
        { 
            canvas.requestFullscreen();  //for google croom
        }
        else if(canvas.mozRequestFullScreen)
        {
            canvas.mozRequestFullScreen();

        }
        else if(canvas.webkitRequestFullscreen)
        {
            canvas.webkitRequestFullscreen();
        }
        else if(canvas.msRequestFullscreen)
        {
            canvas.msRequestFullscreen();
        }
      
    }
    else
    {
        if(document.exitFullscreen)
        {
            document.exitFullscreen();
        }
        else if(document.mozCancelFullScreen)
        {
            document.mozCancelFullScreen();

        }
        else if(document.webkitExitFullscreen)
        {
            document.webkitExitFullscreen();
        }
        else if(document.msExitFullscreen)
        {
            document.msExitFullscreen();
        }
    }

}

function initialize()
{
    // get 2D context from canvas
    gl = canvas.getContext("webgl2");
    if (gl == null) 
    {
        console.log(" webgl2 context can not be obtained.\n");
    }
    else
    {
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
            "in vec4 aPosition;\n"+
            "in vec3 aNormal;\n" +
            "uniform mat4 uModelViewMatrix;\n" +
            "uniform mat4 uProjectionMatrix;\n" +
            "uniform vec3 uLd;\n" +
            "uniform vec3 uKd;\n" +
            "uniform vec4 uLightPosition;\n" +
            "uniform int uLkeyPressed;\n" +
            "out vec3 out_diffueLight;\n" +
            "void main(void)\n" +
            "{gl_Position=uProjectionMatrix * uModelViewMatrix * aPosition;\n" +
            "if\n" +
            "(\n" +
            "uLkeyPressed == 1\n" +
            ")\n" +
            "{\n" +
            "vec4 iCoordinate = uModelViewMatrix * aPosition;\n" +
            "mat3 normalMatrix = mat3(transpose(inverse(uModelViewMatrix)));\n" +
            "vec3 transformedNormal = normalize(normalMatrix * aNormal);\n" +
            "vec3 lightSource = normalize(vec3(uLightPosition - iCoordinate));\n" +
            "out_diffueLight = uLd * uKd * max(dot(lightSource,transformedNormal),0.0);\n" +
            "}\n" +
            "else\n" +
            "{\n" +
            "out_diffueLight = vec3(1.0,1.0,1.0);\n" +
            "}\n" +
            "}\n" 
      );

      gl.shaderSource(vertexShaderObject, vertexShaderSourceCode);
      gl.compileShader(vertexShaderObject);

      if(gl.getShaderParameter(vertexShaderObject,gl.COMPILE_STATUS)== false)
      {
        var error = gl.getShaderInfoLog(vertexShaderObject);
        if(error.length > 0 )
        {
            alert(error);
            uninitialize();
        }
      }
      else
      {
        console.log("vertex shader compilation successfull\n")
      }

      // fragmant shader
      var fragmentShaderObject = gl.createShader(gl.FRAGMENT_SHADER);
      var fragmentShaderSourceCode =
      (
        "#version 300 es\n" +
        "precision highp float;" +
        "in vec3 out_diffueLight;\n" +
        "out vec4 FragColor;\n" +
        "void main(void)\n" +
        "{FragColor=vec4(out_diffueLight,1.0);\n" +
        "}\n"
      );
    
      gl.shaderSource(fragmentShaderObject, fragmentShaderSourceCode);
      gl.compileShader(fragmentShaderObject);
      if(gl.getShaderParameter(fragmentShaderObject,gl.COMPILE_STATUS) == false)
      {
        var error = gl.getShaderInfoLog(fragmentShaderObject);
        if(error.length > 0 )
        {
            alert(error);
            uninitialize();
        }
        
      }
      else
      {
         console.log(" Fragment shader compilation successfull\n")
      }

      shaderProgramObject = gl.createProgram();
      gl.attachShader(shaderProgramObject,vertexShaderObject);
      gl.attachShader(shaderProgramObject,fragmentShaderObject);

      // prelink attributs
      gl.bindAttribLocation(shaderProgramObject,MyAttributs.AMC_ATTRIBUTE_POSITION,"aPosition");
      gl.bindAttribLocation(shaderProgramObject,MyAttributs.AMC_ATTRIBUTE_NORMAL,"aNormal");

        //link
      gl.linkProgram(shaderProgramObject);
      if(gl.getProgramParameter(shaderProgramObject,gl.LINK_STATUS) == false)
      {
        var error = gl.getProgramInfoLog(shaderProgramObject);
        if(error.length > 0 )
        {
            alert(error);
            uninitialize();
        }
      }
      else
      {
        console.log(" Shader program link successfully");
      }

      // get uniform
    modelViewMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uModelViewMatrix");
	projectionMatrixUniform = gl.getUniformLocation(shaderProgramObject, "uProjectionMatrix");
	ldUniform = gl.getUniformLocation(shaderProgramObject, "uLd");
	kdUniform = gl.getUniformLocation(shaderProgramObject, "uKd");
	lightPositionUniform = gl.getUniformLocation(shaderProgramObject, "uLightPosition");
	LKeyPressedUniform = gl.getUniformLocation(shaderProgramObject, "uLkeyPressed");

      // provide vertex position,color,texcode, and normal etc.
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
        
        var pyramid_Normals = new Float32Array([
            // front
			0.000000, 0.447214, 0.894427, // front-top
			0.000000, 0.447214, 0.894427, // front-left
			0.000000, 0.447214, 0.894427, // front-right

			// right
			0.894427, 0.447214, 0.000000, // right-top
			0.894427, 0.447214, 0.000000, // right-left
			0.894427, 0.447214, 0.000000, // right-right

			// back
			0.000000, 0.447214, -0.894427, // back-top
			0.000000, 0.447214, -0.894427, // back-left
			0.000000, 0.447214, -0.894427, // back-right

			// left
			-0.894427, 0.447214, 0.000000, // left-top
			-0.894427, 0.447214, 0.000000, // left-left
			-0.894427, 0.447214, 0.000000, // left-right
            ]);

       //create varetx array
       vao = gl.createVertexArray();
       gl.bindVertexArray(vao);

        vbo = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER,vbo);

        gl.bufferData(gl.ARRAY_BUFFER,pyramid_position,gl.STATIC_DRAW);
        gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_POSITION,3,gl.FLOAT,false,0,0);
        gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_POSITION);

    vbo_normal_pyramid = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER,vbo_normal_pyramid);
    gl.bufferData(gl.ARRAY_BUFFER,pyramid_Normals,gl.STATIC_DRAW);
    gl.vertexAttribPointer(MyAttributs.AMC_ATTRIBUTE_NORMAL,3,gl.FLOAT,false,0,0);
    gl.enableVertexAttribArray(MyAttributs.AMC_ATTRIBUTE_NORMAL);


        gl.bindBuffer(gl.ARRAY_BUFFER,null);
        gl.bindVertexArray(null);

        // Depth initialization
        gl.clearDepth(1.0);
        gl.enable(gl.DEPTH_TEST);
        gl.depthFunc(gl.LEQUAL);

    // clear color
    gl.clearColor(0.2,0.3,0.2,1.0);

    perspectiveProjectionMatrix = mat4.create();
    
}

function resize()
{
    // code
    if(bFullscreen == true)
    {
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
    }
    else
    {
        canvas.width = canvas_original_width;
        canvas.height = canvas_original_height;
    }

   gl.viewport(0.0,0.0,canvas.width,canvas.height);

   // set perspective projection
   mat4.perspective(perspectiveProjectionMatrix,45.0,parseFloat(canvas.width)/parseFloat(canvas.height),0.1,100.0)
   
   
}

function display()
{
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    gl.useProgram(shaderProgramObject);

    let Radian = Angle * Math.PI / 180.0;

    var modelViewMatrix = mat4.create();
    var translationMatrix =  mat4.create();
    var RotationMatrix_X = mat4.create();
    var RotationMatrix_Y = mat4.create();
    var RotationMatrix_Z = mat4.create();
    var RotationMatrix = mat4.create();
    mat4.translate(translationMatrix,translationMatrix,[0.0,0.0,-4.0]);
   mat4.rotateY(RotationMatrix_Y, RotationMatrix_Y, Radian);
    mat4.rotateZ(RotationMatrix_Z, RotationMatrix_Z, Radian);
    mat4.rotateX(RotationMatrix_X, RotationMatrix_X, Radian);
   mat4.multiply( RotationMatrix, RotationMatrix_X,RotationMatrix_Z);
   
    mat4.multiply(modelViewMatrix, translationMatrix, RotationMatrix);
    
    //send this matrix to the vertex shader to the uniform
    gl.uniformMatrix4fv(modelViewMatrixUniform,  false, modelViewMatrix);
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

    //bind with vao
    gl.bindVertexArray(vao);

    //draw vertex array
    gl.drawArrays(gl.TRIANGLES,0,12);

    gl.bindVertexArray(null);

    //unUse shader program
    gl.useProgram(null);


    //update
    if(bAnimation == true)
    update();

    // double buffering
    requestAnimationFrame(display,canvas);
}


function update()
{
    Angle = Angle + 0.4;
    if(Angle >= 360.0)
    {
        Angle = Angle - 360.0;
    }
}

function uninitialize()
{
    //code
    if(bFullscreen == true)
    {
        toggleFullscreen();
    }

    if(vbo_normal_pyramid)
    {
        gl.deleteBuffer(vbo_normal_pyramid);
        vbo_normal_pyramid = null;
    }

    if(vbo)
    {
        gl.deleteBuffer(vbo);
        vbo = null;
    }

    if(vao)
    {
        gl.deleteVertexArray(vao);
        vao = null;
    }

    //detach delete shader program object
    if(shaderProgramObject)
    {
        gl.useProgram(shaderProgramObject);
        var shaderObject = gl.getAttachedShaders(shaderProgramObject);

        for(let i = 0; i < shaderObject.length; i++)
        {
            gl.detachShader(shaderProgramObject,shaderObject[i]);
            gl.deleteShader(shaderObject[i]);
            shaderObject[i] = null;
        }
        gl.useProgram(null);
        gl.deleteProgram(shaderProgramObject);
        shaderProgramObject = null;
    }
}

