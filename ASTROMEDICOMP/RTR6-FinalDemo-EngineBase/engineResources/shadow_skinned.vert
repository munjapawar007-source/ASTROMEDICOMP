#version 460 core

layout(location = 0) in vec3  aPosition;
layout(location = 4) in uvec4 aJoints;
layout(location = 5) in vec4  aWeights;

uniform mat4 uLightSpaceMatrix;

layout(std140, binding = 0) uniform BoneBlock {
    mat4 uBoneMatrices[256];
};

void main(void)
{
    mat4 skinMatrix = aWeights.x * uBoneMatrices[aJoints.x]
                    + aWeights.y * uBoneMatrices[aJoints.y]
                    + aWeights.z * uBoneMatrices[aJoints.z]
                    + aWeights.w * uBoneMatrices[aJoints.w];

    gl_Position = uLightSpaceMatrix * skinMatrix * vec4(aPosition, 1.0);
}
