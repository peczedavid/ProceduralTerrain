#version 460 core
precision highp float;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoords;

layout (std140, binding = 0) uniform CameraBufferObject
{
	mat4 View;
	mat4 Proj;
	mat4 ViewProj;
} u_Camera;

out vec2 v_TexCoords;

uniform mat4 u_Model;

void main()
{
	gl_Position = u_Camera.ViewProj * u_Model * vec4(a_Position, 1);
	v_TexCoords = a_TexCoords;
}