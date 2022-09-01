#version 460 core
precision highp float;

out vec4 outColor;

in vec3 v_WorldPos;
in vec3 v_Normal;
in float v_Visibility;
in vec2 v_TexCoords;

uniform sampler2D u_WaterTexture;

void main()
{
	vec3 lightDirection = normalize(vec3(1.0, 1.0, 0.0));

	vec3 waterColor = texture(u_WaterTexture, v_TexCoords).rgb; //vec3(0.42, 0.69, 0.77);
	float cost = dot(v_Normal, lightDirection);
	waterColor *= cost;
	outColor.xyz = mix(vec3(0.4, 0.5, 0.6), waterColor, v_Visibility);
	outColor.w = 0.9;
}