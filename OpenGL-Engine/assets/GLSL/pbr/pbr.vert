#version 450 core
precision highp float;

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

layout (std140, binding = 0) uniform CameraBufferObject
{
	mat4 View;
	mat4 Proj;
	mat4 ViewProj;
	vec4 Position;
} u_Camera;

out vec3 v_Normal;
out vec2 v_TexCoord;
out vec3 v_WorldPos;

uniform mat4 u_Model;

void main()
{
	vec4 worldPos = u_Model * vec4(a_Position, 1);
	gl_Position = u_Camera.ViewProj * worldPos;
	v_WorldPos = worldPos.xyz;
	v_Normal = vec3(vec4(a_Normal, 0.0) * inverse(u_Model));
	v_TexCoord = a_TexCoord;
}