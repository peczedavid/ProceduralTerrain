#version 460 core

layout (vertices = 3) out;

in vec2 v_TexCoords[];
out vec2 uvsCoord[];

uniform int u_TessLevelInner;
uniform int u_TessLevelOuter;

void main()
{
   gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
   uvsCoord[gl_InvocationID] = v_TexCoords[gl_InvocationID];

   gl_TessLevelOuter[0] = u_TessLevelOuter; // left for triangles
   gl_TessLevelOuter[1] = u_TessLevelOuter; // bot for triangles
   gl_TessLevelOuter[2] = u_TessLevelOuter; // right for triangles
   gl_TessLevelInner[0] = u_TessLevelInner; // all inner sides for triangles
}