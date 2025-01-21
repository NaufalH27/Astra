#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include "workers.h"





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
            struct worker_args args;
            args.fd = acceptfd,
            args.buffer_size = 5 * 1024 * 1024;
            
            pthread_t workers;
            if (pthread_create(&workers, NULL, &worker, (void *)&args) != 0) {
                perror("failed to invoke workers");
            } else {
                pthread_join(workers, NULL); 
            }
        }
    }
    return 0;
}