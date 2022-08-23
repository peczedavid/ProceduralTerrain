#version 330
precision highp float;

out vec4 outColor;

in vec2 v_Position;

void main()
{
	outColor = vec4(v_Position * 0.5 + 0.5, 0, 1);
}