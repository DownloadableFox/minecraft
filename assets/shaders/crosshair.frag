#version 420 core

/* Fragment Shader Outputs */
layout(location = 0) out vec4 o_Color;

/* Fragment Shader Inputs */
in vec2 v_TexCoord;

/* Uniforms */
uniform sampler2D u_Texture[2];

void main() {
    vec4 world = texture(u_Texture[1], v_TexCoord);
    vec4 stencil = texture(u_Texture[0], v_TexCoord);
    
    if (stencil.rgb == vec3(0.0)) {
        discard;
    }

    /* Invert the color */
    o_Color = vec4(1.0f - world.r, 1.0f - world.g, 1.0f - world.b, 1.0f);
}