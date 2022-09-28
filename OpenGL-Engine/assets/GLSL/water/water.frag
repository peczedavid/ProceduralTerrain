#version 450 core
precision highp float;

layout (std140, binding = 2) uniform EnviromentBuffer
{
	vec4 SunDirection;
	vec4 FogColor;
	float FogDensity;
	float FogGradient;
	float Time;
} u_Enviroment;

out vec4 outColor;

in vec4 v_WorldPos;
in vec3 v_Normal;
in float v_Visibility;
in vec2 v_TexCoords;

uniform sampler2D u_WaterTexture;
uniform int u_NormalView;
uniform vec3 u_CameraPos;
uniform float u_Shininess;
uniform float u_Reflectivity;

vec4 shade() {
	vec4 pixelColor;

	const vec3 view = normalize(u_CameraPos - v_WorldPos.xyz);
	const vec3 lightDirection = normalize(u_Enviroment.SunDirection.xyz);
	vec4 waterColor = texture(u_WaterTexture, v_TexCoords);

	vec3 reflectedLight = reflect(normalize(-lightDirection), v_Normal);
	const float specular = pow(max(dot(reflectedLight, view), 0.0), u_Shininess);
	const vec3 specularHighlight = vec3(specular) * u_Reflectivity;
	waterColor.rgb += specularHighlight;

	pixelColor = mix(u_Enviroment.FogColor, waterColor, v_Visibility);
	pixelColor.a = 0.575;
	
	return pixelColor;
}

void main() {
	if(u_NormalView == 1)
		outColor = vec4(vec3(v_Normal + 0.7) / 1.7, 1.0);
	else
		outColor = shade();
}