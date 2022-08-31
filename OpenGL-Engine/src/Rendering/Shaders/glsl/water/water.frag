#version 460 core
precision highp float;

out vec4 outColor;

in float v_Visibility;
in vec2 v_TexCoords;

uniform sampler2D u_WaterTexture;

void main()
{
	outColor = texture(u_WaterTexture, v_TexCoords);
	outColor.xyz = mix(vec3(0.4, 0.5, 0.6), outColor.xyz, v_Visibility);
}