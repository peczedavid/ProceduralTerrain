#version 460 core

layout(quads, fractional_odd_spacing, ccw) in;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform float u_MaxLevel;

uniform float u_Lacunarity;
uniform float u_Gain;
uniform float u_Frequency;
uniform float u_Amplitude;
uniform float u_Scale;

in vec2 v_UVsCoord[];
out vec2 v_TexCoords;
out float v_Height;

float random(in vec2 st);
float noise(in vec2 st);
float fbm(in vec2 st);

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
  vec2 texCustom = pos.xz / 10.0;
  vec2 st = texCoord;
  vec2 tex = vec2(st * u_Scale);
  v_Height = fbm(texCustom * u_Scale);
  pos.y = v_Height * u_MaxLevel;

  gl_Position = u_ViewProj * u_Model * pos;
  v_TexCoords = texCustom;
}

float random(in vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

// 2D Noise based on Morgan McGuire @morgan3d
// https://www.shadertoy.com/view/4dS3Wd
float noise(in vec2 st) {
  vec2 i = floor(st);
  vec2 f = fract(st);

  // Four corners in 2D of a tile
  float a = random(i);
  float b = random(i + vec2(1.0, 0.0));
  float c = random(i + vec2(0.0, 1.0));
  float d = random(i + vec2(1.0, 1.0));

  // Smooth Interpolation

  // Cubic Hermine Curve.  Same as SmoothStep()
  vec2 u = f * f * (3.0 - 2.0 * f);
  // u = smoothstep(0.,1.,f);

  // Mix 4 coorners percentages
  return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float fbm(in vec2 st) {
  // Properties
  const int octaves = 8;
  float lacunarity = u_Lacunarity;
  float gain = u_Gain;

  // Initial values
  float amplitude = u_Amplitude;
  float frequency = u_Frequency;
  float y = 0.0;

  // Loop of octaves
  vec2 offset = vec2(0.0, 0.0);
  for (int i = 0; i < octaves; i++) {
    y += amplitude * noise(frequency * (st + offset));
    frequency *= lacunarity;
    amplitude *= gain;
  }
  return y;
}