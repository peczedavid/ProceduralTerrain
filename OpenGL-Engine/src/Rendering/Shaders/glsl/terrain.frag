#version 460 core
precision highp float;

out vec4 outColor;

in vec2 v_TexCoords;

uniform sampler2D u_Texture;
uniform sampler2D u_GroundTexture;
uniform sampler2D u_RockTexture;
uniform sampler2D u_SnowTexture;

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
	float height = texture(u_Texture, v_TexCoords).y;
	vec4 groundColor = texture(u_GroundTexture, v_TexCoords);
	vec4 rockColor = texture(u_RockTexture, v_TexCoords);
	vec4 snowColor = texture(u_SnowTexture, v_TexCoords);

	float weightGround = eval(height, 0.0, 0.55);
	float weightRock = eval(height, 0.55, 0.90);
	float weightSnow = 1.0 - weightRock - weightGround;

	outColor = groundColor * weightGround + 
			   rockColor * weightRock + 
			   snowColor * weightSnow; 
}