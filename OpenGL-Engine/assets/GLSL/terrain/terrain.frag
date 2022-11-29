#version 450 core
precision highp float;

layout (std140, binding = 2) uniform EnviromentBuffer
{
	vec4 SunDirection;
	vec4 FogColor;
	float FogDensity;
	float FogGradient;
	float Time;
} u_Enviroment;

out vec4 outColor;

in vec4 v_WorldPos;
in vec4 v_Normal;
in vec2 v_TexCoords;
in float v_Visibility;

uniform sampler2D u_GroundTexture;
uniform sampler2D u_RockTexture;
uniform sampler2D u_SnowTexture;
uniform float u_MaxLevel;
uniform int u_NormalView;
uniform int u_Shade;

uniform float u_RockScale;
uniform float u_RockLevel;
uniform float u_SnowScale;
uniform float u_SnowLevel;

float QuinticSmoothStep(float edge0, float edge1, float x)
{
	float t = clamp((x - edge0) / (edge1 - edge0),0.0, 1.0);
	return t*t*t*(t*(t*6.0-15.0)+10.0);
}

void main()
{
	vec4 N = normalize(v_Normal);
	if(u_NormalView == 0)
	{
		const vec4 groundColor = texture(u_GroundTexture, v_TexCoords);
		const vec4 rockColor = texture(u_RockTexture, v_TexCoords);
		const vec4 snowColor = texture(u_SnowTexture, v_TexCoords);
		// Rock texture based on steepness 
		const float rockLambda = QuinticSmoothStep(u_RockLevel, u_RockLevel + u_RockScale, N.y);
		outColor = mix(rockColor, groundColor, rockLambda);
		// Snow texture based on height
		const float snowLambda = QuinticSmoothStep(u_SnowLevel, u_SnowLevel + u_SnowScale, v_WorldPos.y / u_MaxLevel);
		outColor = mix(outColor, snowColor, snowLambda);

		if(u_Shade == 1)
		{
			// Diffuse lighting
			const float cost = dot(N, normalize(u_Enviroment.SunDirection));
			// Ambient lighting
			outColor.xyz *= clamp(cost, 0.25, 1.0);
		}
		
		// Fog
		outColor = mix(u_Enviroment.FogColor, outColor, v_Visibility);
	}
	else
	{
		outColor.rgb = vec3(N + 0.7) / 1.7;
		
		if(u_Shade == 1)
		{
			// Diffuse lighting
			const float cost = dot(N, normalize(u_Enviroment.SunDirection));
			// Ambient lighting
			outColor.xyz *= clamp(cost, 0.25, 1.0);
		}
	}

	outColor.w = 1.0;
}