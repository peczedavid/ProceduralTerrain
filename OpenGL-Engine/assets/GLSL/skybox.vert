#version 450 core
precision highp float;

layout (location = 0) in vec3 a_Position;

layout (std140, binding = 0) uniform CameraBufferObject
{
	mat4 View;
	mat4 Proj;
	mat4 ViewProj;
} u_Camera;

out vec3 v_TexCoords;

void main()
{
    vec4 position = u_Camera.Proj * mat4(mat3(u_Camera.View)) * vec4(a_Position, 1.0);
    // z = w so gl_Position.z gets normaized to 1, meaning it's behind everything else
    gl_Position = position.xyww;
    v_TexCoords = a_Position;
}
