#version 330
precision highp float;

layout (location = 0) in vec3 a_Position;

out vec3 v_TexCoords;

uniform mat4 u_View;
uniform mat4 u_Proj;

void main()
{
    vec4 position = u_Proj * u_View * vec4(a_Position, 1.0);
    // z = w so gl_Position.z gets normaized to 1, meaning it's behind everything else
    gl_Position = position.xyww;
    v_TexCoords = a_Position;
}
