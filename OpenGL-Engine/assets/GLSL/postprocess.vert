#version 450 core
precision highp float;

layout (location = 0) in vec2 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main() {
	gl_Position = vec4(a_Position, 0.0, 1.0);
	v_TexCoord = a_TexCoord;
}