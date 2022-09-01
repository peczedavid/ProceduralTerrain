#version 460 core
precision highp float;

out vec4 outColor;

in vec3 v_Normal;
in float v_Visibility;
in vec2 v_TexCoords;

//uniform sampler2D u_WaterTexture;

void main()
{
	outColor.xyz = vec3(0.42, 0.69, 0.77);
	float cost = dot(v_Normal, normalize(vec3(1.0, 1.0, 0.0)));
	outColor.xyz *= cost;
	outColor.xyz = mix(vec3(0.4, 0.5, 0.6), outColor.xyz, v_Visibility);
	outColor.w = 1.0;
}