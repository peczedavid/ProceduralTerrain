#version 460 core

layout(vertices = 4) out;

in vec2 v_TexCoords[];
out vec2 v_UVsCoord[];

uniform mat4 u_Model;
uniform mat4 u_View;

void main() {
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
  v_UVsCoord[gl_InvocationID] = v_TexCoords[gl_InvocationID];

  if (gl_InvocationID == 0) {
    const int MIN_TESS_LEVEL = 1;
    const int MAX_TESS_LEVEL = 32;
    const float MIN_DISTANCE = 10;
    const float MAX_DISTANCE = 100;

    vec4 eyeSpacePos00 = u_View * u_Model * gl_in[0].gl_Position;
    vec4 eyeSpacePos01 = u_View * u_Model * gl_in[1].gl_Position;
    vec4 eyeSpacePos10 = u_View * u_Model * gl_in[3].gl_Position;
    vec4 eyeSpacePos11 = u_View * u_Model * gl_in[2].gl_Position;

    float distance00 = clamp((abs(eyeSpacePos00.z) - MIN_DISTANCE) /
                                 (MAX_DISTANCE - MIN_DISTANCE),
                             0.0, 1.0);
    float distance01 = clamp((abs(eyeSpacePos01.z) - MIN_DISTANCE) /
                                 (MAX_DISTANCE - MIN_DISTANCE),
                             0.0, 1.0);
    float distance10 = clamp((abs(eyeSpacePos10.z) - MIN_DISTANCE) /
                                 (MAX_DISTANCE - MIN_DISTANCE),
                             0.0, 1.0);
    float distance11 = clamp((abs(eyeSpacePos11.z) - MIN_DISTANCE) /
                                 (MAX_DISTANCE - MIN_DISTANCE),
                             0.0, 1.0);

    float tessLevel0 =
        mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance10, distance00));
    float tessLevel1 =
        mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance00, distance01));
    float tessLevel2 =
        mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance01, distance11));
    float tessLevel3 =
        mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance11, distance10));

    gl_TessLevelOuter[0] = tessLevel0;
    gl_TessLevelOuter[1] = tessLevel1;
    gl_TessLevelOuter[2] = tessLevel2;
    gl_TessLevelOuter[3] = tessLevel3;

    gl_TessLevelInner[0] = max(tessLevel1, tessLevel3);
    gl_TessLevelInner[1] = max(tessLevel0, tessLevel2);
  }
}
// #version 460 core

// layout (vertices = 4) out;

// in vec2 v_TexCoords[];
// out vec2 uvsCoord[];
// out vec4 maskColors[];

// //uniform int u_TessLevelInner;
// //uniform int u_TessLevelOuter;
// uniform sampler2D u_Texture;
// uniform float u_MaxLevel;
// uniform vec3 u_CameraPosition;
// uniform mat4 u_Model;

// int Eval(float x)
// {
//    // float a = 330.0;
//    // float b = 1.77;
//    // float c = 1.0;

//    // float a = 430.0;
//    // float b = 1.7;
//    // float c = 1.3;

//    // return int((a / pow(x, b)) + c);

//    if(x > 70) return 1;
//    if(x > 50 && x < 70) return 3;
//    if(x > 30 && x < 50) return 12;
//    if(x > 10 && x < 30) return 30;
//    if(x > 5 && x < 30) return 40;
//    if(x < 5) return 60;

// }

// int GetTessLevel(vec3 pos)
// {
//    vec3 worldPos = vec3(u_Model * vec4(pos, 1.0));
//    float dist = distance(worldPos.xz, u_CameraPosition.xz);
//    return Eval(dist);
// }

// void main()
// {
//    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
//    uvsCoord[gl_InvocationID] = v_TexCoords[gl_InvocationID];

//    int tess = GetTessLevel(gl_in[gl_InvocationID].gl_Position.xyz);
//    if(tess > 5) maskColors[gl_InvocationID] = vec4(0, 1, 0, 1);
//    else if(tess > 1 && tess < 5) maskColors[gl_InvocationID] = vec4(1, 1, 0,
//    1); else maskColors[gl_InvocationID] = vec4(1, 0, 0, 1);
//    if(gl_InvocationID == 0)
//    {
// 	   gl_TessLevelOuter[0] = tess;
// 	   gl_TessLevelOuter[1] = tess;
// 	   gl_TessLevelOuter[2] = tess;
// 	   gl_TessLevelOuter[3] = tess;
// 	   gl_TessLevelInner[0] = tess;
// 	   gl_TessLevelInner[1] = tess;
//    }

// }