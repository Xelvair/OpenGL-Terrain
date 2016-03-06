#version 330 core

in vec3 position;
in vec3 barycentric;
in vec3 normal;
in vec4 screen_position;
out vec3 color;

uniform sampler2D granite;
uniform sampler2D grass;
uniform sampler2D snow;

float calc_snow_comp(float slope, float height, float ease_begin, float ease_end){
  float snowyness = smoothstep(ease_begin, ease_end, height);
  snowyness += snowyness * (1.0 - slope) * 3;
  return clamp(snowyness, 0.0, 1.0);
}

void main(){
  bool wireframe = false;
  float slope = 1.0 - dot(vec3(0.0, 1.0, 0.0), normal);
  float steep_comp = smoothstep(0.3, 0.35, slope);
  float shallow_comp = 1.0 - steep_comp;
  float snow_comp = calc_snow_comp(slope, position[1], 120.f, 160.f);
  float ground_comp = 1.0 - snow_comp;

  float dist = (screen_position.x * screen_position.x + screen_position.z * screen_position.z + screen_position.y * screen_position.y) / 750.f;
  float bg_component = clamp(dist / 700.f, 0.f, 1.f);

  float wire_comp = float(wireframe && any(lessThan(barycentric, vec3(0.0075, 0.0075, 0.0075))));


  color =     texture2D(grass, mod(position.xz / 32.0, 1.0)).xyz * shallow_comp
            + texture2D(granite, mod(position.xz / 32.0, 1.0)).xyz * steep_comp;
  color =     color * ground_comp
            + texture2D(snow, mod(position.xz / 32.0, 1.0)).xyz * snow_comp;

  color = color * (1.0 - wire_comp);

  float phong_coef = dot(normal * -1.0, normalize(vec3(-1.0, -5.0, -2.0)));
  float light_coef = 0.25 + phong_coef * 0.75 + snow_comp * 0.2;
  color = color * light_coef;
  color = color * (1 - bg_component) + bg_component * vec3(.5f, .7f, 1.f);
}
