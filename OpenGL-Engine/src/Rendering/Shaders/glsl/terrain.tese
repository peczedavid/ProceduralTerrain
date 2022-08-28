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
out vec4 v_Normal;
out vec2 v_TexCoords;
out float v_Height;

vec4 terrainMapD(in vec2 p);
vec2 terrainMap(in vec2 p);
vec3 terrainNormal(in vec2 pos);
vec2 smoothstepd( float a, float b, float x);
float fbm_9(in vec2 p);

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
  vec4 heightInfo = terrainMapD(texCustom);
  v_Height = heightInfo.x;
  pos.y = v_Height * u_MaxLevel;

  gl_Position = u_ViewProj * u_Model * pos;
  v_TexCoords = texCustom;
  v_Normal = vec4(heightInfo.yzw, 0.0);
}

float random(in vec2 st) {
  return fract(sin(dot(st.xy, vec2(12.9898, 78.233))) * 43758.5453123);
}

float hash1(vec2 p) {
  p = 50.0 * fract(p * 0.3183099);
  return fract(p.x * p.y * (p.x + p.y));
}

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

const mat3 m3 = mat3(0.00, 0.80, 0.60, -0.80, 0.36, -0.48, -0.60, -0.48, 0.64);
const mat3 m3i = mat3(0.00, -0.80, -0.60, 0.80, 0.36, -0.48, 0.60, -0.48, 0.64);
const mat2 m2 = mat2(0.80, 0.60, -0.60, 0.80);
const mat2 m2i = mat2(0.80, -0.60, 0.60, 0.80);

float fbm_9(in vec2 x) {
  float f = 2.5;
  float s = 0.55;
  float a = 0.0;
  float b = 0.5;
  for (int i = 0; i < 9; i++) {
    float n = noise(x);
    a += b * n;
    b *= s;
    x = f * m2 * x;
  }

  return a;
}

vec3 fbmd_9(in vec2 x) {
  float f = 2.5;
  float s = 0.55;
  float a = 0.0;
  float b = 0.5;
  vec2 d = vec2(0.0);
  mat2 m = mat2(1.0, 0.0, 0.0, 1.0);
  for (int i = 0; i < 9; i++) {
    vec3 n = noised(x);
    a += b * n.x;      // accumulate values
    d += b * m * n.yz; // accumulate derivatives
    b *= s;
    x = f * m2 * x;
    m = f * m2i * m;
  }

  return vec3(a, d);
}

uniform float u_Mul;
uniform float u_Div;
uniform float u_Hei;

vec2 terrainMap(in vec2 p) {
  float e = fbm_9(p / u_Div + vec2(1.0, -2.0));
  float a =
      1.0 - smoothstep(0.12, 0.13,
                       abs(e + 0.12)); // flag high-slope areas (-0.25, 0.0)
  e = u_Mul * e + u_Mul;

  return vec2(e, a);
}

vec4 terrainMapD(in vec2 p) {
  vec3 e = fbmd_9(p / u_Div + vec2(1.0, -2.0));
  e.x = u_Mul * e.x + u_Mul;
  e.yz = u_Mul * e.yz;

  e.yz /= u_Div;
  return vec4(e.x, normalize(vec3(-e.y, 1.0, -e.z)));
}

vec3 terrainNormal(in vec2 pos) {
#if 0
  return terrainMapD(pos).yzw;
#else
  vec2 e = vec2(0.03, 0.0);
  return normalize(vec3(terrainMap(pos - e.xy).x - terrainMap(pos + e.xy).x,
                        2.0 * e.x,
                        terrainMap(pos - e.yx).x - terrainMap(pos + e.yx).x));
#endif
}

// return smoothstep and its derivative
vec2 smoothstepd( float a, float b, float x)
{
    if( x<a ) return vec2( 0.0, 0.0 );
    if( x>b ) return vec2( 1.0, 0.0 );
    float ir = 1.0/(b-a);
    x = (x-a)*ir;
    return vec2( x*x*(3.0-2.0*x), 6.0*x*(1.0-x)*ir );
}