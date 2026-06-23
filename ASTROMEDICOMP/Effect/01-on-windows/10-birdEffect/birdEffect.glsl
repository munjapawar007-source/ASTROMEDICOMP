let birdShaderVertexSourceCode = 
    `#version 460 core

    in vec4 aPosition;
    in vec4 aColor;
    in vec2 aTexCoord;
    in vec3 aNormal;

    out vec4 outColor;
    out vec2 outTexCoord;
    out vec3 outNormal;

    uniform mat4 uModelMatrix;
    uniform mat4 uViewMatrix;
    uniform mat4 uProjectionMatrix;
    uniform float uTime;

    void main(void)
    {
        outColor = aColor;
        outTexCoord = aTexCoord;
        outNormal = aNormal;
        vec4 finalPosition = aPosition;
        finalPosition.y = abs(finalPosition.x) * sin(abs(finalPosition.x) - 5.0 * uTime) ;
        gl_Position = uProjectionMatrix * uViewMatrix * uModelMatrix * finalPosition;
    }`;


let birdShaderFragmentSourceCode = 
    `#version 460 core
    precision highp float;

    in vec4 outColor;
    in vec2 outTexCoord;
    in vec3 outNormal;
    out vec4 FragColor;

    uniform sampler2D uAlbedo;
    
    uniform float uTime;

    void main(void)
    {
        vec4 finalColor = vec4(1.0, 0.0, 0.0, 1.0);
        vec4 albedoColor = texture(uAlbedo, outTexCoord);

        FragColor = albedoColor;
    }`;