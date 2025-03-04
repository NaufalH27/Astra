#include "port.h"

#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

int create_ipv4_listener(const char *ip, const int port) {
    int listenerfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenerfd == -1) {
        perror("creating socket for listener failed");
        return -1;
    }

    int opt = 1;
    if (setsockopt(listenerfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt failed");
        close(listenerfd);
        return -1;
    }
    
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;  
    if (inet_pton(AF_INET, ip, &addr.sin_addr) <= 0) {
        perror("Invalid IP address");
        return -1;
    }
    addr.sin_port = htons(port); 

    int flags = fcntl(listenerfd, F_GETFL, 0);
    if (flags == -1) {
        perror("failed to asign nonblocking flags to socket");
        return -1;
    }
    if (fcntl(listenerfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("failed to asign nonblocking flags to socket");
        return -1;
    }

    int bs = bind(listenerfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_in));
    if (bs == -1){
        perror("binding listener socket failed");
        return -1;
    }

    int ls = listen(listenerfd,1);
    if (ls == -1) {
        perror("marking socket as listener failed");
        return -1;
    }

    printf("Successfully bind listener socket on %s:%d\n",inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    return listenerfd; 
}
