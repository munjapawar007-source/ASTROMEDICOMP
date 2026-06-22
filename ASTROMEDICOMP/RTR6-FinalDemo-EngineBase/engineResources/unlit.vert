#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out vec3 out_Color;
out vec2 out_TexCoord;

void main(void)
{
    gl_Position = uProjection * uView * uModel * vec4(aPosition, 1.0);
    out_Color = aColor;
    out_TexCoord = aTexCoord;
}
