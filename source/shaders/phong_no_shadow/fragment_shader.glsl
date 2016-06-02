#version 150

struct Light
{
  vec3 position;  // Center coordinates.
  vec3 normal;    // Direction vector (currently not being used).

  float La;   // in [0, 1].
  float Ld;   // in [0, 1].
  float Ls;   // in [0, 1].
};

struct Material
{
  vec3 Ka;
  vec3 Kd;
  vec3 Ks;
};

in vec4 v_color;
in vec3 v_normal;
in vec2 v_tex_coord;
in vec4 f_pos;

out vec4 c;

// Uniforms.
uniform int tex_on;         // Tells if texture is active.
uniform int light_on;       // Tells if lighting is active.
uniform int material_on;    // Tells if current frag has a material.
uniform int invalid_tex;    // Tells if texture is invalid - special rendering.

uniform int numLights;
uniform Light light[8];

uniform sampler2D tex;
uniform Material material;

void main()
{
  if (light_on == 1)
  {
    vec3 Kd, Ka, Ks;

    if (invalid_tex == 0)  // Special case - render magent instead of invalid texture.
    {
      Kd = vec3(1.0, 0.0, 1.0);
      Ka = vec3(0.1, 0.1, 0.1);
      Ks = vec3(0.0, 0.0, 0.0);
    }
    else
    {
      Kd = material_on*material.Kd + (1-material_on)*vec3(1, 1, 1);
      Ka = material_on*material.Ka;
      Ks = material_on*material.Ks;
    }

    // Note: since f_pos is in camera coordinates, the incident ray is simply f_pos.xyz.
    vec3 n = normalize(v_normal);
    vec3 r = reflect(f_pos.xyz, n);  // Compute reflection for camera ray.
    
    vec3 Id = vec3(0);
    vec3 Ia = vec3(0);
    vec3 Is = vec3(0);

    for (int i = 0; i < numLights; i++) 
    { 
      vec3 l = normalize(light[i].position - f_pos.xyz);  // Vector from frag to light.
      float q = length(light[i].position - f_pos.xyz);

      // Basic lighting - Phong model.
      Ia += Ka * (light[i].La);
      Id += Kd * (light[i].Ld * max(dot(l, n), 0.0));
      Is += Ks * (light[i].Ls * pow(max(dot(l, r), 0.0), /* alpha = */ 1.0f));
    }

    vec4 color = (1-tex_on)*vec4(1) + (tex_on)*texture(tex, v_tex_coord);
    c = color * vec4((Ia + Id + Is), 1.0);
  }
  else
  {
    c = (1-tex_on)*v_color + (tex_on)*texture(tex, v_tex_coord);
  }
}