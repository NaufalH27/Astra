#ifndef EPOLL_HELPER_H 
#define EPOLL_HELPER_H

#include "workers.h"

int create_ipv4_listener(const char *ip, const int port); 
void accept_handler(int listenerfd, int epollfd, worker_pool_RR *worker_pool);
void reuse_handler(int clientfd);

#endif
