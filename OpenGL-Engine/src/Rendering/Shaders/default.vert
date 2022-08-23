#version 330
precision highp float;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

out vec2 v_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

void main()
{
	gl_Position = u_ViewProj * u_Model * vec4(a_Position, 1);
	v_TexCoords = a_TexCoords;
}