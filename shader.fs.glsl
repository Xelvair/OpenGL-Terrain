#version 330 core

in vec3 position;
in vec3 barycentric;
in vec3 normal;
in vec4 screen_position;
out vec3 color;

uniform sampler2D granite;

void main(){
float dist = (screen_position.x * screen_position.x + screen_position.z * screen_position.z + screen_position.y * screen_position.y) / 750.f;
 float bg_component = clamp(dist / 350.f, 0.f, 1.f);
 if(any(lessThan(barycentric, vec3(0.0075, 0.0075, 0.0075)))){
   color = vec3(0.0, 0.0, 0.0);
 } else { 
   color = texture2D(granite, mod(position.xz / 64.0, 1.0)).xyz;
 }
 float phong_coef = dot(normal * -1.0, normalize(vec3(-1.0, -5.0, -2.0)));
 float light_coef = 0.25 + phong_coef * 0.75;
 color = color * light_coef;
 color = color * (1 - bg_component) + bg_component * vec3(.5f, .7f, 1.f);
}
