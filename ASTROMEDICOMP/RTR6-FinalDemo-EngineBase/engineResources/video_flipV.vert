#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform float uTime;

out vec3 out_Color;
out vec2 out_TexCoord;
out vec3 out_Normal;
out vec3 out_worldPos;
out vec3 out_vertexInCameraSpace;
out float out_worldHeight;

void main(void)
{
    vec4 worldPos = uModel * vec4(aPosition, 1.0);
    out_worldPos = worldPos.xyz;
    out_worldHeight = worldPos.y;
    out_vertexInCameraSpace = (uView * worldPos).xyz;

    mat3 normalMatrix = transpose(inverse(mat3(uModel)));
    out_Normal = normalize(normalMatrix * aNormal);

    gl_Position = uProjection * uView * worldPos;
    out_Color = aColor;
    out_TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y);
}
