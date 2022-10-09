#version 450 core
const vec4 WIRE_COL = vec4(1, 0.5, 0, 1);
in vec3 dist;
out vec4 outColor;
void main()
{
  // Wireframe rendering is better like this:
  vec3 dist_vec = dist;
 
  // Compute the shortest distance to the edge
  float d = min(dist_vec[0], min(dist_vec[1], dist_vec[2]));
 
  // Compute line intensity and then fragment color
  float I = exp2(-0.1*d*d);
 
  outColor = I*WIRE_COL;
}