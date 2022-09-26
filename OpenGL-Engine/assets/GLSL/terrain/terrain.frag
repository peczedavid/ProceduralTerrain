#version 460 core
precision highp float;

out vec4 outColor;

in vec4 v_Normal;
in vec2 v_TexCoords;
in float v_Visibility;

uniform sampler2D u_GroundTexture;
uniform sampler2D u_RockTexture;
uniform int u_NormalView;

void main()
{
	if(u_NormalView == 0)
	{
		vec4 groundColor = texture(u_GroundTexture, v_TexCoords);
		vec4 rockColor = texture(u_RockTexture, v_TexCoords);
		// Rock texture based on steepness 
		float lambda = smoothstep(0.825, 0.925, v_Normal.y);
		outColor = mix(rockColor, groundColor, lambda);

		// Diffuse lighting
		float cost = dot(v_Normal, normalize(vec4(0.254, 0.341, 0.905, 0.0)));
		// Ambient lighting
		outColor.xyz *= clamp(cost, 0.15, 1.0);
		// Fog
		outColor.rgb = mix(vec3(0.4, 0.5, 0.6), outColor.xyz, v_Visibility);
	}
	else
	{
		outColor.rgb = vec3(v_Normal + 0.7) / 1.7;
	}
	outColor.w = 1.0;
}