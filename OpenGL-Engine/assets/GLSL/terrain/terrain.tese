#version 450 core
precision highp float;

layout (quads, fractional_odd_spacing, ccw) in;

layout (std140, binding = 0) uniform CameraBufferObject
{
	mat4 View;
	mat4 Proj;
	mat4 ViewProj;
	vec4 Position;
} u_Camera;

layout (std140, binding = 2) uniform EnviromentBuffer
{
	vec4 SunDirection;
	vec4 FogColor;
	float FogDensity;
	float FogGradient;
	float Time;
} u_Enviroment;

uniform mat4 u_Model;
uniform float u_MaxLevel;

uniform sampler2D u_NoiseTexture;

in vec2 v_UVsCoord[];

out vec4 v_Normal;
out vec2 v_TexCoords;
out float v_Visibility;

void main()
{
	const float u = gl_TessCoord.x;
	const float v = gl_TessCoord.y;
	
	const vec2 uv0 = v_UVsCoord[0];
	const vec2 uv1 = v_UVsCoord[1];
	const vec2 uv2 = v_UVsCoord[2];
	const vec2 uv3 = v_UVsCoord[3];
	
	const vec2 leftUV = uv0 + v * (uv3 - uv0);
	const vec2 rightUV = uv1 + v * (uv2 - uv1);
	vec2 texCoord = leftUV + u * (rightUV - leftUV);
	
	const vec4 pos0 = gl_in[0].gl_Position;
	const vec4 pos1 = gl_in[1].gl_Position;
	const vec4 pos2 = gl_in[2].gl_Position;
	const vec4 pos3 = gl_in[3].gl_Position;
	
	const vec4 leftPos = pos0 + v * (pos3 - pos0);
	const vec4 rightPos = pos1 + v * (pos2 - pos1);
	vec4 pos = leftPos + u * (rightPos - leftPos);
	vec4 worldPos = u_Model * pos;
	
	if(texCoord.x < 0) texCoord.x = 1 - texCoord.x;
	texCoord.y = 1 - texCoord.y;
	const vec4 vertexInfo = texture(u_NoiseTexture, texCoord);
	worldPos.y = vertexInfo.w * u_MaxLevel;
	
	gl_Position = u_Camera.ViewProj * worldPos;
	v_Normal = vec4(vertexInfo.xyz, 0.0) * 2.0 - 1.0;
	v_TexCoords = worldPos.xz / 12.5;
	
	const float vertexDistance = length((u_Camera.View * worldPos).xyz);
	v_Visibility = exp(-pow((vertexDistance * u_Enviroment.FogDensity), u_Enviroment.FogGradient));
	v_Visibility = clamp(v_Visibility, 0.0, 1.0);
}