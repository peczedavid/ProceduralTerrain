#version 330
precision highp float;

layout(location = 0) in vec2 a_Position;

out vec2 v_Position;

void main()
{
	gl_Position = vec4(a_Position, 0, 1);
	v_Position = a_Position;
}