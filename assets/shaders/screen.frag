#version 420 core

/* Fragment Shader Outputs */
layout(location = 0) out vec4 o_Color;

/* Fragment Shader Inputs */
in vec2 v_TexCoord;

/* Uniforms */
uniform sampler2D u_Texture;

void main() {
    o_Color = texture(u_Texture, v_TexCoord);

}