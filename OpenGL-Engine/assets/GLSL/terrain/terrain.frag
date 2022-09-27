#version 450 core
precision highp float;

out vec4 outColor;

in vec4 v_Normal;
in vec2 v_TexCoords;
in float v_Visibility;

uniform sampler2D u_GroundTexture;
uniform sampler2D u_RockTexture;
uniform int u_NormalView;
uniform int u_Shade;

void main()
{
	if(u_NormalView == 0)
	{
		const vec4 groundColor = texture(u_GroundTexture, v_TexCoords);
		const vec4 rockColor = texture(u_RockTexture, v_TexCoords);
		// Rock texture based on steepness 
		const float lambda = smoothstep(0.825, 0.925, v_Normal.y);
		outColor = mix(rockColor, groundColor, lambda);

		if(u_Shade == 1)
		{
			// Diffuse lighting
			const float cost = dot(v_Normal, normalize(vec4(0.254, 0.341, 0.905, 0.0)));
			// Ambient lighting
			outColor.xyz *= clamp(cost, 0.25, 1.0);
		}
		
		// Fog
		outColor.rgb = mix(vec3(0.4, 0.5, 0.6), outColor.xyz, v_Visibility);
	}
	else
	{
		outColor.rgb = vec3(v_Normal + 0.7) / 1.7;
		
		if(u_Shade == 1)
		{
			// Diffuse lighting
			const float cost = dot(v_Normal, normalize(vec4(0.254, 0.341, 0.905, 0.0)));
			// Ambient lighting
			outColor.xyz *= clamp(cost, 0.25, 1.0);
		}
	}

	outColor.w = 1.0;
}