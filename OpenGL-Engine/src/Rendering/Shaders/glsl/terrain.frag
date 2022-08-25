#version 460 core
precision highp float;

out vec4 outColor;

in vec2 v_TexCoords;
in vec4 maskColor;

uniform sampler2D u_Texture;

void main()
{
	vec4 texColor = texture(u_Texture, v_TexCoords);
	outColor = mix(texColor, maskColor, vec4(0.5, 0.5, 0.5, 0.5));
}