#include "workers.h"
#include "task_queue.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

worker_pool_RR create_worker_pool_roundrobin() {
    worker_pool_RR worker_pool;
    for (int i = 0; i < WORKER_POOL_SIZE; i++) {
        worker_pool.workers[i].status = WIDLE;
        worker_pool.workers[i].t_queue = create_task_queue();
    }
    return worker_pool;
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

void distribute_task(worker_pool_RR *worker_pool, int acceptfd) {
    worker curr_worker = worker_pool->workers[worker_pool->index_ptr];
    curr_worker.status = WBUSY;
    append_task(&curr_worker.t_queue ,acceptfd);
    worker_pool->index_ptr++;
}




