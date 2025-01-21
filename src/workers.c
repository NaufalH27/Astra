#include "workers.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>

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



void *worker(void *arguments) {
    struct worker_args *args = (struct worker_args *)arguments;
    if (args->fd == -1) {
       perror("invalid file descriptor");
       close(args->fd);
       return NULL;
    }    
    if (args->buffer_size < 0) {
        fprintf(stderr, "Invalid buffer size\n"); 
        close(args->fd);
        return NULL;
    }
    
    char package_buffer[args->buffer_size];
    char message[] = "test"; 
   
    close(args->fd);
    return NULL;
    
}