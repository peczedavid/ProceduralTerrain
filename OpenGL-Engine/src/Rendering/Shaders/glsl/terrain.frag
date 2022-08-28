#version 460 core
precision highp float;

out vec4 outColor;

in float v_Height;
in vec2 v_TexCoords;
in vec4 v_Normal;

//uniform sampler2D u_Texture;
uniform sampler2D u_GroundTexture;
uniform sampler2D u_RockTexture;
uniform sampler2D u_SnowTexture;

uniform vec4 u_LightDir;

uniform float u_GrassLevel;
uniform float u_RockLevel;
uniform float u_SnowLevel;

float eval(float x, float start, float end)
{
	float l = end - start;
	float slope = -1.0 / l;
	float c = end / l;
	float value =  slope * x + c;
	return clamp(value, 0.0, 1.0);
}

void main()
{
	//float height = texture(u_Texture, v_TexCoords).y;
	vec4 groundColor = texture(u_GroundTexture, v_TexCoords);
	vec4 rockColor = texture(u_RockTexture, v_TexCoords);
	vec4 snowColor = texture(u_SnowTexture, v_TexCoords);

	// float weightGround = eval(height, 0.0, 0.55);
	// float weightRock = eval(height, 0.55, 0.90);
	// float weightSnow = 1.0 - weightRock - weightGround;

	float weightGround, weightRock, weightSnow;

	if(0 < v_Height && v_Height <= u_GrassLevel)
	{
		weightGround = 1.0;
		weightRock = 0.0;
		weightSnow = 0.0;
	}
	else if(u_GrassLevel < v_Height && v_Height <= u_RockLevel )
	{
		weightGround = eval(v_Height, u_GrassLevel, u_RockLevel);
		weightRock = 1.0 - weightGround;
		weightSnow = 0.0;
	}
	else if(u_RockLevel < v_Height && v_Height <= u_SnowLevel)
	{
		weightGround = 0.0;
		weightRock = eval(v_Height, u_RockLevel, u_SnowLevel);
		weightSnow = 1.0 - weightRock;
	}
	else
	{
		weightGround = 0.0;
		weightRock = 0.0;
		weightSnow = 1.0;
	}

	vec3 texColor = (groundColor * weightGround + 
			   rockColor * weightRock + 
			   snowColor * weightSnow).xyz;

	//float cost = max(dot(normalize(v_Normal), normalize(u_LightDir)), 0.0);
	//texColor *= cost;
	//outColor.xyz = v_Normal.xyz;//texColor;
	outColor.xyz = texColor;
	outColor.w = 1.0;

	//outColor.xyz *= dot(vec4(0.2, 0.8, 0.0, 0.0), vec4(-0.5, -0.5, 0, 1));
}