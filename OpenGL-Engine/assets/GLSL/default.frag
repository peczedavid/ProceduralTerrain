#version 450 core
precision highp float;

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
	const vec3 lightDirection = normalize(u_Enviroment.SunDirection);
	const float cost = dot(lightDirection, v_Normal);
	outColor.rgb = vec3(1.0, 1.0, 1.0) * cost;
	outColor.a = 1.0;
}