#version 450 core
precision highp float;

out vec4 outColor;

in vec3 v_WorldPos;
in vec3 v_Normal;
in float v_Visibility;
in vec2 v_TexCoords;

uniform sampler2D u_WaterTexture;
uniform int u_NormalView;

vec4 shade() {
	vec4 pixelColor;

	vec3 lightDirection = normalize(vec3(0.254, 0.341, 0.905));
	vec3 waterColor = texture(u_WaterTexture, v_TexCoords).rgb;
	float cost = dot(v_Normal, lightDirection);
	waterColor *= cost;
	pixelColor.xyz = mix(vec3(0.4, 0.5, 0.6), waterColor, v_Visibility);
	pixelColor.w = 0.9;
	
	return pixelColor;
}

void main() {
	if(u_NormalView == 1)
		outColor = vec4(vec3(v_Normal + 0.7) / 1.7, 1.0);
	else
		outColor = shade();
}