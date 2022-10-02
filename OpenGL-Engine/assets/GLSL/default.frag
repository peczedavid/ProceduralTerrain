#version 450 core
precision highp float;

layout (std140, binding = 0) uniform CameraBufferObject
{
	mat4 View;
	mat4 Proj;
	mat4 ViewProj;
	vec4 Position;
} u_Camera;

layout (std140, binding = 2) uniform EnviromentBuffer
{
	vec3 SunDirection;
	float Time;
	float FogDensity;
	float FogGradient;
	vec3 FogColor;
} u_Enviroment;

out vec4 outColor;

in vec3 v_Normal;
in vec2 v_TexCoord;

void main()
{
	outColor.rgb = normalize(v_Normal);
	outColor.a = 1.0;
}