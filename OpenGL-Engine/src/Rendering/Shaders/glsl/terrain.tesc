#version 460 core

layout (vertices = 4) out;

in vec2 v_TexCoords[];
out vec2 uvsCoord[];

uniform int u_TessLevelInner;
uniform int u_TessLevelOuter;

void main()
{
   gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
   uvsCoord[gl_InvocationID] = v_TexCoords[gl_InvocationID];

   gl_TessLevelOuter[0] = u_TessLevelOuter;
   gl_TessLevelOuter[1] = u_TessLevelOuter;
   gl_TessLevelOuter[2] = u_TessLevelOuter;
   gl_TessLevelOuter[3] = u_TessLevelOuter;

   gl_TessLevelInner[0] = u_TessLevelInner;
   gl_TessLevelInner[1] = u_TessLevelInner;
}