#ifndef UINT_QUEUE_H
#define UINT_QUEUE_H

#include <stddef.h>
#define QEMPTY (-1)
#define QERROR (-2)

typedef struct uint_node {
    size_t data;
    struct uint_node *next_node;
} uint_node;

typedef struct uint_queue {
    int size;
    uint_node *head;
    uint_node *tail;
} uint_queue;

uint_queue create_uint_queue();
int uintq_append(uint_queue *queue, size_t data); 
int uintq_dequeue(uint_queue *queue); 
int uintq_is_empty(uint_queue *queue);
void uintq_del_queue(uint_queue *queue); 

#endif
