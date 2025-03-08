#include "workers.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

#include "task_queue.h"

int create_worker_pool(worker_pool *pool_buffer, int pool_size) {
    if (pool_size <= 0) {
        return -1;
    }

    pool_buffer->workers = (worker *) malloc(sizeof(worker) * pool_size);
    if (pool_buffer->workers == NULL) {
        return -1;
    }

    pool_buffer->pool_size = pool_size;
    for (int i = 0; i < pool_size; i++) { 
        worker *w = &pool_buffer->workers[i];
        w->id = i+1;
        pthread_cond_init(&w->cond, NULL);
        pthread_mutex_init(&w->lock, NULL);
        w->status = WIDLE; 
        w->t_queue = create_task_queue();
        if((pthread_create(&w->thread, NULL, &worker_routine , (void *)w)) != 0) {
            perror("error creating thread");
            return -1;
        }
    } 
    return 0;
}

void *worker_routine(void *args) {
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
            printf("Thread %lu : failed to pull task from queue\n", w->id);
        } else if (task_fd == QEMPTY) {
            w->status = WIDLE; 
        } else {
            char buf[100];
            int s = read(task_fd, &buf, sizeof(buf));
            if (s == 0) {
                printf("Thead %lu : closed %d\n", w->id ,task_fd);
                close(task_fd);
            } else if (s == -1) {
                printf("error happend forcing to close connection %d \n", task_fd);
                close(task_fd);
            } else {
                printf("Thread %lu : successfully receive package from %d\n", w->id, task_fd);
                usleep(100000);
                buf[s] = '\0';
                printf("%s\n", buf);
                char m[] = "Hey Client";
                send(task_fd, m, sizeof(m), 0);
            }
        }
        pthread_mutex_unlock(&w->lock);
    } 
    return NULL;
}

void distribute_task(worker_pool *pool_buffer, int acceptfd) {
    // initialize round robin counter
    static int rr_counter = 0;

    worker *w = &pool_buffer->workers[rr_counter];
    pthread_mutex_lock(&w->lock);
    append_task(&w->t_queue, acceptfd);
    w->status = WBUSY;
    pthread_cond_signal(&w->cond); 
    pthread_mutex_unlock(&w->lock);
    rr_counter = (rr_counter + 1) % pool_buffer->pool_size;
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

