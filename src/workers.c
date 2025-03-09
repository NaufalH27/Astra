#include "workers.h"

#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

#include "uintqueue.h"

int create_worker_pool(worker_pool *wpool, int size) {
    if (size <= 0) {
        return -1;
    }

    wpool->workers = (worker *) malloc(sizeof(worker) * size);
    if (wpool->workers == NULL) {
        return -1;
    }

    wpool->pool_size = size;
    for (int i = 0; i < size; i++) { 
        worker *w = &wpool->workers[i];
        w->id = i+1;
        pthread_cond_init(&w->cond, NULL);
        pthread_mutex_init(&w->lock, NULL);
        w->status = WIDLE; 
        w->cli_sockq = create_uint_queue();
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

        int clisock = uintq_dequeue(&w->cli_sockq);
        if (clisock == QERROR) {
            printf("Thread %lu : failed to pull client socket from queue\n", w->id);
        } else if (clisock == QEMPTY) {
            w->status = WIDLE; 
        } else {
            char buf[100];
            int s = read(clisock, &buf, sizeof(buf));
            if (s == 0) {
                printf("Thead %lu : closed %d\n", w->id ,clisock);
                close(clisock);
            } else if (s == -1) {
                printf("error happend forcing to close socket %d \n", clisock);
                close(clisock);
            } else {
                printf("Thread %lu : successfully receive package from %d\n", w->id, clisock);
                buf[s] = '\0';
                printf("%s\n", buf);
                char m[] = "Hey Client";
                send(clisock, m, sizeof(m), 0);
            }
        }
        pthread_mutex_unlock(&w->lock);
    } 
    return NULL;
}

void distribute_clientsock(worker_pool *wpool, int client_socket) {
    // initialize round robin counter
    static int rr_counter = 0;

    worker *w = &wpool->workers[rr_counter];
    pthread_mutex_lock(&w->lock);
    uintq_append(&w->cli_sockq, client_socket);
    w->status = WBUSY;
    pthread_cond_signal(&w->cond); 
    pthread_mutex_unlock(&w->lock);
    rr_counter = (rr_counter + 1) % wpool->pool_size;
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

