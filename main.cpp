#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <iostream>
#include <math.h>
#include <functional>
#include <algorithm>
#include <EasyBMP/EasyBMP.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <exception>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "noise.h"

const int MAP_WIDTH = 512;
const int MAP_HEIGHT = 512;
const float MAP_MAX_ELEVATION = 50.f;
const int MAP_QUADS_X = MAP_WIDTH - 1;
const int MAP_QUADS_Y = MAP_HEIGHT - 1;

const char* vert_shader_src =
"#version 330 core\n"
""
"uniform mat4 mv_mat;"
"uniform mat4 p_mat;"
"uniform int map_width;"
"uniform int map_height;"
""
"layout(location = 0) in vec3 position_attr;\n"
"layout(location = 1) in vec3 normal_attr;"
""
"out vec3 position;"
"out vec3 barycentric;"
"out vec3 normal;"
"out vec4 screen_position;"
""
"void main(){\n"
" position = position_attr;"
" screen_position = p_mat * mv_mat * vec4(position_attr, 1.0);"
" gl_Position = screen_position;"
" normal = normal_attr;"
""
" vec3 barycentric_verts_sequence[3] = vec3[](vec3(1.0, 0.0, 0.0), vec3(0.0, 1.0, 0.0), vec3(0.0, 0.0, 1.0));"
" int row = gl_VertexID / map_width;"
" int col = gl_VertexID % map_width;"
" int sequence = (col + (2 * row)) % 3;\n"
" barycentric = barycentric_verts_sequence[sequence];\n"
"}\n"
"";

const char* frag_shader_src =
"#version 330 core\n"
""
"in vec3 position;"
"in vec3 barycentric;"
"in vec3 normal;"
"in vec4 screen_position;"
"out vec3 color;"
""
"uniform sampler2D granite;"
""
"void main(){"
"float dist = (screen_position.x * screen_position.x + screen_position.z * screen_position.z + screen_position.y * screen_position.y) / 750.f;"
" float bg_component = clamp(dist / 350.f, 0.f, 1.f);"
" if(any(lessThan(barycentric, vec3(0.0075, 0.0075, 0.0075)))){"
"   color = vec3(0.0, 0.0, 0.0);"
" } else { "
"   color = texture2D(granite, mod(position.xz / 64.0, 1.0)).xyz;"
" }"
" float phong_coef = dot(normal * -1.0, normalize(vec3(-1.0, -5.0, -2.0)));"
" float light_coef = 0.25 + phong_coef * 0.75;"
" color = color * light_coef;"
" color = color * (1 - bg_component) + bg_component * vec3(.5f, .7f, 1.f);\n"
"}";

//Model-View-Matrix
glm::mat4 mv_mat;
glm::mat4 p_mat;

double rot_x = 0.d;
double rot_y = 0.d;
glm::vec3 cam_pos;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GL_TRUE);
  }
}

void window_resize_callback(GLFWwindow* window, int x, int y){
  glViewport(0, 0, x, y);
}

GLFWwindow* setup_window(int width, int height, int fullscreen, GLFWwindow* share){
    // INITIALIZE AND CREATE GLFW WINDOW ///////////////////
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, "PlayinAround", fullscreen ? glfwGetPrimaryMonitor() : NULL, share);

    if(!window){
        std::cerr << "Failed to create window." << std::endl;
        return NULL;
    }

    if(share){
        glfwDestroyWindow(share);
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, window_resize_callback);
    glfwSetKeyCallback(window, key_callback);

    // INITIALIZE GLEW ///////////////////////////////////
    glewExperimental = GL_TRUE;
    GLenum glew_init_status = glewInit();
    glGetError(); //Discard error message

    if(glew_init_status != GLEW_OK){
        std::cout << "Failed to initialize GLEW." << glew_init_status << std::endl;
        return NULL;
    }

    // INITIALIZE OPENGL /////////////////////////////////
    glClearColor(.5f, .7f, 1.f, 1.f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    GLuint vert_array_id;
    glGenVertexArrays(1, &vert_array_id);
    glBindVertexArray(vert_array_id);

    return window;
}

void update_mouse(GLFWwindow* window){
  const double MOUSE_DEFAULT_X = 400.d;
  const double MOUSE_DEFAULT_Y = 300.d;

  double x, y;
  glfwGetCursorPos(window, &x, &y);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  double delta_x = x - MOUSE_DEFAULT_X;
  double delta_y = y - MOUSE_DEFAULT_Y;

  rot_x -= -delta_y * 0.00075d;
  rot_y += delta_x * 0.00075d;
  rot_x = fmod(rot_x, 3.1415d);
  rot_y = fmod(rot_y, 3.1415d * 2);
  glfwSetCursorPos(window, 400, 300);
}

void glew_error_callback(int type, const char* message){
  std::cout << "GLEW Error: " << type << " : " << message << std::endl;
}

int main(int argc, char* argv[]){
  int window_width = 800;
  int window_height = 600;
  int fullscreen = false;

  if(!glfwInit()){
    std::cout << "Failed to initialize GLFW." << std::endl;
    return EXIT_FAILURE;
  }

  GLFWwindow* window = setup_window(window_width, window_height, fullscreen, NULL);

  glfwSetErrorCallback(&glew_error_callback);

  std::cout << glGetString(GL_VERSION) << std::endl;

  int vert_shader_length = strlen(vert_shader_src);
  GLuint vert_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert_shader, 1, &vert_shader_src, NULL);
  glCompileShader(vert_shader);
  int vert_shader_compile_status;
  glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &vert_shader_compile_status);
  if(vert_shader_compile_status == GL_FALSE){
    char buf[512];
    glGetShaderInfoLog(vert_shader, 512, NULL, buf);
    std::cout << buf << std::endl;
  }

  int frag_shader_length = strlen(frag_shader_src);
  GLuint frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag_shader, 1, &frag_shader_src, NULL);
  glCompileShader(frag_shader);
  int frag_shader_compile_status;
  glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &frag_shader_compile_status);
  if(frag_shader_compile_status == GL_FALSE){
    char buf[512];
    glGetShaderInfoLog(frag_shader, 512, NULL, buf);
    std::cout << buf << std::endl;
  }

  GLuint shader_program = glCreateProgram();
  glAttachShader(shader_program, vert_shader);
  glAttachShader(shader_program, frag_shader);
  glLinkProgram(shader_program);
  glValidateProgram(shader_program);
  glUseProgram(shader_program);
  glDetachShader(shader_program, vert_shader);
  glDetachShader(shader_program, frag_shader);
  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  GLuint vert_array_id;
  glGenVertexArrays(1, &vert_array_id);
  glBindVertexArray(vert_array_id);

  glm::vec3* map_verts = new glm::vec3[MAP_WIDTH * MAP_HEIGHT];

  for(int i = 0; i < MAP_HEIGHT; ++i){
    for(int j = 0; j < MAP_WIDTH; ++j){
      float height = fbm_perlin_noise2d((double)j / 128.d, (double)i / 128.d, 7, .4d, 7564);
      map_verts[i * MAP_WIDTH + j] = glm::vec3((float)j, (height / 2.f + .5f) * MAP_MAX_ELEVATION * 4.f, (float)i);
    }
  }

  GLuint map_buffer_id;
  glGenBuffers(1, &map_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, map_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * MAP_WIDTH * MAP_HEIGHT, map_verts, GL_STATIC_DRAW);

  glm::vec3* map_normals = new glm::vec3[MAP_WIDTH * MAP_HEIGHT];
  for(int i = 0; i < MAP_HEIGHT; ++i){
    for(int j = 0; j < MAP_WIDTH; ++j){
      auto in_bounds = [](int x, int y)->bool{return x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT;};

      double height_orig = map_verts[i * MAP_WIDTH + j][1];
      double height_left = (in_bounds(j - 1, i) ? map_verts[i * MAP_WIDTH + (j - 1)][1] : height_orig);
      double height_top = in_bounds(j, i - 1) ? map_verts[(i - 1) * MAP_WIDTH + j][1] : height_orig;
      double height_right = in_bounds(j + 1, i) ? map_verts[i * MAP_WIDTH + (j + 1)][1] : height_orig;
      double height_bottom = in_bounds(j, i + 1) ? map_verts[(i + 1) * MAP_WIDTH + j][1] : height_orig;

      double diff_x = (height_left - height_orig) + (height_orig - height_right);
      double diff_y = (height_top - height_orig) + (height_orig - height_bottom);

      glm::vec3 normal = glm::normalize(glm::vec3(diff_x, 2.d, diff_y));

      map_normals[i * MAP_WIDTH + j] = normal;
    }
  }

  GLuint normals_buffer_id;
  glGenBuffers(1, &normals_buffer_id);
  glBindBuffer(GL_ARRAY_BUFFER, normals_buffer_id);
  glBufferData(GL_ARRAY_BUFFER, sizeof(glm::ivec3) * MAP_WIDTH * MAP_HEIGHT, map_normals, GL_STATIC_DRAW);

  delete [] map_verts;
  map_verts = nullptr;
  delete [] map_normals;
  map_normals = nullptr;

  unsigned int* map_quad_vert_indexes = new unsigned int[MAP_QUADS_X * MAP_QUADS_Y * 6];
  for(int i = 0; i < MAP_QUADS_Y; ++i){
    for(int j = 0; j < MAP_QUADS_X; ++j){
      int array_index = 6 * (i * MAP_QUADS_X + j);
      map_quad_vert_indexes[array_index]      = i * MAP_WIDTH + j;
      map_quad_vert_indexes[array_index + 1]  = (i + 1) * MAP_WIDTH + j;
      map_quad_vert_indexes[array_index + 2]  = i * MAP_WIDTH + j + 1;

      map_quad_vert_indexes[array_index + 3]  = (i + 1) * MAP_WIDTH + j;
      map_quad_vert_indexes[array_index + 4]  = (i + 1) * MAP_WIDTH + j + 1;
      map_quad_vert_indexes[array_index + 5]  = i * MAP_WIDTH + j + 1;
    }
  }

  GLuint map_quad_vert_index_buffer;
  glGenBuffers(1, &map_quad_vert_index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, map_quad_vert_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, MAP_QUADS_X * MAP_QUADS_Y * 6 * sizeof(int), map_quad_vert_indexes, GL_STATIC_DRAW);
  delete [] map_quad_vert_indexes;
  map_quad_vert_indexes = nullptr;

  int buf_size;
  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &buf_size);
  std::cout << "buf size: " << buf_size << std::endl;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  mv_mat = glm::mat4();
  //p_mat = glm::ortho(-100.f, 100.f, -100.f, 100.f, 10.f, 1000.f);
  p_mat = glm::perspective(1.5f, 800.f / 600.f, 0.1f, 1000.f);
  glClearColor(.5f, .7f, 1.f, 1.f);

  GLint map_width_uniform_location = glGetUniformLocation(shader_program, "map_width");
  glUniform1i(map_width_uniform_location, MAP_WIDTH);

  GLint map_height_uniform_location = glGetUniformLocation(shader_program, "map_height");
  glUniform1i(map_height_uniform_location, MAP_HEIGHT);

  int granite_tex_w,
      granite_tex_h,
      comp;
  unsigned char* granite_buf = stbi_load("granite.tga", &granite_tex_w, &granite_tex_h, &comp, STBI_rgb);

  std::cout << granite_tex_w << ", " << granite_tex_h << std::endl;

  GLuint granite_tex;
  glGenTextures(1, &granite_tex);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, granite_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, granite_tex_w, granite_tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, granite_buf);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glUniform1i(glGetUniformLocation(shader_program, "granite"), 0);

  stbi_image_free(granite_buf);

  std::cout << window << std::endl;

  while(!glfwWindowShouldClose(window)){
    if(glfwGetWindowAttrib(window, GLFW_FOCUSED) == GL_TRUE){
      update_mouse(window);
    }

    if(glfwGetKey(window, GLFW_KEY_W)){
      cam_pos += glm::vec3(sin(rot_y) * cos(rot_x), -sin(rot_x), -cos(rot_y) * cos(rot_x)) * 1.25f;
    }

    if(glfwGetKey(window, GLFW_KEY_S)){
      cam_pos += glm::vec3(-sin(rot_y) * cos(rot_x), sin(rot_x), cos(rot_y) * cos(rot_x)) * 1.25f;
    }

    if(glfwGetKey(window, GLFW_KEY_F11)){
      fullscreen = !fullscreen;
      window = setup_window(window_width, window_height, fullscreen, window);
    }

    std::cout << "rx: " << rot_x << " ry: " << rot_y << std::endl;
    std::cout << "x: " << cam_pos[0] << " y: " << cam_pos[1] << " z: " << cam_pos[2] << std::endl;

    mv_mat = glm::mat4();
    mv_mat = glm::rotate(mv_mat, (float)rot_x, glm::vec3(1.f, 0.f, 0.f));
    mv_mat = glm::rotate(mv_mat, (float)rot_y, glm::vec3(0.f, 1.f, 0.f));
    mv_mat = glm::translate(mv_mat, -cam_pos);
    mv_mat = glm::translate(mv_mat, glm::vec3(0.f, -75.d, -128.f));

    glUseProgram(shader_program);

    GLint mv_mat_uniform = glGetUniformLocation(shader_program, "mv_mat");
    glUniformMatrix4fv(mv_mat_uniform, 1, GL_FALSE, (float*)&mv_mat);

    GLint p_mat_uniform = glGetUniformLocation(shader_program, "p_mat");
    glUniformMatrix4fv(p_mat_uniform, 1, GL_FALSE, (float*)&p_mat);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, granite_tex);
    glUniform1i(glGetUniformLocation(shader_program, "granite"), 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, map_quad_vert_index_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, map_buffer_id);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, normals_buffer_id);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glDrawElements(GL_TRIANGLES, MAP_QUADS_X * MAP_QUADS_Y * 6, GL_UNSIGNED_INT, NULL);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    glfwSwapBuffers(window);
    glfwPollEvents();

    GLuint gl_error = glGetError();
    while(gl_error != GL_NO_ERROR){
      std::cout << "GL Error: " << gl_error << std::endl;
      gl_error = glGetError();
    }
  }

  glfwTerminate();

  BMP input;
  input.SetSize(2048, 2048);
  input.SetBitDepth(32);

  for(int i = 0; i < 2048; ++i){
    for(int j = 0; j < 2048; ++j){
      double noise = fbm_perlin_noise2d((double)i / 1024.d, (double)j / 1024.d, 10, .6, 424422) / 2.d + .5d;
      noise = fclamp(noise, 0.d, 1.d);
      RGBApixel pixel = {255 * noise, 255 * noise, 255 * noise, 255};
      input.SetPixel(i, j, pixel);
    }
  }

  input.WriteToFile("TestBMP.bmp");

  return 0;
}
