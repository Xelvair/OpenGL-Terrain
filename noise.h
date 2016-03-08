#ifndef NOISE_H
#define NOISE_H

int hash_int(int x, int seed);

double noise1d(int x, int seed);
double noise2d(int x, int y, int seed);

double interpolate_cos(double x, double y, double coef);
double interpolate_bilinear_cos(double v1, double v2, double v3, double v4, double x_coef, double y_coef);

double value_noise2d(double x, double y, int seed);
double value_noise1d(double x, int seed);

double fclamp(double val, double floor, double ceil);

double fbm_value_noise1d(double x, int n_octaves, double persistence, int seed);
double fbm_value_noise2d(double x, double y, int n_octaves, double persistence, int seed);

double billowed_value_noise1d(double x, int n_octaves, double persistence, int seed);
double billowed_value_noise2d(double x, double y, int n_octaves, double persistence, int seed);

double perlin_noise2d(double x, double y, int seed);

double fbm_perlin_noise2d(double x, double y, int n_octaves, double persistence, int seed);

#endif // NOISE_H
