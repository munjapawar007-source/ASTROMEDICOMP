#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 3) in vec2 aTexCoord;

uniform mat4  uModel;
uniform mat4  uView;
uniform mat4  uProjection;

out vec2 out_TexCoord;
out vec3 out_worldPos;
out vec3 out_vertexInCameraSpace;

void main(void)
{
    vec4 worldPos        = uModel * vec4(aPosition, 1.0);
    vec4 cameraPos       = uView * worldPos;
    gl_Position          = uProjection * cameraPos;
    out_TexCoord         = aTexCoord;
    out_worldPos         = worldPos.xyz;
    out_vertexInCameraSpace = cameraPos.xyz;
}
