#version 330 core

uniform mat4 mv_mat;
uniform mat4 p_mat;
uniform int map_width;
uniform int map_height;

layout(location = 0) in vec3 position_attr;
layout(location = 1) in vec3 normal_attr;

out vec3 position;
out vec3 barycentric;
out vec3 normal;
out vec4 screen_position;

void main(){
 position = position_attr;
 screen_position = p_mat * mv_mat * vec4(position_attr, 1.0);
 gl_Position = screen_position;
 normal = normal_attr;

 vec3 barycentric_verts_sequence[3] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));
 int row = gl_VertexID / map_width;
 int col = gl_VertexID % map_width;
 int sequence = (col + (2 * row)) % 3;
 barycentric = barycentric_verts_sequence[sequence];
}
