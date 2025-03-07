#ifndef INIT_H
#define INIT_H

#include "cfg.h"
#include "workers.h"

typedef struct server_cfg {
    char *ip_adress;
    int port;
    int max_conn;
    int poll_timeout;
    int worker_pool_size;
} server_cfg;

typedef struct server_context {
    int listenerfd;
    int epollfd;
    struct epoll_event *events;
    int event_size;
    worker_pool worker_pool;
} server_context;

server_cfg init_cfg();
int init_context(server_context *const ctx_buf, const server_cfg cfg);
void server_start(server_context *ctx);

#endif
