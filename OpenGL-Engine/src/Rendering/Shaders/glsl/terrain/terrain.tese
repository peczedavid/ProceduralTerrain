#version 460 core

layout(quads, fractional_odd_spacing, ccw) in;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform mat4 u_View;
uniform float u_MaxLevel;

uniform float u_HeightOffset;
uniform float u_FogDensity;
uniform float u_FogGradient;

uniform sampler2D u_NoiseTexture;

in vec2 v_UVsCoord[];
out vec2 v_TexCoords;
out float v_Height;
out vec3 v_Normal;
out float v_Visibility;

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

  vec4 pos0 = gl_in[0].gl_Position;
  vec4 pos1 = gl_in[1].gl_Position;
  vec4 pos2 = gl_in[2].gl_Position;
  vec4 pos3 = gl_in[3].gl_Position;

  vec4 leftPos = pos0 + v * (pos3 - pos0);
  vec4 rightPos = pos1 + v * (pos2 - pos1);
  vec4 pos = leftPos + u * (rightPos - leftPos);
  vec4 worldPos = u_Model * pos;
  //vec2 texCustom = worldPos.xz;
  //vec2 uvCustom = fract(abs(texCustom)/vec2(textureSize(u_NoiseTexture, 0)));
  //vec2 uvCustom = fract(texCustom/vec2(textureSize(u_NoiseTexture, 0)));
  vec2 uvCustom = texCoord;
  if(uvCustom.x < 0) uvCustom.x = 1 - uvCustom.x;
  uvCustom.y = 1 - uvCustom.y;
  vec3 info = texture(u_NoiseTexture, uvCustom).xyz;
  v_Height = info.x;
  v_Normal = normalize(vec3(-info.y, 1.0, -info.z));
  vec4 uVec = pos2 - pos0;
  vec4 vVec = pos3 - pos0;
  vec4 normal = normalize(vec4(cross(vVec.xyz, uVec.xyz), 0));
  worldPos += normal * v_Height * u_MaxLevel;
  worldPos.y += u_HeightOffset;

  gl_Position = u_ViewProj * worldPos;
  v_TexCoords = worldPos.xz / 10.0;

  float vertexDistance = length((u_View * u_Model * pos).xyz);
  v_Visibility = exp(-pow((vertexDistance * u_FogDensity), u_FogGradient));
  v_Visibility = clamp(v_Visibility, 0.0, 1.0);
}