#ifndef EPOLL_HELPER_H 
#define EPOLL_HELPER_H

int create_ipv4_listener(const char *ip, const int port); 
void accept_handler(int listenerfd, int epollfd);

#endif
