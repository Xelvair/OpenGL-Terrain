#include "async_noise.h"

#include <math.h>
#include <thread>
#include <iostream>
#include <queue>
#include <mutex>

#include "noise.h"

struct work_item{
  int x, y, w, h;
};

static void async_noise_worker(float* buf, int domain_w, int domain_h, int n_octaves, double persistence, int seed, std::queue<work_item>* work_item_queue, std::mutex* work_item_queue_mutex){
  while(true){
    work_item_queue_mutex->lock();

    if(work_item_queue->empty()){
      work_item_queue_mutex->unlock();
      std::cout << "Queue cleared, worker exiting." << std::endl;
      return;
    }

    work_item wi = work_item_queue->front();
    work_item_queue->pop();

    std::cout << "Starting work item." << std::endl;

    work_item_queue_mutex->unlock();

    for(int i = wi.y; i < wi.y + wi.h; ++i){
      for(int j = wi.x; j < wi.x + wi.w; ++j){
        buf[j + i * domain_w] = fbm_perlin_noise2d((double)j / 128.d, (double)i / 128.d, n_octaves, persistence, seed);
      }
    }

    std::cout << "Finished work item." << std::endl;
  }
}

void async_noise(float* buf, int domain_w, int domain_h, int work_item_w, int work_item_h, int worker_count, int n_octaves, double persistence, int seed){
  std::queue<work_item> work_item_queue;
  std::mutex work_item_queue_mutex;

  int work_items_x = (domain_w + work_item_w - 1) / work_item_w;
  int work_items_y = (domain_h + work_item_h - 1) / work_item_h;

  for(int i = 0; i < work_items_y; ++i){
    for(int j = 0; j < work_items_x; ++j){
      work_item wi;
      wi.x = work_item_w * j;
      wi.y = work_item_h * i;

      if(j == work_items_x - 1){
        wi.w = domain_w - j * work_item_w;
      } else {
        wi.w = work_item_w;
      }

      if(i == work_items_y - 1){
        wi.h = domain_h - i * work_item_h;
      } else {
        wi.h = work_item_h;
      }

      work_item_queue.push(wi);
    }
  }

  std::cout << work_item_queue.size() << " work items generated." << std::endl;

  std::thread* workers[worker_count];

  for(int i = 0; i < worker_count; ++i){
    workers[i] = new std::thread(
      async_noise_worker,
      buf, domain_w, domain_h, n_octaves, persistence, seed,
      &work_item_queue, &work_item_queue_mutex
    );
  }

  for(int i = 0; i < worker_count; ++i){
    workers[i]->join();
    delete workers[i];
  }
}
