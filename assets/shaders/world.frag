#version 420 core

/* Fragment Shader Outputs */
layout(location = 0) out vec4 o_Color;

/* Fragment Shader Inputs */
in vec3 v_Color;
in vec3 v_Normal;
in vec3 v_Position;
in vec2 v_TexCoord;

/* Uniforms */
uniform sampler2D u_Texture;

void main() {
    o_Color = vec4(v_Color, 1.0) * texture(u_Texture, v_TexCoord);
}