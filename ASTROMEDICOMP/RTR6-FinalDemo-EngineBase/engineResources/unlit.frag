#version 460 core

in vec3 out_Color;
in vec2 out_TexCoord;

uniform sampler2D uAlbedo;
uniform int       uHasTexture;
uniform vec4      uBaseColor;

layout(location = 0) out vec4 FragColor;
layout(location = 1) out vec4 EmissiveOut;
layout(location = 2) out vec4 GodRayOut;

void main(void)
{
    vec4 color = uHasTexture == 1 ? texture(uAlbedo, out_TexCoord) : vec4(out_Color, 1.0);
    FragColor   = color * uBaseColor;
    EmissiveOut = vec4(0.0);
    GodRayOut   = vec4(0.0);
}
