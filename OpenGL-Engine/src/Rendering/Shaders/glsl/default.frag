#version 460 core
precision highp float;

out vec4 outColor;

in vec2 v_TexCoords;

uniform sampler2D u_Texture;

void main()
{
	outColor = texture(u_Texture, v_TexCoords);
}