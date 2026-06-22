#version 460 core
in  vec3 vColor;
layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 EmissiveOut;

void main(void)
{
    FragColor   = vec4(vColor, 1.0);
    EmissiveOut = vec4(0.0);
}
