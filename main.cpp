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
#include <fstream>

#define GLM_FORCE_RADIANS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "noise.h"

const int MAP_WIDTH = 512;
const int MAP_HEIGHT = 512;
const float MAP_MAX_ELEVATION = 50.f;
const int MAP_QUADS_X = MAP_WIDTH - 1;
const int MAP_QUADS_Y = MAP_HEIGHT - 1;

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

    if(fullscreen){
      int mode_count;
      const GLFWvidmode* modes = glfwGetVideoModes(glfwGetPrimaryMonitor(), &mode_count);
      width = modes[mode_count - 1].width;
      height = modes[mode_count - 1].height;
    }

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

GLuint shader_from_file(const char* filename, GLenum shader_type){
  std::ifstream shader_file(filename, std::ios::ate | std::ios::binary);

  if(!shader_file.is_open()){
    std::cerr << "Failed to load shader from file '" << filename << "'!" << std::endl;
    return (GLuint)NULL;
  }

  int file_size = shader_file.tellg();
  shader_file.seekg(0);

  char buf[file_size];

  const char* bufs[] = {buf};

  shader_file.read(buf, file_size);
  shader_file.close();

  GLuint shader_id = glCreateShader(shader_type);
  glShaderSource(shader_id, 1, (const char* const*)bufs, &file_size);
  glCompileShader(shader_id);

  GLint shader_compile_status;
  glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compile_status);

  if(shader_compile_status == GL_FALSE){
    GLint log_size;
    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_size);
    char log_buf[log_size];
    glGetShaderInfoLog(shader_id, log_size, NULL, log_buf);

    std::cerr << "Failed to compile shader '" << filename << "': " << std::endl;
    std::cerr << log_buf << std::endl;
    glDeleteShader(shader_id);
    return (GLuint)NULL;
  }

  return shader_id;
}

GLuint create_program(int shader_count, GLuint* shaders){
  GLuint program = glCreateProgram();
  for(int i = 0; i < shader_count; ++i){
    glAttachShader(program, shaders[i]);
  }

  glLinkProgram(program);
  glValidateProgram(program);

  for(int i = 0; i < shader_count; ++i){
    glDetachShader(program, shaders[i]);
  }

  return program;
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

  GLuint vert_shader = shader_from_file("shader.vs.glsl", GL_VERTEX_SHADER);
  GLuint frag_shader = shader_from_file("shader.fs.glsl", GL_FRAGMENT_SHADER);

  GLuint shaders[] = {vert_shader, frag_shader};

  GLuint shader_program = create_program(sizeof(shaders) / sizeof(GLuint), shaders);

  glUseProgram(shader_program);

  glDeleteShader(vert_shader);
  glDeleteShader(frag_shader);

  GLuint vert_array_id;
  glGenVertexArrays(1, &vert_array_id);
  glBindVertexArray(vert_array_id);

  glm::vec3* map_verts = new glm::vec3[MAP_WIDTH * MAP_HEIGHT];

  for(int i = 0; i < MAP_HEIGHT; ++i){
    for(int j = 0; j < MAP_WIDTH; ++j){
      float height = fbm_perlin_noise2d((double)j / 128.d, (double)i / 127.d, 5, .425d, 49841);
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
      granite_comp;
  unsigned char* granite_buf = stbi_load("granite.tga", &granite_tex_w, &granite_tex_h, &granite_comp, STBI_rgb);

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

  int grass_tex_w,
      grass_tex_h,
      grass_comp;
  unsigned char* grass_buf = stbi_load("grass.tga", &grass_tex_w, &grass_tex_h, &grass_comp, STBI_rgb);

  GLuint grass_tex;
  glGenTextures(1, &grass_tex);
  glActiveTexture(1);
  glBindTexture(GL_TEXTURE_2D, grass_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, grass_tex_w, grass_tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, grass_buf);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glUniform1i(glGetUniformLocation(shader_program, "grass"), 1);

  stbi_image_free(grass_buf);

  int snow_tex_w,
      snow_tex_h,
      snow_comp;
  unsigned char* snow_buf = stbi_load("snow.tga", &snow_tex_w, &snow_tex_h, &snow_comp, STBI_rgb);

  std::cout << snow_tex_w << ", " << snow_tex_h << std::endl;

  GLuint snow_tex;
  glGenTextures(1, &snow_tex);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, snow_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, snow_tex_w, snow_tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, snow_buf);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glUniform1i(glGetUniformLocation(shader_program, "snow"), 0);

  stbi_image_free(snow_buf);

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

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, grass_tex);
    glUniform1i(glGetUniformLocation(shader_program, "grass"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, snow_tex);
    glUniform1i(glGetUniformLocation(shader_program, "snow"), 2);

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

  for(int i = 0; i < 32; ++i){
    for(int j = 0; j < 32; ++j){
      double noise = fbm_perlin_noise2d((double)i / 1024.d, (double)j / 1024.d, 10, .6, 424422) / 2.d + .5d;
      noise = fclamp(noise, 0.d, 1.d);
      RGBApixel pixel = {255 * noise, 255 * noise, 255 * noise, 255};
      input.SetPixel(i, j, pixel);
    }
  }

  input.WriteToFile("TestBMP.bmp");

  return 0;
}
