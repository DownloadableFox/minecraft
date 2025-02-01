#version 420 core

/* Vertex Shader Inputs */
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Color;
layout(location = 3) in vec2 a_TexCoord;

/* Vertex Shader Outputs */
uniform mat4 u_Projection;
uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_NormalMatrix;

/* Fragment Shader Inputs */
out vec3 v_Color;
out vec3 v_Normal;
out vec3 v_Position;
out vec2 v_TexCoord;

void main() {
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
    v_Color = a_Color;
    v_Normal = mat3(u_NormalMatrix) * a_Normal;
    v_Position = vec3(u_Model * vec4(a_Position, 1.0));
    v_TexCoord = a_TexCoord;
}