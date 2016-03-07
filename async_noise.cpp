#include "async_noise.h"

#include <math.h>
#include <thread>
#include <iostream>

#include "noise.h"

static void async_noise_thread(float* buf, int buf_w, int buf_h, int start_x, int start_y, int w, int h, int n_octaves, double persistence, int seed){
  std::cout << "thread started, doin some work!" << std::endl;
  for(int i = start_y; i < start_y + h; ++i){
    for(int j = start_x; j < start_x + w; ++j){
      buf[j + i * buf_w] = fbm_perlin_noise2d((float)j / 128.d, (float)i / 128.d, n_octaves, persistence, seed);
    }
  }
  std::cout << "thread done" << std::endl;
}

void async_noise(float* buf, int w, int h, int worker_count, int n_octaves, double persistence, int seed){
  int workers_x = sqrt(worker_count);
  int workers_y = sqrt(worker_count);

  int worker_domain_w = w / workers_x;
  int worker_domain_h = h / workers_y;

  std::thread* workers[worker_count];

  for(int i = 0; i < workers_y; ++i){
    for(int j = 0; j < workers_x; ++j){
      std::cout << "creating threads" << std::endl;
      workers[j + i * workers_x] = new std::thread(
        async_noise_thread,
        buf, w, h,
        j * worker_domain_w, i * worker_domain_h,
        worker_domain_w, worker_domain_h,
        n_octaves, persistence, seed
      );
    }
  }

  for(int i = 0; i < worker_count; ++i){
    std::cout << "Joining thread..." << std::endl;
    workers[i]->join();
    delete workers[i];
  }
}
