#version 460 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aColor;
layout(location = 2) in vec3 aNormal;
layout(location = 3) in vec2 aTexCoord;
layout(location = 4) in uvec4 aJoints;
layout(location = 5) in vec4 aWeights;

uniform mat4 uView;
uniform mat4 uProjection;
uniform float uTime;

layout(std140, binding = 0) uniform BoneBlock {
    mat4 uBoneMatrices[256];
};

out vec3 out_Color;
out vec2 out_TexCoord;
out vec3 out_Normal;
out vec3 out_worldPos;
out vec3 out_vertexInCameraSpace;
out float out_worldHeight;

void main(void)
{
    mat4 skinMatrix = aWeights.x * uBoneMatrices[aJoints.x]
                    + aWeights.y * uBoneMatrices[aJoints.y]
                    + aWeights.z * uBoneMatrices[aJoints.z]
                    + aWeights.w * uBoneMatrices[aJoints.w];

    vec4 worldPos = skinMatrix * vec4(aPosition, 1.0);
    out_worldPos = worldPos.xyz;
    out_worldHeight = worldPos.y;
    out_vertexInCameraSpace = (uView * worldPos).xyz;

    mat3 normalMatrix = transpose(inverse(mat3(skinMatrix)));
    out_Normal = normalize(normalMatrix * aNormal);

    gl_Position = uProjection * uView * worldPos;
    out_Color = aColor;
    out_TexCoord = aTexCoord;
}
