#version 460 core
precision highp float;

layout (triangles, equal_spacing , ccw) in;

in vec2 uvsCoord[];
out vec2 v_TexCoords;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

void main()
{
    // barycentric coordinates
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;
    // barycentric interpolation
    vec2 texCoord = u * uvsCoord[0] + v * uvsCoord[1] + w * uvsCoord[2];

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    // barycentric interpolation
    vec4 pos = u * pos0 + v * pos1 + w * pos2;
    
    gl_Position = u_ViewProj * u_Model * pos;
    v_TexCoords = texCoord;
}