#include <stdio.h>
#include <sys/epoll.h>
#include "epoll_helper.h"
#include "task_queue.h"
// CONFIG
#define IP_ADDRESS "127.0.0.1"
#define PORT 8080

#define MAX_CONN 1000
#define MAX_EVENTS MAX_CONN + 1
#define POLL_TIMEOUT 5000

int main() {
    int listenerfd = create_ipv4_listener(IP_ADDRESS, PORT);
    if (listenerfd == -1) {
        perror("failed to create ipv4 listener");
        return 1;
    }

    int epollfd = epoll_create(MAX_EVENTS);    
    if (epollfd == -1) {
        perror("epoll creation failed");
        return 1;
    } 

    struct epoll_event listener_events = {
        .events = EPOLLIN, 
        .data.fd = listenerfd 
    };
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenerfd, &listener_events);    
    struct epoll_event events[MAX_EVENTS];

    task_queue *t_queue = create_task_queue(); 
    
    while (1) {
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, POLL_TIMEOUT);
        if (nfds == -1) {
            perror("POLL ERROR");
            continue;
        } else if (nfds == 0) {
            continue;
        }

        for (int i = 0; i <= nfds; i++) {
            if (events[i].events & EPOLLIN && events[i].data.fd == listenerfd) {
                accept_handler(events[i].data.fd, epollfd, t_queue);
            } else {
                if (events[i].events & EPOLLIN) {
                    reuse_handler(events[i].data.fd);
                }
            }
        }
    }
    return 0;
}



