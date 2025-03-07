#ifndef WORKERS_H
#define WORKERS_H

#include "task_queue.h"
#include <pthread.h>

#define WIDLE 0
#define WBUSY 1

typedef struct worker {
    int status;
    pthread_cond_t cond; 
    pthread_mutex_t lock;
    pthread_t thread;
    task_queue t_queue; 
} worker;

typedef struct worker_pool {
    worker *workers; 
    int pool_size;
} worker_pool;

int create_worker_pool(worker_pool *pool_buffer, int pool_size);
void distribute_task(worker_pool *pool_buffer, int acceptfd); 
void *RR_worker_routine(void *args); 
#endif
