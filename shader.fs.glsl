#version 330 core

in vec3 position;
in vec3 barycentric;
in vec3 normal;
in vec4 screen_position;
out vec3 color;

uniform sampler2D granite;
uniform sampler2D grass;

float calc_steep_comp(float slope, float ease_begin, float ease_end){
  if(slope < ease_begin){
    return 0.0;
  }
  if(slope > ease_end){
    return 1.0;
  }

  return (slope - ease_begin) / (ease_end - ease_begin);
}

void main(){
float slope = 1.0 - dot(vec3(0.0, 1.0, 0.0), normal);
float steep_comp = calc_steep_comp(slope, 0.3, 0.35);
float shallow_comp = 1.0 - steep_comp;

float dist = (screen_position.x * screen_position.x + screen_position.z * screen_position.z + screen_position.y * screen_position.y) / 750.f;
 float bg_component = clamp(dist / 350.f, 0.f, 1.f);
 if(any(lessThan(barycentric, vec3(0.0075, 0.0075, 0.0075)))){
   color = vec3(0.0, 0.0, 0.0);
 } else {
   color =    texture2D(grass, mod(position.xz / 64.0, 1.0)).xyz * shallow_comp
            + texture2D(granite, mod(position.xz / 64.0, 1.0)).xyz * steep_comp;
 }
 float phong_coef = dot(normal * -1.0, normalize(vec3(-1.0, -5.0, -2.0)));
 float light_coef = 0.25 + phong_coef * 0.75;
 color = color * light_coef;
 color = color * (1 - bg_component) + bg_component * vec3(.5f, .7f, 1.f);
}
