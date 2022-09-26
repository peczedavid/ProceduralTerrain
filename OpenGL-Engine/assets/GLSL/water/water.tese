#version 450 core
precision highp float;

#define M_PI 3.1415926535897932384626433832795
#define M_GRAVITY 9.81
#define WAVES_COUNT 16

layout(quads, fractional_odd_spacing, ccw) in;

layout (std140, binding = 0) uniform CameraBufferObject
{
	mat4 View;
	mat4 Proj;
	mat4 ViewProj;
} u_Camera;

layout (std140, binding = 1) uniform WavesBufferObject
{
	vec4[WAVES_COUNT] Waves;
} u_Waves;

uniform mat4 u_Model;

in vec2 v_UVsCoord[];
out vec3 v_WorldPos;
out vec3 v_Normal;
out vec2 v_TexCoords;
out float v_Visibility;

uniform float u_Time;
uniform float u_FogDensity;
uniform float u_FogGradient;

vec3 GerstnerWave(vec4 wave, vec3 pos, inout vec3 tangent, inout vec3 binormal)
{
  float steepness = wave.z;
  float waveLength = wave.w;
  float k = 2 * M_PI / waveLength;
  float c = sqrt(9.81 / k);
  vec2 d = normalize(wave.xy);
  float f = k * (dot(d, pos.xz) - u_Time * c);
  float a = steepness / k;

  tangent += vec3(
    -d.x * d.x * (steepness * sin(f)),
    d.x * (steepness * cos(f)),
    -d.x * d.y * (steepness * sin(f))
  );

  binormal += vec3(
    -d.x * d.y * (steepness * sin(f)),
    d.y * (steepness* cos(f)),
    -d.y * d.y * (steepness * sin(f))
  );

  return vec3(
    d.x * (a * cos(f)),
    a * sin(f),
    d.y * (a * cos(f))
  );
}

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

  vec3 tangent = vec3(1.0, 0.0, 0.0);
  vec3 binormal = vec3(0.0, 0.0, 1.0);
  for(int i = 0; i < WAVES_COUNT; i++)
  {
    pos.xyz += GerstnerWave(u_Waves.Waves[i], pos.xyz, tangent, binormal);
  }

  gl_Position = u_Camera.ViewProj * u_Model * pos;
  v_WorldPos = gl_Position.xyz;
  v_TexCoords = texCoord * 35.0;
  v_Normal = normalize(vec3(cross(binormal, tangent)));

  float vertexDistance = length((u_Camera.View * u_Model * pos).xyz);
  v_Visibility = exp(-pow((vertexDistance * u_FogDensity), u_FogGradient));
  v_Visibility = clamp(v_Visibility, 0.0, 1.0);
}