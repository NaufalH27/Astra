#include "sys/socket.h"
#include "stdlib.h"
#include "stdio.h"
#include <netinet/in.h>
#include "string.h"
#include "errno.h"
#include "poll.h"
#include "unistd.h"
#include "pthread.h"


struct response_args {
    int fd;
    int buffer_size;
};

void *response_worker(void *arguments) {
    struct response_args *args = (struct response_args *)arguments;
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
    
    char context_buffer[args->buffer_size]; 
    int bytes_read = recv(args->fd, context_buffer, args->buffer_size - 1, 0);
    context_buffer[bytes_read] = '\0';
    if (bytes_read == -1) {
        perror("failed to read context");
        close(args->fd);
        return NULL;
    }

    ssize_t sent = send(args->fd, context_buffer, bytes_read, 0);
    if (sent == -1) {
        perror("failed to send response");
        close(args->fd);
        return NULL;
    }
    printf("data send succesfully to client\n"); 
    close(args->fd);
    return NULL;
    
}


int main() {

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket creation failed");
        return 1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;  
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    addr.sin_port = htons(8080); 
    int binding = bind(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if (binding == -1){
        perror("socket bind failed");
        return 1;
    }
    
    int listener = listen(sockfd,1);
    if (listener == -1) {
        perror("marking socket as listener failed");
        return 1;
    }
    
    struct pollfd fds[1];
    fds[0].fd = sockfd; 
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    
    while (1) {
        int socket_poll = poll(fds, 1, 5000);
        if (socket_poll == -1) {
            perror("POLL ERROR");
            return 1;
        } else if (socket_poll == 0) {
            continue;
        }

        if ((fds[0].revents & POLLIN) ) {
            int acceptfd = accept(fds[0].fd,  NULL, NULL); 
            if (acceptfd == -1) {
                perror("error accepting package");
                continue;
            }
            struct response_args args;
            args.fd = acceptfd,
            args.buffer_size = 1024;
            
            pthread_t workers;
            if (pthread_create(&workers, NULL, &response_worker, (void *)&args) != 0) {
                perror("failed to invoke workers");
            } else {
                pthread_join(workers, NULL); 
            }
        }
    }
    return 0;
}