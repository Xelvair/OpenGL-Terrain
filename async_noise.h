#ifndef ASYNC_NOISE_H
#define ASYNC_NOISE_H

void async_noise(float* buf, int domain_w, int domain_h, int work_item_w, int work_item_h, int worker_count, int n_octaves, double persistence, int seed);

#endif //ASYNC_NOISE_H
