#include "workers.h"
#include "task_queue.h"
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

int create_worker_pool_roundrobin(worker_pool_RR *worker_pool) {
    for (int i = 0; i < WORKER_POOL_SIZE; i++) { 
        worker *w = &worker_pool->workers[i];
        pthread_cond_init(&w->cond, NULL);
        pthread_mutex_init(&w->lock, NULL);
        w->status = WIDLE; 
        w->t_queue = create_task_queue();
        if((pthread_create(&w->thread, NULL, &RR_worker_routine , (void *)w)) != 0) {
            return -1;
        }
    } 
    worker_pool->index_ptr = 0;
    return 0;
}

void *RR_worker_routine(void *args) {
    if (args == NULL) {
        pthread_exit((void *)1);
    } 

    worker *w = (worker *) args;
    while (1) {
        pthread_mutex_lock(&w->lock);
        while (w->status == WIDLE) {
            pthread_cond_wait(&w->cond, &w->lock);
        }

        int task_fd = dequeue_task(&w->t_queue);
        if (task_fd == QERROR) {
            printf("Thread %lu : failed to pull task from queue\n", pthread_self());
        } else if (task_fd == QEMPTY) {
            w->status = WIDLE; 
            printf("Thread %lu : task empty\n", pthread_self());
        } else {
            printf("Thread %lu : successfully receive package from %d\n", pthread_self(), task_fd);
            // process the request according to tcp/ip package received
            // TODO : make implementation for processing the request
        }
        pthread_mutex_unlock(&w->lock);
    } 
    return NULL;
}

void distribute_task(worker_pool_RR *worker_pool, int acceptfd) {
    worker *w = &worker_pool->workers[worker_pool->index_ptr];
    pthread_mutex_lock(&w->lock);
    append_task(&w->t_queue, acceptfd);
    w->status = WBUSY;
    pthread_cond_signal(&w->cond); 
    pthread_mutex_unlock(&w->lock);
    worker_pool->index_ptr = (worker_pool->index_ptr + 1) % WORKER_POOL_SIZE;
}

ssize_t recv_all(int sock, void *buffer, size_t length) {
    size_t total_received = 0;
    char *buf = buffer;

    while (total_received < length) {
        ssize_t bytes_received = recv(sock, buf + total_received, length - total_received, 0);
        if (bytes_received < 0) {
            perror("recv failed");
            return -1;
        }
        if (bytes_received == 0) {
            break;
        }
        total_received += bytes_received;
    }
    return total_received;
}

ssize_t send_all(int sock, const void *buffer, size_t length) {
    size_t total_sent = 0;
    const char *buf = buffer;

    while (total_sent < length) {
        ssize_t bytes_sent = send(sock, buf + total_sent, length - total_sent, 0);
        if (bytes_sent < 0) {
            perror("send failed");
            return -1;
        }
        total_sent += bytes_sent;
    }
    return total_sent;
}

