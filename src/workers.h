#ifndef WORKERS_H
#define WORKERS_H

#include "task_queue.h"
#include <bits/pthreadtypes.h>

#define WIDLE 0
#define WBUSY 1
#define WORKER_POOL_SIZE 1000 

typedef struct worker {
    int status;
    pthread_t thread;
    task_queue t_queue; 
} worker;

// round robin implementation for worker pool
typedef struct worker_pool_RR {
    int index_ptr; 
    worker workers[WORKER_POOL_SIZE]; 
} worker_pool_RR;

worker_pool_RR create_worker_pool_roundrobin(); 
void distribute_task(worker_pool_RR *worker_pool, int acceptfd);
#endif
