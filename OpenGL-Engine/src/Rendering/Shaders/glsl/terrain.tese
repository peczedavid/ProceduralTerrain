#version 460 core

layout(quads, fractional_odd_spacing, ccw) in;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform float u_MaxLevel;
uniform sampler2D u_Texture;

in vec2 v_UVsCoord[];
out vec2 v_TexCoords;

void main() {
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;

  vec2 uv0 = v_UVsCoord[0];
  vec2 uv1 = v_UVsCoord[1];
  vec2 uv2 = v_UVsCoord[2];
  vec2 uv3 = v_UVsCoord[3];

  vec2 leftUV = uv0 + v * (uv3 - uv0);
  vec2 rightUV = uv1 + v * (uv2 - uv1);
  vec2 texCoord = leftUV + u * (rightUV - leftUV);

  float y = texture(u_Texture, texCoord).y;

  vec4 pos0 = gl_in[0].gl_Position;
  vec4 pos1 = gl_in[1].gl_Position;
  vec4 pos2 = gl_in[2].gl_Position;
  vec4 pos3 = gl_in[3].gl_Position;

  vec4 leftPos = pos0 + v * (pos3 - pos0);
  vec4 rightPos = pos1 + v * (pos2 - pos1);
  vec4 pos = leftPos + u * (rightPos - leftPos);
  pos.y = y * u_MaxLevel;

  gl_Position = u_ViewProj * u_Model * pos;
  v_TexCoords = texCoord;
  // float u = gl_TessCoord.x;
  // float v = gl_TessCoord.y;

  // vec2 t00 = v_UVsCoord[0];
  // vec2 t01 = v_UVsCoord[1];
  // vec2 t10 = v_UVsCoord[2];
  // vec2 t11 = v_UVsCoord[3];

  // vec2 t0 = (t01 - t00) * u + t00;
  // vec2 t1 = (t11 - t10) * u + t10;
  // vec2 texCoord = (t1 - t0) * v + t0;

  // float height = texture(u_Texture, texCoord).y;

  // vec4 p00 = gl_in[0].gl_Position;
  // vec4 p01 = gl_in[1].gl_Position;
  // vec4 p10 = gl_in[2].gl_Position;
  // vec4 p11 = gl_in[3].gl_Position;

  // // compute patch surface normal
  // vec4 uVec = p01 - p00;
  // vec4 vVec = p10 - p00;
  // vec4 normal = normalize( vec4(cross(vVec.xyz, uVec.xyz), 0) );

  // // bilinearly interpolate position coordinate across patch
  // vec4 p0 = (p01 - p00) * u + p00;
  // vec4 p1 = (p11 - p10) * u + p10;
  // vec4 p = (p1 - p0) * v + p0;

  // // displace point along normal
  // p += normal * height * u_MaxLevel;

  // // ----------------------------------------------------------------------
  // // output patch point position in clip space
  // gl_Position = u_ViewProj * u_Model * p;
  // v_TexCoords = texCoord;
}

// #version 460 core
// precision highp float;

// layout (quads, equal_spacing , ccw) in;

// in vec2 uvsCoord[];
// in vec4 maskColors[];
// out vec2 v_TexCoords;
// out vec4 maskColor;

// uniform mat4 u_Model;
// uniform mat4 u_ViewProj;
// uniform float u_MaxLevel;
// uniform sampler2D u_Texture;

// void main()
// {
//     float u = gl_TessCoord.x;
//     float v = gl_TessCoord.y;

//     vec2 uv0 = uvsCoord[0];
//     vec2 uv1 = uvsCoord[1];
//     vec2 uv2 = uvsCoord[2];
//     vec2 uv3 = uvsCoord[3];

//     vec2 leftUV = uv0 + v * (uv3 - uv0);
//     vec2 rightUV = uv1 + v * (uv2 - uv1);
//     vec2 texCoord = leftUV + u * (rightUV - leftUV);

//     float y = texture(u_Texture, texCoord).y;

//     vec4 pos0 = gl_in[0].gl_Position;
//     vec4 pos1 = gl_in[1].gl_Position;
//     vec4 pos2 = gl_in[2].gl_Position;
//     vec4 pos3 = gl_in[3].gl_Position;

//     vec4 leftPos = pos0 + v * (pos3 - pos0);
//     vec4 rightPos = pos1 + v * (pos2 - pos1);
//     vec4 pos = leftPos + u * (rightPos - leftPos);
//     pos.y = y * u_MaxLevel;

//     gl_Position = u_ViewProj * u_Model * pos;
//     v_TexCoords = texCoord;
//     maskColor = 0.25 * maskColors[0] + 0.25 * maskColors[1] + 0.25 *
//     maskColors[2] + 0.25 * maskColors[3];
// }