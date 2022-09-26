#version 450 core
precision highp float;

out vec4 outColor;

in vec2 v_TexCoord;

uniform sampler2D u_ScreenTexture;
uniform vec3 u_Orientation;

void main() {
  vec3 color = texture(u_ScreenTexture, v_TexCoord).rgb;

  // sun glare
  float sun =
      clamp(dot(normalize(vec3(0.254, 0.341, 0.905)), normalize(u_Orientation)),
            0.0, 1.0);
  color += 0.25 * vec3(0.8, 0.4, 0.2) * pow(sun, 32.0);

  // contrast
  color = color * color * (3.5 - 2.5 * color);

  // color correction
  color = pow(color, vec3(1.0, 0.92, 1.0)); // soft green
  color *= vec3(1.02, 0.99, 0.9);           // tint red

  outColor.xyz = color;
  outColor.w = 1.0;
}