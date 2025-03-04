#include "init.h"

#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <errno.h>

#include "cfg.h"
#include "workers.h"
#include "port.h"

// for runtime configuration change in the future
server_cfg init_cfg() {
    return (server_cfg) {
        .ip_adress = IP_ADDRESS,
        .port = PORT,
        .max_conn = MAX_CONN,
        .poll_timeout = POLL_TIMEOUT
    };
}

int init_context(server_context *const ctx_buf, const server_cfg cfg) {
    int epoll_size = cfg.max_conn + 1;

    ctx_buf->listenerfd = create_ipv4_listener(cfg.ip_adress, cfg.port);
    if (ctx_buf->listenerfd == -1) {
        perror("failed to create ipv4 listener");
        return -1;
    }

    ctx_buf->epollfd = epoll_create(epoll_size);    
    if (ctx_buf->epollfd == -1) {
        perror("epoll creation failed");
        return -1;
    } 

    struct epoll_event listener_events = {
        .events = EPOLLIN, 
        .data.fd = ctx_buf->listenerfd 
    };
    epoll_ctl(ctx_buf->epollfd, EPOLL_CTL_ADD, ctx_buf->listenerfd, &listener_events);

    int wp_s = create_worker_pool_roundrobin(&ctx_buf->worker_pool, cfg.worker_pool_size); 
    if (wp_s == -1) {
        printf("failed to create worker pool");
        return -1;
    }

    ctx_buf->events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * epoll_size);
    
    return 0;
}

void accept_conn(int listenerfd, int epollfd) {
    while (1) {
        int clientfd = accept(listenerfd, NULL, NULL);
        if (clientfd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            } else {
                perror("accept error");
                break;
            }
        }

        struct epoll_event client_events = {
            .events = EPOLLIN | EPOLLET, 
            .data.fd = clientfd
        };
        epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &client_events);
    }
}

void server_start(server_context *ctx) {
    while (1) {
        int nfds = epoll_wait(ctx->epollfd, ctx->events, sizeof(&ctx->events), POLL_TIMEOUT);
        if (nfds == -1) {
            perror("POLL ERROR");
            continue;
        } else if (nfds == 0) {
            continue;
        }

        for (int i = 0; i <= nfds; i++) {
            const struct epoll_event e = ctx->events[i];
            if (e.events & EPOLLIN && e.data.fd == ctx->listenerfd) {
                accept_conn(e.data.fd, ctx->epollfd);
            } else {
                if (e.events & EPOLLIN) {
                    distribute_task(&ctx->worker_pool, e.data.fd, sizeof(ctx->worker_pool));
                }
            }
        }
    }
}
