#version 460 core
precision highp float;

out vec4 outColor;

in float v_Height;
in vec3 v_Normal;
in vec2 v_TexCoords;
in float v_Visibility;

uniform sampler2D u_GroundTexture;
uniform sampler2D u_RockTexture;

uniform float u_GrassLevel;
uniform float u_RockLevel;
uniform float u_SnowLevel;

uniform int u_NormalView;

float eval(float x, float start, float end)
{
	float l = end - start;
	float slope = -1.0 / l;
	float c = end / l;
	float value =  slope * x + c;
	return clamp(value, 0.0, 1.0);
}

void main()
{
	vec4 groundColor = texture(u_GroundTexture, v_TexCoords);
	vec4 rockColor = texture(u_RockTexture, v_TexCoords);
	float lambda = smoothstep(0.6, 0.7, v_Normal.y);
	outColor = mix(rockColor, groundColor, lambda);

	float cost = dot(v_Normal, normalize(vec3(0.254, 0.341, 0.905)));
	outColor.xyz *= clamp(cost, 0.22, 1.0);
	outColor.xyz = mix(vec3(0.4, 0.5, 0.6), outColor.xyz, v_Visibility);
	if(u_NormalView == 1)
		outColor.rgb = vec3(v_Normal + 0.7) / 1.7;
	outColor.w = 1.0;
}