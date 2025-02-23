#ifndef EPOLL_HELPER_H 
#define EPOLL_HELPER_H

#include "task_queue.h"

int create_ipv4_listener(const char *ip, const int port); 
void accept_handler(int listenerfd, int epollfd, task_queue *task_queue);
void reuse_handler(int clientfd);

#endif
