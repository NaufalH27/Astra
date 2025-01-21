#ifndef WORKERS_H
#define WORKERS_H

struct worker_args {
    int fd;
    int buffer_size;
};

void *worker(void *arguments);

#endif