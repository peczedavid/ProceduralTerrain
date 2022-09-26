#version 450 core
precision highp float;

out vec4 outColor;

in vec3 v_TexCoords;

uniform samplerCube u_Skybox;

void main()
{
    outColor = texture(u_Skybox, v_TexCoords);
}