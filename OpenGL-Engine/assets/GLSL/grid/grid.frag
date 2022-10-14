#version 450 core
precision highp float;

#define NEAR 0.01
#define FAR  1000

layout (std140, binding = 0) uniform CameraBufferObject
{
	mat4 View;
	mat4 Proj;
	mat4 ViewProj;
	vec4 Position;
} u_Camera;

layout (std140, binding = 2) uniform EnviromentBuffer
{
	vec4 SunDirection;
	vec4 FogColor;
	float FogDensity;
	float FogGradient;
	float Time;
} u_Enviroment;

out vec4 outColor;

in vec3 v_NearPoint;
in vec3 v_FarPoint;

vec4 Grid(vec3 fragPos, float scale, bool drawAxis) {
    vec2 coord = fragPos.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));
    const float size = 5.0;
    // z axis
    if(fragPos.x > -size * minimumx && fragPos.x < size * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos.z > -size * minimumz && fragPos.z < size * minimumz)
        color.x = 1.0;
    return color;
}

float CalculateDepth(vec3 pos) {
    vec4 clipSpacePos = u_Camera.Proj * u_Camera.View * vec4(pos.xyz, 1.0);
    return (clipSpacePos.z / clipSpacePos.w);
}

float CalculateLinerDepth(vec3 pos) {
    vec4 clipSpacePos = u_Camera.Proj * u_Camera.View * vec4(pos.xyz, 1.0);
    float clipSpaceDepth = (clipSpacePos.z / clipSpacePos.w) * 2.0 - 1.0; // put back between -1 and 1
    float linearDepth = (2.0 * NEAR * FAR) / (FAR + NEAR - clipSpaceDepth * (FAR - NEAR)); // get linear value between 0.01 and 100
    return linearDepth / FAR; // normalize
}

void main()
{
	float t = -v_NearPoint.y / (v_FarPoint.y - v_NearPoint.y);
    vec3 fragPos = v_NearPoint + t * (v_FarPoint - v_NearPoint);

    gl_FragDepth = CalculateDepth(fragPos);

    float linearDepth = CalculateLinerDepth(fragPos);
    float fading = max(0, (0.5 - linearDepth));

    const float vertexDistance = length((u_Camera.View * vec4(fragPos, 1.0)).xyz);
	float visibility = exp(-pow((vertexDistance * u_Enviroment.FogDensity), u_Enviroment.FogGradient));
	visibility = clamp(visibility, 0.0, 1.0);

    outColor = (Grid(fragPos, 0.025, true) + Grid(fragPos, 0.25, true)) * float(t > 0); // adding multiple resolution for the grid
    outColor.a = mix(0.0, outColor.a, visibility);
}