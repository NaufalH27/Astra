#include "uintqueue.h"

#include <stdlib.h>

uint_queue create_uint_queue() {
    uint_queue q;
    q.head = NULL;
    q.tail = NULL;
    q.size = 0;
    return q;
}

int uintq_append(uint_queue *queue, size_t data) {
    if (queue == NULL) {
        return QERROR;
    }

    //create task node
    uint_node *node = (uint_node*) malloc(sizeof(uint_node));
    if (node == NULL) {
        return QERROR;
    }
    node->data = data;
    node->next_node = NULL;

    //update tail
    if (queue->tail != NULL) {
        queue->tail->next_node = node;
        queue->tail = node;
    } else {
        queue->head = queue->tail = node;
    }
    queue->size++;
    return 0;
}

int uintq_dequeue(uint_queue *queue) {
    if (queue == NULL) {
        return QERROR;
    }

    //update head
    if (queue->head == NULL) {
        return QEMPTY;
    }

    uint_node *deq_node = queue->head;
    int deq_data = deq_node->data;

    queue->head = queue->head->next_node;
    if (queue->head == NULL) {
        queue->head = queue->tail = NULL;
    }

    queue->size--;
    free(deq_node);
    return deq_data;
}

int uintq_is_empty(uint_queue *queue) {
    int s;
    if (queue->head == NULL) {
         s = 1;
    } else {
        s = 0;
    }
    return s;
}

void uintq_del_queue(uint_queue *queue) {
    while(queue->head != NULL){ 
       uintq_dequeue(queue); 
    }
    free(queue);
}
