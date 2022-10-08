#version 450 core
precision highp float;

#define M_PI 3.1415926535897932384626433832795

layout (std140, binding = 0) uniform CameraBufferObject
{
	mat4 View;
	mat4 Proj;
	mat4 ViewProj;
	vec4 Position;
} u_Camera;

layout (std140, binding = 2) uniform EnviromentBuffer
{
	vec3 SunDirection;
	float Time;
	float FogDensity;
	float FogGradient;
	vec3 FogColor;
} u_Enviroment;

out vec4 outColor;

in vec3 v_WorldPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

uniform vec3 u_Albedo;
uniform sampler2D u_AlbedoMap;
uniform int u_UseAlbedoMap;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AmbientOcclusion;
uniform float u_F0;

float distributionGGX(vec3 N, vec3 H, float roughness);
float geometrySchlickGGX(float NdotV, float roughness);
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnelSchlick(float cosTheta, vec3 F0);

void main()
{
	const vec3 N = normalize(v_Normal);
    const vec3 V = normalize(u_Camera.Position.xyz - v_WorldPos);
    vec3 albedo;
    if(u_UseAlbedoMap == 1)
    {
        albedo = texture(u_AlbedoMap, v_TexCoord).rgb;
        albedo.x = pow(albedo.x, 2.2);
        albedo.y = pow(albedo.y, 2.2);
        albedo.z = pow(albedo.z, 2.2);
    }
    else
        albedo = u_Albedo;

    vec3 F0 = vec3(u_F0);
	F0 = mix(F0, albedo, u_Metallic);

    const vec3 L = normalize(u_Enviroment.SunDirection);
    const vec3 H = normalize(V + L);
    vec3 radiance = vec3(1.0, 1.0, 1.0);        
    
    float NDF = distributionGGX(N, H, u_Roughness);        
    float G = geometrySmith(N, V, L, u_Roughness);      
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);       
    
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - u_Metallic;	  
    
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular     = numerator / denominator;  
        
    float NdotL = max(dot(N, L), 0.0);               
    vec3 Lo = (kD * albedo / M_PI + specular) * radiance * NdotL; 

    vec3 ambient = vec3(0.03) * albedo * u_AmbientOcclusion;
    vec3 color = ambient + Lo;
	
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(1.0/2.2));

	outColor.rgb = color;
	outColor.a = 1.0;
}

float distributionGGX(vec3 N, vec3 H, float roughness)
{
    float a      = roughness*roughness;
    float a2     = a*a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = M_PI * denom * denom;
	
    return num / denom;
}

float geometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float geometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = geometrySchlickGGX(NdotV, roughness);
    float ggx1  = geometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}