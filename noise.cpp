#include "noise.h"

#include <algorithm>
#include <math.h>
#include <functional>
#include <glm/glm.hpp>

int hash_int(int x, int seed){
  return ((x * (x + 15485867 * seed) * 393342739) << ((seed - 14) * x) % 32) * 198491317;
}

double value1d(int x, int seed){
  return (double)hash_int(x, seed) / 2294967295.d;
}

double value2d(int x, int y, int seed){
  return (double)hash_int(hash_int(x, seed) + hash_int(y, seed + 42), seed) / 2294967295.d;
}

glm::vec2 gradient2d(int x, int y, int seed){
  double rotation = fmod((double)(hash_int(x, seed) + hash_int(y, seed)) / 1000.d, 2 * 3.1415d);
  return glm::vec2(cos(rotation), sin(rotation));
}

double interpolate_cos(double x, double y, double coef){
  double f = (cos(coef * 3.1415d) + 1.d) / 2.d;
  return x * f + y * (1 - f);
}

double interpolate_bilinear_cos(double v1, double v2, double v3, double v4, double x_coef, double y_coef){
  double v12 = interpolate_cos(v1, v2, x_coef);
  double v34 = interpolate_cos(v3, v4, x_coef);
  return interpolate_cos(v12, v34, y_coef);
}

double value_noise2d(double x, double y, int seed){
  double v1 = value2d(floor(x), floor(y), seed);
  double v2 = value2d(ceil(x), floor(y), seed);
  double v3 = value2d(floor(x), ceil(y), seed);
  double v4 = value2d(ceil(x), ceil(y), seed);

  return interpolate_bilinear_cos(v1, v2, v3, v4, fmod(x, 1.d), fmod(y, 1.d));
}

double value_noise1d(double x, int seed){
  double prev = value1d(floor(x), seed);
  double next = value1d(ceil(x), seed);
  return interpolate_cos(prev, next, fmod(x, 1.d));
}

double fclamp(double val, double floor, double ceil){
  return std::max(std::min(val, ceil), floor);
}

double fbm_value_noise1d(double x, int n_octaves, double persistence, int seed){
  double val = 0.d;

  for(int i = 0; i < n_octaves; ++i){
    double frequency = pow(2, i);
    double amplitude = pow(persistence, i);

    val += value_noise1d(x * frequency, seed) * amplitude;
  }

  return val;
}

double fbm_value_noise2d(double x, double y, int n_octaves, double persistence, int seed){
  double val = 0.d;

  for(int i = 0; i < n_octaves; ++i){
    double frequency = pow(2, i);
    double amplitude = pow(persistence, i);

    val += value_noise2d(x * frequency, y * frequency, seed) * amplitude;
  }

  return val;
}

double billowed_value_noise1d(double x, int n_octaves, double persistence, int seed){
  return fabs(fbm_value_noise1d(x, n_octaves, persistence, seed)) * 2.d - 1.d;
}
double billowed_value_noise2d(double x, double y, int n_octaves, double persistence, int seed){
  return fabs(fbm_value_noise2d(x, y, n_octaves, persistence, seed)) * 2.d - 1.d;
}

double perlin_noise2d(double x, double y, int seed){
  glm::vec2 gradient_s11 = gradient2d(floor(x), floor(y), seed);
  glm::vec2 gradient_s21 = gradient2d(ceil(x), floor(y), seed);
  glm::vec2 gradient_s12 = gradient2d(floor(x), ceil(y), seed);
  glm::vec2 gradient_s22 = gradient2d(ceil(x), ceil(y), seed);

  glm::vec2 i(fmod(x, 1.0), fmod(y, 1.0));

  glm::vec2 delta_s11 = glm::vec2(0.0, 0.0) - i;
  glm::vec2 delta_s21 = glm::vec2(1.0, 0.0) - i;
  glm::vec2 delta_s12 = glm::vec2(0.0, 1.0) - i;
  glm::vec2 delta_s22 = glm::vec2(1.0, 1.0) - i;

  double value_s11 = glm::dot(gradient_s11, delta_s11);
  double value_s21 = glm::dot(gradient_s21, delta_s21);
  double value_s12 = glm::dot(gradient_s12, delta_s12);
  double value_s22 = glm::dot(gradient_s22, delta_s22);

  return interpolate_bilinear_cos(value_s11, value_s21, value_s12, value_s22, i[0], i[1]);
}

double fbm_perlin_noise2d(double x, double y, int n_octaves, double persistence, int seed){
  double val = 0.d;

  for(int i = 0; i < n_octaves; ++i){
    double frequency = pow(2, i);
    double amplitude = pow(persistence, i);

    val += perlin_noise2d(x * frequency, y * frequency, seed) * amplitude;
  }

  return val;
}
