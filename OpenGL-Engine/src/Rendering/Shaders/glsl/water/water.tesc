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
    const float MIN_DISTANCE = 200;
    const float MAX_DISTANCE = 1000;

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