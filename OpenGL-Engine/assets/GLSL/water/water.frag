#version 450 core
precision highp float;

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
	const vec3 lightDirection = normalize(vec3(0.254, 0.341, 0.905));
	vec3 waterColor = texture(u_WaterTexture, v_TexCoords).rgb;

	vec3 reflectedLight = reflect(normalize(-lightDirection), v_Normal);
	const float specular = pow(max(dot(reflectedLight, view), 0.0), u_Shininess);
	const vec3 specularHighlight = vec3(specular) * u_Reflectivity;
	waterColor.rgb += specularHighlight;

	pixelColor.rgb = mix(vec3(0.4, 0.5, 0.6), waterColor, v_Visibility);
	pixelColor.a = 0.6;
	
	return pixelColor;
}

void main() {
	if(u_NormalView == 1)
		outColor = vec4(vec3(v_Normal + 0.7) / 1.7, 1.0);
	else
		outColor = shade();
}