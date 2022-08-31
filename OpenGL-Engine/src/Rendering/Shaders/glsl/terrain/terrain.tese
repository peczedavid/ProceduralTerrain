#version 460 core

layout(quads, fractional_odd_spacing, ccw) in;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;
uniform mat4 u_View;
uniform float u_MaxLevel;

uniform vec2 u_NoiseOffset;
uniform float u_Lacunarity;
uniform float u_Gain;
uniform float u_Frequency;
uniform float u_Amplitude;
uniform float u_Scale;
uniform float u_HeightOffset;
uniform float u_FogDensity;
uniform float u_FogGradient;

in vec2 v_UVsCoord[];
out vec2 v_TexCoords;
out float v_Height;
out vec3 v_Normal;
out float v_Visibility;

float random(in vec2 st);
float noise(in vec2 st);
float fbm(in vec2 st);
vec3 fbmd_9(in vec2 x);

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
  vec2 texCustom = (u_Model * pos).xz / 10.0;
  vec2 st = texCoord;
  vec2 tex = vec2(st * u_Scale);
  vec3 info = fbmd_9(texCustom * u_Scale);
  v_Height = info.x;
  v_Normal = normalize(vec3(-info.y, 1.0, -info.z));
  vec4 uVec = pos2 - pos0;
  vec4 vVec = pos3 - pos0;
  vec4 normal = normalize(vec4(cross(vVec.xyz, uVec.xyz), 0));
  pos += normal * v_Height * u_MaxLevel;
  pos.y += u_HeightOffset;

  gl_Position = u_ViewProj * u_Model * pos;
  v_TexCoords = texCustom;

  float vertexDistance = length((u_View * u_Model * pos).xyz);
  v_Visibility = exp(-pow((vertexDistance * u_FogDensity), u_FogGradient));
  v_Visibility = clamp(v_Visibility, 0.0, 1.0);
}

float random(in vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float hash1(vec2 p) {
  p = 50.0 * fract(p * 0.3183099);
  return fract(p.x * p.y * (p.x + p.y));
}

float noise(in vec2 x) {
  vec2 p = floor(x);
  vec2 w = fract(x);
#if 1
  vec2 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
#else
  vec2 u = w * w * (3.0 - 2.0 * w);
#endif

  float a = hash1(p + vec2(0, 0));
  float b = hash1(p + vec2(1, 0));
  float c = hash1(p + vec2(0, 1));
  float d = hash1(p + vec2(1, 1));

  return -1.0 + 2.0 * (a + (b - a) * u.x + (c - a) * u.y +
                       (a - b - c + d) * u.x * u.y);
}

// // 2D Noise based on Morgan McGuire @morgan3d
// // https://www.shadertoy.com/view/4dS3Wd
// float noise(in vec2 st) {
//   vec2 i = floor(st);
//   vec2 f = fract(st);

//   // Four corners in 2D of a tile
//   float a = random(i);
//   float b = random(i + vec2(1.0, 0.0));
//   float c = random(i + vec2(0.0, 1.0));
//   float d = random(i + vec2(1.0, 1.0));

//   // Smooth Interpolation

//   // Cubic Hermine Curve.  Same as SmoothStep()
//   vec2 u = f * f * (3.0 - 2.0 * f);
//   // u = smoothstep(0.,1.,f);

//   // Mix 4 coorners percentages
//   return mix(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
// }

const mat3 m3 = mat3(0.00, 0.80, 0.60, -0.80, 0.36, -0.48, -0.60, -0.48, 0.64);
const mat3 m3i = mat3(0.00, -0.80, -0.60, 0.80, 0.36, -0.48, 0.60, -0.48, 0.64);
const mat2 m2 = mat2(0.80, 0.60, -0.60, 0.80);
const mat2 m2i = mat2(0.80, -0.60, 0.60, 0.80);

vec3 noised(in vec2 x) {
  vec2 p = floor(x);
  vec2 w = fract(x);
#if 1
  vec2 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
  vec2 du = 30.0 * w * w * (w * (w - 2.0) + 1.0);
#else
  vec2 u = w * w * (3.0 - 2.0 * w);
  vec2 du = 6.0 * w * (1.0 - w);
#endif

  float a = hash1(p + vec2(0, 0));
  float b = hash1(p + vec2(1, 0));
  float c = hash1(p + vec2(0, 1));
  float d = hash1(p + vec2(1, 1));

  float k0 = a;
  float k1 = b - a;
  float k2 = c - a;
  float k4 = a - b - c + d;

  return vec3(-1.0 + 2.0 * (k0 + k1 * u.x + k2 * u.y + k4 * u.x * u.y),
              2.0 * du * vec2(k1 + k4 * u.y, k2 + k4 * u.x));
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

vec3 fbmd_9(in vec2 x) {
  float f = u_Frequency;
  float s = u_Gain;
  float a = 0.0;
  float b = u_Amplitude;
  vec2 d = vec2(0.0);
  mat2 m = mat2(1.0, 0.0, 0.0, 1.0);
  for (int i = 0; i < 16; i++) {
    vec3 n = noised(x + u_NoiseOffset);
    a += b * n.x;      // accumulate values
    d += b * m * n.yz; // accumulate derivatives
    b *= s;
    x = f * m2 * x;
    m = f * m2i * m;
  }

  return vec3(a, d);
}