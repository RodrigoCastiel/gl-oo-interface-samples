#version 150

in vec2 in_tex_coord;
in vec3 in_position;
in vec3 in_normal;
in vec3 in_color;

out vec4 v_color;
out vec3 v_normal;
out vec2 v_tex_coord;
out vec4 f_pos;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main()
{
  // compute the transformed and projected vertex position (into gl_Position)
  gl_Position = P * (V * (M * vec4(in_position, 1.0f)));

  // compute the vertex color (into col)
  v_color = vec4(in_color, 1.0);
  v_tex_coord = in_tex_coord;

  // Fragment position computation in camera coordinates.
  f_pos = V * (M * vec4(in_position, 1.0f));
  f_pos = f_pos/f_pos.w;

  // Normal computation in camera coordinates.
  v_normal = (V * (inverse(transpose(M)) * vec4(in_normal, 0.0))).xyz;
}