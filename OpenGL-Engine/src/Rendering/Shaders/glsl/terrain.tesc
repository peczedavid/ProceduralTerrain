#version 460 core

layout (vertices = 4) out;

in vec2 v_TexCoords[];
out vec2 uvsCoord[];
out vec4 maskColors[];

//uniform int u_TessLevelInner;
//uniform int u_TessLevelOuter;
uniform sampler2D u_Texture;
uniform float u_MaxLevel;
uniform vec3 u_CameraPosition;
uniform mat4 u_Model;

int Eval(float x)
{
   // float a = 330.0;
   // float b = 1.77;
   // float c = 1.0;

   // float a = 430.0;
   // float b = 1.7;
   // float c = 1.3;

   // return int((a / pow(x, b)) + c);

   if(x > 70) return 1;
   if(x > 50 && x < 70) return 3;
   if(x > 30 && x < 50) return 12;
   if(x > 10 && x < 30) return 30;
   if(x > 5 && x < 30) return 40;
   if(x < 5) return 60;

}

int GetTessLevel(vec3 _pos, float y)
{
   vec3 pos = _pos;
   pos.y = y;
   vec3 worldPos = vec3(u_Model * vec4(pos, 1.0));
   float dist = distance(worldPos, u_CameraPosition);
   return Eval(dist);
}

void main()
{
   gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
   uvsCoord[gl_InvocationID] = v_TexCoords[gl_InvocationID];

   float y = texture(u_Texture, v_TexCoords[gl_InvocationID]).y;
   int tess = GetTessLevel(gl_in[gl_InvocationID].gl_Position.xyz, y);
   if(tess > 5) maskColors[gl_InvocationID] = vec4(0, 1, 0, 1);
   else if(tess > 1 && tess < 5) maskColors[gl_InvocationID] = vec4(1, 1, 0, 1);
   else maskColors[gl_InvocationID] = vec4(1, 0, 0, 1);

   gl_TessLevelOuter[0] = tess;//tess;
   gl_TessLevelOuter[1] = tess;//tess;
   gl_TessLevelOuter[2] = tess;//tess;
   gl_TessLevelOuter[3] = tess;//tess;
   gl_TessLevelInner[0] = tess;
   gl_TessLevelInner[1] = tess;
}