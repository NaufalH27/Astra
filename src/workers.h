#ifndef WORKERS_H
#define WORKERS_H

#include "task_queue.h"
#include <pthread.h>

#define WIDLE 0
#define WBUSY 1
#define WORKER_POOL_SIZE 1000 

typedef struct worker {
    int status;
    pthread_cond_t cond; 
    pthread_mutex_t lock;
    pthread_t thread;
    task_queue t_queue; 
} worker;

// round robin implementation for worker pool
typedef struct worker_pool_RR {
    int index_ptr; 
    worker workers[WORKER_POOL_SIZE]; 
} worker_pool_RR;

int create_worker_pool_roundrobin(worker_pool_RR *worker_pool); 
void distribute_task(worker_pool_RR *worker_pool, int acceptfd);
void *RR_worker_routine(void *args); 
#endif
