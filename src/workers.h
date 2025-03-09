#ifndef WORKERS_H
#define WORKERS_H

#include "uintqueue.h"
#include <pthread.h>

#define WIDLE 0
#define WBUSY 1

typedef struct worker {
    size_t id;
    int status;
    pthread_cond_t cond; 
    pthread_mutex_t lock;
    pthread_t thread;
    uint_queue cli_sockq; 
} worker;

typedef struct worker_pool {
    worker *workers; 
    int pool_size;
} worker_pool;

int create_worker_pool(worker_pool *wpool, int wpool_size);
void distribute_clientsock(worker_pool *wpool, int client_socket); 
void *worker_routine(void *args); 
#endif
