#version 450 core

in vec3 gxl3d_Position;

void main()
{
  gl_Position = vec4(gxl3d_Position, 1.0);
}