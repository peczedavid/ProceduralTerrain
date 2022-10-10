#version 450 core
precision highp float;

layout(location = 0) in vec3 a_Position;

layout (std140, binding = 0) uniform CameraBufferObject
{
	mat4 View;
	mat4 Proj;
	mat4 ViewProj;
	vec4 Position;
} u_Camera;

out vec3 v_NearPoint;
out vec3 v_FarPoint;

vec3 UnprojectPos(vec3 pos)
{
	vec4 unprojectedPos = inverse(u_Camera.View) * inverse(u_Camera.Proj) * vec4(pos, 1.0);
	return unprojectedPos.xyz / unprojectedPos.w;
}

void main()
{
	v_NearPoint = UnprojectPos(vec3(a_Position.x, a_Position.y, 0.0));
	v_FarPoint = UnprojectPos(vec3(a_Position.x, a_Position.y, 1.0));
	gl_Position = vec4(a_Position, 1.0);
}