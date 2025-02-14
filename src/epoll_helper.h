#define EPOLL_HELPER_H
#ifdef EPOLL_HELPER_H 

int create_ipv4_listener(const char *ip, const int port); 
void accept_handler(int listenerfd, int epollfd);
void reuse_handler(int clientfd);

#endif
