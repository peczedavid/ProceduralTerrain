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
	vec4 Position;
} u_Camera;

layout (std140, binding = 1) uniform WavesBufferObject
{
	vec4[WAVES_COUNT] Waves;
} u_Waves;

layout (std140, binding = 2) uniform EnviromentBuffer
{
	vec4 SunDirection;
	vec4 FogColor;
	float FogDensity;
	float FogGradient;
    float Time;
} u_Enviroment;

uniform mat4 u_Model;

in vec2 v_UVsCoord[];
out vec4 v_WorldPos;
out vec3 v_Normal;
out vec2 v_TexCoords;
out float v_Visibility;

uniform vec3 u_CameraPos;

// TODO: LINK THE WEBSITE TO WORD
vec3 GerstnerWave(vec4 wave, vec3 pos, inout vec3 tangent, inout vec3 binormal)
{
    const float steepness = wave.z;
    const float waveLength = wave.w;
    const float k = 2 * M_PI / waveLength;
    const float c = sqrt(9.81 / k);
    const vec2 d = normalize(wave.xy);
    const float f = k * (dot(d, pos.xz) - u_Enviroment.Time * c);
    const float a = steepness / k;
    
    const float sinf = sin(f);
    const float cosf = cos(f);
    
    tangent += vec3(
        -d.x * d.x * (steepness * sinf),
        d.x * (steepness * cosf),
        -d.x * d.y * (steepness * sinf)
    );
    
    binormal += vec3(
        -d.x * d.y * (steepness * sinf),
        d.y * (steepness* cosf),
        -d.y * d.y * (steepness * sinf)
    );
    
    return vec3(
        d.x * (a * cosf),
        a * sinf,
        d.y * (a * cosf)
    );
}

void main() {
    const float u = gl_TessCoord.x;
    const float v = gl_TessCoord.y;
    
    const vec2 uv0 = v_UVsCoord[0];
    const vec2 uv1 = v_UVsCoord[1];
    const vec2 uv2 = v_UVsCoord[2];
    const vec2 uv3 = v_UVsCoord[3];
    
    const vec2 leftUV = uv0 + v * (uv3 - uv0);
    const vec2 rightUV = uv1 + v * (uv2 - uv1);
    const vec2 texCoord = leftUV + u * (rightUV - leftUV);
    
    const vec4 pos0 = gl_in[0].gl_Position;
    const vec4 pos1 = gl_in[1].gl_Position;
    const vec4 pos2 = gl_in[2].gl_Position;
    const vec4 pos3 = gl_in[3].gl_Position;
    
    const vec4 leftPos = pos0 + v * (pos3 - pos0);
    const vec4 rightPos = pos1 + v * (pos2 - pos1);
    vec4 pos = leftPos + u * (rightPos - leftPos);
    
    vec3 tangent = vec3(1.0, 0.0, 0.0);
    vec3 binormal = vec3(0.0, 0.0, 1.0);
    for(int i = 0; i < WAVES_COUNT; i++)
    {
        pos.xyz += GerstnerWave(u_Waves.Waves[i], pos.xyz, tangent, binormal);
    }
    
    v_WorldPos = u_Model * pos;
    gl_Position = u_Camera.ViewProj * v_WorldPos;
    v_TexCoords = texCoord * 75.0;
    v_Normal = normalize(cross(binormal, tangent));
    
    const float vertexDistance = length((u_Camera.View * v_WorldPos).xyz);
    v_Visibility = exp(-pow((vertexDistance * u_Enviroment.FogDensity), u_Enviroment.FogGradient));
    v_Visibility = clamp(v_Visibility, 0.0, 1.0);
}