#ifndef ASYNC_NOISE_H
#define ASYNC_NOISE_H

void async_noise(float* buf, int w, int h, int worker_count, int n_octaves, double persistence, int seed);

#endif //ASYNC_NOISE_H
